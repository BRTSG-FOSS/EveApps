/*
ESD Framework
Copyright (C) 2018  Bridgetek Pte Lte
Author: Jan Boon <jan.boon@kaetemi.be>
*/

#ifndef ESD_BITMAPHANDLE_H
#define ESD_BITMAPHANDLE_H

#include "Ft_Esd.h"
#include "Ft_Esd_BitmapInfo.h"
#include "Ft_Esd_FontInfo.h"

// Handle allocation state
typedef struct
{
	void *Info[FT_ESD_BITMAPHANDLE_CAP];
	Ft_Esd_GpuHandle GpuHandle[FT_ESD_BITMAPHANDLE_CAP];
	uint8_t Use[FT_ESD_BITMAPHANDLE_CAP];

	ft_bool_t Resized[FT_ESD_BITMAPHANDLE_CAP];
	uint8_t Page[FT_ESD_BITMAPHANDLE_CAP];

} Esd_HandleState;

/// Initialize bitmap handle tracking globally
void Esd_BitmapHandle_Initialize();

/// Call at start of frame to start tracking handle usage for the new frame for the current context
void Esd_BitmapHandle_FrameStart(Esd_HandleState *state);

/// Reset the bitmap handle state
void Esd_BitmapHandle_Reset(Esd_HandleState *state);

uint32_t Ft_Esd_BitmapHandle_GetTotalUsed();
uint32_t Ft_Esd_BitmapHandle_GetTotal();

ESD_FUNCTION(Esd_GetRomFont, Type = Esd_FontInfo *, DisplayName = "Get ROM Font", Category = EsdUtilities, Macro)
ESD_PARAMETER(i, Type = int, DisplayName = "ROM Font", Default = 0, Min = 16, Max = 34)
Esd_FontInfo *Esd_GetRomFont(uint8_t font);

// Move bitmap source address by 128 cells per page
void Ft_Esd_Dl_Bitmap_Page(uint8_t handle, uint8_t page);

// Set cell directly, using page if necessary
void Ft_Esd_Dl_CELL_Paged(uint8_t handle, uint16_t cell);

// Prepares a valid handle for a bitmap. Call during render to get bitmap handle. Does not necessarily call BITMAP_HANDLE, but may call it
ESD_FUNCTION(Ft_Esd_Dl_Bitmap_Setup, Type = uint8_t, DisplayName = "Setup Bitmap", Category = EveRenderFunctions)
ESD_PARAMETER(bitmapInfo, Type = Ft_Esd_BitmapInfo *)
uint8_t Ft_Esd_Dl_Bitmap_Setup(Ft_Esd_BitmapInfo *bitmapInfo);

// Prepares a valid handle for a font. Call during render to get font handle. Does not necessarily change the current bitmap handle, but may change it
ESD_FUNCTION(Ft_Esd_Dl_RomFont_Setup, Type = uint8_t, DisplayName = "Setup ROM Font", Category = EveRenderFunctions)
ESD_PARAMETER(font, Type = uint8_t)
uint8_t Ft_Esd_Dl_RomFont_Setup(uint8_t font);

// Prepares a valid handle for a font. Call during render to get font handle. Does not necessarily call BITMAP_HANDLE, but may call it
ESD_FUNCTION(Ft_Esd_Dl_Font_Setup, Type = uint8_t, DisplayName = "Setup Font", Category = EveRenderFunctions)
ESD_PARAMETER(fontInfo, Type = Esd_FontInfo *)
uint8_t Ft_Esd_Dl_Font_Setup(Esd_FontInfo *fontInfo);

void Ft_Esd_Dl_Bitmap_WidthHeight(uint8_t handle, uint16_t width, uint16_t height);
void Ft_Esd_Dl_Bitmap_WidthHeight_BILINEAR(uint8_t handle, uint16_t width, uint16_t height);
void Ft_Esd_Dl_Bitmap_WidthHeightReset(uint8_t handle);

ESD_FUNCTION(Esd_GetFontHeight, Type = uint16_t, DisplayName = "Get Font Height", Category = EsdUtilities)
ESD_PARAMETER(fontInfo, Type = Esd_FontInfo *)
uint16_t Esd_GetFontHeight(Esd_FontInfo *fontInfo);

ESD_FUNCTION(Esd_GetFontBaseLine, Type = uint16_t, DisplayName = "Get Font Base Line", Category = EsdUtilities)
ESD_PARAMETER(fontInfo, Type = Esd_FontInfo *)
uint16_t Esd_GetFontBaseLine(Esd_FontInfo *fontInfo);

ESD_FUNCTION(Esd_GetFontCapsHeight, Type = uint16_t, DisplayName = "Get Font Caps Height", Category = EsdUtilities)
ESD_PARAMETER(fontInfo, Type = Esd_FontInfo *)
uint16_t Esd_GetFontCapsHeight(Esd_FontInfo *fontInfo);

/* end of supported functions */

#endif /* #ifndef ESD_BITMAPHANDLE_H */

/* end of file */
