cmake_minimum_required(VERSION 3.14)

include(cmake/prelude.cmake)

project(
    bgfutils
    VERSION 2.1.0
    DESCRIPTION "header-only library with uncommon utils"
)

include(cmake/project-is-top-level.cmake)
include(cmake/variables.cmake)

# ---- Declare library ----

add_library(bgfutils INTERFACE)
add_library(bgf::utils ALIAS bgfutils)

set_property(
    TARGET bgfutils PROPERTY
    EXPORT_NAME utils
)

target_include_directories(
    bgfutils ${warning_guard}
    INTERFACE
    "\$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>"
)

target_compile_features(bgfutils INTERFACE cxx_std_20)

# ---- Install rules ----

if(NOT CMAKE_SKIP_INSTALL_RULES)
  include(cmake/install-rules.cmake)
endif()

# ---- Developer mode ----

if(NOT utils_DEVELOPER_MODE)
  return()
elseif(NOT PROJECT_IS_TOP_LEVEL)
  message(
      AUTHOR_WARNING
      "Developer mode is intended for developers of utils"
  )
endif()

include(cmake/dev-mode.cmake)
