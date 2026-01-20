cmake_minimum_required(VERSION 3.14)

include(cmake/prelude.cmake)

project(
    ranges
    VERSION 0.1.0
    DESCRIPTION "header-only library with uncommon ranges"
    HOMEPAGE_URL "https://example.com/"
    LANGUAGES NONE
)

include(cmake/project-is-top-level.cmake)
include(cmake/variables.cmake)

# ---- Declare library ----

add_library(ranges_ranges INTERFACE)
add_library(ranges::ranges ALIAS ranges_ranges)

set_property(
    TARGET ranges_ranges PROPERTY
    EXPORT_NAME ranges
)

target_include_directories(
    ranges_ranges ${warning_guard}
    INTERFACE
    "\$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>"
)

target_compile_features(ranges_ranges INTERFACE cxx_std_20)

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
