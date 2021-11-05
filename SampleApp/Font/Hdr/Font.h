#ifndef APP_H_
#define APP_H_

#include "platform.h"

// Path to UI assets Folder
#if defined(MSVC_PLATFORM) || defined(BT8XXEMU_PLATFORM)
#define TEST_DIR                            "..\\..\\..\\Test\\"
#else
#define TEST_DIR                            "/EveApps/SampleApp/Font/Test"
#endif

#define GET_CALIBRATION                     1

#if defined(FT81X_ENABLE)
#define SAMAPP_NUM_OF_ROM_FONTS 19
#else
#define SAMAPP_NUM_OF_ROM_FONTS 16
#endif

#endif /* APP_H_ */
