#Parameters:
#  INPUT_FILE       source for the content that is stored in the resource file
#  OUTPUT_PATH      path where files are stored
#  OUTPUT_BASENAME  output filename without extension (.h or .cpp are appended)
#  OUTPUT_TYPE      HEADER or SOURCE

if ("${OUTPUT_TYPE}" STREQUAL "HEADER")
  set(OUTPUT_FILE ${OUTPUT_PATH}/${OUTPUT_BASENAME}.h)
elseif ("${OUTPUT_TYPE}" STREQUAL "SOURCE")
  set(OUTPUT_FILE ${OUTPUT_PATH}/${OUTPUT_BASENAME}.cpp)
else()
  error(FATAL_ERROR "Invalid OUTPUT_TYPE")
endif()

file(REMOVE ${OUTPUT_FILE})

get_filename_component(VARIABLE_NAME ${INPUT_FILE} NAME)
# Replace filename spaces & extension separator for C compatibility
string(REGEX REPLACE "\\.| |-" "_" VARIABLE_NAME ${VARIABLE_NAME})
string(TOUPPER ${VARIABLE_NAME} VARIABLE_NAME)

message("Creating resource ${VARIABLE_NAME} (${INPUT_FILE} => ${OUTPUT_FILE})")

get_filename_component(OUTPUT_DIR ${OUTPUT_FILE} PATH)
file(MAKE_DIRECTORY ${OUTPUT_DIR})

file(WRITE ${OUTPUT_FILE} "")

if ("${OUTPUT_TYPE}" STREQUAL "HEADER")

  file(APPEND ${OUTPUT_FILE} "namespace plugin_resources {\n")
  file(APPEND ${OUTPUT_FILE} "  extern const unsigned char* ${VARIABLE_NAME}_data;\n")
  file(APPEND ${OUTPUT_FILE} "  extern const unsigned ${VARIABLE_NAME}_size;\n")
  file(APPEND ${OUTPUT_FILE} "}\n")

elseif ("${OUTPUT_TYPE}" STREQUAL "SOURCE")

  file(APPEND ${OUTPUT_FILE} "#include \"${OUTPUT_BASENAME}.h\"\n")
  file(APPEND ${OUTPUT_FILE} "namespace plugin_resources {\n")
  file(APPEND ${OUTPUT_FILE} "  const unsigned char ${VARIABLE_NAME}_bytes[] = {\n")

  set(file_offset 0)
  set(max_block_size 1048576)
  set(block_size ${max_block_size})
  while(block_size EQUAL max_block_size)
    file(READ ${INPUT_FILE} filedata OFFSET ${file_offset} LIMIT ${max_block_size} HEX)
    string(LENGTH "${filedata}" block_size)
    math(EXPR block_size ${block_size}/2)

    string(REGEX REPLACE "([0-9a-f][0-9a-f])" "0x\\1," filedata "${filedata}")
    file(APPEND ${OUTPUT_FILE} "${filedata}\n")

    math(EXPR file_offset "${file_offset} + ${block_size}")
  endwhile(block_size EQUAL max_block_size)

  set(filesize ${file_offset})

  file(APPEND ${OUTPUT_FILE} "0x00};\n")
  file(APPEND ${OUTPUT_FILE} "  const unsigned char* ${VARIABLE_NAME}_data = ${VARIABLE_NAME}_bytes;\n")
  file(APPEND ${OUTPUT_FILE} "  const unsigned ${VARIABLE_NAME}_size = ${filesize};\n")
  file(APPEND ${OUTPUT_FILE} "}\n")

endif()

