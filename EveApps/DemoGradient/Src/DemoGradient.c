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

#define RAW  4096
#define LUT  421184 
#define SIZ  (LUT+1024)

int getRGB(int trackValue) {
	const int maxTrack = 0xFFFF;
	const int maxColor = 0xFC;
	const int minColor = 0x03;
	const int numPart = 6;
	const int partLen = maxTrack / numPart;
	const int colorRangePerPart = maxColor - minColor;

	int partColor = trackValue / partLen;
	int colorIndex=0; // index in RGB: r=2, g=1, b=0
	int rgb = 0x030303;// Return value
	char direction = 0; // 0 is down, 1 is up

	if (trackValue == maxTrack) partColor--;

	switch (partColor) {
	case 0:
		colorIndex = 1;
		rgb = 0xFC0003;
		direction = 0;
		break;
	case 1:
		colorIndex = 2; 
		rgb = 0x00FC03;
		direction = 1;
		break;
	case 2:
		colorIndex = 0; 
		rgb = 0x03FC00;
		direction = 0;
		break;
	case 3:
		colorIndex = 1; 
		rgb = 0x0300FC;
		direction = 1;
		break;
	case 4:
		colorIndex = 2; 
		rgb = 0x0003FC;
		direction = 0;
		break;
	case 5:
		colorIndex = 0; 
		rgb = 0xFC0300;
		direction = 1;
		break;
	default:break;
	}

	int trackPart = trackValue % partLen;
	if (trackValue == maxTrack) trackPart = partLen;

	if (direction == 1) {
		trackPart = partLen - trackPart;
	}
	// color range from minColor to maxColor
	// track range from 0 to trackPart
	// => color8b = trackValue / maxTrack * "color range"
	int color8b = (float)(trackPart * 1.0 / partLen) * (maxColor - minColor) + 0x03;
	rgb = rgb | color8b << (colorIndex * 8);

	return rgb;
}

void DemoGradient(EVE_HalContext* pHalContext) {
	s_pHalContext = pHalContext;
	uint32_t Read_xy = 0,
		tracker;
	uint16_t x1, x2, y1, y2,
		Read_x = 0, Read_y = 0,

		tx = 48, ty = 20, tval;
	uint32_t val1 = 48568, val2 = 32765;
	uint8_t
		Read_Tag = 0,
		drag, buff[512];
	int32_t TPsize;

	x1 = 50; y1 = 50;

	TPsize = s_pHalContext->Width * 5 / 100;

	x2 = s_pHalContext->Width - 130;
	y2 = s_pHalContext->Height - 60;
	drag = 0;
	// compute the background gradient effect
	for (tval = 0; tval<(s_pHalContext->Height / 2); tval++)
	{
		buff[s_pHalContext->Height - 1 - tval] = buff[tval] = (tval*0.9);
	}
	EVE_Hal_wrMem(s_pHalContext, 4096L, buff, s_pHalContext->Height);

	// Set the bitmap properties for the background
	App_Set_DlBuffer_Index(0);
	App_WrDl_Buffer(s_pHalContext, CLEAR(1, 1, 1));
	App_WrDl_Buffer(s_pHalContext, COLOR_A(255));
	App_WrDl_Buffer(s_pHalContext, COLOR_RGB(255, 255, 255));
	App_WrDl_Buffer(s_pHalContext, BITMAP_HANDLE(2));
	App_WrDl_Buffer(s_pHalContext, BITMAP_SOURCE(4096L));
	App_WrDl_Buffer(s_pHalContext, BITMAP_LAYOUT(L8, 1, s_pHalContext->Height));
	App_WrDl_Buffer(s_pHalContext, BITMAP_SIZE(NEAREST, REPEAT, BORDER, 512, s_pHalContext->Height));
	EVE_CoCmd_track(s_pHalContext, (s_pHalContext->Width - 38), 40, 8, s_pHalContext->Height - 65, 3);
	EVE_CoCmd_track(s_pHalContext, (s_pHalContext->Width - 15), 40, 8, s_pHalContext->Height - 65, 4);
	App_WrDl_Buffer(s_pHalContext, DISPLAY());
	App_Flush_DL_Buffer(s_pHalContext);
	EVE_Hal_wr8(s_pHalContext, REG_DLSWAP, DLSWAP_FRAME);
	EVE_Cmd_waitFlush(s_pHalContext);
	
	int	rgb1 = getRGB(val1);
	int	rgb2 = getRGB(val2);

	do
	{
		/* Read the Touch\drag*/
		Read_xy = EVE_Hal_rd32(s_pHalContext, REG_TOUCH_SCREEN_XY);
		Read_Tag = EVE_Hal_rd8(s_pHalContext, REG_TOUCH_TAG);
		// pick the x& y cordinates of the points  
		Read_y = Read_xy & 0xffff;
		Read_x = (Read_xy >> 16) & 0xffff;
		if (Read_xy == 0x80008000)  drag = 0;
		else if (Read_Tag == 1 || Read_Tag == 2)drag = Read_Tag;
		/* compute the coordintes of two points x&y*/
		if (drag == 1)
		{
			if (Read_x >= (s_pHalContext->Width - 60))0; else	x1 = Read_x;
			if (Read_y >= (s_pHalContext->Height - 20))0; else	y1 = Read_y;
		}
		if (drag == 2)
		{
			if (Read_x >= (s_pHalContext->Width - 60))0; else	x2 = Read_x;
			if (Read_y >= (s_pHalContext->Height - 20))0; else	y2 = Read_y;
		}
		tracker = EVE_Hal_rd32(s_pHalContext, REG_TRACKER);
		if ((tracker & 0xff) > 2)
		{
			if ((tracker & 0xff) == 3)
			{
				val1 = (tracker >> 16);
				rgb1 = getRGB(val1);
			}
			else if ((tracker & 0xff) == 4)
			{
				val2 = (tracker >> 16);
				rgb2 = getRGB(val2);
			}
		}
		// start the new displaylist
		EVE_CoCmd_dlStart(s_pHalContext);
		EVE_Cmd_wr32(s_pHalContext, CLEAR_COLOR_RGB(55, 55, 55));
		EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1));
		EVE_Cmd_wr32(s_pHalContext, BEGIN(BITMAPS));
		EVE_Cmd_wr32(s_pHalContext, VERTEX2II(0, 0, 2, 0));
		EVE_CoCmd_fgColor(s_pHalContext, 0xffffff);

		EVE_Cmd_wr32(s_pHalContext, TAG_MASK(1));

		EVE_CoCmd_bgColor(s_pHalContext, rgb2);
		EVE_Cmd_wr32(s_pHalContext, TAG(4));
		EVE_CoCmd_slider(s_pHalContext, (s_pHalContext->Width - 40), 40, 18, (s_pHalContext->Height - 65), 0, val2, 65535);

		EVE_CoCmd_bgColor(s_pHalContext, rgb1);
		EVE_Cmd_wr32(s_pHalContext, TAG(3));
		EVE_CoCmd_slider(s_pHalContext, (s_pHalContext->Width - 80), 40, 18, (s_pHalContext->Height - 65), 0, val1, 65535);
		EVE_Cmd_wr32(s_pHalContext, TAG_MASK(0));

		EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 255, 255));

		EVE_CoCmd_text(s_pHalContext, s_pHalContext->Width - 80, 10, 26, 0, "C1");
		EVE_CoCmd_text(s_pHalContext, s_pHalContext->Width - 40, 10, 26, 0, "C2");
		EVE_Cmd_wr32(s_pHalContext, SCISSOR_XY(10, 10));
		EVE_Cmd_wr32(s_pHalContext, SCISSOR_SIZE(s_pHalContext->Width - 100, s_pHalContext->Height - 30));
		EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1));
		EVE_CoCmd_gradient(s_pHalContext, x1, y1, rgb1, x2, y2, rgb2);

		EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0, 0, 0));
		// Place a dot for drag the color
		EVE_Cmd_wr32(s_pHalContext, BEGIN(FTPOINTS));
		EVE_Cmd_wr32(s_pHalContext, COLOR_MASK(0, 0, 0, 0));
		EVE_Cmd_wr32(s_pHalContext, POINT_SIZE(TPsize * 16));
		EVE_Cmd_wr32(s_pHalContext, TAG_MASK(1));
		EVE_Cmd_wr32(s_pHalContext, TAG(1));
		EVE_Cmd_wr32(s_pHalContext, VERTEX2F(x1 * 16, y1 * 16));
		EVE_Cmd_wr32(s_pHalContext, TAG(2));
		EVE_Cmd_wr32(s_pHalContext, VERTEX2F(x2 * 16, y2 * 16));
		EVE_Cmd_wr32(s_pHalContext, TAG_MASK(0));

		/* calculation of touch point size */

		EVE_Cmd_wr32(s_pHalContext, COLOR_MASK(1, 1, 1, 1));
		EVE_Cmd_wr32(s_pHalContext, POINT_SIZE(TPsize * 16));
		EVE_Cmd_wr32(s_pHalContext, BEGIN(FTPOINTS));
		EVE_Cmd_wr32(s_pHalContext, VERTEX2F(x1 * 16, y1 * 16));
		EVE_Cmd_wr32(s_pHalContext, VERTEX2F(x2 * 16, y2 * 16));
		EVE_Cmd_wr32(s_pHalContext, COLOR_MASK(1, 1, 1, 1));
		EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 255, 255));
		EVE_Cmd_wr32(s_pHalContext, BEGIN(LINES));
		EVE_Cmd_wr32(s_pHalContext, VERTEX2F((x1 - TPsize) * 16, (y1) * 16));
		EVE_Cmd_wr32(s_pHalContext, VERTEX2F((x1 + TPsize) * 16, (y1) * 16));
		EVE_Cmd_wr32(s_pHalContext, VERTEX2F((x1) * 16, (y1 - TPsize) * 16));
		EVE_Cmd_wr32(s_pHalContext, VERTEX2F((x1) * 16, (y1 + TPsize) * 16));
		EVE_Cmd_wr32(s_pHalContext, VERTEX2F((x2 - TPsize) * 16, (y2) * 16));
		EVE_Cmd_wr32(s_pHalContext, VERTEX2F((x2 + TPsize) * 16, (y2) * 16));
		EVE_Cmd_wr32(s_pHalContext, VERTEX2F((x2) * 16, (y2 - TPsize) * 16));
		EVE_Cmd_wr32(s_pHalContext, VERTEX2F((x2) * 16, (y2 + TPsize) * 16));
		EVE_Cmd_wr32(s_pHalContext, END());
		EVE_Cmd_wr32(s_pHalContext, DISPLAY());
		EVE_CoCmd_swap(s_pHalContext);
		EVE_Cmd_waitFlush(s_pHalContext);
	} while (1);
}
