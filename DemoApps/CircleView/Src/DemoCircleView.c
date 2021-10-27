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

#include "Common.h"
#include "App.h"

#define RAW  4096
#define LUT  421184 
#define SIZ  (LUT+1024)

static uint16_t isTouch, touchX, touchY;
static EVE_HalContext *s_pHalContext;

void getMousePosition() {
	isTouch = EVE_Hal_rd32(s_pHalContext, REG_TOUCH_RAW_XY) != 0xFFFFFFFF;

	uint32_t x = EVE_Hal_rd16(s_pHalContext, REG_TOUCH_SCREEN_XY + 2);
	uint32_t y = EVE_Hal_rd16(s_pHalContext, REG_TOUCH_SCREEN_XY + 4);

	if (x > s_pHalContext->Width || y > s_pHalContext->Height) {
		isTouch = 0;
	}
	else {
		touchX = x;
		touchY = y;
	}
}

void mousePoint() {
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(128, 128, 0));
	EVE_Cmd_wr32(s_pHalContext, BEGIN(POINTS));
	EVE_Cmd_wr32(s_pHalContext, POINT_SIZE(10 * 16));
	EVE_Cmd_wr32(s_pHalContext, VERTEX2F(touchX * 16, touchY * 16));
}
void draw() {
	uint32_t x, y;

	x = touchX * 16;
	y = touchY * 16;

	Display_Start(s_pHalContext);

	EVE_Cmd_wr32(s_pHalContext, CLEAR_COLOR_RGB(255, 255, 255));
	EVE_Cmd_wr32(s_pHalContext, CLEAR_COLOR_A(0));
	EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1));

	EVE_CoCmd_setBitmap(s_pHalContext, RAW, PALETTED8, 802, 520);

	EVE_Cmd_wr32(s_pHalContext, POINT_SIZE(2600));
	EVE_Cmd_wr32(s_pHalContext, BEGIN(POINTS));
	EVE_Cmd_wr32(s_pHalContext, VERTEX_FORMAT(4));
	EVE_Cmd_wr32(s_pHalContext, COLOR_MASK(0, 0, 0, 1));
	EVE_Cmd_wr32(s_pHalContext, COLOR_A(255));
	EVE_Cmd_wr32(s_pHalContext, VERTEX2F(x, y));

	EVE_Cmd_wr32(s_pHalContext, BEGIN(BITMAPS));
	//Alpha channel                        
	EVE_Cmd_wr32(s_pHalContext, BLEND_FUNC(DST_ALPHA, ZERO));
	EVE_Cmd_wr32(s_pHalContext, COLOR_MASK(0, 0, 0, 1));
	EVE_Cmd_wr32(s_pHalContext, PALETTE_SOURCE(LUT + 3));
	EVE_Cmd_wr32(s_pHalContext, VERTEX2II(0, 0, 0, 0));
	
	//Red Channel                          
	EVE_Cmd_wr32(s_pHalContext, COLOR_MASK(1, 0, 0, 0));
	EVE_Cmd_wr32(s_pHalContext, PALETTE_SOURCE(LUT + 2));
	EVE_Cmd_wr32(s_pHalContext, VERTEX2II(0, 0, 0, 0));

	// Green Channel                        
	EVE_Cmd_wr32(s_pHalContext, COLOR_MASK(0, 1, 0, 0));
	EVE_Cmd_wr32(s_pHalContext, PALETTE_SOURCE(LUT+1));
	EVE_Cmd_wr32(s_pHalContext, VERTEX2II(0, 0, 0, 0));

	//Blue Channel                         
	EVE_Cmd_wr32(s_pHalContext, COLOR_MASK(0, 0, 1, 0));
	EVE_Cmd_wr32(s_pHalContext, PALETTE_SOURCE(LUT));
	EVE_Cmd_wr32(s_pHalContext, VERTEX2II(0, 0, 0, 0));

	mousePoint();

	Display_End(s_pHalContext);
}

void DemoCircleView(EVE_HalContext* pHalContext) {
#if !defined(EVE_FLASH_AVAILABLE)
    #error Platform not supported
#endif
	s_pHalContext = pHalContext;
	FlashHelper_SwitchFullMode(s_pHalContext);
	EVE_CoCmd_flashRead(s_pHalContext, 0, 0, SIZ);

	touchX = s_pHalContext->Width / 2;
	touchY = s_pHalContext->Height / 2;

	while (true) {
		draw();
		getMousePosition();
	}	
}

