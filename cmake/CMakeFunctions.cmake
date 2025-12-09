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
    #message(WARNING "SetLinkStaticRuntime is not supported if not using MSVC")
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


macro(get_arch ARCH)
  set(_ARCH "x64")  #default
  if (BUILD_X86)
    set(_ARCH "x86")
  endif()
  if (BUILD_X64)
    set(_ARCH "x64")
  endif()
endmacro()

#
# SetupODKEnvironment
# Setup:
# * Runtime
# * Output directory options
# * Boost Settings
# * XML
macro(SetupODKEnvironment OXYGEN_SDK_PATH)

  SetLinkStaticRuntime()
  SetCommonOutputDirectory()
  find_package(SDKQt REQUIRED)

  AddUniqueTargetFromSubdirectory(pugixml "${OXYGEN_SDK_PATH}/3rdparty/pugixml-1.9/scripts" "3rdparty/pugixml-1.9")

endmacro()