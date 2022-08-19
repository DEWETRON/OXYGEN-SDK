#
# Oxygen example plugin "sum channels"
# Demonstrates adding two oxygen channels and writing the result to an output channel
#
cmake_minimum_required(VERSION 3.1)

# Name of the plugin project and compiled plugin file
set(LIBNAME ex_sum_channels)
# This is just any stable GUID to help Visual Studio identify the project for rebuilds
set("${LIBNAME}_GUID_CMAKE" "3F33F346-D476-4B03-8F62-F38F0DD29A7B" CACHE INTERNAL "remove this and Visual Studio will mess up incremental builds")

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


set(SOURCE_FILES
  odkex_sum_channels.cpp
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

