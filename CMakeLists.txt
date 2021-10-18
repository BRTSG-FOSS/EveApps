# CMake project ################################################################
#
# 1. Build for FT9XX example: Edit the FT9XX_TOOLCHAIN location path at first
#  cd build && cmake  -G "MinGW Makefiles" -DEVE_APPS_PLATFORM=MM900EV3A -DEVE_APPS_GRAPHICS=BT817 -DEVE_APPS_DISPLAY=WXGA -DCMAKE_BUILD_TYPE=Debug ..
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

SET(FT9XX_TOOLCHAIN "PATH_TO:\ Bridgetek/FT9xx\ Toolchain2.5.0") #Any space must be prefixed with '\'

CMAKE_MINIMUM_REQUIRED(VERSION 3.13 FATAL_ERROR)
set (CMAKE_VERBOSE_MAKEFILE ON)
IF (POLICY CMP0020)
	CMAKE_POLICY(SET CMP0020 NEW)
ENDIF ()

IF (POLICY CMP0015)
	CMAKE_POLICY(SET CMP0015 NEW)
ENDIF ()
SET_PROPERTY(GLOBAL PROPERTY USE_FOLDERS ON)

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
                  EVE_GRAPHICS_BT815
                  EVE_GRAPHICS_BT816
                  EVE_GRAPHICS_BT817
                  EVE_GRAPHICS_BT818
  )
set(LIST_PLATFORM MULTI
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
set(LIST_DISPLAY  EVE_DISPLAY_QVGA
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
 
SET(EVE_APPS_PLATFORM "MULTI" CACHE STRING "Platform Target")
SET_PROPERTY(CACHE EVE_APPS_PLATFORM PROPERTY STRINGS ${LIST_PLATFORM})

SET(EVE_APPS_DISPLAY "EVE_DISPLAY_QVGA" CACHE STRING "Platform Target")
SET_PROPERTY(CACHE EVE_APPS_PLATFORM PROPERTY STRINGS ${LIST_DISPLAY})

# Validate input ###############################################################
function(give_sugguest value array global_set)
  FOREACH(item ${${array}})
    STRING(REGEX MATCH ".*${value}.*" search_val "${item}")
    IF(search_val)
      MESSAGE("Notice: ${value} is not defined, selected this similar configure: ${search_val}")
      set(${global_set} "${search_val}" CACHE STRING "Graphics Module" FORCE)
      break()
    ENDIF()
  ENDFOREACH()
endfunction()
  
#cmake  -G "MinGW Makefiles" -DEVE_APPS_PLATFORM=MM900EV3A -DEVE_APPS_GRAPHICS=BT817 -DEVE_APPS_DISPLAY=EVE_DISPLAY_WXGA -DCMAKE_BUILD_TYPE=Debug ..
function(validate_input option_name array)
  set(value "${${option_name}}")
  
  list(FIND ${array} "${value}" _index)
  STRING(REPLACE ";"  ", " list_str "${${array}}")
  if (${_index} EQUAL -1)
    # give sugguestion
    give_sugguest(${value} ${array} "${option_name}")
    
    # if give_sugguest works ok, then value of option_name is changed, if no, stop cmake with error message
    if ("${value}" STREQUAL "${${option_name}}")
      MESSAGE("\r\nError: ${option_name} is not set, the valid ${option_name} is: \r\n")
      MESSAGE("${list_str}\r\n\r\n")
      MESSAGE(FATAL_ERROR "${option_name} is not set") # stop cmake
    ENDIF()
  ENDIF()
  
  MESSAGE("${option_name} is set to ${${option_name}}")
  MESSAGE("Other ${option_name}'s are: ${list_str} \r\n\r\n")
endfunction()

validate_input("EVE_APPS_GRAPHICS" LIST_GRAPHICS)
validate_input("EVE_APPS_PLATFORM" LIST_PLATFORM)
validate_input("EVE_APPS_DISPLAY"  LIST_DISPLAY)

ADD_DEFINITIONS("-D${EVE_APPS_GRAPHICS}")
ADD_DEFINITIONS("-D${EVE_APPS_PLATFORM}")
ADD_DEFINITIONS("-D${EVE_APPS_DISPLAY}")

# Find host platform name ######################################################
STRING(REGEX MATCH "MM90.*"                    IS_PLATFORM_FT9XX    ${EVE_APPS_PLATFORM})
STRING(REGEX MATCH "RP2040.*"                  IS_PLATFORM_RP2040   ${EVE_APPS_PLATFORM})
STRING(REGEX MATCH "(FT4222|MPSSE|BT8XXEMU).*" IS_PLATFORM_WINDOW   ${EVE_APPS_PLATFORM})
STRING(REGEX MATCH "MULTI"                     IS_PLATFORM_MULTI    ${EVE_APPS_PLATFORM})
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

# C language version ###########################################################
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

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
  # Strip binary in release mode
  IF (CMAKE_COMPILER_IS_GNUCXX)
    SET(CMAKE_EXE_LINKER_FLAGS_RELEASE "-s")
  ENDIF ()
  
  SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE})
  SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
  SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
  
  IF(CMAKE_SIZEOF_VOID_P EQUAL 8)
    LINK_DIRECTORIES(${CMAKE_SOURCE_DIR}/common/eve_hal/Bin/Simulation/x64)
    LINK_DIRECTORIES(${CMAKE_SOURCE_DIR}/common/eve_hal/Bin/MSVC/x64)
  ELSE()
    LINK_DIRECTORIES(${CMAKE_SOURCE_DIR}/common/eve_hal/Bin/Simulation/x86)
    LINK_DIRECTORIES(${CMAKE_SOURCE_DIR}/common/eve_hal/Bin/MSVC/x86)
  ENDIF()
  
  PROJECT("EVE_Apps" C CXX ASM)
  
ELSEIF(IS_PLATFORM_FT9XX)
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

# Set host platform build flag #################################################
IF(IS_PLATFORM_FT900)
  ADD_DEFINITIONS("-D__FT900__")
ELSEIF(IS_PLATFORM_FT930)
  ADD_DEFINITIONS("-D__FT930__")
ENDIF()

# Common setup ##########################################################
MESSAGE("EVE_APPS_PLATFORM = ${EVE_APPS_PLATFORM}")
MESSAGE("FT9XX_TOOLCHAIN = ${FT9XX_TOOLCHAIN}")
MESSAGE("EVE_APPS_GRAPHICS = ${EVE_APPS_GRAPHICS}")
MESSAGE("CMAKE_OBJCOPY = ${CMAKE_OBJCOPY}")

ADD_SUBDIRECTORY(common)
INCLUDE_DIRECTORIES("common/eve_hal")
INCLUDE_DIRECTORIES("common/eve_hal/Hdr")

IF(NOT EVE_APPS_GRAPHICS STREQUAL "MULTI")
  ADD_SUBDIRECTORY(EveApps)
ENDIF()

########################################################################
