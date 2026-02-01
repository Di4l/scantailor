# 🎬 Sistemas Qt principales en ScanTailor

Análisis de los 5 sistemas principales que usan Qt.

---

## 📋 Los 5 sistemas

```
1. Visualización de imágenes (ImageView)        - CRÍTICO
2. Interfaz principal (MainWindow + diálogos)  - CRÍTICO
3. Interacción del usuario (mouse, eventos)    - CRÍTICO
4. Threading de procesamiento (QThread)        - IMPORTANTE
5. Sistema de preferencias (QSettings)         - MENOR
```

---

## 1️⃣ VISUALIZACIÓN DE IMÁGENES (ImageView)

### Ubicación
`src/ui/ImageView.h / ImageView.cpp`

### Responsabilidad
Mostrar imagen actual y permitir interacción (zoom, pan, selección)

### Métodos principales

```cpp
class ImageView : public QWidget {
    Q_OBJECT
    
public:
    void setImage(const QImage& img);
    QImage currentImage() const;
    void setZoom(double factor);
    void setPan(int dx, int dy);
    QRect selectionRect() const;
    
signals:
    void selectionChanged(const QRect& rect);
    void imageClicked(const QPoint& pos);
    
protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;
    
private:
    QImage image_;
    QRect selection_;
    double zoomFactor_;
    QPoint panOffset_;
};
```

### Acoplamiento con Qt

| Componente | Acoplamiento | Razón |
|------------|-------------|-------|
| `QImage` | ALTO | Almacenamiento de píxeles |
| `QPainter` | ALTO | Renderizado |
| `QMouseEvent` | ALTO | Eventos del mouse |
| `QPaintEvent` | ALTO | Evento de pintado |
| `QWidget` | ALTO | Base para control |

### Equivalente en wxWidgets

```cpp
class ImageView : public wxPanel {
public:
    void setImage(const wxImage& img);
    wxImage currentImage() const;
    void setZoom(double factor);
    
private:
    wxBitmap bitmap_;
    wxRect selection_;
    
    void OnPaint(wxPaintEvent& event);
    void OnMouseDown(wxMouseEvent& event);
    void OnMouseMove(wxMouseEvent& event);
    void OnMouseUp(wxMouseEvent& event);
    
    wxDECLARE_EVENT_TABLE();
};
```

### Esfuerzo de migración: **ALTO (1-2 días)**
- Reescribir pintado
- Reescribir manejo de eventos
- Ajustar zooms/pan

---

## 2️⃣ INTERFAZ PRINCIPAL (MainWindow + Diálogos)

### Ubicación
```
src/ui/MainWindow.h / MainWindow.cpp
src/ui/dialogs/*.h / *.cpp
```

### MainWindow

```cpp
class MainWindow : public QMainWindow {
    Q_OBJECT
    
public:
    MainWindow(QWidget* parent = nullptr);
    
private slots:
    void onOpenFile();
    void onSaveProject();
    void onApplyFilter(int filterType);
    void onImageProcessed(const QImage& result);
    void onSettingsChanged();
    
    void setupMenus();
    void setupToolbars();
    void createConnections();
    
private:
    QMenu* fileMenu_;
    QToolBar* toolbar_;
    ImageView* imageView_;
    QStatusBar* statusBar_;
};
```

### Diálogos principales

| Diálogo | Propósito |
|---------|-----------|
| `SettingsDialog` | Configurar filtros |
| `ProjectDialog` | Crear/abrir proyecto |
| `ExportDialog` | Opciones de exportación |
| `AboutDialog` | Información de app |

### Acoplamiento con Qt

| Componente | Acoplamiento | Razón |
|------------|-------------|-------|
| `QMainWindow` | ALTO | Base de aplicación |
| `QMenu/QAction` | MEDIO | Menús |
| `QDialog` | MEDIO | Diálogos modales |
| `QStatusBar` | BAJO | Barra de estado |
| `QToolBar` | BAJO | Barra de herramientas |

### Equivalente en wxWidgets

```cpp
class MainWindow : public wxFrame {
public:
    MainWindow(const wxString& title);
    
private:
    void OnOpenFile(wxCommandEvent& event);
    void OnApplyFilter(wxCommandEvent& event);
    
    wxMenuBar* menuBar_;
    wxToolBar* toolBar_;
    ImageView* imageView_;
    
    wxDECLARE_EVENT_TABLE();
};
```

### Esfuerzo de migración: **ALTO (1-2 días)**
- Reescribir menús
- Reescribir diálogos
- Ajustar event handling

---

## 3️⃣ INTERACCIÓN DEL USUARIO

### Ubicación
`src/interaction/` y métodos en `ui/ImageView`

### Tipos de interacción

| Interacción | Actual (Qt) | Cambio |
|------------|----------|--------|
| Click | `mousePressEvent()` | Seguirá siendo igual |
| Arrastre | `mouseMoveEvent()` | Seguirá siendo igual |
| Scroll | `wheelEvent()` | Seguirá siendo igual |
| Teclas | `keyPressEvent()` | Seguirá siendo igual |
| Doble-click | `mouseDoubleClickEvent()` | Seguirá siendo igual |

### Código típico

```cpp
void ImageView::mousePressEvent(QMouseEvent* event) {
    QPoint pos = event->pos();
    selectionStart_ = pos;
    selection_ = QRect(pos, QSize(0, 0));
    
    emit selectionChanged(selection_);
    update();  // Redraw
}

void ImageView::mouseMoveEvent(QMouseEvent* event) {
    if (event->buttons() & Qt::LeftButton) {
        QPoint pos = event->pos();
        selection_ = QRect(selectionStart_, pos).normalized();
        
        emit selectionChanged(selection_);
        update();  // Redraw
    }
}
```

### Equivalente en wxWidgets

```cpp
void ImageView::OnMouseDown(wxMouseEvent& event) {
    wxPoint pos = event.GetPosition();
    selectionStart_ = pos;
    
    Refresh();  // Redraw
}

void ImageView::OnMouseMove(wxMouseEvent& event) {
    if (event.LeftIsDown()) {
        wxPoint pos = event.GetPosition();
        // Actualizar selección
        Refresh();
    }
}
```

### Esfuerzo de migración: **BAJO-MEDIO (½-1 día)**
- Eventos muy similares en wxWidgets
- Solo cambiar métodos override
- Lógica sin cambios

---

## 4️⃣ THREADING (QThread)

### Ubicación
Usado en procesamiento de imágenes (`src/filters/` con Qt)

### Patrón actual

```cpp
class ImageProcessor : public QThread {
    Q_OBJECT
    
    void run() override {
        // Procesa en background thread
        QImage result = processImage(input_);
        
        // Emite signal para ui thread
        emit processingFinished(result);
    }
    
signals:
    void processingFinished(const QImage& result);
    
private:
    QImage input_;
};

// Uso en MainWindow
void MainWindow::onApplyFilter() {
    processor_ = new ImageProcessor();
    processor_->setInput(currentImage_);
    
    connect(processor_, SIGNAL(processingFinished(QImage)),
            this, SLOT(onProcessingFinished(QImage)));
    
    processor_->start();
}
```

### Equivalente en wxWidgets

```cpp
class ImageProcessor : public wxThread {
public:
    wxThread::ExitCode Entry() override {
        wxImage result = processImage(input_);
        
        // Envía evento al main thread
        wxQueueEvent(parent_, 
            new wxThreadEvent(wxEVT_THREAD, GetId(), result));
        
        return nullptr;
    }
    
private:
    wxImage input_;
    wxEvtHandler* parent_;
};
```

### Esfuerzo de migración: **MEDIO (1 día)**
- Cambiar signals/slots → eventos
- Cambiar QImage → wxImage
- Mismo patrón general

---

## 5️⃣ SISTEMA DE PREFERENCIAS (QSettings)

### Ubicación
Disperso en MainWindow y diálogos

### Uso actual

```cpp
// Guardar configuración
QSettings settings("ScanTailor", "ScanTailor");
settings.setValue("windowSize", mainWindow->size());
settings.setValue("lastDirectory", lastDir_);
settings.setValue("filterParams/blur/radius", 5);

// Cargar configuración
QSettings settings("ScanTailor", "ScanTailor");
QSize size = settings.value("windowSize", QSize(800, 600)).toSize();
QString lastDir = settings.value("lastDirectory", "").toString();
int radius = settings.value("filterParams/blur/radius", 3).toInt();
```

### Equivalente en wxWidgets

```cpp
// Guardar
wxConfigBase* config = wxConfigBase::Get();
config->Write("/Window/Width", mainWindow->GetSize().x);
config->Write("/LastDirectory", lastDir);
config->Write("/Filters/BlurRadius", 5);
config->Flush();

// Cargar
long width = config->Read("/Window/Width", 800L);
wxString lastDir = config->Read("/LastDirectory", "");
int radius = config->Read("/Filters/BlurRadius", 3);
```

### Esfuerzo de migración: **BAJO (½ día)**
- API muy similar
- Solo cambiar método de acceso

---

## 📊 Resumen de migración por sistema

| Sistema | Líneas LoC | Complejidad | Esfuerzo | Días |
|---------|-----------|-------------|----------|------|
| ImageView | 1,000 | ALTA | ALTO | 1-2 |
| MainWindow | 2,000 | ALTA | ALTO | 1-2 |
| Interacción | 500 | MEDIA | BAJO | ½-1 |
| Threading | 300 | MEDIA | MEDIO | 1 |
| Preferencias | 200 | BAJA | BAJO | ½ |
| **TOTAL** | **~4,000** | | | **4-6.5** |

---

## 🔄 Orden recomendado de migración

1. **Paso 1**: Preferencias (QSettings → wxConfig)
   - Más simple
   - Genera confianza
   - Tiempo: ½ día

2. **Paso 2**: Interacción (eventos)
   - Mouse/teclado similar
   - Poco acoplamiento
   - Tiempo: ½-1 día

3. **Paso 3**: Threading
   - Patrón bien definido
   - Poco código
   - Tiempo: 1 día

4. **Paso 4**: MainWindow
   - Mediano acoplamiento
   - Menús/diálogos
   - Tiempo: 1-2 días

5. **Paso 5**: ImageView
   - Más complejo (pintado)
   - Último porque es crítico
   - Tiempo: 1-2 días

---

## 📚 Documentación relacionada

- **Análisis Qt general**: [`QT_ANALYSIS.md`](./QT_ANALYSIS.md)
- **Ejemplos de refactoring**: [`REFACTORING_EXAMPLES.md`](./REFACTORING_EXAMPLES.md)
- **Plan de migración**: [`IMPLEMENTATION_PLAN.md`](./IMPLEMENTATION_PLAN.md)

---

**Última actualización**: 1 de febrero de 2026  
**Rama**: `upgrade`
