#include <map>
#include <set>
#include <variant>
#include <vector>

#include "bgf/utils/ranges.hpp"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_range_equals.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

using scal = int;
using set1 = std::set<scal>;
using vec2 = std::vector<set1>;
using vec3 = std::vector<vec2>;

TEST_CASE("flat", "[concepts]") {
  using namespace bgf::detail;

  SECTION("[subtype]") {
    static_assert(type_is_subtype<scal, scal>);
    static_assert(not type_is_subtype<scal, float>);

    static_assert(type_is_subtype<set1, scal>);
    static_assert(type_is_subtype<set1, set1>);
    static_assert(not type_is_subtype<set1, float>);

    static_assert(type_is_subtype<set1, scal>);
    static_assert(type_is_subtype<set1, set1>);
    static_assert(not type_is_subtype<set1, float>);

    static_assert(type_is_subtype<vec3, scal>);
    static_assert(type_is_subtype<vec3, set1>);
    static_assert(type_is_subtype<vec3, vec2>);
    static_assert(type_is_subtype<vec3, vec3>);
    static_assert(not type_is_subtype<vec3, float>);
  }
  SECTION("[dev_type_cvt]") {
    static_assert(std::same_as<depth_type_cvt<scal, 0>::type, scal>);
    static_assert(not std::same_as<depth_type_cvt<scal, 0>::type, float>);

    static_assert(std::same_as<depth_type_cvt<set1, 0>::type, set1>);
    static_assert(std::same_as<depth_type_cvt<set1, 1>::type, scal>);

    static_assert(std::same_as<depth_type_cvt<vec3, 0>::type, vec3>);
    static_assert(std::same_as<depth_type_cvt<vec3, 1>::type, vec2>);
    static_assert(std::same_as<depth_type_cvt<vec3, 2>::type, set1>);
    static_assert(std::same_as<depth_type_cvt<vec3, 3>::type, scal>);
  }
  SECTION("[underlying_type]") {
    static_assert(std::same_as<underlying_type<vec3>::type, scal>);
    static_assert(not std::same_as<underlying_type<vec3>::type, float>);
    static_assert(std::same_as<underlying_type<vec2>::type, scal>);
    static_assert(std::same_as<underlying_type<set1>::type, scal>);
    static_assert(std::same_as<underlying_type<scal>::type, scal>);
  }
}

TEST_CASE("flat_view", "[static checks]") {
  using namespace bgf::detail;
  SECTION("[common]") {
    using vec = std::vector<int>;
    using iterator = std::vector<int>::iterator;

    using nested_vec = std::vector<vec>;
    using nested_iterator = std::vector<vec>::iterator;

    using flat_iterator =
        bgf::flat_iterator<nested_iterator, nested_iterator, int>;
    static_assert(
        std::forward_iterator<bgf::flat_iterator<iterator, iterator, int>>);
    static_assert(std::forward_iterator<flat_iterator>);

    static_assert(std::ranges::range<bgf::flat_view<vec>>);
    static_assert(std::ranges::range<bgf::flat_view<nested_vec>>);

    static_assert(std::same_as<flat_iterator::value_type, int>);
  }
  SECTION("[nested]") {
    static_assert(
        std::same_as<std::ranges::range_value_t<bgf::flat_view<vec3>>, scal>);
    static_assert(
        std::same_as<std::ranges::range_value_t<bgf::flat_view<vec3, scal>>,
                     scal>);
    static_assert(
        std::same_as<std::ranges::range_value_t<bgf::flat_view<vec3, set1>>,
                     set1>);
    static_assert(
        std::same_as<std::ranges::range_value_t<bgf::flat_view<vec3, vec2>>,
                     vec2>);
    static_assert(
        std::same_as<std::ranges::range_value_t<bgf::flat_view<vec3, vec3>>,
                     vec3>);
    static_assert(
        std::same_as<std::ranges::range_value_t<bgf::flat_view<scal, scal>>,
                     scal>);
  }
  SECTION("[depth]") {
    static_assert(
        std::same_as<std::ranges::range_value_t<bgf::flat_view<vec3, depth<3>>>,
                     scal>);
    static_assert(
        std::same_as<std::ranges::range_value_t<bgf::flat_view<vec3, depth<2>>>,
                     set1>);
    static_assert(
        std::same_as<std::ranges::range_value_t<bgf::flat_view<vec3, depth<1>>>,
                     vec2>);
    static_assert(
        std::same_as<std::ranges::range_value_t<bgf::flat_view<vec3, depth<0>>>,
                     vec3>);
    static_assert(
        std::same_as<std::ranges::range_value_t<bgf::flat_view<scal, depth<0>>>,
                     scal>);
  }
}

TEST_CASE("view_iterator", "[vector]") {
  std::vector vec{0, 1, 2, 3};
  using it = decltype(vec.begin());
  using fit = bgf::flat_iterator<it, it, int>;

  auto begin = fit(vec.begin(), vec.end());
  decltype(begin) def{};
  auto end = fit(vec.end(), vec.end());
  REQUIRE(def != begin);
  REQUIRE(def != end);

  auto subr = std::ranges::subrange(begin, end);
  REQUIRE_THAT(subr, Catch::Matchers::RangeEquals(std::array{0, 1, 2, 3}));
}

TEST_CASE("view_iterator", "[vector<vector>]") {
  std::vector<std::vector<int>> vec{{0, 1}, {2}, {}, {3}};
  using it = decltype(vec.begin());
  using fit = bgf::flat_iterator<it, it, int>;

  auto begin = fit(vec.begin(), vec.end());
  auto end = fit(vec.end(), vec.end());

  auto subr = std::ranges::subrange(begin, end);
  REQUIRE_THAT(subr, Catch::Matchers::RangeEquals(std::array{0, 1, 2, 3}));
}

TEST_CASE("view_iterator", "[vector<set>]") {
  std::vector<std::set<int>> vec{{0, 1}, {2}, {}, {3}, {5, 4}};
  using it = decltype(vec.begin());
  using fit = bgf::flat_iterator<it, it, int>;

  auto begin = fit(vec.begin(), vec.end());
  auto end = fit(vec.end(), vec.end());

  auto subr = std::ranges::subrange(begin, end);

  REQUIRE_THAT(subr,
               Catch::Matchers::RangeEquals(std::array{0, 1, 2, 3, 4, 5}));
}

TEST_CASE("flat_view", "[vector<set>]") {
  std::vector<std::set<int>> vec{{0, 1}, {2}, {}, {3}, {5, 4}};

  REQUIRE_THAT(bgf::flat_view(vec),
               Catch::Matchers::RangeEquals(std::array{0, 1, 2, 3, 4, 5}));
}

TEST_CASE("flat_view", "[rval vector<set>]") {
  REQUIRE_THAT(
      bgf::flat_view(std::vector<std::set<int>>{{0, 1}, {2}, {}, {3}, {5, 4}}),
      Catch::Matchers::RangeEquals(std::array{0, 1, 2, 3, 4, 5}));
}

TEST_CASE("flat_view", "[rval]") {
  REQUIRE_THAT(bgf::flat_view(5), Catch::Matchers::RangeEquals(std::array{5}));
}

static auto wrap_func = [](auto &&x) {
  return std::vector<std::decay_t<decltype(x)>>{x};
};

TEST_CASE("flat", "[pipe]") {
  std::vector orig{0, 1, 2};

  auto nested =
      orig | std::views::transform(wrap_func) |
      std::views::transform(wrap_func) | std::views::transform(wrap_func) |
      std::views::transform(wrap_func) | std::views::transform(wrap_func);

  REQUIRE_THAT(nested | bgf::views::flat, Catch::Matchers::RangeEquals(orig));
}

template <typename FTy, typename... Args>
static auto var_cvt(FTy &&func, std::variant<Args...>)
    -> std::variant<decltype(func(std::declval<Args>()))...>;

TEST_CASE("flat", "[almost-yaml]") {
  auto getname = []() {
    static std::array names = {"John",  "Alay",  "Aria", "Bruce",
                               "Bush",  "Stone", "Kirk", "43",
                               "Gliff", "Jake",  "Soft", "Mon"};
    static size_t counter = 0;
    REQUIRE(counter < names.size());
    return names[counter++];
  };
  auto getint = []() {
    static int i = 0;
    return i++;
  };
  auto getmap = [&getname, &getint]() {
    return std::map<int, std::string>{{{getint(), getname()}}};
  };
  auto getvectormap = [&getmap]() { return std::vector{getmap(), getmap()}; };
  auto getsetint = [&getint]() { return std::set{getint(), getint()}; };
  auto getsetstring = [&getname]() { return std::set{getname(), getname()}; };
  auto getvectorsetint = [&getsetint]() {
    return std::vector{getsetint(), getsetint()};
  };
  auto getvectorsetstring = [&getsetstring]() {
    return std::vector{getsetstring(), getsetstring()};
  };

  struct to_str_cvt {
    std::string operator()(int i) const { return std::to_string(i); }

    std::string operator()(const decltype(getmap())::value_type &val) const {
      auto &&[i, str] = val;
      return std::to_string(i) + ":" + str;
    }

    std::string operator()(std::string_view s) const { return std::string{s}; }
  };

  std::vector<
      std::variant<decltype(getvectorsetstring()), decltype(getvectorsetint()),
                   decltype(getmap()), decltype(getvectormap())>>
      vec{getvectorsetstring(), getvectorsetint(), getmap(),
          getvectorsetstring(), getvectorsetint(), getmap(),
          getvectormap()};

  auto strings =
      vec | std::views::transform([](auto &&var) {
        auto cvt_f = [](auto &&value) {
          return value | bgf::views::flat | std::views::transform(to_str_cvt{});
        };

        using ret_var = decltype(var_cvt(cvt_f, var));
        auto cvt_wrapper = [&cvt_f](auto &&value) -> ret_var {
          return cvt_f(value);
        };
        return std::visit(cvt_wrapper, var);
      });

  std::stringstream os;
  for (auto &&var : strings) {
    std::visit(
        [&os](auto &&rng) {
          for (auto &&x : rng) {
            os << x << " | ";
          }
        },
        var);
  }

  constexpr auto ref =
      "John | Alay | Aria | Bruce | 0 | 1 | 2 | 3 | 4:Bush | Stone | Kirk | 43 "
      "| Gliff | 5 | 6 | 7 | 8 | 9:Jake | 10:Soft | 11:Mon | ";

  REQUIRE_THAT(os.str(), Catch::Matchers::Equals(ref));
}

TEST_CASE("flat", "[depth]") {
  std::vector orig{0, 1, 2};

  REQUIRE_THAT(orig | std::views::transform(wrap_func) |
                   bgf::views::flat.nested<int>,
               Catch::Matchers::RangeEquals(orig));

  REQUIRE_THAT(orig | std::views::transform(wrap_func) |
                   bgf::views::flat.nested<int> |
                   std::views::transform(wrap_func) | bgf::views::flat.depth<2>,
               Catch::Matchers::RangeEquals(orig));

  REQUIRE_THAT(orig | std::views::transform(wrap_func) |
                   bgf::views::flat.depth<2>,
               Catch::Matchers::RangeEquals(orig));

  REQUIRE_THAT(orig | std::views::transform(wrap_func) |
                   std::views::transform(wrap_func) |
                   std::views::transform(wrap_func) |
                   std::views::transform(wrap_func) | bgf::views::flat.depth<5>,
               Catch::Matchers::RangeEquals(orig));

  REQUIRE_THAT(5 | bgf::views::flat | std::views::transform(wrap_func) |
                   std::views::transform(wrap_func) |
                   std::views::transform(wrap_func) |
                   std::views::transform(wrap_func) | bgf::views::flat.depth<5>,
               Catch::Matchers::RangeEquals(std::array{5}));

  REQUIRE_THAT(orig | std::views::transform(wrap_func) |
                   bgf::views::flat.depth<1> // flat view is +1 nesting layer
                   | bgf::views::flat.depth<2>,
               Catch::Matchers::RangeEquals(orig));

  REQUIRE_THAT(orig | std::views::transform(wrap_func) |
                   std::views::transform(wrap_func) |
                   bgf::views::flat.nested<std::vector<int>> |
                   bgf::views::flat.depth<2>,
               Catch::Matchers::RangeEquals(orig));

  REQUIRE_THAT(orig | std::views::transform(wrap_func) |
                   std::views::transform(wrap_func) |
                   bgf::views::flat.nested<int> | bgf::views::flat.depth<1>,
               Catch::Matchers::RangeEquals(orig));

  REQUIRE_THAT(
      orig | std::views::transform(wrap_func) |
          std::views::transform(wrap_func) | std::views::transform(wrap_func) |
          bgf::views::flat.nested<std::vector<int>> | bgf::views::flat.depth<2>,
      Catch::Matchers::RangeEquals(orig));

  REQUIRE_THAT(orig | std::views::transform(wrap_func) |
                   std::views::transform(wrap_func) |
                   std::views::transform(wrap_func) | bgf::views::flat,
               Catch::Matchers::RangeEquals(orig));
}
