#
# ex_replay_message Tests

set(TEST_NAME ex_replay_message.${UNIT_TEST_SUFFIX})

#
# System includes have warnings switched off
include_directories(
  SYSTEM
  ${Boost_INCLUDE_DIRS}
)

include_directories(
  ../../../examples/shared
)

set(UNIT_TEST_SOURCES
  test_module.cpp
  ../../../examples/shared/sdk_csv_utils.h
  ../../../examples/shared/sdk_csv_utils.cpp
  csv_utils_test.cpp
)

add_executable(${TEST_NAME}
  ${UNIT_TEST_SOURCES}
)


target_link_libraries(${TEST_NAME}
  ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
)


#
# add this to Visual Studio group UnitTests
set_target_properties(${TEST_NAME} PROPERTIES FOLDER "odk_examples/ex_replay_message")

add_test(NAME ${TEST_NAME}
  COMMAND ${TEST_NAME}
)
