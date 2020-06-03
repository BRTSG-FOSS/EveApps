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
#ifndef FT_ESD_MATH_H
#define FT_ESD_MATH_H

#include "Ft_Esd.h"

ESD_TYPE(Ft_Esd_Point16, Native = Struct) // TODO: Struct support, expose values
typedef struct
{
	ft_int16_t X, Y;
} Ft_Esd_Point16;

ESD_TYPE(Ft_Esd_Size16, Native = Struct) // TODO: Struct support, expose values
typedef struct
{
	ft_int16_t Width, Height;
} Ft_Esd_Size16;

ESD_TYPE(Ft_Esd_Rect16, Native = Struct) // TODO: Struct support, expose values
typedef struct
{
	union
	{
		Ft_Esd_Point16 Position;
		struct
		{
			ft_int16_t X, Y;
		};
	};
	union
	{
		Ft_Esd_Size16 Size;
		struct
		{
			ft_int16_t Width, Height;
		};
	};
} Ft_Esd_Rect16;

ESD_TYPE(Ft_Esd_URect16, Native = Struct) // TODO: Struct support, expose values
typedef struct
{
	ft_uint16_t X, Y, Width, Height;
} Ft_Esd_URect16;
ESD_TYPE(Ft_Esd_Rect32, Native = Struct) // TODO: Struct support, expose values
typedef struct
{
	ft_int32_t X, Y, Width, Height;
} Ft_Esd_Rect32;
ESD_TYPE(Ft_Esd_URect32, Native = Struct) // TODO: Struct support, expose values
typedef struct
{
	ft_uint32_t X, Y, Width, Height;
} Ft_Esd_URect32;

ft_bool_t Ft_Esd_Rect16_Intersects(Ft_Esd_Rect16 a, Ft_Esd_Rect16 b);
ft_bool_t Ft_Esd_Rect16_IsInside(Ft_Esd_Rect16 subrect, Ft_Esd_Rect16 rect);
ft_bool_t Ft_Esd_Rect16_IsInsideHorizontal(Ft_Esd_Rect16 subrect, Ft_Esd_Rect16 rect);
ft_bool_t Ft_Esd_Rect16_IsInsideVertical(Ft_Esd_Rect16 subrect, Ft_Esd_Rect16 rect);

Ft_Esd_Rect16 Ft_Esd_Rect16_Crop(Ft_Esd_Rect16 rect, Ft_Esd_Rect16 crop);

ESD_FUNCTION(Ft_Esd_Int16_ClampedValue, Type = ft_int16_t, DisplayName = "Clamp Int16", Category = EsdUtilities, Macro)
ESD_PARAMETER(value, Type = ft_int16_t, Default = 0)
ESD_PARAMETER(min, Type = ft_int16_t, Default = 0)
ESD_PARAMETER(max, Type = ft_int16_t, Default = 100)
#define Ft_Esd_Int16_ClampedValue(value, min, max) ((value) < (min) ? (min) : ((value) > (max) ? (max) : (value)))

ESD_FUNCTION(Ft_Esd_UInt16_ClampedValue, Type = ft_uint16_t, DisplayName = "Clamp UInt16", Category = EsdUtilities, Macro)
ESD_PARAMETER(value, Type = ft_uint16_t, Default = 0)
ESD_PARAMETER(min, Type = ft_uint16_t, Default = 0)
ESD_PARAMETER(max, Type = ft_uint16_t, Default = 100)
#define Ft_Esd_UInt16_ClampedValue(value, min, max) ((value) < (min) ? (min) : ((value) > (max) ? (max) : (value)))

ESD_FUNCTION(Ft_Esd_Int32_ClampedValue, Type = ft_int32_t, DisplayName = "Clamp Int32", Category = EsdUtilities, Macro)
ESD_PARAMETER(value, Type = ft_int32_t, Default = 0)
ESD_PARAMETER(min, Type = ft_int32_t, Default = 0)
ESD_PARAMETER(max, Type = ft_int32_t, Default = 100)
#define Ft_Esd_Int32_ClampedValue(value, min, max) ((value) < (min) ? (min) : ((value) > (max) ? (max) : (value)))

ESD_FUNCTION(Ft_Esd_UInt32_ClampedValue, Type = ft_uint32_t, DisplayName = "Clamp UInt32", Category = EsdUtilities, Macro)
ESD_PARAMETER(value, Type = ft_uint32_t, Default = 0)
ESD_PARAMETER(min, Type = ft_uint32_t, Default = 0)
ESD_PARAMETER(max, Type = ft_uint32_t, Default = 100)
#define Ft_Esd_UInt32_ClampedValue(value, min, max) ((value) < (min) ? (min) : ((value) > (max) ? (max) : (value)))

ESD_FUNCTION(Ft_Esd_Float_ClampedValue, Type = float, DisplayName = "Clamp Float", Category = EsdUtilities, Macro)
ESD_PARAMETER(value, Type = float, Default = 0)
ESD_PARAMETER(min, Type = float, Default = 0)
ESD_PARAMETER(max, Type = float, Default = 100)
#define Ft_Esd_Float_ClampedValue(value, min, max) ((value) < (min) ? (min) : ((value) > (max) ? (max) : (value)))

#endif /* FT_ESD_MATH_H */

/* end of file */
