cmake_minimum_required (VERSION 3.8)

# based on https://devblogs.microsoft.com/cppblog/clear-functional-c-documentation-with-sphinx-breathe-doxygen-cmake/

# Requires the follwing installed components:
# Python
# Doxygen
# Sphinx (pip install -U sphinx)
#   http://www.sphinx-doc.org/en/master/index.html
# RTD-Theme for Sphinx (pip install sphinx_rtd_theme)
# Breathe (pip install breathe)
#   https://breathe.readthedocs.io/en/latest/
# Exhale (pip install exhale)
#   https://exhale.readthedocs.io/en/latest/index.html


project("OxygenPluginAPIDocs")

# Add the cmake folder so the FindSphinx module is found
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

set(USE_DOXYGEN OFF)


#set(DOXYGEN_INPUT_DIR ${PROJECT_SOURCE_DIR}/CatCutifier)
set(DOXYFILE_IN ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)
set(DOXYFILE_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

set(SPHINX_SOURCE ${CMAKE_CURRENT_SOURCE_DIR})
set(SPHINX_BUILD ${CMAKE_CURRENT_BINARY_DIR}/docs/sphinx)
set(SPHINX_INDEX_FILE ${SPHINX_BUILD}/index.html)

find_package(Sphinx REQUIRED)

if(USE_DOXYGEN)

    find_package(Doxygen REQUIRED)

    set(DOXYGEN_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/docs/doxygen)
    set(DOXYGEN_INDEX_FILE ${DOXYGEN_OUTPUT_DIR}/xml/index.xml)

    # Find all the public headers
    #get_target_property(CAT_CUTIFIER_PUBLIC_HEADER_DIR CatCutifier INTERFACE_INCLUDE_DIRECTORIES)
    #file(GLOB_RECURSE CAT_CUTIFIER_PUBLIC_HEADERS ${CAT_CUTIFIER_PUBLIC_HEADER_DIR}/*.h)

    #Replace variables inside @@ with the current values
    configure_file(${DOXYFILE_IN} ${DOXYFILE_OUT} @ONLY)

    file(MAKE_DIRECTORY ${DOXYGEN_OUTPUT_DIR}) #Doxygen won't create this for us

    add_custom_command(OUTPUT ${DOXYGEN_INDEX_FILE}
                       #DEPENDS ${CAT_CUTIFIER_PUBLIC_HEADERS}
                       COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYFILE_OUT}
                       MAIN_DEPENDENCY ${DOXYFILE_OUT} ${DOXYFILE_IN}
                       COMMENT "Generating docs"
                       VERBATIM)
    add_custom_target(Doxygen ALL DEPENDS ${DOXYGEN_INDEX_FILE})
endif()

file(GLOB MAIN_DOC_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.rst")
file(GLOB EXAMPLES_DOC_FILES "${CMAKE_CURRENT_SOURCE_DIR}/../examples/*/*.rst")


set(ALL_DOC_FILES ${MAIN_DOC_FILES} ${EXAMPLES_DOC_FILES})

# Only regenerate Sphinx when:
# - Doxygen has rerun
# - Our doc files have been updated
# - The Sphinx config has been updated
add_custom_command(OUTPUT ${SPHINX_INDEX_FILE}
                   COMMAND 
                     ${SPHINX_EXECUTABLE} -b html -j auto -a
                     # Tell Breathe where to find the Doxygen output
                     -Dbreathe_projects.OxygenSDK=${DOXYGEN_OUTPUT_DIR}/xml
                   ${SPHINX_SOURCE} ${SPHINX_BUILD}
                   WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                   DEPENDS
                   # Other docs files you want to track should go here (or in some variable)
                   ${ALL_DOC_FILES}
                   ${DOXYGEN_INDEX_FILE}
                   MAIN_DEPENDENCY ${SPHINX_SOURCE}/conf.py
                   COMMENT "Generating documentation with Sphinx")
add_custom_target(ODKDocs ALL DEPENDS ${SPHINX_INDEX_FILE} SOURCES ${ALL_DOC_FILES})

# Add an install target to install the docs
#include(GNUInstallDirs)
#install(DIRECTORY ${SPHINX_BUILD}
#DESTINATION ${CMAKE_INSTALL_DOCDIR})


