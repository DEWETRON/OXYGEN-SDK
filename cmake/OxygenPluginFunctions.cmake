#
# Custom functions that simplify building of oxygen plugins
#

if(my_module_OxygenPluginFunctions_included)
  return()
endif(my_module_OxygenPluginFunctions_included)
set(my_module_OxygenPluginFunctions_included true)


function(SetPluginOutputOptions TARGETNAME)
    # put library/plugin in plugin directory
    set(OUTPUT_DIR ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/plugins)
    set_target_properties(${TARGETNAME} PROPERTIES OUTPUT_NAME ${TARGETNAME})
    set_target_properties(${TARGETNAME} PROPERTIES SUFFIX ".plugin")
    set_target_properties(${TARGETNAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_DIR}) # linux

    IF(MSVC_IDE)
        set_target_properties(${TARGETNAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release/plugins)
        set_target_properties(${TARGETNAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/RelWithDebInfo/plugins)
        set_target_properties(${TARGETNAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug/plugins)
        set_target_properties(${TARGETNAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/MinSizeRel/plugins)
    ELSE(MSVC_IDE)
        set_target_properties(${TARGETNAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIR})
    ENDIF(MSVC_IDE)
endfunction(SetPluginOutputOptions)

macro(AddResourceFile TGT SRC_FILE OUT_PATH)

    get_filename_component(ABS_SRC_FILE ${SRC_FILE} ABSOLUTE)
    get_filename_component(ABS_OUT_PATH ${OUT_PATH} ABSOLUTE)
    get_filename_component(OUT_NAME ${SRC_FILE} NAME)
    set(OUT_FILE ${ABS_OUT_PATH}/${OUT_NAME})

    add_custom_command(
      OUTPUT ${OUT_FILE}.h
      COMMAND ${CMAKE_COMMAND} -D INPUT_FILE="${ABS_SRC_FILE}" -D OUTPUT_PATH="${ABS_OUT_PATH}" -D OUTPUT_BASENAME="${OUT_NAME}" -D OUTPUT_TYPE=HEADER -P ${ODK_ROOT}/cmake/GenerateCppResource.cmake
      )

    add_custom_command(
      DEPENDS ${SRC_FILE}
      OUTPUT ${OUT_FILE}.cpp
      COMMAND ${CMAKE_COMMAND} -D INPUT_FILE="${ABS_SRC_FILE}" -D OUTPUT_PATH="${ABS_OUT_PATH}" -D OUTPUT_BASENAME="${OUT_NAME}" -D OUTPUT_TYPE=SOURCE -P ${ODK_ROOT}/cmake/GenerateCppResource.cmake
      )

    # tell CMake that the file is generated (maybe it would scan the
    # file for dependencies otherwise), and that it is a header file
    # (for whatever reason)

    set_source_files_properties(
      ${OUT_FILE}.h
      PROPERTIES
      GENERATED TRUE
      HEADER_FILE_ONLY TRUE)

    set_source_files_properties(
      ${OUT_FILE}.cpp
      PROPERTIES
      GENERATED TRUE)

    target_sources(${TGT} PRIVATE ${OUT_FILE}.h ${OUT_FILE}.cpp)

endmacro()

