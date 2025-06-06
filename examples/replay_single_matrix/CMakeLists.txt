#
# Oxygen example plugin "replay single matrix"
# Reads data from a CSV file and plays it back to oxygen as a single-value matrix channel.
#
cmake_minimum_required(VERSION 3.10)

# Name of the plugin project and compiled plugin file
set(LIBNAME ex_replay_single_matrix)
# This is just any stable GUID to help Visual Studio identify the project for rebuilds
set("${LIBNAME}_GUID_CMAKE" "102B673A-0F6B-457D-B1D4-19174865215B" CACHE INTERNAL "remove this and Visual Studio will mess up incremental builds")

#
# handle setup of a cmake toplevel project
# finding libraries etc
if (${CMAKE_CURRENT_SOURCE_DIR} STREQUAL ${CMAKE_SOURCE_DIR})

  # project name
  project(${LIBNAME})

  get_filename_component(ODK_ROOT "../.." ABSOLUTE)
  message("ODKROOT = ${ODK_ROOT}")
  # expand cmake search path to check for project settings
  set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ODK_ROOT}/cmake )

  include(CMakeSettings)
  include(OxygenPluginFunctions)

  SetLinkStaticRuntime()
  SetCommonOutputDirectory()
  SetBoostOptions()
  find_package(Boost REQUIRED)

  AddUniqueTargetFromSubdirectory(pugixml "${SW_APP_ROOT}/3rdparty/pugixml-1.9/scripts" "3rdparty/pugixml-1.9")
  AddUniqueTargetFromSubdirectory(odk "${ODK_ROOT}/odk" "odk")
else()
  include(OxygenPluginFunctions)
endif()


include_directories(
  SYSTEM
  ${Boost_INCLUDE_DIRS}
)

include_directories(
  ../../examples/shared
)

set(SOURCE_FILES
  odkex_replay_single_matrix.cpp
  ../../examples/shared/sdk_csv_utils.cpp
  ../../examples/shared/sdk_csv_utils.h
)
source_group("Source Files" FILES ${SOURCE_FILES})

add_library(${LIBNAME} SHARED
  ${SOURCE_FILES}
)

target_link_libraries(${LIBNAME}
  odk_framework
)

SetPluginOutputOptions(${LIBNAME})

#
# add this to Visual Studio group lib
set_target_properties(${LIBNAME} PROPERTIES FOLDER "odk_examples")

