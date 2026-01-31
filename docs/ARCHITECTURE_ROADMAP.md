# 📐 Architecture & Modernization Roadmap

## Current Stack → Proposed Stack

```
┌─────────────────────────────────────┐
│      CURRENT STATE (Qt5 Era)        │
├─────────────────────────────────────┤
│                                     │
│  Framework:     Qt5 (Core, Gui)     │
│  UI Widgets:    QMainWindow         │
│  Graphics:      QGraphicsView       │
│  Threading:     QThread, QMutex     │
│  Signals:       MOC + Q_OBJECT      │
│  Memory:        QAtomicInt          │
│  C++ Standard:  C++20               │
│  Dependencies:  Boost (eliminated)  │
│                                     │
│  Status: ⚠️  Aging, MOC overhead    │
│                                     │
└─────────────────────────────────────┘
              │
              │  MIGRATION
              ▼
┌─────────────────────────────────────┐
│     PROPOSED STATE (Modern Era)     │
├─────────────────────────────────────┤
│                                     │
│  Framework:     wxWidgets 3.2+      │
│  UI Widgets:    wxFrame             │
│  Graphics:      wxDC / wxGLCanvas   │
│  Threading:     wxThread, std::jthread
│  Signals:       Signal<T> concept   │
│  Memory:        std::atomic<int>    │
│  C++ Standard:  C++23               │
│  Dependencies:  None (0 deps)       │
│                                     │
│  Status: ✅ Modern, native, simple  │
│                                     │
└─────────────────────────────────────┘
```

---

## 5 Core Systems Analysis

```
┌──────────────────────────────────────────────────────────────────┐
│                   5 SYSTEMS TO MODERNIZE                         │
├──────────────────────────────────────────────────────────────────┤
│                                                                  │
│  SYSTEM 1: Signal/Slot System                                   │
│  ├─ Current:      Q_OBJECT + MOC (52 classes, 100+ signals)    │
│  ├─ Future:       Signal<T> concept + std::function            │
│  ├─ Portability:  9/10 ✅                                       │
│  ├─ Files:        15 (MainWindow, ThumbnailSequence, dialogs)  │
│  └─ Complexity:   Medium (pattern well-known)                  │
│                                                                  │
│  SYSTEM 2: Memory Management                                    │
│  ├─ Current:      RefCountable + QAtomicInt                    │
│  ├─ Future:       RefCountable + std::atomic<int32_t>          │
│  ├─ Portability:  9.5/10 ✅ (TRIVIAL - 1 line change!)        │
│  ├─ Files:        2 (RefCountable.h, IntrusivePtr.h)          │
│  └─ Complexity:   Trivial (already C++ pure)                   │
│                                                                  │
│  SYSTEM 3: Background Task Execution                           │
│  ├─ Current:      BackgroundExecutor + WorkerThread            │
│  ├─ Future:       wxThread + PostEvent equivalent             │
│  ├─ Portability:  9/10 ✅                                       │
│  ├─ Files:        3 (BackgroundExecutor, WorkerThread, tasks)  │
│  └─ Complexity:   Low (architecture already good)              │
│                                                                  │
│  SYSTEM 4: Graphics & Rendering                               │
│  ├─ Current:      QGraphicsView/Scene, ImageViewBase           │
│  ├─ Future:       wxScrolledWindow + wxDC manual rendering    │
│  ├─ Portability:  7/10 ⚠️ (requires redesign)                  │
│  ├─ Files:        2-3 (ThumbnailSequence, ImageViewBase)      │
│  └─ Complexity:   High (graphics logic rewrite)               │
│                                                                  │
│  SYSTEM 5: Filter Pipeline                                     │
│  ├─ Current:      AbstractFilter + 6 filters (100% C++ pure)  │
│  ├─ Future:       Same, just wxWidgets UI wrappers            │
│  ├─ Portability:  8/10 ✅ (core logic untouched)              │
│  ├─ Files:        18 (6 filters × 3 files each)               │
│  └─ Complexity:   Low (UI wrappers only)                      │
│                                                                  │
├──────────────────────────────────────────────────────────────────┤
│  AVERAGE PORTABILITY:     80% ✅                                 │
│  TOTAL FILES ANALYZED:    ~100 (source code base)             │
│  CODE REUSABLE AS-IS:     80% (pure C++ algorithms)           │
└──────────────────────────────────────────────────────────────────┘
```

---

## Hybrid Architecture (Proposed)

```
┌─────────────────────────────────────────────────────────────────┐
│              LAYER 1: CORE (C++23 Pure)                         │
│  ┌───────────────────────────────────────────────────────────┐  │
│  │ • Image Processing Algorithms (100% portable)             │  │
│  │ • Filter Pipeline (100% portable)                         │  │
│  │ • Background Task Execution (95% portable)                │  │
│  │ • Memory Management (99.5% portable - 1 line change)     │  │
│  │                                                            │  │
│  │ ✅ Status: Minimal changes, maximum C++23 benefits       │  │
│  └───────────────────────────────────────────────────────────┘  │
└─────────────────────────────────────────────────────────────────┘
              ▲
              │
              │ Abstraction Layer (C++23 Concepts)
              │
┌─────────────────────────────────────────────────────────────────┐
│        LAYER 2: ABSTRACTION (C++23 Concepts & Interfaces)      │
│  ┌───────────────────────────────────────────────────────────┐  │
│  │ • ImageViewBase (generic interface)                       │  │
│  │ • FilterUiInterface (abstract callbacks)                  │  │
│  │ • DrawContext (generic rendering)                         │  │
│  │ • InteractionHandler (input handling)                     │  │
│  │                                                            │  │
│  │ ✅ Status: Framework-agnostic, concept-based            │  │
│  └───────────────────────────────────────────────────────────┘  │
└─────────────────────────────────────────────────────────────────┘
              ▲
              │
              │ Platform Binding
              │
┌─────────────────────────────────────────────────────────────────┐
│        LAYER 3: wxWidgets IMPLEMENTATION (Native UI)           │
│  ┌───────────────────────────────────────────────────────────┐  │
│  │ • MainWindow (wxFrame)                                    │  │
│  │ • Dialogs (wxDialog)                                      │  │
│  │ • ThumbnailSequence (wxScrolledWindow + manual rendering)│  │
│  │ • ImageViewImplementation (wxScrolledWindow + wxDC)      │  │
│  │ • Filter OptionsWidgets (wxPanel, wxSizer)              │  │
│  │                                                            │  │
│  │ ✅ Status: Platform-specific, wxWidgets native          │  │
│  └───────────────────────────────────────────────────────────┘  │
└─────────────────────────────────────────────────────────────────┘

BENEFITS:
✅ Clean separation of concerns
✅ Core logic remains platform-independent
✅ Easy to test (mock abstractions)
✅ Framework could be replaced in future
✅ C++23 concepts enable compile-time polymorphism
```

---

## 9-Week Execution Timeline

```
┌─────────────────────────────────────────────────────────────────┐
│                    PHASE 1: FOUNDATION (Weeks 1-2)             │
│  ├─ Day 1-2:  CMakeLists.txt C++23 configuration              │
│  ├─ Day 3-5:  Signal<T> system implementation                 │
│  ├─ Day 6-7:  Memory management (1 line: std::atomic)         │
│  ├─ Day 8-10: Integration testing & validation                │
│  └─ Commits:  First working builds with new stack             │
└─────────────────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────────────────┐
│             PHASE 2: ABSTRACTION LAYER (Weeks 3-4)             │
│  ├─ Day 1-3:  FilterUiInterface abstract callbacks             │
│  ├─ Day 4-6:  ImageViewBase → generic interface                │
│  ├─ Day 7-9:  InteractionHandler refactoring                   │
│  └─ Day 10:   Integration & code review                        │
└─────────────────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────────────────┐
│          PHASE 3: GRAPHICS IMPLEMENTATION (Weeks 5-6)          │
│  ├─ Day 1-5:  ThumbnailSequence → wxScrolledWindow             │
│  ├─ Day 6-9:  ImageView → wxDC rendering                       │
│  ├─ Day 10:   Graphics testing & profiling                     │
│  └─ Commits:  Graphics layer working                           │
└─────────────────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────────────────┐
│           PHASE 4: UI INTEGRATION (Weeks 7-8)                  │
│  ├─ Day 1-3:  MainWindow → wxFrame                             │
│  ├─ Day 4-6:  Dialogs conversion (15+ dialogs)                 │
│  ├─ Day 7-9:  Filter OptionsWidgets refactoring                │
│  ├─ Day 10:   Full UI integration testing                      │
│  └─ Commits:  Complete UI layer working                        │
└─────────────────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────────────────┐
│            PHASE 5: TESTING & RELEASE (Week 9)                 │
│  ├─ Day 1-3:  Unit tests, integration tests                    │
│  ├─ Day 4-5:  Performance profiling & optimization             │
│  ├─ Day 6-7:  Documentation & release notes                    │
│  └─ Day 8-10: Release preparation & validation                 │
└─────────────────────────────────────────────────────────────────┘
```

**Total: 63 days = 9 weeks (2-3 developers)**

---

## Risk Mitigation Strategy

```
RISK                           PROBABILITY    MITIGATION
───────────────────────────────────────────────────────────────
Graphics layer underestimated  MEDIUM        PoC before Phase 3
Team learning curve            MEDIUM        3-day training
Integration issues             LOW           Modular approach
Timeline slippage             MEDIUM         Buffer weeks
wxWidgets API unknowns         LOW           Reference docs
Performance regression         LOW           Profiling Phase 5
```

---

## Success Criteria (Testing Strategy)

```
✅ FUNCTIONAL
   ├─ All image filters working
   ├─ UI responsive on all platforms
   ├─ No crashes in 1-hour load test
   └─ Feature parity with Qt version

✅ PERFORMANCE
   ├─ Binary size -30% vs Qt5
   ├─ Startup time ≤ +5%
   ├─ Memory usage ±5%
   └─ Processing speed ±0%

✅ CODE QUALITY
   ├─ Test coverage ≥85%
   ├─ 0 compiler warnings
   ├─ Code duplication <5%
   └─ Cyclomatic complexity <10

✅ TECHNICAL DEBT
   ├─ MOC completely eliminated
   ├─ Qt completely eliminated
   ├─ C++23 fully adopted
   └─ Documentation complete
```

---

## Decision Point

```
           CURRENT STATE                    DECISION NEEDED
     ┌─────────────────────┐                ┌──────────────┐
     │ Analysis Complete   │ ──────────────▶│ Proceed Y/N? │
     │ Viability: 75%+     │                └──────────────┘
     │ Timeline: 9 weeks   │                      │
     │ Cost: €250K         │                      │
     │ Team: 2-3 devs      │                      │
     └─────────────────────┘                      │
                                                  ├─▶ YES: Phase 1 Kickoff
                                                  │
                                                  └─▶ NO: Continue Qt5 (debt)
```

---

## C++23 Features Leveraged

```
┌─ CONCEPTS
│  └─ Type-safe generic programming without MOC
│
├─ AUTO + STRUCTURED BINDINGS
│  └─ -60% boilerplate in UI setup code
│
├─ std::JTHREAD
│  └─ Scoped threading with automatic joining
│
├─ std::FUNCTION + LAMBDAS
│  └─ Type-safe callbacks (replaces signals)
│
├─ std::RANGES
│  └─ Modern iteration (cleaner loops)
│
├─ std::ATOMIC
│  └─ Lock-free reference counting
│
└─ COROUTINES (FUTURE)
   └─ Async task handling
```

---

## File Organization Post-Migration

```
scantailor/
├── src/
│   ├── core/               ← C++23 Pure (no wx)
│   │   ├── image/
│   │   ├── filters/
│   │   ├── tasks/
│   │   └── memory/
│   │
│   ├── abstraction/        ← Concepts, interfaces
│   │   ├── image_view.h
│   │   ├── ui_interface.h
│   │   └── interaction.h
│   │
│   └── ui/                 ← wxWidgets Implementation
│       ├── main_window.cpp
│       ├── dialogs/
│       ├── widgets/
│       └── thumbnail_view/
│
└── cmake/
    └── C++23 config
```

---

## Success Probability Breakdown

```
Graphics Implementation:     85% (moderate risk)
Signal/Slot Conversion:      95% (well-known pattern)
Memory Management:           99% (trivial change)
Task Execution:              95% (architecture equivalent)
Filter Pipeline:             98% (unchanged algorithms)
UI Integration:              80% (wxWidgets learning)
Testing Coverage:            90% (systematic approach)
Timeline Adherence:          75% (realistic estimation)

OVERALL PROBABILITY:         ≈75-80% ✅ ACCEPTABLE
```

---

*Architecture diagram v1 | Updated: January 31, 2026 | Status: Ready for implementation*
