#
# ODK API Unit Tests
#
set(TEST_NAME odk_api.${UNIT_TEST_SUFFIX})

#
# System includes have warnings switched off
include_directories(
  SYSTEM
  ${Boost_INCLUDE_DIRS}
)

set(ODKAPI_TEST_SOURCES
  odkapi_acquisition_task_test.cpp
  odkapi_property_test.cpp
  odkapi_version_test.cpp
  test_module.cpp
)
source_group("Test Sources" FILES ${ODKAPI_TEST_SOURCES})

add_executable(${TEST_NAME}
  ${ODKAPI_TEST_SOURCES}
)

SetBoostUnitTestFlags(${TEST_NAME})

target_link_libraries(${TEST_NAME}
  ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
  odk_api
)

#
# add this to Visual Studio group
set_target_properties(${TEST_NAME} PROPERTIES FOLDER "odk/unit_tests")

add_test(NAME ${TEST_NAME}  COMMAND ${TEST_NAME})
