#
# Oxygen plugin sdk
#

cmake_minimum_required(VERSION 3.1)

#
# handle setup of a cmake toplevel project
# finding libraries etc
if (${CMAKE_CURRENT_SOURCE_DIR} STREQUAL ${CMAKE_SOURCE_DIR})

  #
  # Force C++14
  set(CMAKE_CXX_STANDARD 14)

  #
  # studio project name
  project(ODK)

  if(WIN32)
    set(3RDPARTY_LINK_TYPE "static")
  endif()
  if(UNIX)
    set(3RDPARTY_LINK_TYPE "shared")
  endif()

  get_filename_component(ODK_ROOT . ABSOLUTE)
  message("ODKROOT = ${ODK_ROOT}")
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

  SetBoostOptions() # set static, dynamic ...

  find_package(Boost REQUIRED)

  AddUniqueTargetFromSubdirectory(pugixml "${ODK_ROOT}/3rdparty/pugixml-1.9/scripts" "3rdparty/pugixml-1.9")


endif()

add_subdirectory(odk)

add_subdirectory("examples/replay_sync_scalar")
add_subdirectory("examples/sum_channels")
add_subdirectory("examples/bin_detector")

