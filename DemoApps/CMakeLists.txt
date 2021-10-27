LINK_LIBRARIES(eve_hal)

IF((IS_PLATFORM_FT9XX) OR (IS_PLATFORM_RP2040))
  LINK_LIBRARIES(fatfs)
  INCLUDE_DIRECTORIES("../common/application/fatfs/source")
ENDIF()

INCLUDE_DIRECTORIES("../common/application")

# Iterate through all sub folder and add into ADD_SUBDIRECTORY
macro(SUBDIRLIST result curdir)
  FILE(GLOB children RELATIVE ${curdir} ${curdir}/*)
  SET(dirlist "")
  FOREACH(child ${children})
    IF(IS_DIRECTORY ${curdir}/${child})
        SET(dirlist ${dirlist} ${child})
    ENDIF()
  ENDFOREACH()
  SET(${result} ${dirlist})
endmacro()

SUBDIRLIST(SUBDIRS ${CMAKE_CURRENT_SOURCE_DIR})

FOREACH(subdir ${SUBDIRS})
  IF(NOT ${subdir} STREQUAL ".vs")
    ADD_SUBDIRECTORY(${subdir})
  ENDIF()
ENDFOREACH()
