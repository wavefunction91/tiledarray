include(CheckCXXSourceCompiles)
include(CMakePushCheckState)

macro (detect_MADNESS_configuration)

  cmake_push_check_state()
  set(CMAKE_REQUIRED_QUIET)
  # only extract include dirs, don't use MADworld target directly since it may have not been built yet
  # unfortunately this is not easy to check since the target is defined but not ready
  get_property(_MADNESS_INCLUDE_DIRS TARGET MADworld PROPERTY INTERFACE_INCLUDE_DIRECTORIES)
  # some targets don't even exist in the build tree (El) so filter them out and cache the result
  if (TARGET El)
    set(MADNESS_INTERNAL_INCLUDE_DIRS )
    foreach(_inc ${_MADNESS_INCLUDE_DIRS})
      if (NOT (${_inc} MATCHES "El"))
        list(APPEND MADNESS_INTERNAL_INCLUDE_DIRS ${_inc})
      endif()
    endforeach()
  else()
    set(MADNESS_INTERNAL_INCLUDE_DIRS ${_MADNESS_INCLUDE_DIRS})
  endif()
  set(MADNESS_INTERNAL_INCLUDE_DIRS "${MADNESS_INTERNAL_INCLUDE_DIRS}"
          CACHE STRING "Sanitized list of MADNESS include directories usable in build tree")

  list(APPEND CMAKE_REQUIRED_INCLUDES ${MADNESS_INTERNAL_INCLUDE_DIRS})
  CHECK_CXX_SOURCE_COMPILES(
          "
  #include <madness/config.h>
  #ifndef HAVE_INTEL_MKL
  # error \"MADNESS does not have MKL\"
  #endif
  int main(int argc, char** argv) {
    return 0;
  }
  "  MADNESS_HAS_MKL)

  if (MADNESS_HAS_MKL)
    unset(MADNESS_HAS_MKL)
    set(MADNESS_HAS_MKL ON CACHE BOOL "MADNESS detected usable Intel MKL")
  endif()

  unset(CMAKE_REQUIRED_QUIET)
  cmake_pop_check_state()

endmacro (detect_MADNESS_configuration)
