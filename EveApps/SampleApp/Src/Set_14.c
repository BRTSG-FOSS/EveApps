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
  *  Description: This is sample demo for extended font format and its glyph data in RAM_G
  */
#include <math.h>

#include "Platform.h"
#include "Gpu_Hal.h"
#include "Common.h"
#include "App.h"
#include "Samapp_Enable_API.h"

#ifdef SAMAPP_ENABLE_APIS_SET14

static EVE_HalContext *s_pHalContext;

/**
  * @brief Draw an image on screen
  *
  * @param file File to load
  * @param format Image format
  * @param w Image W
  * @param h Image H
  * @param type Loading flag
  */
static void App_Show_Image(const char* file, uint16_t format, uint16_t w, uint16_t h, uint8_t type) {
	EVE_CoCmd_dlStart(s_pHalContext);
	EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1));
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 255, 255));

	Gpu_Hal_LoadImageToMemory(s_pHalContext, file, RAM_G, type);
	EVE_Cmd_waitFlush(s_pHalContext);
	EVE_CoCmd_setBitmap(s_pHalContext, RAM_G, format, w, h);

	//Start drawing bitmap
	EVE_Cmd_wr32(s_pHalContext, BEGIN(BITMAPS));
	EVE_Cmd_wr32(s_pHalContext, VERTEX2II(0, 0, 0, 0));
	EVE_Cmd_wr32(s_pHalContext, END());
	EVE_Cmd_wr32(s_pHalContext, RESTORE_CONTEXT());
	EVE_Cmd_wr32(s_pHalContext, DISPLAY());
	EVE_CoCmd_swap(s_pHalContext);
	EVE_Cmd_waitFlush(s_pHalContext);
}

#if defined(FT9XX_PLATFORM)
bool App_loadImageFile(uint32_t address, const char* filename, uint32_t otp)
{
	FRESULT fResult;
	FIL InfSrc;
#define BUFF_SIZE (8 * 1024)
	int32_t blocklen;
	int32_t filesize;
	uint8_t pbuff[BUFF_SIZE];

	fResult = f_open(&InfSrc, filename, FA_READ | FA_OPEN_EXISTING);
	if (fResult == FR_OK)
	{
		filesize = f_size(&InfSrc);

		EVE_Cmd_wr32(s_pHalContext, CMD_LOADIMAGE);
		EVE_Cmd_wr32(s_pHalContext, address);
		EVE_Cmd_wr32(s_pHalContext, OPT_NODL | otp);
		while (filesize > 0)
		{
			int32_t nBytes = filesize > BUFF_SIZE ? BUFF_SIZE : filesize;
			fResult = f_read(&InfSrc, pbuff, nBytes, &blocklen); // read a chunk of src file
			filesize -= blocklen;
			blocklen += 3;
			blocklen -= blocklen % 4;

			if (!EVE_Cmd_wrMem(s_pHalContext, (char*)pbuff, blocklen))
				break;
		}
		f_close(&InfSrc);

		if (!EVE_Cmd_waitFlush(s_pHalContext))
			return false;

		return true;
	}
	else
	{
		printf("Unable to open file: \"%s\"\n", filename);
		return false;
	}
}
#else
bool App_loadImageFile(uint32_t address, const char* filename, uint32_t otp)
{
	FILE* afile;
	uint32_t ftsize = 0;
	uint8_t pbuff[8192];
	uint16_t blocklen;

#pragma warning(push)
#pragma warning(disable : 4996)
	afile = fopen(filename, "rb"); // read Binary (rb)
#pragma warning(pop)
	if (afile == NULL)
	{
		eve_printf_debug("Unable to open: %s\n", filename);
		return 0;
	}
	EVE_Cmd_wr32(s_pHalContext, CMD_LOADIMAGE);
	EVE_Cmd_wr32(s_pHalContext, address);
	EVE_Cmd_wr32(s_pHalContext, OPT_NODL | otp);

	fseek(afile, 0, SEEK_END);
	ftsize = ftell(afile);
	fseek(afile, 0, SEEK_SET);
	while (ftsize > 0)
	{
		blocklen = ftsize > 8192 ? 8192 : (uint16_t)ftsize;
		fread(pbuff, 1, blocklen, afile); /* copy the data into pbuff and then transfter it to command buffer */
		ftsize -= blocklen;
		blocklen += 3;
		blocklen -= blocklen % 4;

		if (!EVE_Cmd_wrMem(s_pHalContext, (char*)pbuff, blocklen))
			break;
	}

	fclose(afile); /* close the opened jpg file */

	if (!EVE_Cmd_waitFlush(s_pHalContext))
		return false;

	return true;
}
#endif

/**
 * @brief API to demonstrate for CMD_CALIBRATESUB
 *
 */
void App_Cmd_Calibratesub() {
	Draw_Text(s_pHalContext, "CMD_CALIBRATESUB");

	uint16_t clb_x = 20;
	uint16_t clb_y = 20;
	uint16_t clb_w = (uint16_t)(s_pHalContext->Width / 2);
	uint16_t clb_h = (uint16_t)(s_pHalContext->Height / 2);
	uint16_t format = RGB565;
	uint16_t w = 800;
	uint16_t h = 480;
	uint8_t type = LOADIMAGE;

	Display_Start(s_pHalContext);
	Gpu_Hal_LoadImageToMemory(s_pHalContext, TEST_DIR "\\Mountain_800x480_RGB565_Converted.png", RAM_G, type);
	EVE_CoCmd_setBitmap(s_pHalContext, RAM_G, format, w, h);
	//Start drawing bitmap
	EVE_Cmd_wr32(s_pHalContext, SAVE_CONTEXT());
	EVE_Cmd_wr32(s_pHalContext, BEGIN(BITMAPS));
	EVE_Cmd_wr32(s_pHalContext, VERTEX2II(0, 0, 0, 0));
	EVE_Cmd_wr32(s_pHalContext, END());

	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0, 255, 255));
	EVE_CoCmd_text(s_pHalContext, clb_x + 260, clb_y + clb_h / 2, 30, 0,
		"Please touch on screen");

	EVE_CoCmd_fgColor(s_pHalContext, 0xb90007);
	EVE_CoCmd_calibrateSub(s_pHalContext, clb_x, clb_y, clb_w, clb_h);

	EVE_Cmd_wr32(s_pHalContext, RESTORE_CONTEXT());
	EVE_Cmd_waitFlush(s_pHalContext);
}

/**
 * @brief API to demonstrate CMD_LOADIMAGE
 * For PNG decoder, the dithering is enabled
 *
 */
void App_Sample_Dithering() {
	uint16_t format = RGB565;
	uint16_t w = 800;
	uint16_t h = 480;
	uint16_t otp = OPT_NODL;
	uint16_t address = RAM_G;

	EVE_Util_clearScreen(s_pHalContext);

	Draw_Text(s_pHalContext, "Dithering support for PNG decoder");

	for (int i = 0; i < 2; i++) {
		if (i == 0) {
			Draw_Text(s_pHalContext, "1st image (OPT_DITHER disabled)");
		}
		else {
			Draw_Text(s_pHalContext, "2nd image (OPT_DITHER enabled)");

			/// Now enable dithering support
			otp |= OPT_DITHER;
		}
		App_loadImageFile(address, TEST_DIR "\\loadimage-dither-testcase.png", otp);
		EVE_Cmd_waitFlush(s_pHalContext);

		//Start drawing bitmap
		Display_Start(s_pHalContext);
		EVE_CoCmd_setBitmap(s_pHalContext, address, format, w, h);
		EVE_Cmd_wr32(s_pHalContext, BEGIN(BITMAPS));
		EVE_Cmd_wr32(s_pHalContext, VERTEX2II(0, 0, 0, 0));
		Display_End(s_pHalContext);

		EVE_sleep(5 * 1000);
	}
}

/**
 * @brief API to demonstrate REG_UNDERRUN
 *
 */
void App_Sample_Underrun() {
	uint16_t w = 800;
	uint16_t h = 600;

	Draw_Text(s_pHalContext, "Underrun detection");

	int i = 0;
	while (i++ < 10) {
		//load bitmap file into graphics RAM
		Gpu_Hal_LoadImageToMemory(s_pHalContext, TEST_DIR "//flower_800x600.jpg", RAM_G, LOADIMAGE);
		EVE_Cmd_waitFlush(s_pHalContext);

		//Start drawing bitmap
		EVE_CoCmd_dlStart(s_pHalContext);
		EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1));
		EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 255, 255));
		//RAM_G is starting address in graphics RAM, for example 00 0000h
		EVE_CoCmd_setBitmap(s_pHalContext, RAM_G, RGB565, w, h);
		EVE_Cmd_wr32(s_pHalContext, BEGIN(BITMAPS));
		for (int j = 0; j < 100; j++) {
			EVE_Cmd_wr32(s_pHalContext, VERTEX2II(0, 0, 0, 0));
		}
		EVE_Cmd_wr32(s_pHalContext, END());
		EVE_Cmd_wr32(s_pHalContext, RESTORE_CONTEXT());
		EVE_Cmd_wr32(s_pHalContext, DISPLAY());
		EVE_CoCmd_swap(s_pHalContext);
		EVE_Cmd_waitFlush(s_pHalContext);

		if (EVE_Hal_rd32(s_pHalContext, REG_UNDERRUN) != 0) {
			Draw_Text(s_pHalContext, "Underrun detected");
			return;
		}
	}
}

/**
 * @brief API to demonstrate for Non Square pixel
 *
 */
void App_Sample_Non_Square_Display() {
#if defined(BT817_ENABLE) || defined(BT818_ENABLE)
	Draw_Text_Format(s_pHalContext, "Non Square pixel on a LCD 862x480");
#ifdef DISPLAY_RESOLUTION_1280x800
#define LCD_W 218
#define LCD_H 136
#define EXPECT_W 1280
#define EXPECT_H 800
#else
#define LCD_W 153
#define LCD_H 86
#define EXPECT_W 800
#define EXPECT_H 480
#endif

#define precision 16

	// Non-square pixel panel support in BT815A
	EVE_Hal_wr8(s_pHalContext, REG_PCLK, 1);
	Ft_Gpu_HorizontalScanoutFilter(s_pHalContext, LCD_W, LCD_H);

	Draw_Text_Format(s_pHalContext, "Round 1\n Draw a bitmap with the resolution 862x480");
	Draw_Image(s_pHalContext, TEST_DIR "/TC_NF_24_001_862x480_862x480_RGB565_Converted.png", RGB565);
	EVE_sleep(2 * 1000);

	Draw_Text_Format(s_pHalContext, "Round 2\n Draw primitive POINTS");

	Display_Start(s_pHalContext);
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0, 128, 0));
	Draw_Point(s_pHalContext, s_pHalContext->Height / 2, s_pHalContext->Height / 2, s_pHalContext->Height / 2);

	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 128, 0));
	EVE_Cmd_wr32(s_pHalContext, BEGIN(LINES));
	EVE_Cmd_wr32(s_pHalContext, VERTEX2F(0, s_pHalContext->Height / 2 * precision));
	EVE_Cmd_wr32(s_pHalContext, VERTEX2F(s_pHalContext->Height * precision, s_pHalContext->Height / 2 * precision));
	EVE_CoCmd_text(s_pHalContext, 10, (uint16_t)(s_pHalContext->Height / 2 + 10), 30, 0, "line x");

	EVE_Cmd_wr32(s_pHalContext, BEGIN(LINES));
	EVE_Cmd_wr32(s_pHalContext, VERTEX2F(s_pHalContext->Height / 2 * precision, 0));
	EVE_Cmd_wr32(s_pHalContext, VERTEX2F(s_pHalContext->Height / 2 * precision, s_pHalContext->Height * precision));
	EVE_CoCmd_text(s_pHalContext, (uint16_t)(s_pHalContext->Height / 2 + 10), 10, 30, 0, "line y");
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 255, 255));
	Display_End(s_pHalContext);
	EVE_sleep(2 * 1000);

	// Non-square pixel panel support in BT815A
	EVE_Hal_wr8(s_pHalContext, REG_PCLK, 1); /* after this display is visible on the LCD */

	// Now draw a circle
	Display_Start(s_pHalContext);
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0, 128, 0));

	uint32_t r[] = { s_pHalContext->Height * 4 / 13 , s_pHalContext->Height * 2 / 13, s_pHalContext->Height * 4 / 13 , s_pHalContext->Height * 2 / 18, 0 };
	uint32_t y[5];
	y[0] = s_pHalContext->Height - r[0] - 10;
	y[1] = y[0] - r[1] - 10;
	y[2] = s_pHalContext->Height - r[2];
	y[3] = s_pHalContext->Height - r[2] - 10;

	uint32_t  x = r[0];
	uint32_t x1;
	uint32_t y1;
	uint32_t y2;
	uint32_t distance = 0;
	for (int i = 0; i < 4; i++) {
		y2 = y[i];
		distance += r[i];

		if (i > 0) x = Math_Points_Nearby_NextX(x1, y1, y2, distance);
		Draw_Point(s_pHalContext, x, y[i], r[i]);
		x1 = x;
		y1 = y[i];
		distance = r[i];
	}
	uint32_t r5 = s_pHalContext->Height * 2 / 11;
	Draw_Point(s_pHalContext, s_pHalContext->Width - r5 - 10, r5 + 10, r5);

	Display_End(s_pHalContext);
	EVE_sleep(5 * 1000);
#endif
}

/**
 * @brief API to demonstrate 32-bit register REG_ANIM_ACTIVE
 *
 */
void App_Sample_Anime_Ative() {
#define ANIM_ADDR     (19584) // address of "anim.object" from Flash map after generating Flash

	char str[1000];
	Draw_Text_Format(s_pHalContext, "REG_ANIM_ACTIVE");

	Gpu_Hal_LoadImageToMemory(s_pHalContext, TEST_DIR "\\flash-eab-updated.bin", RAM_G, LOAD);
	EVE_Cmd_waitFlush(s_pHalContext);

	uint16_t channel = 1;
	const int16_t xAnim = (uint16_t)(s_pHalContext->Width / 2);
	const int16_t yAnim = (uint16_t)(s_pHalContext->Height / 5);

	EVE_CoCmd_animStartRam(s_pHalContext, channel, ANIM_ADDR, ANIM_ONCE);
	EVE_CoCmd_animXY(s_pHalContext, channel, xAnim, yAnim);
	const int MAX_LOOP = 10;
	for (int i = 0; i < MAX_LOOP; i++) {
		EVE_CoCmd_dlStart(s_pHalContext);
		EVE_Cmd_wr32(s_pHalContext, CLEAR_COLOR_RGB(255, 255, 255));
		EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1));

		EVE_CoCmd_animDraw(s_pHalContext, channel);

		uint32_t reg_anim_active = EVE_Hal_rd32(s_pHalContext, REG_ANIM_ACTIVE);
		EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0, 0, 0));
		snprintf(str, 1000, "REG_ANIM_ACTIVE: %u\n", reg_anim_active);
		EVE_CoCmd_text(s_pHalContext, (int16_t)(s_pHalContext->Width / 2), 50, 30, OPT_CENTER, str);

		if (0 == reg_anim_active) {
			EVE_Cmd_wr32(s_pHalContext, DISPLAY());
			EVE_CoCmd_swap(s_pHalContext);
			EVE_Hal_flush(s_pHalContext);
			break;
		}

		EVE_Cmd_wr32(s_pHalContext, DISPLAY());
		EVE_CoCmd_swap(s_pHalContext);
		EVE_Hal_flush(s_pHalContext);
		EVE_sleep(500);
	}

	EVE_sleep(2000);
}

/**
 * @brief API to demonstrate CMD_WAIT
 *
 */
void App_Sample_Cmd_Wait() {
	const uint32_t delayUs = 1000000;
	uint32_t sms = EVE_millis();

	Draw_Text(s_pHalContext, "Now make EVE wait  for 1 second");

	APP_DBG("Time start= %u", sms);
	EVE_CoCmd_wait(s_pHalContext, delayUs);
	EVE_Cmd_waitFlush(s_pHalContext);
	uint32_t ems = EVE_millis();
	APP_DBG("Time end= %u", ems);

	Draw_Text(s_pHalContext, "Done");

	EVE_sleep(1000);
}

/**
 * @brief API to demonstrate CMD_TESTCARD
 *
 */
void App_Sample_Testcard() {
	Draw_Text(s_pHalContext, "Draw a Testcard");
	EVE_CoCmd_testCard(s_pHalContext);
	EVE_Cmd_waitFlush(s_pHalContext);
	EVE_sleep(5000);
}

/**
 * @brief API to demonstrate CMD_GETIMAGE
 *
 */
void App_Sample_Cmd_Getimage() {
	uint32_t source;
	uint32_t fmt;
	uint32_t w;
	uint32_t h;
	uint32_t palette;
	source = 0;
	fmt = 4;
	w = 8;
	h = 12;
	palette = 16;

	Draw_Text(s_pHalContext, "CMD_GETIMAGE");

	// Now try to update the allocation pointer
	App_Show_Image(TEST_DIR "/CR_003_dithering.png", RGB565, 800, 600, LOADIMAGE);
	EVE_sleep(2000);

	// Now getImage properties
	EVE_CoCmd_getImage(s_pHalContext, &source, &fmt, &w, &h, &palette);

#if 0
	// check if allocation pointer is updated by BT817 API
	if (source != RAM_G ||
		fmt != RGB565 ||
		w != 800 ||
		h != 600 ||
		palette != 0
		) {
		return;
	}
#endif

	Draw_Text_Format(s_pHalContext, "Loaded image: \nsource: %u, format: %u, width: %u, height: %u, palette: %u", source, fmt, w, h, palette);
}

/**
 * @brief API to demonstrate CMD_APILEVEL
 *
 */
void App_Sample_Api_Level() {
	Draw_Text(s_pHalContext, "CMD_APILEVEL");
	Draw_Text(s_pHalContext, "Now set API level to 2 (BT817/8)");
	/// set API level to BT817
	EVE_CoCmd_apiLevel(s_pHalContext, 2);
	EVE_Cmd_waitFlush(s_pHalContext);
}

/**
 * @brief API to demonstrate for CMD_CALLLIST, CMD_NEWLIST, CMD_ENDLIST, CMD_RETURN
 *
 */
void App_Sample_Cmd_Calllist() {
	uint32_t endPtr = 4;
	uint32_t endPtrArr[5];
	const uint32_t w = 800;
	const uint32_t h = 480;
	const uint32_t x = 100;
	const uint32_t y = 100;
	uint32_t addrImg = RAM_G_SIZE - w * h * 2;
	char str[1000];

	Draw_Text(s_pHalContext, "CMD_CALLLIST, CMD_NEWLIST, CMD_ENDLIST and CMD_RETURN");

	Draw_Text(s_pHalContext, "Constructing 5 lists");
	for (int i = 0; i < 5; i++) {
		endPtrArr[i] = endPtr;

		EVE_CoCmd_newList(s_pHalContext, endPtr);
		EVE_CoCmd_setBitmap(s_pHalContext, addrImg, RGB565, w, h);

		EVE_Cmd_wr32(s_pHalContext, SAVE_CONTEXT());
		EVE_Cmd_wr32(s_pHalContext, BEGIN(BITMAPS));
		EVE_CoCmd_loadIdentity(s_pHalContext);
		EVE_CoCmd_rotateAround(s_pHalContext, w / 2, h / 2, i * 30 * 65536 / 360, 65536 * 1);
		EVE_CoCmd_setMatrix(s_pHalContext);
		EVE_Cmd_wr32(s_pHalContext, VERTEX2F((x) * 16, (y) * 16));
		EVE_Cmd_wr32(s_pHalContext, END());
		EVE_Cmd_wr32(s_pHalContext, RESTORE_CONTEXT());

		snprintf(str, 1000, "Displaying list number %u", i);
		EVE_CoCmd_text(s_pHalContext, (uint16_t)(x + i * 20), (uint16_t)(y + i * 20), 28, 0, str);
		EVE_CoCmd_endList(s_pHalContext);

		EVE_Cmd_waitFlush(s_pHalContext);
		uint32_t endAddr;
		EVE_CoCmd_getPtr(s_pHalContext, &endAddr);
		EVE_Cmd_waitFlush(s_pHalContext);

		snprintf(str, 1000, "Compiled list %u at %u, size: %u Bytes", i, endPtr, endAddr - endPtr);
		APP_DBG("%s", str);
		endPtr = endAddr;
	}

	Gpu_Hal_LoadImageToMemory(s_pHalContext, TEST_DIR "\\flower_800x480.jpg", addrImg, LOADIMAGE);
	EVE_Cmd_waitFlush(s_pHalContext);
	for (int i = 0; i < 5; i++) {
		snprintf(str, 1000, "Calling List number %u", i);
		Draw_Text(s_pHalContext, str);

		Display_Start(s_pHalContext);
		EVE_CoCmd_callList(s_pHalContext, endPtrArr[i]);
		Display_End(s_pHalContext);

		if (i == 1) {
			EVE_Cmd_waitFlush(s_pHalContext);
			uint32_t cmd = EVE_Hal_rd32(s_pHalContext, RAM_G + endPtrArr[i + 1] - 4);
			APP_DBG("Return: 0x%08x", cmd);

			if (cmd == CMD_RETURN) {
				Draw_Text(s_pHalContext, "CMD_RETURN is appended into the list");
			}
		}
		EVE_sleep(3000);
	}
	EVE_sleep(2000);
}

/**
 * @brief API to demonstrate for command list in RAM_G with alignment
 *
 */
void App_Sample_cmd_List_With_Alignment() {
	Draw_Text(s_pHalContext, "Construct a command list in RAM_G to show a button");

	//Construct a command list in RAM_G to show a button
	EVE_Hal_wr32(s_pHalContext, RAM_G + 0 * 4, SAVE_CONTEXT());
	EVE_Hal_wr32(s_pHalContext, RAM_G + 1 * 4, COLOR_RGB(125, 125, 128));
	EVE_Hal_wr32(s_pHalContext, RAM_G + 2 * 4, CMD_BUTTON);

	EVE_Hal_wr16(s_pHalContext, RAM_G + 3 * 4, 160);     //x coordinate of button
	EVE_Hal_wr16(s_pHalContext, RAM_G + 3 * 4 + 2, 160); //y coordinate of button
	EVE_Hal_wr16(s_pHalContext, RAM_G + 4 * 4, 324);     //w
	EVE_Hal_wr16(s_pHalContext, RAM_G + 4 * 4 + 2, 234); //h
	EVE_Hal_wr16(s_pHalContext, RAM_G + 5 * 4, 31); //Font handle
	EVE_Hal_wr16(s_pHalContext, RAM_G + 5 * 4 + 2, 0); //option parameter of cmd_button
	EVE_Hal_wr8(s_pHalContext, RAM_G + 6 * 4 + 0, 'T');
	EVE_Hal_wr8(s_pHalContext, RAM_G + 6 * 4 + 1, 'E');
	EVE_Hal_wr8(s_pHalContext, RAM_G + 6 * 4 + 2, 'S');
	EVE_Hal_wr8(s_pHalContext, RAM_G + 6 * 4 + 3, 'T');

	EVE_Hal_wr8(s_pHalContext, RAM_G + 7 * 4, '\0');
	EVE_Hal_wr8(s_pHalContext, RAM_G + 7 * 4 + 1, '\0');
	EVE_Hal_wr8(s_pHalContext, RAM_G + 7 * 4 + 2, '\0');
	EVE_Hal_wr8(s_pHalContext, RAM_G + 7 * 4 + 3, '\0');

	EVE_Hal_wr32(s_pHalContext, RAM_G + 8 * 4, RESTORE_CONTEXT()); //Assume 3 bytes padding bytes for alignment 
	EVE_Hal_wr32(s_pHalContext, RAM_G + 9 * 4, CMD_RETURN);  //return to the command buffer

	//Call cmd_list with data in RAM_G
	EVE_CoCmd_dlStart(s_pHalContext);
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 255, 255));
	EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1));
	EVE_CoCmd_callList(s_pHalContext, RAM_G);
	EVE_Cmd_wr32(s_pHalContext, DISPLAY());
	EVE_CoCmd_swap(s_pHalContext);
	EVE_Hal_flush(s_pHalContext);
	EVE_sleep(3 * 1000);
}

/**
 * @brief API to demonstrate CMD_FONTCACHE
 *
 */
void App_Sample_Font_Cache() {
#define UNICODE_HANDLE1   (1)
#define UNICODE_HANDLE2   (2)
	const uint32_t xfontOnRamG = RAM_G;
	const uint32_t glyphOnRamG = 4096;
	const uint32_t startOfCache = 800 * 1024;
	const uint32_t cacheSize = 160 * 1024;
	const int loopDraw = 10;
	const int loopCmdText = 10;
	const uint32_t firstchar = 0;
	uint32_t startms;
	uint32_t endms;
	uint32_t x = 10;
	uint32_t y = 10;
	uint32_t duration[2];
	uint32_t fontHandler = UNICODE_HANDLE1;

	/// Xfont address and size from .map file
	const int _add = 0;
	const int _size = 1;

	int xfont_chinese_ASTC[] = { 7680  , 192 };
	int glyph_chinese_ASTC[] = { 4096  , 3584 };

	char* intro[2] = { "ASTC font format without fontcache", "ASTC font format with fontcache" };
	const char* str = u8"\x0004\x0003\x0009\x0005\x000b\x0006\x0008\x000c\x000d\x0001\x0002\x0007";

#define flashfile "sample_utf8/sample_utf8.bin"
	Ftf_Write_File_To_Flash_With_Progressbar(s_pHalContext, TEST_DIR "/" flashfile, flashfile, 0);
	EVE_sleep(1000);

	// Switch Flash to FULL Mode
	FlashHelper_SwitchFullMode(s_pHalContext);

	// read .xfont to ramg
	APP_DBG("Copying xfont from %d by %d bytes to %d", xfont_chinese_ASTC[_add], xfont_chinese_ASTC[_size], xfontOnRamG);
	EVE_CoCmd_flashRead(s_pHalContext, xfontOnRamG, xfont_chinese_ASTC[_add], xfont_chinese_ASTC[_size]);

	// read glyph to ramg
	APP_DBG("Copying glyph from %d by %d bytes to %d", glyph_chinese_ASTC[_add], glyph_chinese_ASTC[_size], glyphOnRamG);
	EVE_CoCmd_flashRead(s_pHalContext, glyphOnRamG, glyph_chinese_ASTC[_add], glyph_chinese_ASTC[_size]);

	EVE_Cmd_waitFlush(s_pHalContext);

	for (int t = 0; t < 2; t++) {
		if (t == 0) { /// without font cache
			fontHandler = UNICODE_HANDLE1;
			Display_Start(s_pHalContext);
			EVE_CoCmd_setFont2(s_pHalContext, fontHandler, xfontOnRamG, firstchar);
			Display_End(s_pHalContext);
			EVE_Hal_flush(s_pHalContext);
		}
		else { /// with font cache
			/// Now set fontcache
			Display_Start(s_pHalContext);
			fontHandler = t + 1;
			EVE_CoCmd_setFont2(s_pHalContext, fontHandler, xfontOnRamG, firstchar);
			EVE_CoCmd_fontCache(s_pHalContext, fontHandler, startOfCache, cacheSize);
			Display_End(s_pHalContext);
			EVE_Hal_flush(s_pHalContext);
		}

		/// duration without font cache
		startms = EVE_millis();
		for (int i = 0; i < loopDraw; i++) {
			x = y = 20;
			Display_Start(s_pHalContext);
			EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0, 0, 0));

			EVE_CoCmd_text(s_pHalContext, 10, 10, 30, 0, intro[t]);
			/// Draw unicode string
			for (int k = 0; k < loopCmdText; k++) {
				y += 50;
				EVE_CoCmd_text(s_pHalContext, (uint16_t)x, (uint16_t)y, (uint16_t)fontHandler, 0, str);
			}
			Display_End(s_pHalContext);
		}
		endms = EVE_millis();
		EVE_sleep(2000);
		duration[t] = endms - startms;
	}
	EVE_Cmd_waitFlush(s_pHalContext);
	APP_DBG(
		"Duration No FontCache: %u ms\n"
		"Duration With FontCache: %u ms\n"
		"Different: %d ms (%d%%)",
		duration[0], duration[1],
		(int)(duration[0] - duration[1]),
		(int)(abs((int)duration[1] - (int)duration[0]) * 100) / duration[1]);
	EVE_sleep(2000);
}

/**
 * @brief API to demonstrate CMD_FONTCACHEQUERY
 *
 */
void App_Sample_Cmd_Fontcachequery() {
	Draw_Text_Format(s_pHalContext, "CMD_FONTCACHEQUERY");

#define UNICODE_HANDLE   (1)
	const uint32_t xfontOnRamG = RAM_G;
	const uint32_t glyphOnRamG = 4096;
	const uint32_t startOfCache = 800 * 1024;
	const uint32_t cacheSize = 160 * 1024;
	const int loopDraw = 10;
	const int loopText = 10;
	uint32_t x = 10;
	uint32_t y = 10;

	/// Xfont address and size from .map file
	const int _add = 0;
	const int _size = 1;

	int xfont_chinese_ASTC[] = { 7680  , 192 };
	int glyph_chinese_ASTC[] = { 4096  , 3584 };

	const char* str = u8"\x0004\x0003\x0009\x0005\x000b\x0006\x0008\x000c\x000d\x0001\x0002\x0007";

#define flashfile "sample_utf8/sample_utf8.bin"
	Ftf_Write_File_To_Flash_With_Progressbar(s_pHalContext, TEST_DIR "/" flashfile, flashfile, 0);
	EVE_sleep(1000);

	// Switch Flash to FULL Mode
	FlashHelper_SwitchFullMode(s_pHalContext);

	// read glyph to ramg
	APP_DBG("Copying glyph from %d by %d bytes to %d", glyph_chinese_ASTC[_add], glyph_chinese_ASTC[_size], glyphOnRamG);
	EVE_CoCmd_flashRead(s_pHalContext, glyphOnRamG, glyph_chinese_ASTC[_add], glyph_chinese_ASTC[_size]);

	// read .xfont to ramg
	APP_DBG("Copying xfont from %d by %d bytes to %d", xfont_chinese_ASTC[_add], xfont_chinese_ASTC[_size], xfontOnRamG);
	EVE_CoCmd_flashRead(s_pHalContext, xfontOnRamG, xfont_chinese_ASTC[_add], xfont_chinese_ASTC[_size]);

	EVE_Cmd_waitFlush(s_pHalContext);

	/// Prepare font handle and cahe font
	Display_Start(s_pHalContext);
	EVE_CoCmd_setFont2(s_pHalContext, UNICODE_HANDLE, xfontOnRamG, 0);
	EVE_CoCmd_fontCache(s_pHalContext, UNICODE_HANDLE, startOfCache, cacheSize);
	EVE_Cmd_wr32(s_pHalContext, DISPLAY());
	EVE_CoCmd_swap(s_pHalContext);
	Display_End(s_pHalContext);

	/// Query
	Display_Start(s_pHalContext);
	uint32_t total;
	int32_t used;
	EVE_CoCmd_fontCacheQuery(s_pHalContext, &total, &used); /* NOTE: Returns false in case of coprocessor fault, output values will be uninitialized */
	Display_End(s_pHalContext);
	EVE_sleep(1000);

	Draw_Text_Format(s_pHalContext, "Font cache in memory: \ntotal: %u, used: %d", total, used);

	/// Draw unicode string
	for (int i = 0; i < loopDraw; i++) {
		x = 10;
		y = 10;
		Display_Start(s_pHalContext);
		EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0, 0, 0));
		for (int j = 0; j < loopText; j++) {
			y += 50;
			EVE_CoCmd_text(s_pHalContext, (uint16_t)x, (uint16_t)y, UNICODE_HANDLE, 0, str);
		}
		Display_End(s_pHalContext);
		EVE_sleep(300);
	}
	EVE_sleep(1000);

	/// get fontcachequery data
	EVE_Cmd_waitFlush(s_pHalContext);
	total = used = 0;
	EVE_CoCmd_fontCacheQuery(s_pHalContext, &total, &used);
	EVE_Cmd_waitFlush(s_pHalContext);

	Draw_Text_Format(s_pHalContext, "Font cache in memory: \ntotal: %u, used: %d", total, used);
	EVE_sleep(2000);
}

/**
 * @brief API to demonstrate CMD_RUNANIM
 *
 */
void App_Sample_Run_Animation_RAM_G() {
	const int32_t anim_addr = 19584; // offset of byte "0xaaaa0100" in flash.bin

	uint32_t cmd;
	uint32_t waitmask = -1;
	uint32_t play = 1024 * 512;
	uint32_t cmdwrite = 0;
	const int16_t xAnim = 350;
	const int16_t yAnim = 260;
	uint32_t ch = 0;

	Draw_Text(s_pHalContext, "Run animation without host control on RAM_G");

	Gpu_Hal_LoadImageToMemory(s_pHalContext, TEST_DIR "\\flash-eab-updated.bin", RAM_G, LOAD);
	EVE_Cmd_waitFlush(s_pHalContext);

	/// Check Cmd_RunAnim with play control
	EVE_CoCmd_memZero(s_pHalContext, play, 1);
	EVE_CoCmd_animStartRam(s_pHalContext, ch, anim_addr, ANIM_LOOP);
	EVE_CoCmd_animXY(s_pHalContext, ch, xAnim, yAnim);

	EVE_Cmd_waitFlush(s_pHalContext);
	cmdwrite = EVE_Cmd_wp(s_pHalContext);
	EVE_CoCmd_runAnim(s_pHalContext, waitmask, play);
	EVE_Hal_flush(s_pHalContext);

	cmd = EVE_Hal_rd32(s_pHalContext, (RAM_CMD + cmdwrite + 0) & EVE_CMD_FIFO_MASK);
	waitmask = EVE_Hal_rd32(s_pHalContext, (RAM_CMD + cmdwrite + 4) & EVE_CMD_FIFO_MASK);
	uint32_t p = EVE_Hal_rd32(s_pHalContext, (RAM_CMD + cmdwrite + 8) & EVE_CMD_FIFO_MASK);

	APP_DBG("cmd: 0x%08x, waitmask:%u, play:%u", cmd, waitmask, p);
	EVE_sleep(1000);
	EVE_Hal_wr32(s_pHalContext, play, 1);
}

/**
 * @brief API to demonstrate CMD_RUNANIM
 *
 */
void App_Sample_Run_Animation_Flash() {
#define ANIM_ADDR     (19584) // offset of byte "0xaaaa0100" in flash.bin
#define FRAME_COUNT   (4)

	uint32_t waitmask = -1;
	uint32_t play = 1024 * 512;
	const int16_t xAnim = 350;
	const int16_t yAnim = 260;
	uint32_t ch = 0;

	Draw_Text(s_pHalContext, "Run animation without host control on Flash");

#define flashfile2 "flash-eab-updated.bin"
	Ftf_Write_File_To_Flash_With_Progressbar(s_pHalContext, TEST_DIR "/" flashfile, flashfile, 0);
	EVE_sleep(2000);

	for (int i = 0; i < 2; i++) {
		EVE_CoCmd_dlStart(s_pHalContext);
		EVE_CoCmd_swap(s_pHalContext);
	}
	EVE_CoCmd_clearCache(s_pHalContext);
	EVE_Cmd_waitFlush(s_pHalContext);

	if (!FlashHelper_SwitchFullMode(s_pHalContext)) {
		APP_ERR("Flash is not able to switch full mode");
		return;
	}

	EVE_CoCmd_memZero(s_pHalContext, play, 1);
	EVE_CoCmd_animStart(s_pHalContext, ch, 19584, ANIM_ONCE);
	EVE_CoCmd_animXY(s_pHalContext, ch, xAnim, yAnim);
	EVE_Cmd_waitFlush(s_pHalContext);
	EVE_CoCmd_runAnim(s_pHalContext, waitmask, play);
	EVE_Hal_flush(s_pHalContext);

	EVE_sleep(5 * 1000);
	EVE_Hal_wr32(s_pHalContext, play, 1);
}

/**
 * @brief API to demonstrate CMD_ANIMFRAMERAM
 *
 */
void App_Sample_Cmd_AnimFrameRam() {
	const uint32_t ANIM_ADDRS = 19584;// RAM_G + 0xC140
	const uint32_t FRAME_COUNTS = 4;

	Draw_Text(s_pHalContext, "REG_ANIM_ACTIVE register");

	Gpu_Hal_LoadImageToMemory(s_pHalContext, TEST_DIR "\\flash-eab-updated.bin", RAM_G, LOAD);
	EVE_Cmd_waitFlush(s_pHalContext);

	for (uint16_t frame = 0; frame < FRAME_COUNTS; frame++)
	{
		EVE_CoCmd_dlStart(s_pHalContext);
		EVE_Cmd_wr32(s_pHalContext, CLEAR_COLOR_RGB(0, 0, 0));
		EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1));

		EVE_CoCmd_animFrameRam(s_pHalContext, (uint16_t)(s_pHalContext->Width / 2), (uint16_t)(s_pHalContext->Height / 2), ANIM_ADDRS, frame);

		EVE_Cmd_wr32(s_pHalContext, DISPLAY());
		EVE_CoCmd_swap(s_pHalContext);
		EVE_Cmd_waitFlush(s_pHalContext);

		uint32_t reg_anim_active = EVE_Hal_rd32(s_pHalContext, REG_ANIM_ACTIVE);
		APP_DBG("frame: %u, REG_ANIM_ACTIVE: %u (channel mask)\n", frame, reg_anim_active);

		EVE_sleep(1000);
	}
}

/**
 * @brief Draw set 14
 *
 */
void App_Set_14_Draw() {
	App_Sample_Run_Animation_RAM_G();
	App_Sample_Cmd_AnimFrameRam();
	App_Sample_Dithering();
	App_Sample_Underrun();
	App_Sample_Anime_Ative();
	App_Sample_Cmd_Wait();
	App_Sample_Testcard();
	App_Sample_Cmd_Getimage();
	App_Sample_Api_Level();
	App_Sample_Cmd_Calllist();
	App_Sample_cmd_List_With_Alignment();
	App_Sample_Font_Cache();
	App_Sample_Cmd_Fontcachequery();
	App_Cmd_Calibratesub();
	App_Sample_Non_Square_Display();
}

/**
 * @brief Init set 14
 *
 */
void App_Set_14_Init(EVE_HalContext* pHalContext) {
	s_pHalContext = pHalContext;
	FlashHelper_SwitchFullMode(s_pHalContext);

	Draw_Text(s_pHalContext, "Set14   Start!");
}

/**
 * @brief Deinit set 14
 *
 */
void App_Set_14_Deinit() {
	Draw_Text(s_pHalContext, "Set14   End!");
}

#endif // SAMAPP_ENABLE_APIS_SET14
