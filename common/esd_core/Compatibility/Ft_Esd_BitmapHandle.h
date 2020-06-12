/*
ESD Framework
Copyright (C) 2018  Bridgetek Pte Lte
Author: Jan Boon <jan.boon@kaetemi.be>
*/

#ifndef ESD_COMPATIBILITY_BITMAPHANDLE_H
#define ESD_COMPATIBILITY_BITMAPHANDLE_H

#include "ESD_Core.h"

#include "Ft_Esd.h"
#include "Ft_Esd_BitmapInfo.h"
#include "Ft_Esd_FontInfo.h"

#define Esd_HandleState ESD_HandleState

#define Esd_BitmapHandle_Initialize ESD_BitmapHandle_Initialize
#define Esd_BitmapHandle_FrameStart ESD_BitmapHandle_FrameStart
#define Esd_BitmapHandle_Reset ESD_BitmapHandle_Reset
#define Ft_Esd_BitmapHandle_GetTotalUsed ESD_BitmapHandle_GetTotalUsed
#define Ft_Esd_BitmapHandle_GetTotal ESD_BitmapHandle_GetTotal

ESD_FUNCTION(Esd_GetRomFont, Type = Esd_FontInfo *, DisplayName = "Get ROM Font", Category = _GroupHidden, Macro)
ESD_PARAMETER(i, Type = int, DisplayName = "ROM Font", Default = 0, Min = 16, Max = 34)
#define Esd_GetRomFont ESD_GetRomFont

// Move bitmap source address by 128 cells per page
#define Ft_Esd_Dl_Bitmap_Page ESD_CoDl_PagedBitmapSource

// Set cell directly, using page if necessary
#define Ft_Esd_Dl_CELL_Paged ESD_CoDl_PagedCell

// Prepares a valid handle for a bitmap. Call during render to get bitmap handle. Does not necessarily call BITMAP_HANDLE, but may call it
ESD_FUNCTION(Ft_Esd_Dl_Bitmap_Setup, Type = uint8_t, DisplayName = "Setup Bitmap", Category = _GroupHidden, Macro)
ESD_PARAMETER(bitmapInfo, Type = Ft_Esd_BitmapInfo *)
#define Ft_Esd_Dl_Bitmap_Setup ESD_CoDl_SetupBitmap

// Prepares a valid handle for a font. Call during render to get font handle. Does not necessarily change the current bitmap handle, but may change it
ESD_FUNCTION(Ft_Esd_Dl_RomFont_Setup, Type = uint8_t, DisplayName = "Setup ROM Font", Category = _GroupHidden, Macro)
ESD_PARAMETER(font, Type = uint8_t)
#define Ft_Esd_Dl_RomFont_Setup ESD_CoDl_SetupRomFont

// Prepares a valid handle for a font. Call during render to get font handle. Does not necessarily call BITMAP_HANDLE, but may call it
ESD_FUNCTION(Ft_Esd_Dl_Font_Setup, Type = uint8_t, DisplayName = "Setup Font", Category = _GroupHidden, Macro)
ESD_PARAMETER(fontInfo, Type = Esd_FontInfo *)
#define Ft_Esd_Dl_Font_Setup ESD_CoDl_SetupFont

#define Ft_Esd_Dl_Bitmap_WidthHeight ESD_CoDl_BitmapWidthHeight
#define Ft_Esd_Dl_Bitmap_WidthHeight_BILINEAR ESD_CoDl_BitmapSize
#define Ft_Esd_Dl_Bitmap_WidthHeightReset ESD_CoDl_BitmapSizeReset

ESD_FUNCTION(Esd_GetFontHeight, Type = uint16_t, DisplayName = "Get Font Height", Category = _GroupHidden, Macro)
ESD_PARAMETER(fontInfo, Type = Esd_FontInfo *)
#define Esd_GetFontHeight ESD_GetFontHeight

ESD_FUNCTION(Esd_GetFontBaseLine, Type = uint16_t, DisplayName = "Get Font Base Line", Category = _GroupHidden, Macro)
ESD_PARAMETER(fontInfo, Type = Esd_FontInfo *)
#define Esd_GetFontBaseLine ESD_GetFontBaseLine

ESD_FUNCTION(Esd_GetFontCapsHeight, Type = uint16_t, DisplayName = "Get Font Caps Height", Category = _GroupHidden, Macro)
ESD_PARAMETER(fontInfo, Type = Esd_FontInfo *)
#define Esd_GetFontCapsHeight ESD_GetFontCapsHeight

/* end of supported functions */

#endif /* #ifndef ESD_BITMAPHANDLE_H */

/* end of file */
