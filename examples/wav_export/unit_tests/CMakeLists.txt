#
# ex_wav_export Tests

if (CMAKE_COMPILER_IS_GNUCC AND CMAKE_CXX_COMPILER_VERSION VERSION_EQUAL 4.4.7)
  message(WARNING "uni_utils.test not runnable with gcc ${CMAKE_CXX_COMPILER_VERSION}")
  return()
endif()


set(TEST_NAME ex_wav_export.${UNIT_TEST_SUFFIX})
SetProjectGuid(${TEST_NAME} "408257D0-53BA-44CA-B650-D8234DBB74A6")

#
# System includes have warnings switched off
include_directories(
  SYSTEM
  ${Boost_INCLUDE_DIRS}
  ..
)

set(UNIT_TEST_SOURCES
  test_module.cpp
  ../inc/wav_writer.h
  ../src/wav_writer.cpp
  wav_writer_test.cpp
)

add_executable(${TEST_NAME}
  ${UNIT_TEST_SOURCES}
)


#
# -D Defines
set_property(TARGET ${TEST_NAME}
  APPEND
  PROPERTY COMPILE_DEFINITIONS
)

target_link_libraries(${TEST_NAME}
  ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
  ex_wav_export
)


#
# add this to Visual Studio group UnitTests
set_target_properties(${TEST_NAME} PROPERTIES FOLDER "odk_examples/ex_wav_export")



add_test(NAME ${TEST_NAME}
  COMMAND ${TEST_NAME}
)
