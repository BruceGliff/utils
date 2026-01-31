set(ranges_FOUND YES)

include(CMakeFindDependencyMacro)

if(ranges_FOUND)
  include("${CMAKE_CURRENT_LIST_DIR}/bgf_rangesTargets.cmake")
endif()
