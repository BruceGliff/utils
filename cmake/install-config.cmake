set(bgfutils_FOUND YES)

include(CMakeFindDependencyMacro)

if(bgfutils_FOUND)
  include("${CMAKE_CURRENT_LIST_DIR}/bgfutilsTargets.cmake")
endif()
