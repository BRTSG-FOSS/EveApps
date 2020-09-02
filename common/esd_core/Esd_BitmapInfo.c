
#include "Esd_BitmapInfo.h"
#include "Esd_Context.h"

#include "Esd_GpuAlloc.h"

#ifndef NDEBUG
#define ESD_BITMAPINFO_DEBUG
#endif

extern ESD_CORE_EXPORT EVE_HalContext *Esd_Host;
extern ESD_CORE_EXPORT Esd_GpuAlloc *Esd_GAlloc;

#ifdef EVE_FLASH_AVAILABLE
#define ESD_BITMAPINFO_SUPPORT_DIRECT_FLASH(bitmapInfo) (bitmapInfo->Flash && ESD_IS_FORMAT_ASTC(bitmapInfo->Format))
#endif

static bool Esd_LoadFromFile(uint32_t *imageFormat, bool deflate, uint32_t dst, const char *file)
{
	return imageFormat
	    ? EVE_Util_loadImageFile(Esd_Host, dst, file, imageFormat)
	    : (deflate
	              ? EVE_Util_loadInflateFile(Esd_Host, dst, file)
	              : EVE_Util_loadRawFile(Esd_Host, dst, file));
}

#ifdef EVE_FLASH_AVAILABLE

static bool Esd_LoadFromFlash(uint32_t *imageFormat, bool deflate, uint32_t dst, uint32_t src, uint32_t size)
{
	return imageFormat
	    ? EVE_CoCmd_loadImage_flash(Esd_Host, dst, src, imageFormat)
	    : (deflate
	              ? EVE_CoCmd_inflate_flash(Esd_Host, dst, src)
	              : EVE_CoCmd_flashRead_flush(Esd_Host, dst, src, size));
}

#endif

#ifdef EVE_SUPPORT_VIDEO

static bool Esd_LoadVideoFrameFromFile(uint32_t *imageFormat, uint32_t dst, const char *file)
{
	EVE_HalContext *phost = Esd_GetHost();

	/* Allocate RAM_G space for FIFO and completion pointer */
	uint32_t fifoSize = 16 * 1024; /* TODO: What's an ideal FIFO size? */
	Esd_GpuHandle fifoHandle = Esd_GpuAlloc_Alloc(Esd_GAlloc, fifoSize + 4, 0);
	uint32_t fifoAddr = Esd_GpuAlloc_Get(Esd_GAlloc, fifoHandle);
	if (fifoAddr == GA_INVALID)
	{
		return false;
	}

	/* Setup media FIFO */
	if (!EVE_MediaFifo_set(phost, fifoAddr, fifoSize))
	{
		Esd_GpuAlloc_Free(Esd_GAlloc, fifoHandle);
		return false;
	}

	/* Load the first video frame */
	uint32_t transfered = 0;
	uint32_t ptr = fifoAddr + fifoSize;
	EVE_CoCmd_videoStart(phost);
	EVE_CoCmd_videoFrame(phost, dst, ptr);
	bool res = EVE_Util_loadMediaFile(phost, file, &transfered);

	/* Release */
	EVE_MediaFifo_close(phost);
	Esd_GpuAlloc_Free(Esd_GAlloc, fifoHandle);
	*imageFormat = RGB565;
	return res;
}

#ifdef EVE_FLASH_AVAILABLE

static bool Esd_LoadVideoFrameFromFlash(uint32_t *imageFormat, uint32_t dst, uint32_t src)
{
	EVE_HalContext *phost = Esd_GetHost();

	/* Allocate RAM_G space for ompletion pointer */
	Esd_GpuHandle ptrHandle = Esd_GpuAlloc_Alloc(Esd_GAlloc, 4, 0);
	uint32_t ptr = Esd_GpuAlloc_Get(Esd_GAlloc, ptrHandle);
	if (ptr == GA_INVALID)
	{
		return false;
	}

	/* Load the first video frame */
	EVE_CoCmd_flashSource(phost, src);
	EVE_CoCmd_videoStartF(phost);
	EVE_CoCmd_videoFrame(phost, dst, ptr);
	bool res = EVE_Cmd_waitFlush(phost);

	/* Release */
	Esd_GpuAlloc_Free(Esd_GAlloc, ptrHandle);
	*imageFormat = RGB565;
	return res;
}

#endif

#endif

ESD_CORE_EXPORT uint32_t Esd_LoadBitmap(Esd_BitmapInfo *bitmapInfo)
{
	EVE_HalContext *phost = Esd_GetHost();
	uint32_t addr;
	(void)phost;

	if (!bitmapInfo)
	{
#ifdef ESD_BITMAPINFO_DEBUG
		eve_printf_debug("Bitmap info is NULL\n");
#endif
		return GA_INVALID;
	}

	// If PreferRam is set, skip this and try to load to RAM_G first
#ifdef EVE_FLASH_AVAILABLE
	if (!bitmapInfo->PreferRam && ESD_BITMAPINFO_SUPPORT_DIRECT_FLASH(bitmapInfo))
	{
		// Just get the flash address if that's what we want
		uint32_t addr = bitmapInfo->FlashAddress;
		if (addr != FA_INVALID)
			return ESD_DL_FLASH_ADDRESS(addr);
		return GA_INVALID;
	}
#else
	if (bitmapInfo->Flash)
	{
#ifdef ESD_BITMAPINFO_DEBUG
		eve_printf_debug("Flash storage not supported on this platform\n");
#endif
		return GA_INVALID;
	}
#endif

	// Get address of specified handle
	// eve_printf_debug("%i: %i\n", bitmapInfo->GpuHandle.Id, bitmapInfo->GpuHandle.Seq);
	addr = Esd_GpuAlloc_Get(Esd_GAlloc, bitmapInfo->GpuHandle);
	if (addr == GA_INVALID)
	{
		if (bitmapInfo->Flash ? (bitmapInfo->FlashAddress == FA_INVALID) : !bitmapInfo->File)
		{
#ifdef ESD_BITMAPINFO_DEBUG
			// eve_printf_debug("Bitmap info file name is NULL, cannot auto-load\n");
#endif
			return GA_INVALID;
		}

		// Not loaded, load this bitmap
		bitmapInfo->GpuHandle = Esd_GpuAlloc_Alloc(Esd_GAlloc, bitmapInfo->Size,
		    (bitmapInfo->Persistent ? 0 : GA_GC_FLAG) | ((bitmapInfo->Flash && bitmapInfo->PreferRam) ? GA_LOW_FLAG : 0));
		addr = Esd_GpuAlloc_Get(Esd_GAlloc, bitmapInfo->GpuHandle);
		if (addr != GA_INVALID)
		{
			bool coLoad = bitmapInfo->CoLoad || bitmapInfo->Format == JPEG || bitmapInfo->Format == PNG || bitmapInfo->Format == AVI;
			bitmapInfo->CoLoad = coLoad;

#if defined(EVE_SUPPORT_VIDEO)
			bool video = bitmapInfo->Video || bitmapInfo->Format == AVI;
			bitmapInfo->Video = video;
#endif

#ifdef ESD_BITMAPINFO_DEBUG
			eve_printf_debug("Allocated space for bitmap\n");
#endif

			// Allocation space OK
			if (
#if defined(EVE_SUPPORT_VIDEO)
				video ? 
				(
#ifdef EVE_FLASH_AVAILABLE
					bitmapInfo->Flash ? !Esd_LoadVideoFrameFromFlash(&bitmapInfo->Format, addr, bitmapInfo->FlashAddress) :
#endif
					!Esd_LoadVideoFrameFromFile(&bitmapInfo->Format, addr, bitmapInfo->File)
				) :
#endif
				(
#ifdef EVE_FLASH_AVAILABLE
			    bitmapInfo->Flash ? !Esd_LoadFromFlash(coLoad ? &bitmapInfo->Format : NULL, bitmapInfo->Compressed, addr, bitmapInfo->FlashAddress, bitmapInfo->Size) :
#endif
			                      !Esd_LoadFromFile(coLoad ? &bitmapInfo->Format : NULL, bitmapInfo->Compressed, addr, bitmapInfo->File)
				))
			{
#ifdef ESD_BITMAPINFO_DEBUG
				eve_printf_debug(bitmapInfo->Flash ? "Failed to load bitmap from flash\n" : "Failed to load bitmap from file\n");
#endif
				// Failed to load from file
				Esd_GpuAlloc_Free(Esd_GAlloc, bitmapInfo->GpuHandle);
				addr = GA_INVALID;
			}

			// Special load for DXT1, additional file loaded halfway the address space
			if (bitmapInfo->Flash ? (bitmapInfo->AdditionalFlashAddress != FA_INVALID) : (!!bitmapInfo->AdditionalFile))
			{
				if (
#ifdef EVE_FLASH_AVAILABLE
				    bitmapInfo->Flash ? !Esd_LoadFromFlash(coLoad ? &bitmapInfo->Format : NULL, bitmapInfo->Compressed, addr + (bitmapInfo->Size >> 1), bitmapInfo->AdditionalFlashAddress, bitmapInfo->Size) :
#endif
				                      !Esd_LoadFromFile(coLoad ? &bitmapInfo->Format : NULL, bitmapInfo->Compressed, addr + (bitmapInfo->Size >> 1), bitmapInfo->AdditionalFile))
				{
#ifdef ESD_BITMAPINFO_DEBUG
					eve_printf_debug(bitmapInfo->Flash ? "Failed to load additional bitmap from flash\n" : "Failed to load additional bitmap from file\n");
#endif
					// Failed to load from additional file
					// Esd_GpuAlloc_Free(Esd_GAlloc, bitmapInfo->GpuHandle);
					addr = GA_INVALID;
				}
			}
		}
		else
		{
#ifdef ESD_BITMAPINFO_DEBUG
			eve_printf_debug("Unable to allocate space for bitmap\n");
#endif
		}
	}

#ifdef EVE_FLASH_AVAILABLE
	if (addr == GA_INVALID && ESD_BITMAPINFO_SUPPORT_DIRECT_FLASH(bitmapInfo))
	{
		// Just get the flash address anyway if we failed to load (in case of PreferRam)
		addr = bitmapInfo->FlashAddress;
		if (addr != FA_INVALID)
			return ESD_DL_FLASH_ADDRESS(addr);
		return GA_INVALID;
	}
#endif

	if (addr != GA_INVALID)
	{
		addr = ESD_DL_RAM_G_ADDRESS(addr);
	}

	return addr;
}

ESD_CORE_EXPORT uint32_t Esd_LoadPalette(Esd_BitmapInfo *bitmapInfo)
{
	EVE_HalContext *phost = Esd_GetHost();
	uint32_t addr;
	(void)phost;

	if (!bitmapInfo)
	{
#ifdef ESD_BITMAPINFO_DEBUG
		eve_printf_debug("Bitmap info is NULL\n");
#endif
		return GA_INVALID;
	}

#ifndef EVE_FLASH_AVAILABLE
	if (bitmapInfo->Flash)
	{
#ifdef ESD_BITMAPINFO_DEBUG
		// eve_printf_debug("Flash storage not supported on this platform\n");
#endif
		return GA_INVALID;
	}
#endif

	if (EVE_CHIPID >= EVE_FT810)
	{
		// Get palette address of specified handle
		addr = Esd_GpuAlloc_Get(Esd_GAlloc, bitmapInfo->PaletteGpuHandle);
		if (addr == GA_INVALID)
		{
			uint32_t size;

			if (bitmapInfo->Flash ? (bitmapInfo->PaletteFlashAddress == FA_INVALID) : (!bitmapInfo->PaletteFile))
			{
#ifdef ESD_BITMAPINFO_DEBUG
				// eve_printf_debug("Bitmap info palette file name is NULL, cannot auto-load\n");
#endif
				return GA_INVALID;
			}

			switch (bitmapInfo->Format)
			{
			case PALETTED8:
				size = 256 * 4;
				break;
			case PALETTED4444:
				size = 256 * 2;
				break;
			case PALETTED565:
				size = 256 * 2;
				break;
			default:
				eve_printf_debug("Unknown palette format, cannot load\n");
				return GA_INVALID;
			}

			// Not loaded, load this bitmap palette
			bitmapInfo->PaletteGpuHandle = Esd_GpuAlloc_Alloc(Esd_GAlloc, size, bitmapInfo->Persistent ? 0 : GA_GC_FLAG);
			addr = Esd_GpuAlloc_Get(Esd_GAlloc, bitmapInfo->PaletteGpuHandle);
			if (addr != GA_INVALID)
			{
#ifdef ESD_BITMAPINFO_DEBUG
				eve_printf_debug("Allocated space for bitmap palette\n");
#endif
				// Allocation space OK
				if (
#ifdef EVE_FLASH_AVAILABLE
				    bitmapInfo->Flash ? !EVE_CoCmd_flashRead_flush(Esd_Host, addr, bitmapInfo->PaletteFlashAddress, size) :
#endif
				                      !EVE_Util_loadRawFile(Esd_Host, addr, bitmapInfo->PaletteFile))
				{
#ifdef ESD_BITMAPINFO_DEBUG
					eve_printf_debug(bitmapInfo->Flash ? "Failed to load palette from flash\n" : "Failed to load palette from file\n");
#endif
					// Failed to load from file
					Esd_GpuAlloc_Free(Esd_GAlloc, bitmapInfo->PaletteGpuHandle);
					addr = GA_INVALID;
				}
			}
			else
			{
#ifdef ESD_BITMAPINFO_DEBUG
				eve_printf_debug("Unable to allocate space for bitmap palette\n");
#endif
			}
		}
	}
	else // FT_80X_ENABLE
	{
		eve_printf_debug("Palette not yet implemented for FT80x\n");
		addr = GA_INVALID;
	}

	if (addr != GA_INVALID)
	{
		addr = ESD_DL_RAM_G_ADDRESS(addr);
	}

	return addr;
}

ESD_CORE_EXPORT void Esd_BitmapCell_Persist(Esd_BitmapCell bitmapCell)
{
	if (bitmapCell.Info)
	{
		Esd_LoadBitmap(bitmapCell.Info);
		Esd_LoadPalette(bitmapCell.Info);
	}
}

/* end of file */
