#ifndef APP_H_
#define APP_H_

#include "platform.h"

// Path to UI assets Folder
#if defined(MSVC_PLATFORM) || defined(MSVC_FT800EMU)
#define TEST_DIR                            "..\\..\\Test\\"
#else
#define TEST_DIR                            "/EveApps/DemoEvChargePoint/Test"
#endif

#define GET_CALIBRATION                     1

#endif /* APP_H_ */
