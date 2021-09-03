
# CMake configuration for multi-target EVE HAL build

FILE(GLOB SRCS *.c)
FILE(GLOB HDRS *.h)
SOURCE_GROUP("" FILES ${SRCS} ${HDRS})

INCLUDE_DIRECTORIES(Hdr)

IF(WIN32 AND ((EVE_APPS_PLATFORM STREQUAL "MULTI") OR (EVE_APPS_GRAPHICS STREQUAL "MULTI")))
  ADD_DEFINITIONS(-DEVE_HAL_EXPORT)
  ADD_LIBRARY(eve_hal SHARED
    ${SRCS}
    ${HDRS}
  )
ELSE()
  ADD_LIBRARY(eve_hal
    ${SRCS}
    ${HDRS}
  )
ENDIF()

IF(NOT EVE_APPS_PLATFORM STREQUAL "RP2040")

  TARGET_LINK_LIBRARIES(eve_hal
    libMPSSE
    LibFT4222
    ftd2xx
    bt8xxemu
  )
  
ELSE()

  # Add the standard library to the build
  TARGET_LINK_LIBRARIES(eve_hal pico_stdlib)
  
  # Add any user requested libraries
  TARGET_LINK_LIBRARIES(eve_hal
    hardware_spi
    hardware_timer
  )
  LINK_LIBRARIES(fatfs)
  
  INCLUDE_DIRECTORIES(../application/fatfs)
ENDIF()



IF (WIN32)
  IF(CMAKE_SIZEOF_VOID_P EQUAL 8)
    ADD_CUSTOM_COMMAND(TARGET eve_hal POST_BUILD 
      COMMAND ${CMAKE_COMMAND} -E copy_if_different
      ${CMAKE_SOURCE_DIR}/common/eve_hal/Bin/MSVC/x64/libMPSSE.dll
      ${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE}/libMPSSE.dll)
    ADD_CUSTOM_COMMAND(TARGET eve_hal POST_BUILD 
      COMMAND ${CMAKE_COMMAND} -E copy_if_different
      ${CMAKE_SOURCE_DIR}/common/eve_hal/Bin/MSVC/x64/LibFT4222-64.dll
      ${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE}/LibFT4222-64.dll)
    ADD_CUSTOM_COMMAND(TARGET eve_hal POST_BUILD 
      COMMAND ${CMAKE_COMMAND} -E copy_if_different
      ${CMAKE_SOURCE_DIR}/common/eve_hal/Bin/MSVC/x64/ftd2xx.dll
      ${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE}/ftd2xx.dll)
    ADD_CUSTOM_COMMAND(TARGET eve_hal POST_BUILD 
      COMMAND ${CMAKE_COMMAND} -E copy_if_different
      ${CMAKE_SOURCE_DIR}/common/eve_hal/Bin/Simulation/x64/bt8xxemu.dll
      ${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE}/bt8xxemu.dll)
    ADD_CUSTOM_COMMAND(TARGET eve_hal POST_BUILD 
      COMMAND ${CMAKE_COMMAND} -E copy_if_different
      ${CMAKE_SOURCE_DIR}/common/eve_hal/Bin/Simulation/x64/mx25lemu.dll
      ${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE}/mx25lemu.dll)
  ELSE()
    ADD_CUSTOM_COMMAND(TARGET eve_hal POST_BUILD 
      COMMAND ${CMAKE_COMMAND} -E copy_if_different
      ${CMAKE_SOURCE_DIR}/common/eve_hal/Bin/MSVC/libMPSSE.dll
      ${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE}/libMPSSE.dll)
    ADD_CUSTOM_COMMAND(TARGET eve_hal POST_BUILD 
      COMMAND ${CMAKE_COMMAND} -E copy_if_different
      ${CMAKE_SOURCE_DIR}/common/eve_hal/Bin/MSVC/LibFT4222.dll
      ${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE}/LibFT4222.dll)
    ADD_CUSTOM_COMMAND(TARGET eve_hal POST_BUILD 
      COMMAND ${CMAKE_COMMAND} -E copy_if_different
      ${CMAKE_SOURCE_DIR}/common/eve_hal/Bin/MSVC/ftd2xx.dll
      ${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE}/ftd2xx.dll)
    ADD_CUSTOM_COMMAND(TARGET eve_hal POST_BUILD 
      COMMAND ${CMAKE_COMMAND} -E copy_if_different
      ${CMAKE_SOURCE_DIR}/common/eve_hal/Bin/Simulation/bt8xxemu.dll
      ${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE}/bt8xxemu.dll)
    ADD_CUSTOM_COMMAND(TARGET eve_hal POST_BUILD 
      COMMAND ${CMAKE_COMMAND} -E copy_if_different
      ${CMAKE_SOURCE_DIR}/common/eve_hal/Bin/Simulation/mx25lemu.dll
      ${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE}/mx25lemu.dll)
  ENDIF()
ENDIF()
