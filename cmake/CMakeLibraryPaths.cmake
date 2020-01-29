#
# CMake default paths for libraries
#

if(my_module_LibraryPaths_included)
  return()
endif(my_module_LibraryPaths_included)
set(my_module_LibraryPaths_included true)

macro(InitCMakeLibraryPath PREFIX_DIR)

  if(NOT DEFINED BIN_STAGE_INTDIR)
    message(STATUS "BIN_STAGE_INTDIR not set ${BIN_STAGE_INTDIR}")
  endif()

  if(BUILD_X86)
    set(3RDPARTY_LIB_OUTPUT_PATH
      ${PREFIX_DIR}/lib/${BIN_STAGE_INTDIR}
      )

    set(3RDPARTY_DEBUG_LIB
      ${PREFIX_DIR}/lib/${3RDPARTY_LINK_TYPE}/Debug
      )

    set(3RDPARTY_RELEASE_LIB
      ${PREFIX_DIR}/lib/${3RDPARTY_LINK_TYPE}/Release
      )
  
  elseif(BUILD_X64)
    set(3RDPARTY_LIB_OUTPUT_PATH
      ${PREFIX_DIR}/lib64/${BIN_STAGE_INTDIR}
      )

    set(3RDPARTY_DEBUG_LIB
      ${PREFIX_DIR}/lib64/${3RDPARTY_LINK_TYPE}/Debug
      )

    set(3RDPARTY_RELEASE_LIB
      ${PREFIX_DIR}/lib64/${3RDPARTY_LINK_TYPE}/Release
      )
  endif()

  #
  # in Debug prefer debug 3rdparty libs
  if(CMAKE_BUILD_TYPE STREQUAL Debug)
    set(CMAKE_LIBRARY_PATH
      ${CMAKE_LIBRARY_PATH}
      ${3RDPARTY_DEBUG_LIB}
      ${3RDPARTY_RELEASE_LIB}
      )
  else()
    #
    # in Release prefer optimized 3rdparty libs
    set(CMAKE_LIBRARY_PATH
      ${CMAKE_LIBRARY_PATH}
      ${3RDPARTY_RELEASE_LIB}
      ${3RDPARTY_DEBUG_LIB}
      )
  endif()

  if (CMAKE_SCRIPT_DEBUG)
    message("CMAKE_LIBRARY_PATH = ${CMAKE_LIBRARY_PATH}")
    message("3RDPARTY_LIB_OUTPUT_PATH = ${3RDPARTY_LIB_OUTPUT_PATH}")
  endif()

endmacro()


macro(InitCMakeIncludePath PREFIX_DIR)

  set(CMAKE_INCLUDE_PATH
    ${CMAKE_INCLUDE_PATH}
    ${PREFIX_DIR}/include
    )

  if (CMAKE_SCRIPT_DEBUG)
    message("CMAKE_INCLUDE_PATH = ${CMAKE_INCLUDE_PATH}")
  endif()

endmacro()


