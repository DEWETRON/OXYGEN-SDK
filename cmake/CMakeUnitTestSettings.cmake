#
# Special settings for unit tests
#

if(my_module_UnitTestSettings_included)
  return()
endif(my_module_UnitTestSettings_included)
set(my_module_UnitTestSettings_included true)

#
# Enable UNIT TESTS on default.
# Manualy disable it by setting:
# WITH_UNIT_TESTS=TRUE environment variable
# or
# -DWITH_UNIT_TESTS=TRUE on the cmake commandline
if (NOT DEFINED WITH_UNIT_TESTS)
  if (DEFINED ENV{WITH_UNIT_TESTS})
    set(WITH_UNIT_TESTS $ENV{WITH_UNIT_TESTS})
  else()
    set(WITH_UNIT_TESTS TRUE)
  endif() 
endif()


#
# Add general suffix to all unit tests names
set(UNIT_TEST_SUFFIX test)

#
# Allow to specialize settings for the unit test projectst
if(WIN32)

  #
  # default use static linking of the boost test
  macro(SetBoostUnitTestFlags TEST_NAME)
    if(3RDPARTY_LINK_SHARED)
      set_property(TARGET ${TEST_NAME}
        APPEND PROPERTY COMPILE_DEFINITIONS
        BOOST_TEST_DYN_LINK
        )
    endif()  
  endmacro()
endif()

if(UNIX)

  if(USE_SYSTEM_BOOST_LIBS)
    #
    # use dynamic linking of the boost test
    macro(SetBoostUnitTestFlags TEST_NAME)
      set_property(TARGET ${TEST_NAME}
        APPEND PROPERTY COMPILE_DEFINITIONS
        BOOST_TEST_DYN_LINK
        )
    endmacro()

  else()
    #
    # default use static linking of the boost test
    macro(SetBoostUnitTestFlags TEST_NAME)
      if(3RDPARTY_LINK_SHARED)
        set_property(TARGET ${TEST_NAME}
          APPEND PROPERTY COMPILE_DEFINITIONS
          BOOST_TEST_DYN_LINK
          )
      endif()  
    endmacro()
  endif()

endif()
