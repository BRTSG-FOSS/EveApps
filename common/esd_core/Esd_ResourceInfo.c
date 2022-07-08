/*
ESD Framework
Copyright (C) 2018  Bridgetek Pte Lte
Author: Jan Boon <jan.boon@kaetemi.be>
*/

#include "Esd_ResourceInfo.h"
#include "Esd_Context.h"

#ifndef NDEBUG
#define ESD_RESOURCEINFO_DEBUG
#endif

#ifdef ESD_RESOURCEINFO_DEBUG
#define esd_resourceinfo_printf(fmt, ...) eve_printf(fmt, ##__VA_ARGS__)
#else
#define esd_resourceinfo_printf(fmt, ...) \
	do                                    \
	{                                     \
	} while (false)
#endif

extern ESD_CORE_EXPORT EVE_HalContext *Esd_Host;
extern ESD_CORE_EXPORT Esd_GpuAlloc *Esd_GAlloc;

ESD_CORE_EXPORT void Esd_LoadResourceMetadata(Esd_ResourceInfo *resourceInfo, uint8_t *metadata)
{
	if (metadata && metadata[ESD_METADATA_SIGNATURE])
	{
		// Update compression and extracted size from metadata
		ESD_METADATA_SET(uint8_t, resourceInfo, Compressed, metadata[ESD_METADATA_COMPRESSION], resourceInfo->File);
		ESD_METADATA_SET(uint32_t, resourceInfo, RawSize, ESD_RD32_LE(metadata, ESD_METADATA_RAWSIZE), resourceInfo->File);
	}
}

ESD_CORE_EXPORT uint32_t Esd_LoadResourceEx(Esd_ResourceInfo *resourceInfo, uint8_t *metadata, uint32_t *imageFormat)
{
	EVE_HalContext *phost = Esd_GetHost();
	int32_t flashAddr = FA_INVALID;
	uint32_t addr;
	bool loaded;
	(void)phost;

	if (metadata)
		metadata[ESD_METADATA_SIGNATURE] = '\0';

	if (!resourceInfo)
	{
		esd_resourceinfo_printf("Resource info is NULL\n");
		return GA_INVALID;
	}

	if (!resourceInfo->Compressed && resourceInfo->Type == ESD_RESOURCE_DIRECTFLASH)
	{
#ifdef EVE_FLASH_AVAILABLE
		// Just get the flash address if that's what we want
		// Calling function may need to ensure that it's not getting a flash address when it's not supported by the resource
		flashAddr = Esd_BitmapInfo_LoadFlashAddress(&resourceInfo->FlashAddress, resourceInfo->File, metadata);
		Esd_LoadResourceMetadata(resourceInfo, metadata);
		if (!resourceInfo->Compressed)
		{
			// If no metadata, or the metadata still specifies uncompressed data, return as direct flash address
			if (flashAddr != FA_INVALID)
				return ESD_DL_FLASH_ADDRESS(flashAddr);
			return GA_INVALID;
		}
#else
		esd_resourceinfo_printf("Flash storage not supported on this platform\n");
		return GA_INVALID;
#endif
	}

	// Get address of specified handle
	addr = Esd_GpuAlloc_Get(Esd_GAlloc, resourceInfo->GpuHandle);
	if (addr != GA_INVALID)
	{
		return ESD_DL_RAM_G_ADDRESS(addr);
	}

	// Validate source
	switch (resourceInfo->Type)
	{
	case ESD_RESOURCE_FILE:
		if (!resourceInfo->File)
		{
			esd_resourceinfo_printf("Resource file name is NULL\n");
			return GA_INVALID;
		}
		break;
	case ESD_RESOURCE_PROGMEM:
		if (!resourceInfo->ProgMem)
		{
			esd_resourceinfo_printf("Resource program memory address is NULL\n");
			return GA_INVALID;
		}
		break;
	case ESD_RESOURCE_FLASH:
	case ESD_RESOURCE_DIRECTFLASH:
		if ((flashAddr = Esd_BitmapInfo_LoadFlashAddress(&resourceInfo->FlashAddress, resourceInfo->File, metadata)) == FA_INVALID)
		{
			esd_resourceinfo_printf("Resource flash address is invalid\n");
			return GA_INVALID;
		}
		break;
	}

	// Load metadata from file on SD card
	if (metadata && resourceInfo->Type == ESD_RESOURCE_FILE)
	{
		// Generate metafile name
		size_t nameLen = strlen(resourceInfo->File);
		if (nameLen + 6 > 128) // ".esdm" NUL
			return GA_INVALID;
		char metaFile[128];
		strcpy_s(metaFile, sizeof(metaFile), resourceInfo->File);
		strcpy_s(&metaFile[nameLen], sizeof(metaFile) - nameLen, ".esdm");

		// Try to load
		EVE_Util_readFile(phost, metadata, ESD_METADATA_MAX, metaFile);
	}

	// Load metadata
	Esd_LoadResourceMetadata(resourceInfo, metadata);

	// Allocate gpu memory
	resourceInfo->GpuHandle = Esd_GpuAlloc_Alloc(Esd_GAlloc, resourceInfo->RawSize,
	    (resourceInfo->Persistent ? 0 : GA_GC_FLAG)
	        | ((!resourceInfo->Compressed && ESD_RESOURCE_IS_FLASH(resourceInfo->Type)) ? GA_LOW_FLAG : 0));
	addr = Esd_GpuAlloc_Get(Esd_GAlloc, resourceInfo->GpuHandle);
	if (addr == GA_INVALID)
	{
		return GA_INVALID;
	}

	// Attempt to load
	loaded = false;
	switch (resourceInfo->Type)
	{
	case ESD_RESOURCE_FILE:
	{
		switch (resourceInfo->Compressed)
		{
		case ESD_RESOURCE_RAW:
			loaded = EVE_Util_loadRawFile(phost, addr, resourceInfo->File);
			break;
		case ESD_RESOURCE_DEFLATE:
			loaded = EVE_Util_loadInflateFile(phost, addr, resourceInfo->File);
			break;
		case ESD_RESOURCE_IMAGE:
			loaded = EVE_Util_loadImageFile(phost, addr, resourceInfo->File, imageFormat);
			break;
		}
		break;
	}
	case ESD_RESOURCE_PROGMEM:
	{
		switch (resourceInfo->Compressed)
		{
		case ESD_RESOURCE_RAW:
			EVE_Hal_wrProgMem(phost, addr, resourceInfo->ProgMem, resourceInfo->StorageSize << 2);
			loaded = true;
			break;
		case ESD_RESOURCE_DEFLATE:
			loaded = EVE_CoCmd_inflate_progMem(phost, addr, resourceInfo->ProgMem, resourceInfo->StorageSize << 2);
			break;
		case ESD_RESOURCE_IMAGE:
			loaded = EVE_CoCmd_loadImage_progMem(phost, addr, resourceInfo->ProgMem, resourceInfo->StorageSize << 2, imageFormat);
			break;
		}
		break;
	}
#ifdef EVE_FLASH_AVAILABLE
	case ESD_RESOURCE_FLASH:
	case ESD_RESOURCE_DIRECTFLASH:
	{
		switch (resourceInfo->Compressed)
		{
		case ESD_RESOURCE_RAW:
			EVE_CoCmd_flashRead(phost, addr, flashAddr, resourceInfo->StorageSize << 2);
			loaded = EVE_Cmd_waitFlush(phost);
			break;
		case ESD_RESOURCE_DEFLATE:
			loaded = EVE_CoCmd_inflate_flash(phost, addr, flashAddr);
			break;
		case ESD_RESOURCE_IMAGE:
			loaded = EVE_CoCmd_loadImage_flash(phost, addr, flashAddr, imageFormat);
			break;
		}
		break;
	}
#endif
	}

	if (!loaded)
	{
		// Failed to load
		Esd_GpuAlloc_Free(Esd_GAlloc, resourceInfo->GpuHandle);
		addr = GA_INVALID;
	}

	// Return address if we found it
	if (addr != GA_INVALID)
	{
		return ESD_DL_RAM_G_ADDRESS(addr);
	}

	/*
	// May be implemented by the calling function on GA_INVALID result
#ifdef EVE_FLASH_AVAILABLE
	// If flash source, we can use directly from flash (if supported)
	if (supportDirectFlash && resourceInfo->Type == ESD_RESOURCE_FLASH)
	{
		if (flashAddr != FA_INVALID)
			return ESD_DL_FLASH_ADDRESS(flashAddr);
	}
#endif
	*/

	// Fail to load
	return GA_INVALID;
}

ESD_CORE_EXPORT uint32_t Esd_LoadResource(Esd_ResourceInfo *resourceInfo, uint32_t *imageFormat)
{
	uint8_t metadata[ESD_METADATA_MAX];
	return Esd_LoadResourceEx(resourceInfo, metadata, imageFormat);
}

ESD_CORE_EXPORT void Esd_FreeResource(Esd_ResourceInfo *resourceInfo)
{
	if (!resourceInfo)
		return;

	Esd_GpuAlloc_Free(Esd_GAlloc, resourceInfo->GpuHandle);
	resourceInfo->GpuHandle.Id = MAX_NUM_ALLOCATIONS;
#ifdef ESD_LITTLEFS_FLASH
	resourceInfo->FlashAddress = FA_INVALID;
#endif
}

ESD_CORE_EXPORT void Esd_ResourcePersist(Esd_ResourceInfo *resourceInfo)
{
	Esd_LoadResource(resourceInfo, false);
}

/* end of file */
