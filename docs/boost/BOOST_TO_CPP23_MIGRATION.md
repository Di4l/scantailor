# Migración de BOOST a C++23: Análisis y Plan de Sustitución

## Resumen Ejecutivo

El proyecto ScanTailor usa **26 librerías diferentes de BOOST** con **258 includes** distribuidos en **~100+ archivos**.

**Buena noticia:** Con C++23 y STL moderna, **TODAS las dependencias de BOOST pueden eliminarse**.

| Categoría | Archivos Afectados | Complejidad | Impacto |
|-----------|-------------------|------------|--------|
| **FOREACH loops** | 78 | BAJA | Reemplazar por `for (auto&)` |
| **Lambda/Bind** | 28+24 | BAJA | Usar `std::function` y lambdas modernas |
| **Smart Pointers** | 5 | BAJA | Ya en `std::` (C++11) |
| **Multi-Index** | 7 | MEDIA | Refactorizar con `std::set`/`std::map` |
| **Unit Tests** | 25 | BAJA | Cambiar a framework moderno |
| **Otros (Array, Cstdint, etc)** | ~50 | BAJA | STL modernos equivalentes |

---

## 1. FOREACH LOOPS (78 archivos) - PRIORIDAD CRÍTICA

### Uso Actual
```cpp
#include <boost/foreach.hpp>

BOOST_FOREACH(auto& element, container) {
    element.process();
}
```

### Alternativa C++23
```cpp
for (auto& element : container) {
    element.process();
}
```

### Cambios Necesarios
- **Archivos a actualizar:** 78
- **Complejidad:** MUY BAJA
- **Esfuerzo:** 2-3 horas con script

### Script de Migración
```bash
# Reemplazar BOOST_FOREACH por for range-based
find . -type f \( -name "*.cpp" -o -name "*.h" \) ! -path "*/build/*" | xargs sed -i \
  's/BOOST_FOREACH(\([^,]*\),\s*\([^)]*\))/for (\1 : \2)/g'

# Remover include de boost/foreach.hpp
find . -type f \( -name "*.cpp" -o -name "*.h" \) ! -path "*/build/*" | xargs sed -i \
  '/#include <boost\/foreach.hpp>/d'
```

### Archivos Principales Afectados
```
dewarping/*, filters/*, imageproc/*, math/*, src/*, zones/*
```

---

## 2. LAMBDA/BIND (24+28 = 52 archivos) - PRIORIDAD ALTA

### 2.1 Boost Lambda Library

**Uso Actual:**
```cpp
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>

std::vector<int> v = {1, 2, 3, 4, 5};
std::find_if(v.begin(), v.end(), 
    ll::_1 > 3);

std::for_each(v.begin(), v.end(),
    ll::_1 = ll::_2);
```

**Alternativa C++23:**
```cpp
std::vector<int> v = {1, 2, 3, 4, 5};

// Opción 1: Lambda tradicional
std::find_if(v.begin(), v.end(), 
    [](int x) { return x > 3; });

// Opción 2: Comparador nombrado (para operaciones complejas)
auto comp = [](int x) { return x > 3; };
std::find_if(v.begin(), v.end(), comp);

// Opción 3: std::bind (C++11) - último recurso
std::bind(std::greater<>(), std::placeholders::_1, 3)
```

**Archivos más complejos:**
- `dewarping/gui/DetectVertContentBounds.cpp` (uso intensivo)
- `dewarping/gui/TextLineTracer.cpp`
- `dewarping/gui/TowardsLineTracer.cpp`

### 2.2 Boost Bind Library

**Uso Actual:**
```cpp
#include <boost/bind.hpp>

boost::bind(&ClassName::method, &obj, _1)
```

**Alternativa C++23:**
```cpp
// Opción 1: Lambda con captura
[&obj](auto param) { return obj.method(param); }

// Opción 2: std::bind (C++11)
std::bind(&ClassName::method, &obj, std::placeholders::_1)

// Opción 3: Función miembro explícita
[&obj, param]() { return obj.method(param); }
```

### 2.3 Lambda Construct

**Uso Actual (raros casos):**
```cpp
#include <boost/lambda/construct.hpp>

std::transform(input.begin(), input.end(),
    std::back_inserter(output),
    ll::construct<MyClass>(ll::_1));
```

**Alternativa C++23:**
```cpp
std::transform(input.begin(), input.end(),
    std::back_inserter(output),
    [](const auto& x) { return MyClass(x); });
```

---

## 3. BOOST FUNCTION (18 archivos) - PRIORIDAD MEDIA

### Uso Actual
```cpp
#include <boost/function.hpp>

typedef boost::function<void(int, const std::string&)> Callback;

void registerCallback(const Callback& cb) {
    cb(42, "hello");
}
```

### Alternativa C++23
```cpp
// Opción 1: std::function (C++11, disponible desde siempre)
std::function<void(int, const std::string&)> callback;

// Opción 2: Templates genéricos (mejor rendimiento)
template<typename F>
void registerCallback(F&& cb) {
    cb(42, "hello");
}

// Opción 3: std::move_only_function (C++23) - mejor que std::function
std::move_only_function<void(int, const std::string&)> callback;
```

### Cambio Sencillo
```cpp
// Reemplazar boost::function por std::function
#include <functional>

typedef std::function<void(int, const std::string&)> Callback;
```

### Archivos Principales
- `foundation/gui/QtSignalForwarder.h`
- `filters/output/OutputGenerator.h`
- `filters/output/FillZoneEditor.h`

---

## 4. SMART POINTERS (5 archivos) - PRIORIDAD BAJA

### Uso Actual
```cpp
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

boost::shared_ptr<MyClass> ptr(new MyClass());
boost::weak_ptr<MyClass> weak = ptr;
```

### Alternativa C++23 (Ya C++11)
```cpp
#include <memory>

std::shared_ptr<MyClass> ptr = std::make_shared<MyClass>();
std::weak_ptr<MyClass> weak = ptr;
```

### Cambios Necesarios
```bash
# Reemplazar includes
sed -i 's/#include <boost\/shared_ptr.hpp>/#include <memory>/g' *.h
sed -i 's/#include <boost\/weak_ptr.hpp>/#include <memory>/g' *.h

# Reemplazar boost:: por std::
sed -i 's/boost::shared_ptr/std::shared_ptr/g' *.h *.cpp
sed -i 's/boost::weak_ptr/std::weak_ptr/g' *.h *.cpp
```

### Archivos Afectados
- `filters/output/OutputGenerator.cpp`
- `filters/output/Task.cpp`
- `src/ThumbnailBase.h`

---

## 5. MULTI-INDEX CONTAINER (7-8 archivos) - PRIORIDAD MEDIA-ALTA

### Uso Actual
```cpp
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/sequenced_index.hpp>
#include <boost/multi_index/member.hpp>

struct MyData {
    int id;
    std::string name;
    int priority;
};

typedef boost::multi_index_container<
    MyData,
    boost::multi_index::indexed_by<
        boost::multi_index::ordered_unique<
            boost::multi_index::member<MyData, int, &MyData::id>>,
        boost::multi_index::sequenced<>
    >
> MyDataContainer;
```

### Alternativa C++23

**Opción 1: Múltiples contenedores indexados (Recomendado para migraciones simples)**
```cpp
// Usar combinación de std::set + std::vector para mantener orden de inserción
std::set<MyData> byId;      // Índice por ID
std::vector<MyData> byOrder; // Índice por orden de inserción

// Alternativa con std::map + std::vector
std::map<int, MyData> byId;
std::vector<int> insertionOrder; // IDs en orden
```

**Opción 2: Contenedor personalizado (Para casos complejos)**
```cpp
class IndexedDataContainer {
private:
    std::map<int, MyData> m_byId;
    std::vector<MyData> m_byOrder;
    
public:
    void insert(const MyData& data) {
        m_byId[data.id] = data;
        m_byOrder.push_back(data);
    }
    
    auto findById(int id) { return m_byId.find(id); }
    auto getOrdered() { return m_byOrder; }
};
```

**Opción 3: Librerías externas modernas (C++20/23)**
- **Abseil** (Google): `absl::btree_map`, `absl::flat_hash_map`
- **Boost ModernC++**: Futuro de Boost

### Archivos Afectados
```
src/FileNameDisambiguator.cpp
src/ProjectPages.cpp
src/RelinkingModel.cpp
src/ThumbnailSequence.cpp
filters/page_layout/Settings.cpp
```

---

## 6. INTRUSIVE LISTS (6 archivos) - PRIORIDAD MEDIA

### Uso Actual
```cpp
#include <boost/intrusive/list.hpp>

struct Node : public boost::intrusive::list_base_hook<> {
    int data;
};

typedef boost::intrusive::list<Node> NodeList;
```

### Alternativa C++23

**Opción 1: std::list (más simple)**
```cpp
struct Node {
    int data;
};

std::list<Node> nodeList;
```

**Opción 2: Mantener Intrusive pero con implementación propia (raro)**
```cpp
// Para casos de ultra-alto rendimiento
// Implementar estructura doblemente enlazada propia

template<typename T>
class IntrusiveList {
private:
    struct Node {
        T value;
        Node* prev = nullptr;
        Node* next = nullptr;
    };
    
    Node* m_head = nullptr;
public:
    // Implementar operaciones
};
```

### Archivos Afectados
```
foundation/DynamicPool.h
foundation/FastQueue.h
interaction/InteractionState.h
```

---

## 7. SCOPED ARRAY (10 archivos) - PRIORIDAD BAJA

### Uso Actual
```cpp
#include <boost/scoped_array.hpp>

boost::scoped_array<int> buffer(new int[1024]);
```

### Alternativa C++23
```cpp
// Opción 1: std::unique_ptr (C++11)
std::unique_ptr<int[]> buffer(new int[1024]);
// o
auto buffer = std::make_unique<int[]>(1024);

// Opción 2: std::array (para tamaño conocido en compilación)
std::array<int, 1024> buffer;

// Opción 3: std::vector (más flexible)
std::vector<int> buffer(1024);
```

### Cambios Sencillos
```bash
# Reemplazar scoped_array por unique_ptr
sed -i 's/boost::scoped_array<\(.*\)>/std::unique_ptr<\1[]>/g' *.h *.cpp
```

---

## 8. BOOST ARRAY (2 archivos) - PRIORIDAD BAJA

### Uso Actual
```cpp
#include <boost/array.hpp>

boost::array<int, 5> arr = {{1, 2, 3, 4, 5}};
```

### Alternativa C++23
```cpp
// Opción 1: std::array (C++11)
std::array<int, 5> arr = {{1, 2, 3, 4, 5}};

// Opción 2: Initializer list
std::array<int, 5> arr{1, 2, 3, 4, 5};
```

---

## 9. BOOST CSTDINT (2 archivos) - PRIORIDAD BAJA

### Uso Actual
```cpp
#include <boost/cstdint.hpp>

boost::uint8_t value;
boost::int32_t count;
```

### Alternativa C++23
```cpp
#include <cstdint>

uint8_t value;
int32_t count;
```

---

## 10. UNIT TESTS (25 archivos) - PRIORIDAD MEDIA

### Uso Actual
```cpp
#include <boost/test/auto_unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>

BOOST_AUTO_TEST_CASE(MyTest) {
    BOOST_CHECK(condition);
    BOOST_CHECK_CLOSE(a, b, tolerance);
}
```

### Alternativas C++23

**Opción 1: Google Test (GTest) - Recomendado**
```cpp
#include <gtest/gtest.h>

TEST(MyTestSuite, MyTest) {
    EXPECT_TRUE(condition);
    EXPECT_NEAR(a, b, tolerance);
}
```

**Opción 2: Catch2 (Moderno, C++17+)**
```cpp
#include <catch2/catch.hpp>

TEST_CASE("MyTestSuite::MyTest") {
    REQUIRE(condition);
    REQUIRE_THAT(a, Catch::Matchers::WithinRel(b, 0.001));
}
```

**Opción 3: doctest (Ultra-ligero)**
```cpp
#include <doctest/doctest.h>

TEST_CASE("MyTestSuite::MyTest") {
    CHECK(condition);
    CHECK_EQ(a, b);
}
```

---

## 11. OTROS (Type Traits, MPL, Iterator Facade, Ref) - PRIORIDAD BAJA

### Type Traits
```cpp
// Antiguo
#include <boost/type_traits/alignment_of.hpp>
size_t align = boost::alignment_of<MyClass>::value;

// Nuevo
#include <type_traits>
size_t align = alignof(MyClass);
```

### MPL Bool
```cpp
// Antiguo
#include <boost/mpl/bool.hpp>
typedef boost::mpl::bool_<true> MyBool;

// Nuevo (C++23)
// Usar std::bool_constant o std::true_type
using MyBool = std::bool_constant<true>;
```

### Iterator Facade
```cpp
// Antiguo
#include <boost/iterator/iterator_facade.hpp>
class MyIterator : public boost::iterator_facade<...> { };

// Nuevo (C++20)
// Usar std::iterator_facade (si disponible) o escribir iterator manualmente
class MyIterator {
    // Implementar operator++, operator*, etc
};
```

### Ref
```cpp
// Antiguo
#include <boost/ref.hpp>
boost::ref(obj)

// Nuevo
// Usar referencias directas o std::reference_wrapper
std::reference_wrapper<MyClass> ref(obj);
```

---

## Plan de Migración Recomendado

### Fase 1: Migraciones Triviales (1-2 días)
1. **FOREACH loops** (78 archivos) - Script automático
2. **Smart pointers** (5 archivos) - Reemplazos directos
3. **Scoped Array** (10 archivos) - Reemplazos directos
4. **Boost Array** (2 archivos) - Reemplazos directos
5. **Cstdint** (2 archivos) - Reemplazos directos

**Script Total:**
```bash
#!/bin/bash

# 1. FOREACH -> for range-based
find . -type f \( -name "*.cpp" -o -name "*.h" \) ! -path "*/build/*" | xargs sed -i \
  's/BOOST_FOREACH(\([^,]*\),\s*\([^)]*\))/for (\1 : \2)/g'

# 2. Remove boost/foreach.hpp
find . -type f \( -name "*.cpp" -o -name "*.h" \) ! -path "*/build/*" | xargs sed -i \
  '/#include <boost\/foreach.hpp>/d'

# 3. Smart pointers
find . -type f \( -name "*.cpp" -o -name "*.h" \) ! -path "*/build/*" | xargs sed -i \
  's/#include <boost\/shared_ptr.hpp>/#include <memory>/g'
find . -type f \( -name "*.cpp" -o -name "*.h" \) ! -path "*/build/*" | xargs sed -i \
  's/#include <boost\/weak_ptr.hpp>/#include <memory>/g'
find . -type f \( -name "*.cpp" -o -name "*.h" \) ! -path "*/build/*" | xargs sed -i \
  's/boost::shared_ptr/std::shared_ptr/g'
find . -type f \( -name "*.cpp" -o -name "*.h" \) ! -path "*/build/*" | xargs sed -i \
  's/boost::weak_ptr/std::weak_ptr/g'

# 4. Scoped array
find . -type f \( -name "*.cpp" -o -name "*.h" \) ! -path "*/build/*" | xargs sed -i \
  's/#include <boost\/scoped_array.hpp>/#include <memory>/g'
find . -type f \( -name "*.cpp" -o -name "*.h" \) ! -path "*/build/*" | xargs sed -i \
  's/boost::scoped_array</std::unique_ptr</g'

# 5. Array
find . -type f \( -name "*.cpp" -o -name "*.h" \) ! -path "*/build/*" | xargs sed -i \
  's/#include <boost\/array.hpp>/#include <array>/g'
find . -type f \( -name "*.cpp" -o -name "*.h" \) ! -path "*/build/*" | xargs sed -i \
  's/boost::array/std::array/g'

# 6. Cstdint
find . -type f \( -name "*.cpp" -o -name "*.h" \) ! -path "*/build/*" | xargs sed -i \
  's/#include <boost\/cstdint.hpp>/#include <cstdint>/g'
```

### Fase 2: Lambdas y Bind (2-3 días)
1. Reemplazar `boost::lambda::lambda` por lambdas std modernas
2. Reemplazar `boost::bind` por lambdas capturadas
3. Reemplazar `boost::function` por `std::function`

### Fase 3: Migraciones Complejas (3-5 días)
1. **Multi-Index Containers** → Refactorizar con `std::map`+`std::vector`
2. **Intrusive Lists** → `std::list` o implementación propia
3. **Unit Tests** → Migrar a Google Test o Catch2

---

## Impacto de la Migración

### Positivos
✅ Eliminación de dependencia externa  
✅ Mejor compatibilidad con compiladores modernos  
✅ Código más readable (lambdas vs boost::lambda)  
✅ Mejor rendimiento en algunos casos  
✅ Facilita futura migración a wxWidgets  

### Riesgos Potenciales
⚠️ Cambios en semántica de lambdas (captura)  
⚠️ Multi-Index refactorización requiere pruebas exhaustivas  
⚠️ Unit test framework change requiere actualizar CI/CD  

---

## Estimación de Esfuerzo

| Fase | Tarea | Horas | Complejidad |
|------|-------|-------|------------|
| 1 | FOREACH, Smart Ptrs, Arrays, etc | 2-4 | BAJA |
| 2 | Lambdas y Bind | 8-12 | MEDIA |
| 3 | Multi-Index y Tests | 12-20 | ALTA |
| **Total** | **Migración Completa** | **~30-36h** | **MEDIA** |

---

## Recomendación Final

**Proceder de inmediato con Fase 1 (scripts automáticos)**, luego:
1. Compilar y verificar que no hay errores triviales
2. Proceder con Fase 2 manualmente (más cuidado necesario)
3. Fase 3 requiere revisión exhaustiva y pruebas

Con C++23, el proyecto será **100% free de BOOST**, lo que simplificará significativamente la migración a wxWidgets.
