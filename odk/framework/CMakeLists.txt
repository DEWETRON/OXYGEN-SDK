#
# CMakeLists.txt for odk framework
#
include(CMakeSettings)

set(LIBNAME odk_framework)

include_directories(
    SYSTEM
    ${Boost_INCLUDE_DIRS}
)

include_directories(
  inc
)

set(HEADER_FILES
  inc/odkfw_block_iterator.h
  inc/odkfw_channels.h
  inc/odkfw_custom_request_handler.h
  inc/odkfw_data_requester.h
  inc/odkfw_exceptions.h
  inc/odkfw_export_instance.h
  inc/odkfw_export_plugin.h
  inc/odkfw_fwd.h
  inc/odkfw_if_message_handler.h
  inc/odkfw_input_channel.h
  inc/odkfw_interfaces.h
  inc/odkfw_plugin_base.h
  inc/odkfw_properties.h
  inc/odkfw_property_list_utils.h
  inc/odkfw_software_channel_instance.h
  inc/odkfw_software_channel_plugin.h
  inc/odkfw_stream_iterator.h
  inc/odkfw_stream_reader.h
  inc/odkfw_version_check.h
)
source_group("Header Files" FILES ${HEADER_FILES})

set(SOURCE_FILES
  src/odkfw_block_iterator.cpp
  src/odkfw_channels.cpp
  src/odkfw_custom_request_handler.cpp
  src/odkfw_data_requester.cpp
  src/odkfw_export_instance.cpp
  src/odkfw_export_plugin.cpp
  src/odkfw_input_channel.cpp
  src/odkfw_plugin_base.cpp
  src/odkfw_properties.cpp
  src/odkfw_property_list_utils.cpp
  src/odkfw_stream_iterator.cpp
  src/odkfw_stream_reader.cpp
  src/odkfw_software_channel_instance.cpp
  src/odkfw_software_channel_plugin.cpp
  src/odkfw_version_check.cpp
)
source_group("Source Files" FILES ${SOURCE_FILES})

set(LINK_DEPENDENCIES
  odk_api
  odk_uni
)

add_library(${LIBNAME} STATIC
  ${HEADER_FILES}
  ${SOURCE_FILES}
)

target_link_libraries(${LIBNAME}
  ${LINK_DEPENDENCIES}
)

target_include_directories(
    ${LIBNAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
)

#
# add this to Visual Studio group lib
set_target_properties(${LIBNAME} PROPERTIES FOLDER "odk")

# Unit Tests
add_subdirectory(unit_tests)
