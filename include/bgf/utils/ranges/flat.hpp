/**
 * This header provides a lightweight, header-only functionality for flattening
 * deeply nested container structures (e.g., vector<vector<set<int>>>) into
 * flat sequences using the C++20 Ranges framework.
 *
 * @author [Ilya Andreev]
 *
 * @license MIT
 *
 * This software is free to use, modify, and distribute under the MIT License.
 * See LICENSE for full details.
 */

#pragma once

#include <iterator>
#include <optional>
#include <ranges>
#include <span>

namespace bgf {

namespace detail {
template <typename It>
concept iterator_to_range =
    std::forward_iterator<It> && std::ranges::range<std::iter_value_t<It>>;

constexpr auto rng2tpl(std::ranges::range auto &rng) {
  return std::tuple(rng.begin(), rng.end());
}

template <typename T> struct storage_ty {
  // Storage over rvalues to take ownership.
  constexpr explicit storage_ty(T val) : m_data{std::move(val)} {}

  constexpr auto operator*() -> T & { return m_data; }

private:
  T m_data;
};

template <typename T> struct storage_ty<T &> {
  // Storage over refs to avoid copy.
  constexpr explicit storage_ty(T &val) : m_data{val} {}

  constexpr auto operator*() -> T & { return m_data; }

private:
  std::reference_wrapper<T> m_data;
};

template <typename Desired, typename Expected>
struct check_type_is_subtype : std::false_type {};

template <std::ranges::range RTy, typename Expected>
struct check_type_is_subtype<RTy, Expected>
    : check_type_is_subtype<std::ranges::range_value_t<RTy>, Expected> {};

template <typename Desired>
struct check_type_is_subtype<Desired, Desired> : std::true_type {};

template <typename T, typename Desired>
concept type_is_subtype = check_type_is_subtype<T, Desired>::value;

template <typename T, unsigned Depth>
struct depth_type_cvt // Error instantiation
                      /**
                       * Set non-zero depth for a scalar type.
                       * Scalar types can be matched only with Depth=0.
                       */
    ;

template <std::ranges::range RTy, unsigned Depth>
struct depth_type_cvt<RTy, Depth> {
  using incoming_type = RTy;
  using value_type = std::ranges::range_value_t<incoming_type>;
  using type = depth_type_cvt<value_type, Depth - 1>::type;
};

template <typename T>
// Scalar types can be only 0 depth.
struct depth_type_cvt<T, 0> {
  using type = T;
};

template <unsigned Val> struct depth {
  template <typename T> using type = depth_type_cvt<T, Val>::type;
};

template <typename T, typename U>
concept is_depth = requires() { typename T::template type<U>; };

template <typename T, typename U> struct type_resolver;

template <typename T, typename U>
  requires is_depth<U, T>
struct type_resolver<T, U> {
  using type = typename U::template type<T>;
};

template <typename T, typename U>
  requires type_is_subtype<T, U>
struct type_resolver<T, U> {
  using type = U;
};

template <typename T> struct underlying_type {
  using type = T;
};
template <std::ranges::range RTy> struct underlying_type<RTy> {
  using base_type = std::ranges::range_value_t<RTy>;
  using type = underlying_type<base_type>::type;
};

} // namespace detail

template <typename I, typename S, typename TillType>
struct flat_iterator // Error instantiation
    /**
     * Instantiated class does not satisfy any specialization for a range or for
     * a nested range.
     */
    ;

template <std::forward_iterator I, std::sentinel_for<I> S, typename TillType>
  requires std::same_as<std::iter_value_t<I>, TillType>
struct flat_iterator<I, S, TillType> {
  // Iterator only over TillType range.
  using iterator_type = I;
  using sentinel_type = S;
  using recursive_iterator_type = I;
  using range_type = std::ranges::subrange<iterator_type, sentinel_type>;

  // Iterator traits
  using difference_type =
      std::iterator_traits<recursive_iterator_type>::difference_type;
  using iterator_category = std::forward_iterator_tag;
  using value_type = std::iterator_traits<recursive_iterator_type>::value_type;
  using pointer = std::iterator_traits<recursive_iterator_type>::pointer;
  using reference = std::iterator_traits<recursive_iterator_type>::reference;

  constexpr flat_iterator() = default;

  constexpr flat_iterator(iterator_type curr, sentinel_type sent)
      : m_rng{curr, sent} {}

  constexpr auto empty() const -> bool { return m_rng.empty(); }

  constexpr auto operator*() const -> reference { return m_rng.front(); }

  constexpr auto operator->() const -> pointer {
    return std::addressof(operator*());
  }

  constexpr auto operator++() -> flat_iterator & {
    m_rng.advance(1);
    return *this;
  }

  constexpr auto operator++(int) -> flat_iterator {
    flat_iterator copy{*this};
    this->operator++();
    return copy;
  }

  constexpr auto operator==(flat_iterator other) const -> bool {
    return detail::rng2tpl(m_rng) == detail::rng2tpl(other.m_rng);
  }

private:
  range_type m_rng{};
};

template <detail::iterator_to_range I, std::sentinel_for<I> S,
          typename TillType>
  requires(not std::same_as<std::iter_value_t<I>, TillType>)
struct flat_iterator<I, S, TillType> {
  // Iterator over range values
  using iterator_type = I;
  using sentinel_type = S;
  using base_type = std::iter_value_t<iterator_type>;
  using reference_base_type = std::iter_reference_t<iterator_type>;
  using base_iterator_type = std::ranges::iterator_t<base_type>;
  using base_sentinel_type = std::ranges::sentinel_t<base_type>;

  using nested_iterator_type =
      flat_iterator<base_iterator_type, base_sentinel_type, TillType>;
  using recursive_iterator_type = nested_iterator_type::recursive_iterator_type;
  using range_type = std::ranges::subrange<iterator_type, sentinel_type>;

  // Iterator traits
  using difference_type =
      std::iterator_traits<recursive_iterator_type>::difference_type;
  using iterator_category = std::forward_iterator_tag;
  using value_type = std::iterator_traits<recursive_iterator_type>::value_type;
  using pointer = std::iterator_traits<recursive_iterator_type>::pointer;
  using reference = std::iterator_traits<recursive_iterator_type>::reference;

  constexpr flat_iterator() = default;

  constexpr flat_iterator(iterator_type curr, sentinel_type sent)
      // Sentinel is required to stop skipping empty subranges.
      : m_rng{curr, sent}, m_nrng{std::nullopt} {
    skip_empty();
  }

  constexpr auto empty() const -> bool { return m_rng.empty(); }

  constexpr auto operator*() const -> reference { return **m_nrng; }

  constexpr auto operator->() const -> pointer {
    return std::addressof(operator*());
  }

  constexpr auto operator++() -> flat_iterator & {
    ++(*m_nrng);
    if (m_nrng->empty()) {
      m_rng.advance(1);
      skip_empty();
    }
    return *this;
  }

  constexpr auto operator++(int) -> flat_iterator {
    flat_iterator copy{*this};
    this->operator++();
    return copy;
  }

  constexpr auto operator==(flat_iterator other) const -> bool {
    return detail::rng2tpl(m_rng) == detail::rng2tpl(other.m_rng) &&
           m_nrng == other.m_nrng;
  }

private:
  constexpr void skip_empty() {
    while (!m_rng.empty()) {
      auto &subrng = *m_storage.emplace(m_rng.front());
      m_nrng = nested_iterator_type{subrng.begin(), subrng.end()};
      if (!m_nrng->empty()) {
        return;
      }
      m_rng.advance(1);
    }
    m_nrng = std::nullopt;
    m_storage = std::nullopt;
  }

  range_type m_rng{};
  // Storage for a range, if it is a rvalue.
  // This represents flat_view, but this will introduce recurcive dependency.
  std::optional<detail::storage_ty<reference_base_type>> m_storage{};
  std::optional<nested_iterator_type> m_nrng{};
};

template <typename T, typename TillType>
  requires detail::type_is_subtype<T, TillType>
struct flat_view_impl
    : std::ranges::view_interface<flat_view_impl<T, TillType>> {
private:
  constexpr static auto cvt(T &val) {
    if constexpr (std::same_as<T, TillType>) {
      // Emulate stop recursion for non-range types and for range type as
      // TillType.
      return std::span<T>(std::addressof(val), 1);
    } else {
      return std::ranges::subrange(val);
    }
  }

  detail::storage_ty<T> m_storage;
  decltype(cvt(std::declval<T &>())) m_rng;
  using range_iterator = std::ranges::iterator_t<decltype(m_rng)>;
  using range_sentinel = std::ranges::sentinel_t<decltype(m_rng)>;

protected:
  template <typename U>
    requires(!std::same_as<flat_view_impl, U>)
  constexpr explicit flat_view_impl(U &&val)
      : m_storage{std::forward<U>(val)}, m_rng{cvt(*m_storage)} {}

public:
  constexpr auto begin() const {
    return flat_iterator<range_iterator, range_sentinel, TillType>(
        m_rng.begin(), m_rng.end());
  }

  constexpr auto end() const {
    return flat_iterator<range_sentinel, range_sentinel, TillType>(m_rng.end(),
                                                                   m_rng.end());
  }
};

template <typename T,
          typename TillTypeOrDepth = detail::underlying_type<T>::type>
  requires detail::is_depth<TillTypeOrDepth, T> or
           detail::type_is_subtype<T, TillTypeOrDepth>
struct flat_view
    : flat_view_impl<T,
                     typename detail::type_resolver<T, TillTypeOrDepth>::type> {
  using base =
      flat_view_impl<T,
                     typename detail::type_resolver<T, TillTypeOrDepth>::type>;

  template <typename U>
    requires(!std::same_as<flat_view, U>)
  constexpr explicit flat_view(U &&val) : base{std::forward<U>(val)} {}
};

template <typename T>
flat_view(T &&) -> flat_view<T, typename detail::underlying_type<T>::type>;

namespace detail {
struct flat_fn {

  template <typename Till> struct flat_till_fn {
    template <typename T> constexpr auto operator()(T &&rng) const {
      return flat_view<T, Till>(std::forward<T>(rng));
    }

    template <typename T>
    constexpr friend auto operator|(T &&rng, const flat_till_fn &) {
      return flat_view<T, Till>{std::forward<T>(rng)};
    }
  };

  template <unsigned Depth> struct flat_depth_fn {
    template <typename T> constexpr auto operator()(T &&rng) const {
      return flat_view<T, detail::depth<Depth>>{std::forward<T>(rng)};
    }

    template <typename T>
    constexpr friend auto operator|(T &&rng, const flat_depth_fn &) {
      return flat_view<T, detail::depth<Depth>>{std::forward<T>(rng)};
    }
  };

  template <typename T> static inline flat_till_fn<T> nested{};
  template <unsigned Depth> static inline flat_depth_fn<Depth> depth{};

  template <typename T> constexpr auto operator()(T &&rng) const {
    return flat_view(std::forward<T>(rng));
  }

  template <typename T>
  constexpr friend auto operator|(T &&rng, const flat_fn &) {
    return flat_view(std::forward<T>(rng));
  }
};

} // namespace detail

namespace views {

constexpr inline detail::flat_fn flat;

} // namespace views

} // namespace bgf
