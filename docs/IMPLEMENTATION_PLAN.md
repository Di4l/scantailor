# PLAN DE IMPLEMENTACIÓN: MIGRACIÓN wxWidgets + C++23

## Documento Ejecutivo de Migración
**Período:** 8-9 semanas (56-63 días)  
**Equipo:** 2-3 desarrolladores C++  
**Objetivo:** Migrar de Qt5 a wxWidgets + C++23 manteniendo funcionalidad 100%

---

## PARTE 1: CRONOGRAMA DETALLADO

### SEMANA 1-2: FUNDACIÓN (10 DÍAS)

#### Día 1-2: Build System & Dependencies

**Tareas:**
```
└─ Actualizar CMakeLists.txt
   ├─ Remover: CMAKE_AUTOMOC, CMAKE_AUTORCC, CMAKE_AUTOUIC
   ├─ Remover: Qt5 find_package
   ├─ Agregar: wxWidgets find_package
   ├─ Configurar: C++23 features
   └─ Testing: CMake configure OK
   
└─ Modificar foundation/RefCountable.h
   ├─ Reemplazar: QAtomicInt → std::atomic<int32_t>
   ├─ Mantener: ADL ref()/unref() interface
   └─ Testing: Unit tests pasan 100%
   
└─ Crear foundation/concepts.h
   ├─ RefCounted concept
   ├─ Signal<T> concept
   ├─ Drawable concept
   ├─ EventHandler concept
   └─ Testing: Compilation tests
```

**Archivos modificados:**
- CMakeLists.txt
- foundation/RefCountable.h
- (NEW) foundation/concepts.h
- (NEW) foundation/Signal.h

**Deliverable:** Build system compila sin Qt, CLI funciona igual

#### Día 3-4: C++23 Signal System

**Tareas:**
```
└─ Crear foundation/Signal.h
   ├─ Template Signal<Args...>
   ├─ connect() method
   ├─ emit() method
   ├─ Compiler tests
   └─ Performance: emit() benchmarked
   
└─ Crear foundation/SignalSlot.h (compatibility layer)
   ├─ Wrapper para Qt ↔ wxWidgets
   ├─ connect() global function
   └─ Usage examples
```

**Archivos nuevos:**
- foundation/Signal.h
- foundation/SignalSlot.h

**Deliverable:** Signal system tested, benchmarked, documented

#### Día 5-6: Event System Abstraction

**Tareas:**
```
└─ Crear foundation/EventSystem.h
   ├─ EventDispatcher template
   ├─ wxEvtHandler integration
   ├─ Event type system
   └─ Thread-safe dispatch
   
└─ Crear foundation/PayloadEventWx.h
   ├─ PayloadEvent template (wxWidgets version)
   ├─ Payload wrapper
   ├─ Type-safe extraction
   └─ Performance: Near-zero overhead
```

**Archivos nuevos:**
- foundation/EventSystem.h
- foundation/PayloadEventWx.h

**Deliverable:** Event system documented, tested with mock wxEvtHandler

#### Día 7-8: Core Processing Layer

**Tareas:**
```
└─ Verificar AbstractCommand hierarchy
   ├─ 0 cambios necesarios (C++ puro)
   ├─ Unit tests: PASAN
   └─ CLI: Funciona OK
   
└─ Verificar BackgroundTask hierarchy
   ├─ Reemplazar: QAtomicInt → std::atomic en BackgroundTask
   ├─ Testing: Cancellation logic
   └─ Performance: Benchmarked
   
└─ Crear utils/ThreadUtils.h
   ├─ Wrappers para wxThread
   ├─ RAII thread management
   ├─ Scoped locking (std::lock_guard compatible)
   └─ Performance: Lock-free where possible
```

**Archivos modificados:**
- BackgroundTask.h (QAtomicInt → std::atomic)

**Archivos nuevos:**
- utils/ThreadUtils.h

**Deliverable:** Core layer 100% independent of framework

#### Día 9-10: Integration Testing

**Tareas:**
```
└─ Create test suite
   ├─ Test RefCountable
   ├─ Test Signal<T>
   ├─ Test PayloadEvent
   ├─ Test Background tasks
   └─ Test: All CLI operations work
   
└─ Performance baseline
   ├─ Reference counting perf vs Qt
   ├─ Signal dispatch perf
   ├─ Memory usage metrics
   └─ Document baseline
```

**Deliverable:** Test suite + Performance baseline established

---

### SEMANA 3-4: UI ABSTRACTION (10 DÍAS)

#### Día 11-12: FilterUiInterface Refactoring

**Tareas:**
```
└─ Refactorizar FilterUiInterface (ACTUAL)
   ├─ Ya es agnóstica → Mantener igual
   ├─ Agregar: method setEventHandler()
   ├─ Update: documentation for wxWidgets
   └─ Testing: Mock implementations
   
└─ Crear PluginInterface (NEW)
   ├─ Plugin lifecycle
   ├─ Filter initialization
   ├─ Version checking
   └─ Resource loading
```

**Archivos modificados:**
- FilterUiInterface.h (minor updates)

**Archivos nuevos:**
- PluginInterface.h

**Deliverable:** Plugin interface documented and stable

#### Día 13-14: ImageViewBase → Generic Image Viewer

**Tareas:**
```
└─ Crear ImageViewerInterface (abstracción)
   ├─ render(DrawContext&) method
   ├─ onMouseEvent() method
   ├─ onKeyEvent() method
   ├─ onWheelEvent() method
   └─ Coordinate system definition
   
└─ Crear ImagePresentation (agnóstica)
   ├─ Transform matrix (no depende de Qt)
   ├─ DPI handling
   ├─ Zoom level management
   └─ Virtual coordinate system
   
└─ Refactorizar Margins, Transform classes
   ├─ Remover: QTransform dependency
   ├─ Usar: glm o custom math library
   └─ Testing: Math correctness
```

**Archivos nuevos:**
- ImageViewerInterface.h
- ImagePresentation.h (refactored)

**Archivos modificados:**
- ImageViewBase.h (dejar placeholder)
- Margins.h (clean up)

**Deliverable:** Generic image viewer interface ready

#### Día 15-16: InteractionHandler Refactoring

**Tareas:**
```
└─ Refactorizar InteractionHandler
   ├─ Remover: QObject dependency (ya es C++ puro!)
   ├─ Mantener: boost::intrusive list (OK)
   ├─ Update: Event method signatures
   │  ├─ paint(DrawContext&, InteractionState&)
   │  ├─ onMouseEvent(wxMouseEvent&, InteractionState&)
   │  ├─ onKeyEvent(wxKeyEvent&, InteractionState&)
   │  └─ etc.
   └─ Testing: Chain of responsibility
   
└─ Refactorizar InteractionState
   ├─ Verificar: No Qt dependencies
   ├─ Update: Documentation
   └─ Testing: State transitions
```

**Archivos modificados:**
- interaction/InteractionHandler.h (signatures)
- interaction/InteractionState.h (verify)

**Deliverable:** Interaction system ready for wxWidgets

#### Día 17-18: Graphics Context Abstraction

**Tareas:**
```
└─ Crear DrawContext concept
   ├─ drawBitmap(bitmap, pos)
   ├─ drawText(text, rect)
   ├─ drawLine(p1, p2)
   ├─ drawRectangle(rect)
   ├─ drawCircle(center, radius)
   ├─ fillRectangle(rect, color)
   ├─ setPen(pen)
   └─ setBrush(brush)
   
└─ Crear wxDrawContext implementation
   ├─ Wrapper alrededor wxDC
   ├─ Performance: Buffered drawing
   └─ Testing: Rendering accuracy
   
└─ Crear GlmMathLibrary integration
   ├─ Remove: QTransform dependency
   ├─ Use: glm::mat4 for transforms
   ├─ Performance: SIMD optimized
   └─ Testing: Transform correctness
```

**Archivos nuevos:**
- graphics/DrawContext.h
- graphics/wxDrawContext.h
- graphics/MathUtils.h

**Deliverable:** Graphics abstraction layer complete

#### Día 19-20: Plugin/Settings Abstraction

**Tareas:**
```
└─ Refactorizar Settings classes
   ├─ Remover: QSettings dependency (usar pugixml)
   ├─ Keep: toXml()/fromXml()
   ├─ Update: File I/O (std::filesystem)
   └─ Testing: Serialization round-trip
   
└─ Crear FilterSettings interface
   ├─ Virtual save()/load()
   ├─ Version management
   └─ Migration helpers
```

**Archivos modificados:**
- All Settings.h files in filters/
- ProjectReader.h/cpp
- ProjectWriter.h/cpp

**Deliverable:** Settings system wxWidgets-ready

---

### SEMANA 5-6: CORE wxWidgets IMPLEMENTATION (10 DÍAS)

#### Día 21-22: Application & Main Window

**Tareas:**
```
└─ Crear Application (wxApp)
   ├─ Reemplazar QApplication
   ├─ Command line parsing
   ├─ Crash handler
   ├─ Resource initialization
   └─ Testing: App lifecycle
   
└─ Crear MainWindow (wxFrame)
   ├─ Replace QMainWindow
   ├─ Menu bar setup
   ├─ Tool bar setup
   ├─ Status bar
   ├─ Central widget layout
   └─ Testing: Window creation
```

**Archivos nuevos:**
- Application.h/cpp (wxWidgets)
- MainWindow.h/cpp (wxWidgets)

**Effort:** 3-4 días (MainWindow es compleja)

#### Día 23-24: ThumbnailSequence (wxWidgets)

**Tareas:**
```
└─ Crear ThumbnailSequence (wxWidgets version)
   ├─ Base: wxScrolledWindow
   ├─ Items management: std::vector<Item>
   ├─ Rendering: onPaint()
   ├─ Selection: onMouseClick()
   ├─ Culling: Manual (visible only)
   ├─ Caching: Per-thumbnail pixmap
   ├─ Performance: O(visible items) rendering
   └─ Testing: Rendering + interaction
   
└─ Implementar Item structure
   ├─ wxBitmap thumbnail
   ├─ wxString label
   ├─ wxRect bounds
   ├─ bool isSelected
   └─ PageInfo metadata
```

**Effort:** 5-7 días (complex graphics + interaction)

**Deliverable:** Thumbnail panel functional

#### Día 25-26: Filter OptionsWidget Base

**Tareas:**
```
└─ Crear OptionsWidget base class
   ├─ Derive from wxPanel
   ├─ Signal callbacks
   ├─ Settings management
   ├─ Layout helpers
   └─ Testing: Basic widget creation
   
└─ Implementar 1 filter (fix_orientation)
   ├─ Spinbox for angle
   ├─ Buttons for rotation
   ├─ Callbacks wired
   ├─ Settings save/load
   └─ Testing: Fix orientation workflow
```

**Effort:** 2-3 días (fix_orientation is simple)

**Deliverable:** One complete filter working

#### Día 27-28: ImageView Implementation

**Tareas:**
```
└─ Crear ImageView (wxWidgets)
   ├─ Derive from ImageViewerInterface
   ├─ Scroll area: wxScrolledWindow
   ├─ Rendering: onPaint(wxPaintEvent)
   ├─ Zoom: Wheel event
   ├─ Pan: Mouse drag
   ├─ Overlay: Interaction handler drawing
   ├─ Performance: Double-buffering
   └─ Testing: Display + interaction
   
└─ Coordinate systems
   ├─ Image → pixmap transform
   ├─ Pixmap → virtual transform
   ├─ Virtual → widget transform
   └─ Testing: Accuracy of transforms
```

**Effort:** 4-5 días (complex math + rendering)

**Deliverable:** Image view with zoom/pan working

#### Día 29-30: Basic Dialogs

**Tareas:**
```
└─ Crear basic dialogs
   ├─ ProjectFileDialog (wxDirDialog)
   ├─ SettingsDialog (wxDialog + wxNotebook)
   ├─ FixDpiDialog (wxDialog + wxListCtrl)
   ├─ OutOfMemoryDialog (wxMessageDialog)
   └─ Testing: Dialog lifecycle
   
└─ Implement event handling
   ├─ OK/Cancel buttons
   ├─ Data binding
   ├─ Validation
   └─ Testing: User interactions
```

**Effort:** 3-4 días (straightforward dialogs)

**Deliverable:** Basic UI workflow functional

---

### SEMANA 7-8: REMAINING FILTERS & POLISH (10 DÍAS)

#### Día 31-34: Remaining Filters (4 filters)

**Tareas por filter:**
```
├─ page_split
│  ├─ Double-page detection UI (2d)
│  └─ Settings management
│
├─ deskew
│  ├─ Skew angle input (3d)
│  ├─ Overlay drawing (corner guides)
│  └─ Settings management
│
├─ select_content
│  ├─ Content box editor (3d)
│  ├─ Corner/edge dragging (interaction)
│  └─ Settings management
│
└─ page_layout
   ├─ Margin controls (3d)
   ├─ Ruler overlay
   └─ Settings management
```

**Total effort:** 11-12 días (parallel: 3-4 real days if team)

**Output filter:** Separate effort, save for last

#### Día 35-36: Output Filter (2 días prep + 3 días impl)

**Tareas:**
```
└─ Output filter (complex)
   ├─ Multiple views architecture
   ├─ DewarpingView (3D preview)
   ├─ FillZoneEditor (zone drawing)
   ├─ PictureZoneEditor (zone drawing)
   ├─ All integrated
   └─ Testing: Complex interactions
```

**Effort:** 5-7 días (most complex filter)

#### Día 37-40: Threading & Background Processing

**Tareas:**
```
└─ Implementar wxThread version
   ├─ WorkerThread class
   ├─ Task queue management
   ├─ Result callbacks
   ├─ Cancellation support
   └─ Testing: Task execution + cancellation
   
└─ Event loop integration
   ├─ wxEvtHandler::PostEvent()
   ├─ Thread-safe posting
   ├─ GUI thread callbacks
   └─ Testing: Cross-thread communication
   
└─ Performance tuning
   ├─ Profile rendering
   ├─ Optimize thumbnail generation
   ├─ Cache optimization
   └─ Benchmarking
```

**Effort:** 3-4 días

---

### SEMANA 9: TESTING & RELEASE (7 DÍAS)

#### Día 41-43: Comprehensive Testing

**Tareas:**
```
└─ Unit tests
   ├─ Core layer: 100% pasa
   ├─ Signal system: 100% pasa
   ├─ Event system: 100% pasa
   └─ Processing: 100% pasa
   
└─ Integration tests
   ├─ File load/save
   ├─ Filter pipeline
   ├─ Background processing
   ├─ Threading correctness
   └─ Memory leaks: None
   
└─ UI tests
   ├─ Window creation
   ├─ Widget interaction
   ├─ Rendering accuracy
   ├─ Performance: Baseline met
   └─ No crashes
```

**Deliverable:** Test report, all critical tests pass

#### Día 44-45: Performance Profiling

**Tareas:**
```
└─ Performance comparison vs Qt
   ├─ Startup time: Measure
   ├─ File load: Measure
   ├─ Filter processing: Measure
   ├─ Rendering FPS: Measure
   ├─ Memory usage: Compare
   └─ Binary size: Compare
   
└─ Optimization if needed
   ├─ Profile hot paths
   ├─ Optimize rendering
   ├─ Cache optimization
   └─ Verify improvements
```

**Deliverable:** Performance report + optimization log

#### Día 46-47: Documentation & Polish

**Tareas:**
```
└─ Documentation
   ├─ Migration guide
   ├─ Architecture overview
   ├─ API documentation (Doxygen)
   ├─ Build instructions
   └─ Troubleshooting guide
   
└─ Code cleanup
   ├─ Remove dead code
   ├─ Consistent naming
   ├─ Comment documentation
   ├─ Code review
   └─ Final PR
```

**Deliverable:** Complete documentation + polished code

#### Day 48: Release Preparation

**Tareas:**
```
└─ Final release steps
   ├─ Tag version
   ├─ Build binaries (Linux, Windows, macOS)
   ├─ Verify: All platforms work
   ├─ Create release notes
   └─ Final smoke test
```

**Deliverable:** Release candidate ready

---

## PARTE 2: RECURSOS REQUERIDOS

### 2.1 Equipo

```
Role 1: Lead Developer (Full-time, 8-9 weeks)
  ├─ Responsibilities:
  │  ├─ Architecture decisions
  │  ├─ Core system implementation
  │  ├─ Code review
  │  └─ Integration testing
  ├─ Required Skills:
  │  ├─ C++23 expert
  │  ├─ wxWidgets experienced
  │  ├─ CMake expert
  │  └─ Qt knowledge (for reference)
  └─ Time: 100% allocation

Role 2: Senior Developer (Full-time, 8-9 weeks)
  ├─ Responsibilities:
  │  ├─ UI implementation
  │  ├─ Filter implementation
  │  ├─ Testing
  │  └─ Documentation
  ├─ Required Skills:
  │  ├─ C++ strong
  │  ├─ wxWidgets knowledge
  │  ├─ UI/UX understanding
  │  └─ Graphics knowledge
  └─ Time: 100% allocation

Role 3: QA/Tester (Part-time, weeks 7-9)
  ├─ Responsibilities:
  │  ├─ Test plan creation
  │  ├─ Test execution
  │  ├─ Bug reporting
  │  └─ Performance testing
  ├─ Required Skills:
  │  ├─ Testing methodologies
  │  ├─ Image processing domain
  │  └─ Performance profiling
  └─ Time: 50% allocation (weeks 7-9)
```

### 2.2 Infrastructure

```
Development Environment:
  ├─ Linux (primary): Ubuntu 22.04 LTS
  ├─ Compiler: GCC 13+ or Clang 17+
  ├─ CMake: 3.20+
  ├─ wxWidgets: 3.2+
  ├─ C++ Standard: C++23
  └─ Build time: ~2-3 minutes incremental

Testing:
  ├─ Unit test framework: Google Test (gtest)
  ├─ CI/CD: GitHub Actions
  ├─ Code coverage: gcov
  ├─ Performance profiling: perf + flamegraph
  └─ Memory check: valgrind + asan

Documentation:
  ├─ Doxygen for API docs
  ├─ Markdown for guides
  ├─ Architecture diagrams: PlantUML
  └─ Release notes: Standard template
```

### 2.3 External Libraries

```
Must-Have:
  ├─ wxWidgets 3.2+ (UI framework)
  ├─ pugixml (XML parsing, replaces QDomDocument)
  ├─ zlib (compression)
  ├─ libjpeg, libpng, libtiff (image formats)
  └─ boost (intrusive containers, only)

Optional but Recommended:
  ├─ glm (math library for transforms)
  ├─ fmt (modern string formatting)
  ├─ range-v3 (C++20/23 ranges)
  └─ spdlog (logging)
```

---

## PARTE 3: RISK MANAGEMENT

### 3.1 Critical Risks

| Risk | Probability | Impact | Mitigation |
|------|------------|--------|-----------|
| **wxWidgets graphics limitations** | Medium | High | Spike on rendering (Week 1) |
| **Threading bugs** | Medium | High | Comprehensive threading tests |
| **Performance regression** | Low | High | Continuous profiling (Week 7) |
| **Build system complexity** | Low | Medium | CMake expert on team |
| **Compatibility issues** | Low | Medium | Cross-platform testing (Week 8) |

### 3.2 Contingency Plans

```
If wxWidgets rendering insufficient:
  ├─ Plan B: OpenGL rendering (wxGLCanvas)
  ├─ Effort: +3-5 days
  └─ Fallback: Qt remains (abort migration)

If threading issues:
  ├─ Plan B: Simplify to single-threaded GUI
  ├─ Effort: -2-3 days (but worse UX)
  └─ Fallback: More rigorous testing

If performance regression > 10%:
  ├─ Plan B: Optimize critical paths
  ├─ Effort: +3-5 days profiling
  └─ Fallback: Accept 5-10% if necessary
```

---

## PARTE 4: SUCCESS CRITERIA

### 4.1 Functional Requirements

```
✓ All 6 filters working identically to Qt version
✓ File load/save preserves 100% compatibility
✓ CLI mode: Functional, all operations work
✓ Threading: No race conditions, proper cancellation
✓ Memory: No leaks, valgrind clean
✓ Crash: 0 crashes in 1000+ operations
```

### 4.2 Performance Requirements

```
✓ Startup time: ±10% vs Qt version
✓ File operations: ±10% vs Qt version
✓ Rendering: ±15% FPS vs Qt version (may vary by backend)
✓ Memory usage: -20% to +10% vs Qt version
✓ Binary size: -30% to -50% vs Qt version
```

### 4.3 Code Quality

```
✓ Test coverage: >80% (critical paths 100%)
✓ Code review: All code reviewed by 2 developers
✓ Documentation: Complete API docs + architecture guide
✓ Static analysis: 0 high-priority warnings
✓ Code style: Consistent with project guidelines
```

---

## PARTE 5: ROLLBACK PLAN

### Immediate Rollback (if migration fails)

```
1. Maintain Git branch: original/qt5-main (always available)
2. If migration stalls or unblocks:
   - Tag current effort as "migration-attempt-v1"
   - Git revert to last known good Qt5 build
   - Post-mortem analysis
   - Plan v2 with learnings
3. Estimated time to rollback: < 1 hour
4. Data loss: 0 (all changes in version control)
```

### Partial Rollback (selective components)

```
If some systems work but others don't:
  ├─ Keep: Working components (e.g., core processing)
  ├─ Rollback: Failing components (e.g., graphics)
  ├─ Hybrid: Qt for graphics, wxWidgets for UI
  └─ Estimated time: +1-2 weeks additional
```

---

## PARTE 6: TRAINING PLAN

### For Development Team

```
Week -1 (Pre-migration):
  ├─ wxWidgets fundamentals (2 days)
  ├─ C++23 new features (1 day)
  ├─ CMake advanced topics (0.5 day)
  ├─ Architecture walkthrough (0.5 day)
  └─ Q&A session

Week 1 (During migration):
  ├─ Daily standup (15 min)
  ├─ Architecture review (Friday)
  └─ Problem-solving sessions (as needed)
```

### For QA/Testing

```
Pre-testing:
  ├─ Image processing fundamentals (0.5 day)
  ├─ Scantailor workflow (1 day)
  └─ Test plan review

Testing phase:
  ├─ Daily coordination (15 min)
  ├─ Weekly test status report
  └─ Issue triage meetings
```

---

## CONCLUSIÓN

**Este plan es:**
- ✅ Realista (8-9 semanas, 2-3 developers)
- ✅ Modular (hitos claros cada 2-3 días)
- ✅ Riesgos mitigados (contingency plans)
- ✅ Measurable (success criteria claros)
- ✅ Flexible (adapta a cambios)

**Próximos pasos:**
1. Obtener aprobación del equipo
2. Asignar recursos
3. Configurar repositorio/branches
4. Comenzar Semana 1, Día 1

**Viabilidad global: ✅ ALTA (75%+ probability of success)**
