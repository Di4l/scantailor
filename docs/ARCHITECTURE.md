# 🏗️ Arquitectura de ScanTailor

Descripción técnica de la arquitectura del proyecto y sus componentes principales.

---

## 📐 Vista general de la arquitectura

```
┌─────────────────────────────────────────────────────────────┐
│                      UI LAYER (Qt)                          │
│  MainWindow | Dialogs | Widgets | Interactions              │
└──────────────────────────┬──────────────────────────────────┘
                           │
┌──────────────────────────▼──────────────────────────────────┐
│               FILTER PIPELINE                                │
│  FixOrientation | PageLayout | Deskew | Output | etc.       │
└──────────────────────────┬──────────────────────────────────┘
                           │
┌──────────────────────────▼──────────────────────────────────┐
│            IMAGE PROCESSING (imageproc)                      │
│  BinaryImage | GrayImage | Transformations | Convolutions   │
└──────────────────────────┬──────────────────────────────────┘
                           │
┌──────────────────────────▼──────────────────────────────────┐
│              MATH & UTILITIES                                │
│  Geometry | Matrix | Algorithms                              │
└─────────────────────────────────────────────────────────────┘
```

---

## 🔄 Flujo de datos principal

### Procesamiento de documento

```
1. Usuario carga imagen
   ↓
2. Detecta dimensiones y contenido
   ↓
3. Aplica filtros secuencialmente:
   - fix_orientation/    : Detecta orientación de página
   - page_split/         : Separa páginas si hay dos
   - select_content/     : Detecta área de contenido
   - page_layout/        : Calcula márgenes
   - deskew/             : Endereza si está torcida
   - output/             : Genera documento final
   ↓
4. Exporta en formato solicitado (PDF, TIFF, PNG)
```

### Interacción del usuario

```
User Click/Drag
   ↓
InteractionHandler captura evento
   ↓
Modifica zona o parámetro
   ↓
Triggea re-procesamiento
   ↓
ImageView renderiza resultado
   ↓
Usuario ve cambio en tiempo real
```

---

## 📦 Componentes principales

### 1. **UI Layer** (`src/ui/`)

**Responsabilidad**: Interfaz gráfica y manejo de eventos Qt

**Módulos**:
- `MainWindow` - Ventana principal y coordinación
- `ImageView` - Widget de visualización de imágenes
- `dialogs/` - Diálogos de configuración
- `widgets/` - Widgets customizados

**Dependencias**: Qt 5.15+

**Ejemplo de flujo**:
```cpp
MainWindow::onImageLoaded() {
    Image img = loadImage(path);
    imageView->setImage(img);
    updateFilters();
}
```

### 2. **Filter Pipeline** (`src/filters/`)

**Responsabilidad**: Algoritmos de procesamiento por filtro

**Filtros disponibles**:
- `fix_orientation/FixOrientation` - Detecta rotación de página
- `page_split/PageSplit` - Separa múltiples páginas
- `select_content/SelectContent` - Detecta área de contenido
- `page_layout/PageLayout` - Calcula márgenes y espacios
- `deskew/Deskew` - Corrige inclinación
- `output/Output` - Genera formato final

**Arquitectura de filtro**:
```cpp
class Filter {
public:
    virtual Image apply(const Image& input) = 0;
};
```

**Cada filtro es independiente** - No se comunican entre sí, solo pasan datos.

### 3. **Image Processing** (`src/imageproc/`)

**Responsabilidad**: Manipulación de píxeles y transformaciones

**Clases principales**:
- `BinaryImage` - Imagen blanco y negro (1 bit)
- `GrayImage` - Imagen escala de grises (8 bits)
- `RgbImage` - Imagen RGB (24 bits)

**Operaciones comunes**:
```cpp
BinaryImage img = loadImage("doc.png");

// Transformación geométrica
BinaryImage rotated = img.rotate(5.5);  // 5.5 grados
BinaryImage scaled = img.scale(2.0);    // 2x zoom

// Operaciones de píxel
BinaryImage inverted = img.inverted();
BinaryImage dilated = img.dilate(3);    // Expandir píxeles negros
```

### 4. **Interaction** (`src/interaction/`)

**Responsabilidad**: Manejo de eventos del usuario

**Tipos de interacción**:
- Click del ratón
- Arrastres para seleccionar zonas
- Zoom y pan
- Modificación interactiva de zonas

### 5. **Zones** (`src/zones/`)

**Responsabilidad**: Gestión de regiones especiales

**Conceptos**:
- `Zone` - Región rectangular en la imagen
- `ZoneSet` - Conjunto de zonas (ej: márgenes)
- Detección automática vs manual

### 6. **Dewarping** (`src/dewarping/`)

**Responsabilidad**: Corrección de perspectiva

Corrige distorsión causada por fotografiar documentos en ángulo.

**Matemática**: Transforma perspectiva a plano usando malla de control.

### 7. **Math** (`src/math/`)

**Responsabilidad**: Utilidades matemáticas

**Contiene**:
- `Geometry` - Puntos, líneas, rectángulos
- `Matrix` - Operaciones matriciales
- Transformaciones geométricas
- Algoritmos de búsqueda

### 8. **Foundation** (`src/foundation/`)

**Responsabilidad**: Clases base y utilidades generales

**Contiene**:
- `IntrusivePtr` - Puntero intrusivo (referencia contada)
- `Pool` - Pool de memoria
- Traits y type utilities
- Macros de propiedades Qt

---

## 🔌 Patrones de diseño utilizados

### 1. **Pipeline Pattern**
Filtros se aplican secuencialmente, cada uno procesa salida del anterior.

```cpp
BinaryImage result = img
    .apply(new FixOrientation())
    .apply(new PageSplit())
    .apply(new SelectContent())
    .apply(new Output());
```

### 2. **Observer Pattern**
Qt usa signals/slots para eventos:

```cpp
connect(imageView, SIGNAL(selectionChanged(Zone)),
        this, SLOT(onZoneSelected(Zone)));
```

### 3. **Strategy Pattern**
Diferentes algoritmos para un mismo problema (ej: binarización):

```cpp
BinaryImage binarize(const GrayImage& img, Strategy strategy);
```

### 4. **Factory Pattern**
Creación de filtros:

```cpp
Filter* filter = FilterFactory::create("deskew", params);
```

---

## 🧮 Tipos de datos principales

### Imágenes

```cpp
// Blanco y negro (1 bit por píxel)
class BinaryImage {
    void setPixel(int x, int y, bool white);
    bool getPixel(int x, int y) const;
    BinaryImage scale(double factor) const;
    BinaryImage rotate(double degrees) const;
};

// Escala de grises (8 bits)
class GrayImage {
    void setPixel(int x, int y, uint8_t gray);
    uint8_t getPixel(int x, int y) const;
};

// RGB (24 bits)
class RgbImage {
    void setPixel(int x, int y, const QColor& color);
    QColor getPixel(int x, int y) const;
};
```

### Geometría

```cpp
struct Point { int x, y; };
struct Rect { int left, top, width, height; };
struct Line { Point from, to; };

struct LineSegment {
    Point start, end;
    double length() const;
    double angle() const;
};
```

### Zonas

```cpp
class Zone {
    Rect bounds() const;
    bool contains(const Point& p) const;
    void translate(int dx, int dy);
};

class ZoneSet {
    void add(const Zone& z);
    void remove(const Zone& z);
    std::vector<Zone> zones() const;
};
```

---

## 🔗 Dependencias de módulos

```
ui/
  ├─→ filters/
  ├─→ imageproc/
  ├─→ interaction/
  └─→ foundation/

filters/
  ├─→ imageproc/
  ├─→ math/
  ├─→ zones/
  ├─→ dewarping/
  └─→ foundation/

imageproc/
  ├─→ math/
  └─→ foundation/

dewarping/
  ├─→ imageproc/
  ├─→ math/
  └─→ foundation/

math/
  └─→ foundation/

interaction/
  ├─→ zones/
  ├─→ math/
  └─→ foundation/

zones/
  ├─→ math/
  └─→ foundation/

foundation/
  └─→ (no dependencias de proyecto)
```

---

## 🔄 Ciclo de vida de un documento

### Fase 1: Carga
```
Archivo en disco
    ↓ (loadImage)
BinaryImage en memoria
    ↓
Mostrar en ImageView
```

### Fase 2: Análisis
```
FixOrientation analiza píxeles
    ↓
Detecta ángulo de rotación
    ↓
Guarda metadatos de orientación
```

### Fase 3: Procesamiento interactivo
```
Usuario selecciona zona manualmente
    ↓
SelectContent calcula área automática
    ↓
PageLayout calcula márgenes
    ↓
Usuario ajusta si es necesario
```

### Fase 4: Exportación
```
Output aplica transformaciones finales
    ↓
Aplica binarización final
    ↓
Exporta en formato (PDF/TIFF/PNG)
```

---

## 🎯 Decisiones arquitectónicas clave

### 1. **Separación UI ↔ Lógica**
- Toda la lógica en `filters/`, `imageproc/`, `math/`
- Solo Qt en `ui/`
- Permite migración a wxWidgets

### 2. **Filtros independientes**
- Cada filtro no conoce a los demás
- Facilita agregar/remover filtros
- Permite testing independiente

### 3. **Inmutabilidad de imágenes**
- Las imágenes no se modifican in-place
- Cada operación retorna nueva imagen
- Facilita undo/redo

### 4. **Referencia contada**
- Uso de `IntrusivePtr` en lugar de `new/delete`
- Evita memory leaks
- Transferencia automática de propiedad

---

## 🚀 Futuro: Migración a C++23 + wxWidgets

Ver documentación detallada en:
- [`qt/WXWIDGETS_MIGRATION.md`](./qt/WXWIDGETS_MIGRATION.md)
- [`boost/BOOST_TO_CPP23.md`](./boost/BOOST_TO_CPP23.md)

**Cambios principales**:
1. Reemplazar Qt → wxWidgets (solo afecta `src/ui/`)
2. Reemplazar Boost → std:: (afecta varios módulos)
3. Pasar a C++23 (permite usar nuevas características)
4. Arquitectura lógica **permanece igual**

---

**Última actualización**: 1 de febrero de 2026  
**Rama**: `upgrade`
