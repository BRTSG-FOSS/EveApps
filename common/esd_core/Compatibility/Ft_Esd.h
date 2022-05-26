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

#ifndef FT_COMPATIBILITY_ESD_H
#define FT_COMPATIBILITY_ESD_H

#include "Esd_Core.h"

#include "FT_Platform.h"

ESD_TYPE(ft_char8_t, Native = Char, Edit = String)
ESD_TYPE(ft_schar8_t, Native = Char, Edit = Integer)
ESD_TYPE(ft_uchar8_t, Native = Char, Edit = Integer)
ESD_TYPE(ft_uint8_t, Native = UInt8, Edit = Integer)
ESD_TYPE(ft_int16_t, Native = Int16, Edit = Integer)
ESD_TYPE(ft_uint16_t, Native = UInt16, Edit = Integer)
ESD_TYPE(ft_uint32_t, Native = UInt32, Edit = Integer)
ESD_TYPE(ft_int32_t, Native = Int32, Edit = Integer)
ESD_TYPE(ft_void_t, Native = Void)
ESD_TYPE(ft_int64_t, Native = Int64, Edit = Integer)
ESD_TYPE(ft_uint64_t, Native = UInt64, Edit = Integer)
ESD_TYPE(ft_float_t, Native = Float, Edit = Integer)
ESD_TYPE(ft_double_t, Native = Double, Edit = Real)
ESD_TYPE(ft_bool_t, Native = Int8, Edit = Boolean)
ESD_TYPE(ft_char8_t *, Native = Utf8, Edit = String)
ESD_TYPE(const ft_char8_t *, Native = Utf8, Edit = String)

typedef ft_int32_t ft_int32_f16_t;
ESD_TYPE(ft_int32_f16_t, Native = Int32, Edit = Fixed16)

typedef ft_int32_t ft_int32_f8_t;
ESD_TYPE(ft_int32_f8_t, Native = Int16, Edit = Fixed8)

typedef ft_int32_t ft_int32_f4_t;
ESD_TYPE(ft_int32_f4_t, Native = Int16, Edit = Fixed4)

typedef ft_int32_t ft_int32_f3_t;
ESD_TYPE(ft_int32_f3_t, Native = Int16, Edit = Fixed3)

typedef ft_int32_t ft_int32_f2_t;
ESD_TYPE(ft_int32_f2_t, Native = Int16, Edit = Fixed2)

typedef ft_int16_t ft_int16_f8_t;
ESD_TYPE(ft_int16_f8_t, Native = Int16, Edit = Fixed8)

typedef ft_int16_t ft_int16_f4_t;
ESD_TYPE(ft_int16_f4_t, Native = Int16, Edit = Fixed4)

typedef ft_int16_t ft_int16_f2_t;
ESD_TYPE(ft_int16_f2_t, Native = Int16, Edit = Fixed2)

typedef ft_uint32_t ft_argb32_t;
ESD_TYPE(ft_argb32_t, Native = UInt32, Edit = ColorARGB)

typedef ft_uint32_t ft_rgb32_t;
ESD_TYPE(ft_rgb32_t, Native = UInt32, Edit = ColorRGB)

typedef ft_uint32_t esd_classid_t;
ESD_TYPE(esd_classid_t, Native = UInt32, Edit = Library)

ESD_FUNCTION(Ft_Esd_Noop, Category = _GroupHidden, Macro)
ESD_PARAMETER(context, Type = void *)
#define Ft_Esd_Noop Esd_Noop

// TODO: This probably belongs in widgets framework, Ft_Esd.h is for core framework functionality only ->
ESD_ENUM(Ft_AlignX_Opt, Type = ft_uint16_t)
#define OPT_ALIGN_LEFT 0UL
#define OPT_ALIGN_CENTER 1UL
#define OPT_ALIGN_RIGHT 2UL
ESD_END()
ESD_ENUM(Ft_AlignY_Opt, Type = ft_uint16_t)
#define OPT_ALIGN_TOP 0UL
#define OPT_ALIGN_CENTER 1UL
#define OPT_ALIGN_BOTTOM 2UL
ESD_END()
// <-

#define Ft_Esd_LogMessage Esd_LogMessage
#define Ft_Esd_LogWarning Esd_LogWarning
#define Ft_Esd_LogError Esd_LogError

/* compatibility */

#ifdef FT_DispWidth
#undef FT_DispWidth
#endif
#ifdef FT_DispHeight
#undef FT_DispHeight
#endif

#define FT_DispWidth phost->Width
#define FT_DispHeight phost->Height

#define Esd_DispWidth phost->Width
#define Esd_DispHeight phost->Height

#define ESD_DispWidth phost->Width
#define ESD_DispHeight phost->Height

#define Ft_Esd_Host Esd_Host
#define Ft_Esd_GAlloc Esd_GAlloc

ft_uint32_t Ft_Esd_GetMillis();
ft_uint32_t Ft_Esd_GetDeltaMs();
EVE_HalContext *Ft_Esd_GetHost();

#endif /* #ifndef FT_ESD_H */

/* end of file */
