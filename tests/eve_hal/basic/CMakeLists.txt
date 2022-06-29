
ADD_EXECUTABLE(test_eve_hal_basic main.c)

IF(EVE_APPS_PLATFORM STREQUAL "RP2040")
  pico_add_extra_outputs(test_eve_hal_basic)
ENDIF()
