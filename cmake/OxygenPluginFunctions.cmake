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
