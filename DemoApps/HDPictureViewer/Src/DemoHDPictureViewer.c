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

#include <math.h>

#include "Platform.h"
#include "App.h"
#include "Common.h"

#include "DemoHDPictureViewer.h"

typedef struct POS_ {
	uint32_t x;
	uint32_t y;
}POS;

typedef struct Img2 {
    uint32_t index;
    uint32_t addressFlash;
    uint32_t addressRamg;
    uint32_t size;
    uint32_t x;
    uint32_t y;
    uint32_t w;
    uint32_t h;
    uint16_t format;
    uint8_t tag;
    uint8_t isFlash;
}Img2_t;

static EVE_HalContext* s_pHalContext;

int zz = 1;
static POS* App_GetTouchPos() {
	Gesture_Renew(s_pHalContext);
	static POS ret;
	Gesture_Touch_t *mG = Gesture_Get();
	if (zz % 7 == 0) {
		zz = 1;
	}
	zz++;
	if (mG->touchX == 32768)
		mG->touchX = s_pHalContext->Width;
	if (mG->touchY == 32768)
		mG->touchY = s_pHalContext->Height;

	ret.x = s_pHalContext->Width - mG->touchX;
	ret.y = s_pHalContext->Height - mG->touchY;

	return &ret;
}

void App_Draw2_Many_ASTC_1k() {
#define BOX_W                 (800) //1600
#define BOX_H                 (480)
#define IMG_W                 4096
#define IMG_H                 IMG_W
#define IMG_ASTC_W            5
#define IMG_ASTC_H            5
	uint16_t format = COMPRESSED_RGBA_ASTC_4x4_KHR;

#define BOX_SIZE              (16) //ASTC blocks represent between 4x4 to 12x12 pixels. Each block is 16 bytes in memory.
#define ROUNDUP(devide, by)   ((devide + by - 1) / by)
#define ROUNDDOWN(devide, by) ((int)(devide / by))
#define NUM_BOX(width)        (ROUNDUP(width, IMG_ASTC_W))              
#define NUM_BYTE(width)       (ALIGN(NUM_BOX(width) * BOX_SIZE, 64))          

	Img2_t images[] = {
		// index / addressFlash / addressRamg / size / x / y / w / h / format / tag / isFlash
		{ 0, 4096     , 0, 1048576, 0, 0, 1025, 1025, COMPRESSED_RGBA_ASTC_5x5_KHR, 0, 1 }, // image_1_1_1025x1025_COMPRESSED_RGBA_ASTC_5x5_KHR.raw
		{ 0, 676544   , 0, 1048576, 0, 0, 1025, 1025, COMPRESSED_RGBA_ASTC_5x5_KHR, 0, 1 }, // image_1_2_1025x1025_COMPRESSED_RGBA_ASTC_5x5_KHR.raw
		{ 0, 1348992  , 0, 1048576, 0, 0, 1025, 1025, COMPRESSED_RGBA_ASTC_5x5_KHR, 0, 1 }, // image_1_3_1025x1025_COMPRESSED_RGBA_ASTC_5x5_KHR.raw
		{ 0, 2021440  , 0, 1048576, 0, 0, 1025, 1025, COMPRESSED_RGBA_ASTC_5x5_KHR, 0, 1 }, // image_1_4_1025x1025_COMPRESSED_RGBA_ASTC_5x5_KHR.raw
		{ 0, 2693888  , 0, 1048576, 0, 0, 1025, 1025, COMPRESSED_RGBA_ASTC_5x5_KHR, 0, 1 }, // image_2_1_1025x1025_COMPRESSED_RGBA_ASTC_5x5_KHR.raw
		{ 0, 3366336  , 0, 1048576, 0, 0, 1025, 1025, COMPRESSED_RGBA_ASTC_5x5_KHR, 0, 1 }, // image_2_2_1025x1025_COMPRESSED_RGBA_ASTC_5x5_KHR.raw
		{ 0, 4038784  , 0, 1048576, 0, 0, 1025, 1025, COMPRESSED_RGBA_ASTC_5x5_KHR, 0, 1 }, // image_2_3_1025x1025_COMPRESSED_RGBA_ASTC_5x5_KHR.raw
		{ 0, 4711232  , 0, 1048576, 0, 0, 1025, 1025, COMPRESSED_RGBA_ASTC_5x5_KHR, 0, 1 }, // image_2_4_1025x1025_COMPRESSED_RGBA_ASTC_5x5_KHR.raw
		{ 0, 5383680  , 0, 1048576, 0, 0, 1025, 1025, COMPRESSED_RGBA_ASTC_5x5_KHR, 0, 1 }, // image_3_1_1025x1025_COMPRESSED_RGBA_ASTC_5x5_KHR.raw
		{ 0, 6056128  , 0, 1048576, 0, 0, 1025, 1025, COMPRESSED_RGBA_ASTC_5x5_KHR, 0, 1 }, // image_3_2_1025x1025_COMPRESSED_RGBA_ASTC_5x5_KHR.raw
		{ 0, 6728576  , 0, 1048576, 0, 0, 1025, 1025, COMPRESSED_RGBA_ASTC_5x5_KHR, 0, 1 }, // image_3_3_1025x1025_COMPRESSED_RGBA_ASTC_5x5_KHR.raw
		{ 0, 7401024  , 0, 1048576, 0, 0, 1025, 1025, COMPRESSED_RGBA_ASTC_5x5_KHR, 0, 1 }, // image_3_4_1025x1025_COMPRESSED_RGBA_ASTC_5x5_KHR.raw
		{ 0, 8073472  , 0, 1048576, 0, 0, 1025, 1025, COMPRESSED_RGBA_ASTC_5x5_KHR, 0, 1 }, // image_4_1_1025x1025_COMPRESSED_RGBA_ASTC_5x5_KHR.raw
		{ 0, 8745920  , 0, 1048576, 0, 0, 1025, 1025, COMPRESSED_RGBA_ASTC_5x5_KHR, 0, 1 }, // image_4_2_1025x1025_COMPRESSED_RGBA_ASTC_5x5_KHR.raw
		{ 0, 9418368  , 0, 1048576, 0, 0, 1025, 1025, COMPRESSED_RGBA_ASTC_5x5_KHR, 0, 1 }, // image_4_3_1025x1025_COMPRESSED_RGBA_ASTC_5x5_KHR.raw
		{ 0, 10090816 , 0, 1048576, 0, 0, 1025, 1025, COMPRESSED_RGBA_ASTC_5x5_KHR, 0, 1 }, // image_4_4_1025x1025_COMPRESSED_RGBA_ASTC_5x5_KHR.raw
	};

	typedef struct PPP_ {
		int box_id;
		uint32_t x, y, w, h;

		uint32_t scissor_x, scissor_y;
	}PPP;

	PPP box[4];

	char isInit = 1;
	while (1) {
		POS* lcdTouch = App_GetTouchPos();

		if (lcdTouch->x <= 0 || lcdTouch->y <= 0
			|| lcdTouch->x >= s_pHalContext->Width || lcdTouch->y >= s_pHalContext->Height) {
			if (!isInit) continue;
			else {
				lcdTouch->x = s_pHalContext->Width / 2;
				lcdTouch->y = s_pHalContext->Height/ 2;
				isInit = 0;
			}
		}

		int astcPosX = lcdTouch->x * ((IMG_W + BOX_W) / BOX_W);
		int astcPosY = lcdTouch->y * ((IMG_H + BOX_H * 3) / BOX_H);

		astcPosX -= BOX_W;
		astcPosY -= BOX_H;

		int matrix_x = astcPosX / 1024;
		int matrix_y = astcPosY / 1024;

		box[0].box_id = matrix_y * 4 + matrix_x;
		box[1].box_id = box[0].box_id + 1;
		box[2].box_id = box[0].box_id + 4;
		box[3].box_id = box[0].box_id + 5;
		if (matrix_x == 3) {
			box[1].box_id = -1;
			box[3].box_id = -1;
		}
		if (matrix_y == 3) {
			box[2].box_id = -1;
			box[3].box_id = -1;
		}

		int x = astcPosX % 1024;
		int y = astcPosY % 1024;

		box[0].x = -x;
		box[0].y = -y;
		box[0].w = (1024 - x) > BOX_W ? BOX_W : 1024 - x;
		box[0].h = (1024 - y) > BOX_H ? BOX_H : 1024 - y;
		box[0].scissor_x = 0;
		box[0].scissor_y = 0;

		if (box[0].w >= BOX_W) {
			box[1].box_id = -1;
		}
		else {
			box[1].x = box[0].w;
			box[1].y = box[0].y;
			box[1].w = BOX_W - box[0].w;
			box[1].h = box[0].h;
			box[1].scissor_x = box[0].w;
			box[1].scissor_y = 0;
		}

		if (box[0].h >= BOX_H) {
			box[2].box_id = -1;
		}
		else {
			box[2].x = box[0].x;
			box[2].y = box[0].h;
			box[2].w = box[0].w;
			box[2].h = BOX_H - box[0].h;
			box[2].scissor_x = 0;
			box[2].scissor_y = box[0].h;
		}

		if (box[0].h >= BOX_H || box[0].w >= BOX_W) {
			box[3].box_id = -1;
		}
		else {
			box[3].x = box[1].x;
			box[3].y = box[2].y;
			box[3].w = box[1].w;
			box[3].h = box[2].h;
			box[3].scissor_x = box[0].w;
			box[3].scissor_y = box[0].h;
		}
		
		Display_Start(s_pHalContext);
		for (int i = 0; i < 4; i++) {
			if (box[i].box_id == -1 || box[i].box_id > 15) {
				continue;
			}
		
			EVE_CoCmd_setBitmap(s_pHalContext, ATFLASH(images[box[i].box_id].addressFlash), images[box[i].box_id].format, images[box[i].box_id].w, images[box[i].box_id].h);

			EVE_Cmd_wr32(s_pHalContext, SCISSOR_XY((box[i].scissor_x + 0), (box[i].scissor_y + 0)));
			EVE_Cmd_wr32(s_pHalContext, SCISSOR_SIZE(box[i].w, box[i].h));

			EVE_Cmd_wr32(s_pHalContext, BEGIN(BITMAPS));
			EVE_Cmd_wr32(s_pHalContext, VERTEX2F((box[i].x + 0) * 16, (box[i].y + 0) * 16));
			EVE_Cmd_wr32(s_pHalContext, END());
		}
		Display_End(s_pHalContext);
	}
}

void DemoHDPictureViewer(EVE_HalContext* pHalContext) {
#if !defined(EVE_FLASH_AVAILABLE)
    #error Platform not supported
#endif
	s_pHalContext = pHalContext;
	FlashHelper_SwitchFullMode(s_pHalContext);
	App_Draw2_Many_ASTC_1k();
	EVE_sleep(10 * 1000);
}
