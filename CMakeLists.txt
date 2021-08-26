########################################################################
# CMake project

CMAKE_MINIMUM_REQUIRED(VERSION 3.13 FATAL_ERROR)
# set (CMAKE_VERBOSE_MAKEFILE ON)

IF (POLICY CMP0020)
	CMAKE_POLICY(SET CMP0020 NEW)
ENDIF ()

IF (POLICY CMP0015)
	CMAKE_POLICY(SET CMP0015 NEW)
ENDIF ()

SET_PROPERTY(GLOBAL PROPERTY USE_FOLDERS ON)

########################################################################
# Ensure that we are not building in our source directories.

SET(Build_Dir_OK "TRUE")
STRING(REGEX MATCH "^${CMAKE_SOURCE_DIR}" In_Sub_Dir ${CMAKE_BINARY_DIR})
IF (In_Sub_Dir)
  STRING(REGEX MATCH "^${CMAKE_SOURCE_DIR}/[Bb]uild" In_Build_Dir ${CMAKE_BINARY_DIR})
  IF (NOT In_Build_Dir)
    SET(Build_Dir_OK "FALSE")
  ENDIF ()
ENDIF ()

IF (NOT Build_Dir_OK)
  MESSAGE(FATAL_ERROR "You must run cmake from a directory that is not in your source tree, or that is in a special subdirectory of the tree whose name begins with 'build'.")
ENDIF ()

SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/CMake/") 

########################################################################

SET(EVE_APPS_GRAPHICS "MULTI" CACHE STRING "Graphics Module")
SET_PROPERTY(CACHE EVE_APPS_GRAPHICS PROPERTY STRINGS "VM816C" "VM810C" "VM800C" "MULTI")
 
SET(EVE_APPS_PLATFORM "MULTI" CACHE STRING "Platform Target")
SET_PROPERTY(CACHE EVE_APPS_PLATFORM PROPERTY STRINGS "BT8XXEMU" "FT4222" "MPSSE" "MM900EV3A" "RP2040" "MULTI")

IF(EVE_APPS_PLATFORM STREQUAL "RP2040")

  ########################################################################
  # Build for Raspberry Pi Pico
  
  # Pull in Pico SDK (must be before project)
  INCLUDE(pico_sdk_import.cmake)
  
  PROJECT("EVE_Apps" C CXX ASM)

ELSE()

  PROJECT("EVE_Apps" C)

ENDIF()

########################################################################
# Project
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

########################################################################
# Set up the basic build environment

IF (CMAKE_BUILD_TYPE STREQUAL "")
  # CMake defaults to leaving CMAKE_BUILD_TYPE empty. This messes up
  # differentiation between debug and release builds.
  SET(CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE STRING "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel." FORCE)
ENDIF ()

IF(NOT EVE_APPS_PLATFORM STREQUAL "RP2040")

  ########################################################################
  # Strip binary in release mode
  
  IF (CMAKE_COMPILER_IS_GNUCXX)
    SET(CMAKE_EXE_LINKER_FLAGS_RELEASE "-s")
  ENDIF ()
  
  ########################################################################
  
  SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
  SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
  SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
  
  ########################################################################
  
  IF(CMAKE_SIZEOF_VOID_P EQUAL 8)
    LINK_DIRECTORIES(${CMAKE_SOURCE_DIR}/common/eve_hal/Bin/Simulation/x64)
    LINK_DIRECTORIES(${CMAKE_SOURCE_DIR}/common/eve_hal/Bin/MSVC/x64)
  ELSE()
    LINK_DIRECTORIES(${CMAKE_SOURCE_DIR}/common/eve_hal/Bin/Simulation)
    LINK_DIRECTORIES(${CMAKE_SOURCE_DIR}/common/eve_hal/Bin/MSVC)
  ENDIF()

ELSE()
  
  ########################################################################
  # Initialise the Pico SDK
  pico_sdk_init()

ENDIF()

########################################################################

INCLUDE_DIRECTORIES("common/eve_hal")
INCLUDE_DIRECTORIES("common/eve_hal/Hdr")

########################################################################

IF(NOT EVE_APPS_PLATFORM STREQUAL "MULTI")
  IF(EVE_APPS_PLATFORM STREQUAL "BT8XXEMU")
    ADD_DEFINITIONS(-DEVE_PLATFORM_BT8XXEMU)
  ELSEIF(EVE_APPS_PLATFORM STREQUAL "FT4222")
    ADD_DEFINITIONS(-DEVE_PLATFORM_FT4222)
  ELSEIF(EVE_APPS_PLATFORM STREQUAL "MPSSE")
    ADD_DEFINITIONS(-DEVE_PLATFORM_MPSSE)
  ELSEIF(EVE_APPS_PLATFORM STREQUAL "MM900EV3A")
    ADD_DEFINITIONS(-DMM900EV3A)
  ELSEIF(EVE_APPS_PLATFORM STREQUAL "RP2040")
    ADD_DEFINITIONS(-DEVE_PLATFORM_RP2040)
  ELSE()
    ADD_DEFINITIONS("-D${EVE_APPS_PLATFORM}")
  ENDIF()
ENDIF()

IF(NOT EVE_APPS_GRAPHICS STREQUAL "MULTI")
  IF(EVE_APPS_GRAPHICS STREQUAL "VM816C")
    ADD_DEFINITIONS(-DEVE_GRAPHICS_VM816C)
  ELSEIF(EVE_APPS_GRAPHICS STREQUAL "VM810C")
    ADD_DEFINITIONS(-DEVE_GRAPHICS_VM810C)
  ELSEIF(EVE_APPS_GRAPHICS STREQUAL "VM800C")
    ADD_DEFINITIONS(-DEVE_GRAPHICS_VM800C)
  ELSE()
    ADD_DEFINITIONS("-D${EVE_APPS_GRAPHICS}")
  ENDIF()
ENDIF()

if(DEFINED EVE_DISPLAY)
    ADD_DEFINITIONS("-D${EVE_DISPLAY}")
ENDIF()

ADD_SUBDIRECTORY(common)
ADD_SUBDIRECTORY(test_eve_hal)
ADD_SUBDIRECTORY(test_esd_core)
ADD_SUBDIRECTORY(eve_tools)

IF(NOT EVE_APPS_GRAPHICS STREQUAL "MULTI")
  ADD_SUBDIRECTORY(EveApps)
ENDIF()

########################################################################
