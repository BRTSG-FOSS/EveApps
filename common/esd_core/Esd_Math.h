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
#ifndef ESD_MATH__H
#define ESD_MATH__H

#include "ESD_TypeDefs.h"

ESD_TYPE(ESD_Point16, Native = Struct) // TODO: Struct support, expose values
typedef struct
{
	int16_t X, Y;
} ESD_Point16;

ESD_TYPE(ESD_Size16, Native = Struct) // TODO: Struct support, expose values
typedef struct
{
	int16_t Width, Height;
} ESD_Size16;

ESD_TYPE(ESD_Rect16, Native = Struct) // TODO: Struct support, expose values
typedef struct
{
	union
	{
		ESD_Point16 Position;
		struct
		{
			int16_t X, Y;
		};
	};
	union
	{
		ESD_Size16 Size;
		struct
		{
			int16_t Width, Height;
		};
	};
} ESD_Rect16;

ESD_TYPE(ESD_URect16, Native = Struct) // TODO: Struct support, expose values
typedef struct
{
	uint16_t X, Y, Width, Height;
} ESD_URect16;
ESD_TYPE(ESD_Rect32, Native = Struct) // TODO: Struct support, expose values
typedef struct
{
	int32_t X, Y, Width, Height;
} ESD_Rect32;
ESD_TYPE(ESD_URect32, Native = Struct) // TODO: Struct support, expose values
typedef struct
{
	uint32_t X, Y, Width, Height;
} ESD_URect32;

bool ESD_Rect16_intersects(ESD_Rect16 a, ESD_Rect16 b);
bool ESD_Rect16_isInside(ESD_Rect16 subrect, ESD_Rect16 rect);
bool ESD_Rect16_isInsideHorizontal(ESD_Rect16 subrect, ESD_Rect16 rect);
bool ESD_Rect16_isInsideVertical(ESD_Rect16 subrect, ESD_Rect16 rect);

ESD_Rect16 ESD_Rect16_crop(ESD_Rect16 rect, ESD_Rect16 crop);

ESD_FUNCTION(ESD_Int16_clampedValue, Type = int16_t, DisplayName = "Clamp Int16", Category = EsdUtilities, Macro)
ESD_PARAMETER(value, Type = int16_t, Default = 0)
ESD_PARAMETER(min, Type = int16_t, Default = 0)
ESD_PARAMETER(max, Type = int16_t, Default = 100)
#define ESD_Int16_clampedValue(value, min, max) ((value) < (min) ? (min) : ((value) > (max) ? (max) : (value)))

ESD_FUNCTION(ESD_UInt16_clampedValue, Type = uint16_t, DisplayName = "Clamp UInt16", Category = EsdUtilities, Macro)
ESD_PARAMETER(value, Type = uint16_t, Default = 0)
ESD_PARAMETER(min, Type = uint16_t, Default = 0)
ESD_PARAMETER(max, Type = uint16_t, Default = 100)
#define ESD_UInt16_clampedValue(value, min, max) ((value) < (min) ? (min) : ((value) > (max) ? (max) : (value)))

ESD_FUNCTION(ESD_Int32_clampedValue, Type = int32_t, DisplayName = "Clamp Int32", Category = EsdUtilities, Macro)
ESD_PARAMETER(value, Type = int32_t, Default = 0)
ESD_PARAMETER(min, Type = int32_t, Default = 0)
ESD_PARAMETER(max, Type = int32_t, Default = 100)
#define ESD_Int32_clampedValue(value, min, max) ((value) < (min) ? (min) : ((value) > (max) ? (max) : (value)))

ESD_FUNCTION(ESD_UInt32_clampedValue, Type = uint32_t, DisplayName = "Clamp UInt32", Category = EsdUtilities, Macro)
ESD_PARAMETER(value, Type = uint32_t, Default = 0)
ESD_PARAMETER(min, Type = uint32_t, Default = 0)
ESD_PARAMETER(max, Type = uint32_t, Default = 100)
#define ESD_UInt32_clampedValue(value, min, max) ((value) < (min) ? (min) : ((value) > (max) ? (max) : (value)))

ESD_FUNCTION(ESD_Float_clampedValue, Type = float, DisplayName = "Clamp Float", Category = EsdUtilities, Macro)
ESD_PARAMETER(value, Type = float, Default = 0)
ESD_PARAMETER(min, Type = float, Default = 0)
ESD_PARAMETER(max, Type = float, Default = 100)
#define ESD_Float_clampedValue(value, min, max) ((value) < (min) ? (min) : ((value) > (max) ? (max) : (value)))

#endif /* ESD_MATH__H */

/* end of file */
