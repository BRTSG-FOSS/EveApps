#ifndef SAMAPP_ENABLE_APIS_H_
#define SAMAPP_ENABLE_APIS_H_
#include "platform.h"

/* Compile time switch for enabling sample app api sets - please cross check the same in SampleApp_RawData.cpp file as well */

#define SAMAPP_ENABLE_APIS_SET0
#define SAMAPP_ENABLE_APIS_SET1
#define SAMAPP_ENABLE_APIS_SET2
#define SAMAPP_ENABLE_APIS_SET3
#define SAMAPP_ENABLE_APIS_SET4
#define SAMAPP_ENABLE_APIS_SET5

#if defined (BT8XXEMU_PLATFORM) || defined(FT801_ENABLE) || defined(FT811_ENABLE) || defined(FT813_ENABLE) || defined(BT815_ENABLE) || defined (BT817_ENABLE)
#if defined(MSVC_PLATFORM) || defined(BT8XXEMU_PLATFORM)
	#define SAMAPP_ENABLE_APIS_SET6 //Support only multi-touch screen
#endif // MSVC and BT8XXEMU_PLATFORM

	#define SAMAPP_ENABLE_APIS_SET7 //Support only multi-touch screen
#endif

#if defined(FT81X_ENABLE)
	#define SAMAPP_ENABLE_APIS_SET8 //Suport only FT81x
#if defined(MSVC_PLATFORM) || defined(BT8XXEMU_PLATFORM)
	#define SAMAPP_ENABLE_APIS_SET9 //Support only MSVC and FT81x
#endif
#endif

#if defined (BT81X_ENABLE)
#if defined(MSVC_PLATFORM) || defined(BT8XXEMU_PLATFORM)
#define SAMAPP_ENABLE_APIS_SET10 // Support only MSVC and Emulator for BT81X
#endif

#define SAMAPP_ENABLE_APIS_SET11 // Bulk transfer
#define SAMAPP_ENABLE_APIS_SET12 // Rotate and rotateAround
#define SAMAPP_ENABLE_APIS_SET13 // Extended font format
#endif

#if defined(BT817_ENABLE) || defined(BT818_ENABLE)
#if !defined(FT9XX_PLATFORM)
#define SAMAPP_ENABLE_APIS_SET14 // New BT817/8 features
#endif
#endif

#endif /* SAMAPP_ENABLE_APIS_H_ */
