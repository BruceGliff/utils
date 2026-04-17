# utils -- Library with indispensable utils, ranges and views

## Table of Contents

<!--toc-->
- [Table of Contents](#table-of-contents)
- [Includes](#includes)
- [ctmap - compile time map](#ctmap---compile-time-map)
    * [Overview](#overview)
    * [Core Features](#core-features)
    * [Usage Examples](#usage-examples)
- [flat_view - Recursive Flattening View](#flat-view---recursive-flattening-view)
    * [Overview](#overview-1)
    * [Core Features](#core-features-1)
    * [Usage Examples](#usage-examples-1)
    * [Advanced Usage](#advanced-usage)
    * [Limitations](#limitations)
- [Requirements](#requirements)
- [Installation](#installation)
- [Code of Conduct](#code-of-conduct)
- [Contributing](#contributing)

<!-- tocstop -->

## Includes

```cpp
#include "bgf/utils/utils.hpp" // Includes features.

#include "bgf/utils/map.hpp" // Includes maps.
#include "bgf/utils/ranges.hpp" // Includes ranges.

#include "bgf/utils/map/ctmap.hpp" // Includes ctmap.

#include "bgf/utils/ranges/flat.hpp" // Includes flat_view.
```

## ctmap - compile time map

### Overview  

`bgf::ctmap` is a **compile‑time bidirectional map** that let you associate:

* **Types -- Types**  
* **Values -- Types**  
* **Types -- Values**  
* **Values -- Values**

All mappings are resolved at compile time, enabling zero‑runtime‑overhead look‑ups and static assertions. The library works with primitive types, user‑defined structs, and even enum values, making it a versatile tool for metaprogramming tasks such as tag dispatch, type‑based configuration, and static reflection.

### Core Features  

| Feature | Description | Example (from tests) |
|---------|-------------|----------------------|
| **Type‑to‑Type mapping** (`t2t`) | Retrieve the counterpart type for a given source type. | `static_assert(std::is_same_v<m::t2t<int>, float>);` |
| **Value‑to‑Type mapping** (`v2t`) | Get the type associated with a compile‑time constant (including NTTPs). | `static_assert(std::is_same_v<m::v2t<4>, double>);` |
| **Type‑to‑Value mapping** (`t2v`) | Find the compile‑time value linked to a type. | `static_assert(m::t2v<double> == 4);` |
| **Value‑to‑Value mapping** (`v2v`) | Translate one compile‑time value to another. | `static_assert(m::v2v<5> == 4);` |
| **Enum support** | Works with plain enums, enum classes, and mixed enum‑type mappings. | `static_assert(m::t2t<e_a> == e_b);` |
| **Bidirectional consistency** | Each mapping is invertible – the reverse lookup yields the original key. | `static_assert(m::v2v<e_c::a> == e_c::b);` |
| **Custom non‑type template parameters** | Allows NTTPs of user‑defined types (e.g., structs). | `static_assert(std::is_same_v<m::v2t<nttp{1}>, nttp>);` |
| **Static verification** | All look‑ups are verified with `static_assert`, ensuring compile‑time correctness. | See the test file `map_test.cpp` for full coverage. |

---  

### Usage Examples

```cpp
using myMap = bgf::ctmap<
    bgf::t2t<int, float>,
    bgf::v2t<42, std::string>,
    bgf::v2v<'x', 'y'>>;

// Type -- Type
static_assert(std::is_same_v<myMap::t2t<int>, float>);
static_assert(std::is_same_v<myMap::t2t<float>, int>);

// Value -- Type
static_assert(std::is_same_v<myMap::v2t<42>, std::string>);

// Type -- Value
static_assert(myMap::t2v<std::string> == 42);

// Value -- Value
static_assert(myMap::v2v<'x'> == 'y');
static_assert(myMap::v2v<'y'> == 'x');

```

---  

For more detailed usage, refer to the test file `map_test.cpp`.

## flat_view - Recursive Flattening View

### Overview

`bgf::flat_view` is a C++20 header-only library that provides **recursive flattening views** for nested containers. It allows you to traverse deeply nested data structures or ranges as if they were flat ranges, with compile-time control over flattening depth and target types.
This is ideal for working with complex, nested data structures where you need to process all elements regardless of their depth in the hierarchy.

Header:
```cpp
#include "bgf/utils/ranges/flat.hpp"
```

### Core Features

#### 1. **Basic Flattening**
Flatten any nested container to its underlying scalar type:

```cpp
std::vector<std::set<int>> vec{{0, 1}, {2}, {}, {3}, {5, 4}};
auto flat = vec | bgf::views::flat;
// Result: {0, 1, 2, 3, 4, 5}
```

#### 2. **Scalar Support**
Even scalars can be treated as single-element ranges:

```cpp
auto flat = 5 | bgf::views::flat;
// Result: {5}
```

#### 3. **Depth Control**
Flatten to a specific nesting level using `depth<N>`:

```cpp
using vec3 = std::vector<std::vector<std::set<int>>>;
vec3 data;

// Flatten to scalars (depth 3)
auto level3 = data | bgf::views::flat.depth<3>;

// Flatten to sets (depth 2)
auto level2 = data | bgf::views::flat.depth<2>;

// Flatten to vector of sets (depth 1)
auto level1 = data | bgf::views::flat.depth<1>;

// No flattening (depth 0)
auto level0 = data | bgf::views::flat.depth<0>;
```

#### 4. **Type-Based Flattening**
Flatten until reaching a specific container type using `nested<T>`:

```cpp
std::vector<std::vector<int>> nested_vec;

// Flatten until reaching std::vector<int>
auto result = nested_vec | bgf::views::flat.nested<std::vector<int>>;
// Equivalent to depth<1>

// Flatten until reaching int
auto result2 = nested_vec | bgf::views::flat.nested<int>;
// Equivalent to depth<2>
```

### Usage Examples

#### Simple Flattening

```cpp
#include "bgf/utils/ranges.hpp"

std::vector<std::set<int>> data{{1, 2, 3}, {4, 5}, {}, {6}};

// Basic flattening
for (int value : bgf::flat_view(data)) {
    // Accesses: 1, 2, 3, 4, 5, 6
}

// With pipe syntax
auto flattened = data | bgf::views::flat;
```

#### Complex Nesting

```cpp
// Deeply nested structure
std::vector<std::vector<std::set<int>>> deep_data{
    {{1, 2}, {3}},
    {{4}, {5, 6, 7}},
    {}
};

// Full flattening to ints
for (int val : deep_data | bgf::views::flat) {
    // Accesses: 1, 2, 3, 4, 5, 6, 7
}

// Partial flattening to sets
for (const auto& set : deep_data | bgf::views::flat.depth<2>) {
    // Accesses each std::set<int>
}
```

#### Mixed Container Types

```cpp
// Works with various STL containers
std::vector<std::map<int, std::string>> maps{
    {{1, "one"}, {2, "two"}},
    {{3, "three"}}
};

// Flatten map values (pairs)
for (const auto& pair : maps | bgf::views::flat) {
    // Accesses each std::pair<key, T>
}
```

#### Integration with Standard Ranges

```cpp
std::vector<std::vector<int>> data{{1, 2}, {3, 4, 5}};

// Combine with other range adaptors
auto result = data
    | bgf::views::flat
    | std::views::filter([](int x) { return x % 2 == 0; })
    | std::views::transform([](int x) {return std::array{x, x}; })
    | bgf::views::flat

// Result: {2, 2, 4, 4} (even numbers duplicated)
```

#### Temporary Objects

```cpp
// Works with rvalues
auto flat = std::vector<std::set<int>>{{1, 2}, {3, 4}} | bgf::views::flat;
// Result: {1, 2, 3, 4}

auto flat_five = 5 | bgf::views::flat;
// Result: {5}
```

### Advanced Usage

#### Chained Flattening

```cpp
auto wrap_func = [](auto &&x) {
  return std::vector<std::decay_t<decltype(x)>>{x};
};
// Multiple flattening operations
auto complex = data
    | std::views::transform(wrap_func)
    | std::views::transform(wrap_func)
    | std::views::transform(wrap_func)
    | bgf::views::flat.nested<int>
    | std::views::transform(wrap_func)
    | bgf::views::flat.depth<2>;
```

### Limitations
- Produces forward iterator only.

## Requirements
- C++20 compatible compiler
- Standard Library with `<ranges>` support
- Header-only, no linking required

## Installation

Add as library via `cmake`.

```cmake
find_package(bgfutils REQUIRED)
target_link_libraries(${PROJECT} bgf::utils)
```

## Code of Conduct

Please see the [`CODE_OF_CONDUCT.md`](CODE_OF_CONDUCT.md) document

## Contributing

Please see the [`CONTRIBUTING.md`](CONTRIBUTING.md) document
