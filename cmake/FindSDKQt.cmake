if(my_module_Qt_included)
return()
endif(my_module_Qt_included)
set(my_module_Qt_included true)

set(_qt_rcc_candidates 
    "${SDK_QT_PATH}"
    "${ODK_ROOT}/3rdparty/qt"
    "${ODK_ROOT}/qt/qt"
    )

# Win32 specific
foreach(_qt_rcc ${_qt_rcc_candidates})    
    if(EXISTS ${_qt_rcc})
        message(STATUS "Qt rcc found: ${_qt_rcc}/bin/rcc.exe")
        set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${_qt_rcc}/cmake)
        set(Qt5Core_RCC_EXECUTABLE ${_qt_rcc}/bin/rcc.exe)
        include(Qt5CoreMacros)
        break()
    endif()
endforeach()

if (NOT DEFINED Qt5Core_RCC_EXECUTABLE)
    find_package(Qt5Core REQUIRED)
    message(STATUS "Qt found")
endif()
