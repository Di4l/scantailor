# 👨‍💻 Guía para Desarrolladores

Bienvenido al desarrollo de ScanTailor. Esta guía te ayudará a entender el proyecto y contribuir efectivamente.

---

## 🚀 Comienza en 30 minutos

### Paso 1: Clonar y compilar

```bash
git clone https://github.com/Di4l/scantailor.git
cd scantailor
git checkout upgrade
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
make -j$(nproc)
./bin/scantailor  # Lanza la aplicación
```

### Paso 2: Familiarízate con la estructura

Abre estos archivos en este orden:
1. `src/main.cpp` - Punto de entrada
2. `src/ui/MainWindow.h` - Ventana principal
3. `src/filters/` - Alguno de los filtros disponibles
4. `CMakeLists.txt` - Cómo se compila todo

### Paso 3: Entiende el flujo de datos

```
Usuario interactúa en UI
    ↓
MainWindow captura evento
    ↓
Pasa a Filter correspondiente
    ↓
Filter procesa imagen (ImageProc)
    ↓
Resultado se devuelve a UI
    ↓
Se renderiza en pantalla
```

---

## 📂 Dónde agregar código

### Nuevo filtro de procesamiento de imagen

**Ubicación**: `src/filters/tu_filtro/`

```
src/filters/blur/
├── CMakeLists.txt
├── Blur.h
└── Blur.cpp
```

**Paso a paso**:

1. Crea carpeta: `mkdir -p src/filters/tu_filtro/`

2. Crea `Blur.h`:
```cpp
#ifndef SCANTAILOR_BLUR_H
#define SCANTAILOR_BLUR_H

#include <imageproc/BinaryImage.h>

class Blur {
public:
    BinaryImage apply(const BinaryImage& input, int radius = 3);
};

#endif
```

3. Crea `Blur.cpp`:
```cpp
#include "Blur.h"

BinaryImage Blur::apply(const BinaryImage& input, int radius) {
    // Implementación
    return input;  // Placeholder
}
```

4. Crea `CMakeLists.txt`:
```cmake
project(blur)
set(SRCS Blur.cpp Blur.h)
add_library(blur OBJECT ${SRCS})
target_include_directories(blur PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
```

5. Edita `src/filters/CMakeLists.txt`:
```cmake
add_subdirectory(blur)  # Agrega esta línea
# ... resto de filtros
target_sources(scantailor PRIVATE $<TARGET_OBJECTS:blur>)
```

### Interfaz de usuario (UI)

**Ubicación**: `src/ui/`

```
src/ui/
├── MainWindow.h / MainWindow.cpp       # Ventana principal
├── dialogs/
│   ├── SettingsDialog.h
│   └── SettingsDialog.cpp
└── widgets/
    ├── ImageView.h
    └── ImageView.cpp
```

**Ejemplo - Crear nuevo diálogo**:

1. `src/ui/dialogs/MyDialog.h`:
```cpp
#ifndef SCANTAILOR_MYDIALOG_H
#define SCANTAILOR_MYDIALOG_H

#include <QDialog>

class MyDialog : public QDialog {
    Q_OBJECT
public:
    MyDialog(QWidget* parent = nullptr);
private slots:
    void onOkClicked();
};

#endif
```

2. Registra en `CMakeLists.txt` de `src/ui/`

### Lógica matemática o utilidades

**Ubicación**: `src/math/` o `src/foundation/`

Preferiblemente como headers puros (`.h` sin `.cpp`):

```cpp
// src/math/Geometry.h
#ifndef SCANTAILOR_GEOMETRY_H
#define SCANTAILOR_GEOMETRY_H

namespace math {
    struct Point { int x, y; };
    
    inline double distance(const Point& a, const Point& b) {
        return std::sqrt((a.x - b.x) * (a.x - b.x) + 
                        (a.y - b.y) * (a.y - b.y));
    }
}

#endif
```

---

## 🏗️ Arquitectura por módulos

### `src/imageproc/` - Procesamiento de imágenes bajo nivel

Funciones de manipulación de píxeles, transformaciones geométricas, etc.

**Archivos clave**:
- `BinaryImage.h` - Imagen blanca y negra
- `GrayImage.h` - Imagen en escala de grises
- `RgbImage.h` - Imagen RGB

```cpp
BinaryImage img;  // B/W
BinaryImage result = img.scale(2.0);  // Escalar 2x
```

### `src/filters/` - Filtros de procesamiento

Cada filtro es independiente. Toman una imagen, retornan una imagen procesada.

**Estructura típica**:
```cpp
class MiFilter {
    BinaryImage apply(const BinaryImage& input);
};
```

### `src/ui/` - Interfaz Qt

Todo lo relacionado con Qt y la GUI.

**Componentes**:
- `MainWindow` - Ventana principal
- `ImageView` - Widget que muestra la imagen
- Diálogos de configuración
- Menús y toolbars

### `src/interaction/` - Manejo de interacción

Cómo el usuario interactúa:
- Clicks de mouse
- Arrastres
- Rotaciones
- Selecciones

### `src/zones/` - Gestión de zonas

Regiones especiales en la imagen (márgenes, zona de contenido, etc.)

### `src/dewarping/` - Corrección de perspectiva

Endereza páginas distorsionadas por perspectiva de cámara.

---

## 🔄 Ciclo de desarrollo típico

### 1. Crear rama de feature

```bash
git checkout -b feature/mi-nueva-funcionalidad
```

### 2. Modificar código

```bash
# Edita archivos necesarios
code src/filters/mi_filtro/MiFiltro.cpp
```

### 3. Compilar y probar

```bash
cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
make -j$(nproc)

# Pruebas unitarias
make test

# Ejecución manual
./bin/scantailor
```

### 4. Commit y push

```bash
git add .
git commit -m "Add new feature: description"
git push origin feature/mi-nueva-funcionalidad
```

### 5. Pull Request

Abre PR en GitHub con descripción de cambios.

---

## 🐛 Debugging

### Con GDB

```bash
cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug  # IMPORTANTE: Debug
make
gdb ./bin/scantailor
(gdb) run
(gdb) bt  # Backtrace cuando falla
```

### Con Qt Creator

```bash
qtcreator CMakeLists.txt
# Abre interfaz gráfica de debugging
```

### Memory leaks con Valgrind

```bash
valgrind --leak-check=full --show-leak-kinds=all ./bin/scantailor
```

### Logs

```cpp
#include <iostream>

// En cualquier parte del código:
std::cout << "Debug: valor=" << valor << std::endl;
std::cerr << "Error: algo falló" << std::endl;
```

---

## 📦 Dependencias del proyecto

| Librería | Versión | Propósito | Estado |
|----------|---------|----------|--------|
| **Qt** | 5.15+ | GUI | Actual |
| **Boost** | 1.70+ | Utilidades | 🔄 Siendo eliminado |
| **zlib** | - | Compresión | Estable |
| **libpng** | - | Imágenes PNG | Estable |
| **libjpeg** | - | Imágenes JPEG | Estable |
| **CMake** | 3.20+ | Build system | Requerido |

**Para C++23 (rama upgrade)**:
- Reemplazar Qt → wxWidgets
- Reemplazar Boost → std:: y C++23

Ver documentación en [`qt/`](./qt/) y [`boost/`](./boost/)

---

## 🚦 Compilación avanzada

### Build Release optimizado

```bash
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS="-O3 -march=native"
make -j$(nproc)
```

### Limpiar build

```bash
cd build
make clean
# O más agresivo:
cd ..
rm -rf build
mkdir build && cd build && cmake .. && make
```

### Build con flags específicos

```bash
cmake .. \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_CXX_STANDARD=23 \
  -DCMAKE_CXX_COMPILER=g++ \
  -DCMAKE_VERBOSE_MAKEFILE=ON
```

---

## 📝 Convenciones de código

### Nombres

- **Clases**: `CamelCase` - `class ImageProcessor`
- **Funciones**: `camelCase` - `void processImage()`
- **Variables**: `snake_case` - `int image_width`
- **Constantes**: `UPPER_CASE` - `const int MAX_SIZE = 1000`

### Estilo

```cpp
// Headers
#ifndef SCANTAILOR_MYCLASS_H
#define SCANTAILOR_MYCLASS_H

class MyClass {
public:
    MyClass();
    ~MyClass();
    
    int getValue() const;
    void setValue(int val);
    
private:
    int value_;
};

#endif
```

### Documentación

```cpp
/**
 * Procesa una imagen aplicando filtro Gaussian.
 * 
 * @param input Imagen de entrada (blanco y negro)
 * @param radius Radio del blur (píxeles)
 * @return Imagen procesada
 * @throws std::invalid_argument si radius < 0
 */
BinaryImage gaussianBlur(const BinaryImage& input, int radius);
```

---

## 🤝 Antes de hacer commit

- [ ] Código compila sin warnings
- [ ] Tests pasan (si aplica)
- [ ] Nombres siguen convenciones
- [ ] No hay código comentado innecesario
- [ ] Mensajes de commit son descriptivos
- [ ] Cambios en `docs/` si es necesario

---

## 📚 Documentación adicional

- **Estructura del proyecto**: [`ARCHITECTURE.md`](./ARCHITECTURE.md)
- **Instrucciones de compilación**: [`BUILDING.md`](./BUILDING.md)
- **Análisis Qt**: [`qt/`](./qt/README.md)
- **Análisis BOOST**: [`boost/`](./boost/README.md)

---

**¿Preguntas?** Consulta la documentación o abre una issue en GitHub.
