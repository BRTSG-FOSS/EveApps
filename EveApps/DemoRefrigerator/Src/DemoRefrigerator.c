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

#if defined(MSVC_PLATFORM) || defined(BT8XXEMU_PLATFORM)
#include "time.h"
#include "math.h"
#endif

#ifdef FT9XX_PLATFORM
#include "ff.h"
#endif
#define STARTUP_ADDRESS	100*1024L
#define FREEZER_POINT 1;
#define  FRIDGE_POINT 2;

void animation();
#define NBLOBS 128

typedef struct S_RefrigeratorAppSnowLinear
{
	int16_t xOffset;
	int16_t yOffset;
	char8_t dx;
	char8_t dy;
}S_RefrigeratorAppSnowLinear_t;

struct xy {
	int x, y;
}blobs[NBLOBS];

#ifdef FT9XX_PLATFORM
typedef struct _localtime
{
	WORD wYear;
	WORD wMonth;
	WORD wDayOfWeek;
	WORD wDay;
	WORD wHour;
	WORD wMinute;
	WORD wSecond;
	WORD wMilliseconds;
} 	 local_time;
local_time st, lt;

DWORD get_fattime(void)
{
	/* Returns current time packed into a DWORD variable */
	return 0;
}

#endif

uint32_t music_playing = 0, fileszsave = 0;
float_t theta;

/* API to load raw data from file into perticular locatio of ft800 */
void DemoLoadRawFromFile(char8_t *pFileName, uint32_t DstAddr)
{
	char8_t fileName[255] = TEST_DIR "\\";
	FILE *pFile;
	strcat(fileName, pFileName);

	Gpu_Hal_LoadImageToMemory(s_pHalContext, fileName, DstAddr, LOAD);
}

struct
{
	uint8_t unlock : 1;
	uint8_t freeze : 1;
	uint8_t home : 1;
	uint8_t brightness : 1;
	uint8_t fridge : 1;
	uint8_t cubed : 1;
	uint8_t settings : 1;
	uint8_t change_background : 1;
	uint8_t font_size : 1;
	uint8_t sketch : 1;
	uint8_t food_tracker : 1;
	uint8_t clear : 1;
}bitfield;

static uchar8_t istouch()
{
	return !(EVE_Hal_rd16(s_pHalContext, REG_TOUCH_RAW_XY) & 0x8000);
}

int16_t font[] = { 26, 27, 28, 29 };
int16_t freezer_temp[] = { -2,-4,-6,-8,-10,-12,-14,34,36,38,40,42,44,46 };
uint8_t incremental_flag = 1;

void change_background()
{
	int32_t Read_tag, tag = 0, pen_up = 0, xoffset, yoffset, yoffset1, xoffset1, yoffset3;
	uint8_t z = 0;
	char *bgd_raw_files[] = { "31.raw","32.raw","33.raw","34.raw","35.raw","36.raw" };//,"37.raw","38.raw","39.raw"};

	DemoLoadRawFromFile("BG.raw", 158 * 1024);
	EVE_CoCmd_dlStart(s_pHalContext);
	EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_HANDLE(5));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_SOURCE(158 * 1024));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_LAYOUT(RGB565, 100 * 2, 50));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_SIZE(NEAREST, BORDER, BORDER, 100, 50));
	EVE_Cmd_wr32(s_pHalContext, DISPLAY());
	EVE_CoCmd_swap(s_pHalContext);
	EVE_Cmd_waitFlush(s_pHalContext);
	while (1)
	{
		Read_tag = Gesture_GetTag(s_pHalContext);

		if (8 == Read_tag)
		{
			break;
		}
		else if (Read_tag >= 30 && Read_tag < 40)
		{
			DemoLoadRawFromFile(bgd_raw_files[Read_tag - 30], RAM_G);
		}

		EVE_CoCmd_dlStart(s_pHalContext);
		EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1));

		EVE_Cmd_wr32(s_pHalContext, BEGIN(BITMAPS));

		EVE_Cmd_wr32(s_pHalContext, BITMAP_TRANSFORM_A(128));
		EVE_Cmd_wr32(s_pHalContext, BITMAP_TRANSFORM_E(128));
		EVE_Cmd_wr32(s_pHalContext, TAG_MASK(1));
		EVE_Cmd_wr32(s_pHalContext, TAG(0));
		EVE_Cmd_wr32(s_pHalContext, BITMAP_HANDLE(5));
		EVE_Cmd_wr32(s_pHalContext, BITMAP_SIZE(NEAREST, BORDER, BORDER, 800, 480));
		EVE_Cmd_wr32(s_pHalContext, VERTEX2II(0, 0, 1, 0));
		EVE_Cmd_wr32(s_pHalContext, BITMAP_SIZE(NEAREST, BORDER, BORDER, 100, 50));
		EVE_Cmd_wr32(s_pHalContext, BITMAP_TRANSFORM_A(256));
		EVE_Cmd_wr32(s_pHalContext, BITMAP_TRANSFORM_E(256));
		EVE_Cmd_wr32(s_pHalContext, TAG(8));
		EVE_Cmd_wr32(s_pHalContext, VERTEX2II(750, 10, 3, 3));
		EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 255, 255));
		EVE_Cmd_wr32(s_pHalContext, TAG_MASK(1));
		xoffset = 10;
		yoffset = 30;
		for (z = 0; z < 6; z++)
		{
			if (z == 4) { yoffset += 80; xoffset = 10; }

			EVE_Cmd_wr32(s_pHalContext, TAG(30 + z));
			EVE_Cmd_wr32(s_pHalContext, VERTEX2II(xoffset, yoffset, 5, z));

			xoffset += 110;
		}

		EVE_Cmd_wr32(s_pHalContext, TAG_MASK(0));
		EVE_Cmd_wr32(s_pHalContext, DISPLAY());
		EVE_CoCmd_swap(s_pHalContext);
		EVE_Cmd_waitFlush(s_pHalContext);
	}
	bitfield.change_background = 0;
}

void Sketch(int16_t val_font)
{
	const uint16_t width = 480;
	const uint16_t height = 272;
	uint32_t  tracker, color = 0;
	uint16_t  val = 32768;
	uint8_t tag = 0;
	int32_t xoffset1 = 10, Baddr;
	int16_t NumSnowRange = 6, NumSnowEach = 10, RandomVal = 16, xoffset, yoffset, j;
	S_RefrigeratorAppSnowLinear_t S_RefrigeratorSnowArray[8 * 10], *pRefrigeratorSnow = NULL;
	uint8_t keypressed;
	EVE_CoCmd_dlStart(s_pHalContext);
	EVE_CoCmd_sketch(s_pHalContext, 0, 50, width - 60, height - 70, 110 * 1024L, L8);

	EVE_CoCmd_memZero(s_pHalContext, 110 * 1024L, 140 * 1024L);
	EVE_Cmd_wr32(s_pHalContext, BITMAP_SOURCE(110 * 1024L));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_LAYOUT(L8, (width - 60), height - 20));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_SIZE(NEAREST, BORDER, BORDER, (width - 60), (height - 20)));

	EVE_CoCmd_swap(s_pHalContext);
	EVE_Cmd_waitFlush(s_pHalContext);
	while (1)
	{
		tag = Gesture_GetTag(s_pHalContext);
		keypressed = EVE_Hal_rd8(s_pHalContext, REG_TOUCH_TAG);
		if (keypressed == 2)	bitfield.clear = 1;

		if (tag == 8)
		{
			bitfield.sketch = 0;
			EVE_Cmd_wr32(s_pHalContext, CMD_STOP);//to stop the sketch command
			return;

		}

		if (bitfield.clear)
		{
			EVE_CoCmd_dlStart(s_pHalContext);
			EVE_CoCmd_memZero(s_pHalContext, 110 * 1024L, 140 * 1024L); // Clear the gram frm 1024 		
			EVE_Cmd_waitFlush(s_pHalContext);
			bitfield.clear = 0;
		}

		EVE_CoCmd_dlStart(s_pHalContext);                  // Start the display list
		EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1));
		EVE_Cmd_wr32(s_pHalContext, BEGIN(BITMAPS));
		EVE_Cmd_wr32(s_pHalContext, SAVE_CONTEXT());
		EVE_Cmd_wr32(s_pHalContext, BITMAP_TRANSFORM_A(128));
		EVE_Cmd_wr32(s_pHalContext, BITMAP_TRANSFORM_E(128));
		EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 255, 255));
		EVE_Cmd_wr32(s_pHalContext, VERTEX2II(0, 0, 1, 0));
		EVE_Cmd_wr32(s_pHalContext, RESTORE_CONTEXT());
		EVE_CoCmd_text(s_pHalContext, 200, 10, font[val_font], OPT_CENTERX, "Add a description on the Notes");
		EVE_Cmd_wr32(s_pHalContext, TAG_MASK(1));
		EVE_Cmd_wr32(s_pHalContext, TAG(1));          // assign the tag value 

		EVE_CoCmd_fgColor(s_pHalContext, 0x60252);
		EVE_Cmd_wr32(s_pHalContext, TAG(2));          // assign the tag value
		EVE_CoCmd_button(s_pHalContext, (width - 55), (height - 45), 45, 25, font[val_font], 0, "CLR");
		EVE_Cmd_wr32(s_pHalContext, TAG(8));
		EVE_Cmd_wr32(s_pHalContext, BEGIN(BITMAPS));//Bitmap of the home icon
		EVE_Cmd_wr32(s_pHalContext, VERTEX2II(450, 10, 3, 3));

		EVE_Cmd_wr32(s_pHalContext, TAG_MASK(0));

		EVE_Cmd_wr32(s_pHalContext, LINE_WIDTH(1 * 16));
		EVE_Cmd_wr32(s_pHalContext, BEGIN(RECTS));
		EVE_Cmd_wr32(s_pHalContext, VERTEX2F(0, 50 * 16));
		EVE_Cmd_wr32(s_pHalContext, VERTEX2F((int16_t)(width - 60) * 16, (int16_t)(height - 20) * 16));

		EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(8, 8, 62));
		EVE_Cmd_wr32(s_pHalContext, BEGIN(BITMAPS));
		EVE_Cmd_wr32(s_pHalContext, VERTEX2II(0, 50, 0, 0));

		animation();

		EVE_Cmd_wr32(s_pHalContext, END());
		EVE_Cmd_wr32(s_pHalContext, DISPLAY());
		EVE_CoCmd_swap(s_pHalContext);
		EVE_Cmd_waitFlush(s_pHalContext);
	}
}

uint8_t months[12][15] = { "January","February","March","April","May","June","July","August","September","October","November","December" };
uint8_t item_selection = 40;
void food_tracker(int16_t val_font)
{
	int32_t x_fridge_icon, y_fridge_icon, x_fridge_icon_1, z, Read_tag, j, y, Baddr, m, n;
	int32_t r_x1_rect, r_x2_rect, r_y1_rect, r_y2_rect;
	uint8_t keypressed;
	char *raw_files[] = { "app1.raw","car0.raw","ki3.raw","tom1.raw","bery1.raw","chic1.raw","snap1.raw","prk1.raw","bf1.raw","sas1.raw" };

	EVE_CoCmd_dlStart(s_pHalContext);
	EVE_Cmd_wr32(s_pHalContext, CLEAR_COLOR_RGB(255, 255, 255));
	EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1)); // clear screen
	EVE_Cmd_wr32(s_pHalContext, BITMAP_HANDLE(7));//handle 7 is used for fridge items
	EVE_Cmd_wr32(s_pHalContext, BITMAP_SOURCE(78 * 1024));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_LAYOUT(ARGB4, 80 * 2, 80));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_SIZE(BILINEAR, BORDER, BORDER, 80, 80));

	EVE_Cmd_wr32(s_pHalContext, DISPLAY());
	EVE_CoCmd_swap(s_pHalContext);
	EVE_Cmd_waitFlush(s_pHalContext);
	Baddr = 78 * 1024;

	for (y = 0; y < 10; y++)
	{
		DemoLoadRawFromFile(raw_files[y], Baddr);
		Baddr += 80 * 80 * 2;
	}

	while (1)
	{
		keypressed = EVE_Hal_rd8(s_pHalContext, REG_TOUCH_TAG);
		Read_tag = Gesture_GetTag(s_pHalContext);
		if (keypressed != 0 && keypressed >= 40)
			item_selection = keypressed;
		if (Read_tag == 8) {
			bitfield.food_tracker = 0;
			return;
		}
		EVE_CoCmd_dlStart(s_pHalContext);
		EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1));
		EVE_Cmd_wr32(s_pHalContext, BEGIN(BITMAPS));
		EVE_Cmd_wr32(s_pHalContext, SAVE_CONTEXT());
		EVE_Cmd_wr32(s_pHalContext, BITMAP_TRANSFORM_A(128));
		EVE_Cmd_wr32(s_pHalContext, BITMAP_TRANSFORM_E(128));
		EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 255, 255));
		EVE_Cmd_wr32(s_pHalContext, VERTEX2II(0, 0, 1, 0));
		EVE_Cmd_wr32(s_pHalContext, RESTORE_CONTEXT());
		EVE_CoCmd_text(s_pHalContext, 180, 0, font[val_font], OPT_CENTERX, "Food Manager");
		EVE_CoCmd_text(s_pHalContext, 90, 30, font[val_font], OPT_CENTERX, "Left Door");
		EVE_CoCmd_text(s_pHalContext, 310, 30, font[val_font], OPT_CENTERX, "Right Door");
		animation();
		EVE_Cmd_wr32(s_pHalContext, BEGIN(RECTS));
		EVE_Cmd_wr32(s_pHalContext, LINE_WIDTH(5 * 16));

		EVE_Cmd_wr32(s_pHalContext, VERTEX2F(20 * 16, 60 * 16));
		EVE_Cmd_wr32(s_pHalContext, VERTEX2F(180 * 16, 220 * 16));
		EVE_Cmd_wr32(s_pHalContext, VERTEX2F(200 * 16, 60 * 16));
		EVE_Cmd_wr32(s_pHalContext, VERTEX2F(450 * 16, 220 * 16));
		r_x1_rect = 30;
		r_y1_rect = 70;
		for (n = 0; n < 4; n++)
		{
			if (n == 2) { r_y1_rect += 80; r_x1_rect = 30; }
			if (item_selection == 40 + n)
				EVE_Cmd_wr32(s_pHalContext, COLOR_A(255));
			else
				EVE_Cmd_wr32(s_pHalContext, COLOR_A(80));
			EVE_Cmd_wr32(s_pHalContext, BEGIN(RECTS));
			EVE_Cmd_wr32(s_pHalContext, TAG(40 + n));
			EVE_Cmd_wr32(s_pHalContext, VERTEX2F(16 * r_x1_rect, 16 * (r_y1_rect)));
			EVE_Cmd_wr32(s_pHalContext, VERTEX2F(16 * (r_x1_rect + 60), 16 * (r_y1_rect + 60)));
			EVE_Cmd_wr32(s_pHalContext, BEGIN(BITMAPS));
			EVE_Cmd_wr32(s_pHalContext, COLOR_A(255));
			EVE_Cmd_wr32(s_pHalContext, TAG(40 + n));
			EVE_Cmd_wr32(s_pHalContext, VERTEX2II(r_x1_rect - 10, r_y1_rect - 10, 7, n));
			r_x1_rect += 80;
		}
		r_x1_rect = 210;
		r_y1_rect = 70;
		for (n = 0; n < 6; n++)
		{
			if (n == 3) { r_y1_rect += 80; r_x1_rect = 210; }
			if (item_selection == 44 + n)
				EVE_Cmd_wr32(s_pHalContext, COLOR_A(255));
			else
				EVE_Cmd_wr32(s_pHalContext, COLOR_A(80));
			EVE_Cmd_wr32(s_pHalContext, BEGIN(RECTS));
			EVE_Cmd_wr32(s_pHalContext, TAG(44 + n));
			EVE_Cmd_wr32(s_pHalContext, VERTEX2F(16 * r_x1_rect, 16 * (r_y1_rect)));
			EVE_Cmd_wr32(s_pHalContext, VERTEX2F(16 * (r_x1_rect + 60), 16 * (r_y1_rect + 60)));
			EVE_Cmd_wr32(s_pHalContext, BEGIN(BITMAPS));
			EVE_Cmd_wr32(s_pHalContext, COLOR_A(255));
			EVE_Cmd_wr32(s_pHalContext, TAG(44 + n));
			EVE_Cmd_wr32(s_pHalContext, VERTEX2II(r_x1_rect - 10, r_y1_rect - 10, 7, 4 + n));
			r_x1_rect += 80;
		}
		EVE_Cmd_wr32(s_pHalContext, RESTORE_CONTEXT());
		EVE_Cmd_wr32(s_pHalContext, BEGIN(BITMAPS));

		EVE_Cmd_wr32(s_pHalContext, TAG(8));
		EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 255, 255));
		EVE_Cmd_wr32(s_pHalContext, VERTEX2II(450, 0, 3, 3));

		EVE_Cmd_wr32(s_pHalContext, END());
		EVE_Cmd_wr32(s_pHalContext, DISPLAY());
		EVE_CoCmd_swap(s_pHalContext);
		EVE_Cmd_waitFlush(s_pHalContext);

	}
}

void cubed_setting(uint16_t *cubed, uint32_t val_font)
{
	int32_t Read_tag, Baddr;
	static int16_t cubed_funct = 0, img1_flag = 1, img2_flag = 0;
	uint8_t keypressed;
	cubed_funct = *cubed;
	Baddr = 94 * 1024;
	EVE_CoCmd_dlStart(s_pHalContext);
	EVE_Cmd_wr32(s_pHalContext, CLEAR_COLOR_RGB(255, 255, 255));
	EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1)); // clear screen
	DemoLoadRawFromFile("cube1.raw", Baddr);
	EVE_Cmd_wr32(s_pHalContext, BITMAP_HANDLE(8));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_SOURCE(Baddr));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_LAYOUT(ARGB4, 200 * 2, 200));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_SIZE(BILINEAR, BORDER, BORDER, 200, 200));
	Baddr += 200 * 200 * 2;
	DemoLoadRawFromFile("crush1.raw", Baddr);
	EVE_Cmd_wr32(s_pHalContext, BITMAP_HANDLE(9));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_SOURCE(Baddr));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_LAYOUT(ARGB4, 200 * 2, 200));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_SIZE(BILINEAR, BORDER, BORDER, 200, 200));
	EVE_Cmd_wr32(s_pHalContext, DISPLAY());
	EVE_CoCmd_swap(s_pHalContext);
	EVE_Cmd_waitFlush(s_pHalContext);

	while (1)
	{
		Read_tag = Gesture_GetTag(s_pHalContext);
		keypressed = EVE_Hal_rd8(s_pHalContext, REG_TOUCH_TAG);

		if (Read_tag == 8) {
			bitfield.cubed = 0;
			return;
		}

		EVE_CoCmd_dlStart(s_pHalContext);
		EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1));

		EVE_Cmd_wr32(s_pHalContext, BEGIN(BITMAPS));
		EVE_Cmd_wr32(s_pHalContext, SAVE_CONTEXT());
		EVE_Cmd_wr32(s_pHalContext, BITMAP_TRANSFORM_A(128));
		EVE_Cmd_wr32(s_pHalContext, BITMAP_TRANSFORM_E(128));
		EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 255, 255));
		EVE_Cmd_wr32(s_pHalContext, VERTEX2II(0, 0, 1, 0));
		EVE_Cmd_wr32(s_pHalContext, RESTORE_CONTEXT());

		EVE_CoCmd_text(s_pHalContext, 120, 30, font[val_font], OPT_CENTER, "Cubed");
		EVE_CoCmd_text(s_pHalContext, 350, 30, font[val_font], OPT_CENTER, "Crushed");
		animation();
		EVE_Cmd_wr32(s_pHalContext, BEGIN(RECTS));
		if (keypressed == 25 || img1_flag == 1)
		{
			EVE_Cmd_wr32(s_pHalContext, COLOR_A(50));
			EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 255, 255));
			EVE_Cmd_wr32(s_pHalContext, VERTEX2F(10 * 16, 50 * 16));
			EVE_Cmd_wr32(s_pHalContext, VERTEX2F(210 * 16, 250 * 16));
			cubed_funct = 0;
			img1_flag = 1;
			img2_flag = 0;
		}

		if (keypressed == 26 || img2_flag == 1)
		{
			EVE_Cmd_wr32(s_pHalContext, BEGIN(RECTS));
			EVE_Cmd_wr32(s_pHalContext, COLOR_A(50));
			EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 255, 255));
			EVE_Cmd_wr32(s_pHalContext, VERTEX2F(250 * 16, 50 * 16));
			EVE_Cmd_wr32(s_pHalContext, VERTEX2F(450 * 16, 250 * 16));
			cubed_funct = 1;
			img1_flag = 0;
			img2_flag = 1;
		}

		EVE_Cmd_wr32(s_pHalContext, RESTORE_CONTEXT());
		EVE_Cmd_wr32(s_pHalContext, BEGIN(BITMAPS));
		EVE_Cmd_wr32(s_pHalContext, TAG_MASK(1));
		EVE_Cmd_wr32(s_pHalContext, TAG(25));
		EVE_Cmd_wr32(s_pHalContext, COLOR_A(100));
		EVE_Cmd_wr32(s_pHalContext, VERTEX2II(10, 50, 8, 0));
		EVE_Cmd_wr32(s_pHalContext, TAG(26));
		EVE_Cmd_wr32(s_pHalContext, VERTEX2II(250, 50, 9, 0));
		EVE_Cmd_wr32(s_pHalContext, COLOR_A(255));
		EVE_Cmd_wr32(s_pHalContext, TAG(8));
		EVE_Cmd_wr32(s_pHalContext, VERTEX2II(450, 5, 3, 3));
		*cubed = cubed_funct;
		EVE_Cmd_wr32(s_pHalContext, TAG_MASK(0));
		EVE_Cmd_wr32(s_pHalContext, END());
		EVE_Cmd_wr32(s_pHalContext, DISPLAY());
		EVE_CoCmd_swap(s_pHalContext);
		EVE_Cmd_waitFlush(s_pHalContext);

	}
}

int16_t NumSnowRange = 6, NumSnowEach = 10, RandomVal = 16, xoffset, yoffset, j;
S_RefrigeratorAppSnowLinear_t S_RefrigeratorSnowArray[8 * 10], *pRefrigeratorSnow = NULL;

void animation()
{
	/* Draw background snow bitmaps */
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0, 0, 0));
	pRefrigeratorSnow = S_RefrigeratorSnowArray;
	EVE_Cmd_wr32(s_pHalContext, BEGIN(BITMAPS));

	EVE_Cmd_wr32(s_pHalContext, COLOR_A(64));
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 255, 255));
	EVE_Cmd_wr32(s_pHalContext, COLOR_MASK(1, 1, 1, 0));
	for (j = 0; j<(NumSnowRange*NumSnowEach); j++)
	{
		EVE_Cmd_wr32(s_pHalContext, BITMAP_HANDLE(6));

		if (((pRefrigeratorSnow->xOffset >((s_pHalContext->Width + 60) * 16)) || (pRefrigeratorSnow->yOffset > ((s_pHalContext->Height + 60) * 16))) ||
			((pRefrigeratorSnow->xOffset < (-60 * 16)) || (pRefrigeratorSnow->yOffset < (-60 * 16))))
		{
			pRefrigeratorSnow->xOffset = random(s_pHalContext->Width * 16);//s_pHalContext->Width*16 + random(80*16);
			pRefrigeratorSnow->yOffset = s_pHalContext->Height * 16 + random(80 * 16);//random(s_pHalContext->Height*16);
			pRefrigeratorSnow->dx = random(RandomVal * 8) - RandomVal * 4;//-1*random(RandomVal*8);
			pRefrigeratorSnow->dy = -1 * random(RandomVal * 8);//random(RandomVal*8) - RandomVal*4;

		}
		EVE_Cmd_wr32(s_pHalContext, VERTEX2F(pRefrigeratorSnow->xOffset, pRefrigeratorSnow->yOffset));
		pRefrigeratorSnow->xOffset += pRefrigeratorSnow->dx;
		pRefrigeratorSnow->yOffset += pRefrigeratorSnow->dy;
		pRefrigeratorSnow++;
	}
	EVE_Cmd_wr32(s_pHalContext, COLOR_MASK(1, 1, 1, 1));
}

int16_t origin_xy[][4] = { { 65,0,0,-90 },
{ 0,0,90,-65 },
{ 0,0,65,-90 },
{ 0,65,90,0 },
{ -65,90,0,0 },
{ -90,0,0,65 },
{ 0,0,-65,90 },
};

void construct_structure(int32_t xoffset, int16_t flag)//, uint32_t tag)
{
	int32_t z, x0, y0, x1, y1;
	int16_t x2, x3, y2, y3, next_flag, thcurrval = 0;
	uint16_t angle = 0;//, angle1 = 0, angle2 = 0;

	EVE_Cmd_wr32(s_pHalContext, COLOR_A(0));
	EVE_Cmd_wr32(s_pHalContext, BEGIN(FTPOINTS));
	EVE_Cmd_wr32(s_pHalContext, POINT_SIZE(60 * 16));
	EVE_Cmd_wr32(s_pHalContext, STENCIL_OP(INCR, INCR));
	EVE_Cmd_wr32(s_pHalContext, VERTEX2F(xoffset * 16, 150 * 16));

	EVE_Cmd_wr32(s_pHalContext, BEGIN(LINES));
	EVE_Cmd_wr32(s_pHalContext, LINE_WIDTH(2 * 16));
	for (z = 45; z < 361; z += 45)
	{
		theta = Math_Da(z, 0) * 2;
		Math_Polarxy(100, theta, &x0, &y0, xoffset, 150);
		Math_Polarxy(60, theta, &x1, &y1, xoffset, 150);
		EVE_Cmd_wr32(s_pHalContext, VERTEX2F(x0, y0));
		EVE_Cmd_wr32(s_pHalContext, VERTEX2F(x1, y1));
	}

	EVE_Cmd_wr32(s_pHalContext, STENCIL_OP(KEEP, KEEP));
	EVE_Cmd_wr32(s_pHalContext, BEGIN(FTPOINTS));
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(102, 180, 232));
	EVE_Cmd_wr32(s_pHalContext, POINT_SIZE(100 * 16));
	EVE_Cmd_wr32(s_pHalContext, COLOR_A(255));
	EVE_Cmd_wr32(s_pHalContext, STENCIL_FUNC(EQUAL, 0, 255));//stencil function to increment all the values

	EVE_Cmd_wr32(s_pHalContext, VERTEX2F(xoffset * 16, 150 * 16));
	EVE_Cmd_wr32(s_pHalContext, LINE_WIDTH(65 * 16));
	EVE_Cmd_wr32(s_pHalContext, STENCIL_OP(INCR, INCR));

	EVE_Cmd_wr32(s_pHalContext, BEGIN(LINES));

	if (flag >= 0 && flag < 7)//&& angle2 >= 0)
	{
		next_flag = flag + 1;
		EVE_Cmd_wr32(s_pHalContext, COLOR_A(0));
		angle = flag * 45;
		theta = Math_Da(angle, 0) * 2;
		Math_Polarxy(150, theta, &x0, &y0, xoffset, 150);
		theta = Math_Da(angle + 180, 0) * 2;
		Math_Polarxy(150, theta, &x1, &y1, xoffset, 150);
		EVE_Cmd_wr32(s_pHalContext, VERTEX2F(x0 + origin_xy[flag][0] * 16, y0 + origin_xy[flag][2] * 16));
		EVE_Cmd_wr32(s_pHalContext, VERTEX2F(x1 + origin_xy[flag][0] * 16, y1 + origin_xy[flag][2] * 16));

		theta = Math_Da(angle + 45, 0) * 2;
		Math_Polarxy(150, theta, &x0, &y0, xoffset, 150);
		theta = Math_Da(angle + 45 + 180, 0) * 2;
		Math_Polarxy(150, theta, &x1, &y1, xoffset, 150);
		EVE_Cmd_wr32(s_pHalContext, VERTEX2F(x0 + origin_xy[flag][1] * 16, y0 + origin_xy[flag][3] * 16));
		EVE_Cmd_wr32(s_pHalContext, VERTEX2F(x1 + origin_xy[flag][1] * 16, y1 + origin_xy[flag][3] * 16));

		EVE_Cmd_wr32(s_pHalContext, STENCIL_OP(KEEP, KEEP));

		EVE_Cmd_wr32(s_pHalContext, BEGIN(FTPOINTS));
		EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(19, 43, 59));
		EVE_Cmd_wr32(s_pHalContext, POINT_SIZE(100 * 16));
		EVE_Cmd_wr32(s_pHalContext, COLOR_A(255));
		EVE_Cmd_wr32(s_pHalContext, STENCIL_FUNC(EQUAL, 0, 255));
		EVE_Cmd_wr32(s_pHalContext, VERTEX2F(xoffset * 16, 150 * 16));

	}

	EVE_Cmd_wr32(s_pHalContext, STENCIL_OP(KEEP, KEEP));

	EVE_Cmd_wr32(s_pHalContext, BEGIN(FTPOINTS));
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(19, 43, 59));
	EVE_Cmd_wr32(s_pHalContext, POINT_SIZE(100 * 16));

	EVE_Cmd_wr32(s_pHalContext, COLOR_A(255));
	EVE_Cmd_wr32(s_pHalContext, STENCIL_FUNC(EQUAL, 0, 255));
	EVE_Cmd_wr32(s_pHalContext, VERTEX2F(xoffset * 16, 150 * 16));
	EVE_Cmd_wr32(s_pHalContext, RESTORE_CONTEXT());
	EVE_Cmd_wr32(s_pHalContext, COLOR_A(255));
}

void change_temperature(uint32_t *freeze_temp, uint32_t *fridge_temp, int16_t val_font)
{
	uint32_t track_val_min, track_val_max, tracker = 0;
	int32_t Read_tag, trackval = 0, y, z = 45, adj_val = 0, prev_th = 0, BaseTrackValSign = 0, major_axis_number = 0, i, x0, y0, x1, y1;
	uint16_t track_val = 0;
	int16_t x2, x3, y2, y3, temp, thcurrval = 0;
	int16_t thcurr = 0;
	static uint16_t angle = 0, angle1 = 0, angle2 = 0, flag = 0, next_flag, flag_1 = 0, angle_val = 0;
	uint8_t freezer_tempr = 0, fridge_tempr = 0;
	int16_t r;
	uint8_t keypressed;

	int16_t NumSnowRange = 6, NumSnowEach = 10, RandomVal = 16, xoffset, yoffset, j;
	S_RefrigeratorAppSnowLinear_t S_RefrigeratorSnowArray[8 * 10], *pRefrigeratorSnow = NULL;

	thcurr = *freeze_temp;
	thcurrval = *fridge_temp;

	EVE_CoCmd_track(s_pHalContext, 120, 150, 1, 1, 20);
	EVE_CoCmd_track(s_pHalContext, 350, 150, 1, 1, 30);

	while (1)
	{
		Read_tag = Gesture_GetTag(s_pHalContext);
		keypressed = EVE_Hal_rd8(s_pHalContext, REG_TOUCH_TAG);
		tracker = EVE_Hal_rd32(s_pHalContext, REG_TRACKER);
		track_val = tracker >> 16;

		if (keypressed == 8)
		{
			bitfield.freeze = 0;
			bitfield.fridge = 0;
			return;
		}
		if (istouch())
		{
			if (keypressed == 30 || keypressed == 20)
			{
				temp = track_val / 182;
				if (temp < 316)
				{
					if (keypressed == 30)
						angle2 = track_val / 182;
					else if (keypressed == 20)
						angle1 = track_val / 182;

					flag = angle1 / 45;
					flag_1 = angle2 / 45;
				}

			}
		}

		EVE_CoCmd_dlStart(s_pHalContext);
		EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1)); // clear screen

		EVE_Cmd_wr32(s_pHalContext, BEGIN(BITMAPS));
		EVE_Cmd_wr32(s_pHalContext, SAVE_CONTEXT());
		EVE_Cmd_wr32(s_pHalContext, BITMAP_TRANSFORM_A(128));
		EVE_Cmd_wr32(s_pHalContext, BITMAP_TRANSFORM_E(128));
		EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 255, 255));
		EVE_Cmd_wr32(s_pHalContext, VERTEX2II(0, 0, 1, 0));
		EVE_Cmd_wr32(s_pHalContext, RESTORE_CONTEXT());
		EVE_CoCmd_text(s_pHalContext, 120, 30, font[val_font], OPT_CENTER, "Freezer Temperature");
		EVE_CoCmd_text(s_pHalContext, 350, 30, font[val_font], OPT_CENTER, "Fridge Temperature");

		construct_structure(120, flag);//,20);
		EVE_Cmd_wr32(s_pHalContext, RESTORE_CONTEXT());
		EVE_Cmd_wr32(s_pHalContext, CLEAR(0, 1, 0));
		construct_structure(350, flag_1);//,30);

		for (r = 0; r<2; r++)
		{
			if (r == 0) 	x2 = 120;
			else		x2 = 350;

			angle_val = 135 + 23;
			for (z = 0; z<7; z++)
			{
				angle_val += 45;
				angle_val %= 360;
				theta = Math_Da(angle_val, 0) * 2;
				Math_Polarxy(80, theta, &x0, &y0, x2, 150);
				EVE_CoCmd_number(s_pHalContext, x0 / 16, y0 / 16, 28, OPT_SIGNED | OPT_CENTER, freezer_temp[(r * 7) + z]);

			}
		}

		EVE_Cmd_wr32(s_pHalContext, COLOR_A(255));
		EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(1, 1, 1));
		*freeze_temp = flag*-2 - 2;
		Math_Polarxy(0, 0, &x0, &y0, 120, 150);
		EVE_CoCmd_number(s_pHalContext, x0 / 16, y0 / 16, 30, OPT_SIGNED | OPT_CENTER, *freeze_temp);
		EVE_CoCmd_text(s_pHalContext, 150, 145, 17, OPT_CENTER, "x");
		EVE_CoCmd_text(s_pHalContext, 160, 150, 30, OPT_CENTER, "F");
		*fridge_temp = flag_1 * 2 + 34;
		Math_Polarxy(0, 0, &x0, &y0, 350, 150);
		EVE_CoCmd_number(s_pHalContext, x0 / 16, y0 / 16, 30, OPT_SIGNED | OPT_CENTER, *fridge_temp);
		EVE_CoCmd_text(s_pHalContext, 370, 145, 17, OPT_CENTER, "x");
		EVE_CoCmd_text(s_pHalContext, 380, 150, 30, OPT_CENTER, "F");

		EVE_Cmd_wr32(s_pHalContext, VERTEX2II(140, 205, 3, 0));
		EVE_Cmd_wr32(s_pHalContext, VERTEX2II(370, 205, 3, 1));

		animation();

		EVE_Cmd_wr32(s_pHalContext, RESTORE_CONTEXT());
		EVE_Cmd_wr32(s_pHalContext, POINT_SIZE(100 * 16));
		EVE_Cmd_wr32(s_pHalContext, COLOR_A(0));
		EVE_Cmd_wr32(s_pHalContext, BEGIN(FTPOINTS));
		EVE_Cmd_wr32(s_pHalContext, TAG_MASK(1));
		EVE_Cmd_wr32(s_pHalContext, TAG(20));
		EVE_Cmd_wr32(s_pHalContext, VERTEX2F(120 * 16, 150 * 16));
		EVE_Cmd_wr32(s_pHalContext, TAG(30));
		EVE_Cmd_wr32(s_pHalContext, VERTEX2F(350 * 16, 150 * 16));

		EVE_Cmd_wr32(s_pHalContext, POINT_SIZE(60 * 16));
		EVE_Cmd_wr32(s_pHalContext, COLOR_A(0));
		EVE_Cmd_wr32(s_pHalContext, BEGIN(FTPOINTS));
		EVE_Cmd_wr32(s_pHalContext, TAG_MASK(1));
		EVE_Cmd_wr32(s_pHalContext, TAG(31));
		EVE_Cmd_wr32(s_pHalContext, VERTEX2F(120 * 16, 150 * 16));
		EVE_Cmd_wr32(s_pHalContext, TAG(32));
		EVE_Cmd_wr32(s_pHalContext, VERTEX2F(350 * 16, 150 * 16));

		EVE_Cmd_wr32(s_pHalContext, TAG_MASK(0));

		EVE_Cmd_wr32(s_pHalContext, RESTORE_CONTEXT());
		EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 255, 255));
		EVE_Cmd_wr32(s_pHalContext, BEGIN(BITMAPS));
		EVE_Cmd_wr32(s_pHalContext, TAG_MASK(1));
		EVE_Cmd_wr32(s_pHalContext, TAG(8));
		EVE_Cmd_wr32(s_pHalContext, VERTEX2II(450, 5, 3, 3));
		EVE_Cmd_wr32(s_pHalContext, TAG_MASK(0));

		EVE_Cmd_wr32(s_pHalContext, END());
		EVE_Cmd_wr32(s_pHalContext, DISPLAY());
		EVE_CoCmd_swap(s_pHalContext);
		EVE_Cmd_waitFlush(s_pHalContext);

	}
}

uint16_t sleep_cts = 0;
uint32_t val;
void change_brightness(uint32_t val, uint32_t val_font)
{
	int32_t Read_tag, x;
	static uint32_t tracker = 0, track_val = 420;
	uint16_t pwm_val = 0, rval = 0, pwmi_val = 0;
	uint8_t keypressed;
	val = 128;
	EVE_CoCmd_track(s_pHalContext, 20, 200, 420, 40, 12);

	while (1)
	{
		Read_tag = Gesture_GetTag(s_pHalContext);
		keypressed = EVE_Hal_rd8(s_pHalContext, REG_TOUCH_TAG);
		tracker = EVE_Hal_rd32(s_pHalContext, REG_TRACKER);

		if ((tracker & 0xff) == 12)
		{
			pwmi_val = tracker >> 16;

			if (pwmi_val<65535)
				track_val = pwmi_val / 154;
		}

		if (keypressed == 8) {
			bitfield.brightness = 0;
			return;
		}

		EVE_CoCmd_dlStart(s_pHalContext);
		EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1));

		EVE_Cmd_wr32(s_pHalContext, BEGIN(BITMAPS));
		EVE_Cmd_wr32(s_pHalContext, SAVE_CONTEXT());
		EVE_Cmd_wr32(s_pHalContext, BITMAP_TRANSFORM_A(128));
		EVE_Cmd_wr32(s_pHalContext, BITMAP_TRANSFORM_E(128));
		EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 255, 255));
		EVE_Cmd_wr32(s_pHalContext, VERTEX2II(0, 0, 1, 0));
		EVE_Cmd_wr32(s_pHalContext, RESTORE_CONTEXT());
		animation();

		EVE_Cmd_wr32(s_pHalContext, COLOR_A(255));
		EVE_CoCmd_text(s_pHalContext, 200, 10, font[val_font], OPT_CENTERX, "Track the bar to adjust the brightness");
		EVE_Cmd_wr32(s_pHalContext, TAG(8));
		EVE_Cmd_wr32(s_pHalContext, VERTEX2II(450, 5, 3, 3));

		EVE_Cmd_wr32(s_pHalContext, TAG(12));
		EVE_Cmd_wr32(s_pHalContext, SCISSOR_XY(20, 200)); // Scissor rectangle bottom left at (20, 200)
		EVE_Cmd_wr32(s_pHalContext, SCISSOR_SIZE(420, 40)); // Scissor rectangle is 420 x 40 pixels
		EVE_CoCmd_gradient(s_pHalContext, 20, 0, 0x0000ff, 440, 0, 0xff0000);
		EVE_Cmd_wr32(s_pHalContext, CLEAR_COLOR_RGB(255, 255, 255));

		EVE_Cmd_wr32(s_pHalContext, COLOR_A(255));

		if (track_val)
			val = 10 + (track_val / 3);
		EVE_Hal_wr8(s_pHalContext, REG_PWM_DUTY, val);

		EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 255, 255));
		EVE_Cmd_wr32(s_pHalContext, BEGIN(RECTS));

		EVE_Cmd_wr32(s_pHalContext, VERTEX2F((20 + track_val) * 16, 190 * 16));
		EVE_Cmd_wr32(s_pHalContext, VERTEX2F(440 * 16, 250 * 16));

		EVE_Cmd_wr32(s_pHalContext, DISPLAY());
		EVE_CoCmd_swap(s_pHalContext);
		EVE_Cmd_waitFlush(s_pHalContext);

	}
}
void scrensaver()
{
	int32_t xoffset, yoffset, flagloop, xflag = 1, yflag = 1;
	uint8_t Read_tag = 0;

	xoffset = s_pHalContext->Width / 3;
	yoffset = s_pHalContext->Height / 2;

	while (1)
	{
		if (istouch())
		{
			sleep_cts = 0;
			return;
		}
		else
		{
			EVE_Cmd_wr32(s_pHalContext, CMD_SCREENSAVER);//screen saver command will continuously update the macro0 with vertex2f command	
			EVE_CoCmd_dlStart(s_pHalContext);
			EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1));

			EVE_Cmd_wr32(s_pHalContext, BEGIN(BITMAPS));
			EVE_Cmd_wr32(s_pHalContext, BITMAP_TRANSFORM_A(128));
			EVE_Cmd_wr32(s_pHalContext, BITMAP_TRANSFORM_E(128));
			EVE_Cmd_wr32(s_pHalContext, VERTEX2II(0, 0, 1, 0));

			EVE_CoCmd_loadIdentity(s_pHalContext);
			EVE_CoCmd_scale(s_pHalContext, 2 * 65536, 2 * 65536);//scale the bitmap 3 times on both sides
			EVE_CoCmd_setMatrix(s_pHalContext);
			EVE_Cmd_wr32(s_pHalContext, BEGIN(BITMAPS));
			EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 255, 255));
			EVE_Cmd_wr32(s_pHalContext, BITMAP_HANDLE(4));
			EVE_Cmd_wr32(s_pHalContext, MACRO(0));
			EVE_Cmd_wr32(s_pHalContext, END());
			EVE_Cmd_wr32(s_pHalContext, DISPLAY());
			EVE_CoCmd_swap(s_pHalContext);
			EVE_Cmd_waitFlush(s_pHalContext);

		}
	}
}

void DemoRefrigerator(EVE_HalContext* pHalContext) {
	s_pHalContext = pHalContext;
	int32_t xoffset1, xoffest2, BMoffsetx1, BMoffsety1, BMoffsetx2, BMoffsety2, y, j;
	uint32_t val = 0, tracker = 0, tracking_tag = 0, track_val_min, track_val_max, unit_val, val_font = 0;
	char8_t img_n;
	uint16_t track_val = 0, cubed = 0;
	int16_t rval = 0, pwm_val = 0, font_number, m, x;
	int16_t freeze_temp = 0, fridge_temp = 0;
	int32_t Baddr;
	uint32_t trackval = 0;
	uint8_t k, ice_cell;
	static uint8_t blob_i;
	uchar8_t r, g, b;
#ifndef FT9XX_PLATFORM
	SYSTEMTIME lt;
#else

#endif
	uint16_t yoff;
	uint8_t Clocks_DL[548], snapflag = 0;
	uint8_t keypressed;
	const char ice[2][8] = { "Cubed","Crushed" };
	uint32_t thcurr = 0, thcurrval = 0, pwmval_return = 0;
	uint8_t Read_tag = 0;
	int32_t i = 0, pwm = 0;

	uint8_t fonts_size_t = 1;

	xoffset1 = 10;
	xoffest2 = 50;
	thcurr = -2;
	thcurrval = 34;
	cubed = 0;

	EVE_CoCmd_dlStart(s_pHalContext);
	EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1));
	EVE_Cmd_wr32(s_pHalContext, TAG_MASK(1));
	EVE_Cmd_wr32(s_pHalContext, SAVE_CONTEXT());

	Baddr = RAM_G;
	DemoLoadRawFromFile("gn.raw", RAM_G);//240 * 136	green and blue
	EVE_Cmd_wr32(s_pHalContext, BITMAP_HANDLE(1));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_SOURCE(RAM_G));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_LAYOUT(RGB565, 240 * 2, 136));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_SIZE(BILINEAR, BORDER, BORDER, 512, 512));

	Baddr = RAM_G + 240 * 2 * 136;
	//240 * 136	green and blue
	DemoLoadRawFromFile("5icons_5.raw", Baddr);
	EVE_Cmd_wr32(s_pHalContext, BITMAP_HANDLE(2));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_SOURCE(Baddr));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_LAYOUT(L4, 18, 37));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_SIZE(BILINEAR, BORDER, BORDER, 35, 35));

	Baddr += 221 * (36 / 2);
	DemoLoadRawFromFile("f1icons.raw", Baddr);
	EVE_Cmd_wr32(s_pHalContext, BITMAP_HANDLE(3));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_SOURCE(Baddr));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_LAYOUT(L4, 15, 32));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_SIZE(NEAREST, BORDER, BORDER, 30, 32));

	Baddr += 192 * (32 / 2);
	DemoLoadRawFromFile("logo1.raw", Baddr);
	EVE_Cmd_wr32(s_pHalContext, BITMAP_HANDLE(4));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_SOURCE(Baddr));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_LAYOUT(ARGB4, 66 * 2, 20));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_SIZE(BILINEAR, BORDER, BORDER, 512, 512));

	Baddr += 66 * 20 * 2;
	DemoLoadRawFromFile("snow_1.raw", Baddr);
	EVE_Cmd_wr32(s_pHalContext, BITMAP_HANDLE(6));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_SOURCE(Baddr));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_LAYOUT(L4, 25, 50));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_SIZE(BILINEAR, BORDER, BORDER, 50, 50));
	Baddr += 50 * (50 / 2);

	EVE_Cmd_wr32(s_pHalContext, END());

	EVE_CoCmd_track(s_pHalContext, 170, 180, 120, 8, 17);
	EVE_CoCmd_swap(s_pHalContext);
	EVE_Cmd_waitFlush(s_pHalContext);
	EVE_Hal_wr8(s_pHalContext, REG_PWM_DUTY, 128);//App_fadein();

	val = 128;
	pwm_val = 10;
	rval = 0;

	/* compute the random values at the starting*/
	pRefrigeratorSnow = S_RefrigeratorSnowArray;
	for (j = 0; j<(NumSnowRange*NumSnowEach); j++)
	{
		//always start from the right and move towards left
		pRefrigeratorSnow->xOffset = random(s_pHalContext->Height * 16);//random(s_pHalContext->Width*16);
		pRefrigeratorSnow->yOffset = random(s_pHalContext->Width * 16);//random(s_pHalContext->Height*16);
		pRefrigeratorSnow->dx = random(RandomVal * 8) - RandomVal * 8;//-1*random(RandomVal*8);//always -ve
		pRefrigeratorSnow->dy = -1 * random(RandomVal * 8);//random(RandomVal*8) - RandomVal*8;
		pRefrigeratorSnow++;
	}

	while (1)
	{
#ifndef FT9XX_PLATFORM
		GetLocalTime(&lt);
#endif
		Read_tag = Gesture_GetTag(s_pHalContext);
		keypressed = EVE_Hal_rd8(s_pHalContext, REG_TOUCH_TAG);
		if (!istouch() && bitfield.unlock)
		{
			sleep_cts++;
			if (sleep_cts>100)
				scrensaver();
		}

		pwm_val = (val - rval) / 16;
		rval += pwm_val;
		if (rval <= 10)
			rval = 10;
		if (keypressed == 17 || keypressed == 12)
			tracker = EVE_Hal_rd32(s_pHalContext, REG_TRACKER);

		if (Read_tag == 1)
		{
			bitfield.unlock = (bitfield.unlock == 0) ? 1 : 0;
			if (bitfield.unlock == 0)
				EVE_Hal_wr8(s_pHalContext, REG_PWM_DUTY, rval);
		}
		if (Read_tag == 2)
			bitfield.settings = 1;
		if ((Read_tag == 8) &(bitfield.settings == 1))
			bitfield.settings = 0;
		if ((Read_tag == 8) &(bitfield.font_size == 1))
			bitfield.font_size = 0;
		if (Read_tag == 9)
			bitfield.change_background = 1;
		if (Read_tag == 10)
			bitfield.font_size ^= 1;
		if (Read_tag == 3)
			bitfield.brightness ^= 1;
		if (Read_tag == 4)
			bitfield.sketch = 1;
		if (Read_tag == 11)
			bitfield.food_tracker = 1;
		if (Read_tag == 5)
			bitfield.freeze = 1;
		if (Read_tag == 6)
			bitfield.fridge = 1;
		if (Read_tag == 7)
			bitfield.cubed = 1;
		if (Read_tag == 15)
			fonts_size_t = 0;
		if (Read_tag == 16)
			fonts_size_t = 1;

		if (keypressed == 17)
		{
			trackval = tracker >> 16;
			val_font = (trackval / 16384);
		}

		EVE_CoCmd_dlStart(s_pHalContext);
		EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1));

		EVE_Cmd_wr32(s_pHalContext, SAVE_CONTEXT());
		EVE_Cmd_wr32(s_pHalContext, TAG_MASK(1));
		EVE_Cmd_wr32(s_pHalContext, BEGIN(BITMAPS));
		EVE_Cmd_wr32(s_pHalContext, BITMAP_TRANSFORM_A(128));
		EVE_Cmd_wr32(s_pHalContext, BITMAP_TRANSFORM_E(128));
		EVE_Cmd_wr32(s_pHalContext, TAG(0));
		EVE_Cmd_wr32(s_pHalContext, VERTEX2II(0, 0, 1, 0));
		EVE_Cmd_wr32(s_pHalContext, RESTORE_CONTEXT());
		EVE_CoCmd_loadIdentity(s_pHalContext);
		EVE_CoCmd_scale(s_pHalContext, 65536, 65536);//scale the bitmap 3 times on both sides
		EVE_CoCmd_setMatrix(s_pHalContext);
		EVE_Cmd_wr32(s_pHalContext, VERTEX2II(280, 0, 4, 0));

		EVE_Cmd_wr32(s_pHalContext, RESTORE_CONTEXT());
		animation();

		EVE_Cmd_wr32(s_pHalContext, RESTORE_CONTEXT());

		EVE_CoCmd_number(s_pHalContext, (270 - (val_font * 38)), 135, font[val_font], OPT_CENTERX | OPT_CENTERY | OPT_SIGNED, thcurr);
		EVE_CoCmd_text(s_pHalContext, (282 - (val_font * 37)), 135, 17, OPT_CENTERX | OPT_CENTERY, "x");
		EVE_CoCmd_text(s_pHalContext, (292 - (val_font * 38)), 135, font[val_font], OPT_CENTERX | OPT_CENTERY, "F");

		EVE_CoCmd_number(s_pHalContext, (350 - (val_font * 37)), 135, font[val_font], OPT_CENTERX | OPT_CENTERY, thcurrval);
		EVE_CoCmd_text(s_pHalContext, (362 - (val_font * 36)), 135, 17, OPT_CENTERX | OPT_CENTERY, "x");
		EVE_CoCmd_text(s_pHalContext, (372 - (val_font * 36)), 135, font[val_font], OPT_CENTERX | OPT_CENTERY, "F");

		ice_cell = (cubed == 0) ? 4 : 5;
		EVE_Cmd_wr32(s_pHalContext, VERTEX2II((420 - (val_font * 30)), 120, 3, ice_cell));

		EVE_Cmd_wr32(s_pHalContext, TAG(1));
		EVE_Cmd_wr32(s_pHalContext, BEGIN(BITMAPS));
		if (bitfield.unlock)
		{
			EVE_Cmd_wr32(s_pHalContext, VERTEX2II(xoffset1 + 5, 200, 2, 0));
			EVE_Cmd_wr32(s_pHalContext, TAG(0));
			EVE_Hal_wr8(s_pHalContext, REG_PWM_DUTY, 25);
			EVE_Cmd_wr32(s_pHalContext, TAG_MASK(0));

		}
		else
		{
			EVE_Cmd_wr32(s_pHalContext, VERTEX2II(xoffset1 + 5, 200, 2, 1));
		}

		EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 255, 255));
		EVE_Cmd_wr32(s_pHalContext, TAG(2));
		EVE_Cmd_wr32(s_pHalContext, VERTEX2II(xoffset1 + 70, 200, 2, 2));
		EVE_Cmd_wr32(s_pHalContext, TAG(3));
		EVE_Cmd_wr32(s_pHalContext, VERTEX2II(xoffset1 + 142, 200, 2, 3));
		EVE_Cmd_wr32(s_pHalContext, TAG(4));
		EVE_Cmd_wr32(s_pHalContext, VERTEX2II(xoffset1 + 210, 200, 2, 4));
		EVE_Cmd_wr32(s_pHalContext, TAG(11));
		EVE_Cmd_wr32(s_pHalContext, VERTEX2II(xoffset1 + 280, 200, 2, 5));

		EVE_Cmd_wr32(s_pHalContext, TAG(5));
		EVE_Cmd_wr32(s_pHalContext, VERTEX2II(250 - (val_font * 50), 95, 3, 0));
		EVE_CoCmd_text(s_pHalContext, (300 - (val_font * 45)), 110, font[val_font], OPT_CENTERX | OPT_CENTERY, "Freeze");

		EVE_Cmd_wr32(s_pHalContext, TAG(6));
		EVE_Cmd_wr32(s_pHalContext, VERTEX2II(320 - (val_font * 40), 95, 3, 1));
		EVE_CoCmd_text(s_pHalContext, (370 - (val_font * 37)), 110, font[val_font], OPT_CENTERX | OPT_CENTERY, "Fridge");

		EVE_Cmd_wr32(s_pHalContext, TAG(7));
		EVE_Cmd_wr32(s_pHalContext, VERTEX2II((400 - (val_font * 30)), 95, 3, 2));
		EVE_CoCmd_text(s_pHalContext, (450 - (val_font * 25)), 110, font[val_font], OPT_CENTERX | OPT_CENTERY, ice[cubed]);

		//Disable tags for the following objects
		EVE_Cmd_wr32(s_pHalContext, TAG(255));
		switch (val_font)
		{
		case 0:
		{
			EVE_CoCmd_text(s_pHalContext, 53, 34, font[val_font], OPT_CENTERX | OPT_CENTERY, months + (lt.wMonth - 1));
			EVE_CoCmd_number(s_pHalContext, 98, 34, font[val_font], OPT_CENTERX | OPT_CENTERY, lt.wDay);
			EVE_CoCmd_text(s_pHalContext, 108, 34, font[val_font], OPT_CENTERX | OPT_CENTERY, ",");
			EVE_CoCmd_number(s_pHalContext, 128, 34, font[val_font], OPT_CENTERX | OPT_CENTERY, lt.wYear);
			EVE_CoCmd_number(s_pHalContext, 40, 54, font[val_font], OPT_CENTERX | OPT_CENTERY, lt.wHour);
			EVE_CoCmd_text(s_pHalContext, 50, 54, font[val_font], OPT_CENTERX | OPT_CENTERY, ":");
			EVE_CoCmd_number(s_pHalContext, 60, 54, font[val_font], OPT_CENTERX | OPT_CENTERY, lt.wMinute);
			break;
		}

		case 1: {
			EVE_CoCmd_text(s_pHalContext, 65, 34, font[val_font], OPT_CENTERX | OPT_CENTERY, months + (lt.wMonth - 1));
			EVE_CoCmd_number(s_pHalContext, 120, 34, font[val_font], OPT_CENTERX | OPT_CENTERY, lt.wDay);
			EVE_CoCmd_text(s_pHalContext, 130, 34, font[val_font], OPT_CENTERX | OPT_CENTERY, ",");
			EVE_CoCmd_number(s_pHalContext, 155, 34, font[val_font], OPT_CENTERX | OPT_CENTERY, lt.wYear);
			EVE_CoCmd_number(s_pHalContext, 48, 54, font[val_font], OPT_CENTERX | OPT_CENTERY, lt.wHour);
			EVE_CoCmd_text(s_pHalContext, 58, 54, font[val_font], OPT_CENTERX | OPT_CENTERY, ":");
			EVE_CoCmd_number(s_pHalContext, 70, 54, font[val_font], OPT_CENTERX | OPT_CENTERY, lt.wMinute);
			break;
		}
		case 2: {
			EVE_CoCmd_text(s_pHalContext, 65, 34, font[val_font], OPT_CENTERX | OPT_CENTERY, months + (lt.wMonth - 1));
			EVE_CoCmd_number(s_pHalContext, 130, 34, font[val_font], OPT_CENTERX | OPT_CENTERY, lt.wDay);
			EVE_CoCmd_text(s_pHalContext, 145, 34, font[val_font], OPT_CENTERX | OPT_CENTERY, ",");
			EVE_CoCmd_number(s_pHalContext, 175, 34, font[val_font], OPT_CENTERX | OPT_CENTERY, lt.wYear);
			EVE_CoCmd_number(s_pHalContext, 60, 54, font[val_font], OPT_CENTERX | OPT_CENTERY, lt.wHour);
			EVE_CoCmd_text(s_pHalContext, 75, 54, font[val_font], OPT_CENTERX | OPT_CENTERY, ":");
			EVE_CoCmd_number(s_pHalContext, 87, 54, font[val_font], OPT_CENTERX | OPT_CENTERY, lt.wMinute);
			break;
		}
		case 3: {
			EVE_CoCmd_text(s_pHalContext, 65, 34, font[val_font], OPT_CENTERX | OPT_CENTERY, months + (lt.wMonth - 1));
			EVE_CoCmd_number(s_pHalContext, 140, 34, font[val_font], OPT_CENTERX | OPT_CENTERY, lt.wDay);
			EVE_CoCmd_text(s_pHalContext, 160, 34, font[val_font], OPT_CENTERX | OPT_CENTERY, ",");
			EVE_CoCmd_number(s_pHalContext, 190, 34, font[val_font], OPT_CENTERX | OPT_CENTERY, lt.wYear);
			EVE_CoCmd_number(s_pHalContext, 48, 64, font[val_font], OPT_CENTERX | OPT_CENTERY, lt.wHour);
			EVE_CoCmd_text(s_pHalContext, 63, 64, font[val_font], OPT_CENTERX | OPT_CENTERY, ":");
			EVE_CoCmd_number(s_pHalContext, 83, 64, font[val_font], OPT_CENTERX | OPT_CENTERY, lt.wMinute);
			break;
		}

		}

		if (bitfield.settings)
		{
			EVE_Cmd_wr32(s_pHalContext, BEGIN(RECTS));
			EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(34, 50, 224));
			EVE_Cmd_wr32(s_pHalContext, VERTEX2F(150 * 16, 30 * 16));
			EVE_Cmd_wr32(s_pHalContext, VERTEX2F(370 * 16, 60 * 16));
			EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 255, 255));
			EVE_CoCmd_text(s_pHalContext, 240, 45, font[val_font], OPT_CENTERX | OPT_CENTERY, "SETTINGS");

			EVE_Cmd_wr32(s_pHalContext, TAG(9));
			EVE_Cmd_wr32(s_pHalContext, BEGIN(RECTS));
			EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(162, 229, 242));
			EVE_Cmd_wr32(s_pHalContext, VERTEX2F(150 * 16, 60 * 16));
			EVE_Cmd_wr32(s_pHalContext, VERTEX2F(370 * 16, 130 * 16));
			EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0, 0, 0));
			EVE_CoCmd_text(s_pHalContext, 260, 85, font[val_font], OPT_CENTERX | OPT_CENTERY, "Change Background");

			EVE_Cmd_wr32(s_pHalContext, TAG(10));
			EVE_Cmd_wr32(s_pHalContext, BEGIN(RECTS));
			EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(34, 50, 224));
			EVE_Cmd_wr32(s_pHalContext, VERTEX2F(150 * 16, 110 * 16));
			EVE_Cmd_wr32(s_pHalContext, VERTEX2F(370 * 16, 160 * 16));
			EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0, 0, 0));
			EVE_CoCmd_text(s_pHalContext, 250, 130, font[val_font], OPT_CENTERX | OPT_CENTERY, "Change Font Size");

			EVE_Cmd_wr32(s_pHalContext, TAG(8));
			EVE_Cmd_wr32(s_pHalContext, BEGIN(BITMAPS));//Bitmap of the home icon
			EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 255, 255));
			EVE_Cmd_wr32(s_pHalContext, VERTEX2II(320, 30, 3, 3));

		}

		if (bitfield.font_size)
		{
			EVE_Cmd_wr32(s_pHalContext, TAG(17));
			EVE_CoCmd_slider(s_pHalContext, 170, 180, 120, 8, 0, trackval, 65535);
			EVE_CoCmd_text(s_pHalContext, 200, 170, font[val_font], OPT_CENTERX | OPT_CENTERY, "FONT SIZE");
			EVE_CoCmd_number(s_pHalContext, 170, 200, font[val_font], OPT_CENTERX | OPT_CENTERY, val_font);
		}

		EVE_Cmd_wr32(s_pHalContext, RESTORE_CONTEXT());
		EVE_Cmd_wr32(s_pHalContext, TAG_MASK(0));

		EVE_Cmd_wr32(s_pHalContext, END());
		EVE_Cmd_wr32(s_pHalContext, DISPLAY());
		EVE_CoCmd_swap(s_pHalContext);
		EVE_Cmd_waitFlush(s_pHalContext);

		if (bitfield.cubed)
			cubed_setting(&cubed, val_font);
		if (bitfield.brightness)
		{
			change_brightness(val, val_font);
		}

		if (bitfield.change_background)
			change_background();
		if (bitfield.sketch)
			Sketch(val_font);
		if (bitfield.freeze | bitfield.fridge)// | bitfield.cubed)
			change_temperature(&thcurr, &thcurrval, val_font);
		if (bitfield.food_tracker)
			food_tracker(val_font);

	}

	scrensaver();
}

