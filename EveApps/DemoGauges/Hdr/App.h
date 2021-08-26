#ifndef APP_H_
#define APP_H_

#include "platform.h"

// Path to UI assets Folder
#if defined(MSVC_PLATFORM) || defined(BT8XXEMU_PLATFORM)
#define TEST_DIR                            "..\\..\\Test\\"
#else
#define TEST_DIR                            "/EveApps/DemoGauges/Test"
#endif

#define GET_CALIBRATION                     0

#endif /* APP_H_ */
