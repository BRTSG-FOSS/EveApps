
ADD_EXECUTABLE(test_esd_core_basic main.c)

IF(EVE_APPS_PLATFORM STREQUAL "RP2040")
  pico_add_extra_outputs(test_esd_core_basic)
ENDIF()
