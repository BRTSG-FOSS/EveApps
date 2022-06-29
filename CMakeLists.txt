# CMake project ################################################################
#
# 1. Build for FT9XX example: Edit the FT9XX_TOOLCHAIN location path at first
#  cd build && cmake  -G "MinGW Makefiles" -DEVE_APPS_PLATFORM=MM900EV3A -DEVE_APPS_GRAPHICS=BT817 -DEVE_APPS_DISPLAY=WXGA -DCMAKE_BUILD_TYPE=Debug "-DFT9XX_TOOLCHAIN=C:\Program Files (x86)\Bridgetek\FT9xx Toolchain" ..
#  cmake --build ./ --target DemoCircleView VERBOSE=1
#
# 2. Build for RP2040 example:
#  cd build && cmake  -G "NMake Makefiles" -DEVE_APPS_PLATFORM=RP2040 -DEVE_APPS_GRAPHICS=BT817 -DEVE_APPS_DISPLAY=WXGA -DCMAKE_BUILD_TYPE=Debug ..
#  nmake SampleApp
#
# 3. Build for FT4222 example: Use Deveoper Command Promt For VS
#  cd build && cmake  -G "NMake Makefiles" -DEVE_APPS_PLATFORM=FT4222 -DEVE_APPS_GRAPHICS=BT817 -DEVE_APPS_DISPLAY=WXGA -DCMAKE_BUILD_TYPE=Debug ..
#  nmake SampleApp
#
# 4. Build for MPSSE example: Use Deveoper Command Promt For VS
#  cd build && cmake  -G "NMake Makefiles" -DEVE_APPS_PLATFORM=MPSSE -DEVE_APPS_GRAPHICS=BT817 -DEVE_APPS_DISPLAY=WXGA -DCMAKE_BUILD_TYPE=Debug ..
#  nmake SampleApp
#
# 5. Build for EMULATOR example: Use Deveoper Command Promt For VS
#  cd build && cmake  -G "NMake Makefiles" -DEVE_APPS_PLATFORM=EMU -DEVE_APPS_GRAPHICS=BT817 -DEVE_APPS_DISPLAY=WVGA -DCMAKE_BUILD_TYPE=Debug ..
#  nmake SampleApp
#

CMAKE_MINIMUM_REQUIRED(VERSION 3.13 FATAL_ERROR)
#Uncomment to see gcc stdout
#set (CMAKE_VERBOSE_MAKEFILE ON)
IF (POLICY CMP0020)
	CMAKE_POLICY(SET CMP0020 NEW)
ENDIF ()

IF (POLICY CMP0015)
	CMAKE_POLICY(SET CMP0015 NEW)
ENDIF ()

SET_PROPERTY(GLOBAL PROPERTY USE_FOLDERS ON)
define_property(
  TARGET
  PROPERTY FOLDER
  INHERITED
  BRIEF_DOCS "Set the folder name."
  FULL_DOCS  "Use to organize targets in an IDE."
)

# Ensure that we are not building in our source directories. ###################
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

# Options ######################################################################
SET(WITH_EVE_TESTS OFF CACHE BOOL "Build EVE test projects")
SET(WITH_ESD_CORE OFF CACHE BOOL "Build ESD Core library")
SET(WITH_ESD_LITTLEFS OFF CACHE BOOL "Build ESD Core library with LittleFS support")

# Valid option input ###########################################################
set(LIST_GRAPHICS MULTI
                  VM800B35A_BK
                  VM800B43A_BK
                  VM800B50A_BK
                  VM801B43A_BK
                  VM801B50A_BK
                  VM800C35A_N
                  VM800C43A_N
                  VM800C35A_D
                  VM800C43A_D
                  VM800C50A_D
                  ME812A_WH50R
                  ME812AU_WH50R
                  ME813A_WH50C
                  ME813AU_WH50C
                  VM810C50A_D
                  VM810C50A_N
                  ME810A_HV35R
                  VM816C50A_D
                  VM816C50A_N
                  VM816CU50A_D
                  VM816CU50A_N
                  ME810AU_HV35R
                  ME813A_WV7C
                  PANL35
                  PANL50
                  PANL70
                  PANL70PLUS
                  EVE_GRAPHICS_VM800C
                  EVE_GRAPHICS_VM810C
                  EVE_GRAPHICS_VM816C
                  EVE_GRAPHICS_ME817EV
                  EVE_GRAPHICS_FT800
                  EVE_GRAPHICS_FT801
                  EVE_GRAPHICS_FT810
                  EVE_GRAPHICS_FT811
                  EVE_GRAPHICS_FT812
                  EVE_GRAPHICS_FT813
                  EVE_GRAPHICS_BT880
                  EVE_GRAPHICS_BT881
                  EVE_GRAPHICS_BT882
                  EVE_GRAPHICS_BT883
                  EVE_GRAPHICS_BT815
                  EVE_GRAPHICS_BT816
                  EVE_GRAPHICS_BT817
                  EVE_GRAPHICS_BT818
  )
set(LIST_PLATFORM MULTI
                  DEFAULT
                  MM900EV1A
                  MM900EV1B
                  MM900EV2A
                  MM900EV3A
                  MM900EV_LITE
                  MM930MINI
                  MM930LITE
                  MM932LC
                  FT900_PLATFORM
                  FT93X_PLATFORM
                  FT4222_PLATFORM
                  MPSSE_PLATFORM
                  BT8XXEMU_PLATFORM
                  RP2040_PLATFORM
  )
set(LIST_DISPLAY  MULTI
                  DEFAULT
                  EVE_DISPLAY_QVGA
                  EVE_DISPLAY_WQVGA
                  EVE_DISPLAY_WVGA
                  EVE_DISPLAY_WSVGA
                  EVE_DISPLAY_WXGA
                  EVE_DISPLAY_ILI9488_HVGA_PORTRAIT
                  EVE_DISPLAY_KD2401_HVGA_PORTRAIT
  )

# Set cache ####################################################################
SET(EVE_APPS_GRAPHICS "MULTI" CACHE STRING "Graphics Module")
SET_PROPERTY(CACHE EVE_APPS_GRAPHICS PROPERTY STRINGS ${LIST_GRAPHICS})

# TODO: Set a better default here automatically when CMake is run with an embedded platform
SET(EVE_APPS_PLATFORM "MULTI" CACHE STRING "Platform Target")
SET_PROPERTY(CACHE EVE_APPS_PLATFORM PROPERTY STRINGS ${LIST_PLATFORM})

SET(EVE_APPS_DISPLAY "DEFAULT" CACHE STRING "Platform Target")
SET_PROPERTY(CACHE EVE_APPS_DISPLAY PROPERTY STRINGS ${LIST_DISPLAY})

# Validate input ###############################################################
function(give_sugguest value array global_set)
  FOREACH(item ${${array}})
    STRING(REGEX MATCH ".*${value}.*" search_val "${item}")
    IF(search_val)
      MESSAGE(WARNING "Notice: ${value} is not defined, selected this similar configure: ${search_val}")
      set(${global_set} "${search_val}" CACHE STRING "Graphics Module" FORCE)
      break()
    ENDIF()
  ENDFOREACH()
endfunction()
  
function(validate_input option_name array)
  set(value "${${option_name}}")
  
  list(FIND ${array} "${value}" _index)
  STRING(REPLACE ";"  ", " list_str "${${array}}")
  if (${_index} EQUAL -1)
    # give sugguestion
    give_sugguest(${value} ${array} "${option_name}")
    
    # if give_sugguest works ok, then value of option_name is changed, if no, stop cmake with error message
    if ("${value}" STREQUAL "${${option_name}}")
      MESSAGE(WARNING "\nError: ${option_name} is not set, the valid ${option_name} is: ")
      MESSAGE(WARNING "${list_str}\n")
      MESSAGE(FATAL_ERROR "${option_name} is not set") # stop cmake
    ENDIF()
  ENDIF()
  
  MESSAGE(STATUS "${option_name} is set to ${${option_name}}")
  MESSAGE(STATUS "Other ${option_name}'s are: ${list_str} \n")
endfunction()

validate_input("EVE_APPS_GRAPHICS" LIST_GRAPHICS)
validate_input("EVE_APPS_PLATFORM" LIST_PLATFORM)
validate_input("EVE_APPS_DISPLAY"  LIST_DISPLAY)

# Find host platform name ######################################################
STRING(REGEX MATCH "MM90.*"                    IS_PLATFORM_FT9XX    ${EVE_APPS_PLATFORM})
STRING(REGEX MATCH "RP2040.*"                  IS_PLATFORM_RP2040   ${EVE_APPS_PLATFORM})
STRING(REGEX MATCH "(FT4222|MPSSE|BT8XXEMU).*" IS_PLATFORM_WINDOW   ${EVE_APPS_PLATFORM})
STRING(REGEX MATCH "(MULTI|DEFAULT).*"         IS_PLATFORM_MULTI    ${EVE_APPS_PLATFORM})
STRING(REGEX MATCH "MM900.*"                   IS_PLATFORM_FT900    ${EVE_APPS_PLATFORM})
STRING(REGEX MATCH "MM930.*"                   IS_PLATFORM_FT930    ${EVE_APPS_PLATFORM})
STRING(REGEX MATCH "FT4222.*"                  IS_PLATFORM_FT4222   ${EVE_APPS_PLATFORM})
STRING(REGEX MATCH "MPSSE.*"                   IS_PLATFORM_MPSSE    ${EVE_APPS_PLATFORM})
STRING(REGEX MATCH "BT8XXEMU.*"                IS_PLATFORM_EMULATOR ${EVE_APPS_PLATFORM})  
SET(IS_PLATFORM_FT9XX    "${IS_PLATFORM_FT9XX}"    CACHE STRING "Platform Target FT9XX   ")
SET(IS_PLATFORM_RP2040   "${IS_PLATFORM_RP2040}"   CACHE STRING "Platform Target RP2040  ")
SET(IS_PLATFORM_WINDOW   "${IS_PLATFORM_WINDOW}"   CACHE STRING "Platform Target WINDOW  ")
SET(IS_PLATFORM_MULTI    "${IS_PLATFORM_MULTI}"    CACHE STRING "Platform Target MULTI   ")
SET(IS_PLATFORM_FT900    "${IS_PLATFORM_FT900}"    CACHE STRING "Platform Target FT900   ")
SET(IS_PLATFORM_FT930    "${IS_PLATFORM_FT930}"    CACHE STRING "Platform Target FT930   ")
SET(IS_PLATFORM_FT4222   "${IS_PLATFORM_FT4222}"   CACHE STRING "Platform Target FT4222  ")
SET(IS_PLATFORM_MPSSE    "${IS_PLATFORM_MPSSE}"    CACHE STRING "Platform Target MPSSE   ")
SET(IS_PLATFORM_EMULATOR "${IS_PLATFORM_EMULATOR}" CACHE STRING "Platform Target EMULATOR")

mark_as_advanced(IS_PLATFORM_FT9XX   )
mark_as_advanced(IS_PLATFORM_RP2040  )
mark_as_advanced(IS_PLATFORM_WINDOW  )
mark_as_advanced(IS_PLATFORM_MULTI   )
mark_as_advanced(IS_PLATFORM_FT900   )
mark_as_advanced(IS_PLATFORM_FT930   )
mark_as_advanced(IS_PLATFORM_FT4222  )
mark_as_advanced(IS_PLATFORM_MPSSE   )
mark_as_advanced(IS_PLATFORM_EMULATOR)

# Set up the basic build environment ###########################################
IF (CMAKE_BUILD_TYPE STREQUAL "")
  # CMake defaults to leaving CMAKE_BUILD_TYPE empty. This messes up
  # differentiation between debug and release builds.
  SET(CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE STRING "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel." FORCE)
ENDIF ()

# Platform SDk setting #########################################################
IF(IS_PLATFORM_RP2040)
  # Pull in Pico SDK (must be before project)
  INCLUDE(pico_sdk_import.cmake)
  PROJECT("EVE_Apps" C CXX ASM)

  # Initialise the Pico SDK
  pico_sdk_init()
  
ELSEIF(IS_PLATFORM_WINDOW OR IS_PLATFORM_MULTI)
  PROJECT("EVE_Apps" C CXX)
  
  # Strip binary in release mode
  IF (CMAKE_COMPILER_IS_GNUCXX)
    SET(CMAKE_EXE_LINKER_FLAGS_RELEASE "-s")
  ENDIF ()
  
  if (IS_PLATFORM_MULTI)
    MESSAGE(STATUS "Multi-platform desktop EVE HAL build selected (targets FT4222, MPSSE, and BT8XXEMU)")
  endif ()
  
  SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE})
  SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
  SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
  
  IF(CMAKE_SIZEOF_VOID_P EQUAL 8)
    MESSAGE(STATUS "Using 64-bit Windows dependencies")
    LINK_DIRECTORIES(${CMAKE_SOURCE_DIR}/common/eve_hal/Bin/Simulation/x64)
    LINK_DIRECTORIES(${CMAKE_SOURCE_DIR}/common/eve_hal/Bin/MSVC/x64)
  ELSE()
    MESSAGE(STATUS "Using 32-bit Windows dependencies")
    LINK_DIRECTORIES(${CMAKE_SOURCE_DIR}/common/eve_hal/Bin/Simulation/x86)
    LINK_DIRECTORIES(${CMAKE_SOURCE_DIR}/common/eve_hal/Bin/MSVC/x86)
  ENDIF()
  
ELSEIF(IS_PLATFORM_FT9XX)
  # TODO: Get this from the env variable if FT9XX_TOOLCHAIN is not set and the env var exists
  SET(FT9XX_TOOLCHAIN "Bridgetek/FT9xx\ Toolchain2.5.0" CACHE PATH "Location of FT9XX toolchain") #Any space must be prefixed with '\'

  ADD_DEFINITIONS("-D_DEBUG=1")
  SET(CMAKE_SYSTEM_NAME Generic)

  SET(TOOLCHAIN_INCLUDEPATH "${FT9XX_TOOLCHAIN}/Toolchain/hardware/include")
  INCLUDE_DIRECTORIES("${TOOLCHAIN_INCLUDEPATH}")

  # Below build flag is copied from FT9XX toolchain
  set(CMAKE_CXX_FLAGS           "${CMAKE_CXX_FLAGS}         -Og -O2 -g -c -fvar-tracking -fvar-tracking-assignments -Wall -fmessage-length=0 -ffunction-sections -Wno-unknown-pragmas -MMD -MP")
  set(CMAKE_CXX_FLAGS_DEBUG     "${CMAKE_CXX_FLAGS_DEBUG}   -Og -O2 -g -c -fvar-tracking -fvar-tracking-assignments -Wall -fmessage-length=0 -ffunction-sections -Wno-unknown-pragmas -MMD -MP")
  set(CMAKE_CXX_FLAGS_RELEASE   "${CMAKE_CXX_FLAGS_RELEASE} -Og -O2 -g -c -fvar-tracking -fvar-tracking-assignments -Wall -fmessage-length=0 -ffunction-sections -Wno-unknown-pragmas -MMD -MP")
  set(CMAKE_C_FLAGS             "${CMAKE_C_FLAGS}           -Wl,--gc-sections -Wl,--entry=_start")
  set(CMAKE_C_FLAGS_DEBUG       "${CMAKE_C_FLAGS_DEBUG}     -Wl,--gc-sections -Wl,--entry=_start")
  set(CMAKE_C_FLAGS_RELEASE     "${CMAKE_C_FLAGS_RELEASE}   -Wl,--gc-sections -Wl,--entry=_start")
  
  SET(CMAKE_EXE_LINKER_FLAGS  "")


  # Specify the cross compiler
  SET(CMAKE_C_COMPILER "${FT9XX_TOOLCHAIN}/Toolchain/tools/bin/ft32-elf-gcc.exe")
  SET(CMAKE_CXX_COMPILER "${FT9XX_TOOLCHAIN}/Toolchain/tools/bin/ft32-elf-g++.exe")
  SET(CMAKE_MAKE_PROGRAM "${FT9XX_TOOLCHAIN}/External/GnuWin32/bin/make.exe")
  SET(CMAKE_OBJCOPY      "${FT9XX_TOOLCHAIN}/Toolchain/tools/ft32-elf/bin/objcopy.exe")

  # Where is the target environment
  SET(CMAKE_FIND_ROOT_PATH  "${FT9XX_TOOLCHAIN}/Toolchain/tools/bin")
  link_directories("${FT9XX_TOOLCHAIN}/Toolchain/hardware/lib/Debug")

  # Search for programs in the build host directories
  SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)

  # For libraries and headers in the target directories
  SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
  SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
  
  PROJECT("EVE_Apps" C CXX)

ENDIF()

# C language version ###########################################################
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Most graphics boards have a default display and platform specified in the configuration header
# MULTI and DEFAULT behave the same, it depends on the selected graphics board
# For embedded targets, the platforma always needs to be specified, though, since CMake needs to set the proper build options
if(NOT EVE_APPS_GRAPHICS STREQUAL "MULTI")
  if(NOT EVE_APPS_GRAPHICS STREQUAL "")
    add_definitions("-D${EVE_APPS_GRAPHICS}")
  endif()
endif()
if(NOT EVE_APPS_PLATFORM STREQUAL "MULTI")
  if(NOT EVE_APPS_PLATFORM STREQUAL "DEFAULT")
    add_definitions("-D${EVE_APPS_PLATFORM}")
  endif()
endif()
if(NOT EVE_APPS_DISPLAY STREQUAL "MULTI")
  if(NOT EVE_APPS_DISPLAY STREQUAL "DEFAULT")
    add_definitions("-D${EVE_APPS_DISPLAY}")
  endif()
endif()

# Set host platform build flag #################################################
IF(IS_PLATFORM_FT900)
  ADD_DEFINITIONS("-D__FT900__")
ELSEIF(IS_PLATFORM_FT930)
  ADD_DEFINITIONS("-D__FT930__")
ENDIF()

if(WITH_ESD_CORE AND WITH_ESD_LITTLEFS)
  add_definitions("-DESD_LITTLEFS_FLASH")
endif()

IF(WIN32 AND ((EVE_APPS_PLATFORM STREQUAL "MULTI") OR (EVE_APPS_GRAPHICS STREQUAL "MULTI")))
  IF(WITH_ESD_CORE AND WITH_ESD_LITTLEFS)
     ADD_DEFINITIONS(-DLFS_IMPORT)
     INCLUDE_DIRECTORIES("dependencies/littlefs")
  ENDIF()
ENDIF()

# Common setup #################################################################
ADD_SUBDIRECTORY(dependencies)
ADD_SUBDIRECTORY(common)
INCLUDE_DIRECTORIES("common/eve_hal")
INCLUDE_DIRECTORIES("common/eve_hal/Hdr")

# Project setup ################################################################
IF(NOT EVE_APPS_GRAPHICS STREQUAL "MULTI")
  ADD_SUBDIRECTORY(DemoApps)
  ADD_SUBDIRECTORY(SampleApp)
ENDIF()

IF (WITH_EVE_TESTS)
  ADD_SUBDIRECTORY(tests)
ENDIF()

################################################################################
