#
# ODK Uni Unit Tests
#
set(TEST_NAME odk_base.${UNIT_TEST_SUFFIX})

#
# System includes have warnings switched off
include_directories(
  SYSTEM
  ${Boost_INCLUDE_DIRS}
)

set(ODKAPI_TEST_SOURCES
  odkbase_api_object_ptr_test.cpp
  test_module.cpp
)
source_group("Test Sources" FILES ${ODKAPI_TEST_SOURCES})

add_executable(${TEST_NAME}
  ${ODKAPI_TEST_SOURCES}
)

SetBoostUnitTestFlags(${TEST_NAME})

target_link_libraries(${TEST_NAME}
  ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
  odk_base
)

#
# add this to Visual Studio group
set_target_properties(${TEST_NAME} PROPERTIES FOLDER "odk/unit_tests")

add_test(NAME ${TEST_NAME}  COMMAND ${TEST_NAME})
