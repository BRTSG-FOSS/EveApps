/**
* This source code ("the Software") is provided by Bridgetek Pte Ltd
* ("Bridgetek") subject to the licence terms set out
*   http://brtchip.com/BRTSourceCodeLicenseAgreement/ ("the Licence Terms").
* You must read the Licence Terms before downloading or using the Software.
* By installing or using the Software you agree to the Licence Terms. If you
* do not agree to the Licence Terms then do not download or use the Software.
*
* Without prejudice to the Licence Terms, here is a summary of some of the key
* terms of the Licence Terms (and in the event of any conflict between this
* summary and the Licence Terms then the text of the Licence Terms will
* prevail).
*
* The Software is provided "as is".
* There are no warranties (or similar) in relation to the quality of the
* Software. You use it at your own risk.
* The Software should not be used in, or for, any medical device, system or
* appliance. There are exclusions of Bridgetek liability for certain types of loss
* such as: special loss or damage; incidental loss or damage; indirect or
* consequential loss or damage; loss of income; loss of business; loss of
* profits; loss of revenue; loss of contracts; business interruption; loss of
* the use of money or anticipated savings; loss of information; loss of
* opportunity; loss of goodwill or reputation; and/or loss of, damage to or
* corruption of data.
* There is a monetary cap on Bridgetek's liability.
* The Software may have subsequently been amended by another user and then
* distributed by that other user ("Adapted Software").  If so that user may
* have additional licence terms that apply to those amendments. However, Bridgetek
* has no liability in relation to those amendments.
*/

#ifndef ESD_BASE__H
#define ESD_BASE__H

/*
Type definition

ESD_TYPE(uint32_t, Native = UInt32, Edit = Int)

ESD_TYPE(char *, Native = Latin1, Edit = String)

ESD_TYPE(ESD_BitmapCell *, Native = Pointer, Edit = Library)

typedef int32_t esd_int32_f4_t;
ESD_TYPE(esd_int32_f4_t, Native = Int32, Edit = Fixed4)

*/

#define ESD_TYPE(name, ...)
#define ESD_SYMBOL(name, ...)

/*
Enum definition.
Parses the enum identifiers, does not care about the integer or any other type of values

Usage examples:

ESD_ENUM(BitmapFormat, Type = uint32_t)
#define RGB 1
#define ARGB 2
ESD_END()

ESD_ENUM(BitmapFormat)
enum BitmapFormat
{
RGB = 1,
ARGB
};
ESD_END()

*/

#define ESD_ENUM(name, ...)
#define ESD_IDENTIFIER(name, ...)
#define ESD_END()

/* Node definition

Usage examples:

ESD_FUNCTION(testPrint, DisplayName = "Test Print", Icon = ":/icons/card-address.png")
ESD_PARAMETER(message, Type = char *)
void testPrint(char *message);

ESD_FUNCTION(EVE_CoCmd_dl, Include = "EVE_Hal.h")
ESD_PARAMETER(phost, Type = EVE_HalContext *, Default = "ESD_GetHost()", Hidden, Internal)
ESD_PARAMETER(dl, Type = uint32_t)

*/

#define ESD_GLOBAL(name, ...)

#define ESD_FUNCTION(name, ...)
#define ESD_METHOD(name, ...)
#define ESD_UPDATE(name, ...)
#define ESD_RENDER(name, ...)

#define ESD_LOGIC(name, ...)
#define ESD_ACTOR(name, ...)
#define ESD_WIDGET(name, ...)
#define ESD_PAGE(name, ...)
#define ESD_APPLICATION(name, ...)

#define ESD_PARAMETER(name, ...)

#define ESD_SLOT(name, ...)
#define ESD_SIGNAL(name, ...)
#define ESD_INPUT(name, ...)
#define ESD_OUTPUT(name, ...)
#define ESD_VARIABLE(name, ...)
#define ESD_WRITER(name, ...)

#define ESD_CATEGORY(name, ...)

#include "EVE_Hal.h"

#pragma ESD_TYPE(void, Native = Void)
#pragma ESD_TYPE(char, Native = Char, Edit = String)
#pragma ESD_TYPE(signed char, Native = Int8, Edit = Integer)
#pragma ESD_TYPE(short, Native = Int16, Edit = Integer)
#pragma ESD_TYPE(int, Native = Int32, Edit = Integer)
#pragma ESD_TYPE(long, Native = Int32, Edit = Integer)
#pragma ESD_TYPE(long long, Native = Int64, Edit = Integer)
#pragma ESD_TYPE(unsigned char, Native = UInt8, Edit = Integer)
#pragma ESD_TYPE(unsigned short, Native = UInt16, Edit = Integer)
#pragma ESD_TYPE(unsigned int, Native = UInt32, Edit = Integer)
#pragma ESD_TYPE(unsigned long, Native = UInt32, Edit = Integer)
#pragma ESD_TYPE(unsigned long long, Native = UInt64, Edit = Integer)
#pragma ESD_TYPE(float, Native = Float, Edit = Real)
#pragma ESD_TYPE(double, Native = Double, Edit = Real)
#pragma ESD_TYPE(_Bool, Native = Bool, Edit = Boolean)
#pragma ESD_TYPE(char *, Native = Utf8, Edit = String)
#pragma ESD_TYPE(const char *, Native = Utf8, Edit = String)
#pragma ESD_TYPE(void *, Native = Pointer, Edit = None)

#pragma ESD_TYPE(int8_t, Native = Int8, Edit = Integer)
#pragma ESD_TYPE(uint8_t, Native = UInt8, Edit = Integer)
#pragma ESD_TYPE(int16_t, Native = Int16, Edit = Integer)
#pragma ESD_TYPE(uint16_t, Native = UInt16, Edit = Integer)
#pragma ESD_TYPE(int32_t, Native = Int32, Edit = Integer)
#pragma ESD_TYPE(uint32_t, Native = UInt32, Edit = Integer)
#pragma ESD_TYPE(int64_t, Native = Int64, Edit = Integer)
#pragma ESD_TYPE(uint64_t, Native = UInt64, Edit = Integer)
#pragma ESD_TYPE(ptrdiff_t, Native = IntPtr, Edit = Integer)
#pragma ESD_TYPE(size_t, Native = UIntPtr, Edit = Integer)
#pragma ESD_TYPE(bool, Native = Bool, Edit = Boolean)

#pragma ESD_TYPE(esd_int32_f16_t, Native = Int32, Edit = Fixed16)
typedef int32_t esd_int32_f16_t;

#pragma ESD_TYPE(esd_int32_f8_t, Native = Int32, Edit = Fixed8)
typedef int32_t esd_int32_f8_t;

#pragma ESD_TYPE(esd_int32_f4_t, Native = Int32, Edit = Fixed4)
typedef int32_t esd_int32_f4_t;

#pragma ESD_TYPE(esd_int32_f3_t, Native = Int32, Edit = Fixed3)
typedef int32_t esd_int32_f3_t;

#pragma ESD_TYPE(esd_int32_f2_t, Native = Int32, Edit = Fixed2)
typedef int32_t esd_int32_f2_t;

#pragma ESD_TYPE(esd_int16_f8_t, Native = Int16, Edit = Fixed8)
typedef int16_t esd_int16_f8_t;

#pragma ESD_TYPE(esd_int16_f4_t, Native = Int16, Edit = Fixed4)
typedef int16_t esd_int16_f4_t;

#pragma ESD_TYPE(esd_int16_f2_t, Native = Int16, Edit = Fixed2)
typedef int16_t esd_int16_f2_t;

#pragma ESD_TYPE(esd_argb32_t, Native = UInt32, Edit = ColorARGB)
typedef uint32_t esd_argb32_t;

#pragma ESD_TYPE(esd_rgb32_t, Native = UInt32, Edit = ColorRGB)
typedef uint32_t esd_rgb32_t;

#pragma ESD_TYPE(esd_classid_t, Native = UInt32, Edit = Library)
typedef uint32_t esd_classid_t;

#ifdef EVE_MULTI_TARGET
#ifdef ESD_CORE_EXPORT
#undef ESD_CORE_EXPORT
#define ESD_CORE_EXPORT _declspec(dllexport)
#else
#define ESD_CORE_EXPORT _declspec(dllimport)
#endif
#else
#define ESD_CORE_EXPORT
#endif

#pragma ESD_TYPE(EVE_HalContext *, Native = Pointer, Edit = Library)

#pragma ESD_CATEGORY(EsdUtilities, DisplayName = "ESD Utilities")
#pragma ESD_CATEGORY(EsdRenderable, DisplayName = "ESD Render Functions")
#pragma ESD_CATEGORY(EveRenderFunctions, DisplayName = "Display List", Category = EsdRenderable)

#pragma ESD_FUNCTION(ESD_Noop, Category = _GroupHidden)
#pragma ESD_PARAMETER(context, Type = void *)
ESD_CORE_EXPORT void ESD_Noop(void *context);

#define ESD_LOOPSTATE_NONE 0
#define ESD_LOOPSTATE_IDLE 1
#define ESD_LOOPSTATE_UPDATE 2
#define ESD_LOOPSTATE_RENDER 3

#ifdef ESD_SIMULATION
// Functions provided by ESD environment
void logMessage__ESD(const char *str);
void logWarning__ESD(const char *str);
void logError__ESD(const char *str);
#define ESD_LogMessage(s) logMessage__ESD(s)
#define ESD_LogWarning(s) logWarning__ESD(s)
#define ESD_LogError(s) logError__ESD(s)
#else
#define ESD_LogMessage(s) \
	do                       \
	{                        \
	} while (0)
#define ESD_LogWarning(s) \
	do                       \
	{                        \
	} while (0)
#define ESD_LogError(s) \
	do                     \
	{                      \
	} while (0)
#endif

#endif /* #ifndef ESD_BASE__H */

/* end of file */
