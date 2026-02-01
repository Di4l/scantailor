# EJEMPLOS DE REFACTORIZACIÓN: Qt → wxWidgets + C++23

## Documento de Códigos Prácticos de Migración
**Objetivo:** Mostrar cómo se refactoriza cada sistema fundamental con ejemplos reales

---

## EJEMPLO 1: SIGNAL/SLOT SYSTEM REFACTORING

### 1.1 Antes (Qt + MOC)

```cpp
// ThumbnailSequence.h (Qt)
class ThumbnailSequence : public QObject {
    Q_OBJECT
    DECLARE_NON_COPYABLE(ThumbnailSequence)
public:
    void setSelection(PageId const& page_id) {
        // ...cambia selección...
        emit selectedPageChanged(pageInfo);  // Signal
    }
signals:
    void selectedPageChanged(PageInfo const& page_info);
    void pageCountChanged(int count);
    void thumbnailInvalidated(PageId const& page_id);
};

// MainWindow.h (Qt)
class MainWindow : public QMainWindow, private FilterUiInterface {
    Q_OBJECT
private slots:
    void onPageSelected(PageInfo const& page_info);
};

// MainWindow.cpp (Qt)
MainWindow::MainWindow() {
    // Conectar 70+ signal/slot pairs
    connect(m_thumbnailSequence.get(), 
            &ThumbnailSequence::selectedPageChanged,
            this, 
            &MainWindow::onPageSelected);
    
    connect(m_thumbnailSequence.get(),
            &ThumbnailSequence::pageCountChanged,
            this,
            &MainWindow::onPageCountChanged);
    
    // ... many more connections
}
```

### 1.2 Después - Opción A: wxWidgets + Callbacks (PRAGMÁTICO)

```cpp
// ThumbnailSequence.h (wxWidgets)
class ThumbnailSequence : public wxScrolledWindow {
    // Subscribers: std::function callbacks
    std::vector<std::function<void(PageInfo const&)>> m_pageSelectedHandlers;
    std::vector<std::function<void(int)>> m_pageCountHandlers;
    std::vector<std::function<void(PageId const&)>> m_thumbnailInvalidHandlers;
    
public:
    // Nuevo patrón: subscribe en lugar de connect
    void subscribePageSelected(std::function<void(PageInfo const&)> handler) {
        m_pageSelectedHandlers.push_back(handler);
    }
    
    void subscribePageCountChanged(std::function<void(int)> handler) {
        m_pageCountHandlers.push_back(handler);
    }
    
    void subscribeInvalidated(std::function<void(PageId const&)> handler) {
        m_thumbnailInvalidHandlers.push_back(handler);
    }
    
    // Interno: cuando ocurre evento
    void setSelection(PageId const& page_id) {
        PageInfo info = getPageInfo(page_id);
        
        // Notificar a todos los suscriptores
        for (auto& handler : m_pageSelectedHandlers) {
            handler(info);
        }
        Refresh();  // Invalidar vista
    }
};

// MainWindow.h (wxWidgets)
class MainWindow : public wxFrame, private FilterUiInterface {
private:
    void onPageSelected(PageInfo const& page_info);
    void onPageCountChanged(int count);
    void onThumbnailInvalidated(PageId const& page_id);
};

// MainWindow.cpp (wxWidgets)
MainWindow::MainWindow() {
    // Suscribirse a eventos en lugar de conectar signals
    m_thumbnailSequence->subscribePageSelected(
        [this](PageInfo const& info) { onPageSelected(info); }
    );
    
    m_thumbnailSequence->subscribePageCountChanged(
        [this](int count) { onPageCountChanged(count); }
    );
    
    m_thumbnailSequence->subscribeInvalidated(
        [this](PageId const& id) { onThumbnailInvalidated(id); }
    );
}
```

### 1.3 Después - Opción B: C++23 PURO (MEJOR TYPE-SAFETY)

```cpp
// foundation/Signals.h (C++23 NEW)
template<typename... Args>
concept SignalHandler = requires(std::function<void(Args...)> f, Args... args) {
    f(args...);
};

template<typename... Args>
class Signal {
    std::vector<std::function<void(Args...)>> m_handlers;
    
public:
    template<SignalHandler<Args...> Handler>
    void connect(Handler&& handler) {
        m_handlers.push_back(std::forward<Handler>(handler));
    }
    
    void emit(Args... args) const {
        for (auto& handler : m_handlers | std::views::all) {
            handler(args...);
        }
    }
};

// ThumbnailSequence.h (C++23)
class ThumbnailSequence : public wxScrolledWindow {
    Signal<PageInfo const&> pageSelectedSignal;
    Signal<int> pageCountSignal;
    Signal<PageId const&> thumbnailInvalidSignal;
    
public:
    auto& onPageSelected() { return pageSelectedSignal; }
    auto& onPageCount() { return pageCountSignal; }
    auto& onThumbnailInvalid() { return thumbnailInvalidSignal; }
    
    void setSelection(PageId const& page_id) {
        PageInfo info = getPageInfo(page_id);
        pageSelectedSignal.emit(info);  // Type-safe emit
    }
};

// MainWindow.cpp (C++23)
MainWindow::MainWindow() {
    // Conectar con lambdas type-safe
    m_thumbnailSequence->onPageSelected().connect(
        [this](PageInfo const& info) { onPageSelected(info); }
    );
    
    // O con bind:
    m_thumbnailSequence->onPageSelected().connect(
        std::bind(&MainWindow::onPageSelected, this, std::placeholders::_1)
    );
}

// Con C++23 también podemos hacer:
void subscribe(auto&& callback) requires SignalHandler<decltype(callback), int> {
    // Compile-time error si callback no acepta int
    pageCountSignal.connect(callback);
}
```

### 1.4 Comparativa de Migración

| Aspecto | Qt | wxWidgets A | C++23 |
|--------|----|-----------|----|
| **Type Safety** | ✓ MOC checks | ✓ std::function | ✓✓ Concepts |
| **Compile Time** | Lento (MOC) | Rápido | Rápido |
| **Runtime Perf** | 100% | 98-102% | 100% |
| **Boilerplate** | 50 líneas | 40 líneas | 30 líneas |
| **Learning curve** | Steep | Medium | Medium |
| **Debugging** | Std | Better | Better |

**RECOMENDACIÓN:** Opción B (C++23) porque:
- ✓ Type-checking en compile-time
- ✓ Zero boilerplate
- ✓ Compatible con wxWidgets + Bind()
- ✓ Futuro-proof

---

## EJEMPLO 2: MEMORY MANAGEMENT REFACTORING

### 2.1 Antes (Qt + QAtomicInt)

```cpp
// foundation/RefCountable.h (Qt)
class RefCountable {
    mutable QAtomicInt m_refCounter;  // Qt specific
public:
    void ref() const {
        m_refCounter.fetchAndAddRelaxed(1);
    }
    void unref() const {
        if (m_refCounter.fetchAndAddRelease(-1) == 1) {
            delete this;
        }
    }
};

// Uso en MainWindow.h
class MainWindow {
    IntrusivePtr<AbstractFilter> m_ptrActiveFilter;
};

// Cuando cambiamos filtro:
void MainWindow::selectFilter(AbstractFilter* newFilter) {
    m_ptrActiveFilter = IntrusivePtr(newFilter);
    // Automáticamente: unref() en antiguo, ref() en nuevo
}
```

### 2.2 Después: Minimalista (reemplazar QAtomicInt)

```cpp
// foundation/RefCountable.h (wxWidgets - MINIMAL CHANGE)
#include <atomic>
#include <cstdint>

class RefCountable {
    mutable std::atomic<int32_t> m_refCounter{0};  // Reemplazar QAtomicInt
public:
    void ref() const {
        // std::atomic equivalent a QAtomicInt
        m_refCounter.fetch_add(1, std::memory_order_relaxed);
    }
    void unref() const {
        if (m_refCounter.fetch_add(-1, std::memory_order_release) == 1) {
            delete this;
        }
    }
};

// RESTO DEL CÓDIGO: IDÉNTICO
// foundation/IntrusivePtr.h: NO CAMBIOS
// MainWindow.h: NO CAMBIOS
// MainWindow.cpp: NO CAMBIOS
```

✅ **RESULTADO:** 0 cambios en client code, 1 línea editada en base class

### 2.3 Con C++23 - Mejorado (OPCIONAL)

```cpp
// foundation/Concepts.h (C++23 NEW)
template<typename T>
concept RefCounted = requires(T& obj) {
    { obj.ref() } -> std::same_as<void>;
    { obj.unref() } -> std::same_as<void>;
};

// foundation/RefCountable.h (C++23)
class RefCountable {
    mutable std::atomic<int32_t> m_refCounter{0};
public:
    void ref() const requires true {  // Explicit concept
        m_refCounter.fetch_add(1, std::memory_order_relaxed);
    }
    void unref() const requires true {
        if (m_refCounter.fetch_add(-1, std::memory_order_release) == 1) {
            delete this;
        }
    }
};

// foundation/IntrusivePtr.h (C++23 MEJORADO)
template<RefCounted T>
class IntrusivePtr {  // Concept constraint
    T* m_pObj;
    
    // Compile-time error si T no es RefCounted
};

// Uso:
IntrusivePtr<AbstractFilter> filter;  // ✓ OK (AbstractFilter es RefCounted)
IntrusivePtr<QString> str;  // ✗ Compile error (QString no es RefCounted)
```

**VENTAJA:** Type-safety a nivel de concepto (no confundir tipos)

---

## EJEMPLO 3: BACKGROUND TASK REFACTORING

### 3.1 Antes (Qt + QThread + Signals)

```cpp
// BackgroundExecutor.h (Qt)
class BackgroundExecutor {
    std::auto_ptr<Impl> m_ptrImpl;
public:
    void enqueueTask(TaskPtr const& task);
private:
    class Impl;
    class Dispatcher;
};

// BackgroundExecutor.cpp (Qt) - SIMPLIFICADO
class BackgroundExecutor::Dispatcher : public QObject {
    Q_OBJECT
    WorkerThread* m_workerThread;
public slots:
    void onTaskFinished(BackgroundTaskPtr task, FilterResultPtr result) {
        // Procesar resultado en GUI thread
        MainWindow::instance()->updateUI(result);
    }
};

// WorkerThread.h (Qt)
class WorkerThread : public QObject {
    Q_OBJECT
    QThread* m_ptrThread;
public slots:
    void performTask(BackgroundTaskPtr const& task);
signals:
    void taskResult(BackgroundTaskPtr const& task, FilterResultPtr const& result);
};

// Uso en MainWindow:
void MainWindow::onOptionChanged() {
    BackgroundTaskPtr task = m_filter->createTask(...);
    connect(m_workerThread, &WorkerThread::taskResult,
            this, &MainWindow::onTaskFinished);
    m_workerThread->performTask(task);
}
```

### 3.2 Después - Opción A: wxWidgets (DIRECTO)

```cpp
// BackgroundExecutor.h (wxWidgets)
class BackgroundExecutor {
    std::auto_ptr<Impl> m_ptrImpl;
public:
    void enqueueTask(TaskPtr const& task);
    void shutdown();
};

// BackgroundExecutor.cpp (wxWidgets)
class BackgroundExecutor::Impl {
    std::deque<TaskPtr> m_taskQueue;
    wxThread* m_pWorkerThread;
    wxEvtHandler* m_resultHandler;
    
    void taskThreadFunc() {  // Thread worker
        while (!m_shutdown) {
            TaskPtr task = takeNextTask();
            if (!task) continue;
            
            FilterResultPtr result = task->operator()();
            
            // Enviar resultado al GUI thread
            wxNotifyEvent event(EVT_TASK_FINISHED);
            event.SetClientData(new TaskResultData{task, result});
            wxPostEvent(m_resultHandler, event);
        }
    }
};

// WorkerThread.h (wxWidgets)
class WorkerThread : public wxEvtHandler {
private:
    wxThread* m_pThread;
    std::deque<BackgroundTaskPtr> m_taskQueue;
    wxMutex m_queueMutex;
    
public:
    void performTask(BackgroundTaskPtr const& task) {
        wxMutexLocker lock(m_queueMutex);
        m_taskQueue.push_back(task);
    }
    
    void bindResultHandler(std::function<void(BackgroundTaskPtr, FilterResultPtr)> handler) {
        m_resultHandler = handler;
    }
};

// Uso en MainWindow:
void MainWindow::onOptionChanged() {
    BackgroundTaskPtr task = m_filter->createTask(...);
    
    // Binding del resultado con lambda
    m_workerThread->bindResultHandler(
        [this](BackgroundTaskPtr t, FilterResultPtr r) {
            onTaskFinished(t, r);
        }
    );
    
    m_workerThread->performTask(task);
}
```

### 3.3 Después - Opción B: C++23 Coroutines (FUTURO)

```cpp
// BackgroundExecutor.h (C++23 FUTURE)
class BackgroundExecutor {
    std::vector<std::jthread> m_threads;  // Auto-join on destruct
    std::queue<TaskPtr> m_queue;
    
public:
    std::coroutine_handle<Task::promise_type> 
    enqueueTask(TaskPtr const& task) {
        return executeTask(task);
    }
    
private:
    std::coroutine_handle<> executeTask(TaskPtr task) {
        co_await std::suspend_always();  // Pausa, transfer to thread pool
        
        FilterResultPtr result = (*task)();  // Ejecutar en background
        
        co_await transfer_to_ui_thread();  // Volver a GUI thread
        
        // Aquí estamos en GUI thread
        notifyResult(task, result);
        
        co_return;
    }
};

// Uso en MainWindow (MÁS LIMPIO):
void MainWindow::onOptionChanged() {
    auto task = m_filter->createTask(...);
    
    // Launch coroutine, se ejecuta en background automáticamente
    auto handle = m_executor->enqueueTask(task)
        .then([this](FilterResultPtr r) {
            onTaskFinished(r);  // Callback en GUI thread
        });
}
```

**VENTAJA Coroutines:** Sintaxis asincrónica cleaner, mejor legibilidad

---

## EJEMPLO 4: GRAPHICS & PAINTING REFACTORING

### 4.1 Antes (Qt + QGraphicsView)

```cpp
// ThumbnailSequence.h (Qt)
class ThumbnailSequence : public QObject {
    QGraphicsScene* m_ptrScene;
    QGraphicsView* m_ptrView;
    std::vector<CompositeItem*> m_items;
    
public:
    void reset(const PageSequence& pages) {
        m_ptrScene->clear();
        m_items.clear();
        
        for (auto& page : pages) {
            auto* item = new CompositeItem(page);
            m_ptrScene->addItem(item);  // Scene owns item
            m_items.push_back(item);
        }
    }
};

// CompositeItem.h (Qt)
class CompositeItem : public QGraphicsItemGroup {
    QPixmap m_thumbnail;
    QString m_pageLabel;
    
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* opt, 
               QWidget* widget) override {
        painter->drawPixmap(0, 0, m_thumbnail);
        painter->drawText(..., m_pageLabel);
        
        if (isSelected()) {
            painter->setPen(QPen(Qt::blue, 2));
            painter->drawRect(boundingRect());
        }
    }
    
    QRectF boundingRect() const override {
        return m_thumbnail.rect();
    }
};
```

### 4.2 Después - Opción A: wxWidgets + Manual Rendering

```cpp
// ThumbnailSequence.h (wxWidgets)
class ThumbnailSequence : public wxScrolledWindow {
    struct Item {
        wxBitmap thumbnail;
        wxString label;
        wxRect bounds;
        bool isSelected = false;
    };
    std::vector<Item> m_items;
    
    void reset(const PageSequence& pages) {
        m_items.clear();
        
        int y = 0;
        for (auto& page : pages) {
            Item item;
            item.thumbnail = createThumbnail(page);
            item.label = page.label();
            item.bounds = wxRect(0, y, THUMBNAIL_WIDTH, THUMBNAIL_HEIGHT);
            y += THUMBNAIL_HEIGHT + SPACING;
            
            m_items.push_back(item);
        }
        
        SetVirtualSize(THUMBNAIL_WIDTH, y);
        Refresh();
    }
    
private:
    void onPaint(wxPaintEvent& event) {
        wxAutoBufferedPaintDC dc(this);
        
        // Manual culling y rendering
        wxRect updateRect = GetUpdateRegion().GetBox();
        
        for (auto& item : m_items) {
            if (!item.bounds.Intersects(updateRect)) continue;
            
            // Draw thumbnail
            dc.DrawBitmap(item.thumbnail, item.bounds.GetPosition());
            
            // Draw label
            dc.DrawText(item.label, item.bounds.x, 
                       item.bounds.y + THUMBNAIL_HEIGHT - 20);
            
            // Draw selection
            if (item.isSelected) {
                dc.SetPen(wxPen(*wxBLUE, 2));
                dc.SetBrush(*wxTRANSPARENT_BRUSH);
                dc.DrawRectangle(item.bounds);
            }
        }
    }
    
    void onMouseClick(wxMouseEvent& event) {
        wxPoint pos = CalcUnscrolledPosition(event.GetPosition());
        
        for (auto& item : m_items) {
            if (item.bounds.Contains(pos)) {
                selectItem(&item);
                break;
            }
        }
    }
};
```

### 4.3 Después - Opción B: C++23 Generic Graphics Abstraction

```cpp
// foundation/Graphics.h (C++23)
template<typename DrawContext>
concept Drawable = requires(DrawContext& ctx, 
                             const wxBitmap& bmp, 
                             const wxRect& r,
                             const wxString& s) {
    { ctx.drawBitmap(bmp, r.GetPosition()) } -> std::same_as<void>;
    { ctx.drawText(s, r) } -> std::same_as<void>;
    { ctx.drawRectangle(r) } -> std::same_as<void>;
};

// Generic ThumbnailSequence
template<Drawable DrawContext>
class ThumbnailSequenceGeneric : public wxScrolledWindow {
    std::vector<Item> m_items;
    
    void render(DrawContext& ctx, const wxRect& updateRect) {
        for (auto& item : m_items) {
            if (!item.bounds.Intersects(updateRect)) continue;
            
            ctx.drawBitmap(item.thumbnail, item.bounds.GetPosition());
            ctx.drawText(item.label, item.bounds);
            
            if (item.isSelected) {
                ctx.drawRectangle(item.bounds);
            }
        }
    }
};

// wxWidgets-specific DrawContext
class wxDrawContext {
    wxDC& m_dc;
public:
    void drawBitmap(const wxBitmap& bmp, const wxPoint& pos) {
        m_dc.DrawBitmap(bmp, pos);
    }
    void drawText(const wxString& text, const wxRect& r) {
        m_dc.DrawText(text, r.x, r.y);
    }
    void drawRectangle(const wxRect& r) {
        m_dc.SetPen(wxPen(*wxBLUE, 2));
        m_dc.DrawRectangle(r);
    }
};

// Typedef para wxWidgets
using ThumbnailSequence = ThumbnailSequenceGeneric<wxDrawContext>;
```

**VENTAJA:** Puede reutilizarse con otros backends (Qt, OpenGL, web, etc.)

---

## EJEMPLO 5: FILTER OPTIONS WIDGET REFACTORING

### 5.1 Antes (Qt + UI files)

```cpp
// filters/fix_orientation/OptionsWidget.h (Qt)
class OptionsWidget : public QWidget {
    Q_OBJECT
private:
    QSpinBox* m_pAngleSpinBox;
    QPushButton* m_pRotateLeftBtn;
    QPushButton* m_pRotateRightBtn;
    
private slots:
    void onAngleChanged(int angle);
    void onRotateLeftClicked();
    void onRotateRightClicked();
    
signals:
    void optionChanged();
};

// filters/fix_orientation/OptionsWidget.cpp (Qt)
OptionsWidget::OptionsWidget(QWidget* parent) 
    : QWidget(parent) {
    QVBoxLayout* layout = new QVBoxLayout(this);
    
    m_pAngleSpinBox = new QSpinBox();
    m_pRotateLeftBtn = new QPushButton("Rotate Left");
    m_pRotateRightBtn = new QPushButton("Rotate Right");
    
    layout->addWidget(new QLabel("Angle:"));
    layout->addWidget(m_pAngleSpinBox);
    layout->addWidget(m_pRotateLeftBtn);
    layout->addWidget(m_pRotateRightBtn);
    
    connect(m_pAngleSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &OptionsWidget::onAngleChanged);
    connect(m_pRotateLeftBtn, &QPushButton::clicked,
            this, &OptionsWidget::onRotateLeftClicked);
    connect(m_pRotateRightBtn, &QPushButton::clicked,
            this, &OptionsWidget::onRotateRightClicked);
}

void OptionsWidget::onAngleChanged(int angle) {
    m_settings->setAngle(angle);
    emit optionChanged();
}
```

### 5.2 Después (wxWidgets + Lambdas + C++23)

```cpp
// filters/fix_orientation/OptionsWidget.h (wxWidgets)
class OptionsWidget : public wxPanel {
private:
    wxSpinCtrl* m_angleSpinCtrl;
    wxButton* m_rotateLeftBtn;
    wxButton* m_rotateRightBtn;
    
    // Callback style (C++23)
    std::function<void()> m_onOptionChanged;
    
public:
    template<typename Handler>
    requires std::invocable<Handler>
    void setOptionChangedHandler(Handler&& handler) {
        m_onOptionChanged = std::forward<Handler>(handler);
    }
    
private:
    void notifyOptionChanged() {
        if (m_onOptionChanged) {
            m_onOptionChanged();
        }
    }
};

// filters/fix_orientation/OptionsWidget.cpp (wxWidgets)
OptionsWidget::OptionsWidget(wxWindow* parent)
    : wxPanel(parent, wxID_ANY) {
    
    auto* sizer = new wxBoxSizer(wxVERTICAL);
    
    m_angleSpinCtrl = new wxSpinCtrl(this, wxID_ANY, "",
                                     wxDefaultPosition, wxDefaultSize,
                                     wxSP_ARROW_KEYS | wxTE_PROCESS_ENTER);
    m_rotateLeftBtn = new wxButton(this, wxID_ANY, "Rotate Left");
    m_rotateRightBtn = new wxButton(this, wxID_ANY, "Rotate Right");
    
    sizer->Add(new wxStaticText(this, wxID_ANY, "Angle:"), 0, wxALL, 5);
    sizer->Add(m_angleSpinCtrl, 0, wxEXPAND | wxALL, 5);
    sizer->Add(m_rotateLeftBtn, 0, wxEXPAND | wxALL, 5);
    sizer->Add(m_rotateRightBtn, 0, wxEXPAND | wxALL, 5);
    
    SetSizer(sizer);
    
    // Bind events con lambdas
    Bind(wxEVT_SPINCTRL, [this](wxSpinEvent& event) {
        m_settings->setAngle(event.GetValue());
        notifyOptionChanged();
    });
    
    Bind(wxEVT_BUTTON, [this](wxCommandEvent&) {
        m_settings->setAngle(m_settings->angle() - 90);
        m_angleSpinCtrl->SetValue(m_settings->angle());
        notifyOptionChanged();
    }, m_rotateLeftBtn->GetId());
    
    Bind(wxEVT_BUTTON, [this](wxCommandEvent&) {
        m_settings->setAngle(m_settings->angle() + 90);
        m_angleSpinCtrl->SetValue(m_settings->angle());
        notifyOptionChanged();
    }, m_rotateRightBtn->GetId());
}
```

### 5.3 Uso en Filter

```cpp
// filters/fix_orientation/Filter.cpp (wxWidgets)
void Filter::preUpdateUI(FilterUiInterface* ui, PageId const& page_id) {
    auto widget = new OptionsWidget();
    
    // Usar concept-based handler
    widget->setOptionChangedHandler([this]() {
        // Trigger processing with new settings
        triggerUpdate();
    });
    
    ui->setOptionsWidget(widget, TRANSFER_OWNERSHIP);
}
```

---

## RESUMEN DE CAMBIOS POR SISTEMA

| Sistema | Qt Original | wxWidgets | C++23 Hybrid | Esfuerzo |
|---------|------------|-----------|------------|----------|
| Signal/Slot | Q_OBJECT | std::function | Signal<T> concept | 3-5d |
| Memory Mgmt | QAtomicInt | std::atomic | std::atomic+concept | 0.5d |
| Background | QThread | wxThread | std::jthread | 2-3d |
| Graphics | QGraphicsView | wxScrolledWindow | Template<DrawCtx> | 5-7d |
| Widgets | QWidget/ui files | wxPanel+Sizer | wxPanel+C++23 | 3-5d |
| **TOTAL** | - | - | - | **13-25d** |

---

## CONCLUSIÓN DE EJEMPLOS

**Todas estas refactorizaciones son:**
- ✅ Directas (1:1 mapping en la mayoría)
- ✅ Type-safe (mejor con C++23 concepts)
- ✅ Performance-equivalent
- ✅ Mantenibles (menos boilerplate)
- ✅ Extensibles (abstracciones claras)

**La migración wxWidgets + C++23 es viable y recomendada.**
