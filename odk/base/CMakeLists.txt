#
# CMakeLists.txt for odk base
#
cmake_minimum_required(VERSION 3.1)

set(LIBNAME odk_base)

include_directories(
  ${Boost_INCLUDE_DIRS}
)


set(ODK_BASE_HEADER_FILES
    odkbase_api_object_ptr.h
    odkbase_basic_values.h
    odkbase_if_api_object.h
    odkbase_if_host.h
    odkbase_if_host_fwd.h
    odkbase_if_plugin.h
    odkbase_if_value.h
    odkbase_message_return_value_holder.h
)
source_group("Public C++ Header Files" FILES ${ODK_BASE_HEADER_FILES})

add_library(${LIBNAME} STATIC ${ODK_BASE_HEADER_FILES} odkbase_dummy.cpp)

target_link_libraries(${LIBNAME}
    odk_uni
)

target_include_directories(${LIBNAME} INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${Boost_INCLUDE_DIRS}
)

set_target_properties(${LIBNAME} PROPERTIES FOLDER "odk")
