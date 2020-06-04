/*
ESD Framework
Copyright (C) 2018  Bridgetek Pte Lte
Author: Jan Boon <jan.boon@kaetemi.be>
*/

#include "ESD_ResourceInfo.h"
#include <EVE_Hal.h>

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

extern EVE_HalContext *ESD_Host;
extern ESD_GpuAlloc *ESD_GAlloc;

uint32_t Esd_LoadResource(Esd_ResourceInfo *resourceInfo, uint32_t *imageFormat)
{
	EVE_HalContext *phost = ESD_Host;
	uint32_t addr;
	bool loaded;
	(void)phost;

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
		uint32_t addr = resourceInfo->FlashAddress;
		if (addr != FA_INVALID)
			return ESD_DL_FLASH_ADDRESS(addr);
		return GA_INVALID;
#else
		esd_resourceinfo_printf("Flash storage not supported on this platform\n");
		return GA_INVALID;
#endif
	}

	// Get address of specified handle
	addr = ESD_GpuAlloc_Get(ESD_GAlloc, resourceInfo->GpuHandle);
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
		if (resourceInfo->FlashAddress == FA_INVALID)
		{
			esd_resourceinfo_printf("Resource flash address is invalid\n");
			return GA_INVALID;
		}
		break;
	}

	// Allocate gpu memory
	resourceInfo->GpuHandle = ESD_GpuAlloc_Alloc(ESD_GAlloc, resourceInfo->RawSize,
	    (resourceInfo->Persistent ? 0 : GA_GC_FLAG)
	        | ((!resourceInfo->Compressed && ESD_RESOURCE_IS_FLASH(resourceInfo->Type)) ? GA_LOW_FLAG : 0));
	addr = ESD_GpuAlloc_Get(ESD_GAlloc, resourceInfo->GpuHandle);
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
			loaded = Ft_Hal_LoadRawFile(ESD_Host, addr, resourceInfo->File);
			break;
		case ESD_RESOURCE_DEFLATE:
			loaded = Ft_Hal_LoadInflateFile(ESD_Host, addr, resourceInfo->File);
			break;
		case ESD_RESOURCE_IMAGE:
			loaded = Ft_Hal_LoadImageFile(ESD_Host, addr, resourceInfo->File, imageFormat);
			break;
		}
		break;
	}
	case ESD_RESOURCE_PROGMEM:
	{
		switch (resourceInfo->Compressed)
		{
		case ESD_RESOURCE_RAW:
			EVE_Hal_wrProgMem(ESD_Host, addr, resourceInfo->ProgMem, resourceInfo->StorageSize << 2);
			loaded = true;
			break;
		case ESD_RESOURCE_DEFLATE:
			loaded = EVE_CoCmd_inflate_progMem(ESD_Host, addr, resourceInfo->ProgMem, resourceInfo->StorageSize << 2);
			break;
		case ESD_RESOURCE_IMAGE:
			loaded = EVE_CoCmd_loadImage_progMem(ESD_Host, addr, resourceInfo->ProgMem, resourceInfo->StorageSize << 2, imageFormat);
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
			loaded = EVE_CoCmd_flashRead(ESD_Host, addr, resourceInfo->FlashAddress, resourceInfo->StorageSize << 2);
			break;
		case ESD_RESOURCE_DEFLATE:
			loaded = EVE_CoCmd_inflate_Flash(ESD_Host, addr, resourceInfo->FlashAddress);
			break;
		case ESD_RESOURCE_IMAGE:
			loaded = EVE_CoCmd_loadImage_Flash(ESD_Host, addr, resourceInfo->FlashAddress, imageFormat);
			break;
		}
		break;
	}
#endif
	}

	if (!loaded)
	{
		// Failed to load
		ESD_GpuAlloc_Free(ESD_GAlloc, resourceInfo->GpuHandle);
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
		uint32_t addr = resourceInfo->FlashAddress;
		if (addr != FA_INVALID)
			return ESD_DL_FLASH_ADDRESS(addr);
	}
#endif
	*/

	// Fail to load
	return GA_INVALID;
}

void Esd_FreeResource(Esd_ResourceInfo *resourceInfo)
{
	if (!resourceInfo)
		return;

	ESD_GpuAlloc_Free(ESD_GAlloc, resourceInfo->GpuHandle);
	resourceInfo->GpuHandle.Id = MAX_NUM_ALLOCATIONS;
}

void Esd_ResourcePersist(Esd_ResourceInfo *resourceInfo)
{
	Esd_LoadResource(resourceInfo, false);
}

/* end of file */
