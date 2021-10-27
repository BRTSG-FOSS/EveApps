#ifndef APP_H_
#define APP_H_

#include "platform.h"
#include "Common.h"

// Path to UI assets Folder
#if defined(MSVC_PLATFORM) || defined(MSVC_FT800EMU)
#define TEST_DIR                            "..\\..\\Test\\"
#define EVE_FLASH_DIR                       "..\\..\\..\\..\\common\\eve_flash"
#else
#define TEST_DIR                            "/Test/SampleApp"
#define EVE_FLASH_DIR                       "/Test/common/eve_flash"
#endif

/*
* Calibration setting
*    range: 0-1
*    1 : Revoke calibration setting of new hardware
*    0 : Ignore calibration
*/
#define GET_CALIBRATION                     1

/// Common function return value
#define APP_ERROR 0
#define APP_OK    1

// String expand
#define STREXPAND(x)                        #x
#define STR(x)                              STREXPAND(x)

// Versioning
#define MAJOR_NUMBER                        1 // Important update
#define MINOR_NUMBER                        1 // Minor update, bug fixing
#define BUILD_NUMBER                        1 // Increased every build
#define BUILDDATE                           __DATE__
#define VERSION_STRING                      STR(BUILDDATE)"."STR(MAJOR_NUMBER)"."STR(MINOR_NUMBER)"."STR(BUILD_NUMBER)

#if defined(BT815_ENABLE)
#define FILE_BLOB                           (EVE_FLASH_DIR "\\BT815-unified.blob")
#elif defined(BT817_ENABLE)
#define FILE_BLOB                           (EVE_FLASH_DIR "\\BT817-unified.blob")
#else
/// Use BT815 blob as default
#define FILE_BLOB                           (EVE_FLASH_DIR "\\BT815-unified.blob")
#endif

// Mathematics                               
#define DEVIDE_ROUND_UP(x, y)               ((x / y) + ((x % y) > 0?1:0))

// common function return                   
#define APP_ERROR                           0
#define APP_OK                              1

// Common value                             
#define true                                1

// bulk transfer                            
#define CMD_BUFFER_SIZE                     (1024 * 5)

// angle and circle                         
#define MAX_ANGLE                           360
#define MAX_CIRCLE_UNIT                     65536

// common                                   
#define SAMAPP_DELAY_BTW_APIS               (1000)
#define SAMAPP_ENABLE_DELAY()               EVE_sleep(SAMAPP_DELAY_BTW_APIS)
#define SAMAPP_ENABLE_DELAY_VALUE(x)        EVE_sleep(x)

//Set8 buffer frame
#define SCRATCH_BUFF_SZ                     (1024*4)

//Bouncing Circle macros
#define NO_OF_CIRCLE                        (5)

//Bouncing Points macros
#define NBLOBS                              (64)
#define OFFSCREEN                           (-16384)
#define APP_BLOBS_NUMTOUCH                  (5)

//main windows                              
#define ImW                                 (66)
#define ImH                                 (66)
#define NO_OF_TOUCH                         (5)

// buffers
#define APPBUFFERSIZE                       (65536L)
#define APPBUFFERSIZEMINUSONE               (APPBUFFERSIZE - 1)

// App_Slots macros for SampleApp
#define APP_SLOTS_SET_(NAME)                           \
    {                                                  \
        (void(*)(void ))App_Set_ ## NAME ## _Init   ,  \
        (void(*)(void ))App_Set_ ## NAME ## _Draw   ,  \
        (void(*)(void ))App_Set_ ## NAME ## _Deinit    \
    }

#define APP_HEADERS(NAME)                                                \
    void App_Set_ ## NAME ## _Init   (EVE_HalContext *ph);               \
    void App_Set_ ## NAME ## _Draw   ();                                 \
    void App_Set_ ## NAME ## _Deinit ();                                 \

// Function pointer table for common rendering interface (32 bytes)
typedef struct APP_SLOTS {
	union {
		void(*Table[3])(void);
		struct {
			// Pointers to slot functions
			void(*Init)(EVE_HalContext *ph);
			void(*Draw)(void);
			void(*DeInit)(void);
		};
	};
} App_Slots;

/* sample app structure definitions */
typedef struct SAMAPP_Bitmap_header
{
	uint8_t Format;
	int16_t Width;
	int16_t Height;
	int16_t Stride;
	int32_t Arrayoffset;
}SAMAPP_Bitmap_header_t;

//bouncing squares
#define NO_OF_RECTS (5)
typedef struct SAMAPP_BouncingSquares {
	int16_t BRy[5], BRx[5], My[5];
	uint8_t E[5];
	uint8_t RectNo[5];
	int16_t Count;
}SAMAPP_BouncingSquares_t;

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

extern SAMAPP_Bitmap_header_t SAMAPP_Bitmap_RawData_Header[];

#endif /* APP_H_ */
