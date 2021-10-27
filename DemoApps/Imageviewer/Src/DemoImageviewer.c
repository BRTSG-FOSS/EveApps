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
  * Description: Demo application for BT815A platform
  */

#include "Platform.h"
#include "EVE_CoCmd.h"
#include "Common.h"
#include "App.h"

static EVE_HalContext *s_pHalContext;

#if defined(FT9XX_PLATFORM)
#include "ff.h"
#endif

#if defined(FT9XX_PLATFORM)
FATFS FatFs;
FIL CurFile;
FRESULT fResult;
#endif

typedef struct {
	char name[200];
	uint32_t file_size;
} Image;
Image Image_prop[20];

static uint16_t cts = 0;
FILE *pfile;
static uint32_t filesize;

static uchar8_t istouch() {
	return !(EVE_Hal_rd16(s_pHalContext, REG_TOUCH_RAW_XY) & 0x8000);
}

uint8_t isHighResolution(char *fn) {
	uint8_t fileLength = strlen(fn);
	if (fileLength > 5 && fn[fileLength - 5] == 'H' && fn[fileLength - 4] == '.'
		&& (fn[fileLength - 3] == 'j' || fn[fileLength - 3] == 'J')
		&& (fn[fileLength - 2] == 'p' || fn[fileLength - 2] == 'P')
		&& (fn[fileLength - 1] == 'g' || fn[fileLength - 1] == 'G'))
		return 1;
	else
		return 0;
}

#define MAX_FILES_TO_RETRIEVE 20

uint16_t List_Out_files() {
	uint16_t nooffiles = 0;

#if defined(MSVC_PLATFORM)|| defined(BT8XXEMU_PLATFORM)
	char *path = TEST_DIR "";
	struct _finddata_t Files;
	long file_spec;
	if (!_chdir(path))
	{
#if defined(DISPLAY_RESOLUTION_WQVGA) || defined(DISPLAY_RESOLUTION_QVGA)
		file_spec = _findfirst("*.jpg*", &Files);
#else
		file_spec = _findfirst("*H.jpg*", &Files);
#endif
	}
	else
		return nooffiles;
#if defined(DISPLAY_RESOLUTION_WQVGA) || defined(DISPLAY_RESOLUTION_QVGA)
	if (!isHighResolution(Files.name)) {
		strcpy_P(Image_prop[nooffiles].name, Files.name);
		Image_prop[nooffiles].file_size = (Files.size + 3)&~3;
		nooffiles++;
	}
#else
	strcpy_P(Image_prop[nooffiles].name, Files.name);
	Image_prop[nooffiles].file_size = (Files.size + 3)&~3;
	nooffiles++;
#endif

	while (_findnext(file_spec, &Files) == 0)
	{
#if defined(DISPLAY_RESOLUTION_WQVGA) || defined(DISPLAY_RESOLUTION_QVGA)
		if (!isHighResolution(Files.name)) {
			strcpy_P(Image_prop[nooffiles].name, Files.name);
			Image_prop[nooffiles].file_size = (Files.size + 3)&~3;
			nooffiles++;
		}
#else
		strcpy_P(Image_prop[nooffiles].name, Files.name);
		Image_prop[nooffiles].file_size = (Files.size + 3)&~3;
		nooffiles++;
#endif
	}
	return nooffiles;
#endif

#if defined(FT9XX_PLATFORM)
	FRESULT res;
	DIR dir;
	FILINFO fno;
	uint8_t i = 1, fileLength;
	char *fn;

	res = f_opendir(&dir, TEST_DIR "");

	if (res == FR_OK) {
#define MAX_FILES 100
		for (int loop=0;loop<MAX_FILES;loop++) {
			res = f_readdir(&dir, &fno);
			if (res != FR_OK || fno.fname[0] == 0)
				break;
			if (fno.fname[0] == '.')
				continue;
			fn = fno.fname;
			if (fno.fattrib & AM_DIR) {
			}
			else {
				fileLength = strlen(fn);
#if defined(DISPLAY_RESOLUTION_WVGA)
				if (fileLength > 5 && fn[fileLength - 5] == 'H' && fn[fileLength - 4] == '.'
					&& (fn[fileLength - 3] == 'j' || fn[fileLength - 3] == 'J')
					&& (fn[fileLength - 2] == 'p' || fn[fileLength - 2] == 'P')
					&& (fn[fileLength - 1] == 'g' || fn[fileLength - 1] == 'G')) {
#else
				if (fileLength>5 && fn[fileLength - 5] != 'H' && fn[fileLength - 4] == '.' && (fn[fileLength - 3] == 'j' || fn[fileLength - 3] == 'J') && (fn[fileLength - 2] == 'p' || fn[fileLength - 2] == 'P') && (fn[fileLength - 1] == 'g' || fn[fileLength - 1] == 'G')) {
#endif
					strcpy(Image_prop[nooffiles].name, fn);
					Image_prop[nooffiles].file_size = fno.fsize;
					printf("Num:%d , Name:%s , Size:%d\n", nooffiles, Image_prop[nooffiles].name,
						Image_prop[nooffiles].file_size);
					nooffiles++;
					if (nooffiles >= MAX_FILES_TO_RETRIEVE)
						break;
				}
				}
			}
		}
	else {
		printf("\n fail");
	}
	return nooffiles;
#endif
	}

void Load_Jpeg() {
	uint8_t imbuff[8192];
	int32_t blocklen;
	int32_t bytesRead;
	while (filesize > 0) {
		blocklen = filesize > 8192 ? 8192 : filesize;
#if defined(MSVC_PLATFORM) || defined(BT8XXEMU_PLATFORM)
		bytesRead = fread(imbuff, 1, blocklen, pfile); /* copy the data into pbuff and then transfter it to command buffer */
#elif defined(FT9XX_PLATFORM)
		fResult = f_read(&CurFile, imbuff, blocklen, &bytesRead);
#endif
		filesize -= blocklen;
		EVE_Cmd_wrMem(s_pHalContext, imbuff, blocklen); /* copy data continuously into command memory */
	}
#if defined(MSVC_PLATFORM) || defined(BT8XXEMU_PLATFORM)
	fclose(pfile); /* close the opened jpg file */
#elif defined(FT9XX_PLATFORM)
	f_close(&CurFile);
#endif
}

static uint16_t nooffiles = 0;
void Loadimage2ram(uint8_t bmphandle) {
#if defined(MSVC_PLATFORM) || defined(BT8XXEMU_PLATFORM)
	pfile = fopen(Image_prop[cts].name, "rb");			// read Binary (rb)
#elif defined(FT9XX_PLATFORM)
	char newName[160] = TEST_DIR "\\";
	char *dest;
	do {
		memset(newName, 0, sizeof(newName));
		strcat(newName, TEST_DIR "\\");

		dest = strcat(newName, Image_prop[cts].name);
	} while (dest != newName);
	fResult = f_open(&CurFile, newName, FA_READ | FA_OPEN_EXISTING);
	if (fResult != FR_OK)
		printf("Unable to open file: %s", newName);
	fflush(stdout);
#endif

	filesize = Image_prop[cts].file_size;
	cts++;
	if (cts > (nooffiles - 1))
		cts = 0;

#if defined(MSVC_PLATFORM) || defined(BT8XXEMU_PLATFORM)
	fseek(pfile, 0, SEEK_SET);
#endif

	EVE_Cmd_wr32(s_pHalContext, CMD_LOADIMAGE);
#if defined(DISPLAY_RESOLUTION_QVGA) || defined(DISPLAY_RESOLUTION_WQVGA) || defined(DISPLAY_RESOLUTION_HVGA_PORTRAIT)
	EVE_Cmd_wr32(s_pHalContext, (bmphandle ? 131072L : 100));
#else
	EVE_Cmd_wr32(s_pHalContext, (bmphandle ? 400000L : 300));  //different ram location for bigger bitmap

#endif
	EVE_Cmd_wr32(s_pHalContext, OPT_NODL);
	fflush(stdout);
	Load_Jpeg();
}

void DemoImageviewer(EVE_HalContext* pHalContext) {
	s_pHalContext = pHalContext;
	uint16_t i, xv;
	uint16_t aspect_ratio = 0;
	uint16_t transform = 273L, boot = 1, imgWidth = 512, imgHeight = 308, refGradHeight = 64;

#if defined(DISPLAY_RESOLUTION_WQVGA) || defined(DISPLAY_RESOLUTION_QVGA) || defined(DISPLAY_RESOLUTION_HVGA_PORTRAIT)
	int32_t px = ((s_pHalContext->Width - 320) / 2) + aspect_ratio,
		x = ((s_pHalContext->Width - 320) / 2) + aspect_ratio,
		tracker = 0, temp_x;
#else
	int32_t px = ((s_pHalContext->Width - 512) / 2) + aspect_ratio, x = ((s_pHalContext->Width - 512) / 2) + aspect_ratio;
	int32_t tracker = 0, temp_x = 0;
#endif

	uint8_t r = 1, loaded = 0, Touch_detected = 0;

	temp_x = x;

	EVE_Hal_wr16(s_pHalContext, REG_VOL_SOUND, 100);
	EVE_Hal_wr16(s_pHalContext, REG_SOUND, 0x51);

	nooffiles = List_Out_files();
	if (nooffiles == 0) {
		printf("\n Files not found");
		return;
	}

	Loadimage2ram(r);
#if defined(DISPLAY_RESOLUTION_WQVGA) || defined(DISPLAY_RESOLUTION_QVGA) || defined(DISPLAY_RESOLUTION_HVGA_PORTRAIT)
	for (i = 0; i < 64; i++)
		EVE_Hal_wr8(s_pHalContext, i, 96 - (3 * i / 2));
#else
	for (i = 0; i < 180; i++) {
		EVE_Hal_wr8(s_pHalContext, i, 90 - (i / 2));
	}
#endif
	App_Set_CmdBuffer_Index(0);
	EVE_CoCmd_dlStart(s_pHalContext);
	EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_HANDLE(r));
#if defined(DISPLAY_RESOLUTION_WQVGA) || defined(DISPLAY_RESOLUTION_QVGA) || defined(DISPLAY_RESOLUTION_HVGA_PORTRAIT)
	EVE_Cmd_wr32(s_pHalContext, BITMAP_SOURCE((r ? 131072L : 100)));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_LAYOUT(RGB565, 320L * 2, 194));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_SIZE(NEAREST, BORDER, BORDER, 320, 194));
#else
	EVE_Cmd_wr32(s_pHalContext, BITMAP_SOURCE((r ? 400000L : 300)));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_LAYOUT(RGB565, imgWidth * 2, imgHeight));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_LAYOUT_H((imgWidth * 2) >> 10, imgHeight >> 9));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_SIZE(NEAREST, BORDER, BORDER, imgWidth, imgHeight));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_SIZE_H(imgWidth >> 9, imgHeight >> 9));
#endif
	//reflection gradient
#if defined(DISPLAY_RESOLUTION_WQVGA) || defined(DISPLAY_RESOLUTION_QVGA)  || defined(DISPLAY_RESOLUTION_HVGA_PORTRAIT)
	EVE_Cmd_wr32(s_pHalContext, BITMAP_HANDLE(2));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_SOURCE(0));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_LAYOUT(L8, 1, 64));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_SIZE(NEAREST, REPEAT, BORDER, s_pHalContext->Width, 64));
#else
	EVE_Cmd_wr32(s_pHalContext, BITMAP_HANDLE(2));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_SOURCE(0));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_LAYOUT(L8, 1, 180));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_SIZE(NEAREST, REPEAT, BORDER, s_pHalContext->Width, 180));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_SIZE_H(s_pHalContext->Width >> 9, 0));
#endif
	EVE_Cmd_wr32(s_pHalContext, BEGIN(BITMAPS));
#if defined(DISPLAY_RESOLUTION_WQVGA) || defined(DISPLAY_RESOLUTION_QVGA) || defined(DISPLAY_RESOLUTION_HVGA_PORTRAIT)
	EVE_Cmd_wr32(s_pHalContext, VERTEX2II(x, (10 + aspect_ratio), r, 0));
#else
	EVE_Cmd_wr32(s_pHalContext, BITMAP_HANDLE(r));
	EVE_Cmd_wr32(s_pHalContext, CELL(0));
	EVE_Cmd_wr32(s_pHalContext, VERTEX2F(x * 16, (10 + aspect_ratio) * 16));
#endif
	EVE_Cmd_wr32(s_pHalContext, DISPLAY());
	EVE_CoCmd_swap(s_pHalContext);
	EVE_Cmd_waitFlush(s_pHalContext);

	while (1) {
		EVE_CoCmd_dlStart(s_pHalContext);
		EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1));
		EVE_CoCmd_gradient(s_pHalContext, 0, s_pHalContext->Height / 2, 0x000000, 0, s_pHalContext->Height, 0x605040);
		EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 255, 255));
		EVE_Cmd_wr32(s_pHalContext, BEGIN(BITMAPS));
		if (x != temp_x) {
			EVE_Cmd_wr32(s_pHalContext, BITMAP_HANDLE(r ^ 1));
			EVE_Cmd_wr32(s_pHalContext, CELL(0));
#if defined(DISPLAY_RESOLUTION_QVGA) || defined(DISPLAY_RESOLUTION_WQVGA)  ||  defined(DISPLAY_RESOLUTION_HVGA_PORTRAIT)
			EVE_Cmd_wr32(s_pHalContext, VERTEX2F(16 * (x - 400), 16 * (10 + aspect_ratio)));
#else
			EVE_Cmd_wr32(s_pHalContext, VERTEX2F(16 * (x - 656), 16 * (10 + aspect_ratio)));
#endif
		}

#if defined(DISPLAY_RESOLUTION_QVGA) || defined(DISPLAY_RESOLUTION_WQVGA)  ||  defined(DISPLAY_RESOLUTION_HVGA_PORTRAIT)
		EVE_Cmd_wr32(s_pHalContext, VERTEX2II(x, (10 + aspect_ratio), r, 0));
#else
		EVE_Cmd_wr32(s_pHalContext, BITMAP_HANDLE(r));
		EVE_Cmd_wr32(s_pHalContext, VERTEX2F(x * 16, (10 + aspect_ratio) * 16));
#endif
		EVE_Cmd_wr32(s_pHalContext, SAVE_CONTEXT());
		EVE_Cmd_wr32(s_pHalContext, COLOR_MASK(0, 0, 0, 1));
		EVE_Cmd_wr32(s_pHalContext, BLEND_FUNC(ONE, ZERO));

#if defined(DISPLAY_RESOLUTION_QVGA) ||  defined(DISPLAY_RESOLUTION_HVGA_PORTRAIT)
		EVE_Cmd_wr32(s_pHalContext, VERTEX2II(x, 207, 2, 0));
#elif defined(DISPLAY_RESOLUTION_WQVGA)
		EVE_Cmd_wr32(s_pHalContext, VERTEX2II(0, 212, 2, 0));
#else
		EVE_Cmd_wr32(s_pHalContext, VERTEX2II(0, 300, 2, 0));
#endif
		EVE_Cmd_wr32(s_pHalContext, COLOR_MASK(1, 1, 1, 1));
		EVE_Cmd_wr32(s_pHalContext, BLEND_FUNC(DST_ALPHA, ONE_MINUS_DST_ALPHA));

#if defined(DISPLAY_RESOLUTION_QVGA) || defined(DISPLAY_RESOLUTION_WQVGA)  ||  defined(DISPLAY_RESOLUTION_HVGA_PORTRAIT)
		EVE_CoCmd_loadIdentity(s_pHalContext);
		EVE_CoCmd_translate(s_pHalContext, (temp_x) * 65536L, 65536L * 96.5);
		EVE_CoCmd_scale(s_pHalContext, 1 * 65536, 65536 * -1);
		EVE_CoCmd_translate(s_pHalContext, -(temp_x) * 65536L, 65536L * -96.5);
		EVE_CoCmd_setMatrix(s_pHalContext);
#else
		EVE_CoCmd_loadIdentity(s_pHalContext);
		EVE_CoCmd_translate(s_pHalContext, 256 * 65536L, 65536L * 150);
		EVE_CoCmd_scale(s_pHalContext, 1 * 65536, 65536 * -1);
		EVE_CoCmd_translate(s_pHalContext, -256 * 65536L, 65536L * -150);
		EVE_CoCmd_setMatrix(s_pHalContext);
#endif

		if (x != temp_x) {
			EVE_Cmd_wr32(s_pHalContext, BITMAP_HANDLE(r ^ 1));
			EVE_Cmd_wr32(s_pHalContext, CELL(0));
#if defined(DISPLAY_RESOLUTION_QVGA) || defined(DISPLAY_RESOLUTION_WQVGA)  ||  defined(DISPLAY_RESOLUTION_HVGA_PORTRAIT)
			EVE_Cmd_wr32(s_pHalContext, VERTEX2F(16 * (x - 400), 16 * 212));
#else
			EVE_Cmd_wr32(s_pHalContext, VERTEX2F(16 * (x - 656), 16 * 365));
#endif
		}
#if defined(DISPLAY_RESOLUTION_QVGA) ||  defined(DISPLAY_RESOLUTION_HVGA_PORTRAIT)
		EVE_Cmd_wr32(s_pHalContext, VERTEX2II(x, 207, r, 0));
#elif defined(DISPLAY_RESOLUTION_WQVGA)
		EVE_Cmd_wr32(s_pHalContext, VERTEX2II(x, 212, r, 0));
#else
		EVE_Cmd_wr32(s_pHalContext, BITMAP_HANDLE(r));
		EVE_Cmd_wr32(s_pHalContext, VERTEX2F(x * 16, 365 * 16));
#endif
		EVE_Cmd_wr32(s_pHalContext, RESTORE_CONTEXT());
		if (px == temp_x && loaded == 0 && boot == 0) {
#if defined(DISPLAY_RESOLUTION_QVGA) || defined(DISPLAY_RESOLUTION_WQVGA)  ||  defined(DISPLAY_RESOLUTION_HVGA_PORTRAIT)
			EVE_Cmd_wr32(s_pHalContext, BITMAP_HANDLE(r ^ 1));
			EVE_Cmd_wr32(s_pHalContext, BITMAP_SOURCE(((r ^ 1) ? 131072L : 100)));
			EVE_Cmd_wr32(s_pHalContext, BITMAP_LAYOUT(RGB565, 320L * 2, 194));
			EVE_Cmd_wr32(s_pHalContext, BITMAP_SIZE(NEAREST, BORDER, BORDER, 320, 194));
#else
			EVE_Cmd_wr32(s_pHalContext, BITMAP_HANDLE(r ^ 1));
			EVE_Cmd_wr32(s_pHalContext, BITMAP_SOURCE(((r ^ 1) ? 400000L : 300)));
			EVE_Cmd_wr32(s_pHalContext, BITMAP_LAYOUT(RGB565, imgWidth * 2, imgHeight));
			EVE_Cmd_wr32(s_pHalContext, BITMAP_LAYOUT_H((imgWidth * 2) >> 10, imgHeight >> 9));
			EVE_Cmd_wr32(s_pHalContext, BITMAP_SIZE(NEAREST, BORDER, BORDER, imgWidth, imgHeight));
			EVE_Cmd_wr32(s_pHalContext, BITMAP_SIZE_H(imgWidth >> 9, imgHeight >> 9));
#endif
			Loadimage2ram(r ^ 1);
			loaded = 1;
		}

		EVE_Cmd_wr32(s_pHalContext, DISPLAY());
		EVE_CoCmd_swap(s_pHalContext);
		EVE_Cmd_waitFlush(s_pHalContext);
		boot = 0;
		tracker = EVE_Hal_rd32(s_pHalContext, REG_TOUCH_SCREEN_XY);
		if (loaded && x == temp_x && tracker != 0x80008000) {
			EVE_Hal_wr8(s_pHalContext, REG_PLAY, 1);	// Play the Sound
			x = s_pHalContext->Width;
			r ^= 1;
			loaded = 0;
		}
		xv = 1 + x / 16;
		px = x;
		x = MAX(temp_x, x - xv);
	}
}

