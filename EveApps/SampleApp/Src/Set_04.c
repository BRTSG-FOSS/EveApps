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
  *     > Widget_Text
  *     > Widget_Number
  *     > Widget_Button
  *     > AppendCmds
  *     > Sound
  *     > Screensaver
  *     > Snapshot
  *     > Sketch
  *     > Matrix
  *     > Track
  *     > Music_Player
  */

#include "Platform.h"
#include "Gpu_Hal.h"
#include "Common.h"
#include "App.h"
#include "Samapp_Enable_API.h"

#ifdef SAMAPP_ENABLE_APIS_SET4

static EVE_HalContext *s_pHalContext;
/**
 * @brief API to demonstrate text widget
 *
 */
void SAMAPP_CoPro_Widget_Text()
{
	/*************************************************************************/
	/* Below code demonstrates the usage of text function. Text function     */
	/* draws text with either in built or externally configured text. Text   */
	/* color can be changed by colorrgb and text function supports display of*/
	/* texts on left, right, top, bottom and center respectively             */
	/*************************************************************************/
	EVE_CoCmd_dlStart(s_pHalContext);
	EVE_Cmd_wr32(s_pHalContext, CLEAR_COLOR_RGB(64, 64, 64));
	EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1));
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0xff, 0xff, 0xff));
	/* Draw text at 0,0 location */
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0x00, 0x00, 0x80));
	EVE_CoCmd_text(s_pHalContext, 0, 0, 29, 0, "Bridgetek!");
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0xff, 0xff, 0xff));
	EVE_CoCmd_text(s_pHalContext, 0, 40, 26, 0, "Text29 at 0,0");//text info
	/* text with centerx */
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0x80, 0x00, 0x00));
	EVE_CoCmd_text(s_pHalContext, (int16_t)(s_pHalContext->Width / 2), 50, 29, OPT_CENTERX, "Bridgetek!");
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0xff, 0xff, 0xff));
	EVE_CoCmd_text(s_pHalContext, (int16_t)((s_pHalContext->Width / 2) - 30), 90, 26, 0, "Text29 centerX");//text info
	/* text with centery */
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0x41, 0x01, 0x05));
	EVE_CoCmd_text(s_pHalContext, (int16_t)(s_pHalContext->Width / 5), 120, 29, OPT_CENTERY, "Bridgetek!");
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0xff, 0xff, 0xff));
	EVE_CoCmd_text(s_pHalContext, (int16_t)(s_pHalContext->Width / 5), 140, 26, 0, "Text29 centerY");//text info
	/* text with center */
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0x0b, 0x07, 0x21));
	EVE_CoCmd_text(s_pHalContext, (int16_t)(s_pHalContext->Width / 2), 180, 29, OPT_CENTER, "Bridgetek!");
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0xff, 0xff, 0xff));
	EVE_CoCmd_text(s_pHalContext, (int16_t)((s_pHalContext->Width / 2) - 50), 200, 26, 0, "Text29 center");//text info
	/* text with rightx */
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0x57, 0x5e, 0x1b));
	EVE_CoCmd_text(s_pHalContext, (int16_t)s_pHalContext->Width, 60, 29, OPT_RIGHTX, "Bridgetek!");
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0xff, 0xff, 0xff));
	EVE_CoCmd_text(s_pHalContext, (int16_t)(s_pHalContext->Width - 90), 100, 26, 0, "Text29 rightX");//text info
	EVE_Cmd_wr32(s_pHalContext, DISPLAY());
	EVE_CoCmd_swap(s_pHalContext);

	/* Wait till coprocessor completes the operation */
	EVE_Cmd_waitFlush(s_pHalContext);
	SAMAPP_ENABLE_DELAY();
}

/**
 * @brief API to demonstrate number widget
 *
 */
void SAMAPP_CoPro_Widget_Number()
{
	/*************************************************************************/
	/* Below code demonstrates the usage of number function. Number function */
	/* draws text with only 32bit decimal number, signed or unsigned can also*/
	/* be specified as input parameter. Options like centerx, centery, center*/
	/* and rightx are supported                                              */
	/*************************************************************************/
	EVE_CoCmd_dlStart(s_pHalContext);
	EVE_Cmd_wr32(s_pHalContext, CLEAR_COLOR_RGB(64, 64, 64));
	EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1));
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0xff, 0xff, 0xff));
	/* Draw number at 0,0 location */
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0x00, 0x00, 0x80));
	EVE_CoCmd_number(s_pHalContext, 0, 0, 29, 0, 1234);
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0xff, 0xff, 0xff));
	EVE_CoCmd_text(s_pHalContext, 0, 40, 26, 0, "Number29 at 0,0");//text info
	/* number with centerx */
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0x80, 0x00, 0x00));
	EVE_CoCmd_number(s_pHalContext, (int16_t)(s_pHalContext->Width / 2), 50, 29, OPT_CENTERX | OPT_SIGNED, -1234);
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0xff, 0xff, 0xff));
	EVE_CoCmd_text(s_pHalContext, (int16_t)((s_pHalContext->Width / 2) - 30), 90, 26, 0, "Number29 centerX");//text info
	/* number with centery */
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0x41, 0x01, 0x05));
	EVE_CoCmd_number(s_pHalContext, (int16_t)(s_pHalContext->Width / 5), 120, 29, OPT_CENTERY, 1234);
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0xff, 0xff, 0xff));
	EVE_CoCmd_text(s_pHalContext, (int16_t)(s_pHalContext->Width / 5), 140, 26, 0, "Number29 centerY");//text info
	/* number with center */
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0x0b, 0x07, 0x21));
	EVE_CoCmd_number(s_pHalContext, (int16_t)(s_pHalContext->Width / 2), 180, 29, OPT_CENTER | OPT_SIGNED, -1234);
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0xff, 0xff, 0xff));
	EVE_CoCmd_text(s_pHalContext, (int16_t)((s_pHalContext->Width / 2) - 50), 200, 26, 0, "Number29 center");//text info
	/* number with rightx */
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0x57, 0x5e, 0x1b));
	EVE_CoCmd_number(s_pHalContext, (int16_t)s_pHalContext->Width, 60, 29, OPT_RIGHTX, 1234);
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0xff, 0xff, 0xff));
	EVE_CoCmd_text(s_pHalContext, (int16_t)(s_pHalContext->Width - 100), 100, 26, 0, "Number29 rightX");//text info

	EVE_Cmd_wr32(s_pHalContext, DISPLAY());
	EVE_CoCmd_swap(s_pHalContext);

	/* Wait till coprocessor completes the operation */
	EVE_Cmd_waitFlush(s_pHalContext);
	SAMAPP_ENABLE_DELAY();
}

/**
 * @brief API to demonstrate button functionality
 *
 */
void SAMAPP_CoPro_Widget_Button()
{
	/*************************************************************************/
	/* Below code demonstrates the usage of button function. Buttons can be  */
	/* constructed using flat or 3d effect. Button color can be changed by   */
	/* fgcolor command and text color is set by COLOR_RGB graphics command   */
	/*************************************************************************/
	int16_t xOffset;
	int16_t yOffset;
	int16_t bWidth;
	int16_t bHeight;
	int16_t bDistx;
	int16_t bDisty;

	bWidth = 60;
	bHeight = 30;
	bDistx = bWidth + (int16_t)((s_pHalContext->Width - 4 * bWidth) / 5);
	bDisty = bHeight + 5;
	xOffset = 10;
	/************ Construct a buttons with "ONE/TWO/THREE" text and default background *********************/
	/* Draw buttons 60x30 resolution at 10x40,10x75,10x110 */

	EVE_CoCmd_dlStart(s_pHalContext);
	EVE_Cmd_wr32(s_pHalContext, CLEAR_COLOR_RGB(64, 64, 64));
	EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1));
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 255, 255));
	/* flat effect and default color background */
	EVE_CoCmd_fgColor(s_pHalContext, 0x0000ff);
	yOffset = (int16_t)(s_pHalContext->Height / 2 - 2 * bDisty);
	EVE_CoCmd_button(s_pHalContext, xOffset, yOffset, bWidth, bHeight, 28, OPT_FLAT, "ABC");
	yOffset += bDisty;
	EVE_CoCmd_button(s_pHalContext, xOffset, yOffset, bWidth, bHeight, 28, OPT_FLAT, "ABC");
	yOffset += bDisty;
	EVE_CoCmd_button(s_pHalContext, xOffset, yOffset, bWidth, bHeight, 28, OPT_FLAT, "ABC");
	EVE_CoCmd_text(s_pHalContext, xOffset, (yOffset + 40), 26, 0, "Flat effect");//text info
	/* 3D effect */
	xOffset += bDistx;
	yOffset = (int16_t)(s_pHalContext->Height / 2 - 2 * bDisty);
	EVE_CoCmd_button(s_pHalContext, xOffset, yOffset, bWidth, bHeight, 28, 0, "ABC");
	yOffset += bDisty;
	EVE_CoCmd_button(s_pHalContext, xOffset, yOffset, bWidth, bHeight, 28, 0, "ABC");
	yOffset += bDisty;
	EVE_CoCmd_button(s_pHalContext, xOffset, yOffset, bWidth, bHeight, 28, 0, "ABC");
	EVE_CoCmd_text(s_pHalContext, xOffset, (yOffset + 40), 26, 0, "3D Effect");//text info
	/* 3d effect with background color */
	EVE_CoCmd_fgColor(s_pHalContext, 0xffff00);
	xOffset += bDistx;
	yOffset = (int16_t)(s_pHalContext->Height / 2 - 2 * bDisty);
	EVE_CoCmd_button(s_pHalContext, xOffset, yOffset, bWidth, bHeight, 28, 0, "ABC");
	yOffset += bDisty;
	EVE_CoCmd_fgColor(s_pHalContext, 0x00ffff);
	EVE_CoCmd_button(s_pHalContext, xOffset, yOffset, bWidth, bHeight, 28, 0, "ABC");
	yOffset += bDisty;
	EVE_CoCmd_fgColor(s_pHalContext, 0xff00ff);
	EVE_CoCmd_button(s_pHalContext, xOffset, yOffset, bWidth, bHeight, 28, 0, "ABC");
	EVE_CoCmd_text(s_pHalContext, xOffset, (yOffset + 40), 26, 0, "3D Color");//text info
	/* 3d effect with gradient color */
	EVE_CoCmd_fgColor(s_pHalContext, 0x101010);
	EVE_CoCmd_gradColor(s_pHalContext, 0xff0000);
	xOffset += bDistx;
	yOffset = (int16_t)(s_pHalContext->Height / 2 - 2 * bDisty);
	EVE_CoCmd_button(s_pHalContext, xOffset, yOffset, bWidth, bHeight, 28, 0, "ABC");
	yOffset += bDisty;
	EVE_CoCmd_gradColor(s_pHalContext, 0x00ff00);
	EVE_CoCmd_button(s_pHalContext, xOffset, yOffset, bWidth, bHeight, 28, 0, "ABC");
	yOffset += bDisty;
	EVE_CoCmd_gradColor(s_pHalContext, 0x0000ff);
	EVE_CoCmd_button(s_pHalContext, xOffset, yOffset, bWidth, bHeight, 28, 0, "ABC");
	EVE_CoCmd_text(s_pHalContext, xOffset, (yOffset + 40), 26, 0, "3D Gradient");//text info
	EVE_Cmd_wr32(s_pHalContext, DISPLAY());
	EVE_CoCmd_swap(s_pHalContext);

	/* Wait till coprocessor completes the operation */
	EVE_Cmd_waitFlush(s_pHalContext);
	SAMAPP_ENABLE_DELAY();
}

/**
 * @brief API to demonstrate the use of transfer commands
 *
 */
void SAMAPP_CoPro_AppendCmds()
{
	uint32_t AppendCmds[16];
	int16_t xoffset;
	int16_t yoffset;
	/*************************************************************************/
	/* Below code demonstrates the usage of coprocessor append command - to append any*/
	/* mcu specific graphics commands to coprocessor generated graphics commands      */
	/*************************************************************************/

	/* Bitmap construction by MCU - display lena at 200x60 offset */
	/* Construct the bitmap data to be copied in the temp buffer then by using
	coprocessor append command copy it into graphics processor DL memory */
	xoffset = (int16_t)((s_pHalContext->Width - SAMAPP_Bitmap_RawData_Header[0].Width) / 2);
	yoffset = (int16_t)((s_pHalContext->Height / 3) - SAMAPP_Bitmap_RawData_Header[0].Height / 2);

	EVE_Cmd_wr32(s_pHalContext, CMD_DLSTART);
	AppendCmds[0] = CLEAR_COLOR_RGB(255, 0, 0);
	AppendCmds[1] = CLEAR(1, 1, 1);
	AppendCmds[2] = COLOR_RGB(255, 255, 255);
	AppendCmds[3] = BEGIN(BITMAPS);
	AppendCmds[4] = BITMAP_SOURCE(0);
	AppendCmds[5] = BITMAP_LAYOUT(SAMAPP_Bitmap_RawData_Header[0].Format,
		SAMAPP_Bitmap_RawData_Header[0].Stride, SAMAPP_Bitmap_RawData_Header[0].Height);
	AppendCmds[6] = BITMAP_SIZE(BILINEAR, BORDER, BORDER,
		SAMAPP_Bitmap_RawData_Header[0].Width, SAMAPP_Bitmap_RawData_Header[0].Height);
	AppendCmds[7] = VERTEX2F(xoffset * 16, yoffset * 16);
	AppendCmds[8] = END();

	/* Download the bitmap data*/
	Ftf_Write_File_nBytes_To_RAM_G(s_pHalContext, TEST_DIR "\\SAMAPP_Bitmap_RawData.bin", RAM_G,
		SAMAPP_Bitmap_RawData_Header[0].Stride * SAMAPP_Bitmap_RawData_Header[0].Height,
		SAMAPP_Bitmap_RawData_Header[0].Arrayoffset);

	/* Download the DL data constructed by the MCU to location 40*40*2 in sram */
	EVE_Hal_wrMem(s_pHalContext, RAM_G + SAMAPP_Bitmap_RawData_Header[0].Stride * SAMAPP_Bitmap_RawData_Header[0].Height, (uint8_t*)AppendCmds, 9 * 4);

	/* Call the append api for copying the above generated data into graphics processor
	DL commands are stored at location 40*40*2 offset from the starting of the sram */
	EVE_CoCmd_append(s_pHalContext, RAM_G + SAMAPP_Bitmap_RawData_Header[0].Stride * SAMAPP_Bitmap_RawData_Header[0].Height, 9 * 4);
	/*  Display the text information */
	EVE_CoCmd_fgColor(s_pHalContext, 0xffff00);
	xoffset -= 50;
	yoffset += 40;
	EVE_CoCmd_text(s_pHalContext, xoffset, yoffset, 26, 0, "Display bitmap by Append");
	EVE_Cmd_wr32(s_pHalContext, DISPLAY());
	EVE_CoCmd_swap(s_pHalContext);
	/* Download the commands into fifo */
	EVE_Cmd_waitFlush(s_pHalContext);
	SAMAPP_ENABLE_DELAY();
}

/**
 * @brief API to demonstrate screen saver widget - playing of bitmap via macro0
 *
 */
void SAMAPP_CoPro_Screensaver()
{
	/*************************************************************************/
	/* Below code demonstrates the usage of screensaver function. Screen     */
	/* saver modifies macro0 with the vertex2f command.                      */
	/* MCU can display any static display list at the background with the    */
	/* changing bitmap                                                       */
	/*************************************************************************/
		/* Download the bitmap data */
	Ftf_Write_File_nBytes_To_RAM_G(s_pHalContext, TEST_DIR "\\SAMAPP_Bitmap_RawData.bin", RAM_G,
		SAMAPP_Bitmap_RawData_Header[0].Stride * SAMAPP_Bitmap_RawData_Header[0].Height,
		SAMAPP_Bitmap_RawData_Header[0].Arrayoffset);

	/* Send command screen saver */
	EVE_Cmd_wr32(s_pHalContext, CMD_SCREENSAVER);//screen saver command will continuously update the macro0 with vertex2f command
	/* Copy the display list */
	EVE_CoCmd_dlStart(s_pHalContext);
	EVE_Cmd_wr32(s_pHalContext, CLEAR_COLOR_RGB(0, 0, 0x80));
	EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1));
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0xff, 0xff, 0xff));
	/* lena bitmap */
	EVE_CoCmd_loadIdentity(s_pHalContext);
	EVE_CoCmd_scale(s_pHalContext, 3 * 65536, 3 * 65536);//scale the bitmap 3 times on both sides
	EVE_CoCmd_setMatrix(s_pHalContext);
	EVE_Cmd_wr32(s_pHalContext, BEGIN(BITMAPS));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_SOURCE(0));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_LAYOUT(SAMAPP_Bitmap_RawData_Header[0].Format,
		SAMAPP_Bitmap_RawData_Header[0].Stride, SAMAPP_Bitmap_RawData_Header[0].Height));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_SIZE(BILINEAR, BORDER, BORDER,
		SAMAPP_Bitmap_RawData_Header[0].Width * 3, SAMAPP_Bitmap_RawData_Header[0].Height * 3));
	EVE_Cmd_wr32(s_pHalContext, MACRO(0));
	EVE_Cmd_wr32(s_pHalContext, END());
	/* Display the text */
	EVE_CoCmd_loadIdentity(s_pHalContext);
	EVE_CoCmd_setMatrix(s_pHalContext);
	EVE_CoCmd_text(s_pHalContext, (int16_t)(s_pHalContext->Width / 2), (int16_t)(s_pHalContext->Height / 2), 27, OPT_CENTER, "Screen Saver ...");
	EVE_CoCmd_memSet(s_pHalContext, (RAM_G + 3200), 0xff, (160L * 2 * 120));
	EVE_Cmd_wr32(s_pHalContext, DISPLAY());
	EVE_CoCmd_swap(s_pHalContext);

	/* Wait till coprocessor completes the operation */
	EVE_Cmd_waitFlush(s_pHalContext);

	EVE_sleep(3 * 1000);

	/* Send the stop command */
	EVE_Cmd_wr32(s_pHalContext, CMD_STOP);
	/* Update the command buffer pointers - both read and write pointers */
	EVE_Cmd_waitFlush(s_pHalContext);
}

/**
 * @brief Sample app to demonstrate snapshot widget/functionality
 *
 */
void SAMAPP_CoPro_Snapshot()
{
	/*************************************************************************/
	/* Below code demonstrates the usage of snapshot function. Snapshot func */
	/* captures the present screen and dumps into bitmap with color formats  */
	/* argb4.  FT81x supports a simplier snapshot2 command to capture the    */
	/* screen, both methods can be used in FT81X.                            */
	/*************************************************************************/
	uint16_t bitmapWidth = 160;
	uint16_t bitmapHeight = 120;
	uint16_t RGB565RamOffset = 0;
	uint16_t ARGB4RamOffset = RGB565RamOffset + bitmapWidth * 2 * bitmapHeight;
#ifndef BT815_ENABLE
	fadeout(s_pHalContext);
#endif
#ifndef FT81X_ENABLE

#if !defined(ME800A_HV35R) && !defined(ME810A_HV35R)
	/* fadeout before switching off the pclock */

	/* Switch off the lcd */
	{
		uint8_t n = 0;
		EVE_Hal_wr8(s_pHalContext, REG_GPIO, 0x7f);
		EVE_sleep(100);
	}
#endif
	uint32_t WriteByte = 0;
	/* Disable the pclock */
	EVE_Hal_wr8(s_pHalContext, REG_PCLK, WriteByte);
	/* Configure the resolution to 160x120 dimention */
	WriteByte = bitmapWidth;
	EVE_Hal_wr16(s_pHalContext, REG_HSIZE, WriteByte);
	WriteByte = bitmapHeight;
	EVE_Hal_wr16(s_pHalContext, REG_VSIZE, WriteByte);
#endif
	/* Construct screen shot for snapshot */

	EVE_CoCmd_dlStart(s_pHalContext);
	EVE_Cmd_wr32(s_pHalContext, CLEAR_COLOR_RGB(255, 255, 255));
	EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1));
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(28, 20, 99));
	/* captured snapshot */
	EVE_Cmd_wr32(s_pHalContext, BEGIN(FTPOINTS));
	EVE_Cmd_wr32(s_pHalContext, COLOR_A(128));
	EVE_Cmd_wr32(s_pHalContext, POINT_SIZE(20 * 16));
	EVE_Cmd_wr32(s_pHalContext, VERTEX2F(0 * 16, 0 * 16));
	EVE_Cmd_wr32(s_pHalContext, POINT_SIZE(25 * 16));
	EVE_Cmd_wr32(s_pHalContext, VERTEX2F(20 * 16, 10 * 16));
	EVE_Cmd_wr32(s_pHalContext, POINT_SIZE(30 * 16));
	EVE_Cmd_wr32(s_pHalContext, VERTEX2F(40 * 16, 20 * 16));
	EVE_Cmd_wr32(s_pHalContext, POINT_SIZE(35 * 16));
	EVE_Cmd_wr32(s_pHalContext, VERTEX2F(60 * 16, 30 * 16));
	EVE_Cmd_wr32(s_pHalContext, POINT_SIZE(40 * 16));
	EVE_Cmd_wr32(s_pHalContext, VERTEX2F(80 * 16, 40 * 16));
	EVE_Cmd_wr32(s_pHalContext, POINT_SIZE(45 * 16));
	EVE_Cmd_wr32(s_pHalContext, VERTEX2F(100 * 16, 50 * 16));
	EVE_Cmd_wr32(s_pHalContext, POINT_SIZE(50 * 16));
	EVE_Cmd_wr32(s_pHalContext, VERTEX2F(120 * 16, 60 * 16));
	EVE_Cmd_wr32(s_pHalContext, POINT_SIZE(55 * 16));
	EVE_Cmd_wr32(s_pHalContext, VERTEX2F(140 * 16, 70 * 16));
	EVE_Cmd_wr32(s_pHalContext, POINT_SIZE(60 * 16));
	EVE_Cmd_wr32(s_pHalContext, VERTEX2F(160 * 16, 80 * 16));
	EVE_Cmd_wr32(s_pHalContext, POINT_SIZE(65 * 16));
	EVE_Cmd_wr32(s_pHalContext, VERTEX2F(0 * 16, 120 * 16));
	EVE_Cmd_wr32(s_pHalContext, POINT_SIZE(70 * 16));
	EVE_Cmd_wr32(s_pHalContext, VERTEX2F(160 * 16, 0 * 16));
	EVE_Cmd_wr32(s_pHalContext, END()); //display the bitmap at the center of the display
	EVE_Cmd_wr32(s_pHalContext, COLOR_A(255));
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(32, 32, 32));
	EVE_CoCmd_text(s_pHalContext, 80, 60, 26, OPT_CENTER, "Points");

	EVE_Cmd_wr32(s_pHalContext, DISPLAY());
	EVE_CoCmd_swap(s_pHalContext);

	/* Wait till coprocessor completes the operation */
	EVE_Cmd_waitFlush(s_pHalContext);

	EVE_sleep(100);//timeout for snapshot to be performed by coprocessor

#ifndef FT81X_ENABLE
						   /* Take snap shot of the current screen */
	EVE_Cmd_wr32(s_pHalContext, CMD_SNAPSHOT);
	EVE_Cmd_wr32(s_pHalContext, 3200); //store the rgb content at location 3200

	//timeout for snapshot to be performed by coprocessor

	/* Wait till coprocessor completes the operation */
	EVE_Cmd_waitFlush(s_pHalContext);

	EVE_sleep(100); //timeout for snapshot to be performed by coprocessor

	/* reconfigure the resolution wrt configuration */
	WriteByte = s_pHalContext->Width;
	EVE_Hal_wr16(s_pHalContext, REG_HSIZE, WriteByte);
	WriteByte = s_pHalContext->Height;
	EVE_Hal_wr16(s_pHalContext, REG_VSIZE, WriteByte);
#else

	EVE_CoCmd_snapshot2(s_pHalContext, ARGB4, ARGB4RamOffset, 0, 0, bitmapWidth, bitmapHeight);
	/* Wait till coprocessor completes the operation */
	EVE_Cmd_waitFlush(s_pHalContext);

	EVE_sleep(100); //timeout for snapshot to be performed by coprocessor

	EVE_CoCmd_snapshot2(s_pHalContext, RGB565, RGB565RamOffset, 0, 0, bitmapWidth, bitmapHeight);
	/* Wait till coprocessor completes the operation */
	EVE_Cmd_waitFlush(s_pHalContext);

	EVE_sleep(100); //timeout for snapshot to be performed by coprocessor
#endif

	EVE_CoCmd_dlStart(s_pHalContext);
	EVE_Cmd_wr32(s_pHalContext, CLEAR_COLOR_RGB(0xff, 0xff, 0xff));
	EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1));
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0xff, 0xff, 0xff));
	/* captured snapshot */

#ifndef FT81X_ENABLE
	EVE_Cmd_wr32(s_pHalContext, BEGIN(BITMAPS));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_SOURCE(3200));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_LAYOUT(ARGB4, bitmapWidth * 2, bitmapHeight));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_SIZE(BILINEAR, BORDER, BORDER, bitmapWidth, bitmapHeight));
	EVE_Cmd_wr32(s_pHalContext, VERTEX2F(((s_pHalContext->Width - 160) / 2) * 16, ((s_pHalContext->Height - 120) / 2) * 16));
	EVE_Cmd_wr32(s_pHalContext, END()); //display the bitmap at the center of the display
	/* Display the text info */
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(32, 32, 32));
	EVE_CoCmd_text(s_pHalContext, (int16_t)(s_pHalContext->Width / 2), 40, 27, OPT_CENTER, "Snap shot");
#else
	EVE_Cmd_wr32(s_pHalContext, BEGIN(BITMAPS));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_SOURCE(RGB565RamOffset));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_LAYOUT(RGB565, bitmapWidth * 2, bitmapHeight));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_SIZE(NEAREST, BORDER, BORDER, bitmapWidth, bitmapHeight));
	EVE_Cmd_wr32(s_pHalContext, VERTEX2F(((s_pHalContext->Width) / 6) * 16, (int16_t)((s_pHalContext->Height) / 2) * 16));
	EVE_Cmd_wr32(s_pHalContext, END()); //display the bitmap at the center of the display
	/* Display the text info */
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(32, 32, 32));
	EVE_CoCmd_text(s_pHalContext, (int16_t)((s_pHalContext->Width) / 6), (int16_t)((s_pHalContext->Height) / 2) - 20, 27, 0, "RGB565");

	EVE_Cmd_wr32(s_pHalContext, BEGIN(BITMAPS));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_SOURCE(ARGB4RamOffset));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_LAYOUT(ARGB4, bitmapWidth * 2, bitmapHeight));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_SIZE(NEAREST, BORDER, BORDER, bitmapWidth, bitmapHeight));
	EVE_Cmd_wr32(s_pHalContext, VERTEX2F(((s_pHalContext->Width) / 2) * 16, (int16_t)((s_pHalContext->Height) / 2) * 16));
	EVE_Cmd_wr32(s_pHalContext, END()); //display the bitmap at the center of the display
	/* Display the text info */
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(32, 32, 32));
	EVE_CoCmd_text(s_pHalContext, (int16_t)(s_pHalContext->Width / 2), (int16_t)((s_pHalContext->Height) / 2) - 20, 27, 0, "ARGB4");

	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(32, 32, 32));
	EVE_CoCmd_text(s_pHalContext, (int16_t)(s_pHalContext->Width / 2), 40, 30, OPT_CENTER, "Snap Shot");
#endif
	EVE_Cmd_wr32(s_pHalContext, DISPLAY());
	EVE_CoCmd_swap(s_pHalContext);

	/* Wait till coprocessor completes the operation */
	EVE_Cmd_waitFlush(s_pHalContext);
#ifndef FT81X_ENABLE
	/* reenable the pclock */
	WriteByte = s_pHalContext->PCLK;
	EVE_Hal_wr8(s_pHalContext, REG_PCLK, WriteByte);
	EVE_sleep(60);
#if !defined(ME800A_HV35R) && !defined(ME810A_HV35R)
	/* Power on the LCD */
	{
		uint8_t n = 0x80;
		EVE_Hal_wr8(s_pHalContext, REG_GPIO, 0xff);
	}
#endif
#endif
	EVE_sleep(200);//give some time for the lcd to switchon - hack for one perticular panel

	/* set the display pwm back to 128 */
#ifndef BT815_ENABLE
	fadein(s_pHalContext);
#endif
	SAMAPP_ENABLE_DELAY();
}

/**
 * @brief API to demonstrate sketch widget
 *
 */
void SAMAPP_CoPro_Sketch()
{
	/*************************************************************************/
	/* Below code demonstrates the usage of sketch function. Sketch function */
	/* draws line for pen movement. Skecth supports L1 and L8 output formats */
	/* Sketch draws till stop command is executed.                           */
	/*************************************************************************/
	int16_t BorderSz = 40;
	uint32_t MemZeroSz;
	/* Send command sketch */

	MemZeroSz = 1L * (s_pHalContext->Width - 2 * BorderSz) * (s_pHalContext->Height - 2 * BorderSz);
	EVE_CoCmd_memZero(s_pHalContext, RAM_G, MemZeroSz);
#ifdef FT801_ENABLE
	EVE_CoCmd_cSketch(s_pHalContext, BorderSz, BorderSz, (int16_t)(s_pHalContext->Width - 2 * BorderSz), (int16_t)(s_pHalContext->Height - 2 * BorderSz), 0, L1, 1500L);//sketch in L1 format
#else
	EVE_CoCmd_sketch(s_pHalContext, BorderSz, BorderSz, (int16_t)(s_pHalContext->Width - 2 * BorderSz), (int16_t)(s_pHalContext->Height - 2 * BorderSz), RAM_G, L1);//sketch in L1 format
#endif
		/* Display the sketch */
	EVE_CoCmd_dlStart(s_pHalContext);
	EVE_Cmd_wr32(s_pHalContext, CLEAR_COLOR_RGB(0x80, 0, 0x00));
	EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1));
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 255, 255));
	EVE_Cmd_wr32(s_pHalContext, SCISSOR_SIZE((s_pHalContext->Width - 2 * BorderSz), (s_pHalContext->Height - 2 * BorderSz)));
	EVE_Cmd_wr32(s_pHalContext, SCISSOR_XY(BorderSz, BorderSz));
	EVE_Cmd_wr32(s_pHalContext, CLEAR_COLOR_RGB(255, 255, 255));
	EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1));

	/* default the scissor size */
#ifdef FT81X_ENABLE
	EVE_Cmd_wr32(s_pHalContext, SCISSOR_SIZE(2048, 2048));
#else
	EVE_Cmd_wr32(s_pHalContext, SCISSOR_SIZE(512, 512));
#endif

	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0, 0, 0));
	EVE_Cmd_wr32(s_pHalContext, SCISSOR_XY(0, 0));
	/* L1 bitmap display */
	EVE_Cmd_wr32(s_pHalContext, BEGIN(BITMAPS));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_SOURCE(0));
#ifdef FT81X_ENABLE

	EVE_Cmd_wr32(s_pHalContext, BITMAP_LAYOUT(L1, (s_pHalContext->Width - 2 * BorderSz) / 8, (s_pHalContext->Height - 2 * BorderSz)));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_LAYOUT_H((((s_pHalContext->Width - 2 * BorderSz) / 8) >> 10), ((s_pHalContext->Height - 2 * BorderSz) >> 9)));

	EVE_Cmd_wr32(s_pHalContext, BITMAP_SIZE(BILINEAR, BORDER, BORDER, (s_pHalContext->Width - 2 * BorderSz), (s_pHalContext->Height - 2 * BorderSz)));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_SIZE_H(((s_pHalContext->Width - 2 * BorderSz) >> 9), ((s_pHalContext->Height - 2 * BorderSz) >> 9)));

#else
	EVE_Cmd_wr32(s_pHalContext, BITMAP_LAYOUT(L1, (s_pHalContext->Width - 2 * BorderSz) / 8, (s_pHalContext->Height - 2 * BorderSz)));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_SIZE(BILINEAR, BORDER, BORDER, (s_pHalContext->Width - 2 * BorderSz), (s_pHalContext->Height - 2 * BorderSz)));
#endif
	EVE_Cmd_wr32(s_pHalContext, VERTEX2F(BorderSz * 16, BorderSz * 16));
	EVE_Cmd_wr32(s_pHalContext, END());
	/* Display the text */
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0xff, 0xff, 0xff));
	EVE_CoCmd_text(s_pHalContext, (int16_t)(s_pHalContext->Width / 2), 20, 27, OPT_CENTER, "Sketch L1");

	EVE_Cmd_wr32(s_pHalContext, DISPLAY());
	EVE_CoCmd_swap(s_pHalContext);

	/* Wait till coprocessor completes the operation */
	EVE_Cmd_waitFlush(s_pHalContext);

	EVE_sleep(10 * 1000);//sleep for 10 seconds

	/* Send the stop command */
	EVE_Cmd_wr32(s_pHalContext, CMD_STOP);
	/* Update the command buffer pointers - both read and write pointers */
	EVE_Cmd_waitFlush(s_pHalContext);

	/* Sketch L8 format */

	/* Send command sketch */

	EVE_CoCmd_memZero(s_pHalContext, RAM_G, MemZeroSz);
#ifdef FT801_ENABLE
	EVE_CoCmd_cSketch(s_pHalContext, BorderSz, BorderSz, (int16_t)(s_pHalContext->Width - 2 * BorderSz), (int16_t)(s_pHalContext->Height - 2 * BorderSz), 0, L8, 1500L);//sketch in L8 format
#else
	EVE_CoCmd_sketch(s_pHalContext, BorderSz, BorderSz, (int16_t)(s_pHalContext->Width - 2 * BorderSz), (int16_t)(s_pHalContext->Height - 2 * BorderSz), 0, L8);//sketch in L8 format
#endif
		/* Display the sketch */
	EVE_CoCmd_dlStart(s_pHalContext);
	EVE_Cmd_wr32(s_pHalContext, CLEAR_COLOR_RGB(0x00, 0, 0x80));
	EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1));
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0xff, 0xff, 0xff));
	EVE_Cmd_wr32(s_pHalContext, SCISSOR_SIZE((s_pHalContext->Width - 2 * BorderSz), (s_pHalContext->Height - 2 * BorderSz)));
	EVE_Cmd_wr32(s_pHalContext, SCISSOR_XY(BorderSz, BorderSz));
	EVE_Cmd_wr32(s_pHalContext, CLEAR_COLOR_RGB(0xff, 0xff, 0xff));
	EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1));

#ifdef FT81X_ENABLE
	EVE_Cmd_wr32(s_pHalContext, SCISSOR_SIZE(2048, 2048));
#else
	EVE_Cmd_wr32(s_pHalContext, SCISSOR_SIZE(512, 512));
#endif
	EVE_Cmd_wr32(s_pHalContext, SCISSOR_XY(0, 0));
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0, 0, 0));
	/* L8 bitmap display */
	EVE_Cmd_wr32(s_pHalContext, BEGIN(BITMAPS));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_SOURCE(0));
#ifdef FT81X_ENABLE

	EVE_Cmd_wr32(s_pHalContext, BITMAP_LAYOUT(L8, (s_pHalContext->Width - 2 * BorderSz), (s_pHalContext->Height - 2 * BorderSz)));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_LAYOUT_H((((s_pHalContext->Width - 2 * BorderSz)) >> 10), ((s_pHalContext->Height - 2 * BorderSz) >> 9)));

	EVE_Cmd_wr32(s_pHalContext, BITMAP_SIZE(BILINEAR, BORDER, BORDER, (s_pHalContext->Width - 2 * BorderSz), (s_pHalContext->Height - 2 * BorderSz)));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_SIZE_H(((s_pHalContext->Width - 2 * BorderSz) >> 9), ((s_pHalContext->Height - 2 * BorderSz) >> 9)));

#else
	EVE_Cmd_wr32(s_pHalContext, BITMAP_LAYOUT(L8, (s_pHalContext->Width - 2 * BorderSz), (s_pHalContext->Height - 2 * BorderSz)));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_SIZE(BILINEAR, BORDER, BORDER, (s_pHalContext->Width - 2 * BorderSz), (s_pHalContext->Height - 2 * BorderSz)));
#endif
	EVE_Cmd_wr32(s_pHalContext, VERTEX2F(BorderSz * 16, BorderSz * 16));
	EVE_Cmd_wr32(s_pHalContext, END());
	/* Display the text */
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0xff, 0xff, 0xff));
	EVE_CoCmd_text(s_pHalContext, (int16_t)(s_pHalContext->Width / 2), 20, 27, OPT_CENTER, "Sketch L8");

	EVE_Cmd_wr32(s_pHalContext, DISPLAY());
	EVE_CoCmd_swap(s_pHalContext);

	/* Wait till coprocessor completes the operation */
	EVE_Cmd_waitFlush(s_pHalContext);

	EVE_sleep(3 * 1000);//sleep for 3 seconds

	/* Send the stop command */
	EVE_Cmd_wr32(s_pHalContext, CMD_STOP);
	/* Update the command buffer pointers - both read and write pointers */
	EVE_Cmd_waitFlush(s_pHalContext);
}

/**
 * @brief API to demonstrate scale, rotate and translate functionality
 *
 */
void SAMAPP_CoPro_Matrix()
{
	/*************************************************************************/
	/* Below code demonstrates the usage of bitmap matrix processing apis.   */
	/* Mainly matrix apis consists if scale, rotate and translate.           */
	/* Units of translation and scale are interms of 1/65536, rotation is in */
	/* degrees and in terms of 1/65536. +ve theta is anticlock wise, and -ve  */
	/* theta is clock wise rotation                                          */
	/*************************************************************************/

	/* Lena image with 40x40 rgb565 is used as an example */
	int32_t imagewidth;
	int32_t imagestride;
	int32_t imageheight;
	int32_t imagexoffset;
	int32_t imageyoffset;

	/* Download the bitmap data */
	Ftf_Write_File_nBytes_To_RAM_G(s_pHalContext, TEST_DIR "\\SAMAPP_Bitmap_RawData.bin", RAM_G,
		SAMAPP_Bitmap_RawData_Header[0].Stride * SAMAPP_Bitmap_RawData_Header[0].Height, SAMAPP_Bitmap_RawData_Header[0].Arrayoffset);

	EVE_CoCmd_dlStart(s_pHalContext);
	EVE_Cmd_wr32(s_pHalContext, CLEAR_COLOR_RGB(0xff, 0xff, 0xff));
	EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1));
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(32, 32, 32));
	EVE_CoCmd_text(s_pHalContext, 10, 5, 16, 0, "BM with rotation");
	EVE_CoCmd_text(s_pHalContext, 10, 20 + 40 + 5, 16, 0, "BM with scaling");
	EVE_CoCmd_text(s_pHalContext, 10, 20 + 40 + 20 + 80 + 5, 16, 0, "BM with flip");

	imagewidth = SAMAPP_Bitmap_RawData_Header[0].Width;
	imageheight = SAMAPP_Bitmap_RawData_Header[0].Height;
	imagestride = SAMAPP_Bitmap_RawData_Header[0].Stride;
	imagexoffset = 10 * 16;
	imageyoffset = 20 * 16;

	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0xff, 0xff, 0xff));
	EVE_Cmd_wr32(s_pHalContext, BEGIN(BITMAPS));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_SOURCE(0));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_LAYOUT(SAMAPP_Bitmap_RawData_Header[0].Format, imagestride, imageheight));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_SIZE(BILINEAR, BORDER, BORDER, imagewidth, imageheight));
	/******************************************* Perform display of plain bitmap ************************************/
	EVE_Cmd_wr32(s_pHalContext, VERTEX2F(imagexoffset, imageyoffset));

	/* Perform display of plain bitmap with 45 degrees anti clock wise and the rotation is performed on top left coordinate */
	imagexoffset += (imagewidth + 10) * 16;
	EVE_CoCmd_loadIdentity(s_pHalContext);
	EVE_CoCmd_rotate(s_pHalContext, (-45 * 65536 / 360));//rotate by 45 degrees anticlock wise
	EVE_CoCmd_setMatrix(s_pHalContext);
	EVE_Cmd_wr32(s_pHalContext, VERTEX2F(imagexoffset, imageyoffset));

	/* Perform display of plain bitmap with 30 degrees clock wise and the rotation is performed on top left coordinate */
	imagexoffset += (int32_t)(imagewidth * 1.42 + 10) * 16;//add the width*1.41 as diagonal is new width and extra 10 pixels
	EVE_CoCmd_loadIdentity(s_pHalContext);
	EVE_CoCmd_rotate(s_pHalContext, 30 * 65536 / 360);//rotate by 33 degrees clock wise
	EVE_CoCmd_setMatrix(s_pHalContext);
	EVE_Cmd_wr32(s_pHalContext, VERTEX2F(imagexoffset, imageyoffset));

	/* Perform display of plain bitmap with 45 degrees anti clock wise and the rotation is performed wrt centre of the bitmap */
	imagexoffset += (int32_t)(imagewidth * 1.42 + 10) * 16;//add the width*1.41 as diagonal is new width and extra 10 pixels
	EVE_CoCmd_loadIdentity(s_pHalContext);
	EVE_CoCmd_translate(s_pHalContext, 65536 * imagewidth / 2, 65536 * imageheight / 2);//make the rotation coordinates at the center
	EVE_CoCmd_rotate(s_pHalContext, -45 * 65536 / 360);//rotate by 45 degrees anticlock wise
	EVE_CoCmd_translate(s_pHalContext, -65536 * imagewidth / 2, -65536 * imageheight / 2);
	EVE_CoCmd_setMatrix(s_pHalContext);
	EVE_Cmd_wr32(s_pHalContext, VERTEX2F(imagexoffset, imageyoffset));

	/* Perform display of plain bitmap with 45 degrees clock wise and the rotation is performed so that whole bitmap is viewable */
	imagexoffset += (int32_t)(imagewidth * 1.42 + 10) * 16;//add the width*1.41 as diagonal is new width and extra 10 pixels
	EVE_CoCmd_loadIdentity(s_pHalContext);
	EVE_CoCmd_translate(s_pHalContext, 65536 * imagewidth / 2, 65536 * imageheight / 2);//make the rotation coordinates at the center
	EVE_CoCmd_rotate(s_pHalContext, 45 * 65536 / 360);//rotate by 45 degrees clock wise
	EVE_CoCmd_translate(s_pHalContext, -65536 * imagewidth / 10, -65536 * imageheight / 2);
	EVE_CoCmd_setMatrix(s_pHalContext);
	EVE_Cmd_wr32(s_pHalContext, BITMAP_SIZE(BILINEAR, BORDER, BORDER, imagewidth * 2, imageheight * 2));
	EVE_Cmd_wr32(s_pHalContext, VERTEX2F(imagexoffset, imageyoffset));

	/* Perform display of plain bitmap with 90 degrees anti clock wise and the rotation is performed so that whole bitmap is viewable */
	imagexoffset += (imagewidth * 2 + 10) * 16;
	EVE_CoCmd_loadIdentity(s_pHalContext);
	EVE_CoCmd_translate(s_pHalContext, 65536 * imagewidth / 2, 65536 * imageheight / 2);//make the rotation coordinates at the center
	EVE_CoCmd_rotate(s_pHalContext, -90 * 65536 / 360);//rotate by 90 degrees anticlock wise
	EVE_CoCmd_translate(s_pHalContext, -65536 * imagewidth / 2, -65536 * imageheight / 2);
	EVE_CoCmd_setMatrix(s_pHalContext);
	EVE_Cmd_wr32(s_pHalContext, BITMAP_SIZE(BILINEAR, BORDER, BORDER, imagewidth, imageheight));
	EVE_Cmd_wr32(s_pHalContext, VERTEX2F(imagexoffset, imageyoffset));

	/* Perform display of plain bitmap with 180 degrees clock wise and the rotation is performed so that whole bitmap is viewable */
	imagexoffset += (imagewidth + 10) * 16;
	EVE_CoCmd_loadIdentity(s_pHalContext);
	EVE_CoCmd_translate(s_pHalContext, 65536 * imagewidth / 2, 65536 * imageheight / 2);//make the rotation coordinates at the center
	EVE_CoCmd_rotate(s_pHalContext, -180 * 65536 / 360);//rotate by 180 degrees anticlock wise
	EVE_CoCmd_translate(s_pHalContext, -65536 * imagewidth / 2, -65536 * imageheight / 2);
	EVE_CoCmd_setMatrix(s_pHalContext);
	EVE_Cmd_wr32(s_pHalContext, VERTEX2F(imagexoffset, imageyoffset));
	/******************************************* Perform display of bitmap with scale ************************************/
	/* Perform display of plain bitmap with scale factor of 2x2 in x & y direction */
	imagexoffset = (10) * 16;
	imageyoffset += (imageheight + 20) * 16;
	EVE_CoCmd_loadIdentity(s_pHalContext);
	EVE_CoCmd_scale(s_pHalContext, 2 * 65536, 2 * 65536);//scale by 2x2
	EVE_CoCmd_setMatrix(s_pHalContext);
	EVE_Cmd_wr32(s_pHalContext, BITMAP_SIZE(BILINEAR, BORDER, BORDER, imagewidth * 2, imageheight * 2));
	EVE_Cmd_wr32(s_pHalContext, VERTEX2F(imagexoffset, imageyoffset));

	/* Perform display of plain bitmap with scale factor of .5x.25 in x & y direction, rotate by 45 degrees clock wise wrt top left */
	imagexoffset += (imagewidth * 2 + 10) * 16;
	EVE_CoCmd_loadIdentity(s_pHalContext);
	EVE_CoCmd_translate(s_pHalContext, 65536 * imagewidth / 2, 65536 * imageheight / 2);//make the rotation coordinates at the center

	EVE_CoCmd_rotate(s_pHalContext, 45 * 65536 / 360);//rotate by 45 degrees clock wise
	EVE_CoCmd_scale(s_pHalContext, 65536 / 2, 65536 / 4);//scale by .5x.25
	EVE_CoCmd_translate(s_pHalContext, -65536 * imagewidth / 2, -65536 * imageheight / 2);
	EVE_CoCmd_setMatrix(s_pHalContext);
	EVE_Cmd_wr32(s_pHalContext, VERTEX2F(imagexoffset, imageyoffset));

	/* Perform display of plain bitmap with scale factor of .5x2 in x & y direction, rotate by 75 degrees anticlock wise wrt center of the image */
	imagexoffset += (imagewidth + 10) * 16;
	EVE_CoCmd_loadIdentity(s_pHalContext);
	EVE_CoCmd_translate(s_pHalContext, 65536 * imagewidth / 2, 65536 * imageheight / 2);//make the rotation coordinates at the center
	EVE_CoCmd_rotate(s_pHalContext, -75 * 65536 / 360);//rotate by 75 degrees anticlock wise
	EVE_CoCmd_scale(s_pHalContext, 65536 / 2, 2 * 65536);//scale by .5x2
	EVE_CoCmd_translate(s_pHalContext, -65536 * imagewidth / 2, -65536 * imageheight / 8);
	EVE_CoCmd_setMatrix(s_pHalContext);
	EVE_Cmd_wr32(s_pHalContext, BITMAP_SIZE(BILINEAR, BORDER, BORDER, imagewidth * 5 / 2, imageheight * 5 / 2));
	EVE_Cmd_wr32(s_pHalContext, VERTEX2F(imagexoffset, imageyoffset));
	/******************************************* Perform display of bitmap flip ************************************/
	/* perform display of plain bitmap with 1x1 and flip right */
	imagexoffset = (10) * 16;
	imageyoffset += (imageheight * 2 + 20) * 16;
	EVE_CoCmd_loadIdentity(s_pHalContext);
	EVE_CoCmd_translate(s_pHalContext, 65536 * imagewidth / 2, 65536 * imageheight / 2);//make the rotation coordinates at the center
	EVE_CoCmd_scale(s_pHalContext, -1 * 65536, 1 * 65536);//flip right
	EVE_CoCmd_translate(s_pHalContext, -65536 * imagewidth / 2, -65536 * imageheight / 2);
	EVE_CoCmd_setMatrix(s_pHalContext);
	EVE_Cmd_wr32(s_pHalContext, BITMAP_SIZE(BILINEAR, BORDER, BORDER, imagewidth, imageheight));
	EVE_Cmd_wr32(s_pHalContext, VERTEX2F(imagexoffset, imageyoffset));

	/* Perform display of plain bitmap with 2x2 scaling, flip bottom */
	imagexoffset += (imagewidth + 10) * 16;
	EVE_CoCmd_loadIdentity(s_pHalContext);
	EVE_CoCmd_translate(s_pHalContext, 65536 * imagewidth / 2, 65536 * imageheight / 2);//make the rotation coordinates at the center
	EVE_CoCmd_scale(s_pHalContext, 2 * 65536, -2 * 65536);//flip bottom and scale by 2 on both sides
	EVE_CoCmd_translate(s_pHalContext, -65536 * imagewidth / 4, (int32_t)(-65536 * imageheight / 1.42));
	EVE_CoCmd_setMatrix(s_pHalContext);
	EVE_Cmd_wr32(s_pHalContext, BITMAP_SIZE(BILINEAR, BORDER, BORDER, imagewidth * 4, imageheight * 4));
	EVE_Cmd_wr32(s_pHalContext, VERTEX2F(imagexoffset, imageyoffset));

	/* Perform display of plain bitmap with 2x1 scaling, rotation and flip right and make sure whole image is viewable */
	imagexoffset += (imagewidth * 2 + 10) * 16;
	EVE_CoCmd_loadIdentity(s_pHalContext);
	EVE_CoCmd_translate(s_pHalContext, 65536 * imagewidth / 2, 65536 * imageheight / 2);//make the rotation coordinates at the center

	EVE_CoCmd_rotate(s_pHalContext, -45 * 65536 / 360);//rotate by 45 degrees anticlock wise
	EVE_CoCmd_scale(s_pHalContext, -2 * 65536, 1 * 65536);//flip right and scale by 2 on x axis
	EVE_CoCmd_translate(s_pHalContext, -65536 * imagewidth / 2, -65536 * imageheight / 8);
	EVE_CoCmd_setMatrix(s_pHalContext);
	EVE_Cmd_wr32(s_pHalContext, BITMAP_SIZE(BILINEAR, BORDER, BORDER, (imagewidth * 5), (imageheight * 5)));
	EVE_Cmd_wr32(s_pHalContext, VERTEX2F(imagexoffset, imageyoffset));

	EVE_Cmd_wr32(s_pHalContext, END());
	EVE_Cmd_wr32(s_pHalContext, DISPLAY());
	EVE_CoCmd_swap(s_pHalContext);

	/* Wait till coprocessor completes the operation */
	EVE_Cmd_waitFlush(s_pHalContext);
	SAMAPP_ENABLE_DELAY_VALUE(2000);
}

/**
 * @brief Sample app api to demonstrate track widget funtionality
 *
 */
void SAMAPP_CoPro_Track()
{
	/*************************************************************************/
	/* Below code demonstrates the usage of track function. Track function   */
	/* tracks the pen touch on any specific object. Track function supports  */
	/* rotary and horizontal/vertical tracks. Rotary is given by rotation    */
	/* angle and horizontal/vertucal track is offset position.               */
	/*************************************************************************/
	int32_t LoopFlag = 0;
	uint32_t TrackRegisterVal = 0;
	uint16_t angleval = 0;
	uint16_t slideval = 0;
	uint16_t scrollval = 0;

	/* Set the tracker for 3 bojects */

	EVE_CoCmd_track(s_pHalContext, (int16_t)(s_pHalContext->Width / 2), (int16_t)(s_pHalContext->Height / 2), 1, 1, 10);
	EVE_CoCmd_track(s_pHalContext, 40, (int16_t)(s_pHalContext->Height - 40), (int16_t)(s_pHalContext->Width - 80), 8, 11);
	EVE_CoCmd_track(s_pHalContext, (int16_t)(s_pHalContext->Width - 40), 40, 8, (int16_t)(s_pHalContext->Height - 80), 12);
	/* Wait till coprocessor completes the operation */
	EVE_Cmd_waitFlush(s_pHalContext);

	LoopFlag = 600;
	/* update the background color continuously for the color change in any of the trackers */
	while (LoopFlag--)
	{
		uint8_t tagval = 0;
		TrackRegisterVal = EVE_Hal_rd32(s_pHalContext, REG_TRACKER);
		tagval = TrackRegisterVal & 0xff;

		if (10 == tagval)
		{
			angleval = TrackRegisterVal >> 16;
		}
		else if (11 == tagval)
		{
			slideval = TrackRegisterVal >> 16;
		}
		else if (12 == tagval)
		{
			scrollval = TrackRegisterVal >> 16;
			if ((scrollval + 65535 / 10) > (9 * 65535 / 10))
			{
				scrollval = (8 * 65535 / 10);
			}
			else if (scrollval < (1 * 65535 / 10))
			{
				scrollval = 0;
			}
			else
			{
				scrollval -= (1 * 65535 / 10);
			}
		}

		/* Display a rotary dial, horizontal slider and vertical scroll */

		EVE_Cmd_wr32(s_pHalContext, CMD_DLSTART);

		int32_t tmpval0;
		int32_t tmpval1;
		int32_t tmpval2;
		uint8_t angval;
		uint8_t sldval;
		uint8_t scrlval;

		tmpval0 = (int32_t)angleval * 255 / 65536 ;
		tmpval1 = (int32_t)slideval * 255 / 65536 ;
		tmpval2 = (int32_t)scrollval * 255 / 65536;

		angval = tmpval0 & 0xff;
		sldval = tmpval1 & 0xff;
		scrlval = tmpval2 & 0xff;

		EVE_Cmd_wr32(s_pHalContext, CLEAR_COLOR_RGB(angval, sldval, scrlval));
		EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1));
		EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0xff, 0xff, 0xff));

		/* Draw dial with 3d effect */
		EVE_CoCmd_fgColor(s_pHalContext, 0x00ff00);
		EVE_CoCmd_bgColor(s_pHalContext, 0x800000);
		EVE_Cmd_wr32(s_pHalContext, TAG(10));
		EVE_CoCmd_dial(s_pHalContext, (int16_t)(s_pHalContext->Width / 2), (int16_t)(s_pHalContext->Height / 2), (int16_t)(s_pHalContext->Width / 8), 0, angleval);

		/* Draw slider with 3d effect */
		EVE_CoCmd_fgColor(s_pHalContext, 0x00a000);
		EVE_CoCmd_bgColor(s_pHalContext, 0x800000);
		EVE_Cmd_wr32(s_pHalContext, TAG(11));
		EVE_CoCmd_slider(s_pHalContext, 40, (int16_t)(s_pHalContext->Height - 40), (int16_t)(s_pHalContext->Width - 80), 8, 0, slideval, 65535);

		/* Draw scroll with 3d effect */
		EVE_CoCmd_fgColor(s_pHalContext, 0x00a000);
		EVE_CoCmd_bgColor(s_pHalContext, 0x000080);
		EVE_Cmd_wr32(s_pHalContext, TAG(12));
		EVE_CoCmd_scrollbar(s_pHalContext, (int16_t)(s_pHalContext->Width - 40), 40, 8, (int16_t)(s_pHalContext->Height - 80), 0, scrollval, (uint16_t)(65535 * 0.2), 65535);

		EVE_CoCmd_fgColor(s_pHalContext, TAG_MASK(0));
		EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0xff, 0xff, 0xff));
		EVE_CoCmd_text(s_pHalContext, (int16_t)(s_pHalContext->Width / 2), (int16_t)((s_pHalContext->Height / 2) + (s_pHalContext->Width / 8) + 8), 26, OPT_CENTER, "Rotary track");
		EVE_CoCmd_text(s_pHalContext, (int16_t)(s_pHalContext->Width / 2), (int16_t)(s_pHalContext->Height - 40 + 8 + 8), 26, OPT_CENTER, "Horizontal track");
		EVE_CoCmd_text(s_pHalContext, (int16_t)(s_pHalContext->Width - 50), 20, 26, OPT_CENTER, "Vertical track");

		EVE_Cmd_wr32(s_pHalContext, DISPLAY());
		EVE_CoCmd_swap(s_pHalContext);

		/* Wait till coprocessor completes the operation */
		EVE_Cmd_waitFlush(s_pHalContext);

		EVE_sleep(10);
	}

	/* Set the tracker for 3 bojects */

	EVE_CoCmd_track(s_pHalContext, 240, 136, 0, 0, 10);
	EVE_CoCmd_track(s_pHalContext, 40, 232, 0, 0, 11);
	EVE_CoCmd_track(s_pHalContext, 400, 40, 0, 0, 12);

	/* Wait till coprocessor completes the operation */
	EVE_Cmd_waitFlush(s_pHalContext);
}

PROGMEM prog_uchar8_t SAMAPP_Snd_Array[5 * 58] = "Slce\0Sqrq\0Sinw\0Saww\0Triw\0Beep\0Alrm\0Warb\0Crsl\0Pp01\0Pp02\0Pp03\0Pp04\0Pp05\0Pp06\0Pp07\0Pp08\0Pp09\0Pp10\0Pp11\0Pp12\0Pp13\0Pp14\0Pp15\0Pp16\0DMF#\0DMF*\0DMF0\0DMF1\0DMF2\0DMF3\0DMF4\0DMF5\0DMF6\0DMF7\0DMF8\0DMF9\0Harp\0Xyph\0Tuba\0Glok\0Orgn\0Trmp\0Pian\0Chim\0MBox\0Bell\0Clck\0Swth\0Cowb\0Noth\0Hiht\0Kick\0Pop\0Clak\0Chak\0Mute\0uMut\0";

PROGMEM prog_uchar8_t SAMAPP_Snd_TagArray[58] = {
	0x63,0x1,0x2,0x3,0x4,0x5,0x6,0x7,0x8,
	0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1a,0x1b,0x1c,0x1d,0x1e,0x1f,
	0x23,0x2a,
	0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,
	0x40,0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,
	0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,
	0x60,0x61
};

void highLight_Btn(int32_t tagvalsnd, const prog_uchar8_t* pTagArray,
	int32_t wbutton, int32_t hbutton, 
	const char8_t* StringArray, const prog_uchar8_t* pString) {

	int32_t numbtnrow = 7;
	int32_t numbtncol = 8;

	for (int i = 0; i < numbtnrow; i++)
	{
		for (int j = 0; j < numbtncol; j++)
		{
			EVE_Cmd_wr32(s_pHalContext, TAG(pgm_read_byte(pTagArray)));
			if (tagvalsnd == pgm_read_byte(pTagArray))
			{
				/* red color for highlight effect */
				EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0x80, 0x00, 0x00));
				EVE_Cmd_wr32(s_pHalContext, BEGIN(RECTS));
				EVE_Cmd_wr32(s_pHalContext, TAG(pgm_read_byte(pTagArray)));
				EVE_Cmd_wr32(s_pHalContext, VERTEX2F((j * wbutton + 2) * 16, (hbutton * i + 2) * 16));
				EVE_Cmd_wr32(s_pHalContext, VERTEX2F(((j * wbutton) + wbutton - 2) * 16, ((hbutton * i) + hbutton - 2) * 16));
				EVE_Cmd_wr32(s_pHalContext, END());
				/* reset the color to make sure font doesnt get impacted */
				EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0xff, 0xff, 0xff));
			}

			/* to make sure that highlight rectangle as well as font to take the same tag values */
#ifdef ARDUINO_PLATFORM
			strcpy_P(StringArray, (const prog_char8_t*)pString);
#endif
#if defined(FT900_PLATFORM) || defined(FT93X_PLATFORM)
			{
				/* Copy data from flash to RAM */
				int32_t idx = 0;
				int32_t slen;
				char8_t* pstring, tempchar;
				pstring = StringArray;
				do
				{
					tempchar = pgm_read_byte_near(&pString[idx]);
					*pstring++ = tempchar;
					idx++;
				} while (tempchar);
			}
#endif
#if defined(MSVC_PLATFORM) || defined(BT8XXEMU_PLATFORM)
			strcpy(StringArray, pString);
#endif
			EVE_CoCmd_text(s_pHalContext, (int16_t)((wbutton / 2) + j * wbutton),
				(int16_t)((hbutton / 2) + hbutton * i), 26, OPT_CENTER, StringArray);

			pString += (strlen(StringArray) + 1);
			pTagArray++;
		}
	}
}

void draw_Btn(const prog_uchar8_t* pTagArray, int32_t wbutton, int32_t hbutton) {
	int32_t numbtnrow = 7;
	int32_t numbtncol = 8;

	for (int i = 0; i < numbtnrow; i++)
	{
		for (int j = 0; j < numbtncol; j++)
		{
			EVE_Cmd_wr32(s_pHalContext, TAG(pgm_read_byte(pTagArray)));
			EVE_Cmd_wr32(s_pHalContext, VERTEX2F((j * wbutton + 2) * 16, (hbutton * i + 2) * 16));
			EVE_Cmd_wr32(s_pHalContext, VERTEX2F(((j * wbutton) + wbutton - 2) * 16, ((hbutton * i) + hbutton - 2) * 16));
			pTagArray++;
		}
	}
}

/**
 * @brief Sample app api to demonstrate sound
 *
 */
void SAMAPP_Sound()
{
	int32_t LoopFlag = 100;
	int32_t wbutton;
	int32_t hbutton;
	int32_t tagval;
	int32_t tagvalsnd = -1;
	int32_t numbtnrow;
	int32_t numbtncol;
	int32_t prevtag = -1;
	uint32_t freqtrack = 0;
	uint32_t currfreq = 0;
	uint32_t prevcurrfreq;
	const PROGMEM prog_uchar8_t* pString;
	const PROGMEM prog_uchar8_t* pTagArray;
	char8_t StringArray[8] = { 0 };
	/*************************************************************************/
	/* Below code demonstrates the usage of sound function. All the supported*/
	/* sounds and respective pitches are put as part of keys/buttons, by     */
	/* choosing particular key/button the sound is played                    */
	/*************************************************************************/
	numbtnrow = 7;//number of rows to be created - note that mute and unmute are not played in this application
	numbtncol = 8;//number of colomns to be created
	wbutton = (s_pHalContext->Width - 40) / numbtncol;
	hbutton = s_pHalContext->Height / numbtnrow;

	/* set the volume to maximum */
	EVE_Hal_wr8(s_pHalContext, REG_VOL_SOUND, 0xFF);
	/* set the tracker to track the slider for frequency */

	EVE_CoCmd_track(s_pHalContext, (int16_t)(s_pHalContext->Width - 15), 20, 8, (int16_t)(s_pHalContext->Height - 40), 100);
	EVE_Cmd_waitFlush(s_pHalContext);
	while (LoopFlag--)
	{
		tagval = EVE_Hal_rd8(s_pHalContext, REG_TOUCH_TAG);
		freqtrack = EVE_Hal_rd32(s_pHalContext, REG_TRACKER);

		if (100 == (freqtrack & 0xff))
		{
			currfreq = (freqtrack >> 16);
			currfreq = (88 * currfreq) / 65536;
			if (currfreq > 108)
				currfreq = 108;
		}
		if (tagval > 0)
		{
			if (tagval <= 99)
			{
				tagvalsnd = tagval;
			}
			if (0x63 == tagvalsnd)
			{
				tagvalsnd = 0;
			}
			if ((prevtag != tagval) || (prevcurrfreq != currfreq))
			{
				/* Play sound wrt pitch */
				EVE_Hal_wr16(s_pHalContext, REG_SOUND, (int16_t)(((currfreq + 21) << 8) | tagvalsnd));
				EVE_Hal_wr8(s_pHalContext, REG_PLAY, 1);
			}
			if (0 == tagvalsnd)
				tagvalsnd = 99;
		}
		/* start a new display list for construction of screen */

		EVE_CoCmd_dlStart(s_pHalContext);
		EVE_Cmd_wr32(s_pHalContext, CLEAR_COLOR_RGB(64, 64, 64));
		EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1));
		/* line width for the rectangle */
		EVE_Cmd_wr32(s_pHalContext, LINE_WIDTH(1 * 16));

		/* custom keys for sound input */
		pTagArray = SAMAPP_Snd_TagArray;
		/* First draw all the rectangles followed by the font */
		/* yellow color for background color */
		EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0x80, 0x80, 0x00));
		EVE_Cmd_wr32(s_pHalContext, BEGIN(RECTS));
		
		draw_Btn(pTagArray, wbutton, hbutton);

		EVE_Cmd_wr32(s_pHalContext, END());
		EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0xff, 0xff, 0xff));
		/* draw the highlight rectangle and text info */
		pTagArray = SAMAPP_Snd_TagArray;
		pString = SAMAPP_Snd_Array;

		highLight_Btn(tagvalsnd, pTagArray, wbutton, hbutton, StringArray, pString);

		/* Draw vertical slider bar for frequency control */
		StringArray[0] = '\0';
		strcat(StringArray, "Pt ");
		Gpu_Hal_Dec2Ascii(StringArray, (int32_t)(currfreq + 21));
		EVE_Cmd_wr32(s_pHalContext, TAG_MASK(0));
		EVE_CoCmd_text(s_pHalContext, (int16_t)(s_pHalContext->Width - 20), 10, 26, OPT_CENTER, StringArray);
		EVE_Cmd_wr32(s_pHalContext, TAG_MASK(1));
		EVE_Cmd_wr32(s_pHalContext, TAG(100));
		EVE_CoCmd_slider(s_pHalContext, (int16_t)(s_pHalContext->Width - 15), 20, 8, (int16_t)(s_pHalContext->Height - 40), 0, (int16_t)currfreq, 88);

		EVE_Cmd_wr32(s_pHalContext, DISPLAY());
		EVE_CoCmd_swap(s_pHalContext);
		prevtag = tagval;

		prevcurrfreq = currfreq;
		/* Wait till coprocessor completes the operation */
		EVE_Cmd_waitFlush(s_pHalContext);
		EVE_sleep(10);
	}

	EVE_Hal_wr16(s_pHalContext, REG_SOUND, 0);
	EVE_Hal_wr8(s_pHalContext, REG_PLAY, 1);
}

#if defined(MSVC_PLATFORM) || defined(BT8XXEMU_PLATFORM)
/**
 * @brief API to demonstrate music playback in streaming way
 *
 */
void SAMAPP_Aud_Music_Player_Streaming()
{
	FILE* pFile = NULL;
	uint32_t filesz = 0;
	uint32_t chunksize = 16 * 1024;
	uint32_t totalbufflen = 64 * 1024;
	uint32_t currreadlen = 0;
	const uint8_t* pBuff = NULL;
	const uint8_t* music_playing = 0;
	uint32_t wrptr = RAM_G;
	uint32_t rdptr;
	uint32_t freebuffspace;
	pFile = fopen(TEST_DIR "\\Devil_Ride_30_11025hz.raw", "rb+");
	if (!pFile) {
		return;
	}
	fseek(pFile, 0, SEEK_END);
	filesz = ftell(pFile);

	fseek(pFile, 0, SEEK_SET);

	pBuff = (uint8_t*)malloc(totalbufflen);

	EVE_CoCmd_dlStart(s_pHalContext);
	EVE_Cmd_wr32(s_pHalContext, CLEAR_COLOR_RGB(0xff, 0xff, 0xff));//set the background to white
	EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1));
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(32, 32, 32));//black color text

	EVE_CoCmd_text(s_pHalContext, (int16_t)(s_pHalContext->Width / 2), (int16_t)(s_pHalContext->Height / 2), 27, OPT_CENTER, "Now you will hear the music");
	EVE_Cmd_wr32(s_pHalContext, DISPLAY());
	/* Wait till coprocessor completes the operation */
	EVE_Cmd_waitFlush(s_pHalContext);
	GPU_DLSwap(s_pHalContext, DLSWAP_FRAME);

	while (filesz > 0)
	{
		currreadlen = filesz;

		if (currreadlen > chunksize)
		{
			currreadlen = chunksize;
		}
		fread(pBuff, 1, currreadlen, pFile);

		EVE_Hal_wrProgMem(s_pHalContext, wrptr, pBuff, currreadlen);
		wrptr += currreadlen;
		wrptr = wrptr % (RAM_G + totalbufflen);

		filesz -= currreadlen;

		if (music_playing == 0)
		{
			EVE_Hal_wr32(s_pHalContext, REG_PLAYBACK_START, RAM_G);//Audio playback start address
			EVE_Hal_wr32(s_pHalContext, REG_PLAYBACK_LENGTH, totalbufflen);//Length of raw data buffer in bytes
			EVE_Hal_wr16(s_pHalContext, REG_PLAYBACK_FREQ, 11025);//Frequency
			EVE_Hal_wr8(s_pHalContext, REG_PLAYBACK_FORMAT, LINEAR_SAMPLES);//Current sampling frequency
			EVE_Hal_wr8(s_pHalContext, REG_PLAYBACK_LOOP, 1);
			EVE_Hal_wr8(s_pHalContext, REG_VOL_PB, 255);
			EVE_Hal_wr8(s_pHalContext, REG_PLAYBACK_PLAY, 1);
			music_playing = 1;
		}

		//Check the freespace if the file has more
		do
		{
			uint32_t fullness = 0;
			rdptr = EVE_Hal_rd32(s_pHalContext, REG_PLAYBACK_READPTR);
			fullness = (wrptr - rdptr) % totalbufflen;
			freebuffspace = totalbufflen - fullness;
		} while (freebuffspace < chunksize);

		//if the file is sent over and there is one more chunk size free space.
		if (filesz == 0)
		{
			//Clear the chunksize to make mute sound.
			EVE_CoCmd_memSet(s_pHalContext, wrptr, 0, chunksize);
			EVE_Cmd_waitFlush(s_pHalContext);
		}
	}

	//if read pointer is already passed over write pointer
	if (EVE_Hal_rd32(s_pHalContext, REG_PLAYBACK_READPTR) > wrptr) {
		//wait till the read pointer will be wrapped over
		while (EVE_Hal_rd32(s_pHalContext, REG_PLAYBACK_READPTR) > wrptr);
	}

	//wait till read pointer pass through write pointer
	while (EVE_Hal_rd32(s_pHalContext, REG_PLAYBACK_READPTR) < wrptr);

	//The file is done , mute the sound first.
	EVE_Hal_wr8(s_pHalContext, REG_PLAYBACK_LOOP, 0);
	EVE_Hal_wr32(s_pHalContext, REG_PLAYBACK_LENGTH, 0);//Length of raw data buffer in bytes
	EVE_Hal_wr8(s_pHalContext, REG_VOL_PB, 0);
	EVE_Hal_wr8(s_pHalContext, REG_PLAYBACK_PLAY, 1);

	fclose(pFile);
	free(pBuff);
}
#endif
#if defined(MSVC_PLATFORM) || defined(BT8XXEMU_PLATFORM)
/**
 * @brief API to demonstrate music playback
 *
 */
void SAMAPP_Aud_Music_Player()
{
	FILE* pFile = NULL;
	int32_t filesz;
	int32_t freebuffspace = 100 * 1024;
	int32_t chunksize = 10 * 1024;
	int32_t totalbufflen = 100 * 1024;
	int32_t currreadlen;
	uint8_t* pBuff = NULL;
	const uint8_t* loopflag = 1;
	pFile = fopen(TEST_DIR "\\Devil_Ride_30_11025hz.raw", "rb+");
	if (!pFile) {
		return;
	}
	fseek(pFile, 0, SEEK_END);
	filesz = ftell(pFile);

	fseek(pFile, 0, SEEK_SET);

	/* Ideally allocate memory wrt sampling frequency and amount of buffering */
	pBuff = (uint8_t*)malloc(chunksize);

	while (filesz > 0)
	{
		currreadlen = filesz;

		if (currreadlen > chunksize)
		{
			currreadlen = chunksize;
		}
		fread(pBuff, 1, currreadlen, pFile);
		EVE_Hal_wrProgMem(s_pHalContext, RAM_G + (totalbufflen - freebuffspace), pBuff, chunksize);
		filesz -= chunksize;
		freebuffspace -= chunksize;
		if (freebuffspace <= 0)
		{
			break;
		}
	}

	/* check if single shot or not */
	if (filesz <= 0)
	{
		loopflag = 0;

	}
	EVE_Hal_wr32(s_pHalContext, REG_PLAYBACK_START, RAM_G);//Audio playback start address in sram
	EVE_Hal_wr32(s_pHalContext, REG_PLAYBACK_LENGTH, (totalbufflen - freebuffspace));//Length of raw data buffer in bytes
	EVE_Hal_wr16(s_pHalContext, REG_PLAYBACK_FREQ, 11025);//Current read address
	EVE_Hal_wr8(s_pHalContext, REG_PLAYBACK_FORMAT, LINEAR_SAMPLES);//Current sampling frequency
	EVE_Hal_wr8(s_pHalContext, REG_PLAYBACK_LOOP, 0);//Audio playback format
	EVE_Hal_wr8(s_pHalContext, REG_VOL_PB, 255);
	EVE_Hal_wr8(s_pHalContext, REG_PLAYBACK_PLAY, 1);

	fclose(pFile);
	free(pBuff);
}
#endif

/**
 * @brief Draw set 4
 *
 */
void App_Set_04_Draw() {
	/* Sample code for coprocessor widgets */
	SAMAPP_CoPro_Widget_Text();
	SAMAPP_CoPro_Widget_Number();
	SAMAPP_CoPro_Widget_Button();
	SAMAPP_CoPro_AppendCmds();

	EVE_Hal_wr8(s_pHalContext, REG_VOL_SOUND, 255);
	SAMAPP_Sound();
	SAMAPP_CoPro_Screensaver();
	SAMAPP_CoPro_Snapshot();
	SAMAPP_CoPro_Sketch();

	SAMAPP_CoPro_Matrix();
	SAMAPP_CoPro_Track();

#if defined(MSVC_PLATFORM) || defined(BT8XXEMU_PLATFORM)
	//Enable Audio out by setting GPIO
	EVE_Hal_wr8(s_pHalContext, REG_GPIO, 0x083 | EVE_Hal_rd8(s_pHalContext, REG_GPIO));
	/* Audio playback api*/
	EVE_Hal_wr8(s_pHalContext, REG_VOL_SOUND, 255);
	SAMAPP_Aud_Music_Player_Streaming();
	SAMAPP_Aud_Music_Player();
#endif
}

/**
 * @brief Init set 4
 *
 */
void App_Set_04_Init(EVE_HalContext* pHalContext) {
	s_pHalContext = pHalContext;
	Draw_Text(s_pHalContext, "Set4   START");
}

/**
 * @brief Deinit set 4
 *
 */
void App_Set_04_Deinit() {
	Draw_Text(s_pHalContext, "Set4   END!");
}

#endif // SAMAPP_ENABLE_APIS_SET4
