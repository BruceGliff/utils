/**
 * This header provides a lightweight, compile time functionality
 * for storing and accessing type/value correlation in mapping.
 *
 * Useful for any type/value conversions, known in compile time:
 *   using m = bgf::ctmap<
 *               bgf::t2t<int, float>,
 *               bgf::v2v<enumA::val, enumB::val>
 *             >;
 *   static_assert(m::v2v<enumA::val> == enumB::val);
 *   static_assert(std::is_same_v<m::t2t<float>, int>);
 *
 * @author [Ilya Andreev]
 *
 * @license MIT
 *
 * This software is free to use, modify, and distribute under the MIT License.
 * See LICENSE for full details.
 */

#include <tuple>
#include <type_traits>

namespace bgf {

namespace detail {

enum class carrier_types {
  type,
  value,
};

template <typename T> struct type_carrier {
  static constexpr carrier_types tag = carrier_types::type;
  using t = T;
};

template <auto V> struct value_carrier {
  static constexpr carrier_types tag = carrier_types::value;
  using t = decltype(V);
  static constexpr t v = V;
};

struct nothing_carrier {};

template <typename C>
concept has_carrier_tag = requires {
  { C::tag } -> std::same_as<const carrier_types &>;
};

template <typename C>
concept type_carrier_c = has_carrier_tag<C> && C::tag == carrier_types::type;

template <typename C>
concept value_carrier_c = has_carrier_tag<C> && C::tag == carrier_types::value;

template <typename C>
concept carrier_c = type_carrier_c<C> || value_carrier_c<C>;

template <typename C>
concept pair_c =
    carrier_c<decltype(C::first)> and carrier_c<decltype(C::second)>;

} // namespace detail

template <typename A, typename B>
// Type-Type pair
struct t2t {
  static constexpr auto first = detail::type_carrier<A>{};
  static constexpr auto second = detail::type_carrier<B>{};
};

template <auto A, typename B>
// Value-Type pair (Type-Value pair is the same, as map checks both elements)
struct v2t {
  static constexpr auto first = detail::value_carrier<A>{};
  static constexpr auto second = detail::type_carrier<B>{};
};

template <auto A, auto B>
// Value-Value pair
struct v2v {
  static constexpr auto first = detail::value_carrier<A>{};
  static constexpr auto second = detail::value_carrier<B>{};
};

template <detail::pair_c... Pairs> struct ctmap {
private:
  static consteval bool is_equal(detail::carrier_c auto,
                                 detail::carrier_c auto) {
    return false;
  }
  template <detail::carrier_c T> static consteval bool is_equal(T, T) {
    return true;
  }

  template <detail::carrier_types DesiredTag>
  static consteval auto process(detail::carrier_c auto target,
                                detail::pair_c auto head,
                                detail::pair_c auto... tail) {
    if constexpr (is_equal(target, head.first) &&
                  head.second.tag == DesiredTag) {
      return head.second;
    } else if constexpr (is_equal(target, head.second) &&
                         head.first.tag == DesiredTag) {
      return head.first;
    } else if constexpr (sizeof...(tail) == 0) {
      return detail::nothing_carrier{};
    } else {
      return process<DesiredTag>(target, tail...);
    }
  }

  static consteval detail::type_carrier_c auto
  deduce_type_impl(detail::carrier_c auto c) {
    auto v = process<detail::carrier_types::type>(c, Pairs{}...);
    static_assert(detail::type_carrier_c<decltype(v)>,
                  "cannot find a proper type");
    return v;
  }
  static consteval detail::value_carrier_c auto
  deduce_value_impl(detail::carrier_c auto c) {
    auto v = process<detail::carrier_types::value>(c, Pairs{}...);
    static_assert(detail::value_carrier_c<decltype(v)>,
                  "cannot find a proper value");
    return v;
  }

public:
  template <detail::carrier_c T>
  static consteval auto deduce_type() -> decltype(deduce_type_impl(T{}));

  template <typename T>
  // NOTE: clang does not allow to use deduce_type_impl here as it is private
  // member of ctmap.
  using t2t = decltype(deduce_type<detail::type_carrier<T>>())::t;

  template <auto V>
  using v2t = decltype(deduce_type<detail::value_carrier<V>>())::t;

  template <typename T>
  static constexpr auto t2v =
      decltype(ctmap::deduce_value_impl(detail::type_carrier<T>{}))::v;

  template <auto V>
  static constexpr auto v2v =
      decltype(ctmap::deduce_value_impl(detail::value_carrier<V>{}))::v;
};

} // namespace bgf
