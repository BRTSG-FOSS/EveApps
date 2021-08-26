/*
ESD Framework
Copyright (C) 2018  Bridgetek Pte Lte
Author: Jan Boon <jan.boon@kaetemi.be>
*/

#ifndef ESD_COMPATIBILITY_BITMAPHANDLE_H
#define ESD_COMPATIBILITY_BITMAPHANDLE_H

#include "Esd_Core.h"

#include "Ft_Esd.h"
#include "Ft_Esd_BitmapInfo.h"
#include "Ft_Esd_FontInfo.h"

#define Ft_Esd_BitmapHandle_GetTotalUsed Esd_BitmapHandle_GetTotalUsed
#define Ft_Esd_BitmapHandle_GetTotal Esd_BitmapHandle_GetTotal

// Move bitmap source address by 128 cells per page
#define Ft_Esd_Dl_Bitmap_Page Esd_CoDl_PagedBitmapSource

// Set cell directly, using page if necessary
#define Ft_Esd_Dl_CELL_Paged Esd_CoDl_PagedCell

// Prepares a valid handle for a bitmap. Call during render to get bitmap handle. Does not necessarily call BITMAP_HANDLE, but may call it
ESD_FUNCTION(Ft_Esd_Dl_Bitmap_Setup, Type = uint8_t, DisplayName = "Setup Bitmap", Category = _GroupHidden, Macro)
ESD_PARAMETER(bitmapInfo, Type = Ft_Esd_BitmapInfo *)
#define Ft_Esd_Dl_Bitmap_Setup Esd_CoDl_SetupBitmap

// Prepares a valid handle for a font. Call during render to get font handle. Does not necessarily change the current bitmap handle, but may change it
ESD_FUNCTION(Ft_Esd_Dl_RomFont_Setup, Type = uint8_t, DisplayName = "Setup ROM Font", Category = _GroupHidden, Macro)
ESD_PARAMETER(font, Type = uint8_t)
#define Ft_Esd_Dl_RomFont_Setup Esd_CoDl_SetupRomFont

// Prepares a valid handle for a font. Call during render to get font handle. Does not necessarily call BITMAP_HANDLE, but may call it
ESD_FUNCTION(Ft_Esd_Dl_Font_Setup, Type = uint8_t, DisplayName = "Setup Font", Category = _GroupHidden, Macro)
ESD_PARAMETER(fontInfo, Type = Esd_FontInfo *)
#define Ft_Esd_Dl_Font_Setup Esd_CoDl_SetupFont

#define Ft_Esd_Dl_Bitmap_WidthHeight Esd_CoDl_BitmapWidthHeight
#define Ft_Esd_Dl_Bitmap_WidthHeight_BILINEAR Esd_CoDl_BitmapSize
#define Ft_Esd_Dl_Bitmap_WidthHeightReset Esd_CoDl_BitmapSizeReset

/* end of supported functions */

#endif /* #ifndef ESD_BITMAPHANDLE_H */

/* end of file */
