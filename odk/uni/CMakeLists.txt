cmake_minimum_required(VERSION 3.1)

set(LIBNAME odk_uni)

include_directories(
  SYSTEM
  ${SW_APP_ROOT}/pugixml-1.9/src
)

include_directories(
  inc
)

set(ODK_UNI_HEADER_FILES
    inc/odkuni_assert.h
    inc/odkuni_bimap.h
    inc/odkuni_defines.h
    inc/odkuni_logger.h
    inc/odkuni_types.h
    inc/odkuni_string_util.h
    inc/odkuni_uuid.h
    inc/odkuni_xpugixml.h
    inc/odkuni_xpugixml_fwd.h
)
source_group("Public Header Files" FILES ${ODK_UNI_HEADER_FILES})

set(ODK_UNI_SOURCE_FILES
    src/odkuni_xpugixml.cpp
)
source_group("Source Files" FILES ${ODK_UNI_SOURCE_FILES})

add_library(${LIBNAME} STATIC ${ODK_UNI_HEADER_FILES} ${ODK_UNI_SOURCE_FILES})

target_link_libraries(${LIBNAME}
    pugixml
)

target_include_directories(${LIBNAME} SYSTEM
  PUBLIC ${SW_APP_ROOT}/pugixml-1.9/src
  )

target_include_directories(${LIBNAME} INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
)

set_target_properties(${LIBNAME} PROPERTIES FOLDER "odk")
set_target_properties(${LIBNAME} PROPERTIES PUBLIC_HEADER "${ODK_UNI_HEADER_FILES}")
if (CLANG_TIDY_COMMAND)
  set_target_properties(${LIBNAME} PROPERTIES CXX_CLANG_TIDY "${CLANG_TIDY_COMMAND}")
endif()

if (WITH_ODK_INSTALL)
  install(TARGETS ${LIBNAME}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/odk
  )
endif()
