#
# Oxygen example plugin "wav export"
#
cmake_minimum_required(VERSION 3.1)

# Name of the plugin project and compiled plugin file
set(LIBNAME ex_wav_export)
# This is just any stable GUID to help Visual Studio identify the project for rebuilds
set("${LIBNAME}_GUID_CMAKE" "60e4627b-8964-4259-a9e0-54d38847ae68" CACHE INTERNAL "remove this and Visual Studio will mess up incremental builds")

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

find_package(SDKQt REQUIRED)

include_directories(
  SYSTEM
  ${Boost_INCLUDE_DIRS}
)

include_directories(
  ../shared
  inc
)

set(SOURCE_FILES
  inc/wav_writer.h
  src/wav_writer.cpp
  odkex_wav_export.cpp
)
source_group("Source Files" FILES ${SOURCE_FILES})

add_library(${LIBNAME} SHARED
  ${SOURCE_FILES}
)

target_link_libraries(${LIBNAME}
  odk_framework
  odk_uni
)

target_include_directories(${LIBNAME} INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
)

qt5_add_binary_resources(${LIBNAME}_qml qml.qrc DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/qml.rcc")

set(RESOURCE_DIR ${CMAKE_CURRENT_BINARY_DIR}/resources)
include_directories(${RESOURCE_DIR})
AddResourceFile(${LIBNAME} "${CMAKE_CURRENT_BINARY_DIR}/qml.rcc" ${RESOURCE_DIR})


SetPluginOutputOptions(${LIBNAME})

#
# add this to Visual Studio group lib
set_target_properties(${LIBNAME} PROPERTIES FOLDER "odk_examples/ex_wav_export")
set_target_properties(${LIBNAME}_qml PROPERTIES FOLDER "odk_examples/ex_wav_export")

if (NOT GITHUB_REPO)
  add_subdirectory(unit_tests)
endif()
