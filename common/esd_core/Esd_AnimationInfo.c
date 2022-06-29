/*
ESD Framework
Copyright (C) 2022  Bridgetek Pte Lte
Author: Jan Boon <jan.boon@kaetemi.be>
*/

#include "Esd_AnimationInfo.h"
#include "Esd_Context.h"

#include "Esd_BitmapInfo.h"

#define ESD_ANIMATIONINFO_DEBUG_DEV

#ifdef ESD_ANIMATIONINFO_DEBUG_DEV
#define esd_animationinfo_printf_dev(fmt, ...) eve_printf(fmt, ##__VA_ARGS__)
#else
#define esd_animationinfo_printf_dev(fmt, ...) \
	do                                         \
	{                                          \
	} while (false)
#endif

#ifndef NDEBUG
#define ESD_ANIMATIONINFO_DEBUG
#endif

#ifdef ESD_ANIMATIONINFO_DEBUG
#define esd_animationinfo_printf(fmt, ...) eve_printf(fmt, ##__VA_ARGS__)
#else
#define esd_animationinfo_printf(fmt, ...) \
	do                                     \
	{                                      \
	} while (false)
#endif

extern ESD_CORE_EXPORT EVE_HalContext *Esd_Host;
extern ESD_CORE_EXPORT Esd_GpuAlloc *Esd_GAlloc;

#define ESD_NUMFRAMES_INVALID (-1)

#define ESD_METADATA_FRAMESPTR (ESD_METADATA_DATA + 0)
#define ESD_METADATA_FRAMESSIZE (ESD_METADATA_DATA + 4)
#define ESD_METADATA_ATLASBITMAPSOURCE (ESD_METADATA_DATA + 8)
#define ESD_METADATA_ATLASSIZE (ESD_METADATA_DATA + 12)
#define ESD_METADATA_RECTX (ESD_METADATA_DATA + 16)
#define ESD_METADATA_RECTY (ESD_METADATA_DATA + 18)
#define ESD_METADATA_RECTWIDTH (ESD_METADATA_DATA + 20)
#define ESD_METADATA_RECTHEIGHT (ESD_METADATA_DATA + 22)
#define ESD_METADATA_END (ESD_METADATA_DATA + 24)

/*

Animation metadata format (little-endian):

0 uint32 framesPtr; // The address of the frames file that the object file was built with
4 uint32 framesSize; // Size of the display list frames file, if 0 both frames and atlas files are ignored
8 uint32 atlasBitmapSource; // The address in bitmap source display list format of the atlas file that the frames file was built with
12 uint32 atlasSize; // Size of the ASTC bitmap atlas file, if 0 the atlas file is ignored
16 int16 rectX; // Display list bounds of the display list frames
18 int16 rectY; // Display list bounds of the display list frames
20 int16 rectWidth; // Display list bounds of the display list frames
22 int16 rectHeight; // Display list bounds of the display list frames

esdSignature must match "ANI"
version must be 1
size must match ESD_METADATA_END

*/

ESD_CORE_EXPORT uint32_t Esd_LoadAnimation(Esd_AnimationInfo *animationInfo)
{
#ifdef EVE_FLASH_AVAILABLE
	EVE_HalContext *phost = Esd_GetHost();
	Esd_GpuAlloc *ga = Esd_GAlloc;
	(void)phost;

	if (!animationInfo)
	{
		esd_animationinfo_printf("Animation info is NULL\n");
		return GA_INVALID;
	}

	// If the signature was invalid before, don't try again
	if (animationInfo->NumFrames < 0)
		return GA_INVALID;

	// Load the header object resource
	uint8_t metadata[ESD_METADATA_MAX];
	uint32_t objectAddress = Esd_LoadResourceEx(&animationInfo->Object, metadata, NULL);
	if (objectAddress == GA_INVALID)
	{
		esd_animationinfo_printf("Animation header object resource cannot be loaded\n");
		return GA_INVALID;
	}

	// If this object was already validated before, just return as-is
	if (animationInfo->NumFrames > 0 && !metadata[ESD_METADATA_SIGNATURE])
		return objectAddress;

	esd_animationinfo_printf_dev("[DEVELOPMENT] Load animation\n");

	// Some space to read from flash
	Esd_GpuHandle handle;
	uint32_t readAddr;
	if (ESD_DL_IS_FLASH_ADDRESS(objectAddress))
	{
		handle = Esd_GpuAlloc_Alloc(Esd_GAlloc, 64, GA_FIXED_FLAG | GA_GC_FLAG);
		readAddr = Esd_GpuAlloc_Get(Esd_GAlloc, handle);
		if (readAddr == GA_INVALID)
		{
			esd_animationinfo_printf("Unable to validate animation, out of memory\n");
			return GA_INVALID;
		}
	}
	else
	{
		handle = GA_HANDLE_INVALID;
		readAddr = objectAddress;
	}

	// Read header
	if (ESD_DL_IS_FLASH_ADDRESS(objectAddress))
	{
		if (!EVE_CoCmd_flashRead_flush(phost, readAddr, ESD_DL_GET_FLASH_ADDRESS(objectAddress), 8))
		{
			esd_animationinfo_printf("Failed to read animation header from flash\n");
			Esd_GpuAlloc_Free(ga, handle);
			return GA_INVALID;
		}
	}
	uint32_t signature = EVE_Hal_rd32(phost, readAddr);
	int32_t numFrames = EVE_Hal_rd32(phost, readAddr + 4);
	if (signature != 0xAAAA0100)
	{
		esd_animationinfo_printf("Animation signature is invalid\n");
		animationInfo->NumFrames = ESD_NUMFRAMES_INVALID;
		Esd_GpuAlloc_Free(ga, handle);
		return GA_INVALID;
	}

	// No longer need the reading buffer
	Esd_GpuAlloc_Free(ga, handle);

#ifndef ESD_LITTLEFS_FLASH
	if (ESD_DL_IS_FLASH_ADDRESS(objectAddress))
	{
#if 0
		// For testing, fill in metadata based on known data
		ESD_WR32_LE(metadata, ESD_METADATA_SIGNATURE, 0x494E41);
		ESD_WR32_LE(metadata, ESD_METADATA_FRAMESADDRESS, animationInfo->Frames.FlashAddress);
		ESD_WR32_LE(metadata, ESD_METADATA_FRAMESSIZE, animationInfo->Frames.RawSize);
		ESD_WR32_LE(metadata, ESD_METADATA_ATLASBITMAPSOURCE, ESD_DL_FLASH_ADDRESS(animationInfo->Atlas.FlashAddress));
		ESD_WR32_LE(metadata, ESD_METADATA_ATLASSIZE, animationInfo->Atlas.RawSize);
		ESD_WR16_LE(metadata, ESD_METADATA_RECTX, animationInfo->Rect.X);
		ESD_WR16_LE(metadata, ESD_METADATA_RECTY, animationInfo->Rect.Y);
		ESD_WR16_LE(metadata, ESD_METADATA_RECTWIDTH, animationInfo->Rect.Width);
		ESD_WR16_LE(metadata, ESD_METADATA_RECTHEIGHT, animationInfo->Rect.Height);
#else
		esd_animationinfo_printf_dev("[DEVELOPMENT] Animation assumed OK!\n");
		animationInfo->NumFrames = numFrames;
		return objectAddress;
#endif
	}
#endif

	// Additional checks and processing if we have the ESD metadata, otherwise lazily assume OK
	uint32_t esdSignature = ESD_RD32_LE(metadata, ESD_METADATA_SIGNATURE);
	esd_animationinfo_printf_dev("[DEVELOPMENT] Signature 0x%x!\n", (int)esdSignature);
	if (esdSignature == ESD_RESOURCE_ANI)
	{
		esd_animationinfo_printf_dev("[DEVELOPMENT] Animation ESD header found!\n");

		uint32_t framesPtr = ESD_RD32_LE(metadata, ESD_METADATA_FRAMESPTR);
		uint32_t framesSize = ESD_RD32_LE(metadata, ESD_METADATA_FRAMESSIZE);
		uint32_t atlasBitmapSource = ESD_RD32_LE(metadata, ESD_METADATA_ATLASBITMAPSOURCE);
		uint32_t atlasSize = ESD_RD32_LE(metadata, ESD_METADATA_ATLASSIZE);

		ESD_METADATA_SET(int16_t, animationInfo, Rect.X, ESD_RD16I_LE(metadata, ESD_METADATA_RECTX), animationInfo->Object.File);
		ESD_METADATA_SET(int16_t, animationInfo, Rect.Y, ESD_RD16I_LE(metadata, ESD_METADATA_RECTY), animationInfo->Object.File);
		ESD_METADATA_SET(int16_t, animationInfo, Rect.Width, ESD_RD16I_LE(metadata, ESD_METADATA_RECTWIDTH), animationInfo->Object.File);
		ESD_METADATA_SET(int16_t, animationInfo, Rect.Height, ESD_RD16I_LE(metadata, ESD_METADATA_RECTHEIGHT), animationInfo->Object.File);

		if (framesSize)
		{
			// Load the frames object resource
			ESD_METADATA_SET(uint32_t, &animationInfo->Frames, RawSize, framesSize, animationInfo->Frames.File);
			uint32_t framesAddr = Esd_LoadResourceEx(&animationInfo->Frames, NULL, NULL);
			if (framesAddr == GA_INVALID)
			{
				esd_animationinfo_printf("Animation display list frames resource cannot be loaded\n");
				animationInfo->NumFrames = ESD_NUMFRAMES_INVALID;
				return GA_INVALID;
			}

			// Both header and display lists must be in either flash or RAM_G together
			if (ESD_DL_IS_FLASH_ADDRESS(framesAddr) != ESD_DL_IS_FLASH_ADDRESS(objectAddress))
			{
				esd_animationinfo_printf("Both header and display lists must be in either flash or RAM_G together, failed to load animation\n");
				animationInfo->NumFrames = ESD_NUMFRAMES_INVALID;
				return GA_INVALID;
			}

			// Verify that the actual location matches the known location in the header
			uint32_t actualFramesPtr = ESD_DL_IS_FLASH_ADDRESS(framesAddr) ? ESD_DL_GET_FLASH_ADDRESS(framesAddr) : framesAddr;
			if (framesPtr != actualFramesPtr)
			{
				esd_animationinfo_printf("Animation display list frames resource is not in the expected location, runtime remapping of header object is not yet implemented\n");
				animationInfo->NumFrames = ESD_NUMFRAMES_INVALID;
				return GA_INVALID;
			}

			// OK
			esd_animationinfo_printf_dev("[DEVELOPMENT] Frames resource OK!\n");

			if (atlasSize)
			{
				// Load the atlas object resource
				ESD_METADATA_SET(uint32_t, &animationInfo->Atlas, RawSize, atlasSize, animationInfo->Atlas.File);
				uint32_t atlasAddr = Esd_LoadResourceEx(&animationInfo->Atlas, NULL, NULL);
				if (atlasAddr == GA_INVALID)
				{
					esd_animationinfo_printf("Animation ASTC bitmap atlas resource cannot be loaded\n");
					animationInfo->NumFrames = ESD_NUMFRAMES_INVALID;
					return GA_INVALID;
				}

				// Verify that the actual location matches the known location in the header
				if (atlasAddr != atlasBitmapSource)
				{
					esd_animationinfo_printf("Animation ASTC bitmap atlas resource is not in the expected location, runtime remapping of display list frames is not yet implemented\n");
					animationInfo->NumFrames = ESD_NUMFRAMES_INVALID;
					return GA_INVALID;
				}

				// OK
				esd_animationinfo_printf_dev("[DEVELOPMENT] Atlas resource OK!\n");
			}
		}
	}
	else
	{
		// No ESD header
		esd_animationinfo_printf_dev("[DEVELOPMENT] Animation ESD header HAS NOT BEEN found!\n");
		animationInfo->NumFrames = ESD_NUMFRAMES_INVALID;
		return GA_INVALID;
	}

	// OK
	esd_animationinfo_printf_dev("[DEVELOPMENT] Animation load OK!\n");
	animationInfo->NumFrames = numFrames;
	return objectAddress;
#else
	eve_printf("Animation not available on this target platform\n");
	return GA_INVALID;
#endif
}

ESD_CORE_EXPORT void Esd_FreeAnimation(Esd_AnimationInfo *animationInfo)
{
	Esd_FreeResource(&animationInfo->Object);
	Esd_FreeResource(&animationInfo->Frames);
	Esd_FreeResource(&animationInfo->Atlas);
	animationInfo->NumFrames = 0;
}

/* end of file */
