#ifndef APP_H_
#define APP_H_

#include "platform.h"

// Path to UI assets Folder
#if defined(MSVC_PLATFORM) || defined(BT8XXEMU_PLATFORM)
#define TEST_DIR                            "..\\..\\..\\Test\\"
#else
#define TEST_DIR                            "/EveApps/SampleApp/Touch/Test"
#endif

#define GET_CALIBRATION                     1

#define NAMEARRAYSZ 500
typedef struct SAMAPP_Logo_Img {
	//prog_uchar8_t name[14];
	char8_t name[NAMEARRAYSZ];
	uint16_t image_height;
	uint8_t image_format;
	uint8_t filter;
	uint16_t sizex;
	uint16_t sizey;
	uint16_t linestride;
	uint32_t gram_address;
}SAMAPP_Logo_Img_t;

typedef struct SAMAPP_Squares {
	uint16_t x, y;
}SAMAPP_Squares_t;

//bouncing squares
#define NO_OF_RECTS (5)
typedef struct SAMAPP_BouncingSquares {
	int16_t BRy[5], BRx[5], My[5];
	uint8_t E[5];
	uint8_t RectNo[5];
	int16_t Count;
}SAMAPP_BouncingSquares_t;

//Bouncing Circle macros
#define NO_OF_CIRCLE                        (5)
//bouncing circles structures
typedef struct SAMAPP_TouchNo {
	uint8_t F[NO_OF_CIRCLE];
}SAMAPP_TouchNo_t;
typedef struct SAMAPP_BouncingCircles {
	float Tsq1[NO_OF_CIRCLE];
	float C1X[NO_OF_CIRCLE];
	float C1Y[NO_OF_CIRCLE];
	float TouchX[NO_OF_CIRCLE];
	float TouchY[NO_OF_CIRCLE];
	SAMAPP_TouchNo_t TN[NO_OF_CIRCLE];
}SAMAPP_BouncingCircles_t;

//bouncing pints structures
#define NBLOBS                              (64)
typedef struct SAMAPP_Blobs {
	int16_t x;
	int16_t y;
}SAMAPP_Blobs_t;
typedef struct SAMAPP_BlobsInst {
	SAMAPP_Blobs_t blobs[NBLOBS];
	uint8_t CurrIdx;
}SAMAPP_BlobsInst_t;


//moving points structures
#define NO_OF_POINTS (64)
typedef struct SAMAPP_MovingPoints {
	uint8_t Prevtouch;
	int16_t SmallX[6], SmallY;
	uint8_t Flag;
	int32_t val[5];
	int16_t X[(NO_OF_POINTS) * 4], Y[(NO_OF_POINTS) * 4];
	uint8_t t[((NO_OF_POINTS) * 4)];
}SAMAPP_MovingPoints_t;

//main windows                              
#define ImW                                 (66)
#define ImH                                 (66)
#define NO_OF_TOUCH                         (5)
// buffers
#define APPBUFFERSIZE                       (65536L)
#define APPBUFFERSIZEMINUSONE               (APPBUFFERSIZE - 1)
#define OFFSCREEN                           (-16384)
#define APP_BLOBS_NUMTOUCH                  (5)

#endif /* APP_H_ */
