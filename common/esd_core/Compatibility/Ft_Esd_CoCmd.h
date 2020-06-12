
#ifndef ESD_COMPATIBILITY_COCMD__H
#define ESD_COMPATIBILITY_COCMD__H

#include "EVE_Hal.h"
#include "Gpu_CoCmd.h"

#include "ESD_CoWidget.h"

ESD_RENDER(Ft_Gpu_CoCmd_Text, Type = ft_void_t, Category = _GroupHidden)
ESD_PARAMETER(phost, Type = EVE_HalContext *, Default = Ft_Esd_GetHost, Hidden, Internal, Static) // PHOST
ESD_PARAMETER(x, Type = ft_int16_t, Default = 0) // SCREEN_SIZE
ESD_PARAMETER(y, Type = ft_int16_t, Default = 0) // SCREEN_SIZE
ESD_PARAMETER(font, Type = ft_int16_t, Default = 21, Min = 0, Max = 31) // BITMAP_HANDLE
ESD_PARAMETER(options, Type = Ft_CoPro_Opt, Default = 0) // TODO: Option flags / Bit enum?
ESD_PARAMETER(s, Type = const ft_char8_t *, Default = "Text")

ESD_RENDER(Ft_Gpu_CoCmd_Text_Ex, Type = ft_void_t, Category = _GroupHidden)
ESD_PARAMETER(phost, Type = EVE_HalContext *, Default = Ft_Esd_GetHost, Hidden, Internal, Static) // PHOST
ESD_PARAMETER(x, Type = ft_int16_t, Default = 0) // SCREEN_SIZE
ESD_PARAMETER(y, Type = ft_int16_t, Default = 0) // SCREEN_SIZE
ESD_PARAMETER(font, Type = ft_int16_t, Default = 21, Min = 0, Max = 31) // BITMAP_HANDLE
ESD_PARAMETER(options, Type = Ft_CoPro_Opt, Default = 0) // TODO: Option flags / Bit enum?
ESD_PARAMETER(bottom, Type = ft_bool_t, Default = 0)
ESD_PARAMETER(baseLine, Type = ft_int16_t, Default = 0)
ESD_PARAMETER(capsHeight, Type = ft_int16_t, Default = 0)
ESD_PARAMETER(s, Type = const ft_char8_t *, Default = "Text")

ESD_RENDER(Ft_Gpu_CoCmd_Number, Type = ft_void_t, Category = _GroupHidden)
ESD_PARAMETER(phost, Type = EVE_HalContext *, Default = Ft_Esd_GetHost, Hidden, Internal, Static) // PHOST
ESD_PARAMETER(x, Type = ft_int16_t, Default = 0) // SCREEN_SIZE
ESD_PARAMETER(y, Type = ft_int16_t, Default = 0) // SCREEN_SIZE
ESD_PARAMETER(font, Type = ft_int16_t, Default = 21, Min = 16, Max = 34) // BITMAP_HANDLE
ESD_PARAMETER(options, Type = ft_uint16_t, Default = 256) // TODO: Option flags / Bit enum?
ESD_PARAMETER(n, Type = ft_int32_t, Default = 0)

ESD_FUNCTION(Ft_Gpu_CoCmd_LoadIdentity, Type = ft_void_t, Category = _GroupHidden)
ESD_PARAMETER(phost, Type = EVE_HalContext *, Default = Ft_Esd_GetHost, Hidden, Internal, Static) // PHOST

ESD_RENDER(Ft_Gpu_CoCmd_Toggle, Type = ft_void_t, Category = _GroupHidden)
ESD_PARAMETER(phost, Type = EVE_HalContext *, Default = Ft_Esd_GetHost, Hidden, Internal, Static) // PHOST
ESD_PARAMETER(x, Type = ft_int16_t, Default = 0) // SCREEN_SIZE
ESD_PARAMETER(y, Type = ft_int16_t, Default = 0) // SCREEN_SIZE
ESD_PARAMETER(width, Type = ft_int16_t, Default = 60) // SCREEN_SIZE
ESD_PARAMETER(font, Type = ft_int16_t, Default = 21, Min = 0, Max = 31) // BITMAP_HANDLE
ESD_PARAMETER(options, Type = Ft_CoPro_Opt, Default = 0) // TODO: Option flags / Bit enum?
ESD_PARAMETER(state, Type = ft_uint16_t, Default = 0)
ESD_PARAMETER(s, Type = const ft_char8_t *, Default = "on\x00off")

ESD_RENDER(Ft_Gpu_CoCmd_Gauge, Type = ft_void_t, Category = _GroupHidden)
ESD_PARAMETER(phost, Type = EVE_HalContext *, Default = Ft_Esd_GetHost, Hidden, Internal, Static) // PHOST
ESD_PARAMETER(x, Type = ft_int16_t, Default = 0) // SCREEN_SIZE
ESD_PARAMETER(y, Type = ft_int16_t, Default = 0) // SCREEN_SIZE
ESD_PARAMETER(r, Type = ft_int16_t, Default = 40) // SCREEN_SIZE
ESD_PARAMETER(options, Type = Ft_CoPro_Opt, Default = 0) // TODO: Option flags / Bit enum?
ESD_PARAMETER(major, Type = ft_uint16_t, Default = 0)
ESD_PARAMETER(minor, Type = ft_uint16_t, Default = 0)
ESD_PARAMETER(val, Type = ft_uint16_t, Default = 0)
ESD_PARAMETER(range, Type = ft_uint16_t, Default = 0)

ESD_FUNCTION(Ft_Gpu_CoCmd_MemCpy, Type = ft_void_t, Category = _GroupHidden)
ESD_PARAMETER(phost, Type = EVE_HalContext *, Default = Ft_Esd_GetHost, Hidden, Internal, Static) // PHOST
ESD_PARAMETER(dest, Type = ft_uint32_t, Default = 0) // MEMORY_ADDRESS
ESD_PARAMETER(src, Type = ft_uint32_t, Default = 0) // MEMORY_ADDRESS
ESD_PARAMETER(num, Type = ft_uint32_t, Default = 0) // MEMORY_ADDRESS

ESD_FUNCTION(Ft_Gpu_CoCmd_BgColor, Type = ft_void_t, Category = _GroupHidden)
ESD_PARAMETER(phost, Type = EVE_HalContext *, Default = Ft_Esd_GetHost, Hidden, Internal, Static) // PHOST
ESD_PARAMETER(c, Type = ft_rgb32_t, Default = 0) // COLOR

ESD_FUNCTION(Ft_Gpu_CoCmd_Translate, Type = ft_void_t, Category = _GroupHidden)
ESD_PARAMETER(phost, Type = EVE_HalContext *, Default = Ft_Esd_GetHost, Hidden, Internal, Static) // PHOST
ESD_PARAMETER(tx, Type = ft_int32_t, Default = 0)
ESD_PARAMETER(ty, Type = ft_int32_t, Default = 0)

ESD_FUNCTION(Ft_Gpu_CoCmd_SetBase, Type = ft_void_t, Category = _GroupHidden)
ESD_PARAMETER(phost, Type = EVE_HalContext *, Default = Ft_Esd_GetHost, Hidden, Internal, Static) // PHOST
ESD_PARAMETER(base, Type = ft_int32_t, Default = 0)

ESD_RENDER(Ft_Gpu_CoCmd_Slider, Type = ft_void_t, Category = _GroupHidden)
ESD_PARAMETER(phost, Type = EVE_HalContext *, Default = Ft_Esd_GetHost, Hidden, Internal, Static) // PHOST
ESD_PARAMETER(x, Type = ft_int16_t, Default = 0) // SCREEN_SIZE
ESD_PARAMETER(y, Type = ft_int16_t, Default = 0) // SCREEN_SIZE
ESD_PARAMETER(width, Type = ft_int16_t, Default = 60) // SCREEN_SIZE
ESD_PARAMETER(height, Type = ft_int16_t, Default = 40) // SCREEN_SIZE
ESD_PARAMETER(options, Type = Ft_CoPro_Opt, Default = 0) // TODO: Option flags / Bit enum?
ESD_PARAMETER(val, Type = ft_uint16_t, Default = 0)
ESD_PARAMETER(range, Type = ft_uint16_t, Default = 0)

ESD_FUNCTION(Ft_Gpu_CoCmd_Interrupt, Type = ft_void_t, Category = _GroupHidden)
ESD_PARAMETER(phost, Type = EVE_HalContext *, Default = Ft_Esd_GetHost, Hidden, Internal, Static) // PHOST
ESD_PARAMETER(ms, Type = ft_int32_t, Default = 0)

ESD_FUNCTION(Ft_Gpu_CoCmd_FgColor, Type = ft_void_t, Category = _GroupHidden)
ESD_PARAMETER(phost, Type = EVE_HalContext *, Default = Ft_Esd_GetHost, Hidden, Internal, Static) // PHOST
ESD_PARAMETER(c, Type = ft_rgb32_t, Default = 0) // COLOR

ESD_FUNCTION(Ft_Gpu_CoCmd_Rotate, Type = ft_void_t, Category = _GroupHidden)
ESD_PARAMETER(phost, Type = EVE_HalContext *, Default = Ft_Esd_GetHost, Hidden, Internal, Static) // PHOST
ESD_PARAMETER(a, Type = ft_int32_t, Default = 0)

ESD_RENDER(Ft_Gpu_CoCmd_Button, Type = ft_void_t, Category = _GroupHidden)
ESD_PARAMETER(phost, Type = EVE_HalContext *, Default = Ft_Esd_GetHost, Hidden, Internal, Static) // PHOST
ESD_PARAMETER(x, Type = ft_int16_t, Default = 0) // SCREEN_SIZE
ESD_PARAMETER(y, Type = ft_int16_t, Default = 0) // SCREEN_SIZE
ESD_PARAMETER(width, Type = ft_int16_t, Default = 60) // SCREEN_SIZE
ESD_PARAMETER(height, Type = ft_int16_t, Default = 40) // SCREEN_SIZE
ESD_PARAMETER(font, Type = ft_int16_t, Default = 21, Min = 0, Max = 31) // BITMAP_HANDLE
ESD_PARAMETER(options, Type = Ft_CoPro_Opt, Default = 0) // TODO: Option flags / Bit enum?
ESD_PARAMETER(s, Type = const ft_char8_t *, Default = "Button")

ESD_RENDER(Ft_Gpu_CoCmd_Scrollbar, Type = ft_void_t, Category = _GroupHidden)
ESD_PARAMETER(phost, Type = EVE_HalContext *, Default = Ft_Esd_GetHost, Hidden, Internal, Static) // PHOST
ESD_PARAMETER(x, Type = ft_int16_t, Default = 0) // SCREEN_SIZE
ESD_PARAMETER(y, Type = ft_int16_t, Default = 0) // SCREEN_SIZE
ESD_PARAMETER(width, Type = ft_int16_t, Default = 60) // SCREEN_SIZE
ESD_PARAMETER(height, Type = ft_int16_t, Default = 40) // SCREEN_SIZE
ESD_PARAMETER(options, Type = Ft_CoPro_Opt, Default = 0) // TODO: Option flags / Bit enum?
ESD_PARAMETER(val, Type = ft_uint16_t, Default = 0)
ESD_PARAMETER(size, Type = ft_uint16_t, Default = 0)
ESD_PARAMETER(range, Type = ft_uint16_t, Default = 0)

ESD_FUNCTION(Ft_Gpu_CoCmd_MemSet, Type = ft_void_t, Category = _GroupHidden)
ESD_PARAMETER(phost, Type = EVE_HalContext *, Default = Ft_Esd_GetHost, Hidden, Internal, Static) // PHOST
ESD_PARAMETER(ptr, Type = ft_uint32_t, Default = 0) // MEMORY_ADDRESS
ESD_PARAMETER(value, Type = ft_uint32_t, Default = 0)
ESD_PARAMETER(num, Type = ft_uint32_t, Default = 0) // MEMORY_ADDRESS

ESD_FUNCTION(Ft_Gpu_CoCmd_BitmapTransform, Type = ft_void_t, Category = _GroupHidden)
ESD_PARAMETER(phost, Type = EVE_HalContext *, Default = Ft_Esd_GetHost, Hidden, Internal, Static) // PHOST
ESD_PARAMETER(x0, Type = ft_int32_t, Default = 0)
ESD_PARAMETER(y0, Type = ft_int32_t, Default = 0)
ESD_PARAMETER(x1, Type = ft_int32_t, Default = 0)
ESD_PARAMETER(y1, Type = ft_int32_t, Default = 0)
ESD_PARAMETER(x2, Type = ft_int32_t, Default = 0)
ESD_PARAMETER(y2, Type = ft_int32_t, Default = 0)
ESD_PARAMETER(tx0, Type = ft_int32_t, Default = 0)
ESD_PARAMETER(ty0, Type = ft_int32_t, Default = 0)
ESD_PARAMETER(tx1, Type = ft_int32_t, Default = 0)
ESD_PARAMETER(ty1, Type = ft_int32_t, Default = 0)
ESD_PARAMETER(tx2, Type = ft_int32_t, Default = 0)
ESD_PARAMETER(ty2, Type = ft_int32_t, Default = 0)
ESD_PARAMETER(result, Type = ft_uint16_t, Default = 0) // TODO: Check this

ESD_FUNCTION(Ft_Gpu_CoCmd_GradColor, Type = ft_void_t, Category = _GroupHidden)
ESD_PARAMETER(phost, Type = EVE_HalContext *, Default = Ft_Esd_GetHost, Hidden, Internal, Static) // PHOST
ESD_PARAMETER(c, Type = ft_rgb32_t, Default = 0) // COLOR

ESD_FUNCTION(Ft_Gpu_CoCmd_Sync, Type = ft_void_t, Category = _GroupHidden)
ESD_PARAMETER(phost, Type = EVE_HalContext *, Default = Ft_Esd_GetHost, Hidden, Internal, Static) // PHOST

ESD_FUNCTION(Ft_Gpu_CoCmd_Append, Type = ft_void_t, Category = _GroupHidden)
ESD_PARAMETER(phost, Type = EVE_HalContext *, Default = Ft_Esd_GetHost, Hidden, Internal, Static) // PHOST
ESD_PARAMETER(ptr, Type = ft_uint32_t, Default = 0) // MEMORY_ADDRESS
ESD_PARAMETER(num, Type = ft_uint32_t, Default = 0, Min = 0, Max = 2048)

ESD_FUNCTION(Ft_Gpu_CoCmd_MemZero, Type = ft_void_t, Category = _GroupHidden)
ESD_PARAMETER(phost, Type = EVE_HalContext *, Default = Ft_Esd_GetHost, Hidden, Internal, Static) // PHOST
ESD_PARAMETER(ptr, Type = ft_uint32_t, Default = 0) // MEMORY_ADDRESS
ESD_PARAMETER(num, Type = ft_uint32_t, Default = 0) // MEMORY_ADDRESS

ESD_FUNCTION(Ft_Gpu_CoCmd_Scale, Type = ft_void_t, Category = _GroupHidden)
ESD_PARAMETER(phost, Type = EVE_HalContext *, Default = Ft_Esd_GetHost, Hidden, Internal, Static) // PHOST
ESD_PARAMETER(sx, Type = ft_int32_t, Default = 0)
ESD_PARAMETER(sy, Type = ft_int32_t, Default = 0)

ESD_RENDER(Ft_Gpu_CoCmd_Clock, Type = ft_void_t, Category = _GroupHidden)
ESD_PARAMETER(phost, Type = EVE_HalContext *, Default = Ft_Esd_GetHost, Hidden, Internal, Static) // PHOST
ESD_PARAMETER(x, Type = ft_int16_t, Default = 0) // SCREEN_SIZE
ESD_PARAMETER(y, Type = ft_int16_t, Default = 0) // SCREEN_SIZE
ESD_PARAMETER(r, Type = ft_int16_t, Default = 40) // SCREEN_SIZE
ESD_PARAMETER(options, Type = Ft_CoPro_Opt, Default = 0) // TODO: Option flags / Bit enum?
ESD_PARAMETER(height, Type = ft_uint16_t, Default = 0)
ESD_PARAMETER(m, Type = ft_uint16_t, Default = 0)
ESD_PARAMETER(s, Type = ft_uint16_t, Default = 0)
ESD_PARAMETER(ms, Type = ft_uint16_t, Default = 0)

ESD_FUNCTION(Ft_Gpu_CoCmd_Gradient, Type = ft_void_t, Category = _GroupHidden)
ESD_PARAMETER(phost, Type = EVE_HalContext *, Default = Ft_Esd_GetHost, Hidden, Internal, Static) // PHOST
ESD_PARAMETER(x0, Type = ft_int16_t, Default = 0) // SCREEN_SIZE
ESD_PARAMETER(y0, Type = ft_int16_t, Default = 0) // SCREEN_SIZE
ESD_PARAMETER(rgb0, Type = ft_rgb32_t, Default = 0) // COLOR
ESD_PARAMETER(x1, Type = ft_int16_t, Default = 0) // SCREEN_SIZE
ESD_PARAMETER(y1, Type = ft_int16_t, Default = 0) // SCREEN_SIZE
ESD_PARAMETER(rgb1, Type = ft_rgb32_t, Default = 0) // COLOR

ESD_FUNCTION(Ft_Gpu_CoCmd_SetMatrix, Type = ft_void_t, Category = _GroupHidden)
ESD_PARAMETER(phost, Type = EVE_HalContext *, Default = Ft_Esd_GetHost, Hidden, Internal, Static) // PHOST

ESD_RENDER(Ft_Gpu_CoCmd_Progress, Type = ft_void_t, Category = _GroupHidden)
ESD_PARAMETER(phost, Type = EVE_HalContext *, Default = Ft_Esd_GetHost, Hidden, Internal, Static) // PHOST
ESD_PARAMETER(x, Type = ft_int16_t, Default = 0) // SCREEN_SIZE
ESD_PARAMETER(y, Type = ft_int16_t, Default = 0) // SCREEN_SIZE
ESD_PARAMETER(width, Type = ft_int16_t, Default = 60) // SCREEN_SIZE
ESD_PARAMETER(height, Type = ft_int16_t, Default = 40) // SCREEN_SIZE
ESD_PARAMETER(options, Type = Ft_CoPro_Opt, Default = 0) // TODO: Option flags / Bit enum?
ESD_PARAMETER(val, Type = ft_uint16_t, Default = 0)
ESD_PARAMETER(range, Type = ft_uint16_t, Default = 0)

ESD_RENDER(Ft_Gpu_CoCmd_Keys, Type = ft_void_t, Category = _GroupHidden)
ESD_PARAMETER(phost, Type = EVE_HalContext *, Default = Ft_Esd_GetHost, Hidden, Internal, Static) // PHOST
ESD_PARAMETER(x, Type = ft_int16_t, Default = 0) // SCREEN_SIZE
ESD_PARAMETER(y, Type = ft_int16_t, Default = 0) // SCREEN_SIZE
ESD_PARAMETER(width, Type = ft_int16_t, Default = 60) // SCREEN_SIZE
ESD_PARAMETER(height, Type = ft_int16_t, Default = 40) // SCREEN_SIZE
ESD_PARAMETER(font, Type = ft_int16_t, Default = 21, Min = 0, Max = 31) // BITMAP_HANDLE
ESD_PARAMETER(options, Type = Ft_CoPro_Opt, Default = 0) // TODO: Option flags / Bit enum?
ESD_PARAMETER(s, Type = const ft_char8_t *, Default = "Button")

ESD_RENDER(Ft_Gpu_CoCmd_Dial, Type = ft_void_t, Category = _GroupHidden)
ESD_PARAMETER(phost, Type = EVE_HalContext *, Default = Ft_Esd_GetHost, Hidden, Internal, Static) // PHOST
ESD_PARAMETER(x, Type = ft_int16_t, Default = 0) // SCREEN_SIZE
ESD_PARAMETER(y, Type = ft_int16_t, Default = 0) // SCREEN_SIZE
ESD_PARAMETER(r, Type = ft_int16_t, Default = 40) // SCREEN_SIZE
ESD_PARAMETER(options, Type = Ft_CoPro_Opt, Default = 0) // TODO: Option flags / Bit enum?
ESD_PARAMETER(val, Type = ft_uint16_t, Default = 0)

#define Ft_Esd_CoCmd_Spinner ESD_CoWidget_Spinner
#define Ft_Esd_CoCmd_ScreenSaver ESD_CoWidget_ScreenSaver
#define Ft_Esd_CoCmd_Sketch ESD_CoWidget_Sketch
#define Ft_Esd_CoCmd_Stop ESD_CoWidget_Stop

ESD_FUNCTION(Ft_Esd_CoCmd_Spinner, Type = ft_void_t, Category = _GroupHidden)
ESD_PARAMETER(owner, Type = void *, Default = 0, Hidden, Internal)
ESD_PARAMETER(x, Type = ft_int16_t, Default = 0) // SCREEN_SIZE
ESD_PARAMETER(y, Type = ft_int16_t, Default = 0) // SCREEN_SIZE
ESD_PARAMETER(style, Type = ft_uint16_t, Default = 0) // TODO: Spinner style enum
ESD_PARAMETER(scale, Type = ft_uint16_t, Default = 0)

ESD_FUNCTION(Ft_Esd_CoCmd_ScreenSaver, Type = ft_void_t, Category = _GroupHidden)
ESD_PARAMETER(owner, Type = void *, Default = 0, Hidden, Internal)

ESD_FUNCTION(Ft_Esd_CoCmd_Sketch, Type = ft_void_t, Category = _GroupHidden)
ESD_PARAMETER(owner, Type = void *, Default = 0, Hidden, Internal)
ESD_PARAMETER(globalRect, Type = Ft_Esd_Rect16, Default = 0) // SCREEN_SIZE
ESD_PARAMETER(addr, Type = ft_uint32_t, Default = 0)
ESD_PARAMETER(format, Type = ft_uint16_t, Default = L8)

ESD_FUNCTION(Ft_Esd_CoCmd_Stop, Type = ft_void_t, Category = _GroupHidden)
ESD_PARAMETER(owner, Type = void *, Default = 0, Hidden, Internal)

#endif
