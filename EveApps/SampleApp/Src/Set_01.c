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
  *     > Logo
  *     > Widget_Clock
  *     > Widget_Guage
  *     > Widget_Gradient
  *     > Widget_Keys
  *     > Widget_Keys_Interactive
  *     > Widget_Progressbar
  *     > Widget_Scroll
  *     > Widget_Slider
  *     > Widget_Dial
  *     > Widget_Toggle
  *     > Widget_Spinner
  *     > SAMAPP_PowerMode
  *     > Touch
  */
#include <assert.h>

#include "Platform.h"
#include "Gpu_Hal.h"
#include "Common.h"
#include "App.h"
#include "Samapp_Enable_API.h"

#ifdef SAMAPP_ENABLE_APIS_SET1
static EVE_HalContext *s_pHalContext;

/**
 * @brief Show logo
 *
 */
void SAMAPP_CoPro_Logo()
{
	Logo_Showup(s_pHalContext);

	printf("Logo animation has finished playing.\n");

	SAMAPP_ENABLE_DELAY();
	Display_Start(s_pHalContext);
	Display_End(s_pHalContext);

	EVE_Util_clearScreen(s_pHalContext);
}

/**
 * @brief demonstrate clock widget
 *
 */
void SAMAPP_CoPro_Widget_Clock()
{
	/*************************************************************************/
	/* Below code demonstrates the usage of clock function. Clocks can be    */
	/* constructed using flat or 3d effect. Clock background and foreground  */
	/* colors can be set by gbcolor and colorrgb. Clock can be constructed   */
	/* with multiple options such as no background, no needles, no pointer.  */
	/*************************************************************************/
	int16_t xOffset;
	int16_t yOffset;
	int16_t radius;
	int16_t xDistBtwClocks;

	xDistBtwClocks = (int16_t)(s_pHalContext->Width / 5);
	radius = xDistBtwClocks / 2 - (int16_t)(s_pHalContext->Width / 64);

	/* Download the bitmap data for lena faced clock */
	Ftf_Write_File_nBytes_To_RAM_G(s_pHalContext, TEST_DIR "\\SAMAPP_Bitmap_RawData.bin", RAM_G,
		SAMAPP_Bitmap_RawData_Header[0].Stride * SAMAPP_Bitmap_RawData_Header[0].Height,
		SAMAPP_Bitmap_RawData_Header[0].Arrayoffset);

	/* Draw clock with blue as background and read as needle color */
	EVE_CoCmd_dlStart(s_pHalContext);
	EVE_Cmd_wr32(s_pHalContext, CLEAR_COLOR_RGB(64, 64, 64));
	EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1));
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 255, 255));
	/* flat effect and default color background */
	xOffset = xDistBtwClocks / 2;
	yOffset = radius + 5;
	EVE_CoCmd_bgColor(s_pHalContext, 0x0000ff);
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0xff, 0x00, 0x00));
	EVE_CoCmd_clock(s_pHalContext, xOffset, yOffset, radius, OPT_FLAT, 30, 100, 5, 10);
	EVE_CoCmd_text(s_pHalContext, xOffset, (yOffset + radius + 6), 26, OPT_CENTER, "Flat effect");//text info
	/* no seconds needle */
	EVE_CoCmd_bgColor(s_pHalContext, 0x00ff00);
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0xff, 0x00, 0x00));
	EVE_CoCmd_fgColor(s_pHalContext, 0xff0000);
	xOffset += xDistBtwClocks;
	EVE_CoCmd_clock(s_pHalContext, xOffset, yOffset, radius, OPT_NOSECS, 10, 10, 5, 10);
	EVE_Cmd_wr32(s_pHalContext, COLOR_A(255));
	EVE_CoCmd_text(s_pHalContext, xOffset, (yOffset + radius + 6), 26, OPT_CENTER, "No Secs");//text info
	/* no background color */
	EVE_CoCmd_bgColor(s_pHalContext, 0x00ffff);
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0xff, 0xff, 0x00));
	xOffset += xDistBtwClocks;
	EVE_CoCmd_clock(s_pHalContext, xOffset, yOffset, radius, OPT_NOBACK, 10, 10, 5, 10);
	EVE_CoCmd_text(s_pHalContext, xOffset, (yOffset + radius + 6), 26, OPT_CENTER, "No BG");//text info
	/* No ticks */
	EVE_CoCmd_bgColor(s_pHalContext, 0xff00ff);
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0x00, 0xff, 0xff));
	xOffset += xDistBtwClocks;
	EVE_CoCmd_clock(s_pHalContext, xOffset, yOffset, radius, OPT_NOTICKS, 10, 10, 5, 10);
	EVE_CoCmd_text(s_pHalContext, xOffset, (yOffset + radius + 6), 26, OPT_CENTER, "No Ticks");//text info
	/* No hands */
	EVE_CoCmd_bgColor(s_pHalContext, 0x808080);
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0x00, 0xff, 0x00));
	xOffset += xDistBtwClocks;
	EVE_CoCmd_clock(s_pHalContext, xOffset, yOffset, radius, OPT_NOHANDS, 10, 10, 5, 10);
	EVE_CoCmd_text(s_pHalContext, xOffset, (yOffset + radius + 6), 26, OPT_CENTER, "No Hands");//text info
	/* Bigger clock */
	yOffset += (radius + 10);
	radius = (int16_t)(s_pHalContext->Height - (2 * radius + 5 + 10));//calculate radius based on remaining height
	radius = (radius - 5 - 20) / 2;
	xOffset = radius + 10;
	yOffset += radius + 5;
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0x00, 0x00, 0xff));
	EVE_CoCmd_clock(s_pHalContext, xOffset, yOffset, radius, 0, 10, 10, 5, 10);

	xOffset += 2 * radius + 10;
	/* Lena clock with no background and no ticks */
	EVE_Cmd_wr32(s_pHalContext, LINE_WIDTH(10 * 16));
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0xff, 0xff, 0xff));
	EVE_Cmd_wr32(s_pHalContext, BEGIN(RECTS));
	EVE_Cmd_wr32(s_pHalContext, VERTEX2F((xOffset - radius + 10) * 16, (yOffset - radius + 10) * 16));
	EVE_Cmd_wr32(s_pHalContext, VERTEX2F((xOffset + radius - 10) * 16, (yOffset + radius - 10) * 16));
	EVE_Cmd_wr32(s_pHalContext, END());
	EVE_Cmd_wr32(s_pHalContext, COLOR_A(0xff));
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0xff, 0xff, 0xff));
	EVE_Cmd_wr32(s_pHalContext, COLOR_MASK(0, 0, 0, 1));
	EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1));
	EVE_Cmd_wr32(s_pHalContext, BEGIN(RECTS));
	EVE_Cmd_wr32(s_pHalContext, VERTEX2F((xOffset - radius + 12) * 16, (yOffset - radius + 12) * 16));
	EVE_Cmd_wr32(s_pHalContext, VERTEX2F((xOffset + radius - 12) * 16, (yOffset + radius - 12) * 16));
	EVE_Cmd_wr32(s_pHalContext, END());
	EVE_Cmd_wr32(s_pHalContext, COLOR_MASK(1, 1, 1, 1));
	EVE_Cmd_wr32(s_pHalContext, BLEND_FUNC(DST_ALPHA, ONE_MINUS_DST_ALPHA));
	/* Lena bitmap - scale proportionately wrt output resolution */
	EVE_CoCmd_loadIdentity(s_pHalContext);
	EVE_CoCmd_scale(s_pHalContext, 65536 * 2 * radius / SAMAPP_Bitmap_RawData_Header[0].Width,
		65536 * 2 * radius / SAMAPP_Bitmap_RawData_Header[0].Height);
	EVE_CoCmd_setMatrix(s_pHalContext);
	EVE_Cmd_wr32(s_pHalContext, BEGIN(BITMAPS));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_SOURCE(0));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_LAYOUT(SAMAPP_Bitmap_RawData_Header[0].Format,
		SAMAPP_Bitmap_RawData_Header[0].Stride, SAMAPP_Bitmap_RawData_Header[0].Height));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_SIZE(BILINEAR, BORDER, BORDER,
		2 * radius, 2 * radius));
	EVE_Cmd_wr32(s_pHalContext, VERTEX2F((xOffset - radius) * 16, (yOffset - radius) * 16));
	EVE_Cmd_wr32(s_pHalContext, END());
	EVE_Cmd_wr32(s_pHalContext, BLEND_FUNC(SRC_ALPHA, ONE_MINUS_SRC_ALPHA));
	EVE_CoCmd_loadIdentity(s_pHalContext);
	EVE_CoCmd_setMatrix(s_pHalContext);
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0xff, 0xff, 0xff));
	EVE_CoCmd_clock(s_pHalContext, xOffset, yOffset, radius, OPT_NOTICKS | OPT_NOBACK, 10, 10, 5, 10);
	EVE_Cmd_wr32(s_pHalContext, DISPLAY());
	EVE_CoCmd_swap(s_pHalContext);

	/* Wait till coprocessor completes the operation */
	EVE_Cmd_waitFlush(s_pHalContext);
	SAMAPP_ENABLE_DELAY();
}

/**
 * @brief demonstrate guage widget
 *
 */
void SAMAPP_CoPro_Widget_Guage()
{
	/*************************************************************************/
	/* Below code demonstrates the usage of gauge function. Gauge can be     */
	/* constructed using flat or 3d effect. Gauge background and foreground  */
	/* colors can be set by gbcolor and colorrgb. Gauge can be constructed   */
	/* with multiple options such as no background, no minors/majors and     */
	/* no pointer.                                                           */
	/*************************************************************************/
	int16_t xOffset;
	int16_t yOffset;
	int16_t cRadius;
	int16_t xDistBtwClocks;

	xDistBtwClocks = (int16_t)(s_pHalContext->Width / 5);
	cRadius = xDistBtwClocks / 2 - (int16_t)(s_pHalContext->Width / 64);
	yOffset = cRadius + 5;

	/* Download the bitmap data */
	Ftf_Write_File_nBytes_To_RAM_G(s_pHalContext, TEST_DIR "\\SAMAPP_Bitmap_RawData.bin", RAM_G,
		SAMAPP_Bitmap_RawData_Header[0].Stride * SAMAPP_Bitmap_RawData_Header[0].Height,
		SAMAPP_Bitmap_RawData_Header[0].Arrayoffset);

	/* Draw gauge with blue as background and read as needle color */
	EVE_CoCmd_dlStart(s_pHalContext);
	EVE_Cmd_wr32(s_pHalContext, CLEAR_COLOR_RGB(64, 64, 64));
	EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1));
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 255, 255));

	/* flat effect and default color background */
	xOffset = xDistBtwClocks / 2;
	EVE_CoCmd_bgColor(s_pHalContext, 0x0000ff);
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0xff, 0x00, 0x00));
	EVE_CoCmd_gauge(s_pHalContext, xOffset, yOffset, cRadius, OPT_FLAT, 5, 4, 45, 100);
	EVE_CoCmd_text(s_pHalContext, xOffset, (yOffset + cRadius + 6), 26, OPT_CENTER, "Flat effect");//text info

	/* 3d effect */
	EVE_CoCmd_bgColor(s_pHalContext, 0x00ff00);
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0xff, 0x00, 0x00));
	EVE_CoCmd_fgColor(s_pHalContext, 0xff0000);
	xOffset += xDistBtwClocks;
	EVE_CoCmd_gauge(s_pHalContext, xOffset, yOffset, cRadius, 0, 5, 1, 60, 100);
	EVE_CoCmd_text(s_pHalContext, xOffset, (yOffset + cRadius + 6), 26, OPT_CENTER, "3d effect");//text info

	/* no background color */
	EVE_CoCmd_bgColor(s_pHalContext, 0x00ffff);
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0xff, 0xff, 0x00));
	xOffset += xDistBtwClocks;
	EVE_CoCmd_gauge(s_pHalContext, xOffset, yOffset, cRadius, OPT_NOBACK, 1, 6, 90, 100);
	EVE_Cmd_wr32(s_pHalContext, COLOR_A(255));
	EVE_CoCmd_text(s_pHalContext, xOffset, (yOffset + cRadius + 6), 26, OPT_CENTER, "No BG");//text info

	/* No ticks */
	EVE_CoCmd_bgColor(s_pHalContext, 0xff00ff);
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0x00, 0xff, 0xff));
	xOffset += xDistBtwClocks;
	EVE_CoCmd_gauge(s_pHalContext, xOffset, yOffset, cRadius, OPT_NOTICKS, 5, 4, 20, 100);
	EVE_CoCmd_text(s_pHalContext, xOffset, (yOffset + cRadius + 6), 26, OPT_CENTER, "No Ticks");//text info

	/* No hands */
	EVE_CoCmd_bgColor(s_pHalContext, 0x808080);
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0x00, 0xff, 0x00));
	xOffset += xDistBtwClocks;
	EVE_CoCmd_gauge(s_pHalContext, xOffset, yOffset, cRadius, OPT_NOTICKS, 5, 4, 55, 100);
	EVE_Cmd_wr32(s_pHalContext, COLOR_A(255));
	EVE_CoCmd_text(s_pHalContext, xOffset, (yOffset + cRadius + 6), 26, OPT_CENTER, "No Hands");//text info

	/* Bigger gauge */
	yOffset += cRadius + 10;
	cRadius = (int16_t)(s_pHalContext->Height - (2 * cRadius + 5 + 10));//calculate radius based on remaining height
	cRadius = (cRadius - 5 - 20) / 2;
	xOffset = cRadius + 10;
	yOffset += cRadius + 5;
	EVE_CoCmd_bgColor(s_pHalContext, 0x808000);
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0xff, 0xff, 0xff));
	EVE_CoCmd_gauge(s_pHalContext, xOffset, yOffset, cRadius, OPT_NOBACK, 5, 4, 80, 100);
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0xff, 0x00, 0x00));
	EVE_CoCmd_gauge(s_pHalContext, xOffset, yOffset, cRadius, OPT_NOTICKS | OPT_NOBACK, 5, 4, 30, 100);

	xOffset += 2 * cRadius + 10;
	/* Lena guage with no background and no ticks */
	EVE_Cmd_wr32(s_pHalContext, POINT_SIZE(cRadius * 16));
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0xff, 0xff, 0xff));
	EVE_Cmd_wr32(s_pHalContext, BEGIN(FTPOINTS));
	EVE_Cmd_wr32(s_pHalContext, VERTEX2F(xOffset * 16, yOffset * 16));
	EVE_Cmd_wr32(s_pHalContext, END());
	EVE_Cmd_wr32(s_pHalContext, COLOR_A(0xff));
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0xff, 0xff, 0xff));
	EVE_Cmd_wr32(s_pHalContext, COLOR_MASK(0, 0, 0, 1));
	EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1));
	EVE_Cmd_wr32(s_pHalContext, BEGIN(FTPOINTS));
	EVE_Cmd_wr32(s_pHalContext, POINT_SIZE((cRadius - 2) * 16));
	EVE_Cmd_wr32(s_pHalContext, VERTEX2F(xOffset * 16, yOffset * 16));
	EVE_Cmd_wr32(s_pHalContext, END());
	EVE_Cmd_wr32(s_pHalContext, COLOR_MASK(1, 1, 1, 1));
	EVE_Cmd_wr32(s_pHalContext, BLEND_FUNC(DST_ALPHA, ONE_MINUS_DST_ALPHA));
	/* Lena bitmap - scale proportionately wrt output resolution */
	EVE_CoCmd_loadIdentity(s_pHalContext);
	EVE_CoCmd_scale(s_pHalContext, 65536 * 2 * cRadius / SAMAPP_Bitmap_RawData_Header[0].Width, 65536 * 2 * cRadius / SAMAPP_Bitmap_RawData_Header[0].Height);
	EVE_CoCmd_setMatrix(s_pHalContext);
	EVE_Cmd_wr32(s_pHalContext, BEGIN(BITMAPS));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_SOURCE(0));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_LAYOUT(SAMAPP_Bitmap_RawData_Header[0].Format,
		SAMAPP_Bitmap_RawData_Header[0].Stride, SAMAPP_Bitmap_RawData_Header[0].Height));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_SIZE(BILINEAR, BORDER, BORDER,
		2 * cRadius, 2 * cRadius));
	EVE_Cmd_wr32(s_pHalContext, VERTEX2F((xOffset - cRadius) * 16, (yOffset - cRadius) * 16));
	EVE_Cmd_wr32(s_pHalContext, END());
	EVE_Cmd_wr32(s_pHalContext, BLEND_FUNC(SRC_ALPHA, ONE_MINUS_SRC_ALPHA));
	EVE_CoCmd_loadIdentity(s_pHalContext);
	EVE_CoCmd_setMatrix(s_pHalContext);

	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0xff, 0xff, 0xff));
	EVE_CoCmd_gauge(s_pHalContext, xOffset, yOffset, cRadius, OPT_NOTICKS | OPT_NOBACK, 5, 4, 30, 100);

	EVE_Cmd_wr32(s_pHalContext, DISPLAY());
	EVE_CoCmd_swap(s_pHalContext);

	/* Wait till coprocessor completes the operation */
	EVE_Cmd_waitFlush(s_pHalContext);
	SAMAPP_ENABLE_DELAY();
}

/**
 * @brief demonstrate gradient widget
 *
 */
void SAMAPP_CoPro_Widget_Gradient()
{
	/*************************************************************************/
	/* Below code demonstrates the usage of gradient function. Gradient func */
	/* can be used to construct three effects - horizontal, vertical and     */
	/* diagonal effects.                                                      */
	/*************************************************************************/
	int16_t wScissor;
	int16_t hScissor;
	int16_t xOffset;
	int16_t yOffset;

	xOffset = 10;
	yOffset = 20;
	hScissor = (int16_t)((s_pHalContext->Height - 3 * 20) / 2);
	wScissor = (int16_t)((s_pHalContext->Width - 4 * 10) / 3);

	/* Draw gradient  */
	EVE_CoCmd_dlStart(s_pHalContext);
	EVE_Cmd_wr32(s_pHalContext, CLEAR_COLOR_RGB(64, 64, 64));
	EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1));
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 255, 255));
	EVE_Cmd_wr32(s_pHalContext, SCISSOR_SIZE(wScissor, hScissor));
	/* Horizontal gradient effect */
	EVE_Cmd_wr32(s_pHalContext, SCISSOR_XY(xOffset, yOffset));//clip the display
	EVE_CoCmd_gradient(s_pHalContext, xOffset, yOffset, 0x808080, (xOffset + wScissor), yOffset, 0xffff00);
	/* Vertical gradient effect */
	xOffset += wScissor + 10;
	EVE_Cmd_wr32(s_pHalContext, SCISSOR_XY(xOffset, yOffset));//clip the display
	EVE_CoCmd_gradient(s_pHalContext, xOffset, yOffset, 0xff0000, xOffset, (yOffset + hScissor), 0x00ff00);
	/* diagonal gradient effect */
	xOffset += wScissor + 10;
	EVE_Cmd_wr32(s_pHalContext, SCISSOR_XY(xOffset, yOffset));//clip the display
	EVE_CoCmd_gradient(s_pHalContext, xOffset, yOffset, 0x800000, (xOffset + wScissor), (yOffset + hScissor), 0xffffff);
	/* Diagonal gradient with text info */
	xOffset = 10;
	yOffset += hScissor + 20;
	EVE_Cmd_wr32(s_pHalContext, SCISSOR_SIZE(wScissor, 30));
	EVE_Cmd_wr32(s_pHalContext, SCISSOR_XY(xOffset, (yOffset + hScissor / 2 - 15)));//clip the display
	EVE_CoCmd_gradient(s_pHalContext, xOffset, (yOffset + hScissor / 2 - 15), 0x606060, (xOffset + wScissor), (yOffset + hScissor / 2 + 15), 0x404080);
	EVE_CoCmd_text(s_pHalContext, (xOffset + wScissor / 2), (yOffset + hScissor / 2), 28, OPT_CENTER, "Heading 1");//text info

	/* Draw horizontal, vertical and diagonal with alpha */
	xOffset += wScissor + 10;
	EVE_Cmd_wr32(s_pHalContext, SCISSOR_SIZE(wScissor, hScissor));
	EVE_Cmd_wr32(s_pHalContext, SCISSOR_XY(xOffset, yOffset));//clip the display
	EVE_CoCmd_gradient(s_pHalContext, xOffset, yOffset, 0x808080, (xOffset + wScissor), yOffset, 0xffff00);
	wScissor -= 30; hScissor -= 30;
	EVE_Cmd_wr32(s_pHalContext, SCISSOR_SIZE(wScissor, hScissor));
	xOffset += 15; yOffset += 15;
	EVE_Cmd_wr32(s_pHalContext, SCISSOR_XY(xOffset, yOffset));//clip the display
	EVE_CoCmd_gradient(s_pHalContext, xOffset, yOffset, 0x800000, xOffset, (yOffset + hScissor), 0xffffff);
	wScissor -= 30; hScissor -= 30;
	EVE_Cmd_wr32(s_pHalContext, SCISSOR_SIZE(wScissor, hScissor));
	xOffset += 15; yOffset += 15;
	EVE_Cmd_wr32(s_pHalContext, SCISSOR_XY(xOffset, yOffset));//clip the display
	EVE_CoCmd_gradient(s_pHalContext, xOffset, yOffset, 0x606060, (xOffset + wScissor), (yOffset + hScissor), 0x404080);

	/* Display the text wrt gradient */
	wScissor = (int16_t)((s_pHalContext->Width - 4 * 10) / 3);
	hScissor = (int16_t)((s_pHalContext->Height - 3 * 20) / 2);
	xOffset = 10 + wScissor / 2;
	yOffset = 20 + hScissor + 5;
	EVE_Cmd_wr32(s_pHalContext, SCISSOR_XY(0, 0));//set to default values
#ifndef FT81X_ENABLE
	EVE_Cmd_wr32(s_pHalContext, SCISSOR_SIZE(512, 512));
#else
	EVE_Cmd_wr32(s_pHalContext, SCISSOR_SIZE(2048, 2048));
#endif
	EVE_CoCmd_text(s_pHalContext, xOffset, yOffset, 26, OPT_CENTER, "Horizontal grad");//text info
	xOffset += wScissor + 10;
	EVE_CoCmd_text(s_pHalContext, xOffset, yOffset, 26, OPT_CENTER, "Vertical grad");//text info
	xOffset += wScissor + 10;
	EVE_CoCmd_text(s_pHalContext, xOffset, yOffset, 26, OPT_CENTER, "Diagonal grad");//text info

	EVE_Cmd_wr32(s_pHalContext, DISPLAY());
	EVE_CoCmd_swap(s_pHalContext);

	/* Wait till coprocessor completes the operation */
	EVE_Cmd_waitFlush(s_pHalContext);
	SAMAPP_ENABLE_DELAY();
}

/**
 * @brief Key interactive
 *
 */
void SAMAPP_CoPro_Widget_Keys_Interactive()
{
	/*************************************************************************/
	/* Below code demonstrates the usage of keys function. keys function     */
	/* draws buttons with characters given as input parameters. Keys support */
	/* Flat and 3D effects, draw at (x,y) coordinates or center of the display*/
	/* , inbuilt or custom fonts can be used for key display                 */
	/*************************************************************************/
	int32_t loopflag = 600;
	int16_t TextFont = 29;
	int16_t  ButtonW = 30;
	int16_t  ButtonH = 30;
	int16_t  yBtnDst = 5;
	int16_t  yOffset;
#define SAMAPP_COPRO_WIDGET_KEYS_INTERACTIVE_TEXTSIZE (512)
	char8_t DispText[SAMAPP_COPRO_WIDGET_KEYS_INTERACTIVE_TEXTSIZE];
	char8_t CurrChar = '|';
	uint8_t CurrTag = 0;
	uint8_t PrevTag = 0;
	uint8_t  Pendown = 1;
	int32_t CurrTextIdx = 0;
#ifdef SAMAPP_DISPLAY_QVGA
	TextFont = 27;
	ButtonW = 22;
	ButtonH = 22;
	yBtnDst = 3;
#endif

#ifdef 	SAMAPP_DISPLAY_WVGA
	TextFont = 30;
	ButtonW = 38;
	ButtonH = 38;
	yBtnDst = 15;
#endif

	while (loopflag--)
	{
		/* Check the user input and then add the characters into array */
		CurrTag = EVE_Hal_rd8(s_pHalContext, REG_TOUCH_TAG);

		CurrChar = CurrTag;
		if (0 == CurrTag)
		{
			CurrChar = '|';
		}

		/* check whether pwndown has happened */
		if (!CurrTag && PrevTag && 1 == Pendown && 0 != PrevTag)
		{
			CurrTextIdx++;
			/* clear all the charaters after 100 are pressed */
			if (CurrTextIdx > 24)
			{
				CurrTextIdx = 0;
			}
		}

		EVE_CoCmd_dlStart(s_pHalContext);
		EVE_Cmd_wr32(s_pHalContext, CLEAR_COLOR_RGB(64, 64, 64));
		EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1));
		EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0xff, 0xff, 0xff));
		/* Draw text entered by user */
		/* make sure the array is a string */
		DispText[CurrTextIdx] = CurrChar;
		DispText[CurrTextIdx + 1] = '\0';

		EVE_Cmd_wr32(s_pHalContext, TAG_MASK(0));
		EVE_CoCmd_text(s_pHalContext, (int16_t)(s_pHalContext->Width / 2), 40, TextFont, OPT_CENTER, DispText);//text info
		EVE_Cmd_wr32(s_pHalContext, TAG_MASK(1));

		yOffset = ButtonW * 2 + 10;
		/* Construct a simple keyboard - note that the tags associated with the keys are the character values given in the arguments */
		EVE_CoCmd_fgColor(s_pHalContext, 0x404080);
		EVE_CoCmd_gradColor(s_pHalContext, 0x00ff00);
		EVE_CoCmd_keys(s_pHalContext, yBtnDst, yOffset, 10 * ButtonW, ButtonH, TextFont, (OPT_CENTER | CurrTag), "qwertyuiop");
		EVE_CoCmd_gradColor(s_pHalContext, 0x00ffff);
		yOffset += ButtonH + yBtnDst;
		EVE_CoCmd_keys(s_pHalContext, yBtnDst, yOffset, 10 * ButtonW, ButtonH, TextFont, (OPT_CENTER | CurrTag), "asdfghjkl");
		EVE_CoCmd_gradColor(s_pHalContext, 0xffff00);
		yOffset += ButtonH + yBtnDst;
		EVE_CoCmd_keys(s_pHalContext, yBtnDst, yOffset, 10 * ButtonW, ButtonH, TextFont, (OPT_CENTER | CurrTag), "zxcvbnm");//hilight button z
		yOffset += ButtonH + yBtnDst;
		EVE_Cmd_wr32(s_pHalContext, TAG(' '));
		if (' ' == CurrTag)
		{
			EVE_CoCmd_button(s_pHalContext, yBtnDst, yOffset, 10 * ButtonW, ButtonH, TextFont, OPT_FLAT, " ");//mandatory to give '\0' at the end to make sure coprocessor understands the string end
		}
		else
		{
			EVE_CoCmd_button(s_pHalContext, yBtnDst, yOffset, 10 * ButtonW, ButtonH, TextFont, 0, " ");//mandatory to give '\0' at the end to make sure coprocessor understands the string end
		}
		yOffset = ButtonW * 2 + 10;
		EVE_CoCmd_keys(s_pHalContext, 11 * ButtonW, yOffset, 3 * ButtonW, ButtonH, TextFont, (0 | CurrTag), "789");
		yOffset += ButtonH + yBtnDst;
		EVE_CoCmd_keys(s_pHalContext, 11 * ButtonW, yOffset, 3 * ButtonW, ButtonH, TextFont, (0 | CurrTag), "456");
		yOffset += ButtonH + yBtnDst;
		EVE_CoCmd_keys(s_pHalContext, 11 * ButtonW, yOffset, 3 * ButtonW, ButtonH, TextFont, (0 | CurrTag), "123");
		yOffset += ButtonH + yBtnDst;
		EVE_Cmd_wr32(s_pHalContext, COLOR_A(255));
		EVE_CoCmd_keys(s_pHalContext, 11 * ButtonW, yOffset, 3 * ButtonW, ButtonH, TextFont, (0 | CurrTag), "0.");//hilight button 0
		EVE_Cmd_wr32(s_pHalContext, DISPLAY());
		EVE_CoCmd_swap(s_pHalContext);

		/* Wait till coprocessor completes the operation */
		EVE_Cmd_waitFlush(s_pHalContext);
		EVE_sleep(10);
		PrevTag = CurrTag;
	}
}

/**
 * @brief  keys widget
 *
 */
void SAMAPP_CoPro_Widget_Keys()
{
	/*************************************************************************/
	/* Below code demonstrates the usage of keys function. keys function     */
	/* draws buttons with characters given as input parameters. Keys support */
	/* Flat and 3D effects, draw at (x,y) coordinates or center of the display*/
	/* , inbuilt or custom fonts can be used for key display                 */
	/*************************************************************************/
	int16_t TextFont = 29;
	int16_t ButtonW = 30;
	int16_t ButtonH = 30;
	int16_t yBtnDst = 5;
	int16_t yOffset;
	int16_t xOffset;
#ifdef SAMAPP_DISPLAY_QVGA
	TextFont = 27;
	ButtonW = 22;
	ButtonH = 22;
	yBtnDst = 3;
#endif

#ifdef 	SAMAPP_DISPLAY_WVGA
	TextFont = 30;
	ButtonW = 38;
	ButtonH = 38;
	yBtnDst = 15;
#endif
	EVE_CoCmd_dlStart(s_pHalContext);
	EVE_Cmd_wr32(s_pHalContext, CLEAR_COLOR_RGB(64, 64, 64));
	EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1));
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0xff, 0xff, 0xff));
	EVE_Cmd_wr32(s_pHalContext, LINE_WIDTH(1 * 16));
	/* Draw keys with flat effect */
	yOffset = ButtonW + 10 + yBtnDst;
	EVE_CoCmd_fgColor(s_pHalContext, 0x404080);
	EVE_CoCmd_keys(s_pHalContext, 10, 10, 4 * ButtonW, 30, TextFont, OPT_FLAT, "ABCD");
	EVE_CoCmd_text(s_pHalContext, 10, yOffset, 26, 0, "Flat effect");//text info
	/* Draw keys with 3d effect */
	EVE_CoCmd_fgColor(s_pHalContext, 0x800000);
	xOffset = 4 * ButtonW + 20;
	EVE_CoCmd_keys(s_pHalContext, xOffset, 10, 4 * ButtonW, 30, TextFont, 0, "ABCD");
	EVE_CoCmd_text(s_pHalContext, xOffset, yOffset, 26, 0, "3D effect");//text info
	/* Draw keys with center option */
	EVE_CoCmd_fgColor(s_pHalContext, 0xffff000);
	xOffset += 4 * ButtonW + 20;
	EVE_CoCmd_keys(s_pHalContext, xOffset, 10, (int16_t)(s_pHalContext->Width - 230), 30, TextFont, OPT_CENTER, "ABCD");
	EVE_CoCmd_text(s_pHalContext, xOffset, yOffset, 26, 0, "Option Center");//text info

	yOffset = ButtonW * 2 + 10;
	/* Construct a simple keyboard - note that the tags associated with the keys are the character values given in the arguments */
	EVE_CoCmd_fgColor(s_pHalContext, 0x404080);
	EVE_CoCmd_gradColor(s_pHalContext, 0x00ff00);
	EVE_CoCmd_keys(s_pHalContext, yBtnDst, yOffset, 10 * ButtonW, ButtonH, TextFont, OPT_CENTER, "qwertyuiop");
	EVE_CoCmd_gradColor(s_pHalContext, 0x00ffff);
	yOffset += ButtonH + yBtnDst;
	EVE_CoCmd_keys(s_pHalContext, yBtnDst, yOffset, 10 * ButtonW, ButtonH, TextFont, OPT_CENTER, "asdfghjkl");
	EVE_CoCmd_gradColor(s_pHalContext, 0xffff00);
	yOffset += ButtonH + yBtnDst;
	EVE_CoCmd_keys(s_pHalContext, yBtnDst, yOffset, 10 * ButtonW, ButtonH, TextFont, (OPT_CENTER | 'z'), "zxcvbnm");//hilight button z
	yOffset += ButtonH + yBtnDst;
	EVE_CoCmd_button(s_pHalContext, yBtnDst, yOffset, 10 * ButtonW, ButtonH, TextFont, 0, " ");//mandatory to give '\0' at the end to make sure coprocessor understands the string end
	yOffset = 80 + 10;
	EVE_CoCmd_keys(s_pHalContext, 11 * ButtonW, yOffset, 3 * ButtonW, ButtonH, TextFont, 0, "789");
	yOffset += ButtonH + yBtnDst;
	EVE_CoCmd_keys(s_pHalContext, 11 * ButtonW, yOffset, 3 * ButtonW, ButtonH, TextFont, 0, "456");
	yOffset += ButtonH + yBtnDst;
	EVE_CoCmd_keys(s_pHalContext, 11 * ButtonW, yOffset, 3 * ButtonW, ButtonH, TextFont, 0, "123");
	yOffset += ButtonH + yBtnDst;
	EVE_Cmd_wr32(s_pHalContext, COLOR_A(255));
	EVE_CoCmd_keys(s_pHalContext, 11 * ButtonW, yOffset, 3 * ButtonW, ButtonH, TextFont, (0 | '0'), "0.");//hilight button 0
	EVE_Cmd_wr32(s_pHalContext, DISPLAY());
	EVE_CoCmd_swap(s_pHalContext);

	/* Wait till coprocessor completes the operation */
	EVE_Cmd_waitFlush(s_pHalContext);
	SAMAPP_ENABLE_DELAY();
}

/**
 * @brief demonstrate progress bar widget
 *
 */
void SAMAPP_CoPro_Widget_Progressbar()
{
	/*************************************************************************/
	/* Below code demonstrates the usage of progress function. Progress func */
	/* draws process bar with fgcolor for the % completion and bgcolor for   */
	/* % remaining. Progress bar supports flat and 3d effets                 */
	/*************************************************************************/
	int16_t xOffset;
	int16_t yOffset;
	int16_t yDist = (int16_t)(s_pHalContext->Width / 12);
	int16_t ySz = (int16_t)(s_pHalContext->Width / 24);

	EVE_CoCmd_dlStart(s_pHalContext);
	EVE_Cmd_wr32(s_pHalContext, CLEAR_COLOR_RGB(64, 64, 64));
	EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1));
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0xff, 0xff, 0xff));
	/* Draw progress bar with flat effect */
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0xff, 0xff, 0xff));
	EVE_CoCmd_bgColor(s_pHalContext, 0x404080);
	EVE_CoCmd_progress(s_pHalContext, 20, 10, 120, 20, OPT_FLAT, 50, 100);//note that h/2 will be added on both sides of the progress bar
	EVE_Cmd_wr32(s_pHalContext, COLOR_A(255));
	EVE_CoCmd_text(s_pHalContext, 20, 40, 26, 0, "Flat effect");//text info
														/* Draw progress bar with 3d effect */
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0x00, 0xff, 0x00));
	EVE_CoCmd_bgColor(s_pHalContext, 0x800000);
	EVE_CoCmd_progress(s_pHalContext, 180, 10, 120, 20, 0, 75, 100);
	EVE_Cmd_wr32(s_pHalContext, COLOR_A(255));
	EVE_CoCmd_text(s_pHalContext, 180, 40, 26, 0, "3D effect");//text info
													   /* Draw progress bar with 3d effect and string on top */
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0xff, 0x00, 0x00));
	EVE_CoCmd_bgColor(s_pHalContext, 0x000080);
	EVE_CoCmd_progress(s_pHalContext, 30, 60, 120, 30, 0, 19660, 65535);
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0xff, 0xff, 0xff));
	EVE_CoCmd_text(s_pHalContext, 78, 68, 26, 0, "30 %");//text info

	xOffset = 20; yOffset = 120;
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0x00, 0xa0, 0x00));
	EVE_CoCmd_bgColor(s_pHalContext, 0x800000);
	EVE_CoCmd_progress(s_pHalContext, xOffset, yOffset, 150, ySz, 0, 10, 100);
	EVE_CoCmd_bgColor(s_pHalContext, 0x000080);
	yOffset += yDist;
	EVE_CoCmd_progress(s_pHalContext, xOffset, yOffset, 150, ySz, 0, 40, 100);
	EVE_CoCmd_bgColor(s_pHalContext, 0xffff00);
	yOffset += yDist;
	EVE_CoCmd_progress(s_pHalContext, xOffset, yOffset, 150, ySz, 0, 70, 100);
	EVE_CoCmd_bgColor(s_pHalContext, 0x808080);
	yOffset += yDist;
	EVE_CoCmd_progress(s_pHalContext, xOffset, yOffset, 150, ySz, 0, 90, 100);

	EVE_CoCmd_text(s_pHalContext, xOffset + 180, 80, 26, 0, "40 % TopBottom");//text info
	EVE_CoCmd_progress(s_pHalContext, xOffset + 180, 100, ySz, 150, 0, 40, 100);

	EVE_Cmd_wr32(s_pHalContext, DISPLAY());
	EVE_CoCmd_swap(s_pHalContext);

	/* Wait till coprocessor completes the operation */
	EVE_Cmd_waitFlush(s_pHalContext);
	SAMAPP_ENABLE_DELAY();
}

/**
 * @brief demonstrate scroll widget
 *
 */
void SAMAPP_CoPro_Widget_Scroll()
{
	/*************************************************************************/
	/* Below code demonstrates the usage of scroll function. Scroll function */
	/* draws scroll bar with fgcolor for inner color and current location and*/
	/* can be given by val parameter */
	/*************************************************************************/
	int16_t xOffset;
	int16_t yOffset;
	int16_t xDist = (int16_t)(s_pHalContext->Width / 12);
	int16_t yDist = (int16_t)(s_pHalContext->Width / 12);
	int16_t wSz;

	EVE_CoCmd_dlStart(s_pHalContext);
	EVE_Cmd_wr32(s_pHalContext, CLEAR_COLOR_RGB(64, 64, 64));
	EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1));
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0xff, 0xff, 0xff));
	/* Draw scroll bar with flat effect */
	EVE_CoCmd_fgColor(s_pHalContext, 0xffff00);
	EVE_CoCmd_bgColor(s_pHalContext, 0x404080);
	EVE_CoCmd_scrollbar(s_pHalContext, 20, 10, 120, 8, OPT_FLAT, 20, 30, 100);//note that h/2 size will be added on both sides of the progress bar
	EVE_Cmd_wr32(s_pHalContext, COLOR_A(255));
	EVE_CoCmd_text(s_pHalContext, 20, 40, 26, 0, "Flat effect");//text info
	/* Draw scroll bar with 3d effect */
	EVE_CoCmd_fgColor(s_pHalContext, 0x00ff00);
	EVE_CoCmd_bgColor(s_pHalContext, 0x800000);
	EVE_CoCmd_scrollbar(s_pHalContext, 180, 10, 120, 8, 0, 20, 30, 100);
	EVE_Cmd_wr32(s_pHalContext, COLOR_A(255));
	EVE_CoCmd_text(s_pHalContext, 180, 40, 26, 0, "3D effect");//text info

	xOffset = 20;
	yOffset = 120;
	wSz = (int16_t)((s_pHalContext->Width / 2) - 40);
	/* Draw horizontal scroll bars */
	EVE_CoCmd_fgColor(s_pHalContext, 0x00a000);
	EVE_CoCmd_bgColor(s_pHalContext, 0x800000);
	EVE_CoCmd_scrollbar(s_pHalContext, xOffset, yOffset, wSz, 8, 0, 10, 30, 100);
	EVE_CoCmd_bgColor(s_pHalContext, 0x000080);
	yOffset += yDist;
	EVE_CoCmd_scrollbar(s_pHalContext, xOffset, yOffset, wSz, 8, 0, 30, 30, 100);
	EVE_Cmd_wr32(s_pHalContext, COLOR_A(255));
	EVE_CoCmd_bgColor(s_pHalContext, 0xffff00);
	yOffset += yDist;
	EVE_CoCmd_scrollbar(s_pHalContext, xOffset, yOffset, wSz, 8, 0, 50, 30, 100);
	EVE_CoCmd_bgColor(s_pHalContext, 0x808080);
	yOffset += yDist;
	EVE_CoCmd_scrollbar(s_pHalContext, xOffset, yOffset, wSz, 8, 0, 70, 30, 100);

	xOffset = (int16_t)(s_pHalContext->Width / 2 + 40);
	yOffset = 80;
	wSz = (int16_t)(s_pHalContext->Height - 100);
	/* draw vertical scroll bars */
	EVE_CoCmd_bgColor(s_pHalContext, 0x800000);
	EVE_CoCmd_scrollbar(s_pHalContext, xOffset, yOffset, 8, wSz, 0, 10, 30, 100);
	EVE_CoCmd_bgColor(s_pHalContext, 0x000080);
	xOffset += xDist;
	EVE_CoCmd_scrollbar(s_pHalContext, xOffset, yOffset, 8, wSz, 0, 30, 30, 100);
	EVE_Cmd_wr32(s_pHalContext, COLOR_A(255));
	EVE_CoCmd_bgColor(s_pHalContext, 0xffff00);
	xOffset += xDist;
	EVE_CoCmd_scrollbar(s_pHalContext, xOffset, yOffset, 8, wSz, 0, 50, 30, 100);
	EVE_CoCmd_bgColor(s_pHalContext, 0x808080);
	xOffset += xDist;
	EVE_CoCmd_scrollbar(s_pHalContext, xOffset, yOffset, 8, wSz, 0, 70, 30, 100);

	EVE_Cmd_wr32(s_pHalContext, DISPLAY());
	EVE_CoCmd_swap(s_pHalContext);

	/* Wait till coprocessor completes the operation */
	EVE_Cmd_waitFlush(s_pHalContext);
	SAMAPP_ENABLE_DELAY();
}

/**
 * @brief demonstrate slider widget
 *
 */
void SAMAPP_CoPro_Widget_Slider()
{
	/*************************************************************************/
	/* Below code demonstrates the usage of slider function. Slider function */
	/* draws slider bar with fgcolor for inner color and bgcolor for the knob*/
	/* , contains input parameter for position of the knob                   */
	/*************************************************************************/
	int16_t xOffset;
	int16_t yOffset;
	int16_t xDist = (int16_t)(s_pHalContext->Width / 12);
	int16_t yDist = (int16_t)(s_pHalContext->Width / 12);
	int16_t wSz;

	EVE_CoCmd_dlStart(s_pHalContext);
	EVE_Cmd_wr32(s_pHalContext, CLEAR_COLOR_RGB(64, 64, 64));
	EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1));
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0xff, 0xff, 0xff));
	/* Draw scroll bar with flat effect */
	EVE_CoCmd_fgColor(s_pHalContext, 0xffff00);
	EVE_CoCmd_bgColor(s_pHalContext, 0x000080);
	EVE_CoCmd_slider(s_pHalContext, 20, 10, 120, 10, OPT_FLAT, 30, 100);//note that h/2 size will be added on both sides of the progress bar
	EVE_Cmd_wr32(s_pHalContext, COLOR_A(255));
	EVE_CoCmd_text(s_pHalContext, 20, 40, 26, 0, "Flat effect");//text info
	/* Draw scroll bar with 3d effect */
	EVE_CoCmd_fgColor(s_pHalContext, 0x00ff00);
	EVE_CoCmd_bgColor(s_pHalContext, 0x800000);
	EVE_CoCmd_slider(s_pHalContext, 180, 10, 120, 10, 0, 50, 100);
	EVE_Cmd_wr32(s_pHalContext, COLOR_A(255));
	EVE_CoCmd_text(s_pHalContext, 180, 40, 26, 0, "3D effect");//text info

	xOffset = 20;
	yOffset = 120;
	wSz = (int16_t)((s_pHalContext->Width / 2) - 40);
	/* Draw horizontal slider bars */
	EVE_CoCmd_fgColor(s_pHalContext, 0x00a000);
	EVE_CoCmd_bgColor(s_pHalContext, 0x800000);
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(41, 1, 5));
	EVE_CoCmd_slider(s_pHalContext, xOffset, yOffset, wSz, 10, 0, 10, 100);
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(11, 7, 65));
	EVE_CoCmd_bgColor(s_pHalContext, 0x000080);
	yOffset += yDist;
	EVE_CoCmd_slider(s_pHalContext, xOffset, yOffset, wSz, 10, 0, 30, 100);
	EVE_Cmd_wr32(s_pHalContext, COLOR_A(255));
	EVE_CoCmd_bgColor(s_pHalContext, 0xffff00);
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(87, 94, 9));
	yOffset += yDist;
	EVE_CoCmd_slider(s_pHalContext, xOffset, yOffset, wSz, 10, 0, 50, 100);
	EVE_CoCmd_bgColor(s_pHalContext, 0x808080);
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(51, 50, 52));
	yOffset += yDist;
	EVE_CoCmd_slider(s_pHalContext, xOffset, yOffset, wSz, 10, 0, 70, 100);

	xOffset = (int16_t)(s_pHalContext->Width / 2) + 40;
	yOffset = 80;
	wSz = (int16_t)(s_pHalContext->Height - 100);
	/* draw vertical slider bars */
	EVE_CoCmd_bgColor(s_pHalContext, 0x800000);
	EVE_CoCmd_slider(s_pHalContext, xOffset, yOffset, 10, wSz, 0, 10, 100);
	EVE_CoCmd_bgColor(s_pHalContext, 0x000080);
	xOffset += xDist;
	EVE_CoCmd_slider(s_pHalContext, xOffset, yOffset, 10, wSz, 0, 30, 100);
	EVE_Cmd_wr32(s_pHalContext, COLOR_A(255));
	EVE_CoCmd_bgColor(s_pHalContext, 0xffff00);
	xOffset += xDist;
	EVE_CoCmd_slider(s_pHalContext, xOffset, yOffset, 10, wSz, 0, 50, 100);
	EVE_CoCmd_bgColor(s_pHalContext, 0x808080);
	xOffset += xDist;
	EVE_CoCmd_slider(s_pHalContext, xOffset, yOffset, 10, wSz, 0, 70, 100);

	EVE_Cmd_wr32(s_pHalContext, DISPLAY());
	EVE_CoCmd_swap(s_pHalContext);

	/* Wait till coprocessor completes the operation */
	EVE_Cmd_waitFlush(s_pHalContext);
	SAMAPP_ENABLE_DELAY();
}

/**
 * @brief demonstrate dial widget
 *
 */
void SAMAPP_CoPro_Widget_Dial()
{
	/*************************************************************************/
	/* Below code demonstrates the usage of dial function. Dial function     */
	/* draws rounded bar with fgcolor for knob color and colorrgb for pointer*/
	/* , contains input parameter for angle of the pointer                   */
	/*************************************************************************/
	EVE_CoCmd_dlStart(s_pHalContext);
	EVE_Cmd_wr32(s_pHalContext, CLEAR_COLOR_RGB(64, 64, 64));
	EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1));
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0xff, 0xff, 0xff));
	/* Draw dial with flat effect */
	EVE_CoCmd_fgColor(s_pHalContext, 0xffff00);
	EVE_CoCmd_bgColor(s_pHalContext, 0x000080);
	EVE_CoCmd_dial(s_pHalContext, 50, 50, 40, OPT_FLAT, (uint16_t)(0.2 * 65535));//20%
	EVE_Cmd_wr32(s_pHalContext, COLOR_A(255));
	EVE_CoCmd_text(s_pHalContext, 15, 90, 26, 0, "Flat effect");//text info
	/* Draw dial with 3d effect */
	EVE_CoCmd_fgColor(s_pHalContext, 0x00ff00);
	EVE_CoCmd_bgColor(s_pHalContext, 0x800000);
	EVE_CoCmd_dial(s_pHalContext, 140, 50, 40, 0, (uint16_t)(0.45 * 65535));//45%
	EVE_Cmd_wr32(s_pHalContext, COLOR_A(255));
	EVE_CoCmd_text(s_pHalContext, 105, 90, 26, 0, "3D effect");//text info

	/* Draw increasing dials horizontally */
	EVE_CoCmd_fgColor(s_pHalContext, 0x800000);
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(41, 1, 5));
	EVE_CoCmd_dial(s_pHalContext, 30, 160, 20, 0, (uint16_t)(0.30 * 65535));
	EVE_CoCmd_text(s_pHalContext, 20, 180, 26, 0, "30 %");//text info
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(11, 7, 65));
	EVE_CoCmd_fgColor(s_pHalContext, 0x000080);
	EVE_CoCmd_dial(s_pHalContext, 100, 160, 40, 0, (uint16_t)(0.45 * 65535));
	EVE_Cmd_wr32(s_pHalContext, COLOR_A(255));
	EVE_CoCmd_text(s_pHalContext, 90, 200, 26, 0, "45 %");//text info
	EVE_CoCmd_fgColor(s_pHalContext, 0xffff00);
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(87, 94, 9));
	EVE_CoCmd_dial(s_pHalContext, 210, 160, 60, 0, (uint16_t)(0.60 * 65535));
	EVE_CoCmd_text(s_pHalContext, 200, 220, 26, 0, "60 %");//text info
	EVE_CoCmd_fgColor(s_pHalContext, 0x808080);

#ifndef SAMAPP_DISPLAY_QVGA
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(51, 50, 52));
	EVE_CoCmd_dial(s_pHalContext, 360, 160, 80, 0, (uint16_t)(0.75 * 65535));
	EVE_CoCmd_text(s_pHalContext, 350, 240, 26, 0, "75 %");//text info
#endif
	EVE_Cmd_wr32(s_pHalContext, DISPLAY());
	EVE_CoCmd_swap(s_pHalContext);

	/* Wait till coprocessor completes the operation */
	EVE_Cmd_waitFlush(s_pHalContext);
	SAMAPP_ENABLE_DELAY();
}

/**
 * @brief demonstrate toggle widget
 *
 */
void SAMAPP_CoPro_Widget_Toggle()
{
	/*************************************************************************/
	/* Below code demonstrates the usage of toggle function. Toggle function */
	/* draws line with inside knob to choose between left and right. Toggle  */
	/* inside color can be changed by bgcolor and knob color by fgcolor. Left*/
	/* right texts can be written and the color of the text can be changed by*/
	/* colorrgb graphics function                                            */
	/*************************************************************************/
	int16_t xOffset;
	int16_t yOffset;
	int16_t yDist = 40;

	EVE_CoCmd_dlStart(s_pHalContext);
	EVE_Cmd_wr32(s_pHalContext, CLEAR_COLOR_RGB(64, 64, 64));
	EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1));
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0xff, 0xff, 0xff));
	/* Draw toggle with flat effect */
	EVE_CoCmd_fgColor(s_pHalContext, 0xffff00);
	EVE_CoCmd_bgColor(s_pHalContext, 0x000080);

	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0xff, 0xff, 0xff));

	EVE_CoCmd_toggle(s_pHalContext, 40, 10, 30, 27, OPT_FLAT, (uint16_t)(0.5 * 65535), "no\xffyes");//circle radius will be extended on both the horizontal sides
	EVE_Cmd_wr32(s_pHalContext, COLOR_A(255));
	EVE_CoCmd_text(s_pHalContext, 40, 40, 26, 0, "Flat effect");//text info
	/* Draw toggle bar with 3d effect */
	EVE_CoCmd_fgColor(s_pHalContext, 0x00ff00);
	EVE_CoCmd_bgColor(s_pHalContext, 0x800000);
	EVE_CoCmd_toggle(s_pHalContext, 140, 10, 30, 27, 0, 1 * 65535, "stop\xffrun");
	EVE_Cmd_wr32(s_pHalContext, COLOR_A(255));
	EVE_CoCmd_text(s_pHalContext, 140, 40, 26, 0, "3D effect");//text info

	xOffset = 40;
	yOffset = 80;
	/* Draw horizontal toggle bars */
	EVE_CoCmd_bgColor(s_pHalContext, 0x800000);
	EVE_CoCmd_fgColor(s_pHalContext, 0x410105);
	EVE_CoCmd_toggle(s_pHalContext, xOffset, yOffset, 30, 27, 0, 0 * 65535, "-ve\xff+ve");
	EVE_CoCmd_fgColor(s_pHalContext, 0x0b0721);
	EVE_CoCmd_bgColor(s_pHalContext, 0x000080);
	yOffset += yDist;
	EVE_CoCmd_toggle(s_pHalContext, xOffset, yOffset, 30, 27, 0, (uint16_t)(0.25 * 65535), "zero\xffone");
	EVE_CoCmd_bgColor(s_pHalContext, 0xffff00);
	EVE_CoCmd_fgColor(s_pHalContext, 0x575e1b);
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0, 0, 0));
	yOffset += yDist;
	EVE_CoCmd_toggle(s_pHalContext, xOffset, yOffset, 30, 27, 0, (uint16_t)(0.5 * 65535), "exit\xffinit");
	EVE_CoCmd_bgColor(s_pHalContext, 0x808080);
	EVE_CoCmd_fgColor(s_pHalContext, 0x333234);
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0xff, 0xff, 0xff));
	yOffset += yDist;
	EVE_CoCmd_toggle(s_pHalContext, xOffset, yOffset, 30, 27, 0, (uint16_t)(0.75 * 65535), "off\xffon");

	EVE_Cmd_wr32(s_pHalContext, DISPLAY());
	EVE_CoCmd_swap(s_pHalContext);

	/* Wait till coprocessor completes the operation */
	EVE_Cmd_waitFlush(s_pHalContext);
	SAMAPP_ENABLE_DELAY();
}

/**
 * @brief demonstrate spinner widget
 *
 */
void SAMAPP_CoPro_Widget_Spinner()
{
	/*************************************************************************/
	/* Below code demonstrates the usage of spinner function. Spinner func   */
	/* will wait untill stop command is sent from the mcu. Spinner has option*/
	/* for displaying points in circle fashion or in a line fashion.         */
	/*************************************************************************/
	EVE_CoCmd_dlStart(s_pHalContext);
	EVE_Cmd_wr32(s_pHalContext, CLEAR_COLOR_RGB(64, 64, 64));
	EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1));
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0xff, 0xff, 0xff));
	EVE_CoCmd_text(s_pHalContext, (int16_t)(s_pHalContext->Width / 2), 20, 27, OPT_CENTER, "Spinner circle");
	EVE_CoCmd_text(s_pHalContext, (int16_t)(s_pHalContext->Width / 2), 80, 27, OPT_CENTER, "Please Wait ...");
	EVE_CoCmd_spinner(s_pHalContext, (int16_t)(s_pHalContext->Width / 2), (int16_t)(s_pHalContext->Height / 2), 0, 1);//style 0 and scale 0

	/* Wait till coprocessor completes the operation */
	EVE_Cmd_waitFlush(s_pHalContext);

	EVE_sleep(1000);

	/**************************** spinner with style 1 and scale 1 *****************************************************/

	EVE_CoCmd_dlStart(s_pHalContext);
	EVE_Cmd_wr32(s_pHalContext, CLEAR_COLOR_RGB(64, 64, 64));
	EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1));
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0xff, 0xff, 0xff));
	EVE_CoCmd_text(s_pHalContext, (int16_t)(s_pHalContext->Width / 2), 20, 27, OPT_CENTER, "Spinner line");
	EVE_CoCmd_text(s_pHalContext, (int16_t)(s_pHalContext->Width / 2), 80, 27, OPT_CENTER, "Please Wait ...");
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0x00, 0x00, 0x80));
	EVE_CoCmd_spinner(s_pHalContext, (int16_t)(s_pHalContext->Width / 2), (int16_t)(s_pHalContext->Height / 2), 1, 1);//style 1 and scale 1

	/* Wait till coprocessor completes the operation */
	EVE_Cmd_waitFlush(s_pHalContext);

	EVE_sleep(1000);

	EVE_CoCmd_dlStart(s_pHalContext);
	EVE_Cmd_wr32(s_pHalContext, CLEAR_COLOR_RGB(64, 64, 64));
	EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1));
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0xff, 0xff, 0xff));
	EVE_CoCmd_text(s_pHalContext, (int16_t)(s_pHalContext->Width / 2), 20, 27, OPT_CENTER, "Spinner clockhand");
	EVE_CoCmd_text(s_pHalContext, (int16_t)(s_pHalContext->Width / 2), 80, 27, OPT_CENTER, "Please Wait ...");
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0x80, 0x00, 0x00));
	EVE_CoCmd_spinner(s_pHalContext, (int16_t)(s_pHalContext->Width / 2), (int16_t)((s_pHalContext->Height / 2) + 20), 2, 1);//style 2 scale 1

	/* Wait till coprocessor completes the operation */
	EVE_Cmd_waitFlush(s_pHalContext);

	EVE_sleep(1000);

	EVE_CoCmd_dlStart(s_pHalContext);
	EVE_Cmd_wr32(s_pHalContext, CLEAR_COLOR_RGB(64, 64, 64));
	EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1));
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0xff, 0xff, 0xff));
	EVE_CoCmd_text(s_pHalContext, (int16_t)(s_pHalContext->Width / 2), 20, 27, OPT_CENTER, "Spinner two dots");
	EVE_CoCmd_text(s_pHalContext, (int16_t)(s_pHalContext->Width / 2), 80, 27, OPT_CENTER, "Please Wait ...");
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0x80, 0x00, 0x00));
	EVE_CoCmd_spinner(s_pHalContext, (int16_t)(s_pHalContext->Width / 2), (int16_t)((s_pHalContext->Height / 2) + 20), 3, 1);//style 3 scale 0

	/* Wait till coprocessor completes the operation */
	EVE_Cmd_waitFlush(s_pHalContext);

	EVE_sleep(1000);

	/* Send the stop command */
	EVE_Cmd_wr32(s_pHalContext, CMD_STOP);
	/* Update the command buffer pointers - both read and write pointers */
	EVE_Cmd_waitFlush(s_pHalContext);

	EVE_sleep(1000);
}

/**
 * @brief Power commands should be executred in SPI single channel mode
 *
 */
static void initSPIMode() {
#if defined(FT900_PLATFORM) || defined(FT93X_PLATFORM)
#ifdef FT81X_ENABLE
	/* api to set quad and numbe of dummy bytes */
#if (defined(ENABLE_SPI_QUAD) || defined(ENABLE_SPI_DUAL))
	/* Set EVE in single channel mode */
	EVE_Hal_setSPI(s_pHalContext, GPU_SPI_SINGLE_CHANNEL, GPU_SPI_ONEDUMMY);

	/* Set FT900 to single channel mode */
	spi_init(SPIM, spi_dir_master, spi_mode_0, 4);
	spi_option(SPIM, spi_option_fifo_size, 64);
	spi_option(SPIM, spi_option_fifo, 1);
	spi_option(SPIM, spi_option_fifo_receive_trigger, 1);
	spi_option(SPIM, spi_option_bus_width, 1);
#endif
#endif
#endif

#if defined (MSVC_PLATFORM_SPI) && (MSVC_PLATFORM_SPI_LIBFT4222 == 1)
	/* All host commands of FT81x (except system reset) should be executed
	* in Single channel mode
	*/
	if (s_pHalContext->spichannel == SPI_IO_QUAD || s_pHalContext->spichannel == SPI_IO_DUAL)
		EVE_Hal_setSPI(s_pHalContext, GPU_SPI_SINGLE_CHANNEL, GPU_SPI_ONEDUMMY);
#endif
}

/**
 * @brief Power mode
 *
 */
void SAMAPP_PowerMode()
{
	/*************************************************
	Senario1:  Transition from Active mode to Standby mode.
	Transition from Standby mode to Active Mode
	**************************************************/
	//Switch FT800 from Active to Standby mode
	fadeout(s_pHalContext);
	Play_MuteSound(s_pHalContext);//Play mute sound to avoid pop sound
	initSPIMode();

	EVE_Host_powerModeSwitch(s_pHalContext, GPU_STANDBY_M);
	//Wake up from Standby first before accessing FT800 registers.
	EVE_Host_powerModeSwitch(s_pHalContext, GPU_ACTIVE_M);
	Draw_Text(s_pHalContext, "Power Scenario 1");
	fadein(s_pHalContext);
	EVE_sleep(3000);

	/*************************************************
	Senario2:  Transition from Active mode to Sleep mode.
	Transition from Sleep mode to Active Mode
	**************************************************/
	//Switch FT800 from Active to Sleep mode
	fadeout(s_pHalContext);
	Play_MuteSound(s_pHalContext);//Play mute sound to avoid pop sound
	EVE_Host_powerModeSwitch(s_pHalContext, GPU_SLEEP_M);
	EVE_sleep(50);
	//Wake up from Sleep
	EVE_Host_powerModeSwitch(s_pHalContext, GPU_ACTIVE_M);
	EVE_sleep(50);
	Draw_Text(s_pHalContext, "Power Scenario 2");
	fadein(s_pHalContext);
	EVE_sleep(3000);

	/*************************************************
	Senario3:  Transition from Active mode to PowerDown mode.
	Transition from PowerDown mode to Active Mode via Standby mode.
	**************************************************/
	//Switch FT800 from Active to PowerDown mode by sending command
	fadeout(s_pHalContext);
	Play_MuteSound(s_pHalContext);//Play mute sound to avoid pop sound
	initSPIMode();

	EVE_Host_powerModeSwitch(s_pHalContext, GPU_POWERDOWN_M);
	EVE_sleep(50);
	BootupConfig(s_pHalContext);
	//Need download display list again because power down mode lost all registers and memory
	Draw_Text(s_pHalContext, "Power Scenario 3");
	fadein(s_pHalContext);
	EVE_sleep(3000);

	/*************************************************
	Senario4:  Transition from Active mode to PowerDown mode by toggling PDN from high to low.
	Transition from PowerDown mode to Active Mode via Standby mode.
	**************************************************/
	//Switch FT800 from Active to PowerDown mode by toggling PDN
	fadeout(s_pHalContext);
	Play_MuteSound(s_pHalContext);//Play mute sound to avoid pop sound

	/* Toggling PD_N pin power cycles FT81X and all register contents are lost and reset
	to default when the chip is next switched on. This puts FT81X SPI to single mode
	leaving FT4222 in multi channel if ENABLE_SPI_QUAD/ENABLE_SPI_DUAL enabled.
	Hence explicilty switching the communication to single before power cycle.
	*/
	initSPIMode();

	EVE_Hal_powerCycle(s_pHalContext, FALSE);
	BootupConfig(s_pHalContext);
	//Need download display list again because power down mode lost all registers and memory
	Draw_Text(s_pHalContext, "Power Scenario 4");
	fadein(s_pHalContext);
	EVE_sleep(3000);

	/*************************************************
	Senario5:  Transition from Active mode to PowerDown mode via Standby mode.
	Transition from PowerDown mode to Active mode via Standby mode.
	**************************************************/
	//Switch FT800 from Active to standby mode
	fadeout(s_pHalContext);
	Play_MuteSound(s_pHalContext);//Play mute sound to avoid pop sound
	initSPIMode();

	EVE_Host_powerModeSwitch(s_pHalContext, GPU_STANDBY_M);
	EVE_Hal_powerCycle(s_pHalContext, FALSE);

	BootupConfig(s_pHalContext);
	//Need download display list again because power down mode lost all registers and memory
	Draw_Text(s_pHalContext, "Power Scenario 5");
	fadein(s_pHalContext);
	EVE_sleep(3000);

	/*************************************************
	Senario6:  Transition from Active mode to PowerDown mode via Sleep mode.
	Transition from PowerDown mode to Active mode via Standby mode.
	**************************************************/
	//Switch FT800 from Active to standby mode
	fadeout(s_pHalContext);
	Play_MuteSound(s_pHalContext);//Play mute sound to avoid pop sound
	initSPIMode();

	EVE_Host_powerModeSwitch(s_pHalContext, GPU_SLEEP_M);
	EVE_Hal_powerCycle(s_pHalContext, FALSE);//go to powerdown mode

	BootupConfig(s_pHalContext);
	//Need download display list again because power down mode lost all registers and memory
	Draw_Text(s_pHalContext, "Power Scenario 6");
	fadein(s_pHalContext);
	EVE_sleep(3000);
}

#if defined(FT801_ENABLE) || defined(FT811_ENABLE)
/**
 * @brief explain the usage of touch engine
 *
 */
void SAMAPP_Touch()
{
	int32_t LoopFlag = 0;
	int32_t wbutton;
	int32_t hbutton;
	int32_t tagval;
	int32_t tagoption;
	char8_t StringArray[100], StringArray1[100];
	uint32_t ReadWord;
	int16_t xvalue;
	int16_t yvalue;
	int16_t pendown;

	/*************************************************************************/
	/* Below code demonstrates the usage of touch function. Display info     */
	/* touch raw, touch screen, touch tag, raw adc and resistance values     */
	/*************************************************************************/
	LoopFlag = 300;
	wbutton = s_pHalContext->Width / 8;
	hbutton = s_pHalContext->Height / 8;
	EVE_Hal_wr8(s_pHalContext, REG_CTOUCH_EXTENDED, CTOUCH_MODE_EXTENDED);
	while (LoopFlag--)
	{
		EVE_CoCmd_dlStart(s_pHalContext);
		EVE_Cmd_wr32(s_pHalContext, CLEAR_COLOR_RGB(64, 64, 64));
		EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1));
		EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0xff, 0xff, 0xff));
		EVE_Cmd_wr32(s_pHalContext, TAG_MASK(0));

		StringArray[0] = '\0';
		strcat(StringArray, "Touch Screen XY0 (");
		ReadWord = EVE_Hal_rd32(s_pHalContext, REG_CTOUCH_TOUCH0_XY);
		/*yvalue = (uint16_t)(ReadWord & 0xffff);
		xvalue = (uint16_t)((ReadWord>>16) & 0xffff);*/
		yvalue = (ReadWord & 0xffff);
		xvalue = (ReadWord >> 16);
		Gpu_Hal_Dec2Ascii(StringArray, (int32_t)xvalue);
		strcat(StringArray, ",");
		Gpu_Hal_Dec2Ascii(StringArray, (int32_t)yvalue);
		strcat(StringArray, ")");
		EVE_CoCmd_text(s_pHalContext, (int16_t)(s_pHalContext->Width / 2), 50, 26, OPT_CENTER, StringArray);

		StringArray[0] = '\0';
		strcat(StringArray, "Touch Screen XY1 (");
		ReadWord = EVE_Hal_rd32(s_pHalContext, REG_CTOUCH_TOUCH1_XY);
		yvalue = (ReadWord & 0xffff);
		xvalue = (ReadWord >> 16);
		Gpu_Hal_Dec2Ascii(StringArray, (int32_t)xvalue);
		strcat(StringArray, ",");
		Gpu_Hal_Dec2Ascii(StringArray, (int32_t)yvalue);
		strcat(StringArray, ")");
		EVE_CoCmd_text(s_pHalContext, (int16_t)(s_pHalContext->Width / 2), 70, 26, OPT_CENTER, StringArray);

		StringArray[0] = '\0';
		strcat(StringArray, "Touch Screen XY2 (");
		ReadWord = EVE_Hal_rd32(s_pHalContext, REG_CTOUCH_TOUCH2_XY);
		yvalue = (ReadWord & 0xffff);
		xvalue = (ReadWord >> 16);
		Gpu_Hal_Dec2Ascii(StringArray, (int32_t)xvalue);
		strcat(StringArray, ",");
		Gpu_Hal_Dec2Ascii(StringArray, (int32_t)yvalue);
		strcat(StringArray, ")");
		EVE_CoCmd_text(s_pHalContext, (int16_t)(s_pHalContext->Width / 2), 90, 26, OPT_CENTER, StringArray);

		StringArray[0] = '\0';
		strcat(StringArray, "Touch Screen XY3 (");
		ReadWord = EVE_Hal_rd32(s_pHalContext, REG_CTOUCH_TOUCH3_XY);
		yvalue = (ReadWord & 0xffff);
		xvalue = (ReadWord >> 16);
		Gpu_Hal_Dec2Ascii(StringArray, (int32_t)xvalue);
		strcat(StringArray, ",");
		Gpu_Hal_Dec2Ascii(StringArray, (int32_t)yvalue);
		strcat(StringArray, ")");
		EVE_CoCmd_text(s_pHalContext, (int16_t)(s_pHalContext->Width / 2), 110, 26, OPT_CENTER, StringArray);

		StringArray[0] = '\0';
		StringArray1[0] = '\0';
		strcat(StringArray, "Touch Screen XY4 (");
		xvalue = EVE_Hal_rd16(s_pHalContext, REG_CTOUCH_TOUCH4_X);
		yvalue = EVE_Hal_rd16(s_pHalContext, REG_CTOUCH_TOUCH4_Y);

		Gpu_Hal_Dec2Ascii(StringArray, (int32_t)xvalue);
		strcat(StringArray, ",");
		Gpu_Hal_Dec2Ascii(StringArray1, (int32_t)yvalue);
		strcat(StringArray1, ")");
		strcat(StringArray, StringArray1);
		EVE_CoCmd_text(s_pHalContext, (int16_t)(s_pHalContext->Width / 2), 130, 26, OPT_CENTER, StringArray);

		StringArray[0] = '\0';
		strcat(StringArray, "Touch TAG (");
		ReadWord = EVE_Hal_rd8(s_pHalContext, REG_TOUCH_TAG);
		Gpu_Hal_Dec2Ascii(StringArray, ReadWord);
		strcat(StringArray, ")");
		EVE_CoCmd_text(s_pHalContext, (int16_t)(s_pHalContext->Width / 2), 170, 26, OPT_CENTER, StringArray);
		tagval = ReadWord;

		EVE_CoCmd_fgColor(s_pHalContext, 0x008000);
		EVE_Cmd_wr32(s_pHalContext, TAG_MASK(1));

		EVE_Cmd_wr32(s_pHalContext, TAG(13));
		tagoption = 0;
		if (13 == tagval)
		{
			tagoption = OPT_FLAT;
		}
		EVE_CoCmd_button(s_pHalContext, (s_pHalContext->Width / 2) - (wbutton / 2), (s_pHalContext->Height * 3 / 4) - (hbutton / 2), wbutton, hbutton, 26, tagoption, "Tag13");

		EVE_Cmd_wr32(s_pHalContext, DISPLAY());
		EVE_CoCmd_swap(s_pHalContext);

		/* Wait till coprocessor completes the operation */
		EVE_Cmd_waitFlush(s_pHalContext);
		EVE_sleep(30);

	}

	EVE_Hal_wr8(s_pHalContext, REG_CTOUCH_EXTENDED, CTOUCH_MODE_COMPATIBILITY);
	EVE_sleep(30);
}
#else
/**
 * @brief explain the usage of touch engine
 *
 */
void SAMAPP_Touch()
{
	int32_t LoopFlag = 0;
	int32_t wbutton;
	int32_t hbutton;
	int32_t tagval;
	int32_t tagoption;
	char8_t StringArray[100];
	uint32_t ReadWord;
	uint16_t xvalue;
	uint16_t yvalue;
	uint16_t pendown;

	/*************************************************************************/
	/* Below code demonstrates the usage of touch function. Display info     */
	/* touch raw, touch screen, touch tag, raw adc and resistance values     */
	/*************************************************************************/
	LoopFlag = 300;
	wbutton = s_pHalContext->Width / 8;
	hbutton = s_pHalContext->Height / 8;
	while (LoopFlag--)
	{
		EVE_CoCmd_dlStart(s_pHalContext);
		EVE_Cmd_wr32(s_pHalContext, CLEAR_COLOR_RGB(64, 64, 64));
		EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1));
		EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0xff, 0xff, 0xff));
		EVE_Cmd_wr32(s_pHalContext, TAG_MASK(0));
		/* Draw informative text at width/2,20 location */
		StringArray[0] = '\0';
		strcat(StringArray, "Touch Raw XY (");
		ReadWord = EVE_Hal_rd32(s_pHalContext, REG_TOUCH_RAW_XY);
		yvalue = (uint16_t)(ReadWord & 0xffff);
		xvalue = (uint16_t)((ReadWord >> 16) & 0xffff);
		Gpu_Hal_Dec2Ascii(StringArray, (uint32_t)xvalue);
		strcat(StringArray, ",");
		Gpu_Hal_Dec2Ascii(StringArray, (uint32_t)yvalue);
		strcat(StringArray, ")");
		EVE_CoCmd_text(s_pHalContext, (int16_t)(s_pHalContext->Width / 2), 10, 26, OPT_CENTER, StringArray);

		StringArray[0] = '\0';
		strcat(StringArray, "Touch RZ (");
		ReadWord = EVE_Hal_rd16(s_pHalContext, REG_TOUCH_RZ);
		Gpu_Hal_Dec2Ascii(StringArray, ReadWord);
		strcat(StringArray, ")");
		EVE_CoCmd_text(s_pHalContext, (int16_t)(s_pHalContext->Width / 2), 25, 26, OPT_CENTER, StringArray);

		StringArray[0] = '\0';
		strcat(StringArray, "Touch Screen XY (");
		ReadWord = EVE_Hal_rd32(s_pHalContext, REG_TOUCH_SCREEN_XY);
		yvalue = (int16_t)(ReadWord & 0xffff);
		xvalue = (int16_t)((ReadWord >> 16) & 0xffff);
		Gpu_Hal_Dec2Ascii(StringArray, (int32_t)xvalue);
		strcat(StringArray, ",");
		Gpu_Hal_Dec2Ascii(StringArray, (int32_t)yvalue);
		strcat(StringArray, ")");
		EVE_CoCmd_text(s_pHalContext, (int16_t)(s_pHalContext->Width / 2), 40, 26, OPT_CENTER, StringArray);

		StringArray[0] = '\0';
		strcat(StringArray, "Touch TAG (");
		ReadWord = EVE_Hal_rd8(s_pHalContext, REG_TOUCH_TAG);
		Gpu_Hal_Dec2Ascii(StringArray, ReadWord);
		strcat(StringArray, ")");
		EVE_CoCmd_text(s_pHalContext, (int16_t)(s_pHalContext->Width / 2), 55, 26, OPT_CENTER, StringArray);
		tagval = ReadWord;
		StringArray[0] = '\0';
		strcat(StringArray, "Touch Direct XY (");
		ReadWord = EVE_Hal_rd32(s_pHalContext, REG_TOUCH_DIRECT_XY);
		yvalue = (int16_t)(ReadWord & 0x03ff);
		xvalue = (int16_t)((ReadWord >> 16) & 0x03ff);
		Gpu_Hal_Dec2Ascii(StringArray, (int32_t)xvalue);
		strcat(StringArray, ",");
		Gpu_Hal_Dec2Ascii(StringArray, (int32_t)yvalue);
		pendown = (int16_t)((ReadWord >> 31) & 0x01);
		strcat(StringArray, ",");
		Gpu_Hal_Dec2Ascii(StringArray, (int32_t)pendown);
		strcat(StringArray, ")");
		EVE_CoCmd_text(s_pHalContext, (int16_t)(s_pHalContext->Width / 2), 70, 26, OPT_CENTER, StringArray);

		StringArray[0] = '\0';
		strcat(StringArray, "Touch Direct Z1Z2 (");
		ReadWord = EVE_Hal_rd32(s_pHalContext, REG_TOUCH_DIRECT_Z1Z2);
		yvalue = (int16_t)(ReadWord & 0x03ff);
		xvalue = (int16_t)((ReadWord >> 16) & 0x03ff);
		Gpu_Hal_Dec2Ascii(StringArray, (int32_t)xvalue);
		strcat(StringArray, ",");
		Gpu_Hal_Dec2Ascii(StringArray, (int32_t)yvalue);
		strcat(StringArray, ")");

		EVE_CoCmd_text(s_pHalContext, (int16_t)(s_pHalContext->Width / 2), 85, 26, OPT_CENTER, StringArray);

		EVE_CoCmd_fgColor(s_pHalContext, 0x008000);
		EVE_Cmd_wr32(s_pHalContext, TAG_MASK(1));
		tagoption = 0;
		if (12 == tagval)
		{
			tagoption = OPT_FLAT;
		}

		EVE_Cmd_wr32(s_pHalContext, TAG(12));
		EVE_CoCmd_button(s_pHalContext, (int16_t)((s_pHalContext->Width / 4) - (wbutton / 2)),
			(int16_t)((s_pHalContext->Height * 2 / 4) - (hbutton / 2)),
			(int16_t)wbutton, (int16_t)hbutton, 26, (int16_t)tagoption, "Tag12");
		EVE_Cmd_wr32(s_pHalContext, TAG(13));
		tagoption = 0;
		if (13 == tagval)
		{
			tagoption = OPT_FLAT;
		}
		EVE_CoCmd_button(s_pHalContext, (int16_t)((s_pHalContext->Width * 3 / 4) - (wbutton / 2)),
			(int16_t)((s_pHalContext->Height * 3 / 4) - (hbutton / 2)),
			(int16_t)wbutton, (int16_t)hbutton, 26, (int16_t)tagoption, "Tag13");

		EVE_Cmd_wr32(s_pHalContext, DISPLAY());
		EVE_CoCmd_swap(s_pHalContext);

		/* Wait till coprocessor completes the operation */
		EVE_Cmd_waitFlush(s_pHalContext);
		EVE_sleep(30);

	}
}
#endif

/**
 * @brief read back the result of a command
 *
 */
void SAMAPP_CoPro_Command_CrcCheck() {
	const uint32_t memWrite = 0x66666666;// 66 = 'f'
	const uint32_t memSizeTest = 1024; // bytes
	const uint32_t crcExpected = 0x17D32620;
	uint32_t memcrcRet = 0;

	Draw_Text(s_pHalContext, "Checking Crc32 for 1024 bytes on RAM_G...");

	EVE_CoCmd_memSet(s_pHalContext, RAM_G, memWrite, memSizeTest);
	EVE_Cmd_waitFlush(s_pHalContext);/*reset cmd index*/

	uint16_t  cmdbuff_write_ptr = EVE_Cmd_wp(s_pHalContext);
	uint32_t  crc_result_addr = RAM_CMD + ((cmdbuff_write_ptr + 12) & 4095);

	EVE_CoCmd_memCrc(s_pHalContext, 0, memSizeTest, 0);
	EVE_Cmd_waitFlush(s_pHalContext);

	memcrcRet = EVE_Hal_rd32(s_pHalContext, crc_result_addr);
	printf("current CRC number [0,1023) is 0x%x \r\n", memcrcRet);

	if (memcrcRet == crcExpected) {
		printf("Crc return value is as expected");
		Draw_Text(s_pHalContext, "Done, Crc value is as expected");
	}
}

/**
 * @brief Draw set 1
 *
 */
void App_Set_01_Draw() {
	SAMAPP_CoPro_Command_CrcCheck();
	SAMAPP_CoPro_Logo();
	SAMAPP_CoPro_Widget_Clock();
	SAMAPP_CoPro_Widget_Guage();
	SAMAPP_CoPro_Widget_Gradient();
	SAMAPP_CoPro_Widget_Keys();
	SAMAPP_CoPro_Widget_Keys_Interactive();
	SAMAPP_CoPro_Widget_Progressbar();
	SAMAPP_CoPro_Widget_Scroll();
	SAMAPP_CoPro_Widget_Slider();
	SAMAPP_CoPro_Widget_Dial();
	SAMAPP_CoPro_Widget_Toggle();
	SAMAPP_CoPro_Widget_Spinner();
	SAMAPP_Touch();
	SAMAPP_PowerMode();
}

/**
 * @brief Init set 1
 *
 */
void App_Set_01_Init(EVE_HalContext* pHalContext) {
	s_pHalContext = pHalContext;
	Draw_Text(s_pHalContext, "Set1   START");
}

/**
 * @brief Deinit set 1
 *
 */
void App_Set_01_Deinit() {
	Draw_Text(s_pHalContext, "Set1   END!");
}

#endif // SAMAPP_ENABLE_APIS_SET1
