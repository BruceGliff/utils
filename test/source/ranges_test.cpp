#include "ranges/ranges.hpp"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("Name is ranges", "[library]")
{
  REQUIRE(name() == "ranges");
}
