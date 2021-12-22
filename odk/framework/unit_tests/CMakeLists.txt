#
# ODK Framework Unit Tests
#
set(TEST_NAME odk_framework.${UNIT_TEST_SUFFIX})

#
# System includes have warnings switched off
include_directories(
  SYSTEM
  ${Boost_INCLUDE_DIRS}
)

set(ODKFW_TEST_SOURCES
  odkfw_export_instance_test.cpp
  test_module.cpp
  test_host.h
  test_host.cpp
  values.h
)
source_group("Test Sources" FILES ${ODKFW_TEST_SOURCES})

add_executable(${TEST_NAME}
  ${ODKFW_TEST_SOURCES}
)

SetBoostUnitTestFlags(${TEST_NAME})

target_link_libraries(${TEST_NAME}
  ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
  odk_framework
)

#
# add this to Visual Studio group
set_target_properties(${TEST_NAME} PROPERTIES FOLDER "odk/unit_tests")

add_test(NAME ${TEST_NAME}  COMMAND ${TEST_NAME})
