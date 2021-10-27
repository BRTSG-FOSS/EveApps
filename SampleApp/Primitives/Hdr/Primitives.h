#ifndef APP_H_
#define APP_H_

#include "platform.h"

// Path to UI assets Folder
#if defined(MSVC_PLATFORM) || defined(BT8XXEMU_PLATFORM)
#define TEST_DIR                            "..\\..\\..\\Test\\"
#else
#define TEST_DIR                            "/EveApps/Primitives/Test"
#endif

#define GET_CALIBRATION                     1

#define SAMAPP_ENABLE_DELAY()               EVE_sleep(1000)
#define SAMAPP_ENABLE_DELAY_VALUE(x)        EVE_sleep(x)

/* sample app structure definitions */
typedef struct SAMAPP_Bitmap_header
{
    uint8_t Format;
    int16_t Width;
    int16_t Height;
    int16_t Stride;
    int32_t Arrayoffset;
}SAMAPP_Bitmap_header_t;

#endif /* APP_H_ */
