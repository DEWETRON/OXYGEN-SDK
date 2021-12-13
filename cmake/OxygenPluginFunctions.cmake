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

macro(AddTranslationResources TGT SRC_DIR OUT_PATH)
    get_filename_component(ABS_SRC_DIR ${SRC_DIR} ABSOLUTE)

    file(GLOB _TS_FILES CONFIGURE_DEPENDS "${ABS_SRC_DIR}/*.ts")
    message("Translation = ${_TS_FILES}")
    foreach(_TS_FILE ${_TS_FILES})
        AddResourceFile(${TGT} "${_TS_FILE}" "${OUT_PATH}")
    endforeach()
endmacro()

function(FindTranslationFiles SRC_DIR PRIMARY_LANGUAGE PRIMARY_FILE OTHER_FILES)
    message("FindTranslationFiles(${SRC_DIR}, ${PRIMARY_LANGUAGE})")
    file(GLOB _TS_FILES1 CONFIGURE_DEPENDS "${SRC_DIR}/*_${PRIMARY_LANGUAGE}.ts")
    file(GLOB _TS_FILES CONFIGURE_DEPENDS "${SRC_DIR}/*.ts")
    list(LENGTH _TS_FILES1 _TS_FILES1_LEN)
    if(NOT ${_TS_FILES1_LEN} EQUAL 1)
        message(FATAL_ERROR "Primary translation file was not found")
    endif()
    list(GET _TS_FILES1 0 _TS_FILE1)
    message("Primary translation = ${_TS_FILE1}")
    list(REMOVE_ITEM _TS_FILES "${_TS_FILE1}" _TS_FILES)
    message("Other translations = ${_TS_FILES}")
    set(${PRIMARY_FILE} ${_TS_FILE1} PARENT_SCOPE)
    set(${OTHER_FILES} ${_TS_FILES} PARENT_SCOPE)
endfunction()

macro(AddUpdateTranslations TGT SRC_DIR PRIMARY_LANGUAGE)
    FindTranslationFiles(${SRC_DIR} ${PRIMARY_LANGUAGE} PRIMARY_TS OTHER_TS)

	if (Qt5_LCONVERT_EXECUTABLE)
		foreach(_TS_FILE ${OTHER_TS})
			get_filename_component(_OUTPUT_BASENAME ${_TS_FILE} NAME)
			string(REGEX MATCH ".*_(..)\\.ts" _LANG_CODE ${_TS_FILE})
			set(_LANG_CODE ${CMAKE_MATCH_1})

			ADD_CUSTOM_TARGET(
				update_translation_${_LANG_CODE}
				COMMAND ${Qt5_LCONVERT_EXECUTABLE} -i ${PRIMARY_TS} ${_TS_FILE} -source-language en -target-language ${_LANG_CODE} -no-obsolete -o ${_TS_FILE}
				COMMAND ${Unix2Dos}
				#SOURCES ${TRANSLATION_FILES}
				WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${LANG_CODE}
				)
			set_target_properties(update_translation_${_LANG_CODE} PROPERTIES EXCLUDE_FROM_DEFAULT_BUILD 1)
		endforeach()
	endif()
endmacro()

macro(AddTranslationList TGT SRC_DIR PRIMARY_LANGUAGE OUT_PATH)
    FindTranslationFiles(${SRC_DIR} ${PRIMARY_LANGUAGE} PRIMARY_TS OTHER_TS)
    get_filename_component(ABS_OUT_PATH "${OUT_PATH}" ABSOLUTE)
    set(OUTPUT_HEADER ${ABS_OUT_PATH}/all_translations.h)
    set(OUTPUT_FILE ${ABS_OUT_PATH}/all_translations.cpp)
    file(REMOVE ${OUTPUT_HEADER})
    file(REMOVE ${OUTPUT_FILE})

	#Write header
	file(APPEND ${OUTPUT_HEADER} "#pragma once\n")
	file(APPEND ${OUTPUT_HEADER} "#include \"odkbase_if_host_fwd.h\"\n")
    file(APPEND ${OUTPUT_HEADER} "namespace plugin_resources {\n")
    file(APPEND ${OUTPUT_HEADER} "  bool addAllTranslations(odk::IfHost* host);\n")
    file(APPEND ${OUTPUT_HEADER} "}\n")

	#Write source
	file(APPEND ${OUTPUT_FILE} "#include \"all_translations.h\"\n")
	file(APPEND ${OUTPUT_FILE} "#include \"odkapi_message_ids.h\"\n")
	file(APPEND ${OUTPUT_FILE} "#include \"odkapi_utils.h\"\n")

	get_filename_component(_OUTPUT_BASENAME ${PRIMARY_TS} NAME)
	file(APPEND ${OUTPUT_FILE} "#include \"${_OUTPUT_BASENAME}.h\"\n")

    foreach(_TS_FILE ${OTHER_TS})
        get_filename_component(_OUTPUT_BASENAME ${_TS_FILE} NAME)
        file(APPEND ${OUTPUT_FILE} "#include \"${_OUTPUT_BASENAME}.h\"\n")
    endforeach()

    file(APPEND ${OUTPUT_FILE} "\n")

    file(APPEND ${OUTPUT_FILE} "namespace {\n")
	file(APPEND ${OUTPUT_FILE} "  bool addTranslation(odk::IfHost* host, const char* translation_xml) {\n")
    file(APPEND ${OUTPUT_FILE} "    if (odk::sendSyncXMLMessage(host, odk::host_msg::ADD_TRANSLATION, 0, translation_xml, strlen(translation_xml) + 1, nullptr)) {\n")
    file(APPEND ${OUTPUT_FILE} "      return false;\n")
    file(APPEND ${OUTPUT_FILE} "    }\n")
    file(APPEND ${OUTPUT_FILE} "    return true;\n")
	file(APPEND ${OUTPUT_FILE} "  }\n")
    file(APPEND ${OUTPUT_FILE} "}\n")


    file(APPEND ${OUTPUT_FILE} "namespace plugin_resources {\n")

    file(APPEND ${OUTPUT_FILE} "  bool addAllTranslations(odk::IfHost* host) {\n")

	get_filename_component(_VARIABLE_NAME "${PRIMARY_TS}" NAME)
	# Replace filename spaces & extension separator for C compatibility
	string(REGEX REPLACE "\\.| |-" "_" _VARIABLE_NAME ${_VARIABLE_NAME})
	string(TOUPPER ${_VARIABLE_NAME} _VARIABLE_NAME)
	file(APPEND ${OUTPUT_FILE} "    bool success = addTranslation(host, reinterpret_cast<const char*>(${_VARIABLE_NAME}_data));\n")

    foreach(_TS_FILE ${OTHER_TS})
		get_filename_component(_VARIABLE_NAME "${_TS_FILE}" NAME)
		# Replace filename spaces & extension separator for C compatibility
		string(REGEX REPLACE "\\.| |-" "_" _VARIABLE_NAME ${_VARIABLE_NAME})
		string(TOUPPER ${_VARIABLE_NAME} _VARIABLE_NAME)
		file(APPEND ${OUTPUT_FILE} "    success = addTranslation(host, reinterpret_cast<const char*>(${_VARIABLE_NAME}_data)) && success;\n")
    endforeach()

    file(APPEND ${OUTPUT_FILE} "    return success;\n")
    file(APPEND ${OUTPUT_FILE} "  }\n")
    file(APPEND ${OUTPUT_FILE} "}\n")
    file(APPEND ${OUTPUT_FILE} "\n")

    set_source_files_properties(
      ${OUTPUT_HEADER}
      PROPERTIES
      GENERATED TRUE
      HEADER_FILE_ONLY TRUE)

    set_source_files_properties(
      ${OUTPUT_FILE}
      PROPERTIES
      GENERATED TRUE)

    target_sources(${TGT} PRIVATE ${OUTPUT_HEADER} ${OUTPUT_FILE})

endmacro()

