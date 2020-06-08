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
void ESD_BitmapHandle_Initialize();

/// Call at start of frame to start tracking handle usage for the new frame for the current context
ESD_CORE_EXPORT void ESD_BitmapHandle_FrameStart(ESD_HandleState *state);

/// Reset the bitmap handle state
ESD_CORE_EXPORT void ESD_BitmapHandle_Reset(ESD_HandleState *state);

ESD_CORE_EXPORT uint32_t ESD_BitmapHandle_GetTotalUsed();
ESD_CORE_EXPORT uint32_t ESD_BitmapHandle_GetTotal();

#pragma ESD_FUNCTION(ESD_GetRomFont, Type = ESD_FontInfo *, Attributes = ESD_CORE_EXPORT, DisplayName = "Get ROM Font", Category = EsdUtilities)
#pragma ESD_PARAMETER(i, Type = int, DisplayName = "ROM Font", Default = 0, Min = 16, Max = 34)
ESD_CORE_EXPORT ESD_FontInfo *ESD_GetRomFont(uint8_t font);

// Move bitmap source address by 128 cells per page, page is `cell >> 7`
ESD_CORE_EXPORT void ESD_CoDl_PagedBitmapSource(uint8_t handle, uint8_t page);

// Set cell directly, using page if necessary
ESD_CORE_EXPORT void ESD_CoDl_PagedCell(uint8_t handle, uint16_t cell);

// Prepares a valid handle for a bitmap. Call during render to get bitmap handle. Does not necessarily call BITMAP_HANDLE, but may call it
#pragma ESD_FUNCTION(ESD_CoDl_SetupBitmap, Type = uint8_t, Attributes = ESD_CORE_EXPORT, DisplayName = "Setup Bitmap", Category = EveRenderFunctions)
#pragma ESD_PARAMETER(bitmapInfo, Type = ESD_BitmapInfo *)
ESD_CORE_EXPORT uint8_t ESD_CoDl_SetupBitmap(ESD_BitmapInfo *bitmapInfo);

// Prepares a valid handle for a font. Call during render to get font handle. Does not necessarily change the current bitmap handle, but may change it
#pragma ESD_FUNCTION(ESD_CoDl_SetupRomFont, Type = uint8_t, Attributes = ESD_CORE_EXPORT, DisplayName = "Setup ROM Font", Category = EveRenderFunctions)
#pragma ESD_PARAMETER(font, Type = uint8_t)
ESD_CORE_EXPORT uint8_t ESD_CoDl_SetupRomFont(uint8_t font);

// Prepares a valid handle for a font. Call during render to get font handle. Does not necessarily call BITMAP_HANDLE, but may call it
#pragma ESD_FUNCTION(ESD_CoDl_SetupFont, Type = uint8_t, Attributes = ESD_CORE_EXPORT, DisplayName = "Setup Font", Category = EveRenderFunctions)
#pragma ESD_PARAMETER(fontInfo, Type = ESD_FontInfo *)
ESD_CORE_EXPORT uint8_t ESD_CoDl_SetupFont(ESD_FontInfo *fontInfo);

ESD_CORE_EXPORT void ESD_CoDl_BitmapWidthHeight(uint8_t handle, uint16_t width, uint16_t height);
ESD_CORE_EXPORT void ESD_CoDl_BitmapSize(uint8_t handle, uint8_t filter, uint8_t wrapx, uint8_t wrapy, uint16_t width, uint16_t height);
ESD_CORE_EXPORT void ESD_CoDl_BitmapSizeReset(uint8_t handle);

#pragma ESD_FUNCTION(ESD_GetFontHeight, Type = uint16_t, Attributes = ESD_CORE_EXPORT, DisplayName = "Get Font Height", Category = EsdUtilities)
#pragma ESD_PARAMETER(fontInfo, Type = ESD_FontInfo *)
ESD_CORE_EXPORT uint16_t ESD_GetFontHeight(ESD_FontInfo *fontInfo);

/* Get the height of a builtin ROM font */
#pragma ESD_FUNCTION(ESD_GetRomFontHeight, Type = uint16_t, DisplayName = "Get ROM Font Height", Category = _GroupHidden, Macro)
#pragma ESD_PARAMETER(handle, Type = uint8_t, DisplayName = "ROM Font Handle", Default = 0, Min = 0, Max = 31)
#define ESD_GetRomFontHeight(handle) (ESD_GetFontHeight(ESD_GetRomFont(handle)));

#pragma ESD_FUNCTION(ESD_GetFontBaseLine, Type = uint16_t, Attributes = ESD_CORE_EXPORT, DisplayName = "Get Font Base Line", Category = EsdUtilities)
#pragma ESD_PARAMETER(fontInfo, Type = ESD_FontInfo *)
ESD_CORE_EXPORT uint16_t ESD_GetFontBaseLine(ESD_FontInfo *fontInfo);

#pragma ESD_FUNCTION(ESD_GetFontCapsHeight, Type = uint16_t, Attributes = ESD_CORE_EXPORT, DisplayName = "Get Font Caps Height", Category = EsdUtilities)
#pragma ESD_PARAMETER(fontInfo, Type = ESD_FontInfo *)
ESD_CORE_EXPORT uint16_t ESD_GetFontCapsHeight(ESD_FontInfo *fontInfo);

/* end of supported functions */

#endif /* #ifndef ESD_BITMAPHANDLE__H */

/* end of file */
