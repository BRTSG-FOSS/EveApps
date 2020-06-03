/*
ESD Framework
Copyright (C) 2018  Bridgetek Pte Lte
Author: Jan Boon <jan.boon@kaetemi.be>
*/

/*
Resource info structure
*/

#ifndef ESD_RESOURCEINFO__H
#define ESD_RESOURCEINFO__H

#include "ESD_TypeDefs.h"
#include "ESD_GpuAlloc.h"

// Resource loading mechanisms
ESD_ENUM(Esd_ResourceType, DisplayName = "Resource Type")
#define ESD_RESOURCE_FILE 0
#define ESD_RESOURCE_PROGMEM 1
#define ESD_RESOURCE_FLASH 2
#define ESD_RESOURCE_DIRECTFLASH 3
ESD_END()

ESD_ENUM(Esd_ResourceCompression, DisplayName = "Resource Compression")
#define ESD_RESOURCE_RAW 0
#define ESD_RESOURCE_DEFLATE 1
#define ESD_RESOURCE_IMAGE 2
ESD_END()

// Both flash and direct flash have the same type bit set on purpose
#define ESD_RESOURCE_IS_FLASH(resourceType) ((resourceType & ESD_RESOURCE_FLASH) == ESD_RESOURCE_FLASH)

ESD_TYPE(Esd_ResourceInfo, Native = Struct)
typedef struct Esd_ResourceInfo // (16 bytes) (24 bytes on 64 bit)
{
	// Source of data
	union
	{
		const char *File;
		int32_t FlashAddress;
		eve_progmem_const uint8_t *ProgMem;
	};

	// (Runtime) Handle pointing to the address in RAM_G if it is allocated
	ESD_GpuHandle GpuHandle;

	// Size of data in storage, divided by 4, rounded up ((size + 3) >> 2) (size in int32)
	uint32_t StorageSize : 27;

	// (Esd_ResourceType) Source of data
	uint32_t Type : 2;

	// Load from compressed format
	uint32_t Compressed : 2;

	// When this is set, the allocated ram is not free'd automatically
	// Use ESD_GpuAlloc_Free(GpuAlloc, GpuHandle) or Esd_FreeResource to free the GPU ram manually
	uint32_t Persistent : 1;

	// Size of data when decompressed into RAM_G
	uint32_t RawSize;

} Esd_ResourceInfo;

ESD_TYPE(Esd_ResourceInfo *, Native = Pointer, Edit = Library)

#if (EVE_SUPPORT_CHIPID >= EVE_FT810)
#ifdef EVE_FLASH_AVAILABLE
/// Transform flash address to BITMAP_SOURCE address parameter
#define ESD_DL_FLASH_ADDRESS(address) ((address >> 5) | 0x800000)
#endif
/// Transform RAM_G address to BITMAP_SOURCE address paramter
#define ESD_DL_RAM_G_ADDRESS(address) (EVE_CHIPID >= EVE_FT810 ? (address & 0x3FFFFF) : (address & 0xFFFFF))
#else
/// Transform RAM_G address to BITMAP_SOURCE address paramter
#define ESD_DL_RAM_G_ADDRESS(address) (address & 0xFFFFF)
#endif

/// Flash address invalid
#define FA_INVALID 0

/// A function to load resource data into RAM_G (or to use the resource from flash directly)
/// Returns address in the format as specified by the BITMAP_SOURCE command (see ESD_DL_FLASH_ADDRESS and ESD_DL_RAM_G_ADDRESS macros)
/// Returns the output image format if the resource is an image loaded through the coprocessor
uint32_t Esd_LoadResource(Esd_ResourceInfo *resourceInfo, uint32_t *imageFormat);

/// Free a currently loaded resource from RAM_G. Can be used to enforce reloading a resource.
void Esd_FreeResource(Esd_ResourceInfo *resourceInfo);

/// A function to make fonts persistent in memory by reloading the data if necessary, called during the Update cycle of each frame
ESD_UPDATE(Esd_ResourcePersist, DisplayName = "Persist Resource", Category = EsdUtilities)
ESD_PARAMETER(resourceInfo, Type = Esd_ResourceInfo *)
void Esd_ResourcePersist(Esd_ResourceInfo *resourceInfo);

// Number of available bitmap handles
#define ESD_BITMAPHANDLE_NB (EVE_CHIPID >= EVE_FT810 ? 32UL : 16UL)
#if (EVE_SUPPORT_CHIPID >= EVE_FT810)
#define ESD_BITMAPHANDLE_CAP (32UL)
#else
#define ESD_BITMAPHANDLE_CAP (16UL)
#endif

// An invalid bitmap handle
#define ESD_BITMAPHANDLE_INVALID 0x3F
#define ESD_BITMAPHANDLE_VALID(bitmapHandle) (bitmapHandle < ESD_BITMAPHANDLE_NB)

/// Check if a format is an ASTC format
#define ESD_IS_FORMAT_ASTC(format) ((format & 0xFFF0) == 0x93B0)

#endif /* #ifndef ESD_FONTINFO_H */

/* end of file */
