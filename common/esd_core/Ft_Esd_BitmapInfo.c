
#include "Ft_LoadFile.h"
#include "Ft_Esd_BitmapInfo.h"
#include "Ft_Esd_GpuAlloc.h"
#include "Ft_Esd_CoCmd.h"

#ifndef NDEBUG
#define ESD_BITMAPINFO_DEBUG
#endif

extern EVE_HalContext *Ft_Esd_Host;
extern Ft_Esd_GpuAlloc *Ft_Esd_GAlloc;

#ifdef EVE_FLASH_AVAILABLE
#define ESD_BITMAPINFO_SUPPORT_DIRECT_FLASH(bitmapInfo) (bitmapInfo->Flash && ESD_IS_FORMAT_ASTC(bitmapInfo->Format))
#endif

static ft_bool_t Ft_Esd_LoadFromFile(ft_uint32_t *imageFormat, ft_bool_t deflate, ft_uint32_t dst, const char *file)
{
	return imageFormat
	    ? Ft_Hal_LoadImageFile(Ft_Esd_Host, dst, file, imageFormat)
	    : (deflate
	              ? Ft_Hal_LoadInflateFile(Ft_Esd_Host, dst, file)
	              : Ft_Hal_LoadRawFile(Ft_Esd_Host, dst, file));
}

#ifdef EVE_FLASH_AVAILABLE

static ft_bool_t Ft_Esd_LoadFromFlash(ft_uint32_t *imageFormat, ft_bool_t deflate, ft_uint32_t dst, ft_uint32_t src, ft_uint32_t size)
{
	return imageFormat
	    ? Ft_Gpu_CoCmd_LoadImage_Flash(Ft_Esd_Host, dst, src, imageFormat)
	    : (deflate
	              ? Ft_Gpu_CoCmd_Inflate_Flash(Ft_Esd_Host, dst, src)
	              : Ft_Gpu_CoCmd_FlashRead(Ft_Esd_Host, dst, src, size));
}

#endif

ft_uint32_t Ft_Esd_LoadBitmap(Ft_Esd_BitmapInfo *bitmapInfo)
{
	EVE_HalContext *phost = Ft_Esd_Host;
	ft_uint32_t addr;
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
		ft_uint32_t addr = bitmapInfo->FlashAddress;
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
	addr = Ft_Esd_GpuAlloc_Get(Ft_Esd_GAlloc, bitmapInfo->GpuHandle);
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
		bitmapInfo->GpuHandle = Ft_Esd_GpuAlloc_Alloc(Ft_Esd_GAlloc, bitmapInfo->Size,
		    (bitmapInfo->Persistent ? 0 : GA_GC_FLAG) | ((bitmapInfo->Flash && bitmapInfo->PreferRam) ? GA_LOW_FLAG : 0));
		addr = Ft_Esd_GpuAlloc_Get(Ft_Esd_GAlloc, bitmapInfo->GpuHandle);
		if (addr != GA_INVALID)
		{
			ft_bool_t coLoad = bitmapInfo->CoLoad || bitmapInfo->Format == JPEG || bitmapInfo->Format == PNG;
			bitmapInfo->CoLoad = coLoad;

#ifdef ESD_BITMAPINFO_DEBUG
			eve_printf_debug("Allocated space for bitmap\n");
#endif

			// Allocation space OK
			if (
#ifdef EVE_FLASH_AVAILABLE
			    bitmapInfo->Flash ? !Ft_Esd_LoadFromFlash(coLoad ? &bitmapInfo->Format : NULL, bitmapInfo->Compressed, addr, bitmapInfo->FlashAddress, bitmapInfo->Size) :
#endif
			                      !Ft_Esd_LoadFromFile(coLoad ? &bitmapInfo->Format : NULL, bitmapInfo->Compressed, addr, bitmapInfo->File))
			{
#ifdef ESD_BITMAPINFO_DEBUG
				eve_printf_debug(bitmapInfo->Flash ? "Failed to load bitmap from flash\n" : "Failed to load bitmap from file\n");
#endif
				// Failed to load from file
				Ft_Esd_GpuAlloc_Free(Ft_Esd_GAlloc, bitmapInfo->GpuHandle);
				addr = GA_INVALID;
			}

			// Special load for DXT1, additional file loaded halfway the address space
			if (bitmapInfo->Flash ? (bitmapInfo->AdditionalFlashAddress != FA_INVALID) : (!!bitmapInfo->AdditionalFile))
			{
				if (
#ifdef EVE_FLASH_AVAILABLE
				    bitmapInfo->Flash ? !Ft_Esd_LoadFromFlash(coLoad ? &bitmapInfo->Format : NULL, bitmapInfo->Compressed, addr + (bitmapInfo->Size >> 1), bitmapInfo->AdditionalFlashAddress, bitmapInfo->Size) :
#endif
				                      !Ft_Esd_LoadFromFile(coLoad ? &bitmapInfo->Format : NULL, bitmapInfo->Compressed, addr + (bitmapInfo->Size >> 1), bitmapInfo->AdditionalFile))
				{
#ifdef ESD_BITMAPINFO_DEBUG
					eve_printf_debug(bitmapInfo->Flash ? "Failed to load additional bitmap from flash\n" : "Failed to load additional bitmap from file\n");
#endif
					// Failed to load from additional file
					// Ft_Esd_GpuAlloc_Free(Ft_Esd_GAlloc, bitmapInfo->GpuHandle);
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

ft_uint32_t Ft_Esd_LoadPalette(Ft_Esd_BitmapInfo *bitmapInfo)
{
	EVE_HalContext *phost = Ft_Esd_Host;
	ft_uint32_t addr;
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
		addr = Ft_Esd_GpuAlloc_Get(Ft_Esd_GAlloc, bitmapInfo->PaletteGpuHandle);
		if (addr == GA_INVALID)
		{
			ft_uint32_t size;

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
			bitmapInfo->PaletteGpuHandle = Ft_Esd_GpuAlloc_Alloc(Ft_Esd_GAlloc, size, bitmapInfo->Persistent ? 0 : GA_GC_FLAG);
			addr = Ft_Esd_GpuAlloc_Get(Ft_Esd_GAlloc, bitmapInfo->PaletteGpuHandle);
			if (addr != GA_INVALID)
			{
#ifdef ESD_BITMAPINFO_DEBUG
				eve_printf_debug("Allocated space for bitmap palette\n");
#endif
				// Allocation space OK
				if (
#ifdef EVE_FLASH_AVAILABLE
				    bitmapInfo->Flash ? !Ft_Gpu_CoCmd_FlashRead(Ft_Esd_Host, addr, bitmapInfo->PaletteFlashAddress, size) :
#endif
				                      !Ft_Hal_LoadRawFile(Ft_Esd_Host, addr, bitmapInfo->PaletteFile))
				{
#ifdef ESD_BITMAPINFO_DEBUG
					eve_printf_debug(bitmapInfo->Flash ? "Failed to load palette from flash\n" : "Failed to load palette from file\n");
#endif
					// Failed to load from file
					Ft_Esd_GpuAlloc_Free(Ft_Esd_GAlloc, bitmapInfo->PaletteGpuHandle);
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

Ft_Esd_BitmapCell Ft_Esd_SwitchBitmapCell(Ft_Esd_BitmapCell bitmapCell, ft_uint16_t cell)
{
	bitmapCell.Cell = cell;
	return bitmapCell;
}

void Ft_Esd_BitmapPersist(Ft_Esd_BitmapCell bitmapCell)
{
	if (bitmapCell.Info)
	{
		Ft_Esd_LoadBitmap(bitmapCell.Info);
		Ft_Esd_LoadPalette(bitmapCell.Info);
	}
}

/* end of file */
