/*
ESD Framework
Copyright (C) 2018  Bridgetek Pte Lte
Author: Jan Boon <jan.boon@kaetemi.be>
*/

#include "ESD_BitmapHandle.h"
#include "ESD_Context.h"

// Number of bitmap handles to use (and also the scratch handle)
// List which bitmap info is using a handle
// void *ESD_BitmapHandleInfo[ESD_BITMAPHANDLE_NB] = { 0 };
// ESD_GpuHandle ESD_BitmapHandleGpuHandle[ESD_BITMAPHANDLE_NB] = { 0 };
// uint8_t ESD_BitmapHandleUse[ESD_BITMAPHANDLE_NB] = { 0 };
// uint8_t ESD_ScratchHandle = 15;
#define ESD_SCRATCHHANDLE ESD_CO_SCRATCH_HANDLE

// bool ESD_BitmapHandleResized[ESD_BITMAPHANDLE_NB] = { 0 };
// uint8_t ESD_BitmapHandlePage[ESD_BITMAPHANDLE_NB] = { 0 };

// #define ESD_FONTHANDLE_NB 16UL // First font handle
// #define ESD_ROMFONT_NB 35UL
// uint8_t ESD_RomFontHandles[ESD_ROMFONT_NB - ESD_FONTHANDLE_NB] = { 0 };

#if (EVE_SUPPORT_CHIPID >= EVE_FT810)
#define ESD_ROMFONT_CAP 35UL // Max, rom font handle, exclusive
#else
#define ESD_ROMFONT_CAP 32UL // Max, rom font handle, exclusive
#endif
#define ESD_ROMFONT_MAX (phost ? ((EVE_CHIPID >= EVE_FT810) ? 35UL : 32UL) : ESD_ROMFONT_CAP)
#define ESD_ROMFONT_MIN 16UL // Min, rom font handle, inclusive
#define ESD_ROMFONT_NB (ESD_ROMFONT_CAP - ESD_ROMFONT_MIN)
Esd_RomFontInfo Esd_RomFonts[ESD_ROMFONT_NB] = {
	{ .Type = ESD_FONT_ROM, .RomFont = 16UL },
	{ .Type = ESD_FONT_ROM, .RomFont = 17UL },
	{ .Type = ESD_FONT_ROM, .RomFont = 18UL },
	{ .Type = ESD_FONT_ROM, .RomFont = 19UL },
	{ .Type = ESD_FONT_ROM, .RomFont = 20UL },
	{ .Type = ESD_FONT_ROM, .RomFont = 21UL },
	{ .Type = ESD_FONT_ROM, .RomFont = 22UL },
	{ .Type = ESD_FONT_ROM, .RomFont = 23UL },
	{ .Type = ESD_FONT_ROM, .RomFont = 24UL },
	{ .Type = ESD_FONT_ROM, .RomFont = 25UL },
	{ .Type = ESD_FONT_ROM, .RomFont = 26UL },
	{ .Type = ESD_FONT_ROM, .RomFont = 27UL },
	{ .Type = ESD_FONT_ROM, .RomFont = 28UL },
	{ .Type = ESD_FONT_ROM, .RomFont = 29UL },
	{ .Type = ESD_FONT_ROM, .RomFont = 30UL },
	{ .Type = ESD_FONT_ROM, .RomFont = 31UL },
#if (EVE_SUPPORT_CHIPID >= EVE_FT810)
	{ .Type = ESD_FONT_ROM, .RomFont = 32UL },
	{ .Type = ESD_FONT_ROM, .RomFont = 33UL },
	{ .Type = ESD_FONT_ROM, .RomFont = 34UL },
#endif
};

void Esd_InitRomFontHeight()
{
	int i;
	EVE_HalContext *phost = ESD_Host;
	uint32_t ft = EVE_Hal_rd32(ESD_Host, ROMFONT_TABLEADDRESS);
	(void)phost;

	for (i = 0; i < ESD_ROMFONT_NB; ++i)
	{
		uint32_t addr = ft + (EVE_GPU_FONT_TABLE_SIZE * i) + (uint32_t)(uintptr_t)(&(((EVE_Gpu_Fonts *)(void *)0)->FontHeightInPixels));
		Esd_RomFonts[i].FontHeight = EVE_Hal_rd16(ESD_Host, addr);
		Esd_RomFonts[i].BaseLine = Esd_RomFonts[i].FontHeight;
		Esd_RomFonts[i].CapsHeight = Esd_RomFonts[i].FontHeight;
	}
}

uint16_t ESD_getFontHeight(Esd_FontInfo *fontInfo)
{
	if (fontInfo)
		return fontInfo->FontHeight;
	return 0;
}

uint16_t ESD_getFontBaseLine(Esd_FontInfo *fontInfo)
{
	if (fontInfo)
		return fontInfo->BaseLine;
	return 0;
}

uint16_t ESD_getFontCapsHeight(Esd_FontInfo *fontInfo)
{
	if (fontInfo)
		return fontInfo->CapsHeight;
	return 0;
}

void ESD_BitmapHandle_initialize()
{
	// memset(ESD_BitmapHandleGpuHandle, 0, sizeof(ESD_BitmapHandleGpuHandle));
	Esd_InitRomFontHeight();
}

void ESD_BitmapHandle_frameStart(ESD_HandleState *handleState)
{
	EVE_HalContext *phost = ESD_Host;
	(void)phost;
	for (uint32_t i = 0; i < ESD_BITMAPHANDLE_NB; ++i)
	{
		// 2: In use last frame
		// 1: Not in use anymore
		// 0: Not in use
		if (handleState->Use[i] < 2)
			handleState->Use[i] = 0;
		else
			--handleState->Use[i];
		// 2: In use this frame
		// 1: In use last frame
		// 0: Not in use
	}
}

/// Reset the bitmap handle state
void ESD_BitmapHandle_reset(ESD_HandleState *state)
{
	memset(state, 0, sizeof(ESD_HandleState));
}

uint32_t ESD_BitmapHandle_getTotalUsed()
{
	EVE_HalContext *phost = ESD_Host;
	uint32_t total = 0;
	(void)phost;
	for (uint32_t i = 0; i < ESD_BITMAPHANDLE_NB; ++i)
	{
		if ((i != ESD_SCRATCHHANDLE) && (ESD_CurrentContext->HandleState.Use[i] > 0))
		{
			++total;
		}
	}
	return total;
}

uint32_t ESD_BitmapHandle_getTotal()
{
	EVE_HalContext *phost = ESD_Host;
	(void)phost;
	return ESD_BITMAPHANDLE_NB - 1; // NB minus one used for scratch
}

Esd_FontInfo *ESD_getRomFont(uint8_t font)
{
	EVE_HalContext *phost = ESD_Host;
	(void)phost;

	if (font >= ESD_ROMFONT_MIN && font < ESD_ROMFONT_MAX)
		return (Esd_FontInfo *)(void *)&Esd_RomFonts[font - ESD_ROMFONT_MIN];
	return NULL;
}

static eve_progmem_const uint8_t c_AstcBlockHeight[] = {
	4, 4, 5, 5, 6, 5, 6, 8, 5, 6, 8, 10, 10, 12, 0xFF, 0xFF
};

void ESD_CoDl_pagedBitmapSource(uint8_t handle, uint8_t page)
{
	EVE_HalContext *phost = ESD_Host;
	if (ESD_BITMAPHANDLE_VALID(handle) && ESD_CurrentContext->HandleState.Page[handle] != page)
	{
		ESD_BitmapInfo *info = ESD_CurrentContext->HandleState.Info[handle];
		uint32_t addr = ESD_GpuAlloc_Get(ESD_GAlloc, ESD_CurrentContext->HandleState.GpuHandle[handle]);
		EVE_CoDl_bitmapHandle(phost, handle);
		uint32_t pageOffset = ((((uint32_t)page) << 7) * info->Stride * info->Height);
		if (EVE_CHIPID >= EVE_BT815 && ESD_IS_FORMAT_ASTC(info->Format))
			pageOffset /= c_AstcBlockHeight[info->Format & 0xF]; // Stride under ASTC is by block row
		uint32_t pageAddr = addr + pageOffset;
		EVE_CoCmd_dl(phost, BITMAP_SOURCE(pageAddr));
		ESD_CurrentContext->HandleState.Page[handle] = page;
	}
}

void ESD_CoDl_pagedCell(uint8_t handle, uint16_t cell)
{
	EVE_HalContext *phost = ESD_Host;
	EVE_CoDl_bitmapHandle(phost, handle);
	ESD_CoDl_pagedBitmapSource(handle, cell >> 7);
	EVE_CoDl_cell(phost, cell & 0x7F);
}

uint8_t ESD_CoDl_setupBitmap(ESD_BitmapInfo *bitmapInfo)
{
	// Get bitmap address
	EVE_HalContext *phost = ESD_Host;
	uint32_t addr = ESD_loadBitmap(bitmapInfo);

	if (addr == GA_INVALID)
		return ESD_BITMAPHANDLE_INVALID; // Bitmap not loaded (out of memory or file not found)

	uint32_t handle = bitmapInfo->BitmapHandle;
	if (!(ESD_BITMAPHANDLE_VALID(handle)
	        && (handle != ESD_SCRATCHHANDLE)
	        && (ESD_CurrentContext->HandleState.Info[handle] == bitmapInfo)
	        && (ESD_CurrentContext->HandleState.GpuHandle[handle].Id == bitmapInfo->GpuHandle.Id)
	        && (ESD_CurrentContext->HandleState.GpuHandle[handle].Seq == bitmapInfo->GpuHandle.Seq)))
	{
		// Bitmap is loaded but no handle is setup, create a new handle for this bitmap
		// eve_printf_debug("Find free bitmap handle for addr %i\n", (int)addr);

		if (ESD_CurrentContext->LoopState != ESD_LOOPSTATE_RENDER)
		{
			eve_printf_debug("Warning: Can only setup bitmap during render pass\n");
			return ESD_BITMAPHANDLE_INVALID;
		}

		// Find a free handle
		handle = ESD_SCRATCHHANDLE; // Fallback to scratch handle
		for (uint32_t i = 0; i < ESD_BITMAPHANDLE_NB; ++i)
		{
			if ((i != ESD_SCRATCHHANDLE) && (!ESD_CurrentContext->HandleState.Use[i]))
			{
				// Attach this handle to the bitmap info
				handle = i;
				ESD_CurrentContext->HandleState.Info[i] = bitmapInfo;
				ESD_CurrentContext->HandleState.GpuHandle[i] = bitmapInfo->GpuHandle;
				break;
			}
		}

		eve_printf_debug("Use handle %i, addr %i, gpu alloc %i, %i, file %s\n",
		    (int)handle, (int)addr, (int)bitmapInfo->GpuHandle.Id, (int)bitmapInfo->GpuHandle.Seq,
		    (!bitmapInfo->Flash && bitmapInfo->File) ? bitmapInfo->File : "<no file>");

		bitmapInfo->BitmapHandle = handle;

		// Setup the handle
		EVE_CoDl_bitmapHandle(phost, handle);
		uint32_t format = bitmapInfo->Format;
		if (format == DXT1)
			format = L1;
		else if (format == JPEG)
			format = RGB565; // TODO: Support for grayscale
		else if (format == PNG)
			format = RGB565; // TODO: Support for other PNG formats

		if (EVE_CHIPID >= EVE_FT810)
			EVE_CoCmd_setBitmap(phost, addr, format, bitmapInfo->Width, bitmapInfo->Height); // TODO: What with stride?
		else
			eve_assert_ex(false, "No support yet in ESD for bitmaps for FT800 target");

#if (EVE_SUPPORT_CHIPID >= EVE_BT815)
		if (EVE_CHIPID >= EVE_BT815)
		{
			// Important. Bitmap swizzle not reset by SETBITMAP
			if (bitmapInfo->Swizzle)
				EVE_CoCmd_dl(phost, BITMAP_SWIZZLE(bitmapInfo->SwizzleR, bitmapInfo->SwizzleG, bitmapInfo->SwizzleB, bitmapInfo->SwizzleA));
			else
				EVE_CoCmd_dl(phost, BITMAP_SWIZZLE(RED, GREEN, BLUE, ALPHA));
		}
#endif

		ESD_CurrentContext->HandleState.Resized[handle] = 0;
		ESD_CurrentContext->HandleState.Page[handle] = 0;
	}

	// TEMPORARY WORKAROUND: SetBitmap not correctly being applied some frames... Need to check!
	// EVE_CoDl_bitmapHandle(handle);
	// uint32_t format = bitmapInfo->Format;
	// if (format == DXT1) format = L1;
	// else if (format == JPEG) format = RGB565; // TODO: Support for grayscale
	// else if (format == PNG) format = RGB565; // TODO: Support for other PNG formats
	// EVE_CoCmd_setBitmap(ESD_Host, addr, format, bitmapInfo->Width, bitmapInfo->Height); // TODO: What with stride?
	// ESD_BitmapHandleResized[handle] = 0;

	if (ESD_BITMAPHANDLE_VALID(handle) && (handle != ESD_SCRATCHHANDLE)) // When valid and not using scratch handle
	{
		ESD_CurrentContext->HandleState.Use[handle] = 2; // In use
	}

	if (EVE_CHIPID >= EVE_FT810)
	{
		// Use palette if available
		uint32_t paletteAddr = ESD_loadPalette(bitmapInfo);
		if (paletteAddr != GA_INVALID && bitmapInfo->Format != PALETTED8) // PALETTED8 uses custom palette setup
		{
			EVE_CoDl_paletteSource(phost, paletteAddr);
		}
	}

	return handle;
}

uint8_t ESD_CoDl_setupRomFont(uint8_t font)
{
	return ESD_CoDl_setupFont(ESD_getRomFont(font));
}

uint8_t ESD_CoDl_setupFont(Esd_FontInfo *fontInfo)
{
	EVE_HalContext *phost = ESD_Host;
	uint32_t handle = fontInfo->BitmapHandle;
	if (fontInfo->Type == ESD_FONT_ROM)
	{
		// Get rom font
		Esd_RomFontInfo *romFontInfo = (Esd_RomFontInfo *)(void *)fontInfo;
		uint8_t font = romFontInfo->RomFont;
		if (EVE_CHIPID >= EVE_FT810)
		{
			if (!(font >= ESD_ROMFONT_MIN && font < ESD_ROMFONT_MAX))
			{
				return ESD_BITMAPHANDLE_INVALID; // Not a valid rom font
			}

			if (!ESD_BITMAPHANDLE_VALID(handle)
			    || (handle == ESD_SCRATCHHANDLE)
			    || (ESD_CurrentContext->HandleState.Info[handle] != romFontInfo)
			    || (ESD_CurrentContext->HandleState.GpuHandle[handle].Id != MAX_NUM_ALLOCATIONS)
			    || (ESD_CurrentContext->HandleState.GpuHandle[handle].Seq != font))
			{
				// The handle is no longer valid, make a new one

				if (ESD_CurrentContext->LoopState != ESD_LOOPSTATE_RENDER)
				{
					eve_printf_debug("Warning: Can only setup rom font during render pass\n");
					return ESD_BITMAPHANDLE_INVALID;
				}

				// Find a free handle
				handle = ESD_SCRATCHHANDLE; // Fallback to scratch handle
				for (uint32_t i = 0; i < ESD_BITMAPHANDLE_NB; ++i)
				{
					if ((i != ESD_SCRATCHHANDLE) && (!ESD_CurrentContext->HandleState.Use[i]))
					{
						// Attach this handle to the bitmap info
						handle = i;
						ESD_CurrentContext->HandleState.Info[i] = romFontInfo;
						ESD_CurrentContext->HandleState.GpuHandle[i].Id = MAX_NUM_ALLOCATIONS;
						ESD_CurrentContext->HandleState.GpuHandle[i].Seq = font;
						break;
					}
				}

				eve_printf_debug("Use handle %i, font %i\n",
				    (int)handle, (int)font);

				// Set the font
				romFontInfo->BitmapHandle = handle;
				EVE_CoCmd_romFont(ESD_Host, handle, font);
#if ESD_DL_OPTIMIZE
				ESD_STATE.Handle = handle;
#endif
				ESD_CurrentContext->HandleState.Resized[handle] = 0;
				ESD_CurrentContext->HandleState.Page[handle] = 0;
			}
		}
		else
		{
			romFontInfo->BitmapHandle = font;
			return font;
		}
	}
	else
	{
		// Get font address
		uint32_t addr = Esd_LoadFont(fontInfo);
		if (addr == GA_INVALID)
			return ESD_BITMAPHANDLE_INVALID; // Font not loaded (out of memory or file not found)

		if (!ESD_BITMAPHANDLE_VALID(handle)
		    || (handle == ESD_SCRATCHHANDLE)
		    || (ESD_CurrentContext->HandleState.Info[handle] != fontInfo)
		    || (ESD_CurrentContext->HandleState.GpuHandle[handle].Id != fontInfo->FontResource.GpuHandle.Id)
		    || (ESD_CurrentContext->HandleState.GpuHandle[handle].Seq != fontInfo->FontResource.GpuHandle.Seq))
		{
			// The handle is no longer valid, make a new one

			if (ESD_CurrentContext->LoopState != ESD_LOOPSTATE_RENDER)
			{
				eve_printf_debug("Warning: Can only setup font during render pass\n");
				return ESD_BITMAPHANDLE_INVALID;
			}

			// Find a free handle
			handle = ESD_SCRATCHHANDLE; // Fallback to scratch handle
			for (uint32_t i = 0; i < ESD_BITMAPHANDLE_NB; ++i)
			{
				if ((i != ESD_SCRATCHHANDLE) && (!ESD_CurrentContext->HandleState.Use[i]))
				{
					// Attach this handle to the font info
					handle = i;
					ESD_CurrentContext->HandleState.Info[i] = fontInfo;
					ESD_CurrentContext->HandleState.GpuHandle[i] = fontInfo->FontResource.GpuHandle;
					break;
				}
			}

			eve_printf_debug("Use handle %i, addr %i, %i, gpu alloc %i, %i, %i, %i, file %s, %s, flash %i, %i\n",
			    (int)handle, (int)addr, (int)ESD_GpuAlloc_Get(ESD_GAlloc, fontInfo->GlyphResource.GpuHandle),
			    (int)fontInfo->FontResource.GpuHandle.Id, (int)fontInfo->FontResource.GpuHandle.Seq,
			    (int)fontInfo->GlyphResource.GpuHandle.Id, (int)fontInfo->GlyphResource.GpuHandle.Seq,
			    (fontInfo->FontResource.Type == ESD_RESOURCE_FILE) ? fontInfo->FontResource.File : "<no file>",
			    (fontInfo->GlyphResource.Type == ESD_RESOURCE_FILE) ? fontInfo->GlyphResource.File : "<no file>",
			    (ESD_RESOURCE_IS_FLASH(fontInfo->FontResource.Type) ? (int)fontInfo->FontResource.FlashAddress : 0),
			    (ESD_RESOURCE_IS_FLASH(fontInfo->GlyphResource.Type) ? (int)fontInfo->GlyphResource.FlashAddress : 0));

			// Set the font
			fontInfo->BitmapHandle = handle;
			if (EVE_CHIPID >= EVE_FT810)
				EVE_CoCmd_setFont2(ESD_Host, handle, addr, fontInfo->FirstChar);
			else
				eve_assert_ex(false, "No support yet in ESD for custom fonts");
#if ESD_DL_OPTIMIZE
			ESD_STATE.Handle = handle;
#endif
			ESD_CurrentContext->HandleState.Resized[handle] = 0;
			ESD_CurrentContext->HandleState.Page[handle] = 0;
		}
	}

	if (ESD_BITMAPHANDLE_VALID(handle) && (handle != ESD_SCRATCHHANDLE)) // When valid and not using scratch handle
	{
		ESD_CurrentContext->HandleState.Use[handle] = 2; // In use
	}

	return handle;
}

void ESD_CoDl_bitmapWidthHeight(uint8_t handle, uint16_t width, uint16_t height)
{
	EVE_HalContext *phost = ESD_Host;
	EVE_CoDl_bitmapHandle(phost, handle);
	EVE_CoDl_bitmapSize(phost, NEAREST, BORDER, BORDER, width, height);
	ESD_CurrentContext->HandleState.Resized[handle] = true;
}

void ESD_CoDl_bitmapSize(uint8_t handle, uint8_t filter, uint8_t wrapx, uint8_t wrapy, uint16_t width, uint16_t height)
{
	EVE_HalContext *phost = ESD_Host;
	EVE_CoDl_bitmapHandle(phost, handle);
	EVE_CoDl_bitmapSize(phost, filter, wrapx, wrapy, width, height);
	ESD_CurrentContext->HandleState.Resized[handle] = true;
}

void ESD_CoDl_bitmapSizeReset(uint8_t handle)
{
	if (ESD_CurrentContext->HandleState.Resized[handle])
	{
		ESD_BitmapInfo *bitmapInfo = (ESD_BitmapInfo *)ESD_CurrentContext->HandleState.Info[handle];
		ESD_CoDl_bitmapWidthHeight(handle, bitmapInfo->Width, bitmapInfo->Height);
		ESD_CurrentContext->HandleState.Resized[handle] = false;
	}
}

/* end of file */
