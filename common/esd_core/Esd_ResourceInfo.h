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

#include "Esd_Base.h"
#include "Esd_GpuAlloc.h"

#ifdef __cplusplus
extern "C" {
#endif

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
		eve_progmem_const uint8_t *ProgMem;
		const char *File;
#ifdef ESD_LITTLEFS_FLASH
	};
	union
	{
#endif
		int32_t FlashAddress;
	};

	// (Runtime) Handle pointing to the address in RAM_G if it is allocated
	Esd_GpuHandle GpuHandle;

	// Size of data in storage, divided by 4, rounded up ((size + 3) >> 2) (size in int32)
	uint32_t StorageSize : 27;

	// (Esd_ResourceType) Source of data
	uint32_t Type : 2;

	// Load from compressed format
	uint32_t Compressed : 2;

	// When this is set, the allocated ram is not free'd automatically
	// Use Esd_GpuAlloc_Free(GpuAlloc, GpuHandle) or Esd_FreeResource to free the GPU ram manually
	uint32_t Persistent : 1;

	// Size of data when decompressed into RAM_G
	uint32_t RawSize;

} Esd_ResourceInfo;

ESD_TYPE(Esd_ResourceInfo *, Native = Pointer, Edit = Library)

#if (EVE_SUPPORT_CHIPID >= EVE_FT810)
#ifdef EVE_FLASH_AVAILABLE
/// Transform flash address to BITMAP_SOURCE address parameter
#define ESD_DL_FLASH_ADDRESS(address) ((address >> 5) | 0x800000L)
/// Check if the address parameter is a flash address
#define ESD_DL_IS_FLASH_ADDRESS(address) ((address & 0x800000L) != 0)
/// Get the flash address from a BITMAP_SOURCE address parameter
#define ESD_DL_GET_FLASH_ADDRESS(address) ((address & ~0x800000L) << 5)
#endif
/// Transform RAM_G address to BITMAP_SOURCE address paramter
#define ESD_DL_RAM_G_ADDRESS(address) (EVE_CHIPID >= EVE_FT810 ? (address & 0x3FFFFFL) : (address & 0xFFFFFL))
#else
/// Transform RAM_G address to BITMAP_SOURCE address paramter
#define ESD_DL_RAM_G_ADDRESS(address) (address & 0xFFFFF)
#endif

/// Flash address invalid
#define FA_INVALID 0

/// Largest size of resource metadata file
#define ESD_METADATA_MAX 64

/// Resource signatures
#define ESD_RESOURCE_RES 0x534552
#define ESD_RESOURCE_BMP 0x504D42
#define ESD_RESOURCE_FNT 0x544E46
#define ESD_RESOURCE_ANI 0x494E41

/// Offsets for the first 12 bytes
#define ESD_METADATA_SIGNATURE 0
#define ESD_METADATA_VERSION 4
#define ESD_METADATA_SIZE 5
#define ESD_METADATA_COMPRESSION 6
#define ESD_METADATA_EXTLEN 7
#define ESD_METADATA_RAWSIZE 8
#define ESD_METADATA_DATA 12

/*

Resource metadata format (little-endian):

0 uint32 esdSignature; // Serialized byte sequence equals "RES", "ANI", "BMP" or "FNT", NUL-terminated
4 uint8 version; // Version of the metadata format
5 uint8 size; // Size of the metadata structure
6 uint8 compression; // The Compression option in ResourceInfo
7 uint8 extLen; // The string length of the complete file extension of the resource
8 uint32 rawSize; // The uncompressed size of the resource, RawSize in ResourceInfo

Type-specific data follows immediately after

*/

/// Read utility
#define ESD_RD32_LE(buffer, offset) ((uint32_t)buffer[offset] | (((uint32_t)buffer[offset + 1]) << 8) | (((uint32_t)buffer[offset + 2]) << 16) | (((uint32_t)buffer[offset + 3]) << 24))
#define ESD_RD16_LE(buffer, offset) ((uint16_t)buffer[offset] | (((uint16_t)buffer[offset + 1]) << 8))
#define ESD_RD32I_LE(buffer, offset) ((int32_t)ESD_RD32_LE(buffer, offset))
#define ESD_RD16I_LE(buffer, offset) ((int16_t)ESD_RD16_LE(buffer, offset))
#define ESD_WR32_LE(buffer, offset, value) \
	do \
	{ \
		buffer[offset] = (uint32_t)(value) & 0xFF; \
		buffer[offset + 1] = (((uint32_t)(value)) >> 8) & 0xFF; \
		buffer[offset + 2] = (((uint32_t)(value)) >> 16) & 0xFF; \
		buffer[offset + 3] = (((uint32_t)(value)) >> 24) & 0xFF; \
	} while (0)
#define ESD_WR16_LE(buffer, offset, value) \
	do \
	{ \
		buffer[offset] = (uint16_t)(value) & 0xFF; \
		buffer[offset + 1] = (((uint16_t)(value)) >> 8) & 0xFF; \
	} while (0)
#define ESD_WR32I_LE(buffer, offset, value) ESD_WR32_LE(buffer, offset, (int32_t)(value))
#define ESD_WR16I_LE(buffer, offset, value) ESD_WR16_LE(buffer, offset, (int16_t)(value))

/// Set utility with logging
#define ESD_METADATA_SET_EX(type, info, member, value, filename, expression) \
	do \
	{ \
		type value_ = (value); \
		if (((info)->member) != (value_)) \
		{ \
			eve_printf_debug("Metadata %s in resource %s changed from %i to %i\n", (#member), (filename) ? (filename) : "NULL", (int)((info)->member), (int)(value_)); \
			expression; \
			(info)->member = (value_); \
		} \
	} while (0)
#define ESD_METADATA_SET(type, info, member, value, filename) ESD_METADATA_SET_EX(type, info, member, value, filename, do {} while (0))

/// A function to load resource data into RAM_G (or to use the resource from flash directly)
/// Returns address in the format as specified by the BITMAP_SOURCE command (see ESD_DL_FLASH_ADDRESS and ESD_DL_RAM_G_ADDRESS macros)
/// Returns the output image format if the resource is an image loaded through the coprocessor
ESD_CORE_EXPORT uint32_t Esd_LoadResourceEx(Esd_ResourceInfo *resourceInfo, uint8_t *metadata, uint32_t *imageFormat);
ESD_CORE_EXPORT uint32_t Esd_LoadResource(Esd_ResourceInfo *resourceInfo, uint32_t *imageFormat);

/// Free a currently loaded resource from RAM_G. Can be used to enforce reloading a resource.
ESD_CORE_EXPORT void Esd_FreeResource(Esd_ResourceInfo *resourceInfo);

/// A function to make fonts persistent in memory by reloading the data if necessary, called during the Update cycle of each frame
ESD_UPDATE(Esd_ResourcePersist, DisplayName = "Persist Resource", Category = EsdUtilities)
ESD_PARAMETER(resourceInfo, Type = Esd_ResourceInfo *)
ESD_CORE_EXPORT void Esd_ResourcePersist(Esd_ResourceInfo *resourceInfo);

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

#ifdef __cplusplus
}
#endif

#endif /* #ifndef ESD_FONTINFO__H */

/* end of file */
