# Análisis Detallado de Uso de BOOST: Casos de Uso Real

## 1. FOREACH LOOPS - Análisis Detallado (78 archivos)

### Uso Típico en el Código

```cpp
// ACTUAL - dewarping/gui/Curve.cpp
BOOST_FOREACH(auto segment, m_segments) {
    segment.update();
}

// C++23 - Reemplazo directo
for (auto segment : m_segments) {
    segment.update();
}
```

### Patrones Encontrados

#### Patrón 1: Iteración Simple (90% de los casos)
```cpp
// ANTES
BOOST_FOREACH(auto& item, container) {
    item.process();
}

// DESPUÉS
for (auto& item : container) {
    item.process();
}
```

#### Patrón 2: Iteración de Pairs (Diccionarios)
```cpp
// ANTES
BOOST_FOREACH(auto pair, mapContainer) {
    auto key = pair.first;
    auto value = pair.second;
}

// DESPUÉS
for (auto& [key, value] : mapContainer) {  // C++17 Structured bindings
    // use key and value
}
```

#### Patrón 3: Iteración Anidada
```cpp
// ANTES
BOOST_FOREACH(auto outer, outerList) {
    BOOST_FOREACH(auto inner, outer.children) {
        inner.process();
    }
}

// DESPUÉS
for (auto& outer : outerList) {
    for (auto& inner : outer.children) {
        inner.process();
    }
}
```

### Archivo de Compatibilidad Existente

**compat/boost_multi_index_foreach_fix.h** - Contiene workarounds para BOOST_FOREACH con multi_index_container

```cpp
#ifndef BOOST_MULTI_INDEX_FOREACH_FIX_H_
#define BOOST_MULTI_INDEX_FOREACH_FIX_H_

#include <boost/mpl/bool.hpp>
#include <boost/multi_index_container.hpp>
#include <boost/foreach.hpp>

// Workaround para problemas de FOREACH con multi_index
// Este archivo se puede eliminar completamente después de la migración
```

---

## 2. LAMBDA/BIND - Análisis Detallado (52 archivos)

### 2.1 Casos Complejos de Boost Lambda

#### Archivo: dewarping/gui/DetectVertContentBounds.cpp (Uso Intensivo)

```cpp
// ACTUAL - Uso de Boost Lambda
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>

namespace ll = boost::lambda;

// Búsqueda con predicado complejo
auto it = std::find_if(
    lines.begin(), lines.end(),
    ll::bind(&Line::height, ll::_1) > threshold
);

// Transformación con lógica condicional
std::transform(
    bounds.begin(), bounds.end(),
    std::back_inserter(result),
    ll::if_(ll::_1 > 0)[ll::_1 * 2].else_[0]
);
```

```cpp
// C++23 - Lambda Moderna
// Opción 1: Lambda explícita (más readable)
auto it = std::find_if(
    lines.begin(), lines.end(),
    [](const Line& line) { return line.height() > threshold; }
);

// Opción 2: Para transformaciones complejas
std::transform(
    bounds.begin(), bounds.end(),
    std::back_inserter(result),
    [](int x) { return x > 0 ? x * 2 : 0; }
);
```

#### Archivo: dewarping/gui/TextLineTracer.cpp

```cpp
// ACTUAL
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/if.hpp>
#include <boost/lambda/construct.hpp>

std::transform(
    pointsList.begin(), pointsList.end(),
    std::back_inserter(tracedLines),
    ll::construct<TracedLine>(
        ll::bind(&Point::x, ll::_1),
        ll::bind(&Point::y, ll::_1)
    )
);
```

```cpp
// C++23 - Lambda Simple
std::transform(
    pointsList.begin(), pointsList.end(),
    std::back_inserter(tracedLines),
    [](const Point& p) { return TracedLine(p.x(), p.y()); }
);
```

### 2.2 Boost Bind para Métodos

#### Archivo: interaction/gui/InteractionHandler.cpp

```cpp
// ACTUAL
#include <boost/bind.hpp>

void setupCallbacks() {
    m_onMouseMove = boost::bind(
        &InteractionHandler::handleMouseMove, 
        this, 
        _1
    );
}

// Usando el callback
m_onMouseMove(event);
```

```cpp
// C++23 Alternativa 1: std::bind (backward compatible)
#include <functional>

void setupCallbacks() {
    m_onMouseMove = std::bind(
        &InteractionHandler::handleMouseMove,
        this,
        std::placeholders::_1
    );
}
```

```cpp
// C++23 Alternativa 2: Lambda capturada (mejor)
void setupCallbacks() {
    m_onMouseMove = [this](const auto& event) {
        this->handleMouseMove(event);
    };
}
```

```cpp
// C++23 Alternativa 3: std::move_only_function (C++23)
#include <functional>

std::move_only_function<void(const MouseEvent&)> m_onMouseMove;

void setupCallbacks() {
    m_onMouseMove = [this](const MouseEvent& event) {
        this->handleMouseMove(event);
    };
}
```

### 2.3 Casos de Control Estructurado

#### Archivo: src/EstimateBackground.cpp

```cpp
// ACTUAL - Control estructura complejo
#include <boost/lambda/control_structures.hpp>

std::for_each(
    pixels.begin(), pixels.end(),
    ll::if_(ll::_1 > threshold)[
        ll::_1 = ll::_1 - adjustment
    ].else_[
        ll::_1 = 0
    ]
);
```

```cpp
// C++23 - Reemplazo Simple
std::for_each(
    pixels.begin(), pixels.end(),
    [](auto& pixel) {
        if (pixel > threshold) {
            pixel = pixel - adjustment;
        } else {
            pixel = 0;
        }
    }
);
```

---

## 3. BOOST FUNCTION - Análisis Detallado (18 archivos)

### Uso en foundation/gui/QtSignalForwarder.h

```cpp
// ACTUAL
#include <boost/function.hpp>

class QtSignalForwarder {
public:
    typedef boost::function<void()> VoidCallback;
    typedef boost::function<void(const QString&)> StringCallback;
    
    void setCallback(const VoidCallback& cb) {
        m_callback = cb;
    }
    
private:
    VoidCallback m_callback;
};
```

```cpp
// C++23 - Opción 1: std::function (C++11, recomendado para compatibilidad)
#include <functional>

class QtSignalForwarder {
public:
    using VoidCallback = std::function<void()>;
    using StringCallback = std::function<void(const QString&)>;
    
    void setCallback(const VoidCallback& cb) {
        m_callback = cb;
    }
    
private:
    VoidCallback m_callback;
};
```

```cpp
// C++23 - Opción 2: std::move_only_function (C++23, mejor rendimiento)
#include <functional>

class QtSignalForwarder {
public:
    using VoidCallback = std::move_only_function<void()>;
    using StringCallback = std::move_only_function<void(const QString&)>;
    
    void setCallback(VoidCallback cb) {
        m_callback = std::move(cb);
    }
    
private:
    VoidCallback m_callback;
};
```

### Patrones de Uso

#### Patrón 1: Callbacks Genéricos
```cpp
// ACTUAL
filters::output::OutputGenerator::Delegate delegate;
delegate = boost::bind(&ImageView::updateImage, &view, _1);

// C++23
filters::output::OutputGenerator::Delegate delegate = 
    [&view](const QImage& img) { view.updateImage(img); };
```

#### Patrón 2: Callback Almacenado
```cpp
// ACTUAL
class FillZoneEditor {
    boost::function<void(const Zone&)> m_onZoneChanged;
};

// C++23
class FillZoneEditor {
    std::function<void(const Zone&)> m_onZoneChanged;
    // o mejor aún:
    std::move_only_function<void(const Zone&)> m_onZoneChanged;
};
```

---

## 4. MULTI-INDEX CONTAINER - Análisis Detallado (7 archivos)

### Casos de Uso Complejos

#### Archivo: src/FileNameDisambiguator.cpp

```cpp
// ACTUAL - Multi-Index con 2 índices
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/sequenced_index.hpp>
#include <boost/multi_index/member.hpp>

struct FileEntry {
    int id;
    std::string originalName;
    std::string disambiguatedName;
    int priority;
};

typedef boost::multi_index_container<
    FileEntry,
    boost::multi_index::indexed_by<
        // Índice 1: Ordenado por ID
        boost::multi_index::ordered_unique<
            boost::multi_index::member<FileEntry, int, &FileEntry::id>
        >,
        // Índice 2: Secuencial (orden de inserción)
        boost::multi_index::sequenced<>
    >
> FileContainer;
```

### Alternativa C++23 - Opción 1: Dual-Container Simple

```cpp
#include <map>
#include <vector>

struct FileEntry {
    int id;
    std::string originalName;
    std::string disambiguatedName;
    int priority;
};

class FileContainer {
private:
    std::map<int, FileEntry> m_byId;           // Índice 1
    std::vector<FileEntry> m_insertionOrder;   // Índice 2

public:
    void insert(const FileEntry& entry) {
        m_byId[entry.id] = entry;
        m_insertionOrder.push_back(entry);
    }
    
    auto find(int id) {
        return m_byId.find(id);
    }
    
    auto getInOrder() const {
        return m_insertionOrder;
    }
    
    size_t size() const {
        return m_byId.size();
    }
};
```

### Alternativa C++23 - Opción 2: Contenedor Personalizado (Más Complejo)

```cpp
template<typename T, typename Key>
class DualIndexedContainer {
private:
    std::map<Key, T> m_indexed;
    std::vector<Key> m_order;

public:
    using iterator = std::vector<Key>::iterator;
    using const_iterator = std::vector<Key>::const_iterator;
    
    void insert(const Key& key, const T& value) {
        if (m_indexed.find(key) == m_indexed.end()) {
            m_order.push_back(key);
        }
        m_indexed[key] = value;
    }
    
    auto find(const Key& key) {
        return m_indexed.find(key);
    }
    
    iterator begin() { return m_order.begin(); }
    iterator end() { return m_order.end(); }
    const_iterator begin() const { return m_order.begin(); }
    const_iterator end() const { return m_order.end(); }
    
    size_t size() const { return m_indexed.size(); }
};
```

#### Archivo: filters/page_layout/Settings.cpp

```cpp
// ACTUAL - Más complejos: 3 índices
typedef boost::multi_index_container<
    PageInfo,
    boost::multi_index::indexed_by<
        // Índice 1: Por ID único
        boost::multi_index::ordered_unique<
            boost::multi_index::member<PageInfo, int, &PageInfo::pageId>
        >,
        // Índice 2: Por nombre no único
        boost::multi_index::ordered_non_unique<
            boost::multi_index::member<PageInfo, std::string, &PageInfo::name>
        >,
        // Índice 3: Secuencial
        boost::multi_index::sequenced<>
    >
> PageInfoContainer;
```

### Alternativa C++23 para 3+ Índices

```cpp
class PageInfoContainer {
private:
    std::map<int, PageInfo> m_byId;                    // Índice 1
    std::multimap<std::string, int> m_byName;          // Índice 2
    std::vector<PageInfo> m_insertionOrder;            // Índice 3

public:
    void insert(const PageInfo& page) {
        m_byId[page.pageId] = page;
        m_byName.insert({page.name, page.pageId});
        m_insertionOrder.push_back(page);
    }
    
    auto findById(int id) {
        return m_byId.find(id);
    }
    
    auto findByName(const std::string& name) {
        return m_byName.equal_range(name);
    }
    
    const auto& getInOrder() const {
        return m_insertionOrder;
    }
};
```

---

## 5. INTRUSIVE LIST - Análisis Detallado (6 archivos)

### Archivo: foundation/DynamicPool.h

```cpp
// ACTUAL - Intrusive List para gestión de memoria eficiente
#include <boost/intrusive/list.hpp>

template<typename T>
class DynamicPool {
private:
    struct Node : public boost::intrusive::list_base_hook<> {
        T data;
        bool inUse = false;
    };
    
    typedef boost::intrusive::list<Node> FreeList;
    FreeList m_freeList;
    std::vector<Node*> m_allNodes;
    
public:
    T* allocate() {
        if (!m_freeList.empty()) {
            Node* node = &m_freeList.front();
            m_freeList.pop_front();
            node->inUse = true;
            return &node->data;
        }
        // Allocate new
        Node* newNode = new Node();
        m_allNodes.push_back(newNode);
        newNode->inUse = true;
        return &newNode->data;
    }
    
    void deallocate(T* ptr) {
        // Find node and push to free list
        // ...
        m_freeList.push_back(*node);
    }
};
```

### Alternativa C++23 - Opción 1: std::list

```cpp
template<typename T>
class DynamicPool {
private:
    struct Node {
        T data;
        bool inUse = false;
    };
    
    std::list<Node> m_freeList;
    std::list<Node> m_allNodes;

public:
    T* allocate() {
        if (!m_freeList.empty()) {
            Node& node = m_freeList.front();
            auto it = m_allNodes.begin();
            // Transfer from free to used
            m_freeList.pop_front();
            return &node.data;
        }
        // Allocate new
        m_allNodes.emplace_back();
        return &m_allNodes.back().data;
    }
};
```

### Alternativa C++23 - Opción 2: Implementación Manual (Ultra-Eficiente)

```cpp
template<typename T>
class IntrusiveList {
private:
    struct Node {
        T data;
        Node* prev = nullptr;
        Node* next = nullptr;
    };
    
    Node* m_head = nullptr;
    Node* m_tail = nullptr;
    size_t m_size = 0;

public:
    void push_back(T&& data) {
        Node* newNode = new Node{std::move(data), m_tail, nullptr};
        if (m_tail) {
            m_tail->next = newNode;
        } else {
            m_head = newNode;
        }
        m_tail = newNode;
        ++m_size;
    }
    
    void pop_front() {
        if (m_head) {
            Node* old = m_head;
            m_head = m_head->next;
            if (m_head) m_head->prev = nullptr;
            else m_tail = nullptr;
            delete old;
            --m_size;
        }
    }
    
    // ... rest of implementation
};
```

---

## 6. UNIT TESTS - Análisis Detallado (25 archivos)

### Ejemplos Actuales

#### Archivo: imageproc/tests/TestBinaryImage.cpp

```cpp
// ACTUAL - Boost.Test
#include <boost/test/auto_unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>

BOOST_AUTO_TEST_CASE(TestBinaryImageCreation) {
    BinaryImage img(100, 100);
    BOOST_CHECK_EQUAL(img.width(), 100);
    BOOST_CHECK_EQUAL(img.height(), 100);
}

BOOST_AUTO_TEST_CASE(TestBinaryImagePixels) {
    BinaryImage img(50, 50);
    img.setPixel(10, 10, true);
    BOOST_CHECK(img.pixel(10, 10));
    BOOST_CHECK(!img.pixel(11, 11));
}

BOOST_AUTO_TEST_CASE(TestFloatComparison) {
    double a = 1.0;
    double b = 1.0 + 1e-7;
    BOOST_CHECK_CLOSE(a, b, 0.001);  // Tolerance in %
}
```

### Alternativa C++23 - Google Test

```cpp
// Nuevo - Google Test
#include <gtest/gtest.h>

class BinaryImageTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup code
    }
};

TEST_F(BinaryImageTest, TestBinaryImageCreation) {
    BinaryImage img(100, 100);
    EXPECT_EQ(img.width(), 100);
    EXPECT_EQ(img.height(), 100);
}

TEST_F(BinaryImageTest, TestBinaryImagePixels) {
    BinaryImage img(50, 50);
    img.setPixel(10, 10, true);
    EXPECT_TRUE(img.pixel(10, 10));
    EXPECT_FALSE(img.pixel(11, 11));
}

TEST_F(BinaryImageTest, TestFloatComparison) {
    double a = 1.0;
    double b = 1.0 + 1e-7;
    EXPECT_NEAR(a, b, 1e-6);  // Absolute tolerance
}
```

### Alternativa C++23 - Catch2

```cpp
// Alternativa con Catch2
#include <catch2/catch_all.hpp>

SCENARIO("BinaryImage creation and manipulation") {
    GIVEN("A 100x100 binary image") {
        BinaryImage img(100, 100);
        
        WHEN("checking dimensions") {
            THEN("width should be 100") {
                REQUIRE(img.width() == 100);
            }
            AND_THEN("height should be 100") {
                REQUIRE(img.height() == 100);
            }
        }
    }
    
    GIVEN("A 50x50 binary image") {
        BinaryImage img(50, 50);
        
        WHEN("setting pixel at (10, 10) to true") {
            img.setPixel(10, 10, true);
            
            THEN("pixel at (10, 10) should be true") {
                REQUIRE(img.pixel(10, 10));
            }
            AND_THEN("pixel at (11, 11) should be false") {
                REQUIRE(!img.pixel(11, 11));
            }
        }
    }
}
```

---

## Recomendaciones Específicas por Módulo

### foundation/
- ✅ Eliminar boost/shared_ptr, boost/weak_ptr → std::shared_ptr, std::weak_ptr
- ✅ Eliminar boost/scoped_array → std::unique_ptr
- ✅ Eliminar boost/intrusive/list → std::list (o implementación propia si performance crítica)
- ⚠️ Revisar foundation/gui/QtSignalForwarder.h → usar std::move_only_function

### imageproc/
- ✅ FOREACH → for range-based (78 archivos)
- ✅ Eliminar boost::cstdint → cstdint estándar
- ⚠️ Algunos archivos complejos con boost::lambda

### math/
- ✅ FOREACH loops
- ⚠️ spfit/PolylineModelShape.h usa boost::lambda

### dewarping/
- ✅ FOREACH loops
- ⚠️ **Archivo crítico:** gui/DetectVertContentBounds.cpp (uso intensivo de boost::lambda)
- ⚠️ **Archivo crítico:** gui/TextLineTracer.cpp (boost::lambda::construct)

### interaction/
- ✅ FOREACH loops
- ⚠️ gui/InteractionHandler.cpp usa boost::bind intensivamente

### zones/
- ✅ FOREACH loops
- ⚠️ Intrusive list en ZoneSet.h

### filters/
- ✅ FOREACH loops
- ✅ boost::bind → lambdas
- ⚠️ **Multi-index en page_layout/Settings.cpp**
- ⚠️ **boost::function en varios lugares**

### src/
- ⚠️ **Multi-index containers** (FileNameDisambiguator, ProjectPages, etc.)
- ⚠️ boost::shared_ptr en varios lugares
- ⚠️ boost::weak_ptr en ThumbnailPixmapCache

---

## Checklist de Migración

```
FASE 1: Trivial (Scripts Automáticos)
□ FOREACH → for range-based (78 files)
□ boost::shared_ptr → std::shared_ptr
□ boost::weak_ptr → std::weak_ptr
□ boost::scoped_array → std::unique_ptr
□ boost::array → std::array
□ boost::cstdint → cstdint
□ Remover includes de boost/foreach.hpp
□ Remover includes de boost/shared_ptr.hpp, boost/weak_ptr.hpp

FASE 2: Lambdas y Callbacks (Manual)
□ boost::lambda → lambdas std
□ boost::bind → lambdas capturadas
□ boost::function → std::function / std::move_only_function
□ boost::lambda::construct → lambdas

FASE 3: Estructuras Complejas (Refactorización)
□ Multi-index → Dual-container o personalizado
□ Intrusive lists → std::list o implementación propia
□ Unit tests: Boost.Test → Google Test / Catch2

VERIFICACIÓN
□ Compilación sin errores
□ Todos los tests pasan
□ No warnings adicionales
□ CMakeLists.txt actualizado (remover BOOST)
```
