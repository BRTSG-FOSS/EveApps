/*
ESD Framework
Copyright (C) 2022  Bridgetek Pte Lte
Author: Jan Boon <jan.boon@kaetemi.be>
*/

/*
Animation info structure
*/

#ifndef ESD_ANIMATIONINFO__H
#define ESD_ANIMATIONINFO__H

#include "Esd_Base.h"
#include "Esd_ResourceInfo.h"
#include "Esd_Math.h"

#ifdef __cplusplus
extern "C" {
#endif

ESD_TYPE(Esd_AnimationInfo, Native = Struct)
typedef struct Esd_AnimationInfo
{
	// Animation header object resource
	Esd_ResourceInfo Object;

	// Animation display list frames resource
	Esd_ResourceInfo Frames;

	// Animation ASTC bitmap atlas resource
	Esd_ResourceInfo Atlas;

	// (Runtime) Number of frames in the animation (from the animation header)
	int32_t NumFrames;

	// Display bounds of the animation
	Esd_Rect16 Rect;

} Esd_AnimationInfo;

ESD_TYPE(Esd_AnimationInfo *, Native = Pointer, Edit = Library)

/// Loads address of an animation object when using flash filesystem
/// Returns address in the format as specified by the BITMAP_SOURCE command (see ESD_DL_FLASH_ADDRESS and ESD_DL_RAM_G_ADDRESS macros)
/// Use `EVE_CoCmd_animStart` with ESD_DL_GET_FLASH_ADDRESS or `EVE_CoCmd_animStartRam` depending on ESD_DL_IS_FLASH_ADDRESS
/// Returns GA_INVALID if the animation cannot be loaded
ESD_CORE_EXPORT uint32_t Esd_LoadAnimation(Esd_AnimationInfo *animationInfo);

/// Remove any allocated RAM_G used by an animation
/// Reset cached addresses of an animation when using flash filesystem
ESD_CORE_EXPORT void Esd_FreeAnimation(Esd_AnimationInfo *animationInfo);

/// A function to get the number of frames in an animation. Returns 0 if the animation passed is NULL
ESD_FUNCTION(Esd_AnimationInfo_NumFrames, Type = int32_t, DisplayName = "ESD AnimationInfo GetNumFrames", Category = EsdUtilities, Inline)
ESD_PARAMETER(animationInfo, Type = Esd_AnimationInfo *)
static inline int32_t Esd_AnimationInfo_NumFrames(Esd_AnimationInfo *animationInfo)
{
	if (!animationInfo)
		return 0;
	return animationInfo->NumFrames;
}

#ifdef __cplusplus
}
#endif

#endif /* #ifndef ESD_FONTINFO__H */

/* end of file */
