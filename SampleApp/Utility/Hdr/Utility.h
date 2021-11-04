#ifndef APP_H_
#define APP_H_

#include "platform.h"

// Path to UI assets Folder
#if defined(MSVC_PLATFORM) || defined(BT8XXEMU_PLATFORM)
#define TEST_DIR                            "..\\..\\..\\Test\\"
#else
#define TEST_DIR                            "/EveApps/SampleApp/Utility/Test"
#endif

#define GET_CALIBRATION                     1

/* sample app structure definitions */
typedef struct SAMAPP_Bitmap_header
{
	uint8_t Format;
	int16_t Width;
	int16_t Height;
	int16_t Stride;
	int32_t Arrayoffset;
}SAMAPP_Bitmap_header_t;

typedef struct SAMAPP_ColorRGB {
	int r;
	int g;
	int b;
}SAMAPP_ColorRGB_t;

typedef struct SAMAPP_Circle {
	int visible;
	int x;
	int y;
	int radius;
	SAMAPP_ColorRGB_t color;
	int opacity; // 0 to 256
	int step;
}SAMAPP_Circle_t;

#endif /* APP_H_ */
