# 📱 ScanTailor

**Escáner de documentos avanzado** con post-procesamiento interactivo para digitalizar documentos de forma profesional.

---

## 🎯 ¿Qué es ScanTailor?

ScanTailor automatiza las tareas tediosas del post-procesamiento de escaneo:

- 📄 Detección automática de páginas
- 🔄 Corrección de perspectiva e inclinación
- ✂️ Recorte y detección de contenido
- 🎨 Binarización inteligente
- 📊 Exportación a PDF, TIFF, PNG

**Rama actual**: `upgrade` - Preparación para C++23 + wxWidgets

---

## 📚 Documentación

La documentación completa está en [`docs/`](./docs/) organizada por categoría:

### 🚀 Comienza aquí (5-10 minutos)

1. **¿Qué es esto?** → [`docs/README.md`](./docs/README.md)
2. **Compilar rápido** → [`docs/BUILDING.md`](./docs/BUILDING.md) (primeras 3 secciones)
3. **Índice general** → [`docs/INDEX.md`](./docs/INDEX.md)

### 👨‍💻 Para Desarrolladores

- **Guía de inicio**: [`docs/DEVELOPER.md`](./docs/DEVELOPER.md) (20 min)
- **Arquitectura**: [`docs/ARCHITECTURE.md`](./docs/ARCHITECTURE.md) (30 min)
- **Estructura de código**: [`docs/PROJECT_STRUCTURE.md`](./docs/PROJECT_STRUCTURE.md)
- **Compilación avanzada**: [`docs/BUILDING.md`](./docs/BUILDING.md)

### 🔴 Qt → wxWidgets Migration

- **Análisis**: [`docs/qt/README.md`](./docs/qt/README.md) ← Comienza aquí
- **Dónde está Qt**: [`docs/qt/QT_ANALYSIS.md`](./docs/qt/QT_ANALYSIS.md)
- **5 sistemas Qt**: [`docs/qt/QT_SYSTEMS.md`](./docs/qt/QT_SYSTEMS.md)
- **Plan de migración**: [`docs/qt/IMPLEMENTATION_PLAN.md`](./docs/qt/IMPLEMENTATION_PLAN.md)
- **Ejemplos de código**: [`docs/qt/REFACTORING_EXAMPLES.md`](./docs/qt/REFACTORING_EXAMPLES.md)

### 📦 BOOST → C++23 Migration

- **Análisis**: [`docs/boost/README.md`](./docs/boost/README.md) ← Comienza aquí
- **Dónde está BOOST**: [`docs/boost/BOOST_ANALYSIS.md`](./docs/boost/BOOST_ANALYSIS.md)
- **Guía de migración**: [`docs/BOOST_MIGRATION_GUIDE.md`](./docs/BOOST_MIGRATION_GUIDE.md)
- **Script automatizado**: [`docs/boost/migrate_boost_phase1.sh`](./docs/boost/migrate_boost_phase1.sh)
- **Ejemplos de código**: [`docs/BOOST_EXAMPLES.md`](./docs/BOOST_EXAMPLES.md)

---

## ⚡ Quick Start (3 minutos)

```bash
# Clonar
git clone https://github.com/Di4l/scantailor.git
cd scantailor
git checkout upgrade

# Compilar
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)

# Ejecutar
./bin/scantailor
```

---

## 🛠️ Requisitos

- **Compilador**: GCC 11+ o Clang 14+ (C++20 mínimo, C++23 en rama `upgrade`)
- **Build**: CMake 3.20+
- **Qt**: Qt 5.15+ (siendo migrado a wxWidgets)
- **Librerías**: zlib, libpng, libjpeg
- **Sistema**: Linux, macOS, Windows

Ver [`docs/BUILDING.md`](./docs/BUILDING.md) para detalles completos.

---

## 📊 Estado del Proyecto

| Aspecto | Estado | Detalles |
|---------|--------|----------|
| **Funcionalidad** | ✅ Estable | Totalmente funcional |
| **Qt 5.15** | 🔄 Siendo reemplazado | → wxWidgets (análisis listo) |
| **BOOST libs** | 🔄 Siendo eliminadas | → C++23 (análisis listo) |
| **C++23** | 🔄 En preparación | rama `upgrade` |
| **Testing** | ✅ Disponible | Google Test |
| **Documentación** | ✅ Completa | 18+ archivos markdown |

---

## 🔄 Modernización en Curso

Se está preparando la transición a tecnologías modernas:

- ✅ **Análisis completado**: Dónde está Qt, dónde está BOOST, alternativas C++23
- ✅ **Documentación**: Guías completas de migración
- ✅ **Scripts**: Automatización para Phase 1 de BOOST
- 🔄 **Próximo**: Implementación de migraciones
- 📋 **Plan**: 4-6 semanas de trabajo (2-3 developers)

**Detalles**: Ver [`docs/IMPLEMENTATION_PLAN.md`](./docs/IMPLEMENTATION_PLAN.md)

---

## 📖 Documentación por audiencia

### 👶 **Desarrollador nuevo**
→ Lee en este orden:
1. [`docs/README.md`](./docs/README.md) - Qué es
2. [`docs/DEVELOPER.md`](./docs/DEVELOPER.md) - Cómo desarrollar
3. [`docs/ARCHITECTURE.md`](./docs/ARCHITECTURE.md) - Cómo funciona

**Tiempo**: ~55 minutos

### 🏗️ **Arquitecto / Tech Lead**
→ Lee en este orden:
1. [`docs/ARCHITECTURE.md`](./docs/ARCHITECTURE.md) - Diseño general
2. [`docs/PROJECT_STRUCTURE.md`](./docs/PROJECT_STRUCTURE.md) - Estructura
3. [`docs/qt/QT_SYSTEMS.md`](./docs/qt/QT_SYSTEMS.md) - Sistemas Qt (si migras)
4. [`docs/IMPLEMENTATION_PLAN.md`](./docs/IMPLEMENTATION_PLAN.md) - Timeline

**Tiempo**: ~2-3 horas

### 👔 **CTO / Product Manager**
→ Lee en este orden:
1. Este README (5 min)
2. [`docs/IMPLEMENTATION_PLAN.md`](./docs/IMPLEMENTATION_PLAN.md) (30 min)
3. [`docs/qt/QT_ANALYSIS.md`](./docs/qt/QT_ANALYSIS.md) - Resumen ejecutivo (15 min)

**Tiempo**: ~50 minutos para decisión

---

## 🤝 Contribuciones

1. Consulta [`docs/DEVELOPER.md`](./docs/DEVELOPER.md)
2. Crea rama `feature/mi-feature`
3. Haz cambios y tests
4. Abre PR

---

## 📄 Licencia

GNU General Public License v3.0 - Ver `COPYING`

---

## 📞 Soporte

- **¿Dónde empiezo?** → [`docs/INDEX.md`](./docs/INDEX.md)
- **Preguntas técnicas** → Consulta la carpeta [`docs/`](./docs/)
- **Bugs/Features** → Abre una issue en GitHub

---

**Rama actual**: `upgrade` (C++23 + wxWidgets)  
**Última actualización**: 1 de febrero de 2026
