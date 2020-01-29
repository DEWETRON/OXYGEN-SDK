#
# Set of custom made practical functions for cmake
#

if(my_module_CmakeFunctions_included)
  return()
endif(my_module_CmakeFunctions_included)
set(my_module_CmakeFunctions_included true)

#
# Debug or trace messages cabe activated with with:
# set(CMAKE_SCRIPT_DEBUG ON)
# Just set it in a CMakeLists.txt file before calling a function or macro!




function(set_library_export_flag TARGETNAME)

  if(WIN32)
    #get type of target
    get_property(_target_type
      TARGET ${TARGETNAME}
      PROPERTY TYPE
      )
    if(${ARGC} EQUAL 2)
      set(_lib_name ${ARGV1})
    else()
      set(_lib_name ${TARGETNAME})
    endif()

    # only add xxxx_LIB for static libs
    # to remove declspec specifiers
    if(_target_type STREQUAL STATIC_LIBRARY)
      message("Setting ${_lib_name}_LIB for ${TARGETNAME}")
      set_property(TARGET ${TARGETNAME}
        APPEND PROPERTY COMPILE_DEFINITIONS
        ${_lib_name}_LIB
        )
    elseif(_target_type STREQUAL SHARED_LIBRARY)
      if(NOT _lib_name STREQUAL ${TARGETNAME})
        message("Setting ${_lib_name}_LIB} for ${TARGETNAME}")
        set_property(TARGET ${TARGETNAME}
          APPEND PROPERTY COMPILE_DEFINITIONS
          ${_lib_name}_LIB
          )
      endif()
    endif()

    # nothing to do for shared libs
    # their export flag is handled by cmake
    endif(WIN32)
endfunction(set_library_export_flag)


#
# Use this macro to set a common output directory for all artifacts
# of a build.
#
macro(SetCommonOutputDirectory)
  if(MSVC)
    if (NOT CMAKE_LIBRARY_OUTPUT_DIRECTORY)
      set(CMAKE_LIBRARY_OUTPUT_DIRECTORY    ${CMAKE_CURRENT_BINARY_DIR})
    endif()
    if (NOT CMAKE_ARCHIVE_OUTPUT_DIRECTORY)
      set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY    ${CMAKE_CURRENT_BINARY_DIR})
    endif()
    if (NOT CMAKE_RUNTIME_OUTPUT_DIRECTORY)
      set(CMAKE_RUNTIME_OUTPUT_DIRECTORY    ${CMAKE_CURRENT_BINARY_DIR})
    endif()
  elseif(APPLE)
    if (XCODE_VERSION)
      if (NOT CMAKE_LIBRARY_OUTPUT_DIRECTORY)
        set(CMAKE_LIBRARY_OUTPUT_DIRECTORY    ${CMAKE_CURRENT_BINARY_DIR})
      endif()
      if (NOT CMAKE_ARCHIVE_OUTPUT_DIRECTORY)
        set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY    ${CMAKE_CURRENT_BINARY_DIR})
      endif()
      if (NOT CMAKE_RUNTIME_OUTPUT_DIRECTORY)
        set(CMAKE_RUNTIME_OUTPUT_DIRECTORY    ${CMAKE_CURRENT_BINARY_DIR})
      endif()
    else()
      if (NOT CMAKE_LIBRARY_OUTPUT_DIRECTORY)
        set(CMAKE_LIBRARY_OUTPUT_DIRECTORY    ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE})
      endif()
      if (NOT CMAKE_ARCHIVE_OUTPUT_DIRECTORY)
        set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY    ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE})
      endif()
      if (NOT CMAKE_RUNTIME_OUTPUT_DIRECTORY)
        set(CMAKE_RUNTIME_OUTPUT_DIRECTORY    ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE})
      endif()
    endif()
  else()
    if (NOT CMAKE_LIBRARY_OUTPUT_DIRECTORY)
      set(CMAKE_LIBRARY_OUTPUT_DIRECTORY    ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE})
    endif()
    if (NOT CMAKE_ARCHIVE_OUTPUT_DIRECTORY)
      set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY    ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE})
    endif()
    if (NOT CMAKE_RUNTIME_OUTPUT_DIRECTORY)
      set(CMAKE_RUNTIME_OUTPUT_DIRECTORY    ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE})
    endif()
  endif()

endmacro()

#
# Set the boost link options
# Windows: static build and static runtime
# Linux: dynamic libs(from OS) and dynamic runtime
macro(SetBoostOptions)
  if (WIN32)
    if(3RDPARTY_LINK_SHARED)
      set(Boost_USE_STATIC_LIBS    OFF)
    else()
      set(Boost_USE_STATIC_LIBS    ON)
    endif()

    set(Boost_USE_MULTITHREADED  ON)

    if(3RDPARTY_LINK_SHAREDRT)
      set(Boost_USE_STATIC_RUNTIME OFF)
    else()
      set(Boost_USE_STATIC_RUNTIME ON)
    endif()
  endif()


  if (UNIX)

    # Handle macosx -> tell the compiler used
    if (APPLE)
      if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
        set(Boost_COMPILER "-xgcc42")
      endif()
    else()
      if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
        if (NOT DEFINED Boost_COMPILER)
          #set(Boost_COMPILER "-gcc49")
          # want to determine the version of the installed gcc/g++
          # to be able to use the same "versioned" libs with clang
          # limit to major.minor => 4.9.2 --> 4.9
          execute_process(
            COMMAND g++ -dumpversion
            OUTPUT_VARIABLE GCC_VERSION
            )
          string(REGEX REPLACE "\\." "" GCC_VERSION ${GCC_VERSION})
          string(LENGTH ${GCC_VERSION} GCC_VERSION_LEN)
          if (${GCC_VERSION_LEN} GREATER 2)
            string(SUBSTRING ${GCC_VERSION} 0 2 GCC_VERSION)
          endif()
          set(Boost_COMPILER "-gcc${GCC_VERSION}")
        endif()
      endif()
    endif()

    # We may depend on boost of the current linux distribution
    if(USE_SYSTEM_BOOST_LIBS)
      set(Boost_USE_STATIC_LIBS    OFF)
      set(Boost_USE_MULTITHREADED  ON)
      set(Boost_USE_STATIC_RUNTIME OFF)
    else()
      # Or we use our own boost:
      if(3RDPARTY_LINK_SHARED)
        set(Boost_USE_STATIC_LIBS    OFF)
      else()
        set(Boost_USE_STATIC_LIBS    ON)
      endif()

      set(Boost_USE_MULTITHREADED  ON)

      #
      # ignore static runtime for now
      # does not build well in some settings

      # if(3RDPARTY_LINK_SHAREDRT)

      # set(Boost_USE_STATIC_RUNTIME OFF)
      # else()
      #   set(Boost_USE_STATIC_RUNTIME ON)
      # endif()

      set(Boost_USE_STATIC_RUNTIME OFF)

    endif()
  endif()

  if (CMAKE_SCRIPT_DEBUG)
      message(STATUS "BOOST_ROOT = ${BOOST_ROOT}")
      message(STATUS "Boost_USE_STATIC_LIBS    = ${Boost_USE_STATIC_LIBS}")
      message(STATUS "Boost_USE_MULTITHREADED  = ${Boost_USE_MULTITHREADED}")
      message(STATUS "Boost_USE_STATIC_RUNTIME = ${Boost_USE_STATIC_RUNTIME}")
  endif()
endmacro()


macro(AddGenerateComputerInfoFile CMP_FILE SW_ROOT)
  get_filename_component(OUT_DIR ${CMP_FILE} PATH)
  add_custom_command(OUTPUT ${CMP_FILE}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${OUT_DIR}
    COMMAND python ${SW_ROOT}/build_util/BuildTool/computer_info.py ${CMP_FILE}
  )

  # tell CMake that the file is generated (maybe it would scan the
  # file for dependencies otherwise), and that it is a header file
  # (for whatever reason)

  set_source_files_properties(
      ${VER_FILE}
      PROPERTIES
          GENERATED TRUE
 	  HEADER_FILE_ONLY TRUE)

endmacro()


#
# Add a manual copy step after target build completed
# param LIBNAME is the target name
# param CREATE_CMD custom command name
# param SRC_DIR relative path to a file/directory starting from CMAKE_CURRENT_SOURCE_DIR
# param DEST_DIR relative path to a file/directory starting from CMAKE_BINARY_DIR
macro(AppendDirCopyStep LIBNAME CREATE_LST_CMD SRC_DIR DEST_DIR)
  message(STATUS "AppendDirCopyStep ${LIBNAME} ${CREATE_LST_CMD} ${SRC_DIR} ${DEST_DIR}")
  add_custom_command(
    OUTPUT ${CREATE_LST_CMD} APPEND
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${SRC_DIR} ${DEST_DIR}
    )
endmacro()

macro(AppendDirCopyStepFullDestPath LIBNAME CREATE_LST_CMD SRC_DIR DEST_DIR)
  message(STATUS "AppendDirCopyStepFullDestPath ${LIBNAME} ${CREATE_LST_CMD} ${SRC_DIR} ${DEST_DIR}")
  add_custom_command(
    OUTPUT ${CREATE_LST_CMD} APPEND
    COMMAND ${CMAKE_COMMAND} -E echo "AppendDirCopyStepFullDestPath ${LIBNAME} ${CREATE_LST_CMD} ${SRC_DIR} ${DEST_DIR}"
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${SRC_DIR} ${DEST_DIR}
    )
endmacro()


#
# Enable improved floating point precision
macro(SetFloatingPointPrecision LIBNAME)
  #Floating Point precision Switch
  #modify this section to
  #if(MSVC)
  #   try compile with /Op
  #   try to compile with /fp:precise
  #endif
  #pick the option, that does not throw a error/warning
  #issue an error, if none of them satisfy the above condition
  if(MSVC)
    #message(${CMAKE_CXX_COMPILER_ID})
    if(CMAKE_COMPILER_2005 OR MSVC90 OR MSVC10 OR MSVC11 OR MSVC12 OR MSVC80 OR MSVC14)
      set( FP_PRECISE /fp:precise)
    elseif( MSVC70 OR MSVC71 )
      set( FP_PRECISE /Op)
    else()
      message( FATAL_ERROR "Unable to determine compiler switch for improved floatingpoint-precision")
    endif()

    #improved floating point consistency
    set_property(TARGET ${LIBNAME}
      APPEND
      PROPERTY COMPILE_FLAGS
      ${FP_PRECISE}
      )
  endif()
endmacro()


#
# Set Debug/Release .. as define
macro(SetBuildConfigFlag LIBNAME)
  if(MSVC)
    if(${CMAKE_GENERATOR} STREQUAL "Ninja")
      set_property(TARGET ${LIBNAME}
        APPEND
        PROPERTY COMPILE_DEFINITIONS
        _BUILDCONFIG="${CMAKE_BUILD_TYPE}"
        _CRT_SECURE_NO_WARNINGS
      )
    else()
      set_property(TARGET ${LIBNAME}
        APPEND
        PROPERTY COMPILE_DEFINITIONS
        _BUILDCONFIG="$(Configuration)"
        _CRT_SECURE_NO_WARNINGS
      )
    endif()
  else()
    set_property(TARGET ${LIBNAME}
      APPEND
      PROPERTY COMPILE_DEFINITIONS
      _BUILDCONFIG="${CMAKE_BUILD_TYPE}"
      )
  endif()
endmacro()


#
# The binary should use static runtime
macro(SetLinkStaticRuntime)

  if (MSVC)
    set(CMAKE_CXX_FLAGS_DEBUG          "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
    set(CMAKE_CXX_FLAGS_MINSIZEREL     "${CMAKE_CXX_FLAGS_MINSIZEREL} /MT")
    set(CMAKE_CXX_FLAGS_RELEASE        "${CMAKE_CXX_FLAGS_RELEASE} /MT")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} /MT")

    set(CMAKE_C_FLAGS_DEBUG            "${CMAKE_C_FLAGS_DEBUG} /MTd")
    set(CMAKE_C_FLAGS_MINSIZEREL       "${CMAKE_C_FLAGS_MINSIZEREL} /MT")
    set(CMAKE_C_FLAGS_RELEASE          "${CMAKE_C_FLAGS_RELEASE} /MT")
    set(CMAKE_C_FLAGS_RELWITHDEBINFO   "${CMAKE_C_FLAGS_RELWITHDEBINFO} /MT")
  else()
    message(WARNING "SetLinkStaticRuntime is not supported if not using MSVC")
  endif()

endmacro()

macro(SetLinkSharedRuntime)
  if (MSVC)
    set(CMAKE_CXX_FLAGS_DEBUG          "${CMAKE_CXX_FLAGS_DEBUG} /MDd")
    set(CMAKE_CXX_FLAGS_MINSIZEREL     "${CMAKE_CXX_FLAGS_MINSIZEREL} /MD")
    set(CMAKE_CXX_FLAGS_RELEASE        "${CMAKE_CXX_FLAGS_RELEASE} /MD")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} /MD")

    set(CMAKE_C_FLAGS_DEBUG            "${CMAKE_C_FLAGS_DEBUG} /MDd")
    set(CMAKE_C_FLAGS_MINSIZEREL       "${CMAKE_C_FLAGS_MINSIZEREL} /MD")
    set(CMAKE_C_FLAGS_RELEASE          "${CMAKE_C_FLAGS_RELEASE} /MD")
    set(CMAKE_C_FLAGS_RELWITHDEBINFO   "${CMAKE_C_FLAGS_RELWITHDEBINFO} /MD")
  endif()
endmacro()

macro(SetFullOptimization)
  set(ADDITIONAL_COMPILE_FLAGS_FOR_RELEASE "")
  if(MSVC)
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
      set(ADDITIONAL_COMPILE_FLAGS_FOR_RELEASE "/GS-")
    else()
      set(ADDITIONAL_COMPILE_FLAGS_FOR_RELEASE "/GS- /arch:SSE2")
    endif()
  endif()
  if(UNIX)
    if (NOT APPLE)
      # gcc
      set(ADDITIONAL_COMPILE_FLAGS_FOR_RELEASE "-msse -msse2 -mfpmath=sse")
    else()
      # clang
      set(ADDITIONAL_COMPILE_FLAGS_FOR_RELEASE "-msse -msse2")
    endif()
  endif()

  set(CMAKE_CXX_FLAGS_MINSIZEREL     "${CMAKE_CXX_FLAGS_MINSIZEREL} ${ADDITIONAL_COMPILE_FLAGS_FOR_RELEASE}")
  set(CMAKE_CXX_FLAGS_RELEASE        "${CMAKE_CXX_FLAGS_RELEASE} ${ADDITIONAL_COMPILE_FLAGS_FOR_RELEASE}")
  set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} ${ADDITIONAL_COMPILE_FLAGS_FOR_RELEASE}")

  set(CMAKE_C_FLAGS_MINSIZEREL       "${CMAKE_C_FLAGS_MINSIZEREL} ${ADDITIONAL_COMPILE_FLAGS_FOR_RELEASE}")
  set(CMAKE_C_FLAGS_RELEASE          "${CMAKE_C_FLAGS_RELEASE} ${ADDITIONAL_COMPILE_FLAGS_FOR_RELEASE}")
  set(CMAKE_C_FLAGS_RELWITHDEBINFO   "${CMAKE_C_FLAGS_RELWITHDEBINFO} ${ADDITIONAL_COMPILE_FLAGS_FOR_RELEASE}")
endmacro()


#
# Helper functions for install rules
#

#
# SetUsedRuntimeSharedLibraries
# Tries to use a given list of external libraries to determine its dll name
# It looks if the dll exists and returns a list of full path to dll.
#
macro(SetUsedRuntimeSharedLibraries VAR_LIST_NAME BIN_DIR LIB_LIST)

  set(SHARED_LIB_LIST)
  set(is_optimized TRUE)

  foreach(f ${LIB_LIST})
    if(f STREQUAL "optimized")
      set(is_optimized TRUE)
    elseif(f STREQUAL "debug")
      set(is_optimized FALSE)
    else()

      if(is_optimized)
        get_filename_component(LIB_NAME ${f} NAME_WE ${f})

        set(T_VAR_LIST_NAME "${BIN_DIR}/${LIB_NAME}.dll")

        if(EXISTS ${T_VAR_LIST_NAME})
          list(APPEND SHARED_LIB_LIST ${T_VAR_LIST_NAME})
        else()
          message("Warning: ${T_VAR_LIST_NAME} does not exist")
        endif()
      endif()
    endif()
  endforeach()

#  message("SHARED_LIB_LIST = ${SHARED_LIB_LIST}")
  set(${VAR_LIST_NAME} ${SHARED_LIB_LIST})

endmacro()

#
# SetUsedRuntimeSharedLibrariesByTool
# Similar to SetUsedRuntimeSharedLibraries but tries to deduce dependencies
# using ldd or dumpbin
# param VAR_LIST_NAME
# param TARGET_FILE_NAME
# param BIN_DIRS
macro(SetUsedRuntimeSharedLibrariesByTool VAR_LIST_NAME TARGET_FILE_NAME BIN_DIRS)
  include(GetPrerequisites)

  set(SHARED_LIB_LIST)

  # hints where to find dumpbin
  set(gp_cmd_paths
    ${gp_cmd_paths}
    "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Wow6432Node\\Microsoft\\VisualStudio\\10.0;InstallDir]/../../VC/bin"
    )

  set(PRE_REQ)
  set(OPT_RECURSIVE 1)
  set(OPT_SYSTEM    1)

  get_prerequisites(${TARGET_FILE_NAME} PRE_REQ ${OPT_SYSTEM} ${OPT_RECURSIVE} "" "${BIN_DIRS}")

  if (CMAKE_SCRIPT_DEBUG)
    message("PRE_REQ = ${PRE_REQ}")
  endif()

  foreach(f ${PRE_REQ})

    unset(T_VAR_LIST_NAME) #from set()
    unset(T_VAR_LIST_NAME CACHE) #from find_path()
    find_path(T_VAR_LIST_NAME "${f}"
      ${BIN_DIRS}
      NO_DEFAULT_PATH
    )
    set(T_VAR_LIST_NAME "${T_VAR_LIST_NAME}/${f}")

    if (CMAKE_SCRIPT_DEBUG)
      message("${f} => ${T_VAR_LIST_NAME}")
    endif()

    if(EXISTS ${T_VAR_LIST_NAME})
      list(APPEND SHARED_LIB_LIST ${T_VAR_LIST_NAME})
    else()
      message("Warning: ${T_VAR_LIST_NAME} does not exist")
    endif()
  endforeach()

  set(${VAR_LIST_NAME} ${SHARED_LIB_LIST})

  if (CMAKE_SCRIPT_DEBUG)
    message("VAR_LIST_NAME = ${VAR_LIST_NAME}")
  endif()

endmacro()


#
# Similar to InstallRequiredSystemLibraries
# Install all dependent 3rdparty libs
# param TARGET_FILE_NAME
# param DIST_DIR
# param BIN_DIRS
#
macro(InstallRequiredSharedLibraries TARGET_FILE_NAME DIST_DIR BIN_DIRS)

  install(CODE "

   #
   # Begin InstallRequiredSharedLibraries ${TARGET_FILE_NAME} ${BIN_DIRS}
   #

   set(CMAKE_MODULE_PATH
       ${CMAKE_MODULE_PATH}
       ${SW_APP_ROOT}/build_util/cmake
   )

   include(CMakeFunctions)

   #set(CMAKE_SCRIPT_DEBUG ON)

   SetUsedRuntimeSharedLibrariesByTool(3RDPARTY_RUNTIME_LIBS ${TARGET_FILE_NAME} \"${BIN_DIRS}\")

   #message(\"3RDPARTY_RUNTIME_LIBS = \${3RDPARTY_RUNTIME_LIBS}\")
   FILE(INSTALL DESTINATION \"${DIST_DIR}\" TYPE FILE FILES \${3RDPARTY_RUNTIME_LIBS})

   #
   # End InstallRequiredSharedLibraries ${TARGET_FILE_NAME} ${BIN_DIRS}
   #
   "
    COMPONENT RUNTIME)

endmacro()


#
# Standard link libraries for unit tests
# param TEST_NAME
# not defined params:
# param WIN_LIBS List of additional libs for windows
# param LIN_LIBS List of additional libs for linux
# param OSX_LIBS List of additional libs for osx
macro(SetStandardLinkLibrariesForTest TEST_NAME)

  # copy extra arguments to be usable as list
  set(extra_macro_args ${ARGN})
  list(LENGTH extra_macro_args num_extra_macro_args)

  set(WIN_LIBS "")
  set(LIN_LIBS "")
  set(OSX_LIBS "")

  if (num_extra_macro_args GREATER 0)
    list(GET extra_macro_args 0 T_WIN_LIBS)
    string(REPLACE " " ";" WIN_LIBS ${T_WIN_LIBS})
  endif()

  if (num_extra_macro_args GREATER 1)
    list(GET extra_macro_args 1 T_LIN_LIBS)
    string(REPLACE " " ";" LIN_LIBS ${T_LIN_LIBS})
  endif()

  if (num_extra_macro_args GREATER 2)
    list(GET extra_macro_args 2 T_OSX_LIBS)
    string(REPLACE " " ";" OSX_LIBS ${T_OSX_LIBS})
  endif()

  if(WIN32)
    #
    # Libraries for windows
    target_link_libraries(${TEST_NAME}
      ws2_32.lib
      ${WIN_LIBS}
      )
  endif()

  if(UNIX)
    if(APPLE)
      #
      # Libraries for apple/darwin/osx
      if(NOT ${OSX_LIBS} STREQUAL "")
	    target_link_libraries(${TEST_NAME}
	      ${OSX_LIBS}
	      )
      endif()
    else()
      #
      # Libraries for Linux
      target_link_libraries(${TEST_NAME}
	    dl
	    rt
	    ${LIN_LIBS}
        pthread
	    )
    endif()

  endif()

endmacro()

#
# Check if the given library exists.
#  - checks if the variable containing the lib is set.
macro(TargetLinkLibrariesChecked TARGET)

  set(_dep_libs "")

  foreach(_lib ${ARGN})
    if(NOT ${_lib})
      message(FATAL_ERROR "${_lib} not set")
    else()
      list(APPEND _dep_libs ${${_lib}})
    endif()
  endforeach()

  target_link_libraries(${TARGET} ${_dep_libs})

  if (CMAKE_SCRIPT_DEBUG)
    message(STATUS "${TARGET} ${_dep_libs}")
  endif()
endmacro()

#
# "Extract" boost dlls from boost libs
# Note: Macro only takes release libs!!!
macro(GetDllFromLib DLL_LIST)
  foreach(_lib ${ARGN})
    if(${_lib} MATCHES "(mt-gd)" OR ${_lib} MATCHES "(optimized)" OR ${_lib} MATCHES "(debug)")
      # do nothing ...
    else()
      string(REPLACE ".lib" ".dll" _dll ${_lib})
      list(APPEND DLL_LIST ${_dll})
    endif()
  endforeach()
endmacro()


#
# replace __ varviables (from us) with their correct cmake counterpart
macro(UpdateUnderscoredPaths VERSION_SOURCE_FILES)
  unset(VERSION_SOURCE_FILES)
  foreach(_p ${ARGN})
    string(REPLACE "__CMAKE_CURRENT_BINARY_DIR__" "${CMAKE_CURRENT_BINARY_DIR}" _r ${_p})
    list(APPEND VERSION_SOURCE_FILES ${_r})
  endforeach()

  list(REMOVE_DUPLICATES VERSION_SOURCE_FILES)
endmacro()

#
# Assign a stable GUID to visual studio 2013 projects
macro(SetProjectGuid LIBNAME GUID)
# http://stackoverflow.com/questions/28959488/how-to-enable-incremental-builds-for-vs-2013-with-cmake-and-long-target-names
  set("${LIBNAME}_GUID_CMAKE" ${GUID} CACHE INTERNAL "remove this and Visual Studio will mess up incremental builds")
endmacro()


#
# make sure a target is built exactly once
macro(AddUniqueTargetFromSubdirectory TARGET_NAME SOURCE_DIR BINARY_DIR)
  if(NOT TARGET ${TARGET_NAME})
    add_subdirectory(${SOURCE_DIR} ${BINARY_DIR})
  endif()
endmacro()

macro(AddUniqueTargetFromSubdirectory_EFA TARGET_NAME SOURCE_DIR BINARY_DIR)
  if(NOT TARGET ${TARGET_NAME})
    add_subdirectory(${SOURCE_DIR} ${BINARY_DIR} EXCLUDE_FROM_ALL)
  endif()
endmacro()

#
# add subdirectory if cond is TRUE
macro(add_subdirectory_if cond par1)

  if (${cond})
    message(STATUS "adding ${par1}")
    add_subdirectory(${par1} ${ARGN})
  else()
    message(STATUS "omitting ${par1}")
  endif()
  
endmacro()

#
# add subdirectory if cond is FALSE
macro(add_subdirectory_not_if cond par1)

  if (NOT ${cond})
    message(STATUS "adding ${par1}")
    add_subdirectory(${par1} ${ARGN})
  else()
    message(STATUS "omitting ${par1}")
  endif()
  
endmacro()


#
# Determine COMPUTERNAME USERNAME from the current environment
macro(GetUserName uname)
if (WIN32)
  set(${uname}     $ENV{USERNAME})
endif()
if (UNIX)
  set(${uname}     $ENV{USER})
endif()
endmacro()

macro(get_arch ARCH)
  set(_ARCH "x64")  #default
  if (BUILD_X86)
    set(_ARCH "x86")
  endif()
  if (BUILD_X64)
    set(_ARCH "x64")
  endif()
endmacro()

