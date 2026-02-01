# BOOST → C++23 Migration: Side-by-Side Comparison Guide

## Quick Reference Cheat Sheet

### 1. FOREACH LOOPS

**BEFORE:**
```cpp
#include <boost/foreach.hpp>

BOOST_FOREACH(auto element, container) {
    element.doSomething();
}
```

**AFTER:**
```cpp
// No include needed - C++11 feature

for (auto element : container) {
    element.doSomething();
}
```

**Migration Effort**: ⭐ Trivial  
**Automation**: ✅ 100% (script available)  
**Files**: 78  

---

### 2. RANGE-BASED FOR WITH MODIFICATIONS

**BEFORE:**
```cpp
BOOST_FOREACH(auto& element, container) {
    element.modify();
}
```

**AFTER:**
```cpp
for (auto& element : container) {
    element.modify();
}
```

---

### 3. MAP/DICT ITERATION (Structured Bindings)

**BEFORE:**
```cpp
BOOST_FOREACH(auto pair, myMap) {
    auto key = pair.first;
    auto value = pair.second;
    processKeyValue(key, value);
}
```

**AFTER (C++17+):**
```cpp
for (auto& [key, value] : myMap) {
    processKeyValue(key, value);
}
```

---

## SMART POINTERS

### Shared Pointers

**BEFORE:**
```cpp
#include <boost/shared_ptr.hpp>

boost::shared_ptr<MyClass> ptr(new MyClass());
boost::shared_ptr<MyClass> ptr2 = ptr;
```

**AFTER:**
```cpp
#include <memory>

auto ptr = std::make_shared<MyClass>();
std::shared_ptr<MyClass> ptr2 = ptr;
```

**Migration Effort**: ⭐ Trivial  
**Automation**: ✅ 100% (script available)  
**Files**: 3  

---

### Weak Pointers

**BEFORE:**
```cpp
#include <boost/weak_ptr.hpp>

boost::shared_ptr<MyClass> shared = ...;
boost::weak_ptr<MyClass> weak = shared;

if (auto locked = weak.lock()) {
    locked->doSomething();
}
```

**AFTER:**
```cpp
#include <memory>

std::shared_ptr<MyClass> shared = ...;
std::weak_ptr<MyClass> weak = shared;

if (auto locked = weak.lock()) {
    locked->doSomething();
}
```

---

### Scoped Arrays

**BEFORE:**
```cpp
#include <boost/scoped_array.hpp>

boost::scoped_array<int> buffer(new int[1024]);
buffer[0] = 42;
// Automatic cleanup on scope exit
```

**AFTER (Option 1 - Recommended):**
```cpp
#include <memory>

auto buffer = std::make_unique<int[]>(1024);
buffer[0] = 42;
// Automatic cleanup on scope exit
```

**AFTER (Option 2 - Alternative):**
```cpp
#include <vector>

std::vector<int> buffer(1024);
buffer[0] = 42;
// More flexible, automatic cleanup
```

---

## CONTAINERS

### Boost Array

**BEFORE:**
```cpp
#include <boost/array.hpp>

boost::array<int, 5> arr = {{1, 2, 3, 4, 5}};
```

**AFTER:**
```cpp
#include <array>

std::array<int, 5> arr = {1, 2, 3, 4, 5};
```

**Migration Effort**: ⭐ Trivial  
**Automation**: ✅ 100% (script available)  
**Files**: 2  

---

### Multi-Index Container

**BEFORE (Complex):**
```cpp
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/sequenced_index.hpp>
#include <boost/multi_index/member.hpp>

struct Item {
    int id;
    std::string name;
    int priority;
};

typedef boost::multi_index_container<
    Item,
    boost::multi_index::indexed_by<
        boost::multi_index::ordered_unique<
            boost::multi_index::member<Item, int, &Item::id>
        >,
        boost::multi_index::sequenced<>
    >
> ItemContainer;

// Usage
ItemContainer container;
container.insert(Item{1, "name", 10});

// Find by ID
auto it = container.get<0>().find(1);

// Iterate in insertion order
for (auto& item : container.get<1>()) {
    std::cout << item.name << "\n";
}
```

**AFTER (Dual Container):**
```cpp
#include <map>
#include <vector>

struct Item {
    int id;
    std::string name;
    int priority;
};

class ItemContainer {
private:
    std::map<int, Item> m_byId;        // Index by ID
    std::vector<Item> m_insertOrder;   // Insertion order

public:
    void insert(const Item& item) {
        m_byId[item.id] = item;
        m_insertOrder.push_back(item);
    }
    
    auto findById(int id) {
        return m_byId.find(id);
    }
    
    auto& getInOrder() {
        return m_insertOrder;
    }
};

// Usage
ItemContainer container;
container.insert(Item{1, "name", 10});

// Find by ID
auto it = container.findById(1);

// Iterate in insertion order
for (auto& item : container.getInOrder()) {
    std::cout << item.name << "\n";
}
```

**Migration Effort**: ⭐⭐⭐⭐ Hard  
**Automation**: ❌ Manual refactoring required  
**Files**: 7  

---

## FUNCTION OBJECTS

### Boost Function

**BEFORE:**
```cpp
#include <boost/function.hpp>

typedef boost::function<int(double, const std::string&)> Callback;

void registerCallback(const Callback& cb) {
    int result = cb(3.14, "hello");
}

// Usage
registerCallback([](double d, const std::string& s) -> int {
    return d * s.length();
});
```

**AFTER (Option 1 - Std Function):**
```cpp
#include <functional>

typedef std::function<int(double, const std::string&)> Callback;

void registerCallback(const Callback& cb) {
    int result = cb(3.14, "hello");
}

// Usage remains the same
registerCallback([](double d, const std::string& s) -> int {
    return d * s.length();
});
```

**AFTER (Option 2 - Move Only Function, C++23):**
```cpp
#include <functional>

typedef std::move_only_function<int(double, const std::string&)> Callback;

void registerCallback(Callback cb) {
    int result = cb(3.14, "hello");
}

// Usage with move
registerCallback([](double d, const std::string& s) -> int {
    return d * s.length();
});
```

**Migration Effort**: ⭐⭐ Low  
**Automation**: ✅ Mostly (rename and add #include)  
**Files**: 18  

---

## LAMBDA PATTERNS

### Boost Lambda Library

**BEFORE (Simple predicate):**
```cpp
#include <boost/lambda/lambda.hpp>

namespace ll = boost::lambda;

auto it = std::find_if(
    vec.begin(), vec.end(),
    ll::_1 > 10
);
```

**AFTER:**
```cpp
auto it = std::find_if(
    vec.begin(), vec.end(),
    [](int x) { return x > 10; }
);
```

---

**BEFORE (Complex predicate):**
```cpp
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/if.hpp>

namespace ll = boost::lambda;

std::transform(
    input.begin(), input.end(),
    std::back_inserter(output),
    ll::if_(ll::_1 > 100)[ll::_1 * 2].else_[0]
);
```

**AFTER:**
```cpp
std::transform(
    input.begin(), input.end(),
    std::back_inserter(output),
    [](int x) { return x > 100 ? x * 2 : 0; }
);
```

---

### Boost Bind

**BEFORE (Method binding):**
```cpp
#include <boost/bind.hpp>

class MyClass {
    void handler(const Event& e) { ... }
};

MyClass obj;
auto callback = boost::bind(&MyClass::handler, &obj, _1);
callback(event);
```

**AFTER (Option 1 - Lambda capture):**
```cpp
MyClass obj;
auto callback = [&obj](const Event& e) { 
    obj.handler(e); 
};
callback(event);
```

**AFTER (Option 2 - Std bind, backward compatible):**
```cpp
#include <functional>

MyClass obj;
auto callback = std::bind(
    &MyClass::handler, &obj, std::placeholders::_1
);
callback(event);
```

**Migration Effort**: ⭐⭐ Low  
**Automation**: ⚠️ Partial (requires careful capture review)  
**Files**: 24+15  

---

## LAMBDA CONSTRUCTS

**BEFORE:**
```cpp
#include <boost/lambda/construct.hpp>

namespace ll = boost::lambda;

std::transform(
    sourceList.begin(), sourceList.end(),
    std::back_inserter(resultList),
    ll::construct<MyClass>(ll::_1, ll::_2)
);
```

**AFTER:**
```cpp
std::transform(
    sourceList.begin(), sourceList.end(),
    std::back_inserter(resultList),
    [](const auto& src1, const auto& src2) {
        return MyClass(src1, src2);
    }
);
```

---

## STANDARD TYPES

### Boost CStdInt

**BEFORE:**
```cpp
#include <boost/cstdint.hpp>

boost::uint8_t byte = 255;
boost::int32_t count = -1;
boost::uint64_t bigNum = 1000000000000;
```

**AFTER:**
```cpp
#include <cstdint>

uint8_t byte = 255;
int32_t count = -1;
uint64_t bigNum = 1000000000000;
```

**Migration Effort**: ⭐ Trivial  
**Automation**: ✅ 100% (script available)  
**Files**: 2  

---

### Type Traits

**BEFORE:**
```cpp
#include <boost/type_traits/alignment_of.hpp>

size_t align = boost::alignment_of<MyClass>::value;
```

**AFTER (C++11+):**
```cpp
#include <type_traits>

size_t align = alignof(MyClass);
```

**OR (C++17+):**
```cpp
#include <type_traits>

size_t align = std::alignment_of_v<MyClass>;
```

---

## TESTING

### Boost Test

**BEFORE:**
```cpp
#include <boost/test/auto_unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>

BOOST_AUTO_TEST_CASE(TestMyFeature) {
    int result = compute();
    BOOST_CHECK_EQUAL(result, 42);
    BOOST_CHECK_CLOSE(3.14, PI, 0.001);  // tolerance in %
}
```

**AFTER (Google Test):**
```cpp
#include <gtest/gtest.h>

class MyFeatureTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup
    }
};

TEST_F(MyFeatureTest, ComputeReturns42) {
    int result = compute();
    EXPECT_EQ(result, 42);
    EXPECT_NEAR(3.14, PI, 0.001);  // absolute tolerance
}
```

**AFTER (Catch2):**
```cpp
#include <catch2/catch_all.hpp>

TEST_CASE("Compute returns 42") {
    int result = compute();
    REQUIRE(result == 42);
    REQUIRE_THAT(3.14, Catch::Matchers::WithinAbs(PI, 0.001));
}
```

**Migration Effort**: ⭐⭐⭐ Medium  
**Automation**: ❌ Manual refactoring required  
**Files**: 25  

---

## QUICK DECISION TREE

```
"I found a BOOST include..."

├─ ...#include <boost/foreach.hpp>
│  └─ FOREACH → for (auto x : container)
│     Migration: Script (phase1.sh)
│
├─ ...#include <boost/shared_ptr.hpp>
│  └─ boost::shared_ptr → std::shared_ptr
│     Migration: Script (phase1.sh)
│
├─ ...#include <boost/weak_ptr.hpp>
│  └─ boost::weak_ptr → std::weak_ptr
│     Migration: Script (phase1.sh)
│
├─ ...#include <boost/scoped_array.hpp>
│  └─ boost::scoped_array → std::unique_ptr or std::vector
│     Migration: Script (phase1.sh)
│
├─ ...#include <boost/array.hpp>
│  └─ boost::array → std::array
│     Migration: Script (phase1.sh)
│
├─ ...#include <boost/cstdint.hpp>
│  └─ boost::uint* → uint*
│     Migration: Script (phase1.sh)
│
├─ ...#include <boost/function.hpp>
│  └─ boost::function → std::function
│     Migration: Manual (phase2)
│
├─ ...#include <boost/bind.hpp>
│  └─ boost::bind → lambda captures
│     Migration: Manual (phase2)
│
├─ ...#include <boost/lambda/*.hpp>
│  └─ boost::lambda expressions → std lambdas
│     Migration: Manual (phase2)
│
├─ ...#include <boost/multi_index*.hpp>
│  └─ multi_index → dual containers or custom
│     Migration: Refactor (phase3)
│
├─ ...#include <boost/intrusive/list.hpp>
│  └─ intrusive → std::list or custom
│     Migration: Refactor (phase3)
│
└─ ...#include <boost/test/*.hpp>
   └─ BOOST_AUTO_TEST → TEST with Google Test
      Migration: Refactor (phase3)
```

---

## Performance Implications

| Change | Performance | Notes |
|--------|-------------|-------|
| FOREACH → for | Identical | No difference |
| shared_ptr → shared_ptr | Identical | Same implementation |
| scoped_array → unique_ptr | Identical | Same overhead |
| function → function | Same | std implementation |
| lambda → lambda | Better | Less indirection |
| bind → lambda captures | Better | Direct invocation |

**Summary**: ✅ No performance degradation, some improvements possible

---

## Compatibility Notes

- C++11 minimum requirement for most changes
- C++17 for structured bindings (maps)
- C++23 for std::move_only_function
- All changes backward compatible within C++ versions

---

## File-Specific Migration Map

| File | Current BOOST | Replacement | Difficulty |
|------|---------------|-------------|-----------|
| compat/boost_multi_index_foreach_fix.h | FOREACH, mpl::bool | Delete file | Easy |
| foundation/DynamicPool.h | intrusive::list | std::list | Medium |
| foundation/FastQueue.h | intrusive::list | std::list | Medium |
| foundation/gui/QtSignalForwarder.h | boost::function | std::function | Easy |
| imageproc/*.cpp | FOREACH | for loops | Easy |
| math/spfit/*.cpp | lambda | lambdas | Medium |
| dewarping/gui/*.cpp | lambda, bind | lambdas | Medium |
| filters/page_layout/Settings.cpp | multi_index | dual containers | Hard |
| src/FileNameDisambiguator.cpp | multi_index | dual containers | Hard |
| *.tests.cpp | BOOST.Test | Google Test | Medium |

---

## Testing Checklist

After migration of each file:

- [ ] Compiles without warnings
- [ ] Original tests pass
- [ ] No new compiler warnings
- [ ] Code review approved
- [ ] Performance benchmarks OK (if applicable)
- [ ] Committed with clear message

---

**Last Updated**: 2026-02-01
