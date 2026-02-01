# 🎨 Qt → wxWidgets Migration Documentation

Documentación completa sobre Qt en ScanTailor y su migración a wxWidgets + C++23.

---

## 📚 Documentos en esta carpeta

### 1. **QT_ANALYSIS.md** ⭐ COMIENZA AQUÍ
Análisis exhaustivo de Qt en el proyecto:
- Resumen ejecutivo y métricas
- Dónde está Qt en el código
- Clases Qt principales usadas
- Patrones Qt en el código
- Acoplamiento Qt-Lógica
- Análisis por componente
- Flujo de datos con Qt
- Problemas actuales de Qt
- Por qué wxWidgets
- Equivalencias wxWidgets

**Lee esto si**: Quieres entender qué es Qt en el proyecto

---

### 2. **QT_SYSTEMS.md**
Análisis de los 5 sistemas principales que usan Qt:

1. **Visualización de imágenes (ImageView)** - CRÍTICO
   - Métodos principales
   - Acoplamiento detallado
   - Equivalente en wxWidgets
   - Esfuerzo: 1-2 días

2. **Interfaz principal (MainWindow + diálogos)** - CRÍTICO
   - MainWindow detallado
   - Diálogos principales
   - Acoplamiento detallado
   - Equivalente en wxWidgets
   - Esfuerzo: 1-2 días

3. **Interacción del usuario** - CRÍTICO
   - Tipos de interacción
   - Código típico
   - Equivalente en wxWidgets
   - Esfuerzo: ½-1 día

4. **Threading (QThread)** - IMPORTANTE
   - Patrón actual
   - Equivalente en wxWidgets
   - Esfuerzo: 1 día

5. **Sistema de preferencias** - MENOR
   - Configuración actual
   - Equivalente en wxWidgets
   - Esfuerzo: ½ día

**Resumen**: Total 4-6.5 días de trabajo

**Lee esto si**: Planeas hacer la migración o quieres entender el esfuerzo

---

### 3. **WXWIDGETS_MIGRATION.md** (En construcción)
Guía práctica de migración:
- Instalación de wxWidgets
- Conversión de código paso a paso
- Patrones de migration
- Troubleshooting
- Verificación post-migración

**Lee esto si**: Estás haciendo la migración

---

### 4. **REFACTORING_EXAMPLES.md** (Disponible en raíz)
Ejemplos reales de refactoring:
- ImageView: Qt → wxWidgets
- MainWindow: Qt → wxWidgets
- Diálogos: Qt → wxWidgets
- Event handling
- Threading

**Lee esto si**: Necesitas ejemplos de código concretos

---

### 5. **IMPLEMENTATION_PLAN.md** (Disponible en raíz)
Plan de ejecución de 9 semanas:
- Timeline detallado
- Tareas por semana
- Recursos requeridos
- Risk management
- Success criteria

**Lee esto si**: Necesitas planificar la migración

---

## 🎯 Ruta de lectura recomendada

### Para entender el estado actual (1 hora)
1. QT_ANALYSIS.md (toda) - 30 min
2. QT_SYSTEMS.md (resumen) - 15 min
3. Este README - 15 min

### Para planificar migración (3-4 horas)
1. QT_ANALYSIS.md (toda) - 30 min
2. QT_SYSTEMS.md (toda) - 90 min
3. IMPLEMENTATION_PLAN.md - 60 min
4. REFACTORING_EXAMPLES.md - 30-60 min

### Para ejecutar migración (4-6.5 días)
1. Leer WXWIDGETS_MIGRATION.md (cuando esté)
2. Seguir IMPLEMENTATION_PLAN.md semana a semana
3. Consultar REFACTORING_EXAMPLES.md según sea necesario
4. Usar QT_SYSTEMS.md para entender complejidad de cada módulo

---

## 📊 Métricas clave

```
Ubicación:           src/ui/ (y parcialmente en interaction/)
Archivos afectados:  ~150 archivos
Líneas de código:    ~8,000-10,000 LoC Qt
Dependencia:         Crítica para GUI, no para lógica
Acoplamiento:        BAJO fuera de ui/
Complejidad:         MEDIA (interfaces, eventos, threading)
Esfuerzo:            2-3 semanas (1 developer experimentado)
Viabilidad:          ✅ 90%+ con wxWidgets
Urgencia:            MEDIA (Qt legacy pero funciona)
```

---

## 🔄 Decisiones arquitectónicas

### ✅ BUENO: Separación clara

**Lógica (SIN Qt)**:
- `src/filters/` - Procesamiento de imágenes
- `src/imageproc/` - Manipulación de píxeles
- `src/math/` - Utilidades matemáticas
- `src/dewarping/` - Corrección de perspectiva
- `src/zones/` - Gestión de regiones
- `src/foundation/` - Clases base

**UI (CON Qt)**:
- `src/ui/` - Todo sobre Qt
- `src/interaction/` - Parcialmente

### 🎯 Ventaja: Migración aislada

Cambiar Qt a wxWidgets **no afecta** la lógica de procesamiento.
Solo hay que modificar `src/ui/` e `src/interaction/` parcialmente.

---

## 🚀 Siguiente paso

**Para desarrolladores nuevos**:
→ Lee [`QT_ANALYSIS.md`](./QT_ANALYSIS.md)

**Para arquitectos**:
→ Lee [`QT_SYSTEMS.md`](./QT_SYSTEMS.md)

**Para ejecutar migración**:
→ Lee [`IMPLEMENTATION_PLAN.md`](../IMPLEMENTATION_PLAN.md) (en root)

**Para ejemplos de código**:
→ Lee [`REFACTORING_EXAMPLES.md`](../REFACTORING_EXAMPLES.md) (en root)

---

## 📝 Estado actual

| Documento | Estado | Completado |
|-----------|--------|-----------|
| QT_ANALYSIS.md | ✅ COMPLETO | 100% |
| QT_SYSTEMS.md | ✅ COMPLETO | 100% |
| WXWIDGETS_MIGRATION.md | 🔄 En construcción | 30% |
| REFACTORING_EXAMPLES.md | ✅ En root | 100% |
| IMPLEMENTATION_PLAN.md | ✅ En root | 100% |

---

**Última actualización**: 1 de febrero de 2026  
**Rama**: `upgrade` (preparación para wxWidgets)
**Autor**: Análisis automático de codebase
