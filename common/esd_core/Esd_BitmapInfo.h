/*
Bitmap info structure
*/

#ifndef ESD_BITMAPINFO__H
#define ESD_BITMAPINFO__H

#include "Esd_Base.h"
#include "Esd_GpuAlloc.h"
#include "Esd_ResourceInfo.h"

// #define ESD_COMPATIBILITY_ADDITIONALFILE

#ifdef __cplusplus
extern "C" {
#endif

ESD_TYPE(Esd_BitmapInfo, Native = Struct) // TODO: Struct support, expose values
typedef struct Esd_BitmapInfo
{
	int32_t Width;
	int32_t Height;
	int32_t Stride;
	uint32_t Format;
	int32_t Size;
	
#ifndef ESD_LITTLEFS_FLASH
	union
	{
#endif
		int32_t FlashAddress;
		const char *File;
#ifndef ESD_LITTLEFS_FLASH
	};
#endif

	// (Runtime) Handle pointing to the address in RAM_G if it is allocated
	Esd_GpuHandle GpuHandle;

	// (Runtime) Bitmap handle that is being used
	uint32_t BitmapHandle;

	// Used for DXT1 format
	struct Esd_BitmapInfo *AdditionalInfo;

	// Used for paletted format
#ifndef ESD_LITTLEFS_FLASH
	union
	{
#endif
		const char *PaletteFile;
		int32_t PaletteFlashAddress;
#ifndef ESD_LITTLEFS_FLASH
	};
#endif
	Esd_GpuHandle PaletteGpuHandle;
	
	// Used for DXT1 format
	// AdditionalFile and AdditionalFlashAddress are deprecated
	// DXT1 is now loaded from a single file per bitmap resource
#ifdef ESD_COMPATIBILITY_ADDITIONALFILE
#ifndef ESD_LITTLEFS_FLASH
	union
	{
#endif
		const char *AdditionalFile;
		int32_t AdditionalFlashAddress;
#ifndef ESD_LITTLEFS_FLASH
	};
#endif
#endif

	// Number of cells usable by the user. There may be additional internally used cells after this
	uint16_t Cells;

#if (EVE_SUPPORT_CHIPID >= EVE_BT815)
	// BT815 bitmap swizzle
	uint16_t SwizzleB : 3;
	uint16_t SwizzleG : 3;
	uint16_t SwizzleR : 3;
	uint16_t SwizzleA : 3;

	// Use swizzle
	bool Swizzle : 1;
#endif

	// Load file from compressed format using inflate
	bool Compressed : 1;

	// When this is set, the allocated ram is not free'd automatically
	// Use Esd_GpuAlloc_Free(GpuAlloc, GpuHandle) to free the GPU ram manually
	bool Persistent : 1;

	// Load from flash. Structure has flash addresses set, rather than file names, if there is no flash filesystem
	bool Flash : 1;

	// Prefer loading the bitmap into RAM_G, even if it's an ASTC stored on Flash
	bool PreferRam : 1;

	// Load image using coprocessor (for JPEG and PNG)
	// Flagged at runtime whenever Format is JPEG, PNG, or AVI
	// When set, the Format field may be read out from the coprocessor
	bool CoLoad : 1;

	// This is a video (for AVI)
	// Flagged at runtime whenever Format is AVI
	bool Video : 1;
	
} Esd_BitmapInfo;

ESD_TYPE(Esd_BitmapInfo *, Native = Pointer, Edit = Library)

ESD_TYPE(Esd_BitmapCell, Native = Struct, Edit = Library) // TODO: Struct support, expose values
typedef struct
{
	Esd_BitmapInfo *Info;
	int32_t Cell;
} Esd_BitmapCell;

ESD_TYPE(Esd_BitmapCell *, Native = Pointer, Edit = Library)

#define ESD_IS_FORMAT_PALETTED(format) (format >= PALETTED565 && format <= PALETTED8)

/// A function to load bitmap data(not including palette data) into RAM_G
ESD_FUNCTION(Esd_LoadBitmap, Type = uint32_t, Attributes = ESD_CORE_EXPORT, Include = "Esd_BitmapInfo.h", DisplayName = "Load Bitmap to RAM_G", Category = EsdUtilities)
ESD_PARAMETER(bitmapInfo, Type = Esd_BitmapInfo *)
ESD_CORE_EXPORT uint32_t Esd_LoadBitmap(Esd_BitmapInfo *bitmapInfo);

/// A function to load palette data of bitmap into RAM_G
ESD_FUNCTION(Esd_LoadPalette, Type = uint32_t, Attributes = ESD_CORE_EXPORT, Include = "Esd_BitmapInfo.h", DisplayName = "Load Palette to RAM_G", Category = EsdUtilities)
ESD_PARAMETER(bitmapInfo, Type = Esd_BitmapInfo *)
ESD_CORE_EXPORT uint32_t Esd_LoadPalette(Esd_BitmapInfo *bitmapInfo);

ESD_ENUM(_BitmapResourceFormat, DisplayName = "Bitmap Format")
// Hardware bitmap formats
ESD_IDENTIFIER(ARGB1555)
ESD_IDENTIFIER(L1)
ESD_IDENTIFIER(L2)
ESD_IDENTIFIER(L4)
ESD_IDENTIFIER(L8)
ESD_IDENTIFIER(RGB332)
ESD_IDENTIFIER(ARGB2)
ESD_IDENTIFIER(ARGB4)
ESD_IDENTIFIER(RGB565)
ESD_IDENTIFIER(PALETTED565)
ESD_IDENTIFIER(PALETTED4444)
ESD_IDENTIFIER(PALETTED8)
// Specially loaded bitmap formats
#define DXT1 0x81
#define DXT1L2 0x82
#define JPEG 0x91
#define PNG 0x92
#define AVI 0x93
#if (EVE_SUPPORT_CHIPID >= EVE_BT815)
ESD_IDENTIFIER(COMPRESSED_RGBA_ASTC_4x4_KHR)
ESD_IDENTIFIER(COMPRESSED_RGBA_ASTC_5x4_KHR)
ESD_IDENTIFIER(COMPRESSED_RGBA_ASTC_5x5_KHR)
ESD_IDENTIFIER(COMPRESSED_RGBA_ASTC_6x5_KHR)
ESD_IDENTIFIER(COMPRESSED_RGBA_ASTC_6x6_KHR)
ESD_IDENTIFIER(COMPRESSED_RGBA_ASTC_8x5_KHR)
ESD_IDENTIFIER(COMPRESSED_RGBA_ASTC_8x6_KHR)
ESD_IDENTIFIER(COMPRESSED_RGBA_ASTC_8x8_KHR)
ESD_IDENTIFIER(COMPRESSED_RGBA_ASTC_10x5_KHR)
ESD_IDENTIFIER(COMPRESSED_RGBA_ASTC_10x6_KHR)
ESD_IDENTIFIER(COMPRESSED_RGBA_ASTC_10x8_KHR)
ESD_IDENTIFIER(COMPRESSED_RGBA_ASTC_10x10_KHR)
ESD_IDENTIFIER(COMPRESSED_RGBA_ASTC_12x10_KHR)
ESD_IDENTIFIER(COMPRESSED_RGBA_ASTC_12x12_KHR)
#endif
ESD_END()

///  Switch bitmap cell number
ESD_FUNCTION(Esd_BitmapCell_Switched, Type = Esd_BitmapCell, DisplayName = "Switch Bitmap Cell", Category = EsdUtilities, Inline)
ESD_PARAMETER(BitmapInfo, Type = Esd_BitmapCell, DisplayName = "Bitmap Cell")
ESD_PARAMETER(Cell, Type = uint16_t, DisplayName = "Cell")
static inline Esd_BitmapCell Esd_BitmapCell_Switched(Esd_BitmapCell bitmapCell, uint16_t cell)
{
	bitmapCell.Cell = cell;
	return bitmapCell;
}

/// A function to make bitmap persistent in memory by reloading the data if necessary, called during the Update cycle of each frame
ESD_UPDATE(Esd_BitmapCell_Persist, DisplayName = "Persist Bitmap", Category = EsdUtilities)
ESD_PARAMETER(bitmapCell, Type = Esd_BitmapCell)
ESD_CORE_EXPORT void Esd_BitmapCell_Persist(Esd_BitmapCell bitmapCell);

/// A function to get bitmap information structure from bitmap cell
ESD_FUNCTION(Esd_BitmapCell_GetInfo, Type = Esd_BitmapInfo *, DisplayName = "ESD BitmapCell GetInfo", Category = EsdUtilities, Inline)
ESD_PARAMETER(bitmapCell, Type = Esd_BitmapCell)
static inline Esd_BitmapInfo *Esd_BitmapCell_GetInfo(Esd_BitmapCell bitmapCell)
{
	return bitmapCell.Info;
}

/// A function to get the bitmap width from bitmap information. Returns width of 1 pixel for NULL bitmapInfo
ESD_FUNCTION(Esd_BitmapInfo_GetWidth, Type = int32_t, DisplayName = "ESD BitmapInfo GetWidth", Category = EsdUtilities, Inline)
ESD_PARAMETER(bitmapInfo, Type = Esd_BitmapInfo *)
static inline int32_t Esd_BitmapInfo_GetWidth(Esd_BitmapInfo *bitmapInfo)
{
	if (!bitmapInfo)
		return 1;
	return bitmapInfo->Width;
}

/// A function to get the bitmap height from bitmap information. Returns height of 1 pixel for NULL bitmapInfo
ESD_FUNCTION(Esd_BitmapInfo_GetHeight, Type = int32_t, DisplayName = "ESD BitmapInfo GetHeight", Category = EsdUtilities, Inline)
ESD_PARAMETER(bitmapInfo, Type = Esd_BitmapInfo *)
static inline int32_t Esd_BitmapInfo_GetHeight(Esd_BitmapInfo *bitmapInfo)
{
	if (!bitmapInfo)
		return 1;
	return bitmapInfo->Height;
}

#ifdef EVE_FLASH_AVAILABLE
#ifdef ESD_LITTLEFS_FLASH
ESD_CORE_EXPORT int32_t Esd_BitmapInfo_LoadFlashAddress(int32_t *addr, const char *file, uint8_t *metadata);
#else
#define Esd_BitmapInfo_LoadFlashAddress(addr, file, metadata) (*(addr))
#endif
#else
#define Esd_BitmapInfo_LoadFlashAddress(addr, file, metadata) (FA_INVALID)
#endif

#ifdef __cplusplus
}
#endif

#endif /* #ifndef ESD_BITMAPINFO__H */

/* end of file */
