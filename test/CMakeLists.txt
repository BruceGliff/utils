cmake_minimum_required(VERSION 3.14)

project(rangesTests LANGUAGES CXX)

include(../cmake/project-is-top-level.cmake)
include(../cmake/folders.cmake)

# ---- Dependencies ----

if(PROJECT_IS_TOP_LEVEL)
  find_package(ranges REQUIRED)
  enable_testing()
endif()

find_package(Catch2 REQUIRED)
include(Catch)

# ---- Tests ----

add_executable(ranges_test source/ranges_test.cpp)
target_link_libraries(
    ranges_test PRIVATE
    ranges::ranges
    Catch2::Catch2WithMain
)
target_compile_features(ranges_test PRIVATE cxx_std_20)

catch_discover_tests(ranges_test)

# ---- End-of-file commands ----

add_folders(Test)
