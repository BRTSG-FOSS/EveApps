/*
ESD Core
Copyright (C) 2018-2020  Bridgetek Pte Lte
Author: Jan Boon <jan.boon@kaetemi.be>
*/

#ifndef ESD_BITMAPHANDLE__H
#define ESD_BITMAPHANDLE__H

#include "Esd_Base.h"
#include "Esd_BitmapInfo.h"
#include "Esd_FontInfo.h"

#ifdef __cplusplus
extern "C" {
#endif

// Handle allocation state
typedef struct
{
	void *Info[ESD_BITMAPHANDLE_CAP];
	uint32_t Address[ESD_BITMAPHANDLE_CAP];
	Esd_GpuHandle GpuHandle[ESD_BITMAPHANDLE_CAP];
	uint8_t Use[ESD_BITMAPHANDLE_CAP];

	bool Resized[ESD_BITMAPHANDLE_CAP];
	uint8_t Page[ESD_BITMAPHANDLE_CAP];

} Esd_HandleState;

/// Initialize bitmap handle tracking globally
void Esd_BitmapHandle_Initialize();

/// Call at start of frame to start tracking handle usage for the new frame for the current context
ESD_CORE_EXPORT void Esd_BitmapHandle_FrameStart(Esd_HandleState *state);

/// Reset the bitmap handle state
ESD_CORE_EXPORT void Esd_BitmapHandle_Reset(Esd_HandleState *state);

ESD_CORE_EXPORT uint32_t Esd_BitmapHandle_GetTotalUsed();
ESD_CORE_EXPORT uint32_t Esd_BitmapHandle_GetTotal();

ESD_FUNCTION(Esd_GetRomFont, Type = Esd_FontInfo *, Attributes = ESD_CORE_EXPORT, DisplayName = "Get ROM Font", Category = EsdUtilities)
ESD_PARAMETER(i, Type = uint8_t, DisplayName = "ROM Font", Default = 0, Min = 16, Max = 34)
ESD_CORE_EXPORT Esd_FontInfo *Esd_GetRomFont(uint8_t font);

// Move bitmap source address by 128 cells per page, page is `cell >> 7`
ESD_CORE_EXPORT void Esd_CoDl_PagedBitmapSource(uint8_t handle, uint8_t page);

// Set cell directly, using page if necessary
ESD_CORE_EXPORT void Esd_CoDl_PagedCell(uint8_t handle, uint16_t cell);

// Prepares a valid handle for a bitmap. Call during render to get bitmap handle. Does not necessarily call BITMAP_HANDLE, but may call it
ESD_FUNCTION(Esd_CoDl_SetupBitmap, Type = uint8_t, Attributes = ESD_CORE_EXPORT, DisplayName = "Setup Bitmap", Category = EveRenderFunctions, Include = "Esd_Core.h")
ESD_PARAMETER(bitmapInfo, Type = Esd_BitmapInfo *)
ESD_CORE_EXPORT uint8_t Esd_CoDl_SetupBitmap(Esd_BitmapInfo *bitmapInfo);

// Prepares a valid handle for a font. Call during render to get font handle. Does not necessarily change the current bitmap handle, but may change it
ESD_FUNCTION(Esd_CoDl_SetupRomFont, Type = uint8_t, Attributes = ESD_CORE_EXPORT, DisplayName = "Setup ROM Font", Category = EveRenderFunctions, Include = "Esd_Core.h")
ESD_PARAMETER(font, Type = uint8_t)
ESD_CORE_EXPORT uint8_t Esd_CoDl_SetupRomFont(uint8_t font);

// Prepares a valid handle for a font. Call during render to get font handle. Does not necessarily call BITMAP_HANDLE, but may call it
ESD_FUNCTION(Esd_CoDl_SetupFont, Type = uint8_t, Attributes = ESD_CORE_EXPORT, DisplayName = "Setup Font", Category = EveRenderFunctions, Include = "Esd_Core.h")
ESD_PARAMETER(fontInfo, Type = Esd_FontInfo *)
ESD_CORE_EXPORT uint8_t Esd_CoDl_SetupFont(Esd_FontInfo *fontInfo);

ESD_CORE_EXPORT void Esd_CoDl_BitmapWidthHeight(uint8_t handle, uint16_t width, uint16_t height);
ESD_CORE_EXPORT void Esd_CoDl_BitmapSize(uint8_t handle, uint8_t filter, uint8_t wrapx, uint8_t wrapy, uint16_t width, uint16_t height);
ESD_CORE_EXPORT void Esd_CoDl_BitmapSizeReset(uint8_t handle);

ESD_FUNCTION(Esd_GetFontHeight, Type = uint16_t, Attributes = ESD_CORE_EXPORT, DisplayName = "Get Font Height", Category = EsdUtilities)
ESD_PARAMETER(fontInfo, Type = Esd_FontInfo *)
ESD_CORE_EXPORT uint16_t Esd_GetFontHeight(Esd_FontInfo *fontInfo);

/* Get the height of a builtin ROM font */
ESD_FUNCTION(Esd_GetRomFontHeight, Type = uint16_t, DisplayName = "Get ROM Font Height", Category = _GroupHidden, Macro)
ESD_PARAMETER(handle, Type = uint8_t, DisplayName = "ROM Font Handle", Default = 0, Min = 0, Max = 31)
#define Esd_GetRomFontHeight(handle) (Esd_GetFontHeight(Esd_GetRomFont(handle)));

ESD_FUNCTION(Esd_GetFontBaseLine, Type = uint16_t, Attributes = ESD_CORE_EXPORT, DisplayName = "Get Font Base Line", Category = EsdUtilities)
ESD_PARAMETER(fontInfo, Type = Esd_FontInfo *)
ESD_CORE_EXPORT uint16_t Esd_GetFontBaseLine(Esd_FontInfo *fontInfo);

ESD_FUNCTION(Esd_GetFontCapsHeight, Type = uint16_t, Attributes = ESD_CORE_EXPORT, DisplayName = "Get Font Caps Height", Category = EsdUtilities)
ESD_PARAMETER(fontInfo, Type = Esd_FontInfo *)
ESD_CORE_EXPORT uint16_t Esd_GetFontCapsHeight(Esd_FontInfo *fontInfo);

ESD_FUNCTION(Esd_GetFontXOffset, Type = uint16_t, Attributes = ESD_CORE_EXPORT, DisplayName = "Get Font X Offset", Category = EsdUtilities)
ESD_PARAMETER(fontInfo, Type = Esd_FontInfo *)
ESD_CORE_EXPORT uint16_t Esd_GetFontXOffset(Esd_FontInfo *fontInfo);

/* end of supported functions */

#ifdef __cplusplus
}
#endif

#endif /* #ifndef ESD_BITMAPHANDLE__H */

/* end of file */
