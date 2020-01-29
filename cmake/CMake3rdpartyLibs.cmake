#
# CMake Settings for usage of 3rdparty libs
#

if(my_module_3rdParty_included)
  return()
endif(my_module_3rdParty_included)
set(my_module_3rdParty_included true)

if(NOT DEFINED BIN_STAGE_INTDIR)
  message("BIN_STAGE_INTDIR not set ${BIN_STAGE_INTDIR}")
endif()

if(BUILD_X86)
  set(3RDPARTY_LIB_OUTPUT_PATH
    ${OPT_ROOT}/lib/${BIN_STAGE_INTDIR}
    )

  set(3RDPARTY_DEBUG_LIB
    ${OPT_ROOT}/lib/${3RDPARTY_LINK_TYPE}/Debug
    )

  set(3RDPARTY_RELEASE_LIB
    ${OPT_ROOT}/lib/${3RDPARTY_LINK_TYPE}/Release
    )

elseif(BUILD_X64)
  set(3RDPARTY_LIB_OUTPUT_PATH
    ${OPT_ROOT}/lib64/${BIN_STAGE_INTDIR}
    )

  set(3RDPARTY_DEBUG_LIB
    ${OPT_ROOT}/lib64/${3RDPARTY_LINK_TYPE}/Debug
    )

  set(3RDPARTY_RELEASE_LIB
    ${OPT_ROOT}/lib64/${3RDPARTY_LINK_TYPE}/Release
    )
endif()

#
# in Debug prefer debug 3rdparty libs
if(CMAKE_BUILD_TYPE STREQUAL Debug)
  list(APPEND CMAKE_LIBRARY_PATH ${3RDPARTY_DEBUG_LIB})
  list(APPEND CMAKE_LIBRARY_PATH ${3RDPARTY_RELEASE_LIB})
else()
#
# in Release prefer optimized 3rdparty libs
  list(APPEND CMAKE_LIBRARY_PATH ${3RDPARTY_RELEASE_LIB})
  list(APPEND CMAKE_LIBRARY_PATH ${3RDPARTY_DEBUG_LIB})
endif()

# cleanup list
list(REMOVE_DUPLICATES CMAKE_LIBRARY_PATH)

if (CMAKE_SCRIPT_DEBUG)
  message("CMAKE_LIBRARY_PATH = ${CMAKE_LIBRARY_PATH}")
  message("3RDPARTY_LIB_OUTPUT_PATH = ${3RDPARTY_LIB_OUTPUT_PATH}")
endif()




#
# Special 3rdparty settings for Windows Visual Studio builds
if(WIN32)
  # BOOST
  if(NOT DEFINED BOOST_ROOT)
    file(GLOB BOOST_PATHS ${ODK_ROOT}/3rdparty/boost_1_*)
    #message("BOOSTS: ${BOOST_PATHS}")
    find_path(BOOST_ROOT NAMES "boost/smart_ptr.hpp" PATHS ${BOOST_PATHS})
    #message("BOOST_ROOT: ${BOOST_ROOT}")
  endif(NOT DEFINED BOOST_ROOT)

  if(MSVC)
    if(3RDPARTY_LINK_SHARED)
      add_definitions("-DBOOST_ALL_DYN_LINK")
    endif()
    # disable auto-link feature:
    add_definitions("-DBOOST_ALL_NO_LIB")
  endif()

endif()


#
# Special 3rdparty settings for Linux
if(UNIX)

  if(NOT BOOST_ROOT)
    # is boost installed in /opt then use that one
    if(EXISTS /opt/include/boost)
      set(BOOST_ROOT         /opt/include/boost)
    else()
      if(EXISTS ${OPT_ROOT}/include/boost)
        set(BOOST_ROOT         ${OPT_ROOT}/include/boost)
      else()
        set(BOOST_ROOT         ${3RDPARTY_ROOT}/boost)
      endif()
    endif()
  endif()

  if(NOT BOOST_ROOT)
    set(BOOST_ROOT         ${3RDPARTY_ROOT}/boost)
  endif()

  if(3RDPARTY_LINK_SHARED)
    add_definitions("-DBOOST_ALL_DYN_LINK")
  endif()

  if (CMAKE_SCRIPT_DEBUG)
    message(STATUS "BOOST_ROOT = ${BOOST_ROOT}")
    message(STATUS "CMAKE_INCLUDE_PATH = ${CMAKE_INCLUDE_PATH}")
  endif()
endif()
