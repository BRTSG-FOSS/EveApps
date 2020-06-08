/*
ESD Framework
Copyright (C) 2018  Bridgetek Pte Lte
Author: Jan Boon <jan.boon@kaetemi.be>
*/

#include "ESD_FontInfo.h"

#ifndef NDEBUG
#define ESD_FONTINFO_DEBUG
#endif

#ifdef ESD_FONTINFO_DEBUG
#define esd_resourceinfo_printf(fmt, ...) eve_printf(fmt, ##__VA_ARGS__)
#else
#define esd_resourceinfo_printf(fmt, ...) eve_noop()
#endif

extern ESD_CORE_EXPORT EVE_HalContext *ESD_Host;
extern ESD_CORE_EXPORT ESD_GpuAlloc *ESD_GAlloc;

uint32_t Esd_LoadFont(Esd_FontInfo *fontInfo)
{
	uint32_t glyphAddr;

	if (fontInfo->Type != ESD_FONT_LEGACY && fontInfo->Type != ESD_FONT_EXTENDED)
	{
		return GA_INVALID;
	}

	// Load glyphs
	glyphAddr = Esd_LoadResource(&fontInfo->GlyphResource, NULL);
	if (glyphAddr != GA_INVALID)
	{
		// Load map
		uint32_t fontAddr;
		bool rewriteAddr;

#ifdef EVE_FLASH_AVAILABLE
		if (fontInfo->FontResource.Type == ESD_RESOURCE_DIRECTFLASH)
			fontInfo->FontResource.Type = ESD_RESOURCE_FLASH;
#endif
		fontAddr = ESD_GpuAlloc_Get(ESD_GAlloc, fontInfo->FontResource.GpuHandle);
		if (fontAddr == GA_INVALID)
		{
			fontAddr = Esd_LoadResource(&fontInfo->FontResource, NULL);
			rewriteAddr = true;
		}
		else
		{
			rewriteAddr = (glyphAddr != fontInfo->GlyphAddress);
		}

		if (fontAddr == GA_INVALID)
		{
			// Failed to load font block, unload glyphs
			esd_resourceinfo_printf("Failed to load font block, free glyphs\n");
			Esd_FreeResource(&fontInfo->GlyphResource);
			return GA_INVALID;
		}

		if (rewriteAddr)
		{
			// Rewrite glyph address in the font block
			uint32_t format;
			(void)format;
			switch (fontInfo->Type)
			{
			case ESD_FONT_LEGACY:
			{
				format = EVE_Hal_rd32(ESD_Host, fontAddr + 128);
				fontInfo->FontHeight = EVE_Hal_rd32(ESD_Host, fontAddr + 140);
				esd_resourceinfo_printf("Set legacy glyph address to %i\n", (int)glyphAddr);
				EVE_Hal_wr32(ESD_Host, fontAddr + 144, glyphAddr);
				fontInfo->BitmapHandle = ESD_BITMAPHANDLE_INVALID;
				break;
			}
			case ESD_FONT_EXTENDED:
			{
				format = EVE_Hal_rd32(ESD_Host, fontAddr + 8);
				fontInfo->FontHeight = EVE_Hal_rd32(ESD_Host, fontAddr + 28);
				esd_resourceinfo_printf("Set extended glyph address to %i\n", (int)glyphAddr);
				EVE_Hal_wr32(ESD_Host, fontAddr + 32, glyphAddr);
				fontInfo->BitmapHandle = ESD_BITMAPHANDLE_INVALID;
				break;
			}
			}
			if (!fontInfo->BaseLine)
				fontInfo->BaseLine = fontInfo->FontHeight;
			if (!fontInfo->CapsHeight)
				fontInfo->CapsHeight = fontInfo->FontHeight;
			fontInfo->GlyphAddress = glyphAddr;

#ifdef EVE_FLASH_AVAILABLE
			// Special case for user convenience, handling user error
			if (fontInfo->GlyphResource.Type == ESD_RESOURCE_DIRECTFLASH && !ESD_IS_FORMAT_ASTC(format))
			{
				// In case the glyph resource was set to direct flash but is not in ASTC format, reload it
				esd_resourceinfo_printf("Glyph resource set to Direct Flash but is not in ASTC format, reload\n");
				fontInfo->GlyphResource.Type = ESD_RESOURCE_FLASH;
				glyphAddr = Esd_LoadResource(&fontInfo->GlyphResource, NULL);
				if (glyphAddr == GA_INVALID)
				{
					// Failed to reload glyph, free font block resource
					esd_resourceinfo_printf("Failed to reload glyph, free font block resource\n");
					Esd_FreeResource(&fontInfo->FontResource);
					return GA_INVALID;
				}
			}
#endif
		}

		return fontAddr;
	}

	return GA_INVALID;
}

void Esd_FontPersist(Esd_FontInfo *fontInfo)
{
	Esd_LoadFont(fontInfo);
}

/* end of file */
