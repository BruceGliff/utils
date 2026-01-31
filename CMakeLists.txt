cmake_minimum_required(VERSION 3.14)

include(cmake/prelude.cmake)

project(
    ranges
    VERSION 1.0.0
    DESCRIPTION "header-only library with uncommon ranges"
)

include(cmake/project-is-top-level.cmake)
include(cmake/variables.cmake)

# ---- Declare library ----

add_library(bgf_ranges INTERFACE)
add_library(bgf::ranges ALIAS bgf_ranges)

set_property(
    TARGET bgf_ranges PROPERTY
    EXPORT_NAME ranges
)

target_include_directories(
    bgf_ranges ${warning_guard}
    INTERFACE
    "\$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>"
)

target_compile_features(bgf_ranges INTERFACE cxx_std_20)

# ---- Install rules ----

if(NOT CMAKE_SKIP_INSTALL_RULES)
  include(cmake/install-rules.cmake)
endif()

# ---- Developer mode ----

if(NOT ranges_DEVELOPER_MODE)
  return()
elseif(NOT PROJECT_IS_TOP_LEVEL)
  message(
      AUTHOR_WARNING
      "Developer mode is intended for developers of ranges"
  )
endif()

include(cmake/dev-mode.cmake)
