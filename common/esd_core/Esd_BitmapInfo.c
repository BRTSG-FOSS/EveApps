
#include "ESD_BitmapInfo.h"
#include <EVE_Hal.h>

#include "ESD_GpuAlloc.h"

#ifndef NDEBUG
#define ESD_BITMAPINFO_DEBUG
#endif

extern ESD_CORE_EXPORT EVE_HalContext *ESD_Host;
extern ESD_CORE_EXPORT ESD_GpuAlloc *ESD_GAlloc;

#ifdef EVE_FLASH_AVAILABLE
#define ESD_BITMAPINFO_SUPPORT_DIRECT_FLASH(bitmapInfo) (bitmapInfo->Flash && ESD_IS_FORMAT_ASTC(bitmapInfo->Format))
#endif

static bool ESD_LoadFromFile(uint32_t *imageFormat, bool deflate, uint32_t dst, const char *file)
{
	return imageFormat
	    ? EVE_Util_loadImageFile(ESD_Host, dst, file, imageFormat)
	    : (deflate
	              ? EVE_Util_loadInflateFile(ESD_Host, dst, file)
	              : EVE_Util_loadRawFile(ESD_Host, dst, file));
}

#ifdef EVE_FLASH_AVAILABLE

static bool ESD_LoadFromFlash(uint32_t *imageFormat, bool deflate, uint32_t dst, uint32_t src, uint32_t size)
{
	return imageFormat
	    ? EVE_CoCmd_loadImage_flash(ESD_Host, dst, src, imageFormat)
	    : (deflate
	              ? EVE_CoCmd_inflate_flash(ESD_Host, dst, src)
	              : EVE_CoCmd_flashRead_flush(ESD_Host, dst, src, size));
}

#endif

uint32_t ESD_LoadBitmap(ESD_BitmapInfo *bitmapInfo)
{
	EVE_HalContext *phost = ESD_Host;
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
	addr = ESD_GpuAlloc_Get(ESD_GAlloc, bitmapInfo->GpuHandle);
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
		bitmapInfo->GpuHandle = ESD_GpuAlloc_Alloc(ESD_GAlloc, bitmapInfo->Size,
		    (bitmapInfo->Persistent ? 0 : GA_GC_FLAG) | ((bitmapInfo->Flash && bitmapInfo->PreferRam) ? GA_LOW_FLAG : 0));
		addr = ESD_GpuAlloc_Get(ESD_GAlloc, bitmapInfo->GpuHandle);
		if (addr != GA_INVALID)
		{
			bool coLoad = bitmapInfo->CoLoad || bitmapInfo->Format == JPEG || bitmapInfo->Format == PNG;
			bitmapInfo->CoLoad = coLoad;

#ifdef ESD_BITMAPINFO_DEBUG
			eve_printf_debug("Allocated space for bitmap\n");
#endif

			// Allocation space OK
			if (
#ifdef EVE_FLASH_AVAILABLE
			    bitmapInfo->Flash ? !ESD_LoadFromFlash(coLoad ? &bitmapInfo->Format : NULL, bitmapInfo->Compressed, addr, bitmapInfo->FlashAddress, bitmapInfo->Size) :
#endif
			                      !ESD_LoadFromFile(coLoad ? &bitmapInfo->Format : NULL, bitmapInfo->Compressed, addr, bitmapInfo->File))
			{
#ifdef ESD_BITMAPINFO_DEBUG
				eve_printf_debug(bitmapInfo->Flash ? "Failed to load bitmap from flash\n" : "Failed to load bitmap from file\n");
#endif
				// Failed to load from file
				ESD_GpuAlloc_Free(ESD_GAlloc, bitmapInfo->GpuHandle);
				addr = GA_INVALID;
			}

			// Special load for DXT1, additional file loaded halfway the address space
			if (bitmapInfo->Flash ? (bitmapInfo->AdditionalFlashAddress != FA_INVALID) : (!!bitmapInfo->AdditionalFile))
			{
				if (
#ifdef EVE_FLASH_AVAILABLE
				    bitmapInfo->Flash ? !ESD_LoadFromFlash(coLoad ? &bitmapInfo->Format : NULL, bitmapInfo->Compressed, addr + (bitmapInfo->Size >> 1), bitmapInfo->AdditionalFlashAddress, bitmapInfo->Size) :
#endif
				                      !ESD_LoadFromFile(coLoad ? &bitmapInfo->Format : NULL, bitmapInfo->Compressed, addr + (bitmapInfo->Size >> 1), bitmapInfo->AdditionalFile))
				{
#ifdef ESD_BITMAPINFO_DEBUG
					eve_printf_debug(bitmapInfo->Flash ? "Failed to load additional bitmap from flash\n" : "Failed to load additional bitmap from file\n");
#endif
					// Failed to load from additional file
					// ESD_GpuAlloc_Free(ESD_GAlloc, bitmapInfo->GpuHandle);
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

uint32_t ESD_LoadPalette(ESD_BitmapInfo *bitmapInfo)
{
	EVE_HalContext *phost = ESD_Host;
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
		addr = ESD_GpuAlloc_Get(ESD_GAlloc, bitmapInfo->PaletteGpuHandle);
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
			bitmapInfo->PaletteGpuHandle = ESD_GpuAlloc_Alloc(ESD_GAlloc, size, bitmapInfo->Persistent ? 0 : GA_GC_FLAG);
			addr = ESD_GpuAlloc_Get(ESD_GAlloc, bitmapInfo->PaletteGpuHandle);
			if (addr != GA_INVALID)
			{
#ifdef ESD_BITMAPINFO_DEBUG
				eve_printf_debug("Allocated space for bitmap palette\n");
#endif
				// Allocation space OK
				if (
#ifdef EVE_FLASH_AVAILABLE
				    bitmapInfo->Flash ? !EVE_CoCmd_flashRead_flush(ESD_Host, addr, bitmapInfo->PaletteFlashAddress, size) :
#endif
				                      !EVE_Util_loadRawFile(ESD_Host, addr, bitmapInfo->PaletteFile))
				{
#ifdef ESD_BITMAPINFO_DEBUG
					eve_printf_debug(bitmapInfo->Flash ? "Failed to load palette from flash\n" : "Failed to load palette from file\n");
#endif
					// Failed to load from file
					ESD_GpuAlloc_Free(ESD_GAlloc, bitmapInfo->PaletteGpuHandle);
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

ESD_BitmapCell ESD_BitmapCell_Switched(ESD_BitmapCell bitmapCell, uint16_t cell)
{
	bitmapCell.Cell = cell;
	return bitmapCell;
}

void ESD_BitmapCell_Persist(ESD_BitmapCell bitmapCell)
{
	if (bitmapCell.Info)
	{
		ESD_LoadBitmap(bitmapCell.Info);
		ESD_LoadPalette(bitmapCell.Info);
	}
}

/* end of file */
