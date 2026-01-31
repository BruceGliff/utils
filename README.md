# ranges -- Library with indispensable ranges and views

## Quick Navigation
- [flat_view - Recursive Flattening View](#flat_view---recursive-flattening-view)
  - [Overview](#overview)
  - [Core Features](#core-features)
  - [Usage Examples](#usage-examples)
  - [Advanced Usage](#advanced-usage)
  - [Requirements](#requirements)
  - [Installation](#installation)
  - [Limitations](#limitations)
- [Code of Conduct](#code-of-conduct)
- [Contributing](#contributing)

---

## flat_view - Recursive Flattening View

### Overview

`bgf::flat_view` is a C++20 header-only library that provides **recursive flattening views** for nested containers. It allows you to traverse deeply nested data structures or ranges as if they were flat ranges, with compile-time control over flattening depth and target types.
This is ideal for working with complex, nested data structures where you need to process all elements regardless of their depth in the hierarchy.

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
#include "bgf/ranges/ranges.hpp"

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

### Requirements
- C++20 compatible compiler
- Standard Library with `<ranges>` support
- Header-only, no linking required

### Installation

Add as library via `cmake`.

```cmake
find_package(bgf_ranges REQUIRED)
target_link_libraries(${PROJECT} bgf::ranges)
```

Include the `bgf/ranges/ranges.hpp` header in your project:

```cpp
#include "bgf/ranges/ranges.hpp"
```

### Limitations
- Produces forward iterator only.

## Code of Conduct

Please see the [`CODE_OF_CONDUCT.md`](CODE_OF_CONDUCT.md) document

## Contributing

Please see the [`CONTRIBUTING.md`](CONTRIBUTING.md) document
