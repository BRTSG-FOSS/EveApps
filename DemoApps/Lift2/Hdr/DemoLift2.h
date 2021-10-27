#ifndef SET00_H_
#define SET00_H_

#if defined(EVE_SUPPORT_UNICODE)

#define AVI_HEADER_SIZE (0xE4)
//#define AVI_HEADER_SIZE (0x26FE)
//#define VIDEO_BUFFER_SIZE (25*1024)

#define DEMO_ASSETS_RAM_OFFSET (828658)//751744 /*800*1024*/ /*828658*/) //location where all assets are placed at the end of the ram
#define VIDEO_RING_BUFFER_SIZE (16*1024)
#define AUDIO_RING_BUFFER_SIZE (8*1024)
#define VIDEO_BUFFER_SIZE VIDEO_RING_BUFFER_SIZE
#define AUDIO_FILES_QUEUE_SIZE (5)
#define AUDIO_FILE_NAME_SIZE (16)

#define LIFTAPPMAXCHARS (12)
typedef struct S_LiftAppFont
{
	uint32_t MaxWidth;
	uint32_t MaxHeight;
	uint32_t Stride;
	uint8_t Format;
	uint8_t Width[LIFTAPPMAXCHARS];
} S_LiftAppFont_t;

/* bitmap structure to contain both for legacy and astc */
typedef struct 
{
	/* below in terms of pixel resolution */
	uint16_t layoutstride;
	uint16_t layoutheight;
	uint16_t displaywidth;
	uint16_t displayheight;
	uint16_t swizzle;
	uint32_t format;
	uint8_t handle;
	uint32_t addr;//for flash its 0x800000 + flashaddress/32
	uint8_t maxnumber;
}LiftBitmap_t;

/* Structure to for xfont */
typedef struct AppxFont_t
{
	Gpu_FontsExt_t *pfontext;
	int32_t *pglyphoffsets;
	int32_t *pwidthoffsets;
}AppxFont_t;

int32_t AppLift2_LoadAssetsFromFlash(EVE_HalContext *pHalContext, LiftBitmap_t *pbitmap, uint8_t bitmaphandle);
int32_t AppLift2_FontString(EVE_HalContext *pHalContext, wchar_t *pstring, AppxFont_t *pappxfont, uint8_t fonthandle, int32_t x, int32_t y);
void DemoLift2_InitialVideoSetupFlash(EVE_HalContext *pHalContext, LiftBitmap_t *paviframe);
bool Lift_IsVideoEnded();
bool AppLift2_IsAudioFinished();
void DemoLift2(EVE_HalContext *ph);

#endif /* EVE_SUPPORT_UNICODE */
#endif /* SET00_H_ */
