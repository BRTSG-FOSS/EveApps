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

#define INDICATOR_FIELD_HANDLE    1
#define GLOWIMAGE_HANDLE          2
#define INDICATOR_BAR_HANDLE      3
#define INDICATOR_FRAME_HANDLE    4

#define PALETTED_LUT              (2048)

static uint32_t paletted8_source = RAM_G;

void drawIndicator() {
	static uint8_t  veh_phase = 0;
	/*
	* Draw border of the running bar
	*/
	{
		// set blue color for border bitmap
		EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0x06, 0x32, 0x47));

		// draw bitmap
		EVE_Cmd_wr32(s_pHalContext, BEGIN(BITMAPS));
		EVE_Cmd_wr32(s_pHalContext, VERTEX2II(20, 32, INDICATOR_FRAME_HANDLE, 0));
	}

	/*
	* Draw transparent scissor clip
	*/
	{
		// backup current graphics context
		EVE_Cmd_wr32(s_pHalContext, SAVE_CONTEXT());

		// only draw alpha part of the raw image - to make scissor clip transparent
		EVE_Cmd_wr32(s_pHalContext, COLOR_MASK(0, 0, 0, 1));

		// pixel_color_out = source * 1 + destination * 0: set scissor clip, clear background
		EVE_Cmd_wr32(s_pHalContext, BLEND_FUNC(ONE, ZERO));

		// draw bitmap
		EVE_Cmd_wr32(s_pHalContext, BEGIN(BITMAPS));
		EVE_Cmd_wr32(s_pHalContext, VERTEX2II(15, 29, INDICATOR_BAR_HANDLE, 0));
		EVE_Cmd_wr32(s_pHalContext, RESTORE_CONTEXT());
	}
	/*
	* draw running green column on the scissor clip
	*/
	{
		// set green color for running column
		EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0x76, 0xff, 0x35));

		// pixel_color_out = source * DST_ALPHA + destination * (1 - DST_ALPHA): 
		// ->if destination alpha ==  1: draw green column, else draw dest
		EVE_Cmd_wr32(s_pHalContext, BLEND_FUNC(DST_ALPHA, ONE_MINUS_DST_ALPHA));

		// Move PALETTE_SOURCE offset: to make the column move
		EVE_Cmd_wr32(s_pHalContext, PALETTE_SOURCE(paletted8_source + 4 * veh_phase));

		// draw the bitmap
		EVE_Cmd_wr32(s_pHalContext, BEGIN(BITMAPS));
		EVE_Cmd_wr32(s_pHalContext, VERTEX2II(20, 28, INDICATOR_FIELD_HANDLE, 0));
	}

	// increase offset of running green column
	veh_phase += 1; // veh_phase  is 8-bit, if veh_phase > 2^8, veh_phase = 0
}

void setupBitmapHandles() {
	EVE_Cmd_wr32(s_pHalContext, CMD_DLSTART);
	uint32_t ram_image_addr = RAM_G;

	{
		/*Indicator field*/
		uint16_t iw = 296;
		uint16_t ih = 1;
		uint16_t format = L8;
		Gpu_Hal_LoadImageToMemory(s_pHalContext, TEST_DIR "\\indicator_field_296x1_L8.raw", ram_image_addr, LOAD);
		EVE_Cmd_wr32(s_pHalContext, BITMAP_HANDLE(INDICATOR_FIELD_HANDLE));
		EVE_CoCmd_setBitmap(s_pHalContext, ram_image_addr, format, iw, ih);
		ram_image_addr += iw * ih;

		/*paletted8*/
		paletted8_source = ram_image_addr;

		Gpu_Hal_LoadImageToMemory(s_pHalContext, TEST_DIR "\\indicator_field_NewPaletted8.raw", ram_image_addr, LOAD);
		EVE_Cmd_wr32(s_pHalContext, BITMAP_LAYOUT(PALETTED8, iw, ih));
		EVE_Cmd_wr32(s_pHalContext, BITMAP_LAYOUT_H(0, 0));
		EVE_Cmd_wr32(s_pHalContext, BITMAP_SIZE(NEAREST, BORDER, REPEAT, iw, 57));
		EVE_Cmd_wr32(s_pHalContext, BITMAP_SIZE_H(0, 0));
		ram_image_addr += PALETTED_LUT;
	}

	{
		uint16_t iw = 310;
		uint16_t ih = 57;
		Gpu_Hal_LoadImageToMemory(s_pHalContext, TEST_DIR "\\indicator_bar_310x57_L4.raw", ram_image_addr, LOAD);
		EVE_Cmd_wr32(s_pHalContext, BITMAP_HANDLE(INDICATOR_BAR_HANDLE));
		EVE_CoCmd_setBitmap(s_pHalContext, ram_image_addr, L4, iw, ih);

		ram_image_addr += (iw * ih) / 2;
	}

	{
		uint16_t iw = 304;
		uint16_t ih = 53;
		Gpu_Hal_LoadImageToMemory(s_pHalContext, TEST_DIR "\\indicator_frame_304x53_L4.raw", ram_image_addr, LOAD);
		EVE_Cmd_wr32(s_pHalContext, BITMAP_HANDLE(INDICATOR_FRAME_HANDLE));
		EVE_CoCmd_setBitmap(s_pHalContext, ram_image_addr, L4, iw, ih);
		ram_image_addr += (iw * ih) / 2;
	}
	EVE_Cmd_wr32(s_pHalContext, CMD_SWAP);
	EVE_Cmd_waitFlush(s_pHalContext);
}

void DemoRunningBar(EVE_HalContext* pHalContext) {
#if EVE_CHIPID < EVE_BT815
#error Platform not supported
#endif
	
	s_pHalContext = pHalContext;
	// init bitmaps
	setupBitmapHandles();

	while (1) {
		Display_Start(s_pHalContext);
		drawIndicator();
		Display_End(s_pHalContext);
	}
}

