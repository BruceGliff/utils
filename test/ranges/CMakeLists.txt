cmake_minimum_required(VERSION 3.14)

set(TEST_NAME ranges_test)
project(${TEST_NAME} LANGUAGES CXX)

add_executable(${TEST_NAME} source/ranges_test.cpp)
target_link_libraries(
    ${TEST_NAME} PRIVATE
    bgf::utils
    Catch2::Catch2WithMain
)
target_compile_features(${TEST_NAME} PRIVATE cxx_std_20)

catch_discover_tests(${TEST_NAME})

