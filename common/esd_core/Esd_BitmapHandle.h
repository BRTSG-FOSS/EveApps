/*
ESD Core
Copyright (C) 2018-2020  Bridgetek Pte Lte
Author: Jan Boon <jan.boon@kaetemi.be>
*/

#ifndef ESD_BITMAPHANDLE__H
#define ESD_BITMAPHANDLE__H

#include "ESD_Base.h"
#include "ESD_BitmapInfo.h"
#include "ESD_FontInfo.h"

// Handle allocation state
typedef struct
{
	void *Info[ESD_BITMAPHANDLE_CAP];
	ESD_GpuHandle GpuHandle[ESD_BITMAPHANDLE_CAP];
	uint8_t Use[ESD_BITMAPHANDLE_CAP];

	bool Resized[ESD_BITMAPHANDLE_CAP];
	uint8_t Page[ESD_BITMAPHANDLE_CAP];

} ESD_HandleState;

/// Initialize bitmap handle tracking globally
void ESD_BitmapHandle_initialize();

/// Call at start of frame to start tracking handle usage for the new frame for the current context
void ESD_BitmapHandle_frameStart(ESD_HandleState *state);

/// Reset the bitmap handle state
void ESD_BitmapHandle_reset(ESD_HandleState *state);

uint32_t ESD_BitmapHandle_GetTotalUsed();
uint32_t ESD_BitmapHandle_GetTotal();

ESD_FUNCTION(ESD_getRomFont, Type = Esd_FontInfo *, DisplayName = "Get ROM Font", Category = EsdUtilities, Macro)
ESD_PARAMETER(i, Type = int, DisplayName = "ROM Font", Default = 0, Min = 16, Max = 34)
Esd_FontInfo *ESD_getRomFont(uint8_t font);

// Move bitmap source address by 128 cells per page, page is `cell >> 7`
void ESD_CoDl_pagedBitmapSource(uint8_t handle, uint8_t page);

// Set cell directly, using page if necessary
void ESD_CoDl_pagedCell(uint8_t handle, uint16_t cell);

// Prepares a valid handle for a bitmap. Call during render to get bitmap handle. Does not necessarily call BITMAP_HANDLE, but may call it
ESD_FUNCTION(ESD_CoDl_setupBitmap, Type = uint8_t, DisplayName = "Setup Bitmap", Category = EveRenderFunctions)
ESD_PARAMETER(bitmapInfo, Type = ESD_BitmapInfo *)
uint8_t ESD_CoDl_setupBitmap(ESD_BitmapInfo *bitmapInfo);

// Prepares a valid handle for a font. Call during render to get font handle. Does not necessarily change the current bitmap handle, but may change it
ESD_FUNCTION(ESD_CoDl_setupRomFont, Type = uint8_t, DisplayName = "Setup ROM Font", Category = EveRenderFunctions)
ESD_PARAMETER(font, Type = uint8_t)
uint8_t ESD_CoDl_setupRomFont(uint8_t font);

// Prepares a valid handle for a font. Call during render to get font handle. Does not necessarily call BITMAP_HANDLE, but may call it
ESD_FUNCTION(ESD_CoDl_setupFont, Type = uint8_t, DisplayName = "Setup Font", Category = EveRenderFunctions)
ESD_PARAMETER(fontInfo, Type = Esd_FontInfo *)
uint8_t ESD_CoDl_setupFont(Esd_FontInfo *fontInfo);

void ESD_CoDl_bitmapWidthHeight(uint8_t handle, uint16_t width, uint16_t height);
void ESD_CoDl_bitmapSize(uint8_t handle, uint8_t filter, uint8_t wrapx, uint8_t wrapy, uint16_t width, uint16_t height);
void ESD_CoDl_bitmapSizeReset(uint8_t handle);

ESD_FUNCTION(ESD_getFontHeight, Type = uint16_t, DisplayName = "Get Font Height", Category = EsdUtilities)
ESD_PARAMETER(fontInfo, Type = Esd_FontInfo *)
uint16_t ESD_getFontHeight(Esd_FontInfo *fontInfo);

ESD_FUNCTION(ESD_getFontBaseLine, Type = uint16_t, DisplayName = "Get Font Base Line", Category = EsdUtilities)
ESD_PARAMETER(fontInfo, Type = Esd_FontInfo *)
uint16_t ESD_getFontBaseLine(Esd_FontInfo *fontInfo);

ESD_FUNCTION(ESD_getFontCapsHeight, Type = uint16_t, DisplayName = "Get Font Caps Height", Category = EsdUtilities)
ESD_PARAMETER(fontInfo, Type = Esd_FontInfo *)
uint16_t ESD_getFontCapsHeight(Esd_FontInfo *fontInfo);

/* end of supported functions */

#endif /* #ifndef ESD_BITMAPHANDLE__H */

/* end of file */
