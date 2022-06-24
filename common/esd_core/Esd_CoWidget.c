
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

	if (!ec->BgVideoInfo)
		return;

	if (phost->CmdFault)
	{
		eve_printf_debug("Coprocessor fault, stopping background video\n");
		Esd_CoWidget_StopBgVideo();
		return;
	}

	if (ec->BgVideoInfo->Flash)
	{
#ifdef EVE_FLASH_AVAILABLE
		/* Get completion pointer address */
		uint32_t ptr = Esd_GpuAlloc_Get(ga, ec->MediaFifoHandle);
		if (ptr == GA_INVALID)
		{
			/* Completion pointer lost, stop the video. */
			eve_printf_debug("Completion pointer lost, stopping background video\n");
			Esd_CoWidget_StopBgVideo();
			return;
		}

		/* Check previous completion */
		uint32_t moreFrames = EVE_Hal_rd32(phost, ptr);
		if (!moreFrames)
		{
			/* End of video, stop */
			eve_printf_debug("Background video completed\n");
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

		/* Video frame, no need to wait for completion pointer */
		EVE_CoCmd_videoFrame(phost, addr, ptr);
#endif
	}
	else
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

		uint32_t moreFrames = EVE_Hal_rd32(phost, ptr);
		if (!moreFrames)
		{
			/* End of video, stop */
			/* TODO: Add a looping flag for background video? */
			/* TODO: Add a media fifo lock flag to avoid background video interruption? */
			eve_printf_debug("Background video completed\n");
			Esd_CoWidget_StopBgVideo();
			return;
		}

		if (!phost->LoadFileRemaining)
		{
			/* End of video, stop */
			eve_printf_debug("Background video completed, end of file\n");
			Esd_CoWidget_StopBgVideo();
			return;
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

	Esd_BitmapInfo *info = video.Info;
	if (!info->Video && info->Format != AVI)
	{
		eve_printf_debug("Not a video bitmap info object\n");
		return false;
	}

	Esd_Context *ec = Esd_CurrentContext;
	EVE_HalContext *phost = Esd_GetHost();
	Esd_GpuAlloc *ga = Esd_GAlloc;

	if (!EVE_Hal_supportVideo(phost))
	{
		eve_assert_ex(false, "Esd_CoWidget_PlayBgVideo is not available on the current graphics platform\n");
		return false;
	}

	if (!info->Flash && !EVE_Util_sdCardReady(phost))
	{
		eve_printf_debug("SD card not ready\n");
		return false;
	}

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

	uint32_t ptr;
	uint32_t fifoAddr;
	Esd_GpuHandle fifoHandle;
	if (info->Flash)
	{
#ifdef EVE_FLASH_AVAILABLE
		if (!EVE_Hal_supportFlash(phost))
		{
			eve_printf_debug("This device doesn't support flash for video playback\n");
			return false;
		}
#endif

		/* Allocate RAM_G space for completion pointer */
		fifoHandle = Esd_GpuAlloc_Alloc(Esd_GAlloc, 4, 0);
		ptr = Esd_GpuAlloc_Get(Esd_GAlloc, fifoHandle);
		if (ptr == GA_INVALID)
		{
			return false;
		}
	}
	else
	{
		/* Allocate RAM_G space for FIFO and completion pointer */
		uint32_t fifoSize = 16 * 1024; /* TODO: What's an ideal FIFO size? */
		fifoHandle = Esd_GpuAlloc_Alloc(Esd_GAlloc, fifoSize + 4, GA_FIXED_FLAG);
		fifoAddr = Esd_GpuAlloc_Get(Esd_GAlloc, fifoHandle);
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

		ptr = fifoAddr + fifoSize;
	}

	/* Load the first video frame */
#if defined(_DEBUG) && 1 // DEBUG WORKAROUND CMD_VIDEOFRAME
	uint8_t regDlSwap = EVE_Hal_rd8(phost, REG_DLSWAP);
	eve_assert(regDlSwap == 0);
	eve_assert(addr < RAM_G_SIZE);
	eve_assert(ptr < RAM_G_SIZE);
	EVE_Cmd_waitFlush(phost);
	uint32_t dl0 = EVE_Hal_rd32(phost, REG_CMD_DL);
#endif

	uint16_t bkpCmdDl;
	uint32_t bkpDl[10];

	if (EVE_CHIPID < EVE_BT815)
	{
		if (EVE_Cmd_waitFlush(phost))
		{
			/* Save */
			bkpCmdDl = EVE_Hal_rd16(phost, REG_CMD_DL);
			if (bkpCmdDl)
			{
				for (int i = 0; i < 10; ++i)
					bkpDl[i] = EVE_Hal_rd32(phost, RAM_DL + 4 * i);
			}
		}
		else
		{
			bkpCmdDl = 0;
		}
	}

	bool res;
	if (info->Flash)
	{
#ifdef EVE_FLASH_AVAILABLE
		EVE_CoCmd_flashSource(phost, Esd_BitmapInfo_LoadFlashAddress(&info->FlashAddress, info->File, NULL));
		EVE_CoCmd_videoStartF(phost);
		EVE_CoCmd_videoFrame(phost, addr, ptr);
		res = EVE_Cmd_waitFlush(phost);
#else
		res = false;
#endif
	}
	else
	{
		uint32_t transfered = 0;
		EVE_CoCmd_videoStart(phost);
		EVE_CoCmd_videoFrame(phost, addr, ptr);
		res = EVE_Util_loadMediaFile(phost, info->File, &transfered);
		if (res)
			ec->BgVideoTransfered = transfered;
		else
			EVE_Util_closeFile(phost);
	}

	if (res)
	{
		/* Store */
		ec->BgVideoInfo = info;
		ec->MediaFifoHandle = fifoHandle;
		return true;
	}
	else
	{
		/* Release */
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
	}

	if (EVE_CHIPID < EVE_BT815)
	{
		if (bkpCmdDl && EVE_Cmd_waitFlush(phost))
		{
			/* Restore */
			EVE_Hal_wr16(phost, REG_CMD_DL, bkpCmdDl);
			for (int i = 0; i < 10; ++i)
				EVE_Hal_wr32(phost, RAM_DL + 4 * i, bkpDl[i]);
		}
	}

#if defined(_DEBUG) && 1 // DEBUG WORKAROUND CMD_VIDEOFRAME
	EVE_Cmd_waitFlush(phost);
	uint32_t dl1 = EVE_Hal_rd32(phost, REG_CMD_DL);
	eve_assert(dl0 == dl1);
	regDlSwap = EVE_Hal_rd8(phost, REG_DLSWAP);
	eve_assert(regDlSwap == 0);
#endif

	return res;
#else
	eve_printf_debug("Video is not supported\n");
	return false;
#endif
}

bool Esd_CoWidget_PlayVideoFile(const char *filename, uint16_t options)
{
#ifdef EVE_SUPPORT_VIDEO
	Esd_Context *ec = Esd_CurrentContext;
	EVE_HalContext *phost = Esd_GetHost();
	Esd_GpuAlloc *ga = &ec->GpuAlloc;

	if (!EVE_Hal_supportVideo(phost))
	{
		eve_assert_ex(false, "Esd_CoWidget_PlayVideoFile is not available on the current graphics platform\n");
		return false;
	}

	if (!EVE_Util_sdCardReady(phost))
	{
		eve_printf_debug("SD card not ready\n");
		return false;
	}

	if (phost->CmdFault)
		return false;

	/* Trash all memory */
	Esd_GpuAlloc_Reset(ga);
	Esd_GpuAlloc_Alloc(Esd_GAlloc, RAM_G_SIZE, GA_GC_FLAG); /* Block allocation */
	Esd_BitmapHandle_Reset(&ec->HandleState);

	/* FIFO at end of RAM_G */
	uint32_t fifoSize = 16 * 1024; /* TODO: What's an ideal FIFO size? */
	uint32_t fifoAddr = RAM_G_SIZE - fifoSize;

	EVE_MediaFifo_set(phost, fifoAddr, fifoSize);
	EVE_CoCmd_playVideo(phost, (options | OPT_MEDIAFIFO) & ~(OPT_FLASH | OPT_OVERLAY | OPT_NODL));

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
#else
	eve_printf_debug("Video is not supported\n");
	return false;
#endif
}

bool Esd_CoWidget_PlayVideoFlash(uint32_t addr, uint16_t options)
{
#if defined(EVE_SUPPORT_VIDEO) && defined(EVE_FLASH_AVAILABLE)
	Esd_Context *ec = Esd_CurrentContext;
	EVE_HalContext *phost = Esd_GetHost();
	Esd_GpuAlloc *ga = &ec->GpuAlloc;

	if (!EVE_Hal_supportVideo(phost))
	{
		eve_assert_ex(false, "Esd_CoWidget_PlayVideoFlash is not available on the current graphics platform\n");
		return false;
	}

	if (!EVE_Hal_supportFlash(phost))
	{
		eve_printf_debug("This device doesn't support flash for video playback\n");
		return false;
	}

	if (phost->CmdFault)
		return false;

	/* Trash all memory */
	Esd_GpuAlloc_Reset(ga);
	Esd_GpuAlloc_Alloc(Esd_GAlloc, RAM_G_SIZE, GA_GC_FLAG); /* Block allocation */
	Esd_BitmapHandle_Reset(&ec->HandleState);

	/* Play from flash */
	EVE_CoCmd_flashSource(phost, addr);
	EVE_CoCmd_playVideo(phost, (options | OPT_FLASH) & ~(OPT_MEDIAFIFO | OPT_OVERLAY | OPT_NODL));

	bool res = EVE_Cmd_waitFlush(phost);

	if (EVE_Cmd_rp(phost) != EVE_Cmd_wp(phost))
	{
		eve_printf_debug("Coprocessor did not complete command, force fault\n");
		EVE_Util_forceFault(phost, "ESD Core: CMD_PLAYVIDEO aborted");
		res = false;
	}

	EVE_CoCmd_stop(phost);
	EVE_MediaFifo_close(phost);

	return res;
#else
	eve_printf_debug("Video from flash storage is not supported\n");
	return false;
#endif
}

bool Esd_CoWidget_PlayVideo(Esd_BitmapCell video, uint16_t options)
{
#if defined(EVE_SUPPORT_VIDEO)
	Esd_BitmapInfo *info = video.Info;
	if (!info->Video && info->Format != AVI)
	{
		eve_printf_debug("Not a video bitmap info object\n");
		return false;
	}

	if (info->Flash)
	{
		return Esd_CoWidget_PlayVideoFlash(Esd_BitmapInfo_LoadFlashAddress(&info->FlashAddress, info->File, NULL), options);
	}
	else
	{
		return Esd_CoWidget_PlayVideoFile(info->File, options);
	}
#else
	eve_printf_debug("Video is not supported\n");
	return false;
#endif
}
/* end of file */
