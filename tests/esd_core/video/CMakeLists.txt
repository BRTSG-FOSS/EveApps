
ADD_EXECUTABLE(test_esd_core_video main.c)

IF(EVE_APPS_PLATFORM STREQUAL "RP2040")
  pico_add_extra_outputs(test_esd_core_video)
ENDIF()

if(WIN32)
  set_target_properties(test_esd_core_video PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/EveApps/SampleApp/Test")
endif()
