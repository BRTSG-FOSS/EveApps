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
  *     > Rotate by RotateAround
  *     > Rotate by Rotate and Translate
  */

#include "Platform.h"
#include "Gpu_Hal.h"
#include "Common.h"
#include "App.h"
#include "Samapp_Enable_API.h"

#ifdef SAMAPP_ENABLE_APIS_SET12

static EVE_HalContext *s_pHalContext;

/**
 * @brief Load image to RAM_G
 *
 */
static void set12_loadImage() {
	uint32_t fileSize = 0;
	const uint32_t bytePerTrans = 1000;
	uint8_t buff[1000];

	const uint8_t* image = TEST_DIR "\\mandrill256.jpg";
	fileSize = FileIO_File_Open(image, FILEIO_E_FOPEN_READ);

	/******************* Decode jpg output into location 0 and output color format as RGB565 *********************/
	EVE_Cmd_wr32(s_pHalContext, CMD_LOADIMAGE);
	EVE_Cmd_wr32(s_pHalContext, 0);//destination address of jpg decode
	EVE_Cmd_wr32(s_pHalContext, 0);//output format of the bitmap

	while (fileSize > 0) {
		/* copy the data into pbuff and then transfter it to command buffer */
		int bytes = FileIO_File_Read(buff, bytePerTrans);
		EVE_Cmd_wrMem(s_pHalContext, buff, bytes);
		fileSize -= bytes;
	}
	/* close the opened jpg file */
	FileIO_File_Close();
}

/**
 * @brief Rotate image by CMD_ROTATEAROUND
 *
 * @param address Address of image
 * @param format Image format
 * @param x Image X
 * @param y Image Y
 * @param w Image W
 * @param h Image H
 * @param rotation_angle Rotate angle
 */
static void set12_rotateAroundOne(uint32_t address, uint32_t format, uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t rotation_angle) {
	int16_t lw;
    int16_t lh;
	const uint32_t TRANSLATE_XY = 100;
	const uint32_t precision = 16;

	EVE_Cmd_wr32(s_pHalContext, SAVE_CONTEXT());

	EVE_Cmd_wr32(s_pHalContext, BEGIN(BITMAPS));
	EVE_CoCmd_setBitmap(s_pHalContext, (int16_t)address, (int16_t)format, (int16_t)w, (int16_t)h);

	lw = (int16_t)(w + TRANSLATE_XY);
	lh = (int16_t)(h + TRANSLATE_XY);

	EVE_Cmd_wr32(s_pHalContext, BITMAP_SIZE(NEAREST, BORDER, BORDER, lw, lh));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_SIZE_H(lw >> 8, lh >> 8));

	EVE_CoCmd_loadIdentity(s_pHalContext);
	EVE_CoCmd_translate(s_pHalContext, TRANSLATE_XY * MAX_CIRCLE_UNIT, TRANSLATE_XY * MAX_CIRCLE_UNIT);
	EVE_CoCmd_rotateAround(s_pHalContext, w / 2, h / 2, rotation_angle * MAX_CIRCLE_UNIT / MAX_ANGLE, MAX_CIRCLE_UNIT);
	EVE_CoCmd_setMatrix(s_pHalContext);

	EVE_Cmd_wr32(s_pHalContext, VERTEX2F(x * precision, y * precision));

	EVE_Cmd_wr32(s_pHalContext, END());

	EVE_Cmd_wr32(s_pHalContext, RESTORE_CONTEXT());
}

/**
 * @brief Rotate image by CMD_ROTATEA and CMD_TRANSLATE
 *
 * @param address Address of image
 * @param format Image format
 * @param x Image X
 * @param y Image Y
 * @param w Image W
 * @param h Image H
 * @param rotation_angle Rotate angle
 */
static void set12_rotateAndTranslateOne(uint32_t address, uint32_t format, uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t rotation_angle) {
	int16_t lw;
    int16_t lh;
	const uint32_t TRANSLATE_XY = 100;
	const uint32_t precision = 16;

	EVE_Cmd_wr32(s_pHalContext, SAVE_CONTEXT());

	EVE_Cmd_wr32(s_pHalContext, BEGIN(BITMAPS));
	EVE_CoCmd_setBitmap(s_pHalContext, (int16_t)address, (int16_t)format, (int16_t)w, (int16_t)h);

	lw = (int16_t)(w * 2);
	lh = (int16_t)(h * 2);

	EVE_Cmd_wr32(s_pHalContext, BITMAP_SIZE(NEAREST, BORDER, BORDER, lw, lh));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_SIZE_H(lw >> 8, lh >> 8));

	EVE_CoCmd_loadIdentity(s_pHalContext);

	EVE_CoCmd_translate(s_pHalContext, (w / 2 + TRANSLATE_XY) * MAX_CIRCLE_UNIT, (w / 2 + TRANSLATE_XY) * MAX_CIRCLE_UNIT);
	EVE_CoCmd_rotate(s_pHalContext, rotation_angle * MAX_CIRCLE_UNIT / MAX_ANGLE);
	EVE_CoCmd_translate(s_pHalContext, -((int32_t)(w / 2)) * MAX_CIRCLE_UNIT, -((int32_t)(h / 2)) * MAX_CIRCLE_UNIT);

	EVE_CoCmd_setMatrix(s_pHalContext);

	EVE_Cmd_wr32(s_pHalContext, VERTEX2F(x * precision, y * precision));

	EVE_Cmd_wr32(s_pHalContext, END());

	EVE_Cmd_wr32(s_pHalContext, RESTORE_CONTEXT());
}

/**
 * @brief Draw set 12
 *
 */
void App_Set_12_Draw() {
	int count = 0;
	const uint32_t TRANSLATE_XY = 100;

	// tiles
	uint16_t tile_w = 256;
	uint16_t tile_h = 256;
	uint16_t tile1_x = -30;
	uint16_t tile1_y = 50;
	uint16_t tile2_x = (uint16_t)(s_pHalContext->Width - tile1_x - tile_w - 200);
	uint16_t tile2_y = tile1_y;

	set12_loadImage();

	while (count++ < 60 * 10) {// wait 10 seconds, 60 FPS
		/*Display List start*/
		EVE_CoCmd_dlStart(s_pHalContext);
		EVE_Cmd_wr32(s_pHalContext, CLEAR_COLOR_RGB(0, 0, 0));
		EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1));

		static uint16_t rotation_angle = 0;
		static uint16_t font_size = 29;
		static uint16_t option = 0;

		rotation_angle = (rotation_angle + 1) % MAX_ANGLE;

		set12_rotateAroundOne(RAM_G, RGB565, tile1_x, tile1_y, tile_w, tile_h, rotation_angle);
		set12_rotateAndTranslateOne(RAM_G, RGB565, tile2_x, tile2_y, tile_w, tile_h, rotation_angle);

		EVE_CoCmd_text(s_pHalContext, tile1_x + TRANSLATE_XY / 2, tile1_y, font_size, option, "Rotate by RotateAround");
		EVE_CoCmd_text(s_pHalContext, tile2_x + TRANSLATE_XY / 2, tile2_y, font_size, option, "Rotate by Rotate and Translate");

		EVE_Cmd_wr32(s_pHalContext, DISPLAY()); //send command display - to end display commands
		EVE_CoCmd_swap(s_pHalContext); // draw the new screen
		EVE_Cmd_waitFlush(s_pHalContext);// Wait until EVE is free
	}
}

/**
 * @brief Init set 12
 *
 */
void App_Set_12_Init(EVE_HalContext* pHalContext) {
	s_pHalContext = pHalContext;
	Draw_Text(s_pHalContext, "Set12   Start!");
}

/**
 * @brief Deinit set 12
 *
 */
void App_Set_12_Deinit() {
	Draw_Text(s_pHalContext, "Set12   End!");
}

#endif // SAMAPP_ENABLE_APIS_SET12
