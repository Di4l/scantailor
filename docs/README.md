# 📱 ScanTailor - Escáner de Documentos Avanzado

**ScanTailor** es una aplicación de escritorio multiplataforma para escanear, procesar y limpiar imágenes de documentos. Convierte escaneos caóticos en documentos limpios, bien formateados y listos para archivar.

---

## 🎯 ¿Qué es ScanTailor?

ScanTailor es una herramienta que automatiza las tareas tediosas del post-procesamiento de escaneo:

- **Detección de página**: Encuentra automáticamente los límites del contenido
- **Corrección de perspectiva**: Endereza páginas desalineadas
- **Rotación de contenido**: Detecta y corrige páginas invertidas o rotadas
- **Recorte inteligente**: Elimina márgenes y bordes blancos
- **Deskew**: Ajusta el ángulo de inclinación
- **Separación de columnas**: Detecta y maneja layouts multi-columna
- **Binarización**: Convierte a B/N con calidad optimizada

### Características principales:

✅ **Interfaz gráfica interactiva** - Visualización en tiempo real  
✅ **Procesamiento batch** - Procesa múltiples documentos automáticamente  
✅ **Exportación flexible** - PDF, TIFF, PNG con configuración personalizada  
✅ **Multiplataforma** - Windows, Linux, macOS (con wxWidgets)  
✅ **Módulos independientes** - Arquitectura plugin-friendly  

---

## 🚀 Compilación y Uso

### Requisitos

**Mínimos:**
- GCC 11+ o Clang 14+ (C++20 mínimo, C++23 en rama `upgrade`)
- CMake 3.20+
- Qt 5.15+ (versión actual)
- Boost 1.70+ (siendo eliminado en rama `upgrade`)
- zlib, libpng, libjpeg

**Para compilar:**
```bash
# Clonar repositorio
git clone https://github.com/Di4l/scantailor.git
cd scantailor
git checkout upgrade

# Crear build directory
mkdir -p build
cd build

# Configurar con CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Compilar
make -j$(nproc)

# Instalar (opcional)
sudo make install
```

### Uso básico

```bash
# Lanzar GUI
./bin/scantailor

# Procesamiento batch
./bin/scantailor-cli --input-dir scans/ --output-dir output/
```

---

## 📂 Estructura de Carpetas

```
scantailor/
├── src/                    # Código fuente principal
│   ├── ui/                 # Código Qt (GUI, ventanas, diálogos)
│   ├── filters/            # Algoritmos de procesamiento de imágenes
│   ├── interaction/        # Manejo de eventos, interacción del usuario
│   ├── zones/              # Gestión de zonas y regiones
│   ├── imageproc/          # Procesamiento de imágenes de bajo nivel
│   ├── math/               # Utilidades matemáticas
│   ├── foundation/         # Clases base y utilidades
│   ├── dewarping/          # Corrección de perspectiva
│   └── crash_reporter/     # Reportes de errores
│
├── tests/                  # Suite de pruebas unitarias
├── docs/                   # Documentación
│   ├── README.md          # Este archivo - guía general
│   ├── DEVELOPER.md        # Guía para desarrolladores
│   ├── ARCHITECTURE.md     # Descripción arquitectónica general
│   ├── BUILDING.md         # Instrucciones de compilación detalladas
│   ├── CONTRIBUTING.md     # Guía de contribución
│   │
│   ├── qt/                # Todo sobre Qt y migración a wxWidgets
│   │   ├── QT_ANALYSIS.md
│   │   ├── QT_SYSTEMS.md
│   │   ├── WXWIDGETS_MIGRATION.md
│   │   ├── REFACTORING_EXAMPLES.md
│   │   └── IMPLEMENTATION_PLAN.md
│   │
│   └── boost/             # Todo sobre BOOST y migración a C++23
│       ├── BOOST_ANALYSIS.md
│       ├── BOOST_TO_CPP23.md
│       ├── BOOST_MIGRATION_GUIDE.md
│       ├── BOOST_EXAMPLES.md
│       └── migrate_boost_phase1.sh
│
├── cmake/                 # Configuración CMake
├── build/                 # Build directory (no commitear)
├── CMakeLists.txt         # Configuración de compilación
├── README.md (raíz)       # Resumen ejecutivo
└── .git/
```

---

## 🔧 Para Desarrolladores

### Agregar nueva funcionalidad

#### 1. **¿Es un filtro de procesamiento de imágenes?**
   → Carpeta: `src/filters/tu_filtro/`
   - Crea subcarpeta con nombre descriptivo
   - Archivos: `TuFiltro.h`, `TuFiltro.cpp`, `CMakeLists.txt`
   - Registra en `src/filters/CMakeLists.txt`

**Ejemplo:**
```cpp
// src/filters/despeckle/Despeckle.h
class Despeckle {
    BWImage apply(const BWImage& img);
};
```

#### 2. **¿Es lógica matemática o de utilidad?**
   → Carpeta: `src/math/` o `src/foundation/`
   - Encabezados puros (.h) preferiblemente
   - Implementaciones en .cpp si es complejo

#### 3. **¿Es interfaz de usuario?**
   → Carpeta: `src/ui/`
   - Pantallas: `src/ui/MainWindow.cpp`
   - Diálogos: `src/ui/dialogs/TuDialog.cpp`
   - Widgets customizados: `src/ui/widgets/TuWidget.cpp`

#### 4. **¿Es interacción/evento?**
   → Carpeta: `src/interaction/`
   - Handlers de mouse: `src/interaction/InteractionHandler.cpp`
   - Deslizadores/gestos: `src/interaction/`

### Dependencias de bibliotecas

| Área | Actual | Alternativa C++23 |
|------|--------|-------------------|
| GUI | Qt 5.15+ | wxWidgets 3.2+ |
| Contenedores | boost::multi_index | std::map/std::set |
| Rango | BOOST_FOREACH | range-for |
| Punteros | boost::shared_ptr | std::shared_ptr |
| Utilidades | boost::* | std:: (C++23) |

### Compilación incremental

```bash
cd build
make -j$(nproc)        # Compilación rápida
make -j1 VERBOSE=1     # Detalles de compilación
cmake --build . --target clean  # Limpia build
```

### Ejecución con debugging

```bash
# Con gdb
gdb ./bin/scantailor

# Con valgrind (memory leaks)
valgrind --leak-check=full ./bin/scantailor

# Con Qt Creator
qtcreator CMakeLists.txt
```

---

## 📊 Métodos en desarrollo

```
Compilación:       CMake (C++20 mínimo, C++23 en upgrade)
Build System:      Make / Ninja
VCS:               Git
Rama principal:    upgrade
Testing:           Google Test (tests/)
Documentación:     Markdown + Doxygen
CI/CD:             (a configurar)
```

---

## 🔗 Documentación relacionada

- **Desarrolladores nuevos**: Lee primero [`DEVELOPER.md`](./DEVELOPER.md)
- **Arquitectura**: Ver [`ARCHITECTURE.md`](./ARCHITECTURE.md)
- **Compilación avanzada**: Ver [`BUILDING.md`](./BUILDING.md)
- **Contribuciones**: Ver [`CONTRIBUTING.md`](./CONTRIBUTING.md)
- **Migración Qt**: Ver [`qt/`](./qt/) (migración a wxWidgets)
- **Migración BOOST**: Ver [`boost/`](./boost/) (migración a C++23)

---

## 📜 Licencia

GNU General Public License v3.0 - Ver `COPYING` en la raíz del proyecto

---

## 👥 Contribuir

Para reportar bugs, sugerir features, o contribuir código:
1. Lee [`CONTRIBUTING.md`](./CONTRIBUTING.md)
2. Fork el repositorio
3. Crea rama feature: `git checkout -b feature/mi-feature`
4. Commit cambios: `git commit -am 'Add new feature'`
5. Push: `git push origin feature/mi-feature`
6. Abre Pull Request

---

**Última actualización**: 1 de febrero de 2026  
**Rama**: `upgrade` (C++23 + wxWidgets)
