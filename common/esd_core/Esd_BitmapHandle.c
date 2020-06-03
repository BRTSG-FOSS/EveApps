/*
ESD Framework
Copyright (C) 2018  Bridgetek Pte Lte
Author: Jan Boon <jan.boon@kaetemi.be>
*/

#include "Ft_Esd_BitmapHandle.h"
#include "Ft_Esd_Core.h"

// Number of bitmap handles to use (and also the scratch handle)
// List which bitmap info is using a handle
// void *Ft_Esd_BitmapHandleInfo[FT_ESD_BITMAPHANDLE_NB] = { 0 };
// Ft_Esd_GpuHandle Ft_Esd_BitmapHandleGpuHandle[FT_ESD_BITMAPHANDLE_NB] = { 0 };
// ft_uint8_t Ft_Esd_BitmapHandleUse[FT_ESD_BITMAPHANDLE_NB] = { 0 };
// ft_uint8_t Ft_Esd_ScratchHandle = 15;
#define FT_ESD_SCRATCHHANDLE ESD_CO_SCRATCH_HANDLE

// ft_bool_t Ft_Esd_BitmapHandleResized[FT_ESD_BITMAPHANDLE_NB] = { 0 };
// ft_uint8_t Ft_Esd_BitmapHandlePage[FT_ESD_BITMAPHANDLE_NB] = { 0 };

// #define FT_ESD_FONTHANDLE_NB 16UL // First font handle
// #define FT_ESD_ROMFONT_NB 35UL
// ft_uint8_t Ft_Esd_RomFontHandles[FT_ESD_ROMFONT_NB - FT_ESD_FONTHANDLE_NB] = { 0 };

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
	EVE_HalContext *phost = Ft_Esd_Host;
	uint32_t ft = Ft_Gpu_Hal_Rd32(Ft_Esd_Host, ROMFONT_TABLEADDRESS);
	(void)phost;

	for (i = 0; i < ESD_ROMFONT_NB; ++i)
	{
		uint32_t addr = ft + (FT_GPU_FONT_TABLE_SIZE * i) + (uint32_t)(uintptr_t)(&(((FT_Gpu_Fonts_t *)(void *)0)->FontHeightInPixels));
		Esd_RomFonts[i].FontHeight = Ft_Gpu_Hal_Rd16(Ft_Esd_Host, addr);
		Esd_RomFonts[i].BaseLine = Esd_RomFonts[i].FontHeight;
		Esd_RomFonts[i].CapsHeight = Esd_RomFonts[i].FontHeight;
	}
}

uint16_t Esd_GetFontHeight(Esd_FontInfo *fontInfo)
{
	if (fontInfo)
		return fontInfo->FontHeight;
	return 0;
}

uint16_t Esd_GetFontBaseLine(Esd_FontInfo *fontInfo)
{
	if (fontInfo)
		return fontInfo->BaseLine;
	return 0;
}

uint16_t Esd_GetFontCapsHeight(Esd_FontInfo *fontInfo)
{
	if (fontInfo)
		return fontInfo->CapsHeight;
	return 0;
}

void Esd_BitmapHandle_Initialize()
{
	// memset(Ft_Esd_BitmapHandleGpuHandle, 0, sizeof(Ft_Esd_BitmapHandleGpuHandle));
	Esd_InitRomFontHeight();
}

void Esd_BitmapHandle_FrameStart(Esd_HandleState *handleState)
{
	EVE_HalContext *phost = Ft_Esd_Host;
	(void)phost;
	for (uint32_t i = 0; i < FT_ESD_BITMAPHANDLE_NB; ++i)
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
void Esd_BitmapHandle_Reset(Esd_HandleState *state)
{
	memset(state, 0, sizeof(Esd_HandleState));
}

uint32_t Ft_Esd_BitmapHandle_GetTotalUsed()
{
	EVE_HalContext *phost = Ft_Esd_Host;
	uint32_t total = 0;
	(void)phost;
	for (uint32_t i = 0; i < FT_ESD_BITMAPHANDLE_NB; ++i)
	{
		if ((i != FT_ESD_SCRATCHHANDLE) && (Esd_CurrentContext->HandleState.Use[i] > 0))
		{
			++total;
		}
	}
	return total;
}

uint32_t Ft_Esd_BitmapHandle_GetTotal()
{
	EVE_HalContext *phost = Ft_Esd_Host;
	(void)phost;
	return FT_ESD_BITMAPHANDLE_NB - 1; // NB minus one used for scratch
}

Esd_FontInfo *Esd_GetRomFont(uint8_t font)
{
	EVE_HalContext *phost = Ft_Esd_Host;
	if (font >= ESD_ROMFONT_MIN && font < ESD_ROMFONT_MAX)
		return (Esd_FontInfo *)(void *)&Esd_RomFonts[font - ESD_ROMFONT_MIN];
	return NULL;
}

static uint8_t FT_PROGMEM_CONST c_AstcBlockHeight[] = {
	4, 4, 5, 5, 6, 5, 6, 8, 5, 6, 8, 10, 10, 12
};

void Ft_Esd_Dl_Bitmap_Page(uint8_t handle, uint8_t page)
{
	EVE_HalContext *phost = Ft_Esd_Host;
	if (FT_ESD_BITMAPHANDLE_VALID(handle) && Esd_CurrentContext->HandleState.Page[handle] != page)
	{
		Ft_Esd_BitmapInfo *info = Esd_CurrentContext->HandleState.Info[handle];
		uint32_t addr = Ft_Esd_GpuAlloc_Get(Ft_Esd_GAlloc, Esd_CurrentContext->HandleState.GpuHandle[handle]);
		Ft_Esd_Dl_BITMAP_HANDLE(handle);
		uint32_t pageOffset = ((((uint32_t)page) << 7) * info->Stride * info->Height);
		if (EVE_CHIPID >= EVE_BT815 && ESD_IS_FORMAT_ASTC(info->Format))
			pageOffset /= c_AstcBlockHeight[info->Format & 0xF]; // Stride under ASTC is by block row
		uint32_t pageAddr = addr + pageOffset;
		Ft_Gpu_CoCmd_SendCmd(phost, BITMAP_SOURCE(pageAddr));
		Esd_CurrentContext->HandleState.Page[handle] = page;
	}
}

void Ft_Esd_Dl_CELL_Paged(uint8_t handle, uint16_t cell)
{
	Ft_Esd_Dl_BITMAP_HANDLE(handle);
	Ft_Esd_Dl_Bitmap_Page(handle, cell >> 7);
	Ft_Esd_Dl_CELL(cell & 0x7F);
}

uint8_t Ft_Esd_Dl_Bitmap_Setup(Ft_Esd_BitmapInfo *bitmapInfo)
{
	// Get bitmap address
	EVE_HalContext *phost = Ft_Esd_Host;
	uint32_t addr = Ft_Esd_LoadBitmap(bitmapInfo);
	(void)phost;
	if (addr == GA_INVALID)
		return FT_ESD_BITMAPHANDLE_INVALID; // Bitmap not loaded (out of memory or file not found)

	uint32_t handle = bitmapInfo->BitmapHandle;
	if (!(FT_ESD_BITMAPHANDLE_VALID(handle)
	        && (handle != FT_ESD_SCRATCHHANDLE)
	        && (Esd_CurrentContext->HandleState.Info[handle] == bitmapInfo)
	        && (Esd_CurrentContext->HandleState.GpuHandle[handle].Id == bitmapInfo->GpuHandle.Id)
	        && (Esd_CurrentContext->HandleState.GpuHandle[handle].Seq == bitmapInfo->GpuHandle.Seq)))
	{
		// Bitmap is loaded but no handle is setup, create a new handle for this bitmap
		// eve_printf_debug("Find free bitmap handle for addr %i\n", (int)addr);

		if (Esd_CurrentContext->LoopState != ESD_LOOPSTATE_RENDER)
		{
			eve_printf_debug("Warning: Can only setup bitmap during render pass\n");
			return FT_ESD_BITMAPHANDLE_INVALID;
		}

		// Find a free handle
		handle = FT_ESD_SCRATCHHANDLE; // Fallback to scratch handle
		for (uint32_t i = 0; i < FT_ESD_BITMAPHANDLE_NB; ++i)
		{
			if ((i != FT_ESD_SCRATCHHANDLE) && (!Esd_CurrentContext->HandleState.Use[i]))
			{
				// Attach this handle to the bitmap info
				handle = i;
				Esd_CurrentContext->HandleState.Info[i] = bitmapInfo;
				Esd_CurrentContext->HandleState.GpuHandle[i] = bitmapInfo->GpuHandle;
				break;
			}
		}

		eve_printf_debug("Use handle %i, addr %i, gpu alloc %i, %i, file %s\n",
		    (int)handle, (int)addr, (int)bitmapInfo->GpuHandle.Id, (int)bitmapInfo->GpuHandle.Seq,
		    (!bitmapInfo->Flash && bitmapInfo->File) ? bitmapInfo->File : "<no file>");

		bitmapInfo->BitmapHandle = handle;

		// Setup the handle
		Ft_Esd_Dl_BITMAP_HANDLE(handle);
		uint32_t format = bitmapInfo->Format;
		if (format == DXT1)
			format = L1;
		else if (format == JPEG)
			format = RGB565; // TODO: Support for grayscale
		else if (format == PNG)
			format = RGB565; // TODO: Support for other PNG formats

		if (EVE_CHIPID >= EVE_FT810)
			Ft_Gpu_CoCmd_SetBitmap(Ft_Esd_Host, addr, format, bitmapInfo->Width, bitmapInfo->Height); // TODO: What with stride?
		else
			eve_assert_ex(false, "No support yet in ESD for bitmaps for FT800 target");

#if (EVE_SUPPORT_CHIPID >= EVE_BT815)
		if (EVE_CHIPID >= EVE_BT815)
		{
			// Important. Bitmap swizzle not reset by SETBITMAP
			if (bitmapInfo->Swizzle)
				Ft_Gpu_CoCmd_SendCmd(Ft_Esd_Host, BITMAP_SWIZZLE(bitmapInfo->SwizzleR, bitmapInfo->SwizzleG, bitmapInfo->SwizzleB, bitmapInfo->SwizzleA));
			else
				Ft_Gpu_CoCmd_SendCmd(Ft_Esd_Host, BITMAP_SWIZZLE(RED, GREEN, BLUE, ALPHA));
		}
#endif

		Esd_CurrentContext->HandleState.Resized[handle] = 0;
		Esd_CurrentContext->HandleState.Page[handle] = 0;
	}

	// TEMPORARY WORKAROUND: SetBitmap not correctly being applied some frames... Need to check!
	// Ft_Esd_Dl_BITMAP_HANDLE(handle);
	// uint32_t format = bitmapInfo->Format;
	// if (format == DXT1) format = L1;
	// else if (format == JPEG) format = RGB565; // TODO: Support for grayscale
	// else if (format == PNG) format = RGB565; // TODO: Support for other PNG formats
	// Ft_Gpu_CoCmd_SetBitmap(Ft_Esd_Host, addr, format, bitmapInfo->Width, bitmapInfo->Height); // TODO: What with stride?
	// Ft_Esd_BitmapHandleResized[handle] = 0;

	if (FT_ESD_BITMAPHANDLE_VALID(handle) && (handle != FT_ESD_SCRATCHHANDLE)) // When valid and not using scratch handle
	{
		Esd_CurrentContext->HandleState.Use[handle] = 2; // In use
	}

	if (EVE_CHIPID >= EVE_FT810)
	{
		// Use palette if available
		uint32_t paletteAddr = Ft_Esd_LoadPalette(bitmapInfo);
		if (paletteAddr != GA_INVALID && bitmapInfo->Format != PALETTED8) // PALETTED8 uses custom palette setup
		{
			Ft_Esd_Dl_PALETTE_SOURCE(paletteAddr);
		}
	}

	return handle;
}

uint8_t Ft_Esd_Dl_RomFont_Setup(uint8_t font)
{
	return Ft_Esd_Dl_Font_Setup(Esd_GetRomFont(font));
}

uint8_t Ft_Esd_Dl_Font_Setup(Esd_FontInfo *fontInfo)
{
	EVE_HalContext *phost = Ft_Esd_Host;
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
				return FT_ESD_BITMAPHANDLE_INVALID; // Not a valid rom font
			}

			if (!FT_ESD_BITMAPHANDLE_VALID(handle)
			    || (handle == FT_ESD_SCRATCHHANDLE)
			    || (Esd_CurrentContext->HandleState.Info[handle] != romFontInfo)
			    || (Esd_CurrentContext->HandleState.GpuHandle[handle].Id != MAX_NUM_ALLOCATIONS)
			    || (Esd_CurrentContext->HandleState.GpuHandle[handle].Seq != font))
			{
				// The handle is no longer valid, make a new one

				if (Esd_CurrentContext->LoopState != ESD_LOOPSTATE_RENDER)
				{
					eve_printf_debug("Warning: Can only setup rom font during render pass\n");
					return FT_ESD_BITMAPHANDLE_INVALID;
				}

				// Find a free handle
				handle = FT_ESD_SCRATCHHANDLE; // Fallback to scratch handle
				for (uint32_t i = 0; i < FT_ESD_BITMAPHANDLE_NB; ++i)
				{
					if ((i != FT_ESD_SCRATCHHANDLE) && (!Esd_CurrentContext->HandleState.Use[i]))
					{
						// Attach this handle to the bitmap info
						handle = i;
						Esd_CurrentContext->HandleState.Info[i] = romFontInfo;
						Esd_CurrentContext->HandleState.GpuHandle[i].Id = MAX_NUM_ALLOCATIONS;
						Esd_CurrentContext->HandleState.GpuHandle[i].Seq = font;
						break;
					}
				}

				eve_printf_debug("Use handle %i, font %i\n",
				    (int)handle, (int)font);

				// Set the font
				romFontInfo->BitmapHandle = handle;
				Ft_Gpu_CoCmd_RomFont(Ft_Esd_Host, handle, font);
#if ESD_DL_OPTIMIZE
				FT_ESD_STATE.Handle = handle;
#endif
				Esd_CurrentContext->HandleState.Resized[handle] = 0;
				Esd_CurrentContext->HandleState.Page[handle] = 0;
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
			return FT_ESD_BITMAPHANDLE_INVALID; // Font not loaded (out of memory or file not found)

		if (!FT_ESD_BITMAPHANDLE_VALID(handle)
		    || (handle == FT_ESD_SCRATCHHANDLE)
		    || (Esd_CurrentContext->HandleState.Info[handle] != fontInfo)
		    || (Esd_CurrentContext->HandleState.GpuHandle[handle].Id != fontInfo->FontResource.GpuHandle.Id)
		    || (Esd_CurrentContext->HandleState.GpuHandle[handle].Seq != fontInfo->FontResource.GpuHandle.Seq))
		{
			// The handle is no longer valid, make a new one

			if (Esd_CurrentContext->LoopState != ESD_LOOPSTATE_RENDER)
			{
				eve_printf_debug("Warning: Can only setup font during render pass\n");
				return FT_ESD_BITMAPHANDLE_INVALID;
			}

			// Find a free handle
			handle = FT_ESD_SCRATCHHANDLE; // Fallback to scratch handle
			for (uint32_t i = 0; i < FT_ESD_BITMAPHANDLE_NB; ++i)
			{
				if ((i != FT_ESD_SCRATCHHANDLE) && (!Esd_CurrentContext->HandleState.Use[i]))
				{
					// Attach this handle to the font info
					handle = i;
					Esd_CurrentContext->HandleState.Info[i] = fontInfo;
					Esd_CurrentContext->HandleState.GpuHandle[i] = fontInfo->FontResource.GpuHandle;
					break;
				}
			}

			eve_printf_debug("Use handle %i, addr %i, %i, gpu alloc %i, %i, %i, %i, file %s, %s, flash %i, %i\n",
			    (int)handle, (int)addr, (int)Ft_Esd_GpuAlloc_Get(Ft_Esd_GAlloc, fontInfo->GlyphResource.GpuHandle),
			    (int)fontInfo->FontResource.GpuHandle.Id, (int)fontInfo->FontResource.GpuHandle.Seq,
			    (int)fontInfo->GlyphResource.GpuHandle.Id, (int)fontInfo->GlyphResource.GpuHandle.Seq,
			    (fontInfo->FontResource.Type == ESD_RESOURCE_FILE) ? fontInfo->FontResource.File : "<no file>",
			    (fontInfo->GlyphResource.Type == ESD_RESOURCE_FILE) ? fontInfo->GlyphResource.File : "<no file>",
			    (ESD_RESOURCE_IS_FLASH(fontInfo->FontResource.Type) ? (int)fontInfo->FontResource.FlashAddress : 0),
			    (ESD_RESOURCE_IS_FLASH(fontInfo->GlyphResource.Type) ? (int)fontInfo->GlyphResource.FlashAddress : 0));

			// Set the font
			fontInfo->BitmapHandle = handle;
			if (EVE_CHIPID >= EVE_FT810)
				Ft_Gpu_CoCmd_SetFont2(Ft_Esd_Host, handle, addr, fontInfo->FirstChar);
			else
				eve_assert_ex(false, "No support yet in ESD for custom fonts");
#if ESD_DL_OPTIMIZE
			FT_ESD_STATE.Handle = handle;
#endif
			Esd_CurrentContext->HandleState.Resized[handle] = 0;
			Esd_CurrentContext->HandleState.Page[handle] = 0;
		}
	}

	if (FT_ESD_BITMAPHANDLE_VALID(handle) && (handle != FT_ESD_SCRATCHHANDLE)) // When valid and not using scratch handle
	{
		Esd_CurrentContext->HandleState.Use[handle] = 2; // In use
	}

	return handle;
}

void Ft_Esd_Dl_Bitmap_WidthHeight(uint8_t handle, uint16_t width, uint16_t height)
{
	EVE_HalContext *phost = Ft_Esd_Host;
	Ft_Esd_Dl_BITMAP_HANDLE(handle);
	Ft_Gpu_CoCmd_SendCmd(phost, BITMAP_SIZE(NEAREST, BORDER, BORDER, width & 0x1ff, height & 0x1ff));
	if (EVE_CHIPID >= EVE_FT810)
		Ft_Gpu_CoCmd_SendCmd(phost, BITMAP_SIZE_H(width >> 9, height >> 9));
	Esd_CurrentContext->HandleState.Resized[handle] = 1;
}

void Ft_Esd_Dl_Bitmap_WidthHeight_BILINEAR(uint8_t handle, uint16_t width, uint16_t height)
{
	EVE_HalContext *phost = Ft_Esd_Host;
	Ft_Esd_Dl_BITMAP_HANDLE(handle);
	Ft_Gpu_CoCmd_SendCmd(phost, BITMAP_SIZE(BILINEAR, BORDER, BORDER, width & 0x1ff, height & 0x1ff));
	if (EVE_CHIPID >= EVE_FT810)
		Ft_Gpu_CoCmd_SendCmd(phost, BITMAP_SIZE_H(width >> 9, height >> 9));
	Esd_CurrentContext->HandleState.Resized[handle] = 1;
}

void Ft_Esd_Dl_Bitmap_WidthHeightReset(uint8_t handle)
{
	if (Esd_CurrentContext->HandleState.Resized[handle])
	{
		Ft_Esd_BitmapInfo *bitmapInfo = (Ft_Esd_BitmapInfo *)Esd_CurrentContext->HandleState.Info[handle];
		Ft_Esd_Dl_Bitmap_WidthHeight(handle, bitmapInfo->Width, bitmapInfo->Height);
		Esd_CurrentContext->HandleState.Resized[handle] = 0;
	}
}

/* end of file */
