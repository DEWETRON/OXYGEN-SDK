#
# Oxygen example plugin "replay sync scalar"
# Reads data from a CSV file and plays it back to oxygen as a synchronous scalar channel.
#
cmake_minimum_required(VERSION 3.10)

# Name of the plugin project and compiled plugin file
set(LIBNAME ex_replay_sync_scalar)
# This is just any stable GUID to help Visual Studio identify the project for rebuilds
set("${LIBNAME}_GUID_CMAKE" "9954B143-99EF-4272-8DB6-2CAFA9F7BC40" CACHE INTERNAL "remove this and Visual Studio will mess up incremental builds")

#
# handle setup of a cmake toplevel project
# finding libraries etc
if (${CMAKE_CURRENT_SOURCE_DIR} STREQUAL ${CMAKE_SOURCE_DIR})

  #
  # Force C++17
  set(CMAKE_CXX_STANDARD 17)

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

  AddUniqueTargetFromSubdirectory(pugixml "${SW_APP_ROOT}/3rdparty/pugixml-1.9/scripts" "3rdparty/pugixml-1.9")
  AddUniqueTargetFromSubdirectory(odk "${ODK_ROOT}/odk" "odk")
else()
  include(OxygenPluginFunctions)
endif()

find_package(SDKQt REQUIRED)

include_directories(
  ../shared
)

set(SOURCE_FILES
  odkex_replay_sync_scalar.cpp
  ../shared/sdk_csv_utils.cpp
  ../shared/sdk_csv_utils.h
)
source_group("Source Files" FILES ${SOURCE_FILES})

add_library(${LIBNAME} SHARED
  ${SOURCE_FILES}
)

target_link_libraries(${LIBNAME}
  odk_framework
)

qt5_add_binary_resources(${LIBNAME}_qml qml.qrc DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/qml.rcc")

set(RESOURCE_DIR ${CMAKE_CURRENT_BINARY_DIR}/resources)
include_directories(${RESOURCE_DIR})
AddResourceFile(${LIBNAME} "${CMAKE_CURRENT_BINARY_DIR}/qml.rcc" ${RESOURCE_DIR})

SetPluginOutputOptions(${LIBNAME})

#
# add this to Visual Studio group lib
set_target_properties(${LIBNAME} PROPERTIES FOLDER "odk_examples/ex_replay_sync_scalar")
set_target_properties(${LIBNAME}_qml PROPERTIES FOLDER "odk_examples/ex_replay_sync_scalar")

