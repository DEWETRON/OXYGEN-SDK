#=============================================================================
# Copyright 2005-2011 Kitware, Inc.
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
# * Redistributions of source code must retain the above copyright
#   notice, this list of conditions and the following disclaimer.
#
# * Redistributions in binary form must reproduce the above copyright
#   notice, this list of conditions and the following disclaimer in the
#   documentation and/or other materials provided with the distribution.
#
# * Neither the name of Kitware, Inc. nor the names of its
#   contributors may be used to endorse or promote products derived
#   from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
# A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
# HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#=============================================================================

######################################
#
#       Macros for building Qt files
#
######################################

include(CMakeParseArguments)

# macro used to create the names of output files preserving relative dirs
macro(QT5_MAKE_OUTPUT_FILE infile prefix ext outfile )
    string(LENGTH ${CMAKE_CURRENT_BINARY_DIR} _binlength)
    string(LENGTH ${infile} _infileLength)
    set(_checkinfile ${CMAKE_CURRENT_SOURCE_DIR})
    if(_infileLength GREATER _binlength)
        string(SUBSTRING "${infile}" 0 ${_binlength} _checkinfile)
        if(_checkinfile STREQUAL "${CMAKE_CURRENT_BINARY_DIR}")
            file(RELATIVE_PATH rel ${CMAKE_CURRENT_BINARY_DIR} ${infile})
        else()
            file(RELATIVE_PATH rel ${CMAKE_CURRENT_SOURCE_DIR} ${infile})
        endif()
    else()
        file(RELATIVE_PATH rel ${CMAKE_CURRENT_SOURCE_DIR} ${infile})
    endif()
    if(WIN32 AND rel MATCHES "^([a-zA-Z]):(.*)$") # absolute path
        set(rel "${CMAKE_MATCH_1}_${CMAKE_MATCH_2}")
    endif()
    set(_outfile "${CMAKE_CURRENT_BINARY_DIR}/${rel}")
    string(REPLACE ".." "__" _outfile ${_outfile})
    get_filename_component(outpath ${_outfile} PATH)
    get_filename_component(_outfile ${_outfile} NAME_WE)
    file(MAKE_DIRECTORY ${outpath})
    set(${outfile} ${outpath}/${prefix}${_outfile}.${ext})
endmacro()


# _qt5_parse_qrc_file(infile _out_depends _rc_depends)
# internal

function(_QT5_PARSE_QRC_FILE infile _out_depends _rc_depends)
    get_filename_component(rc_path ${infile} PATH)

    if(EXISTS "${infile}")
        #  parse file for dependencies
        #  all files are absolute paths or relative to the location of the qrc file
        file(READ "${infile}" RC_FILE_CONTENTS)
        string(REGEX MATCHALL "<file[^<]+" RC_FILES "${RC_FILE_CONTENTS}")
        foreach(RC_FILE ${RC_FILES})
            string(REGEX REPLACE "^<file[^>]*>" "" RC_FILE "${RC_FILE}")
            if(NOT IS_ABSOLUTE "${RC_FILE}")
                set(RC_FILE "${rc_path}/${RC_FILE}")
            endif()
            set(RC_DEPENDS ${RC_DEPENDS} "${RC_FILE}")
        endforeach()
        # Since this cmake macro is doing the dependency scanning for these files,
        # let's make a configured file and add it as a dependency so cmake is run
        # again when dependencies need to be recomputed.
        qt5_make_output_file("${infile}" "" "qrc.depends" out_depends)
        configure_file("${infile}" "${out_depends}" COPYONLY)
    else()
        # The .qrc file does not exist (yet). Let's add a dependency and hope
        # that it will be generated later
        set(out_depends)
    endif()

    set(${_out_depends} ${out_depends} PARENT_SCOPE)
    set(${_rc_depends} ${RC_DEPENDS} PARENT_SCOPE)
endfunction()


# qt5_add_binary_resources(target inputfiles ... )

function(QT5_ADD_BINARY_RESOURCES target )

    set(options)
    set(oneValueArgs DESTINATION)
    set(multiValueArgs OPTIONS)

    cmake_parse_arguments(_RCC "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    set(rcc_files ${_RCC_UNPARSED_ARGUMENTS})
    set(rcc_options ${_RCC_OPTIONS})
    set(rcc_destination ${_RCC_DESTINATION})

    if(NOT rcc_destination)
        set(rcc_destination ${CMAKE_CURRENT_BINARY_DIR}/${target}.rcc)
    endif()

    foreach(it ${rcc_files})
        get_filename_component(infile ${it} ABSOLUTE)

        _QT5_PARSE_QRC_FILE(${infile} _out_depends _rc_depends)
        set_source_files_properties(${infile} PROPERTIES SKIP_AUTORCC ON)
        set(infiles ${infiles} ${infile})
        set(out_depends ${out_depends} ${_out_depends})
        set(rc_depends ${rc_depends} ${_rc_depends})
    endforeach()

    add_custom_command(OUTPUT ${rcc_destination}
                       COMMAND ${Qt5Core_RCC_EXECUTABLE}
                       ARGS ${rcc_options} --binary --name ${target} --output ${rcc_destination} ${infiles}
                       DEPENDS ${rc_depends} ${out_depends} ${infiles} VERBATIM)
    add_custom_target(${target} ALL DEPENDS ${rcc_destination})
endfunction()

