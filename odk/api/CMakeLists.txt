#
# CMakeLists.txt for odk api
#
cmake_minimum_required(VERSION 3.1)

set(LIBNAME odk_api)

include_directories(
  inc
)

set(ODK_API_HEADER_FILES
    inc/odkapi_acquisition_task_xml.h
    inc/odkapi_block_descriptor_xml.h
    inc/odkapi_block_descriptor_xml_fwd.h
    inc/odkapi_channel_config_changed_xml.h
    inc/odkapi_channel_dataformat_xml.h
    inc/odkapi_channel_list_xml.h
    inc/odkapi_channel_mapping_xml.h
    inc/odkapi_config_item_keys.h
    inc/odkapi_data_set_xml.h
    inc/odkapi_data_set_descriptor_xml.h
    inc/odkapi_data_set_descriptor_xml_fwd.h
    inc/odkapi_error_codes.h
    inc/odkapi_event_ids.h
    inc/odkapi_export_xml.h
    inc/odkapi_marker_xml.h
    inc/odkapi_message_ids.h
    inc/odkapi_message_ids.inc
    inc/odkapi_node_list_xml.h
    inc/odkapi_oxygen_queries.h
    inc/odkapi_property_xml.h
    inc/odkapi_property_list_xml.h
    inc/odkapi_software_channel_xml.h
    inc/odkapi_timebase_xml.h
    inc/odkapi_timestamp_xml.h
    inc/odkapi_types.h
    inc/odkapi_update_channels_xml.h
    inc/odkapi_update_config_xml.h
    inc/odkapi_utils.h
    inc/odkapi_version_xml.h
)
source_group("Public Header Files" FILES ${ODK_API_HEADER_FILES})

set(ODK_API_SOURCE_FILES
    src/assert_bimap_size.h
    src/odkapi_acquisition_task_xml.cpp
    src/odkapi_block_descriptor_xml.cpp
    src/odkapi_channel_config_changed_xml.cpp
    src/odkapi_channel_dataformat_xml.cpp
    src/odkapi_channel_list_xml.cpp
    src/odkapi_channel_mapping_xml.cpp
    src/odkapi_data_set_xml.cpp
    src/odkapi_data_set_descriptor_xml.cpp
    src/odkapi_export_xml.cpp
    src/odkapi_marker_xml.cpp
    src/odkapi_property_xml.cpp
    src/odkapi_property_list_xml.cpp
    src/odkapi_software_channel_xml.cpp
    src/odkapi_timebase_xml.cpp
    src/odkapi_timestamp_xml.cpp
    src/odkapi_types.cpp
    src/odkapi_update_channels_xml.cpp
    src/odkapi_update_config_xml.cpp
    src/odkapi_version_xml.cpp
)
source_group("Source Files" FILES ${ODK_API_SOURCE_FILES})

add_library(${LIBNAME} STATIC ${ODK_API_HEADER_FILES} ${ODK_API_SOURCE_FILES})

target_link_libraries(${LIBNAME}
    odk_base
    odk_uni
)

target_include_directories(${LIBNAME} INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
)

set_target_properties(${LIBNAME} PROPERTIES FOLDER "odk")

