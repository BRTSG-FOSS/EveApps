
#include "Esd_CoWidget.h"
#include "Esd_Context.h"

void Esd_CoWidget_Spinner(void *owner, int16_t x, int16_t y, uint16_t style, uint16_t scale)
{
	Esd_CurrentContext->CmdOwner = owner;
	EVE_CoDl_vertexFormat(Esd_GetHost(), 4);
	EVE_CoCmd_spinner(Esd_GetHost(), x, y, style, scale);
}

void Esd_CoWidget_ScreenSaver(void *owner)
{
	Esd_CurrentContext->CmdOwner = owner;
	EVE_CoCmd_screenSaver(Esd_GetHost());
}

void Esd_CoWidget_Sketch(void *owner, Esd_Rect16 globalRect, uint32_t addr, uint16_t format)
{
	Esd_CurrentContext->CmdOwner = owner;
	EVE_CoCmd_sketch(Esd_GetHost(), globalRect.X, globalRect.Y, globalRect.Width, globalRect.Height, addr, format);
}

void Esd_CoWidget_Stop(void *owner)
{
	if ((owner == Esd_CurrentContext->CmdOwner) || !owner)
	{
		EVE_CoCmd_stop(Esd_GetHost());
		Esd_CurrentContext->CmdOwner = NULL;
	}
}

void Esd_CoWidget_PopupSpinner()
{
	Esd_CurrentContext->SpinnerPopup = true;
}

void Esd_CoWidget_StopBgVideo()
{
#ifdef EVE_SUPPORT_VIDEO
	Esd_Context *ec = Esd_CurrentContext;

	if (ec->BgVideoInfo)
	{
		EVE_HalContext *phost = Esd_GetHost();
		Esd_GpuAlloc *ga = Esd_GAlloc;

		EVE_Util_closeFile(phost);
		ec->BgVideoInfo = 0;

		EVE_MediaFifo_close(phost);

		Esd_GpuAlloc_Free(ga, ec->MediaFifoHandle);
		ec->MediaFifoHandle = GA_HANDLE_INVALID;
	}
#endif
}

static void Esd_CoWidget_LoadBgVideoFrame()
{
#ifdef EVE_SUPPORT_VIDEO
	Esd_Context *ec = Esd_CurrentContext;
	EVE_HalContext *phost = Esd_GetHost();
	Esd_GpuAlloc *ga = Esd_GAlloc;

	if (phost->CmdFault)
	{
		eve_printf_debug("Coprocessor fault, stopping background video\n");
		Esd_CoWidget_StopBgVideo();
		return;
	}

	if (ec->BgVideoInfo)
	{
		// eve_assert(phost->LoadFileRemaining);

		/* Get media FIFO address */
		uint32_t fifoAddr = Esd_GpuAlloc_Get(ga, ec->MediaFifoHandle);
		if (fifoAddr == GA_INVALID || fifoAddr != phost->MediaFifoAddress)
		{
			/* Media fifo memory allocation lost, stop the video. */
			eve_printf_debug("Media FIFO lost, stopping background video\n");
			Esd_CoWidget_StopBgVideo();
			return;
		}

		/* Get destination bitmap address */
		uint32_t addr = Esd_GpuAlloc_Get(ga, ec->BgVideoInfo->GpuHandle);
		if (addr == GA_INVALID)
		{
			/* Bitmap memory allocation lost, stop the video. */
			eve_printf_debug("Bitmap lost, stopping background video\n");
			Esd_CoWidget_StopBgVideo();
			return;
		}

		/* Load the next frame */
		uint32_t ptr = fifoAddr + phost->MediaFifoSize;
#if _DEBUG
		uint8_t regDlSwap = EVE_Hal_rd8(phost, REG_DLSWAP);
		eve_assert(regDlSwap == 0);
		eve_assert(addr < RAM_G_SIZE);
		eve_assert(ptr < RAM_G_SIZE);
#endif
		EVE_CoCmd_videoFrame(phost, addr, ptr);
		bool loadRes = EVE_Util_loadMediaFile(phost, NULL, &ec->BgVideoTransfered);
		if (!loadRes)
		{
			/* Video frame failed, stop the video. */
			eve_printf_debug("Video frame failed, stopping background video\n");
			Esd_CoWidget_StopBgVideo();
			if (EVE_Cmd_rp(phost) != EVE_Cmd_wp(phost))
			{
				eve_printf_debug("Coprocessor did not complete command, force fault\n");
				EVE_Util_forceFault(phost, "ESD Core: CMD_VIDEOFRAME aborted");
			}
			return;
		}
#if _DEBUG
		uint32_t lastCmdA = EVE_Hal_rd32(phost, RAM_CMD + ((EVE_Cmd_wp(phost) - 12) & EVE_CMD_FIFO_MASK));
		uint32_t lastCmdB = EVE_Hal_rd32(phost, RAM_CMD + ((EVE_Cmd_wp(phost) - 8) & EVE_CMD_FIFO_MASK));
		uint32_t lastCmdC = EVE_Hal_rd32(phost, RAM_CMD + ((EVE_Cmd_wp(phost) - 4) & EVE_CMD_FIFO_MASK));
		regDlSwap = EVE_Hal_rd8(phost, REG_DLSWAP);
		eve_assert(regDlSwap == 0);
#endif

		uint32_t moreFrames = EVE_Hal_rd32(phost, ptr);
		if (!moreFrames)
		{
			/* End of video, stop */
			/* TODO: Add a looping flag for background video? */
			/* TODO: Add a media fifo lock flag to avoid background video interruption? */
			eve_printf_debug("Background video completed\n");
			Esd_CoWidget_StopBgVideo();
		}

		if (!phost->LoadFileRemaining)
		{
			/* End of video, stop */
			eve_printf_debug("Background video completed, end of file\n");
			Esd_CoWidget_StopBgVideo();
		}
	}
#endif
}

void Esd_CoWidget_Render()
{
	Esd_CoWidget_LoadBgVideoFrame();
}

/* Plays the specified Esd_BitmapInfo video in the background.
Video only, not applicable to bitmap cell animation. 
Only one video can play in the background at a time. 
Background play will be interrupted when any other video is started, and cannot be resumed. */
bool Esd_CoWidget_PlayBgVideo(Esd_BitmapCell video)
{
#ifdef EVE_SUPPORT_VIDEO
	Esd_CoWidget_StopBgVideo();

	/* TODO: Flash BgVideo */

	Esd_BitmapInfo *info = video.Info;
	if (!info->Video && info->Format != AVI)
	{
		eve_printf_debug("Not a video bitmap info object\n");
		return false;
	}

	Esd_Context *ec = Esd_CurrentContext;
	EVE_HalContext *phost = Esd_GetHost();
	Esd_GpuAlloc *ga = Esd_GAlloc;

	if (phost->CmdFault)
		return false;

	/* Allocate RAM_G space for the bitmap, if necessary */
	uint32_t addr = Esd_GpuAlloc_Get(ga, info->GpuHandle);
	bool addrAllocated = (addr == GA_INVALID);
	if (addrAllocated)
	{
		info->GpuHandle = Esd_GpuAlloc_Alloc(Esd_GAlloc, info->Size, GA_GC_FLAG);
		addr = Esd_GpuAlloc_Get(ga, info->GpuHandle);
		if (addr == GA_INVALID)
		{
			eve_printf_debug("Not enough RAM_G available to allocate video bitmap\n");
			return false;
		}
	}

	/* Allocate RAM_G space for FIFO and completion pointer */
	uint32_t fifoSize = 16 * 1024; /* TODO: What's an ideal FIFO size? */
	Esd_GpuHandle fifoHandle = Esd_GpuAlloc_Alloc(Esd_GAlloc, fifoSize + 4, GA_FIXED_FLAG);
	uint32_t fifoAddr = Esd_GpuAlloc_Get(Esd_GAlloc, fifoHandle);
	if (fifoAddr == GA_INVALID)
	{
		eve_printf_debug("Not enough RAM_G available to allocate media FIFO for video\n");
		if (addrAllocated)
		{
			Esd_GpuAlloc_Free(ga, info->GpuHandle);
			info->GpuHandle = GA_HANDLE_INVALID;
		}
		return false;
	}

	/* Setup media FIFO */
	if (!EVE_MediaFifo_set(phost, fifoAddr, fifoSize))
	{
		if (addrAllocated)
		{
			Esd_GpuAlloc_Free(ga, info->GpuHandle);
			info->GpuHandle = GA_HANDLE_INVALID;
		}
		Esd_GpuAlloc_Free(Esd_GAlloc, fifoHandle);
		return false;
	}

	/* Load the first video frame */
	uint32_t transfered = 0;
	uint32_t ptr = fifoAddr + fifoSize;
#if _DEBUG
	uint8_t regDlSwap = EVE_Hal_rd8(phost, REG_DLSWAP);
	eve_assert(regDlSwap == 0);
	eve_assert(addr < RAM_G_SIZE);
	eve_assert(ptr < RAM_G_SIZE);
#endif
	EVE_CoCmd_videoStart(phost);
	EVE_CoCmd_videoFrame(phost, addr, ptr);
	if (EVE_Util_loadMediaFile(phost, info->File, &transfered))
	{
		/* Store */
		ec->BgVideoInfo = info;
		ec->BgVideoTransfered = transfered;
		ec->MediaFifoHandle = fifoHandle;
#if _DEBUG
		regDlSwap = EVE_Hal_rd8(phost, REG_DLSWAP);
		eve_assert(regDlSwap == 0);
#endif
		return true;
	}
	else
	{
		/* Release */
		EVE_Util_closeFile(phost);
		EVE_MediaFifo_close(phost);
		Esd_GpuAlloc_Free(Esd_GAlloc, fifoHandle);
		if (addrAllocated)
		{
			Esd_GpuAlloc_Free(ga, info->GpuHandle);
			info->GpuHandle = GA_HANDLE_INVALID;
		}
		if (EVE_Cmd_rp(phost) != EVE_Cmd_wp(phost))
		{
			eve_printf_debug("Coprocessor did not complete command, force fault\n");
			EVE_Util_forceFault(phost, "ESD Core: CMD_VIDEOSTART and CMD_VIDEOFRAME aborted");
		}
#if _DEBUG
		regDlSwap = EVE_Hal_rd8(phost, REG_DLSWAP);
		eve_assert(regDlSwap == 0);
#endif
		return false;
	}
#endif
}

bool Esd_CoWidget_PlayVideoFile(const char *filename, uint32_t options)
{
	Esd_Context *ec = Esd_CurrentContext;
	EVE_HalContext *phost = Esd_GetHost();
	Esd_GpuAlloc *ga = &ec->GpuAlloc;

	if (phost->CmdFault)
		return false;

	/* Trash all memory */
	Esd_GpuAlloc_Reset(ga);
	Esd_BitmapHandle_Reset(&ec->HandleState);

	/* FIFO at end of RAM_G */
	uint32_t fifoSize = 16 * 1024; /* TODO: What's an ideal FIFO size? */
	uint32_t fifoAddr = RAM_G_SIZE - fifoSize;

	EVE_MediaFifo_set(phost, fifoAddr, fifoSize);
	EVE_CoCmd_playVideo(phost, options | OPT_MEDIAFIFO /*| OPT_FULLSCREEN*/);

	//uint32_t transfered = 0;
	//bool res = EVE_Util_loadMediaFile(phost, filename, &transfered);
	//EVE_Util_closeFile(phost);

	bool res = EVE_Util_loadMediaFile(phost, filename, NULL);

	if (EVE_Cmd_rp(phost) != EVE_Cmd_wp(phost))
	{
		eve_printf_debug("Coprocessor did not complete command, force fault\n");
		EVE_Util_forceFault(phost, "ESD Core: CMD_PLAYVIDEO aborted");
		res = false;
	}

	EVE_CoCmd_stop(phost);
	EVE_MediaFifo_close(phost);

	return res;
}

/* end of file */
