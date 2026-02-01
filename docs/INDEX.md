# 📚 Documentación ScanTailor

Bienvenido a la documentación del proyecto ScanTailor. Esta carpeta contiene toda la información necesaria para entender, compilar, desarrollar y migrar el proyecto.

---

## 🗂️ Estructura de documentación

```
docs/
├── README.md (ESTE ARCHIVO)         Guía de navegación general
├── PROJECT_STRUCTURE.md              Estructura de carpetas del proyecto
├── DEVELOPER.md                      Guía para desarrolladores
├── ARCHITECTURE.md                   Descripción arquitectónica
├── BUILDING.md                       Guía de compilación detallada
├── CONTRIBUTING.md                   Cómo contribuir (si existe)
│
├── qt/                               📘 Qt5 → wxWidgets Migration
│   ├── README.md                     Guía de navegación Qt
│   ├── QT_ANALYSIS.md                Análisis exhaustivo de Qt
│   ├── QT_SYSTEMS.md                 Los 5 sistemas Qt principales
│   ├── WXWIDGETS_MIGRATION.md        Guía práctica de migración (en construcción)
│   ├── REFACTORING_EXAMPLES.md       Ejemplos de código (en root)
│   └── IMPLEMENTATION_PLAN.md        Plan de 9 semanas (en root)
│
└── boost/                            📦 BOOST → C++23 Elimination
    ├── README.md                     Guía de navegación BOOST
    ├── BOOST_ANALYSIS.md             Análisis exhaustivo de BOOST
    ├── BOOST_TO_CPP23.md             Guía técnica (en root)
    ├── BOOST_MIGRATION_GUIDE.md      Guía práctica (en root)
    ├── BOOST_EXAMPLES.md             Ejemplos de código (en root)
    └── migrate_boost_phase1.sh       Script automatizado (ejecutable)
```

---

## 🎯 ¿Por dónde empezar?

### 👶 Si acabas de clonar el proyecto
1. **Léeme 5 minutos**: Lee secciones "Quick Start" de este README
2. **Compila 15 minutos**: Sigue [`BUILDING.md`](./BUILDING.md)
3. **Entiende 30 minutos**: Lee [`DEVELOPER.md`](./DEVELOPER.md)
4. **Explora 30 minutos**: Abre el código en `src/` y navega

**Total**: ~80 minutos para empezar a desarrollar

---

### 🏗️ Si vas a agregar funcionalidad
1. **Lee**: [`ARCHITECTURE.md`](./ARCHITECTURE.md) (30 min)
2. **Lee**: [`DEVELOPER.md`](./DEVELOPER.md) (20 min)
3. **Consulta**: [`PROJECT_STRUCTURE.md`](./PROJECT_STRUCTURE.md) para saber dónde
4. **Código**: Comienza a escribir

**Total**: ~50 minutos + tiempo de desarrollo

---

### 🔄 Si vas a migrar de Qt a wxWidgets
1. **Lee**: [`qt/README.md`](./qt/README.md) (15 min)
2. **Lee**: [`qt/QT_ANALYSIS.md`](./qt/QT_ANALYSIS.md) (30 min)
3. **Lee**: [`qt/QT_SYSTEMS.md`](./qt/QT_SYSTEMS.md) (90 min)
4. **Lee**: [`IMPLEMENTATION_PLAN.md`](./IMPLEMENTATION_PLAN.md) - Qt section (60 min)
5. **Consulta**: [`qt/REFACTORING_EXAMPLES.md`](./REFACTORING_EXAMPLES.md) mientras codeas

**Total**: ~4 horas de lectura + 2-3 semanas de implementación

---

### 📦 Si vas a eliminar BOOST y migrar a C++23
1. **Lee**: [`boost/README.md`](./boost/README.md) (15 min)
2. **Lee**: [`boost/BOOST_ANALYSIS.md`](./boost/BOOST_ANALYSIS.md) (30 min)
3. **Decide**: ¿Usar script automático o manual?
4. **Ejecuta**: [`boost/migrate_boost_phase1.sh`](./boost/migrate_boost_phase1.sh) (1-2 horas automáticas)
5. **Continúa**: Con [`BOOST_MIGRATION_GUIDE.md`](./BOOST_MIGRATION_GUIDE.md) para Phases 2-3

**Total**: ~45 minutos lectura + 2-4 semanas implementación

---

### 👔 Si eres CTO/PM y necesitas decidir sobre modernización
1. **Lee**: Este README (5 min)
2. **Lee**: Resúmenes ejecutivos en:
   - [`qt/QT_ANALYSIS.md`](./qt/QT_ANALYSIS.md) - Resumen ejecutivo
   - [`boost/BOOST_ANALYSIS.md`](./boost/BOOST_ANALYSIS.md) - Resumen ejecutivo
3. **Lee**: [`IMPLEMENTATION_PLAN.md`](./IMPLEMENTATION_PLAN.md) (completo)
4. **Decide**: Timeline, recursos, budget

**Total**: ~2-3 horas para decisión informada

---

## 📖 Documentos principales explicados

### 1. **Documentación General del Proyecto** (en `docs/`)

| Archivo | Qué es | Leer si |
|---------|--------|---------|
| **README.md** | Este archivo - Guía general | Acabas de clonar |
| **PROJECT_STRUCTURE.md** | Estructura de carpetas y archivos | Quieres navegar el código |
| **DEVELOPER.md** | Cómo crear código nuevo | Vas a escribir funcionalidad |
| **ARCHITECTURE.md** | Cómo funciona todo | Quieres entender el diseño |
| **BUILDING.md** | Cómo compilar en detalle | Tienes problemas de compilación |

### 2. **Documentación Qt → wxWidgets** (en `docs/qt/`)

| Archivo | Qué es | Leer si |
|---------|--------|---------|
| **qt/README.md** | Índice de documentación Qt | Es tu primera vez en Qt |
| **qt/QT_ANALYSIS.md** | ¿Dónde está Qt? ¿Qué hace? | Quieres entender Qt |
| **qt/QT_SYSTEMS.md** | 5 sistemas Qt y costo de migración | Planeas migrar |
| **qt/REFACTORING_EXAMPLES.md** | Ejemplos código antes/después | Necesitas código concreto |
| **qt/IMPLEMENTATION_PLAN.md** | Plan de 9 semanas | Ejecutarás la migración |

### 3. **Documentación BOOST → C++23** (en `docs/boost/`)

| Archivo | Qué es | Leer si |
|---------|--------|---------|
| **boost/README.md** | Índice de documentación BOOST | Es tu primera vez con BOOST |
| **boost/BOOST_ANALYSIS.md** | ¿Dónde está BOOST? ¿Qué hace? | Quieres entender BOOST |
| **boost/BOOST_TO_CPP23.md** | Cómo migrar cada librería | Necesitas alternativas |
| **boost/BOOST_EXAMPLES.md** | Ejemplos código antes/después | Necesitas código concreto |
| **boost/BOOST_MIGRATION_GUIDE.md** | Guía paso a paso | Ejecutarás la migración |
| **boost/migrate_boost_phase1.sh** | Script automatizado | Quieres automatizar 60% |

---

## 📊 Proyecto ScanTailor - Información clave

### ¿Qué es?
Escáner de documentos avanzado que automatiza tareas de post-procesamiento:
- Detección de página
- Corrección de perspectiva
- Deskew (enderezamiento)
- Binarización
- Exportación a PDF/TIFF/PNG

### ¿En qué lenguaje?
- **C++20** (actual)
- **C++23** (rama `upgrade`)
- **Qt 5.15+** (actual, migrando a wxWidgets)
- **BOOST** (siendo eliminado)

### ¿Cómo es la arquitectura?
```
UI Layer (Qt/wxWidgets)
    ↓
Filter Pipeline (independiente de UI)
    ↓
Image Processing (bajo nivel, sin dependencias)
    ↓
Math & Utilities (puro C++)
```

### Características clave
✅ Modular y extensible  
✅ Lógica separada de UI  
✅ Procesamiento en tiempo real  
✅ Multiplataforma (Windows, Linux, macOS)  
✅ Documentado  

---

## 🔗 Enlaces rápidos por tarea

### Quiero...

- **Compilar el proyecto** → [`BUILDING.md`](./BUILDING.md)
- **Agregar un nuevo filtro** → [`DEVELOPER.md`](./DEVELOPER.md) + [`ARCHITECTURE.md`](./ARCHITECTURE.md)
- **Entender cómo funciona todo** → [`ARCHITECTURE.md`](./ARCHITECTURE.md)
- **Contribuir al código** → [`CONTRIBUTING.md`](./CONTRIBUTING.md) (si existe)
- **Migrar de Qt a wxWidgets** → [`qt/README.md`](./qt/README.md)
- **Eliminar BOOST y usar C++23** → [`boost/README.md`](./boost/README.md)
- **Planificar la modernización** → [`IMPLEMENTATION_PLAN.md`](./IMPLEMENTATION_PLAN.md)
- **Ver ejemplos de refactoring** → [`qt/REFACTORING_EXAMPLES.md`](./REFACTORING_EXAMPLES.md) o [`boost/BOOST_EXAMPLES.md`](./BOOST_EXAMPLES.md)

---

## 🚀 Quick Start (5 minutos)

### Compilar

```bash
git clone https://github.com/Di4l/scantailor.git
cd scantailor
git checkout upgrade
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
./bin/scantailor
```

### Crear rama de desarrollo

```bash
git checkout -b feature/mi-funcionalidad
```

### Editar código

```bash
# Abre cualquier archivo en src/
code src/filters/mi_filtro/MiFiltro.cpp
```

### Compilar cambios

```bash
cd build
make -j$(nproc)
./bin/scantailor  # Test
```

---

## 📋 Checklist para desarrolladores nuevos

- [ ] Cloné el repositorio (`git clone ...`)
- [ ] Seleccioné rama `upgrade` (`git checkout upgrade`)
- [ ] Compilé exitosamente (`make -j$(nproc)`)
- [ ] La aplicación ejecuta (`./bin/scantailor`)
- [ ] Leí [`DEVELOPER.md`](./DEVELOPER.md)
- [ ] Leí [`ARCHITECTURE.md`](./ARCHITECTURE.md)
- [ ] Entiendo dónde agregar código (ver [`PROJECT_STRUCTURE.md`](./PROJECT_STRUCTURE.md))
- [ ] Creé mi rama (`git checkout -b feature/...`)
- [ ] Estoy listo para escribir código

---

## 📞 ¿Preguntas?

1. **¿Dónde está X?** → [`PROJECT_STRUCTURE.md`](./PROJECT_STRUCTURE.md)
2. **¿Cómo hago Y?** → [`DEVELOPER.md`](./DEVELOPER.md)
3. **¿Por qué el código es así?** → [`ARCHITECTURE.md`](./ARCHITECTURE.md)
4. **¿Cómo compilo?** → [`BUILDING.md`](./BUILDING.md)
5. **¿Cómo migro Qt?** → [`qt/README.md`](./qt/README.md)
6. **¿Cómo elimino BOOST?** → [`boost/README.md`](./boost/README.md)

---

## 📚 Resumen de documentación

| Categoría | Archivos | Total LoC |
|-----------|----------|-----------|
| **Documentación general** | 6 archivos | ~5,000 líneas |
| **Documentación Qt** | 5 archivos | ~7,000 líneas |
| **Documentación BOOST** | 6 archivos | ~6,000 líneas |
| **Scripts** | 1 ejecutable | ~270 líneas |
| **TOTAL** | 18 archivos | ~18,000 líneas |

---

## ✅ Estado de la documentación

```
✅ README.md (GENERAL)                  100% Completo
✅ PROJECT_STRUCTURE.md                 100% Completo
✅ DEVELOPER.md                         100% Completo
✅ ARCHITECTURE.md                      100% Completo
✅ BUILDING.md                          100% Completo
✅ qt/README.md                         100% Completo
✅ qt/QT_ANALYSIS.md                    100% Completo
✅ qt/QT_SYSTEMS.md                     100% Completo
🔄 qt/WXWIDGETS_MIGRATION.md           30% En construcción
✅ boost/README.md                      100% Completo
✅ boost/BOOST_ANALYSIS.md              100% Completo
✅ migrate_boost_phase1.sh              100% Funcional
```

---

**Última actualización**: 1 de febrero de 2026  
**Rama**: `upgrade` (C++23 + wxWidgets)  
**Versión**: 2.0 - Documentación reorganizada por categoría
