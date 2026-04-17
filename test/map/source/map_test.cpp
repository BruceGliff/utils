#include "bgf/utils/map/ctmap.hpp"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("conversion", "[compile time]") {
  struct nttp {
    int v;
  };

  using m = bgf::ctmap<bgf::t2t<int, float>, bgf::t2t<char, double>,
                       bgf::v2t<4, double>, bgf::v2v<5, 4>, bgf::v2t<5, char>,
                       bgf::v2t<nttp{1}, nttp>>;

  SECTION("[type-to-type]") {
    STATIC_REQUIRE(std::is_same_v<m::t2t<int>, float>);
    STATIC_REQUIRE(std::is_same_v<m::t2t<float>, int>);
    STATIC_REQUIRE(std::is_same_v<m::t2t<char>, double>);
    STATIC_REQUIRE(std::is_same_v<m::t2t<double>, char>);
  }
  SECTION("[value-to-type]") {
    STATIC_REQUIRE(std::is_same_v<m::v2t<4>, double>);
    STATIC_REQUIRE(std::is_same_v<m::v2t<5>, char>);
    STATIC_REQUIRE(std::is_same_v<m::v2t<nttp{1}>, nttp>);
  }
  SECTION("[type-to-value]") {
    STATIC_REQUIRE(m::t2v<double> == 4);
    STATIC_REQUIRE(m::t2v<char> == 5);
    STATIC_REQUIRE(m::t2v<nttp>.v == 1);
  }
  SECTION("[value-to-value]") {
    STATIC_REQUIRE(m::v2v<5> == 4);
    STATIC_REQUIRE(m::v2v<4> == 5);
  }
}

TEST_CASE("enums", "[compile time]") {

  enum e_a { a };
  enum e_b { b };
  enum class e_c { a, b };

  using m = bgf::ctmap<bgf::v2t<a, int>, bgf::v2t<b, float>,
                       bgf::v2t<e_c::a, e_a>, bgf::v2t<e_c::b, e_b>,
                       bgf::v2v<e_c::b, e_c::a>, bgf::t2t<e_a, e_b>>;

  SECTION("[type-to-type]") {
    STATIC_REQUIRE(std::is_same_v<m::t2t<e_a>, e_b>);
    STATIC_REQUIRE(std::is_same_v<m::t2t<e_b>, e_a>);
  }
  SECTION("[value-to-type]") {
    STATIC_REQUIRE(std::is_same_v<m::v2t<a>, int>);
    STATIC_REQUIRE(std::is_same_v<m::v2t<b>, float>);

    STATIC_REQUIRE(std::is_same_v<m::v2t<e_c::a>, e_a>);
    STATIC_REQUIRE(std::is_same_v<m::v2t<e_c::b>, e_b>);
  }
  SECTION("[type-to-value]") {
    STATIC_REQUIRE(m::t2v<int> == a);
    STATIC_REQUIRE(m::t2v<float> == b);

    STATIC_REQUIRE(m::t2v<e_a> == e_c::a);
    STATIC_REQUIRE(m::t2v<e_b> == e_c::b);
  }
  SECTION("[value-to-value]") {
    STATIC_REQUIRE(m::v2v<e_c::a> == e_c::b);
    STATIC_REQUIRE(m::v2v<e_c::b> == e_c::a);
  }
}
