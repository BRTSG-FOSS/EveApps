/*
Bitmap info structure
*/

#ifndef ESD_BITMAPINFO__H
#define ESD_BITMAPINFO__H

#include "Ft_Esd.h"
#include "ESD_GpuAlloc.h"
#include "ESD_ResourceInfo.h"

ESD_TYPE(ESD_BitmapInfo, Native = Struct) // TODO: Struct support, expose values
typedef struct ESD_BitmapInfo
{
	ft_int32_t Width;
	ft_int32_t Height;
	ft_int32_t Stride;
	ft_uint32_t Format;
	ft_int32_t Size;
	union
	{
		const char *File;
		ft_int32_t FlashAddress;
	};

	// (Runtime) Handle pointing to the address in RAM_G if it is allocated
	ESD_GpuHandle GpuHandle;

	// (Runtime) Bitmap handle that is being used
	ft_uint32_t BitmapHandle;

	// Used for paletted format
	union
	{
		const char *PaletteFile;
		ft_int32_t PaletteFlashAddress;
	};
	ESD_GpuHandle PaletteGpuHandle;

	// Used for DXT1 format
	union
	{
		const char *AdditionalFile;
		ft_int32_t AdditionalFlashAddress;
	};
	struct ESD_BitmapInfo *AdditionalInfo;

	// Number of cells usable by the user. There may be additional internally used cells after this
	ft_uint16_t Cells;

#if (EVE_SUPPORT_CHIPID >= EVE_BT815)
	// BT815 bitmap swizzle
	ft_uint16_t SwizzleB : 3;
	ft_uint16_t SwizzleG : 3;
	ft_uint16_t SwizzleR : 3;
	ft_uint16_t SwizzleA : 3;

	// Use swizzle
	ft_bool_t Swizzle : 1;
#endif

	// Load file from compressed format using inflate
	ft_bool_t Compressed : 1;

	// When this is set, the allocated ram is not free'd automatically
	// Use ESD_GpuAlloc_Free(GpuAlloc, GpuHandle) to free the GPU ram manually
	ft_bool_t Persistent : 1;

	// Load from flash. Structure has flash addresses set, rather than file names
	ft_bool_t Flash : 1;

	// Prefer loading the bitmap into RAM_G, even if it's an ASTC stored on Flash
	ft_bool_t PreferRam : 1;

	// Load image using coprocessor (for JPEG and PNG)
	ft_bool_t CoLoad : 1;

} ESD_BitmapInfo;

ESD_TYPE(ESD_BitmapInfo *, Native = Pointer, Edit = Library)

ESD_TYPE(ESD_BitmapCell, Native = Struct, Edit = Library) // TODO: Struct support, expose values
typedef struct
{
	ESD_BitmapInfo *Info;
	ft_int32_t Cell;
} ESD_BitmapCell;

ESD_TYPE(ESD_BitmapCell *, Native = Pointer, Edit = Library)

/// A function to load bitmap data(not including palette data) into RAM_G
ESD_FUNCTION(ESD_LoadBitmap, Type = ft_uint32_t, Include = "ESD_BitmapInfo.h", DisplayName = "Load Bitmap to RAM_G", Category = EsdUtilities)
ESD_PARAMETER(bitmapInfo, Type = ESD_BitmapInfo *)
ft_uint32_t ESD_LoadBitmap(ESD_BitmapInfo *bitmapInfo);

/// A function to load palette data of bitmap into RAM_G
ESD_FUNCTION(ESD_LoadPalette, Type = ft_uint32_t, Include = "ESD_BitmapInfo.h", DisplayName = "Load Palette to RAM_G", Category = EsdUtilities)
ESD_PARAMETER(bitmapInfo, Type = ESD_BitmapInfo *)
ft_uint32_t ESD_LoadPalette(ESD_BitmapInfo *bitmapInfo);

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
#define JPEG 0x91
#define PNG 0x92
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
ESD_FUNCTION(ESD_SwitchBitmapCell, Type = ESD_BitmapCell, DisplayName = "Switch Bitmap Cell", Category = EsdUtilities)
ESD_PARAMETER(BitmapInfo, Type = ESD_BitmapCell, DisplayName = "Bitmap Cell")
ESD_PARAMETER(Cell, Type = ft_uint16_t, DisplayName = "Cell")
ESD_BitmapCell ESD_SwitchBitmapCell(ESD_BitmapCell bitmapCell, ft_uint16_t cell);

/// A function to make bitmap persistent in memory by reloading the data if necessary, called during the Update cycle of each frame
ESD_UPDATE(ESD_BitmapPersist, DisplayName = "Persist Bitmap", Category = EsdUtilities)
ESD_PARAMETER(bitmapCell, Type = ESD_BitmapCell)
void ESD_BitmapPersist(ESD_BitmapCell bitmapCell);

/// A function to get bitmap information structure from bitmap cell
ESD_FUNCTION(ESD_BitmapCell_GetInfo, Type = ESD_BitmapInfo *, DisplayName = "ESD BitmapCell GetInfo", Category = EsdUtilities, Inline)
ESD_PARAMETER(bitmapCell, Type = ESD_BitmapCell)
static inline ESD_BitmapInfo *ESD_BitmapCell_GetInfo(ESD_BitmapCell bitmapCell)
{
	return bitmapCell.Info;
}

/// A function to get the bitmap width from bitmap information. Returns width of 1 pixel for NULL bitmapInfo
ESD_FUNCTION(ESD_BitmapInfo_GetWidth, Type = ft_int32_t, DisplayName = "ESD BitmapInfo GetWidth", Category = EsdUtilities, Inline)
ESD_PARAMETER(bitmapInfo, Type = ESD_BitmapInfo *)
static inline ft_int32_t ESD_BitmapInfo_GetWidth(ESD_BitmapInfo *bitmapInfo)
{
	if (!bitmapInfo)
		return 1;
	return bitmapInfo->Width;
}

/// A function to get the bitmap height from bitmap information. Returns height of 1 pixel for NULL bitmapInfo
ESD_FUNCTION(ESD_BitmapInfo_GetHeight, Type = ft_int32_t, DisplayName = "ESD BitmapInfo GetHeight", Category = EsdUtilities, Inline)
ESD_PARAMETER(bitmapInfo, Type = ESD_BitmapInfo *)
static inline ft_int32_t ESD_BitmapInfo_GetHeight(ESD_BitmapInfo *bitmapInfo)
{
	if (!bitmapInfo)
		return 1;
	return bitmapInfo->Height;
}

#endif /* #ifndef ESD_BITMAPINFO_H */

/* end of file */
