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
#ifndef FT_ESD_COMPATIBILITY_MATH_H
#define FT_ESD_COMPATIBILITY_MATH_H

#include "Esd_Core.h"
#include "Ft_Esd.h"

ESD_TYPE(Ft_Esd_Point16, Native = Struct) // TODO: Struct support, expose values
#define Ft_Esd_Point16 Esd_Point16

ESD_TYPE(Ft_Esd_Size16, Native = Struct) // TODO: Struct support, expose values
#define Ft_Esd_Size16 Esd_Size16

ESD_TYPE(Ft_Esd_Rect16, Native = Struct) // TODO: Struct support, expose values
#define Ft_Esd_Rect16 Esd_Rect16
ESD_TYPE(Ft_Esd_URect16, Native = Struct) // TODO: Struct support, expose values
#define Ft_Esd_URect16 Esd_URect16
ESD_TYPE(Ft_Esd_Rect32, Native = Struct) // TODO: Struct support, expose values
#define Ft_Esd_Rect32 Esd_Rect32
ESD_TYPE(Ft_Esd_URect32, Native = Struct) // TODO: Struct support, expose values
#define Ft_Esd_URect32 Esd_URect32

#define Ft_Esd_Rect16_Intersects Esd_Rect16_Intersects
#define Ft_Esd_Rect16_IsInside Esd_Rect16_IsInside
#define Ft_Esd_Rect16_IsInsideHorizontal Esd_Rect16_IsInsideHorizontal
#define Ft_Esd_Rect16_IsInsideVertical Esd_Rect16_IsInsideVertical

#define Ft_Esd_Rect16_Crop Esd_Rect16_Crop

ESD_FUNCTION(Ft_Esd_Int16_ClampedValue, Type = ft_int16_t, DisplayName = "Clamp Int16", Category = _GroupHidden, Macro)
ESD_PARAMETER(value, Type = ft_int16_t, Default = 0)
ESD_PARAMETER(min, Type = ft_int16_t, Default = 0)
ESD_PARAMETER(max, Type = ft_int16_t, Default = 100)
#define Ft_Esd_Int16_ClampedValue Esd_Int16_ClampedValue

ESD_FUNCTION(Ft_Esd_UInt16_ClampedValue, Type = ft_uint16_t, DisplayName = "Clamp UInt16", Category = _GroupHidden, Macro)
ESD_PARAMETER(value, Type = ft_uint16_t, Default = 0)
ESD_PARAMETER(min, Type = ft_uint16_t, Default = 0)
ESD_PARAMETER(max, Type = ft_uint16_t, Default = 100)
#define Ft_Esd_UInt16_ClampedValue Esd_UInt16_ClampedValue

ESD_FUNCTION(Ft_Esd_Int32_ClampedValue, Type = ft_int32_t, DisplayName = "Clamp Int32", Category = _GroupHidden, Macro)
ESD_PARAMETER(value, Type = ft_int32_t, Default = 0)
ESD_PARAMETER(min, Type = ft_int32_t, Default = 0)
ESD_PARAMETER(max, Type = ft_int32_t, Default = 100)
#define Ft_Esd_Int32_ClampedValue Esd_Int32_ClampedValue

ESD_FUNCTION(Ft_Esd_UInt32_ClampedValue, Type = ft_uint32_t, DisplayName = "Clamp UInt32", Category = _GroupHidden, Macro)
ESD_PARAMETER(value, Type = ft_uint32_t, Default = 0)
ESD_PARAMETER(min, Type = ft_uint32_t, Default = 0)
ESD_PARAMETER(max, Type = ft_uint32_t, Default = 100)
#define Ft_Esd_UInt32_ClampedValue Esd_UInt32_ClampedValue

ESD_FUNCTION(Ft_Esd_Float_ClampedValue, Type = float, DisplayName = "Clamp Float", Category = _GroupHidden, Macro)
ESD_PARAMETER(value, Type = float, Default = 0)
ESD_PARAMETER(min, Type = float, Default = 0)
ESD_PARAMETER(max, Type = float, Default = 100)
#define Ft_Esd_Float_ClampedValue Esd_Float_ClampedValue

#endif /* FT_ESD_MATH_H */

/* end of file */
