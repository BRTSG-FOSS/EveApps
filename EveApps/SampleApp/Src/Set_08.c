/*
 * Copyright (c) Bridgetek Pte Ltd
 *
 * THIS SOFTWARE IS PROVIDED BY BRIDGETEK PTE LTD "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 * BRIDGETEK PTE LTD BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES LOSS OF USE, DATA, OR PROFITS OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
 * TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * BRIDGETEK DRIVERS MAY BE USED ONLY IN CONJUNCTION WITH PRODUCTS BASED ON BRIDGETEK PARTS.
 *
 * BRIDGETEK DRIVERS MAY BE DISTRIBUTED IN ANY FORM AS LONG AS LICENSE INFORMATION IS NOT MODIFIED.
 *
 * IF A CUSTOM VENDOR ID AND/OR PRODUCT ID OR DESCRIPTION STRING ARE USED, IT IS THE
 * RESPONSIBILITY OF THE PRODUCT MANUFACTURER TO MAINTAIN ANY CHANGES AND SUBSEQUENT WHQL
 * RE-CERTIFICATION AS A RESULT OF MAKING THESE CHANGES.
 *
 * Abstract: Application to demonstrate function of EVE.
 * Author : Bridgetek
 */

 /*
  *  Description: This is sample demo for below EVE functions:
  *     > MultiTracker
  *     > NumberBases
  *     > Screen Orientation
  *     > Bitmap up to 2048pix x 2048pix.
  *     > Paletted bitmap.
  *     > Paletted ARGB888 format.
  *     > DXT1: reduce bitmap size upto 4 folds.
  *     > AVI video playback
  *     > AVI video playback via CMD buffer
  *     > AVI video playback via frame by frame
  */

#include "Platform.h"
#include "Gpu_Hal.h"
#include "Common.h"
#include "App.h"
#include "Samapp_Enable_API.h"

#ifdef SAMAPP_ENABLE_APIS_SET8

static EVE_HalContext *s_pHalContext;

#ifdef DISPLAY_RESOLUTION_HVGA_PORTRAIT
#define DISPLAY_RESOLUTION_HVGA_PORTRAIT 1
#endif
#ifdef DISPLAY_RESOLUTION_WQVGA
#define DISPLAY_RESOLUTION_WQVGA 1
#endif

/**
 * @brief Load raw file to RAM_G
 *
 * @param fileName File to load
 * @param ramOffset Address on RAM_G
 * @return int32_t
 */
int32_t SAMAPP_LoadRawFromFile(const char8_t* fileName, int32_t ramOffset);

static int stopVideoCmdFifo() {
	int availfreesz = 0;
	int cmdRead = 0;
	int cmdWrite = 0;
#define EMPTY_BUFFER_SIZE (4 * 50)

#if defined(BT81X_ENABLE)
	eve_printf_debug("End of video\n");
	EVE_sleep(1000);
	eve_printf_debug("Stop video\n");
	EVE_Hal_wr8(s_pHalContext, REG_PLAY_CONTROL, 0xFF); // stop the video

	EVE_sleep(1000);
	eve_printf_debug("Restore play control video\n");
	EVE_Hal_wr8(s_pHalContext, REG_PLAY_CONTROL, 1); // restore default value
#endif							   
	//fill remain padding bytes to command fifo
	EVE_sleep(5000);
	eve_printf_debug("Pad FIFO\n");
	int emptyBuffer[EMPTY_BUFFER_SIZE] = { 0 };
	while (true) {
		cmdRead = EVE_Hal_rd16(s_pHalContext, REG_CMD_READ);
		cmdWrite = EVE_Hal_rd16(s_pHalContext, REG_CMD_WRITE);
		APP_DBG("cmdRead: %d, cmdWrite: %d\n", cmdRead, cmdWrite);

		if (cmdRead == cmdWrite) {
			EVE_Hal_wrMem(s_pHalContext, REG_CMDB_WRITE, (uint8_t*)emptyBuffer, EMPTY_BUFFER_SIZE);
			break;
		}
		else {
			availfreesz = EVE_Hal_rd32(s_pHalContext, REG_CMDB_SPACE);
			if (availfreesz > EMPTY_BUFFER_SIZE) availfreesz = EMPTY_BUFFER_SIZE;

			if (availfreesz > 0)
				EVE_Hal_wrMem(s_pHalContext, REG_CMDB_WRITE, (uint8_t*)emptyBuffer, availfreesz);
		}
		EVE_sleep(100);
	}
	eve_printf_debug("Ready\n");
	return 1;
}

#if (!defined(FT900_PLATFORM)) && (!defined(FT93X_PLATFORM) ) && !(defined(ARDUINO_PLATFORM))

#if defined(EVE_FLASH_AVAILABLE)
void VideoPlayBackFrameByFrame_FromFlash() {
	const uint32_t flashSource = 4096;

	// video 1st settings
	const uint32_t  videoW = 256;
	const uint32_t  videoH = 144;
	const uint32_t  videoX = 0;
	const uint32_t  videoY = 0;
	const uint32_t  videoSource = 8;
	const uint32_t  videoHANDLE = 1;
	const uint32_t  completionPtr = 0;

	if (!FlashHelper_SwitchFullMode(s_pHalContext)) {
		APP_ERR("SwitchFullMode failed");
		return;
	}

	Display_Start(s_pHalContext);
	// video 1st
	EVE_Cmd_wr32(s_pHalContext, BITMAP_HANDLE(videoHANDLE));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_SOURCE(videoSource));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_LAYOUT(RGB565, videoW * 2L, videoH));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_LAYOUT_H(((videoW * 2L) >> 10), ((videoH) >> 9)));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_SIZE(NEAREST, BORDER, BORDER, videoW, videoH));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_SIZE_H((videoW >> 9), (videoH >> 9)));
	EVE_Cmd_wr32(s_pHalContext, BEGIN(BITMAPS));
	EVE_Cmd_wr32(s_pHalContext, VERTEX2F(videoX * 16, videoY * 16));
	EVE_Cmd_wr32(s_pHalContext, END());
	Display_End(s_pHalContext);

	EVE_CoCmd_flashSource(s_pHalContext, flashSource);
	EVE_CoCmd_videoStartF(s_pHalContext);
	do {
		EVE_CoCmd_videoFrame(s_pHalContext, videoSource, completionPtr);
		EVE_Cmd_waitFlush(s_pHalContext);
		EVE_sleep(16); // wait 16ms to next frame, so maximum FPS ~= 1000 / 16 = ~60fps
	} while (EVE_Hal_rd32(s_pHalContext, completionPtr) != 0);
}
#endif
/**
 * @brief video playback via frame by frame
 *
 */
void VideoPlayBackFrameByFrame() {
	uint16_t aviw = 854;
	uint16_t avih = 480;
	uint16_t videoFrameStatusAddr;//TBD put a parser and get the values from it
	Fifo_t stFifo;
	uint32_t mediafifo;
	uint32_t mediafifolen;
	uint8_t g_scratch[SCRATCH_BUFF_SZ];
	uint32_t filesz;
	uint32_t currchunk;
	uint32_t bytesread;
	uint32_t cmdrd = 0;
	uint32_t cmdwr = 0;
#if defined(MSVC_PLATFORM) || defined(BT8XXEMU_PLATFORM)
	FILE* pFile;
#endif

	/* construct the DL and display */
	EVE_CoCmd_dlStart(s_pHalContext);
	EVE_Cmd_wr32(s_pHalContext, CLEAR_COLOR_RGB(255, 255, 255));
	EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1));
	EVE_Cmd_wr32(s_pHalContext, DISPLAY());
	EVE_CoCmd_swap(s_pHalContext);
	EVE_Cmd_waitFlush(s_pHalContext);

	/* start video playback, load the data into media fifo */
	mediafifo = aviw * avih * 2L + 32L + 4; //the starting address of the media fifo, the begining space is for the decoded video frame,
	videoFrameStatusAddr = RAM_G;  //the 4 byte address for the videoframe status
	mediafifolen = RAM_G_SIZE - mediafifo;
	EVE_CoCmd_mediaFifo(s_pHalContext, mediafifo, mediafifolen);//address of the media fifo buffer

	printf("Mediafifo: Start address and length %d %d\n", mediafifo, mediafifolen);
	EVE_Cmd_waitFlush(s_pHalContext);

#if defined(MSVC_PLATFORM) || defined(BT8XXEMU_PLATFORM)
	pFile = fopen(TEST_DIR "\\chickens-4.avi", "rb");
	if (pFile != NULL) {
		printf("\nFopen success!\n");
	}
	else {
		printf("\nFailed to open file.\n");
		return;
	}
#endif

#if defined(MSVC_PLATFORM) || defined(BT8XXEMU_PLATFORM)
	fseek(pFile, 0, SEEK_END);
	filesz = ftell(pFile);
	fseek(pFile, 0, SEEK_SET);
#endif

	printf("file size %x %d %d %d %d\n", filesz, mediafifo, mediafifolen, EVE_Hal_rd32(s_pHalContext, REG_CMD_READ), EVE_Hal_rd32(s_pHalContext, REG_CMD_WRITE));
	Fifo_Init(&stFifo, mediafifo, mediafifolen, REG_MEDIAFIFO_READ, REG_MEDIAFIFO_WRITE);

	EVE_CoCmd_mediaFifo(s_pHalContext, mediafifo, mediafifolen);//address of the media fifo buffer - init of fifo done by coprocessor

	EVE_CoCmd_dlStart(s_pHalContext);

	EVE_Cmd_wr32(s_pHalContext, CLEAR_COLOR_RGB(255, 255, 255));
	EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1));

	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 255, 255));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_HANDLE(0));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_SOURCE(4));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_LAYOUT(RGB565, aviw * 2L, avih));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_LAYOUT_H(((aviw * 2L) >> 10), ((avih) >> 9)));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_SIZE(NEAREST, BORDER, BORDER, aviw, avih));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_SIZE_H((aviw >> 9), (avih >> 9)));
	EVE_Cmd_wr32(s_pHalContext, BEGIN(BITMAPS));
	EVE_Cmd_wr32(s_pHalContext, VERTEX2F(0, 0));

	EVE_Cmd_wr32(s_pHalContext, DISPLAY());
	EVE_CoCmd_swap(s_pHalContext);
	EVE_Cmd_waitFlush(s_pHalContext);

	printf("media fifo init done \n");

	/* fill the complete fifo buffer before entering into steady state */
	printf("Start fifofill %d %x %x %x %x\n", filesz, cmdrd, cmdwr, EVE_Hal_rd32(s_pHalContext, stFifo.HW_Read_Reg), EVE_Hal_rd32(s_pHalContext, stFifo.HW_Write_Reg));
	currchunk = mediafifolen - SCRATCH_BUFF_SZ;
	currchunk = ALIGN_TWO_POWER_N(currchunk, SCRATCH_BUFF_SZ);
	while (currchunk > 0) {
		uint32_t tempchunk = currchunk;
		if (tempchunk > SCRATCH_BUFF_SZ) {
			tempchunk = SCRATCH_BUFF_SZ;
		}
		else {
			tempchunk = currchunk;
		}
#if defined(MSVC_PLATFORM)|| defined(BT8XXEMU_PLATFORM)
		bytesread = fread(g_scratch, 1, tempchunk, pFile);
#endif

#if defined(MSVC_PLATFORM)|| defined(BT8XXEMU_PLATFORM)
		EVE_Hal_wrMem(s_pHalContext, stFifo.fifo_buff + stFifo.fifo_wp, g_scratch, tempchunk);
#endif
		stFifo.fifo_wp += tempchunk;
		currchunk -= tempchunk;
	}

	/* update the data written */
	filesz -= stFifo.fifo_wp;

	/* update the read and write pointers of mediafifo */
	EVE_Hal_wr32(s_pHalContext, stFifo.HW_Write_Reg, stFifo.fifo_wp);
	printf("Media fifo rd wr %d %d %d %d \n", EVE_Hal_rd32(s_pHalContext, stFifo.HW_Read_Reg), EVE_Hal_rd32(s_pHalContext, stFifo.HW_Write_Reg), EVE_Hal_rd32(s_pHalContext, REG_CMD_READ), EVE_Hal_rd32(s_pHalContext, REG_CMD_WRITE));

	EVE_Hal_wr8(s_pHalContext, REG_VOL_PB, 255);
	EVE_CoCmd_videoStart(s_pHalContext);  //initialize AVI video decoder
	EVE_CoCmd_videoFrame(s_pHalContext, 4, videoFrameStatusAddr);  //load 1 frame
	EVE_Cmd_waitFlush(s_pHalContext);

	cmdrd = EVE_Hal_rd32(s_pHalContext, REG_CMD_READ);
	cmdwr = EVE_Hal_rd32(s_pHalContext, REG_CMD_WRITE);

	while ((Fifo_GetFreeSpace(s_pHalContext, &stFifo) != (stFifo.fifo_len - 4)) || (filesz > 0))//loop till end of the file
	{
		int32_t availfreesz = Fifo_GetFreeSpace(s_pHalContext, &stFifo);
		int32_t chunkfilled = 0;

		if (EVE_Hal_rd16(s_pHalContext, REG_CMD_READ) == EVE_Hal_rd16(s_pHalContext, REG_CMD_WRITE)) {
			EVE_CoCmd_videoFrame(s_pHalContext, 4, videoFrameStatusAddr);
			EVE_Cmd_waitFlush(s_pHalContext);
		}
		while (filesz > 0) {
			if (availfreesz <= SCRATCH_BUFF_SZ) {
				break;
			}
			if (filesz > SCRATCH_BUFF_SZ) {
				currchunk = SCRATCH_BUFF_SZ;
			}
			else {
				currchunk = filesz;
			}
#if defined(MSVC_PLATFORM)|| defined(BT8XXEMU_PLATFORM)
			bytesread = fread(g_scratch, 1, currchunk, pFile);
#endif
			chunkfilled = Fifo_Write(s_pHalContext, &stFifo, g_scratch, bytesread);//download the data and update
			if (chunkfilled != bytesread) {
				printf("filled sz and curr sz are wrong %d %d %d\n ", chunkfilled, currchunk, bytesread);
			}
			filesz -= chunkfilled;
			availfreesz = Fifo_GetFreeSpace(s_pHalContext, &stFifo);
		}

		if (EVE_Hal_rd32(s_pHalContext, stFifo.HW_Read_Reg) == EVE_Hal_rd32(s_pHalContext, stFifo.HW_Write_Reg)) {
			printf("end of videoframe %d %d %d %x %x %x %x\n", filesz, currchunk, bytesread, cmdrd, cmdwr, EVE_Hal_rd32(s_pHalContext, stFifo.HW_Read_Reg), EVE_Hal_rd32(s_pHalContext, stFifo.HW_Write_Reg));
			break;
		}

		cmdrd = EVE_Hal_rd16(s_pHalContext, REG_CMD_READ);
		cmdwr = EVE_Hal_rd16(s_pHalContext, REG_CMD_WRITE);

	}

	printf("playback finished %d %x %x %x %x\n", filesz, cmdrd, cmdwr, EVE_Hal_rd32(s_pHalContext, stFifo.HW_Read_Reg), EVE_Hal_rd32(s_pHalContext, stFifo.HW_Write_Reg));
#if defined(MSVC_PLATFORM) || defined(BT8XXEMU_PLATFORM)
	fclose(pFile);
#endif 

	EVE_Cmd_waitFlush(s_pHalContext);
}
#endif

/**
 * @brief API to demonstrate AVI video playback with ASTC overlay
 *
 */
void testVideoPlayBackCmdBuff_ASTC() {
#if defined(BT81X_ENABLE) && defined(EVE_FLASH_AVAILABLE)
#define VIDEO_HEIGHT         (480)
#define VIDEO_WIDTH          (854)
#define VIDEO_RAM_SPACE      (VIDEO_WIDTH * VIDEO_HEIGHT *2)
#define SPARE_RAM_G_SIZE     (RAM_G_SIZE - VIDEO_RAM_SPACE)
#define SPARE_RAM_G_ADDRESS   (RAM_G + VIDEO_RAM_SPACE)
#define ASTC_LOGO_FLASH_ADDR  (4096)

	Draw_Text(s_pHalContext, "AVI video playback with ASTC overlay");

	// Add one waiting screen while programming flash the logo image file
	Display_Start(s_pHalContext);
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0, 0, 0));
	EVE_CoCmd_text(s_pHalContext, (int16_t)(s_pHalContext->Width / 2), 80, 27, OPT_CENTER, "Loading video ...");
	EVE_CoCmd_spinner(s_pHalContext, (int16_t)(s_pHalContext->Width / 2), (int16_t)(s_pHalContext->Height / 2), 0, (int16_t)(float)1.0 / 2);//style 0 and scale 0.5
	Display_End(s_pHalContext);
	EVE_sleep(500);

	FlashHelper_SwitchFullMode(s_pHalContext);

	char* files = TEST_DIR "\\Logo_480x272_480x272_COMPRESSED_RGBA_ASTC_4x4_KHR.raw";
	if (0 >= Ftf_Write_File_To_Flash_By_RAM_G(s_pHalContext, files, ASTC_LOGO_FLASH_ADDR)) {
		return;
	}

	EVE_Cmd_wr32(s_pHalContext, CMD_DLSTART);
	EVE_Cmd_wr32(s_pHalContext, CLEAR_COLOR_RGB(255, 255, 255));
	EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1));

	EVE_Cmd_wr32(s_pHalContext, BITMAP_HANDLE(0));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_SOURCE(0));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_LAYOUT(RGB565, 854 * 2L, 480));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_LAYOUT_H(((854 * 2L) >> 10), ((480) >> 9)));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_SIZE(NEAREST, BORDER, BORDER, 854, 480));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_SIZE_H((854 >> 9), (480 >> 9)));

	EVE_Cmd_wr32(s_pHalContext, BITMAP_HANDLE(1));
	EVE_CoCmd_setBitmap(s_pHalContext, 0x800000 | (ASTC_LOGO_FLASH_ADDR / 32), COMPRESSED_RGBA_ASTC_4x4_KHR, 480, 272);

	EVE_Cmd_wr32(s_pHalContext, BEGIN(BITMAPS));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_HANDLE(0));
	EVE_Cmd_wr32(s_pHalContext, VERTEX2F(0, 0));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_HANDLE(1));
	EVE_Cmd_wr32(s_pHalContext, VERTEX2F(0 * 16, 100 * 16));

	EVE_Cmd_wr32(s_pHalContext, DISPLAY());
	EVE_CoCmd_swap(s_pHalContext);
	EVE_Hal_flush(s_pHalContext);
	EVE_Cmd_waitFlush(s_pHalContext);

	//The video data is following: (through command buffer)
	uint8_t g_scratch[SCRATCH_BUFF_SZ];
	uint32_t filesz;
	uint32_t bytesread;
	FILE* pFile = fopen(TEST_DIR "\\chickens-4.avi", "rb");

	if (pFile != NULL) {
		printf("\nFopen success!\n");
	}
	else {
		printf("\nFailed to open file.\n");
		return;
	}

	fseek(pFile, 0, SEEK_END);
	filesz = ftell(pFile);
	fseek(pFile, 0, SEEK_SET);

#if defined(BT81X_ENABLE)
	EVE_Hal_wr8(s_pHalContext, REG_PLAY_CONTROL, 1); // restore default value
#endif		
	EVE_Cmd_wr32(s_pHalContext, CMD_PLAYVIDEO);
	EVE_Cmd_wr32(s_pHalContext, OPT_NODL);

	int32_t availfreesz = 0;
	int32_t freadbufffill = 0;
	const uint8_t* pbuff = g_scratch;
	while (filesz > 0) {
		availfreesz = EVE_Hal_rd32(s_pHalContext, REG_CMDB_SPACE);
		if (availfreesz <= 0) {
			continue;
		}
		if (0 == freadbufffill) {
			memset(g_scratch, 0, sizeof(g_scratch));

			if (filesz > SCRATCH_BUFF_SZ) {
				freadbufffill = SCRATCH_BUFF_SZ;
			}
			else {
				freadbufffill = filesz;
			}
			bytesread = fread(g_scratch, 1, freadbufffill, pFile);
			pbuff = g_scratch;
			filesz -= bytesread;
		}

		if (availfreesz > freadbufffill) {
			availfreesz = freadbufffill;
		}

		if (availfreesz > 0) {
			EVE_Hal_wrMem(s_pHalContext, REG_CMDB_WRITE, pbuff, availfreesz);
		}
		pbuff += availfreesz;
		freadbufffill -= availfreesz;
	}
	fclose(pFile);
	stopVideoCmdFifo();
	EVE_Cmd_restore(s_pHalContext);
#endif
}

/**
 * @brief Test AVI video playback via REG_CMDB_WRITE/REG_CMDB_SPACE
 *
 */
void testVideoPlayBackCmdBuff() {
	uint8_t g_scratch[SCRATCH_BUFF_SZ];
	uint32_t filesz;
	uint32_t bytesread;
#define VIDEO_HEIGHT         (480)
#define VIDEO_WIDTH          (854)
#define VIDEO_RAM_SPACE      (VIDEO_WIDTH * VIDEO_HEIGHT *2)
#define SPARE_RAM_G_SIZE     (RAM_G_SIZE - VIDEO_RAM_SPACE)
#define SPARE_RAM_G_ADDRESS  (RAM_G + VIDEO_RAM_SPACE)

	//read back the logo ARGB data to file
	Ftf_Read_File_From_RAM_G(s_pHalContext, TEST_DIR "\\chicken_before.raw", SPARE_RAM_G_ADDRESS, SPARE_RAM_G_SIZE);

	EVE_Cmd_wr32(s_pHalContext, CMD_DLSTART);
	EVE_Cmd_wr32(s_pHalContext, CLEAR_COLOR_RGB(255, 255, 255));
	EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1));
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0xff, 0xff, 0xff));

	EVE_Cmd_wr32(s_pHalContext, BITMAP_HANDLE(0));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_SOURCE(0));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_LAYOUT(RGB565, 854 * 2L, 480));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_LAYOUT_H(((854 * 2L) >> 10), ((480) >> 9)));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_SIZE(NEAREST, BORDER, BORDER, 854, 480));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_SIZE_H((854 >> 9), (480 >> 9)));

	EVE_Cmd_wr32(s_pHalContext, BEGIN(BITMAPS));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_HANDLE(0));
	EVE_Cmd_wr32(s_pHalContext, VERTEX2F(0, 0));
	EVE_Cmd_wr32(s_pHalContext, DISPLAY());

	EVE_CoCmd_swap(s_pHalContext);
	EVE_Cmd_waitFlush(s_pHalContext);

	//The video data is following: (through command buffer)
	FILE* pFile = fopen(TEST_DIR "\\chickens-4.avi", "rb");

	if (pFile != NULL) {
		printf("\nFopen success!\n");
	}
	else {
		printf("\nFailed to open file.\n");
		return;
	}

	fseek(pFile, 0, SEEK_END);
	filesz = ftell(pFile);
	fseek(pFile, 0, SEEK_SET);

	EVE_Cmd_wr32(s_pHalContext, CMD_PLAYVIDEO);
	EVE_Cmd_wr32(s_pHalContext, OPT_NODL);

	int32_t availfreesz = 0;
	int32_t freadbufffill = 0;
	const uint8_t* pbuff = g_scratch;
	while (filesz > 0) {
		availfreesz = EVE_Hal_rd32(s_pHalContext, REG_CMDB_SPACE);
		if (availfreesz <= 0) {
			continue;
		}
		if (0 == freadbufffill) {
			if (filesz > SCRATCH_BUFF_SZ) {
				freadbufffill = SCRATCH_BUFF_SZ;
			}
			else {
				freadbufffill = filesz;
			}
			bytesread = fread(g_scratch, 1, freadbufffill, pFile);
			filesz -= bytesread;
			pbuff = g_scratch;
		}

		if (availfreesz > freadbufffill) {
			availfreesz = freadbufffill;
		}

		if (availfreesz > 0) {
			EVE_Hal_wrMem(s_pHalContext, REG_CMDB_WRITE, pbuff, availfreesz);
		}
		pbuff += availfreesz;
		freadbufffill -= availfreesz;
	}
	stopVideoCmdFifo();
	fclose(pFile);
	EVE_Cmd_restore(s_pHalContext);
}

/**
 * @brief AVI video playback via CMD buffer
 *
 */
void VideoPlaybackViaCMDBuff() {
#if defined(MSVC_PLATFORM) || defined(BT8XXEMU_PLATFORM)
#if defined(BT81X_ENABLE)

	uint8_t g_scratch[SCRATCH_BUFF_SZ];
	uint32_t filesz;
	uint32_t bytesread;
#if defined(MSVC_PLATFORM) || defined(BT8XXEMU_PLATFORM)
	FILE* pFile;
#endif

#define LOGO_ADDRESS   (854 * 480 * 2)
#define PNG_W 182
#define PNG_H 103

	/* construct the DL and display */
	FILE* PicpFile = fopen(TEST_DIR "\\Logo_182x103_182x103_ARGB4_Converted.png", "rb");

	if (PicpFile == NULL)
		return;
	uchar8_t* Picpbuff = (uchar8_t*)malloc(8192);
	fseek(PicpFile, 0, SEEK_END);
	uint32_t PicFileLen = ftell(PicpFile);
	fseek(PicpFile, 0, SEEK_SET);

	EVE_Cmd_waitFlush(s_pHalContext);

	EVE_Cmd_wr32(s_pHalContext, CMD_LOADIMAGE);
	EVE_Cmd_wr32(s_pHalContext, LOGO_ADDRESS);//destination address of jpg decode
	EVE_Cmd_wr32(s_pHalContext, OPT_NODL);//output format of the bitmap, supported options are OPT_MONO-bitmap to be monochrome, OPT_FULLSCREEN-attempt to scale to full screen size, and OPT_MEDIAFIFO-use media fifo for image data. If the bitmap format to be used is other than RGB565 and mono.  NODL option was chosen becuase the png has transparancy and we want to draw it by ARGB4 format

	while (PicFileLen > 0) {
		/* download the data into the command buffer by 2kb one shot */
		uint16_t Picblocklen = PicFileLen > 8192 ? 8192 : (uint16_t)PicFileLen;

		/* copy the data into pbuff and then transfter it to command buffer */
		fread(Picpbuff, 1, Picblocklen, PicpFile);
		PicFileLen -= Picblocklen;
		/* copy data continuously into command memory */
		EVE_Cmd_wrMem(s_pHalContext, Picpbuff, Picblocklen);//alignment is already taken care by this api
	}
	/* close the opened binary zlib file */
	fclose(PicpFile);
	free(Picpbuff);
	EVE_Cmd_waitFlush(s_pHalContext);

	//read back the logo ARGB data to file
	Ftf_Read_File_From_RAM_G(s_pHalContext, TEST_DIR "\\logo_argb4_first_2.raw", LOGO_ADDRESS, PNG_W * PNG_H * 2);

	EVE_Cmd_wr32(s_pHalContext, CMD_DLSTART);
	EVE_Cmd_wr32(s_pHalContext, CLEAR_COLOR_RGB(255, 255, 255));
	EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1));
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 255, 255));

	EVE_Cmd_wr32(s_pHalContext, BITMAP_HANDLE(0));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_SOURCE(0));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_LAYOUT(RGB565, 854 * 2L, 480));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_LAYOUT_H(((854 * 2L) >> 10), ((480) >> 9)));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_SIZE(NEAREST, BORDER, BORDER, 854, 480));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_SIZE_H((854 >> 9), (480 >> 9)));

	EVE_Cmd_wr32(s_pHalContext, BITMAP_HANDLE(1));
	EVE_CoCmd_setBitmap(s_pHalContext, LOGO_ADDRESS, ARGB4, PNG_W, PNG_H);

	EVE_Cmd_wr32(s_pHalContext, BEGIN(BITMAPS));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_HANDLE(0));
	EVE_Cmd_wr32(s_pHalContext, VERTEX2F(0, 0));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_HANDLE(1));
	EVE_Cmd_wr32(s_pHalContext, VERTEX2F(100 * 16, 100 * 16));

	EVE_Cmd_wr32(s_pHalContext, DISPLAY());

	EVE_CoCmd_swap(s_pHalContext);
	EVE_Cmd_waitFlush(s_pHalContext);

	pFile = fopen(TEST_DIR "\\chickens-4.avi", "rb");
	if (pFile != NULL) {
		printf("\nFopen success!\n");
	}
	else {
		printf("\nFailed to open file.\n");
		return;
	}

	fseek(pFile, 0, SEEK_END);
	filesz = ftell(pFile);
	fseek(pFile, 0, SEEK_SET);

	EVE_Hal_wr8(s_pHalContext, REG_VOL_PB, 255);
#if defined(BT81X_ENABLE)
	EVE_Hal_wr8(s_pHalContext, REG_PLAY_CONTROL, 1); // restore default value
#endif		
	EVE_Cmd_wr32(s_pHalContext, CMD_PLAYVIDEO);
	EVE_Cmd_wr32(s_pHalContext, OPT_NODL);

	int32_t availfreesz = 0;
	int32_t freadbufffill = 0;
	const uint8_t* pbuff = g_scratch;
	APP_DBG("Video playback starts.\n");
	while (filesz > 0) {
		availfreesz = EVE_Hal_rd32(s_pHalContext, REG_CMDB_SPACE);
		if (availfreesz <= 0) {
			continue;
		}
		if (0 == freadbufffill) {
			if (filesz > SCRATCH_BUFF_SZ) {
				freadbufffill = SCRATCH_BUFF_SZ;
			}
			else {
				freadbufffill = filesz;
			}
			bytesread = fread(g_scratch, 1, freadbufffill, pFile);
			pbuff = g_scratch;
			filesz -= bytesread;
		}

		if (availfreesz > freadbufffill) {
			availfreesz = freadbufffill;
		}

		if (availfreesz > 0) {
			EVE_Hal_wrMem(s_pHalContext, REG_CMDB_WRITE, pbuff, availfreesz);
		}
		pbuff += availfreesz;
		freadbufffill -= availfreesz;
	}

	fclose(pFile);

	stopVideoCmdFifo();
	EVE_Cmd_restore(s_pHalContext);

	EVE_Cmd_waitFlush(s_pHalContext);

	//read back the logo ARGB data to file
	Ftf_Read_File_From_RAM_G(s_pHalContext, TEST_DIR "\\logo_argb4_after_2.raw", LOGO_ADDRESS, PNG_W * PNG_H * 2);

#endif
#endif
}

#if defined(MSVC_PLATFORM) || defined(BT8XXEMU_PLATFORM)
/**
 * @brief Video playback with audio
 *
 */
void VideoPlayback() {
	uint16_t aviw = 256;
	uint16_t avih = 144;
	uint8_t g_scratch[SCRATCH_BUFF_SZ];
	Fifo_t stFifo;
	uint32_t mediafifo;
	uint32_t mediafifolen;
	uint32_t filesz;
	uint32_t currchunk;
	uint32_t bytesread;
	uint32_t cmdrd;
	uint32_t cmdwr;
	FILE* pFile;

	pFile = fopen(TEST_DIR "\\bbb_lo.avi", "rb");
	if (pFile == NULL) {
		EVE_CoCmd_dlStart(s_pHalContext);
		EVE_Cmd_wr32(s_pHalContext, CLEAR_COLOR_RGB(255, 255, 255));
		EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1));
		EVE_CoCmd_text(s_pHalContext, (int16_t)(s_pHalContext->Width / 2), (int16_t)(s_pHalContext->Height / 2), 29, OPT_CENTER, "Unable to open file.");
		EVE_Cmd_wr32(s_pHalContext, DISPLAY());
		EVE_CoCmd_swap(s_pHalContext);
		EVE_Cmd_waitFlush(s_pHalContext);
		printf("Error while openning file.\n");
		return;
	}
	else {
		/* start video playback, load the data into media fifo */
		mediafifo = aviw * avih * 2L + 32 * 1024L; //starting address of the media fifo
		mediafifolen = RAM_G_SIZE - mediafifo; //how much memory will be allocated for the video playback fifo
		EVE_CoCmd_mediaFifo(s_pHalContext, mediafifo, mediafifolen); //address of the media fifo buffer
		printf("Mediafifo: Start address and length %d %d\n", mediafifo, mediafifolen);

		Fifo_Init(&stFifo, mediafifo, mediafifolen, REG_MEDIAFIFO_READ, REG_MEDIAFIFO_WRITE); //initialize application media fifo structure
		fseek(pFile, 0, SEEK_END);
		filesz = ftell(pFile);
		fseek(pFile, 0, SEEK_SET);

		EVE_Cmd_waitFlush(s_pHalContext);
		EVE_Hal_wr8(s_pHalContext, REG_VOL_PB, 255);
		EVE_Cmd_wr32(s_pHalContext, CMD_PLAYVIDEO);
		EVE_Cmd_wr32(s_pHalContext, OPT_MEDIAFIFO | OPT_NOTEAR | OPT_SOUND | OPT_FULLSCREEN);

		/* fill the complete fifo buffer before entering into steady state */
		currchunk = mediafifolen - SCRATCH_BUFF_SZ;
		currchunk = ALIGN_TWO_POWER_N(currchunk, SCRATCH_BUFF_SZ);
		while (currchunk > 0) {
			uint32_t tempchunk = currchunk;
			if (tempchunk > SCRATCH_BUFF_SZ) {
				tempchunk = SCRATCH_BUFF_SZ;
			}
			else {
				tempchunk = currchunk;
			}
			fread(g_scratch, 1, tempchunk, pFile);
			Fifo_WriteWait(s_pHalContext, &stFifo, g_scratch, tempchunk);
			currchunk -= tempchunk;
		}
		cmdrd = EVE_Hal_rd16(s_pHalContext, REG_CMD_READ);
		cmdwr = EVE_Hal_rd16(s_pHalContext, REG_CMD_WRITE);

		filesz -= stFifo.fifo_wp;
		while ((cmdrd != cmdwr) || (filesz > 0))  //loop till end of the file
		{
			if (filesz <= 0) {
				cmdrd = EVE_Hal_rd16(s_pHalContext, REG_CMD_READ);
				cmdwr = EVE_Hal_rd16(s_pHalContext, REG_CMD_WRITE);
				continue;
			}

			if (filesz > SCRATCH_BUFF_SZ) {
				currchunk = SCRATCH_BUFF_SZ;
			}
			else {
				currchunk = filesz;
			}
			bytesread = fread(g_scratch, 1, currchunk, pFile);
			Fifo_WriteWait(s_pHalContext, &stFifo, g_scratch, bytesread); //download the whole chunk into ring buffer - blocking call

			filesz -= currchunk;


			cmdrd = EVE_Hal_rd16(s_pHalContext, REG_CMD_READ);
			cmdwr = EVE_Hal_rd16(s_pHalContext, REG_CMD_WRITE);

		}
		fclose(pFile);
		EVE_Cmd_restore(s_pHalContext);
	}
}
#endif

/**
 * @brief Load DXT1 compressed image. The BRIDGETEK DXT1 conversion utility outputs 4 seperate files: c0,c1,b0,b1.
 * The 4 files should be combined to create the final image.  The bitmap size can be reduced up to 4 folds of the original size.
 *
 */
void SAMAPP_GPU_DXT1() {
	//load each .raw file into graphics RAM from directory ‘test’
	//RAM_G is starting address in graphics RAM, for example 00 0000h
	uint16_t imgWidth = 128;
	uint16_t imgHeight = 128;
	uint16_t c0_c1_width = 32;
	uint16_t c0_c1_height = 32;
	uint16_t c0_c1_stride = c0_c1_width * 2;
	uint16_t b0_b1_width = imgWidth;
	uint16_t b0_b1_height = imgHeight;
	uint16_t b0_b1_stride = b0_b1_width / 8;
	uint16_t szPerFile = 2048;

	Ftf_Write_File_To_RAM_G(s_pHalContext, TEST_DIR "\\SAMAPP_Tomato_DXT1_C0_Data_Raw.bin", RAM_G);
	Ftf_Write_File_To_RAM_G(s_pHalContext, TEST_DIR "\\SAMAPP_Tomato_DXT1_C1_Data_Raw.bin", RAM_G + szPerFile);
	Ftf_Write_File_To_RAM_G(s_pHalContext, TEST_DIR "\\SAMAPP_Tomato_DXT1_B0_Data_Raw.bin", RAM_G + szPerFile * 2);
	Ftf_Write_File_To_RAM_G(s_pHalContext, TEST_DIR "\\SAMAPP_Tomato_DXT1_B1_Data_Raw.bin", RAM_G + szPerFile * 3);

	EVE_CoCmd_dlStart(s_pHalContext);
	EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1));
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 255, 255));

	EVE_CoCmd_loadIdentity(s_pHalContext);
	EVE_CoCmd_setMatrix(s_pHalContext);

	EVE_Cmd_wr32(s_pHalContext, SAVE_CONTEXT());
	//B0&B1 handle
	EVE_Cmd_wr32(s_pHalContext, BITMAP_HANDLE(1));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_SOURCE(RAM_G + szPerFile * 2));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_LAYOUT(L1, b0_b1_stride, b0_b1_height)); //L1 format stride is 1 bit per pixel
	EVE_Cmd_wr32(s_pHalContext,
		BITMAP_SIZE(NEAREST, BORDER, BORDER, imgWidth, imgHeight)); //draw in full size
																	//C0&C1 handle
	EVE_Cmd_wr32(s_pHalContext, BITMAP_HANDLE(2));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_SOURCE(RAM_G));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_LAYOUT(RGB565, c0_c1_stride, c0_c1_height)); //RGB565 format stride is 2 bytes per pixel
	EVE_Cmd_wr32(s_pHalContext,
		BITMAP_SIZE(NEAREST, BORDER, BORDER, imgWidth, imgHeight)); //draw in full size
																	// start drawing bitmaps
	EVE_Cmd_wr32(s_pHalContext, BEGIN(BITMAPS));
	EVE_Cmd_wr32(s_pHalContext, BLEND_FUNC(ONE, ZERO));
	EVE_Cmd_wr32(s_pHalContext, COLOR_A(0x55));
	EVE_Cmd_wr32(s_pHalContext, VERTEX2II(s_pHalContext->Width / 2 - b0_b1_width / 2, s_pHalContext->Height / 2 - b0_b1_height / 2, 1, 0));
	EVE_Cmd_wr32(s_pHalContext, BLEND_FUNC(ONE, ONE));
	EVE_Cmd_wr32(s_pHalContext, COLOR_A(0xAA));
	EVE_Cmd_wr32(s_pHalContext, VERTEX2II(s_pHalContext->Width / 2 - b0_b1_width / 2, s_pHalContext->Height / 2 - b0_b1_height / 2, 1, 1));
	EVE_Cmd_wr32(s_pHalContext, COLOR_MASK(1, 1, 1, 0));
	EVE_CoCmd_scale(s_pHalContext, 4 * 65536, 4 * 65536);
	EVE_CoCmd_setMatrix(s_pHalContext);
	EVE_Cmd_wr32(s_pHalContext, BLEND_FUNC(DST_ALPHA, ZERO));
	EVE_Cmd_wr32(s_pHalContext, VERTEX2II(s_pHalContext->Width / 2 - b0_b1_width / 2, s_pHalContext->Height / 2 - b0_b1_height / 2, 2, 1));
	EVE_Cmd_wr32(s_pHalContext, BLEND_FUNC(ONE_MINUS_DST_ALPHA, ONE));
	EVE_Cmd_wr32(s_pHalContext, VERTEX2II(s_pHalContext->Width / 2 - b0_b1_width / 2, s_pHalContext->Height / 2 - b0_b1_height / 2, 2, 0));
	EVE_Cmd_wr32(s_pHalContext, END());
	EVE_Cmd_wr32(s_pHalContext, RESTORE_CONTEXT());

	//reset the transformation matrix because its not part of the context, RESTORE_CONTEXT() command will not revert the command.
	EVE_CoCmd_loadIdentity(s_pHalContext);
	EVE_CoCmd_setMatrix(s_pHalContext);

	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 0, 0));
	EVE_CoCmd_text(s_pHalContext, (int16_t)(s_pHalContext->Width / 2), 50, 31, OPT_CENTER, "DXT1: 8KB.");
	EVE_CoCmd_text(s_pHalContext, (int16_t)(s_pHalContext->Width / 2), 80, 31, OPT_CENTER,
		"Original: 32KB.");

	EVE_Cmd_wr32(s_pHalContext, DISPLAY());
	//swap the current display list with the new display list
	EVE_CoCmd_swap(s_pHalContext);
	//write to the FT800 FIFO command buffer - bitmap will appear after this command
	EVE_Cmd_waitFlush(s_pHalContext);
	EVE_sleep(3000);
}

/**
 * @brief API to demonstrate Paletted8 format
 *
 */
void SAMAPP_GPU_Bitmap_Paletted8() {
#if defined(MSVC_PLATFORM) || defined(BT8XXEMU_PLATFORM)
	const SAMAPP_Bitmap_header_t* p_bmhdr;

	int32_t  ft800_memaddr = RAM_G;
	int32_t pal_mem_addr = 900 * 1024;

	Gpu_Hal_LoadImageToMemory(s_pHalContext, TEST_DIR "\\Background_index.raw", RAM_G, LOAD);
	Gpu_Hal_LoadImageToMemory(s_pHalContext, TEST_DIR "\\Background_lut.raw", pal_mem_addr, LOAD);
#ifndef FT81X_ENABLE
	pal_mem_addr = RAM_PAL;
#else
	ft800_memaddr = pal_mem_addr;
#endif
	p_bmhdr = &SAMAPP_Bitmap_RawData_Header[3];
	EVE_Cmd_waitFlush(s_pHalContext);
	App_WrDl_Buffer(s_pHalContext, CLEAR(1, 1, 1)); // clear screen
	App_WrDl_Buffer(s_pHalContext, BEGIN(EDGE_STRIP_B)); // clear screen
	App_WrDl_Buffer(s_pHalContext, VERTEX2II(0, 0, 0, 0));
	App_WrDl_Buffer(s_pHalContext, VERTEX2F(s_pHalContext->Width * 16, 0));

	App_WrDl_Buffer(s_pHalContext, BITMAP_SOURCE(RAM_G));
#ifdef FT81X_ENABLE
	App_WrDl_Buffer(s_pHalContext, BITMAP_LAYOUT(PALETTED8, p_bmhdr->Stride, p_bmhdr->Height));
	App_WrDl_Buffer(s_pHalContext, BITMAP_LAYOUT_H(p_bmhdr->Stride >> 10, p_bmhdr->Height >> 9));
#else
	App_WrDl_Buffer(s_pHalContext, BITMAP_LAYOUT(p_bmhdr->Format, p_bmhdr->Stride, p_bmhdr->Height));
#endif

	App_WrDl_Buffer(s_pHalContext, BITMAP_SIZE(NEAREST, BORDER, BORDER, p_bmhdr->Width, p_bmhdr->Height));
	App_WrDl_Buffer(s_pHalContext, BITMAP_SIZE_H(p_bmhdr->Width >> 9, p_bmhdr->Height >> 9));
	App_WrDl_Buffer(s_pHalContext, BEGIN(BITMAPS)); // start drawing bitmaps

#ifdef FT81X_ENABLE
	App_WrDl_Buffer(s_pHalContext, BLEND_FUNC(ONE, ZERO));

	App_WrDl_Buffer(s_pHalContext, COLOR_MASK(0, 0, 0, 1));
	App_WrDl_Buffer(s_pHalContext, PALETTE_SOURCE(ft800_memaddr + 3));
	App_WrDl_Buffer(s_pHalContext, VERTEX2II(0, 0, 0, 0));

	App_WrDl_Buffer(s_pHalContext, BLEND_FUNC(DST_ALPHA, ONE_MINUS_DST_ALPHA));
	App_WrDl_Buffer(s_pHalContext, COLOR_MASK(1, 0, 0, 0));
	App_WrDl_Buffer(s_pHalContext, PALETTE_SOURCE(ft800_memaddr + 2));
	App_WrDl_Buffer(s_pHalContext, VERTEX2II(0, 0, 0, 0));

	App_WrDl_Buffer(s_pHalContext, COLOR_MASK(0, 1, 0, 0));
	App_WrDl_Buffer(s_pHalContext, PALETTE_SOURCE(ft800_memaddr + 1));
	App_WrDl_Buffer(s_pHalContext, VERTEX2II(0, 0, 0, 0));

	App_WrDl_Buffer(s_pHalContext, COLOR_MASK(0, 0, 1, 0));
	App_WrDl_Buffer(s_pHalContext, PALETTE_SOURCE(ft800_memaddr + 0));
	App_WrDl_Buffer(s_pHalContext, VERTEX2II(0, 0, 0, 0));

#else
	App_WrDl_Buffer(s_pHalContext, PALETTE_SOURCE(ft800_memaddr));
	App_WrDl_Buffer(s_pHalContext, VERTEX2II(0, 0, 0, 0));
#endif

	App_WrDl_Buffer(s_pHalContext, END());
	App_WrDl_Buffer(s_pHalContext, DISPLAY());

	/* Download the DL into DL RAM */
	App_Flush_DL_Buffer(s_pHalContext);

	/* Do a swap */
	GPU_DLSwap(s_pHalContext, DLSWAP_FRAME);
	SAMAPP_ENABLE_DELAY();
#endif
}

/**
 * @brief simpler method to load RAM font.  Use the font conversion utility to convert the desired subset of the ASCII characters, load font data, and use cmd_setfont2 command.
 *
 */
void SAMAPP_CoPro_Setfont2() {
#define FONTFILE_RAM_G_ADDRESS  1000
#define CUSTOM_RAM_FONT_HANDLE  0

#if defined(MSVC_PLATFORM) || defined(BT8XXEMU_PLATFORM)
	SAMAPP_LoadRawFromFile(TEST_DIR "\\Tuffy_Bold.raw", FONTFILE_RAM_G_ADDRESS);
#endif

	EVE_CoCmd_dlStart(s_pHalContext);

	EVE_Cmd_wr32(s_pHalContext, CLEAR_COLOR_RGB(255, 255, 255));
	EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1));
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0, 0, 255));

	EVE_CoCmd_setFont2(s_pHalContext, CUSTOM_RAM_FONT_HANDLE, FONTFILE_RAM_G_ADDRESS, 32);

	EVE_CoCmd_text(s_pHalContext, (int16_t)(s_pHalContext->Width / 2 - 30), (int16_t)(s_pHalContext->Height / 2 - 80), CUSTOM_RAM_FONT_HANDLE, OPT_CENTER, "cmd_setfont2 example");
	EVE_CoCmd_text(s_pHalContext, (int16_t)(s_pHalContext->Width / 2 - 30), (int16_t)(s_pHalContext->Height / 2), CUSTOM_RAM_FONT_HANDLE, OPT_CENTER, "1234test  JumpingFox");

	EVE_Cmd_wr32(s_pHalContext, DISPLAY());
	EVE_CoCmd_swap(s_pHalContext);
	EVE_Cmd_waitFlush(s_pHalContext);
	SAMAPP_ENABLE_DELAY_VALUE(5000);
}

/**
 * @brief Change screen orientation from landscape to portrait mode.  Setscratch command is also introduced, it sets the temporary bitmap handle for buttons, keys, and graidents.
 *
 */
void ScreenOrientation() {
	uint8_t text[100];
	for (uint32_t r = 0; r < 8; r++) {
		EVE_CoCmd_dlStart(s_pHalContext);
		EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1));

		EVE_CoCmd_setRotate(s_pHalContext, r);

		switch (r) {
		case 0:
		case 1:
		case 4:
		case 5:
			snprintf(text, 100, "Landscape Mode, rotate value= %d", r);
			EVE_CoCmd_text(s_pHalContext, (int16_t)(s_pHalContext->Width / 2), 50, 31, OPT_CENTER, text);
			break;
		default:
			snprintf(text, 100, "Portrait Mode\nRotate value= %d", r);
			EVE_CoCmd_text(s_pHalContext, (int16_t)(s_pHalContext->Height / 2), 50, 31, OPT_CENTER, text);
			break;
		}

		EVE_Cmd_wr32(s_pHalContext, DISPLAY());
		EVE_CoCmd_swap(s_pHalContext);
		EVE_Cmd_waitFlush(s_pHalContext);
		SAMAPP_ENABLE_DELAY_VALUE(3000);
	}
	//reset the rotation angle
	EVE_CoCmd_setRotate(s_pHalContext, 0);
}

/**
 * @brief support bitmap resolutions up to 2048x2048
 * FT80x can only display bitmaps no larger than 512x512, FT81x
 * If the bitmap dimensions are bigger than 512 in either direction

 *
 */
void HigherResolutionBitmap() {
	EVE_Cmd_waitFlush(s_pHalContext);

	EVE_CoCmd_dlStart(s_pHalContext);
	EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1));
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 255, 255));

	uint16_t iw = 800;
	uint16_t ih = 480;
	uint16_t format = RGB565;
#ifdef USE_BT81X_FLASH
#define BITMAP_ADDRESS_ON_FLASH <address> //address of bitmap file from Flash map after generating Flash
	/* Switch Flash to FULL Mode */
	FlashHelper_SwitchFullMode(s_pHalContext);
	EVE_CoCmd_setBitmap(s_pHalContext, (0x800000 | BITMAP_ADDRESS_ON_FLASH / 32), format, iw, ih);
#else
	//load bitmap file into graphics RAM
	//RAM_G is starting address in graphics RAM, for example 00 0000h
	Gpu_Hal_LoadImageToMemory(s_pHalContext, TEST_DIR "\\flower_800x480_800x480_RGB565.raw", RAM_G, LOAD);
	EVE_CoCmd_setBitmap(s_pHalContext, RAM_G, format, iw, ih);
#endif
	//Start drawing bitmap
	EVE_Cmd_wr32(s_pHalContext, BITMAP_HANDLE(1));
	EVE_Cmd_wr32(s_pHalContext, SAVE_CONTEXT());
	EVE_Cmd_wr32(s_pHalContext, BEGIN(BITMAPS));
	EVE_Cmd_wr32(s_pHalContext, VERTEX2II(0, 0, 0, 0));
	EVE_Cmd_wr32(s_pHalContext, END());
	EVE_Cmd_wr32(s_pHalContext, RESTORE_CONTEXT());
	EVE_Cmd_wr32(s_pHalContext, DISPLAY());
	EVE_CoCmd_swap(s_pHalContext);
	EVE_Cmd_waitFlush(s_pHalContext);

	SAMAPP_ENABLE_DELAY_VALUE(3000);
}

/**
 * @brief API to demonstrate load a raw file to Coprocessor
 *
 * @param fileName File to load
 * @param ramOffset Offset on RAM_G
 * @return int32_t Always 0
 */
int32_t SAMAPP_LoadRawFromFile(const char8_t* fileName, int32_t ramOffset) {
#if defined(MSVC_PLATFORM) || defined(BT8XXEMU_PLATFORM)
	int32_t FileLen = 0;
	int32_t FileSz = 0;
	uint8_t* pbuff = NULL;
	FILE* file = fopen(fileName, "rb");
	if (NULL == file) {
		printf("Error while opening file.\n");
		return 0;
	}
	else {
		fseek(file, 0, SEEK_END);
		FileSz = FileLen = ftell(file);
		fseek(file, 0, SEEK_SET);
		pbuff = (uint8_t*)malloc(8192);
		while (FileLen > 0) {
			/* download the data into the command buffer by 2kb one shot */
			uint16_t blocklen = FileLen > 8192 ? 8192 : (uint16_t)FileLen;

			/* copy the data into pbuff and then transfter it to command buffer */
			fread(pbuff, 1, blocklen, file);
			FileLen -= blocklen;
			/* copy data continuously into command memory */
			EVE_Hal_wrMem(s_pHalContext, ramOffset, pbuff, blocklen);
			ramOffset += blocklen;
		}
		fclose(file);
		free(pbuff);
	}
	return FileSz;
#elif defined(FT900_PLATFORM) || defined(FT93X_PLATFORM) || defined(ARDUINO_PLATFORM)
	return 0;
#endif
}

/**
 * @brief API to demonstrate number base
 *
 */
void NumberBases() {
	EVE_CoCmd_dlStart(s_pHalContext);
	EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1));
	EVE_CoCmd_text(s_pHalContext, (int16_t)(s_pHalContext->Width / 2), 50, 31, OPT_CENTER, "Built-in bases conversion");
	EVE_CoCmd_text(s_pHalContext, (int16_t)(s_pHalContext->Width / 2), 100, 31, OPT_CENTER, "bases from 2 to 32:");

#ifdef DISPLAY_RESOLUTION_WVGA
	EVE_CoCmd_text(s_pHalContext, (int16_t)(s_pHalContext->Width / 2) - 130, 150, 30, 0, "Binary(2):");
	EVE_CoCmd_setBase(s_pHalContext, 2);
	EVE_CoCmd_number(s_pHalContext, (int16_t)(s_pHalContext->Width / 2 + 80), 150, 30, 0, 1000);

	EVE_CoCmd_text(s_pHalContext, (int16_t)(s_pHalContext->Width / 2 - 130), 200, 30, 0, "Octal(8):");
	EVE_CoCmd_setBase(s_pHalContext, 8);
	EVE_CoCmd_number(s_pHalContext, (int16_t)(s_pHalContext->Width / 2 + 80), 200, 30, 0, 1000);

	EVE_CoCmd_text(s_pHalContext, (int16_t)(s_pHalContext->Width / 2 - 130), 250, 30, 0, "Decimal(10):");
	EVE_CoCmd_setBase(s_pHalContext, 10);
	EVE_CoCmd_number(s_pHalContext, (int16_t)(s_pHalContext->Width / 2 + 80), 250, 30, 0, 1000);

	EVE_CoCmd_text(s_pHalContext, (int16_t)(s_pHalContext->Width / 2 - 130), 300, 30, 0, "Hex(16):");
	EVE_CoCmd_setBase(s_pHalContext, 16);
	EVE_CoCmd_number(s_pHalContext, (int16_t)(s_pHalContext->Width / 2 + 80), 300, 30, 0, 1000);
#else
	EVE_CoCmd_text(s_pHalContext, (int16_t)(s_pHalContext->Width / 2 - 130), 150, 30, 0, "Binary(2):");
	EVE_CoCmd_setBase(s_pHalContext, 2);
	EVE_CoCmd_number(s_pHalContext, s_pHalContext->Width / 2 + 80, 150, 30, 0, 1000);

	EVE_CoCmd_text(s_pHalContext, (int16_t)(s_pHalContext->Width / 2 - 130), 180, 30, 0, "Octal(8):");
	EVE_CoCmd_setBase(s_pHalContext, 8);
	EVE_CoCmd_number(s_pHalContext, s_pHalContext->Width / 2 + 80, 180, 30, 0, 1000);

	EVE_CoCmd_text(s_pHalContext, (int16_t)(s_pHalContext->Width / 2 - 130), 230, 30, 0, "Decimal(10):");
	EVE_CoCmd_setBase(s_pHalContext, 10);
	EVE_CoCmd_number(s_pHalContext, s_pHalContext->Width / 2 + 80, 230, 30, 0, 1000);

	EVE_CoCmd_text(s_pHalContext, (int16_t)(s_pHalContext->Width / 2 - 130), 280, 30, 0, "Hex(16):");
	EVE_CoCmd_setBase(s_pHalContext, 16);
	EVE_CoCmd_number(s_pHalContext, s_pHalContext->Width / 2 + 80, 280, 30, 0, 1000);
#endif

	//Revert the base back to decimal because numeric base is part of the global context.
	EVE_CoCmd_setBase(s_pHalContext, 10);

	EVE_Cmd_wr32(s_pHalContext, DISPLAY());
	EVE_CoCmd_swap(s_pHalContext);
	EVE_Cmd_waitFlush(s_pHalContext);
	SAMAPP_ENABLE_DELAY_VALUE(3000);
}

#define SCRATCH_BUFF_SZ		(1024*4)

/**
 * @brief this function demonstrates the usage of the paletted bitmap converted by the BRIDGETEK palette converter
 *
 */
void SAMAPP_81X_Paletted_Bitmap() {
#if defined(MSVC_PLATFORM) || defined(BT8XXEMU_PLATFORM)
	uint16_t bitmapHeight = 128;
	uint16_t bitmapWidth = 128;

	EVE_CoCmd_dlStart(s_pHalContext);
	EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1));
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 255, 255));

	SAMAPP_LoadRawFromFile(TEST_DIR "\\Tomato_lut.raw", RAM_G);
	SAMAPP_LoadRawFromFile(TEST_DIR "\\Tomato_index.raw", 1024);

	EVE_Cmd_wr32(s_pHalContext, BEGIN(BITMAPS));
	EVE_Cmd_wr32(s_pHalContext, PALETTE_SOURCE(RAM_G));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_SOURCE(1024));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_LAYOUT(PALETTED4444, bitmapWidth, bitmapHeight));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_LAYOUT_H(((bitmapWidth * 2L) >> 10), ((bitmapHeight) >> 9)));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_SIZE(NEAREST, BORDER, BORDER, bitmapWidth, bitmapHeight));
	EVE_Cmd_wr32(s_pHalContext, VERTEX2F((s_pHalContext->Width / 2 - bitmapWidth / 2) * 16, (s_pHalContext->Height / 2 - bitmapHeight / 2 - bitmapHeight) * 16));

	EVE_Cmd_wr32(s_pHalContext, DISPLAY());
	EVE_CoCmd_swap(s_pHalContext);
	EVE_Cmd_waitFlush(s_pHalContext);
	SAMAPP_ENABLE_DELAY_VALUE(2000);
#endif
}

/**
 * @brief Multi touch on a single tracked object can be individually tracked to save the MCU calculations on rotary and linear tracking. Maximum of 5 trackers.
 *
 */
void SAMAPP_CoPro_MultiTracker() {
	uint32_t trackers[5];
	uint32_t delayLoop = 300;
	uint32_t trackerVal;
	uint8_t tagval;
	uint8_t RDialTag = 100;
	uint8_t GDialTag = 101;
	uint8_t BDialTag = 102;
	uint8_t ADialTag = 103;
	uint8_t DialR = (uint8_t)(s_pHalContext->Width / 10);
	uint8_t rectRed = 0;
	uint8_t rectGreen = 0;
	uint8_t rectBlue = 0;
	uint8_t rectAlpha = 255;

	uint16_t RDialX = DialR + 20;
	uint16_t RDialY = (uint16_t)s_pHalContext->Height / 4;
	uint16_t GDialX = DialR + 20;
	uint16_t GDialY = RDialY * 3;
	uint16_t BDialX = (uint16_t)(s_pHalContext->Width - 20 - DialR);
	uint16_t BDialY = (uint16_t)(s_pHalContext->Height / 4);
	uint16_t ADialX = (uint16_t)(s_pHalContext->Width - 20 - DialR);
	uint16_t ADialY = BDialY * 3;
	uint16_t rectWidth = (uint16_t)(s_pHalContext->Width / 2.6);
	uint16_t rectHeight = (uint16_t)(s_pHalContext->Height / 2);
	uint16_t rectX = (uint16_t)(s_pHalContext->Width / 2 - rectWidth / 2);
	uint16_t rectY = (uint16_t)(s_pHalContext->Height / 2 - rectHeight / 2);
	uint16_t RDialTrackVal = 0;
	uint16_t GDialTrackVal = 0;
	uint16_t BDialTrackVal = 0;
	uint16_t ADialTrackVal = 65535;

	EVE_CoCmd_track(s_pHalContext, RDialX, RDialY, 1, 1, RDialTag);
	EVE_CoCmd_track(s_pHalContext, GDialX, GDialY, 1, 1, GDialTag);
	EVE_CoCmd_track(s_pHalContext, BDialX, BDialY, 1, 1, BDialTag);
	EVE_CoCmd_track(s_pHalContext, ADialX, ADialY, 1, 1, ADialTag);
#ifndef RESISTANCE_THRESHOLD
	EVE_Hal_wr8(s_pHalContext, REG_CTOUCH_EXTENDED, CTOUCH_MODE_EXTENDED);
#endif
	while (delayLoop != 0) {
		trackers[0] = EVE_Hal_rd32(s_pHalContext, REG_TRACKER);
		trackers[1] = EVE_Hal_rd32(s_pHalContext, REG_TRACKER_1);
		trackers[2] = EVE_Hal_rd32(s_pHalContext, REG_TRACKER_2);
		trackers[3] = EVE_Hal_rd32(s_pHalContext, REG_TRACKER_3);

		for (uint8_t i = 0; i < 4; i++) {
			tagval = (trackers[i] & 0xff);
			trackerVal = (trackers[i] >> 16) & 0xffff;
			if (tagval == RDialTag) {
				rectRed = (uint8_t)(trackerVal * 255 / 65536);
				RDialTrackVal = (uint16_t)trackerVal;
			}
			else if (tagval == GDialTag) {
				rectGreen = (uint8_t)(trackerVal * 255 / 65536);
				GDialTrackVal = (uint16_t)trackerVal;
			}
			else if (tagval == BDialTag) {
				rectBlue = (uint8_t)(trackerVal * 255 / 65536);
				BDialTrackVal = (uint16_t)trackerVal;
			}
			else if (tagval == ADialTag) {
				rectAlpha = (uint8_t)(trackerVal * 255 / 65536);
				ADialTrackVal = (uint16_t)trackerVal;
			}
		}
		EVE_CoCmd_dlStart(s_pHalContext); // clear screen
		EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1));
		EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 255, 255));
		EVE_Cmd_wr32(s_pHalContext, COLOR_A(255));
		EVE_Cmd_wr32(s_pHalContext, TAG_MASK(1));
		EVE_Cmd_wr32(s_pHalContext, TAG(RDialTag));
		EVE_CoCmd_dial(s_pHalContext, RDialX, RDialY, DialR, 0, RDialTrackVal);
		EVE_Cmd_wr32(s_pHalContext, TAG(GDialTag));
		EVE_CoCmd_dial(s_pHalContext, GDialX, GDialY, DialR, 0, GDialTrackVal);
		EVE_Cmd_wr32(s_pHalContext, TAG(BDialTag));
		EVE_CoCmd_dial(s_pHalContext, BDialX, BDialY, DialR, 0, BDialTrackVal);
		EVE_Cmd_wr32(s_pHalContext, TAG(ADialTag));
		EVE_CoCmd_dial(s_pHalContext, ADialX, ADialY, DialR, 0, ADialTrackVal);
		EVE_Cmd_wr32(s_pHalContext, TAG_MASK(0));

		EVE_CoCmd_text(s_pHalContext, RDialX, RDialY, 28, OPT_CENTER, "Red"); //text info
		EVE_CoCmd_text(s_pHalContext, GDialX, GDialY, 28, OPT_CENTER, "Green"); //text info
		EVE_CoCmd_text(s_pHalContext, BDialX, BDialY, 28, OPT_CENTER, "Blue"); //text info
		EVE_CoCmd_text(s_pHalContext, ADialX, ADialY, 28, OPT_CENTER, "Alpha"); //text info

		EVE_Cmd_wr32(s_pHalContext, BEGIN(RECTS));
		EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(rectRed, rectGreen, rectBlue));
		EVE_Cmd_wr32(s_pHalContext, COLOR_A(rectAlpha));
		EVE_Cmd_wr32(s_pHalContext, VERTEX2F(rectX * 16, rectY * 16));
		EVE_Cmd_wr32(s_pHalContext, VERTEX2F((rectX + rectWidth) * 16, (rectY + rectHeight) * 16));

		EVE_Cmd_wr32(s_pHalContext, DISPLAY());
		EVE_CoCmd_swap(s_pHalContext);
		/* Wait till coprocessor completes the operation */
		EVE_Cmd_waitFlush(s_pHalContext);

		delayLoop--;
	}
}

#if defined(EVE_FLASH_AVAILABLE)

/**
 * @brief Test AVI video playback from flash
 *
 */
void testVideoPlayBackFromFlash() {
	const uint32_t flashSource = 4096;

	if (!FlashHelper_SwitchFullMode(s_pHalContext)) {
		APP_ERR("SwitchFullMode failed");
		return;
	}

	EVE_CoCmd_dlStart(s_pHalContext);
	EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1));
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 255, 255));

	EVE_CoCmd_flashSource(s_pHalContext, flashSource);
	EVE_CoCmd_playVideo(s_pHalContext, OPT_FULLSCREEN | OPT_FLASH | OPT_SOUND | OPT_NOTEAR);

	EVE_Cmd_waitFlush(s_pHalContext); //Video plays after this
	EVE_CoCmd_nop(s_pHalContext);
	EVE_Cmd_waitFlush(s_pHalContext);
	EVE_sleep(5000);
	EVE_Hal_wr8(s_pHalContext, REG_PLAY_CONTROL, -1);
}
#endif

/**
 * @brief Reset Eve_Hal framework
 *
 */
void environmentReset() {
	Gpu_Release(s_pHalContext);
	Gpu_Init(s_pHalContext);

#if !defined(BT8XXEMU_PLATFORM) && GET_CALIBRATION == 1
	Calibration_Restore(s_pHalContext);
#endif
}

/**
 * @brief Draw set 8
 *
 */
void App_Set_08_Draw() {
	Draw_Text(s_pHalContext, "Multi-tracker usage");
	SAMAPP_CoPro_MultiTracker();

	Draw_Text(s_pHalContext, "Number Bases");
	NumberBases();

	Draw_Text(s_pHalContext, "Screen Orientation");
	ScreenOrientation();

	Draw_Text(s_pHalContext, "Bitmap up to 2048pix x 2048pix.");
	HigherResolutionBitmap();

#if defined(MSVC_PLATFORM) || defined(BT8XXEMU_PLATFORM)
	Draw_Text(s_pHalContext, "Paletted4444 bitmap.");
	SAMAPP_81X_Paletted_Bitmap();

	Draw_Text(s_pHalContext, "Paletted8 format.");
	SAMAPP_GPU_Bitmap_Paletted8();
#endif

	Draw_Text(s_pHalContext, "Display DXT1 format bitmap");
	SAMAPP_GPU_DXT1();

#if defined(MSVC_PLATFORM) || defined(BT8XXEMU_PLATFORM)
	environmentReset();
	
	Draw_Text(s_pHalContext, "AVI video playback via MediaFifo");
	VideoPlayback();
	
	Draw_Text(s_pHalContext, "AVI video playback via CMD buffer");
	VideoPlaybackViaCMDBuff();
	
	Draw_Text(s_pHalContext, "AVI video playback via frame by frame");
	VideoPlayBackFrameByFrame();
	
	Draw_Text(s_pHalContext, "Test AVI video playback via \nREG_CMDB_WRITE/REG_CMDB_SPACE");
	testVideoPlayBackCmdBuff();

#if defined(EVE_FLASH_AVAILABLE)
	Draw_Text(s_pHalContext, "Test AVI video playback from flash");
	testVideoPlayBackFromFlash();

	Draw_Text(s_pHalContext, "AVI video playback via frame by frame from flash");
	VideoPlayBackFrameByFrame_FromFlash();
#endif

	testVideoPlayBackCmdBuff_ASTC();
#endif
}

/**
 * @brief Init set 8
 *
 */
void App_Set_08_Init(EVE_HalContext* pHalContext) {
	s_pHalContext = pHalContext;
	Draw_Text(s_pHalContext, "Set8   Start!");
}

/**
 * @brief Deinit set 8
 *
 */
void App_Set_08_Deinit() {
	Draw_Text(s_pHalContext, "Set8   End!");
}

#endif // SAMAPP_ENABLE_APIS_SET8
