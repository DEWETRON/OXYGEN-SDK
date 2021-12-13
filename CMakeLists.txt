#
# Oxygen plugin sdk
#

cmake_minimum_required(VERSION 3.1)

#
# handle setup of a cmake toplevel project
# finding libraries etc
if (${CMAKE_CURRENT_SOURCE_DIR} STREQUAL ${CMAKE_SOURCE_DIR})

  #
  # studio project name
  project(ODK)


  if( MSVC )
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
  else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
  endif()

  if(WIN32)
    set(3RDPARTY_LINK_TYPE "static")
  elseif(UNIX)
    set(3RDPARTY_LINK_TYPE "shared")
  endif()

  get_filename_component(ODK_ROOT . ABSOLUTE)
  message(STATUS "ODKROOT = ${ODK_ROOT}")
  # expand cmake search path to check for project settings
  set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ODK_ROOT}/cmake )

  #
  # Get general cmake settings
  include(CMakeSettings)

  #
  # Set static runtime
  SetLinkStaticRuntime()

  #
  # Set the output directory so that all artefacts will be at the same place
  # (moved here to set output directory for 3rdparty libs too)
  SetCommonOutputDirectory()

  #
  # Is github repository (or inside company)
  if(NOT DEFINED GITHUB_REPO)
    set(GITHUB_REPO True)   # default
  endif()

  # Quiz "inside company"
  if (EXISTS driver)
    set(GITHUB_REPO False)
  endif()

  message(STATUS "GITHUB_REPO = ${GITHUB_REPO}")
  
  SetBoostOptions() # set static, dynamic ...

  set(UNIT_TEST_SUFFIX test)
  enable_testing()

  if (NOT GITHUB_REPO)
    set(BOOST_MODULES
      chrono
      date_time
      filesystem
      locale
      log
      program_options
      regex
      system
      thread
      unit_test_framework
    )
  else()
    set(BOOST_MODULES
      unit_test_framework
    )
  endif()

  find_package(Boost     REQUIRED COMPONENTS
    ${BOOST_MODULES}
    )

  if (EXISTS "${ODK_ROOT}/3rdparty/pugixml-1.9/scripts")
    AddUniqueTargetFromSubdirectory(pugixml "${ODK_ROOT}/3rdparty/pugixml-1.9/scripts" "3rdparty/pugixml-1.9")
  else()
    get_filename_component(SW_APP_ROOT ../../.. ABSOLUTE)

    if(EXISTS "${SW_APP_ROOT}/3rdparty/pugixml-1.9/scripts")
      AddUniqueTargetFromSubdirectory(pugixml "${SW_APP_ROOT}/3rdparty/pugixml-1.9/scripts" "3rdparty/pugixml-1.9")
    else()
      message(FATAL_ERROR "pugixml library not found")
    endif()
  endif()

  find_package(SDKQt REQUIRED)

endif()

add_subdirectory(odk)
add_subdirectory(examples)

