
#include <stdio.h>
#include <Esd_Core.h>

static Esd_Context s_Host;

/*

Platform limitations:
- When loading a video, we can easily use CMD_VIDEOFRAME to load the first frame as if it's a bitmap.
- The same command can be used to play back a video, only one video resource can be playing at once.
- When another video wants a frame, the playing video must be stopped.
- Other than that, one video can be played fullscreen with audio using CMD_PLAYVIDEO. 
This will cause the coprocessor to take over the display list and RAM_G, so flip to a CLEAR color frame before playing.

*/

/* X:\source\eve_apps\EveApps\SampleApp\Test */
/* On FT800, ESD core will simply not show the video */
#if EVE_SUPPORT_GEN > EVE1
Esd_BitmapInfo TestVideo_Info = {
	.Width = 854,
	.Height = 480,
	.Format = AVI,
	.Stride = 854 * 2, /* Width * 2 */
	.Size = 854 * 2 * 480, /* Stride * Height */
	.File = "chickens-4.avi",
	.GpuHandle = GA_HANDLE_INIT,
	.BitmapHandle = ~0,
};
#else
Esd_BitmapInfo TestVideo_Info = {
	.Width = 256,
	.Height = 144,
	.Format = AVI,
	.Stride = 256 * 2, /* Width * 2 */
	.Size = 256 * 2 * 144, /* Stride * Height */
	.File = "bbb_lo.avi",
	.GpuHandle = GA_HANDLE_INIT,
	.BitmapHandle = ~0,
};
#endif

Esd_BitmapCell TestVideo()
{
	return (Esd_BitmapCell){
		.Info = &TestVideo_Info
	};
}

Esd_TouchTag s_PlayVideoTag;
Esd_TouchTag s_PlayBackgroundTag;
Esd_TouchTag s_ResetBitmapHandlesTag;
Esd_TouchTag s_ResetGpuAllocTag;
uint32_t s_Frame = 0;

void PlayVideoTag_Tap(void *context)
{
	eve_printf_debug("Play video!\n");
	Esd_CoWidget_PlayVideoFile(TestVideo().Info->File, OPT_FULLSCREEN | OPT_SOUND);
}

bool s_SuppressOther = false;

void PlayBackgroundTag_Tap(void *context)
{
	eve_printf_debug("Play background!\n");
	// s_SuppressOther = true; // TEST: BT815 and BT817 fail on VIDEOFRAME when widgets are rendered inbetween
	Esd_CoWidget_PlayBgVideo(TestVideo());
}

void ResetBitmapHandlesTag_Tap(void *context)
{
	Esd_BitmapHandle_Reset(&Esd_CurrentContext->HandleState);
}

void ResetGpuAllocTag_Tap(void *context)
{
	Esd_GpuAlloc_Reset(&Esd_CurrentContext->GpuAlloc);
}

void Main_Start(void *context)
{
	eve_printf_debug("Start!\n");

	Esd_TouchTag__Initializer(&s_PlayVideoTag);
	s_PlayVideoTag.Tap = PlayVideoTag_Tap;
	Esd_TouchTag_Start(&s_PlayVideoTag);

	Esd_TouchTag__Initializer(&s_PlayBackgroundTag);
	s_PlayBackgroundTag.Tap = PlayBackgroundTag_Tap;
	Esd_TouchTag_Start(&s_PlayBackgroundTag);

	Esd_TouchTag__Initializer(&s_ResetBitmapHandlesTag);
	s_ResetBitmapHandlesTag.Tap = ResetBitmapHandlesTag_Tap;
	Esd_TouchTag_Start(&s_ResetBitmapHandlesTag);

	Esd_TouchTag__Initializer(&s_ResetGpuAllocTag);
	s_ResetGpuAllocTag.Tap = ResetGpuAllocTag_Tap;
	Esd_TouchTag_Start(&s_ResetGpuAllocTag);
}

void Main_Update(void *context)
{
	// EVE_sleep(100);
	Esd_TouchTag_Update(&s_PlayVideoTag);
	Esd_TouchTag_Update(&s_PlayBackgroundTag);
	Esd_TouchTag_Update(&s_ResetBitmapHandlesTag);
	Esd_TouchTag_Update(&s_ResetGpuAllocTag);
}

void Main_Render(void *context)
{
	Esd_Context *ec = Esd_CurrentContext;
	EVE_HalContext *phost = Esd_GetHost();

	Esd_Render_Bitmap(0, 0, TestVideo(), 0xFFFFFFFF);
	if (s_SuppressOther)
		return;

	EVE_CoCmd_text(phost, 5, 5, Esd_CoDl_SetupRomFont(33), 0, "ESD Core Video Test");

	EVE_CoDl_tag(phost, Esd_TouchTag_Tag(&s_PlayVideoTag));
	EVE_CoCmd_button(phost, 5, 100, 150, 25, Esd_CoDl_SetupRomFont(16), Esd_TouchTag_Inside(&s_PlayVideoTag) ? OPT_FLAT : 0, "Play Video");

	EVE_CoDl_tag(phost, Esd_TouchTag_Tag(&s_PlayBackgroundTag));
	EVE_CoCmd_button(phost, 160, 100, 150, 25, Esd_CoDl_SetupRomFont(16), Esd_TouchTag_Inside(&s_PlayBackgroundTag) ? OPT_FLAT : 0, "Play Background");

	EVE_CoDl_tag(phost, Esd_TouchTag_Tag(&s_ResetBitmapHandlesTag));
	EVE_CoCmd_button(phost, 315, 100, 150, 25, Esd_CoDl_SetupRomFont(16), Esd_TouchTag_Inside(&s_ResetBitmapHandlesTag) ? OPT_FLAT : 0, "Reset Handles");

	EVE_CoDl_tag(phost, Esd_TouchTag_Tag(&s_ResetGpuAllocTag));
	EVE_CoCmd_button(phost, 470, 100, 150, 25, Esd_CoDl_SetupRomFont(16), Esd_TouchTag_Inside(&s_ResetGpuAllocTag) ? OPT_FLAT : 0, "Reset RAM_G");

	EVE_CoDl_tag(phost, 255);

	uint8_t font = Esd_CoDl_SetupRomFont(16);
	EVE_CoCmd_text(phost, 245, 140, font, OPT_RIGHTX, "Background Video Transfered:");
#if defined(EVE_SUPPORT_MEDIAFIFO)
	EVE_CoCmd_number(phost, 250, 140, font, 0, ec->BgVideoTransfered);
#else
	EVE_CoCmd_text(phost, 250, 140, font, 0, "Not Supported");
#endif
	EVE_CoCmd_text(phost, 245, 150, font, OPT_RIGHTX, "Background Video Remaining:");
#if defined(EVE_SUPPORT_MEDIAFIFO)
	EVE_CoCmd_number(phost, 250, 150, font, 0, (int32_t)phost->LoadFileRemaining);
#else
	EVE_CoCmd_text(phost, 250, 150, font, 0, "Not Supported");
#endif
	EVE_CoCmd_text(phost, 245, 160, font, OPT_RIGHTX, "Frame:");
	EVE_CoCmd_number(phost, 250, 160, font, 0, (int32_t)s_Frame++);
}

void Main_End(void *context)
{
	Esd_TouchTag_End(&s_PlayVideoTag);
	Esd_TouchTag_End(&s_PlayBackgroundTag);
	Esd_TouchTag_End(&s_ResetBitmapHandlesTag);
	Esd_TouchTag_End(&s_ResetGpuAllocTag);
}

int main(int argc, char *argv[])
{
	Esd_Initialize();

	{
		Esd_Parameters ep;
		Esd_Defaults(&ep);
		ep.Start = Main_Start;
		ep.Update = Main_Update;
		ep.Render = Main_Render;
		ep.End = Main_End;
#ifdef ESD_FLASH_FILES
		for (int i = 0; i < ESD_FLASH_NB; ++i)
			ep.FlashFilePaths[i][0] = '\0'; /* Disable flash */
#endif

		if (!Esd_Open(&s_Host, &ep))
			return EXIT_FAILURE;
	}

	Esd_Loop(&s_Host);
	Esd_Close(&s_Host);

	Esd_Release();
	return EXIT_SUCCESS;
}

/* end of file */
