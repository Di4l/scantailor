# 🔴 Qt en ScanTailor - Análisis Completo

Análisis detallado del uso de Qt 5.15 en el proyecto ScanTailor y su impacto en la arquitectura.

---

## 📊 Resumen ejecutivo

| Métrica | Valor |
|---------|-------|
| **Archivos con Qt** | ~150 archivos (~26% del codebase) |
| **Clases Q_OBJECT** | 52+ clases con Qt meta-object |
| **Líneas de código Qt** | ~8,000-10,000 LoC |
| **Dependencia** | Crítica (solo interfaz es Qt) |
| **Costo de migración** | 2-3 semanas (1 desarrollador) |
| **Viabilidad wxWidgets** | ✅ 90%+ |

---

## 🎯 ¿Por qué Qt en el proyecto?

ScanTailor utiliza Qt **únicamente** para:

1. **Interfaz gráfica (GUI)**
   - Ventana principal
   - Diálogos de configuración
   - Widgets customizados
   - Renderizado de imágenes

2. **Manejo de eventos**
   - Clicks de mouse
   - Arrastres
   - Teclas
   - Custom signals/slots

3. **Utilidades de interfaz**
   - Threading (QThread)
   - Timers
   - File dialogs
   - Settings/Preferences

**La lógica de procesamiento de imágenes es 100% independiente de Qt.**

---

## 📂 Dónde está Qt en el código

### Carpeta principal: `src/ui/`

```
src/ui/
├── MainWindow.h / .cpp             # Ventana principal
├── ImageView.h / .cpp              # Widget visualizador imágenes
├── dialogs/
│   ├── SettingsDialog.h / .cpp
│   ├── ProjectDialog.h / .cpp
│   └── ... (otros diálogos)
├── widgets/
│   ├── StatusBar.h / .cpp
│   ├── Toolbar.h / .cpp
│   └── ... (widgets customizados)
└── CMakeLists.txt
```

### Archivos fuera de ui/ que usan Qt

| Archivo | Razón |
|---------|-------|
| `main.cpp` | Punto de entrada, crea QApplication |
| `crash_reporter/` | Diálogos de error con Qt |
| Algunos headers en `filters/` | Pasar QImage entre componentes |

---

## 🔌 Clases Qt principales usadas

### Clases con Q_OBJECT (52+)

```cpp
// Herencia de Qt
class MainWindow : public QMainWindow { Q_OBJECT ... };
class ImageView : public QWidget { Q_OBJECT ... };
class SettingsDialog : public QDialog { Q_OBJECT ... };
class MyThread : public QThread { Q_OBJECT ... };
```

### Widgets principales

| Clase | Propósito |
|-------|-----------|
| `QMainWindow` | Ventana principal con menús/toolbars |
| `QWidget` | Widgets base customizados |
| `QDialog` | Diálogos modales |
| `QImage` / `QPixmap` | Almacenamiento de imágenes |
| `QLabel` | Etiquetas de texto |
| `QSlider` | Controles deslizantes |
| `QSpinBox` | Entrada numérica |
| `QPushButton` | Botones |
| `QMenu` / `QAction` | Menús |

### Signals y Slots

```cpp
// Definición
class MyClass : public QObject {
    Q_OBJECT
signals:
    void imageLoaded(const QImage& img);
    
public slots:
    void onImageLoaded(const QImage& img);
    
private:
    void processImage();
};

// Conexión
connect(imageView, SIGNAL(selectionChanged(QRect)),
        this, SLOT(onSelectionChanged(QRect)));
```

### Threading

```cpp
// Procesamiento en thread
class ImageProcessor : public QThread {
    Q_OBJECT
    
    void run() override {
        // Procesa en background
        QImage result = processImage(input);
        emit processingFinished(result);
    }
    
signals:
    void processingFinished(const QImage& result);
};
```

---

## 🔄 Patrones Qt en el código

### 1. Model-View-Controller

```
    Model (ImageData)
         ↓
    View (ImageView - QWidget)
         ↓
    Controller (MainWindow)
```

### 2. Signals/Slots

```
User Click
  ↓ (evento de Qt)
ImageView::mousePressEvent()
  ↓
Emite signal: selectionChanged(rect)
  ↓
MainWindow conectado al signal
  ↓
Slot: MainWindow::onSelectionChanged(rect)
  ↓
Procesa cambio
```

### 3. Custom Painting

```cpp
void ImageView::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    painter.drawImage(0, 0, currentImage);
    painter.drawRect(selectionRect);
}
```

---

## 🧪 Acoplamiento Qt-Lógica

### ✅ BUE acoplamiento (poco)

**Arquivos procesamiento NO dependen de Qt**:
- `src/filters/*.cpp` - ✅ Cero Qt
- `src/imageproc/*.cpp` - ✅ Cero Qt
- `src/math/*.cpp` - ✅ Cero Qt
- `src/dewarping/*.cpp` - ✅ Cero Qt
- `src/zones/*.cpp` - ✅ Cero Qt

### ❌ Acoplamiento fuerte (existe)

**Aquí sí hay Qt**:
- `src/ui/*.cpp` - ✅ Todo Qt
- `src/interaction/` - ⚠️ Algunos Qt (eventos)
- Headers que usan QImage - ⚠️ Acoplamiento mínimo

### Cambios necesarios para desacoplamiento

```cpp
// ACTUAL (acoplado)
void loadImage(const QString& path);  // Usa Qt

// IDEAL (desacoplado)
void loadImage(const std::string& path);  // Usa std
void setImage(const QImage& img);  // Conversión en UI
```

---

## 📊 Análisis por componente

### MainWindow

**Responsabilidad**: Ventana principal y coordinación

**Líneas**: ~1,500-2,000 LoC

**Funciones principales**:
```cpp
void MainWindow::openProject();
void MainWindow::loadImage(const QString& path);
void MainWindow::applyFilter(FilterType type);
void MainWindow::onImageProcessed(const QImage& result);
```

**Dependencias Qt**: QMainWindow, QAction, QMenu, QThread, signals/slots

### ImageView

**Responsabilidad**: Visualizar imagen y capturar interacción

**Líneas**: ~800-1,000 LoC

**Funciones principales**:
```cpp
void ImageView::paintEvent(QPaintEvent* event);
void ImageView::mousePressEvent(QMouseEvent* event);
void ImageView::mouseMoveEvent(QMouseEvent* event);
void ImageView::resizeEvent(QResizeEvent* event);
```

**Dependencias Qt**: QWidget, QPainter, QPaintEvent, QMouseEvent

### Diálogos

**Responsabilidad**: Capturar entrada del usuario

**Líneas**: ~200-500 cada uno

**Ejemplos**:
- `SettingsDialog` - Configuración de filtros
- `ProjectDialog` - Crear nuevo proyecto
- `ExportDialog` - Opciones de exportación

**Dependencias Qt**: QDialog, QLineEdit, QSpinBox, QPushButton

---

## 🔄 Flujo de datos Qt

### Carga de imagen

```
User: File → Open
    ↓
MainWindow::openFile()
    ↓
QFileDialog::getOpenFileName()  // Diálogo Qt
    ↓
ImageLoader::loadImage(path)  // SIN Qt
    ↓
Emite: imageLoaded(QImage)  // Signal Qt
    ↓
ImageView::setImage(QImage)
    ↓
ImageView::repaint()  // Renderiza con QPainter
    ↓
User ve imagen
```

### Procesamiento

```
User arrastra zona
    ↓
ImageView::mouseMoveEvent(QMouseEvent)  // Qt event
    ↓
Emite: selectionChanged(rect)  // Signal Qt
    ↓
MainWindow::onSelectionChanged(rect)
    ↓
Filter::apply(image, rect)  // SIN Qt
    ↓
Emite: processingFinished(result)  // Signal Qt
    ↓
ImageView::setImage(result)
    ↓
repaint()
    ↓
User ve resultado
```

---

## 🔧 Problemas técnicos actuales de Qt

### 1. Qt 5 es legacy
- Qt 6 ya disponible desde 2021
- Qt 5 en mantenimiento
- Soporte limitado

### 2. Compilación lenta
- Meta Object Compiler (moc) lento
- Headers largos
- Muchas macros

### 3. Binarias grandes
- Qt libraries ~100MB
- Difícil distribuir

### 4. Acoplamiento a Qt
- Signals/slots requieren Q_OBJECT
- Limits en uso de patrones C++ modernos
- Qt strings (QString) vs std::string

---

## 🚀 Migración a wxWidgets

### ¿Por qué wxWidgets?

| Aspecto | Qt | wxWidgets |
|--------|----|----|
| **Licencia** | LGPL/Comercial | wxWindows (permisiva) |
| **Compilación** | Lenta (moc) | Rápida |
| **Tamaño** | Grande | Pequeño |
| **C++ moderno** | Limitado | Total |
| **Distribución** | Difícil | Fácil |

### Equivalencias wxWidgets

| Qt | wxWidgets |
|----|----|
| `QMainWindow` | `wxFrame` |
| `QDialog` | `wxDialog` |
| `QWidget` | `wxPanel` |
| `QImage/QPixmap` | `wxImage/wxBitmap` |
| `signals/slots` | Callbacks o eventos |
| `QThread` | `wxThread` |
| `QPainter` | `wxDC` |

### Ejemplo de migración

**Antes (Qt)**:
```cpp
class ImageView : public QWidget {
    Q_OBJECT
signals:
    void selectionChanged(QRect rect);
    
private slots:
    void onMousePress(QMouseEvent* event);
    
protected:
    void paintEvent(QPaintEvent* event) override;
};
```

**Después (wxWidgets)**:
```cpp
class ImageView : public wxPanel {
public:
    wxDECLARE_EVENT(wxEVT_SELECTION_CHANGED, wxNotifyEvent);
    
private:
    void OnMousePress(wxMouseEvent& event);
    void OnPaint(wxPaintEvent& event);
    
    wxDECLARE_EVENT_TABLE();
};
```

### Plan de migración por fases

**Fase 1** (1 semana): Preparar código
- Desacoplamiento mínimo
- Crear abstracciones para Qt
- Setup wxWidgets en CMake

**Fase 2** (2 semanas): Migrar UI
- ImageView → wxPanel
- Diálogos principales
- Menús y toolbars

**Fase 3** (1 semana): Testing y polish
- Pruebas exhaustivas
- Ajustes de interfaz
- Performance

---

## 📚 Documentación relacionada

- **Guía de migración**: [`WXWIDGETS_MIGRATION.md`](./WXWIDGETS_MIGRATION.md)
- **Ejemplos de refactoring**: [`REFACTORING_EXAMPLES.md`](./REFACTORING_EXAMPLES.md)
- **Plan de implementación**: [`IMPLEMENTATION_PLAN.md`](./IMPLEMENTATION_PLAN.md)
- **Análisis de sistemas**: [`QT_SYSTEMS.md`](./QT_SYSTEMS.md)

---

**Última actualización**: 1 de febrero de 2026  
**Rama**: `upgrade` (preparación para wxWidgets)
