cmake_minimum_required(VERSION 3.14)

project(utilsTests LANGUAGES CXX)

include(../cmake/project-is-top-level.cmake)
include(../cmake/folders.cmake)

# ---- Dependencies ----

if(PROJECT_IS_TOP_LEVEL)
  find_package(bgfutils REQUIRED)
  enable_testing()
endif()

find_package(Catch2 REQUIRED)
include(Catch)

# ---- Tests ----

add_subdirectory(ranges)
add_subdirectory(map)

# ---- End-of-file commands ----

add_folders(Test)
