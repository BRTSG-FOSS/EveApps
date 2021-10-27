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

#include <math.h>

static EVE_HalContext *s_pHalContext;

const uint8_t rate = 125;
typedef struct _sinwave {
	int y;
	int w;
} SINWAVE;

#define pixelPoint(x, y) (((y) * w) + (x))

void sin_lut() {
	int offsetX = 0;
	int offsetY = s_pHalContext->Height / 2;

	int w = s_pHalContext->Width, h = s_pHalContext->Height, adr = 100 * 1024;
	char rawh[] = { 0x04, 0x6E, 0x0F, 0x0F, 0x8B, 0x48, 0x54, 0x04, 0x18, 0x62,
			0x57, 0x77, 0xBB, 0x32, 0x9C, 0x64, 0x7B, 0x3E, 0x29, 0x3D, 0xAE,
			0x9D, 0x81, 0x42, 0x3A, 0x6A, 0xB2, 0x55, 0x70, 0x21, 0x4F, 0x50,
			0x5B, 0x1C, 0x4C, 0xA8, 0xAF, 0x9E, 0x27, 0x91, 0xB4, 0x98, 0x2B,
			0xA7, 0x68, 0xBF, 0x6B, 0x87, 0x7C, 0x28, 0xA9, 0x80, 0x41, 0x96,
			0xAC, 0xA5, 0x90, 0x4B, 0xBD, 0x3B, 0x95, 0x83, 0xBC, 0x39, 0x1E,
			0xA4, 0x8F, 0x75, 0x2A, 0x9B, 0x46, 0xAD, 0x97, 0x1A, 0x40, 0x65,
			0xBA, 0x6D, 0x92, 0x52, 0x7A, 0x8D, 0x9F, 0x1F, 0x58, 0x59, 0x76,
			0x25, 0x30, 0x2F, 0x88, 0xB3, 0x33, 0x67, 0x7E, 0x3C, 0x1D, 0x06,
			0x06, 0x84, 0x26, 0x5E, 0x0B, 0x0B, 0x11, 0x11, 0x03, 0x03, 0x66,
			0x07, 0x07, 0x24, 0x49, 0x44, 0x22, 0x5A, 0x01, 0x01, 0x0A, 0x0A,
			0x51, 0xA3, 0x12, 0x12, 0x0D, 0x0D, 0x31, 0xAB, 0x8C, 0x3F, 0x8E,
			0x56, 0x9A, 0x7F, 0x47, 0xA0, 0x8A, 0x63, 0x13, 0x13, 0x09, 0x09,
			0x00, 0x00, 0x00, 0x4D, 0x79, 0x15, 0x14, 0x14, 0x99, 0x53, 0xB6,
			0x7D, 0x69, 0x5F, 0x2D, 0x94, 0x08, 0x08, 0x0C, 0x0C, 0x2C, 0x73,
			0x38, 0xB0, 0x17, 0x6F, 0x23, 0x85, 0x35, 0x2E, 0x82, 0x1B, 0xA6,
			0xB7, 0xA2, 0xB9, 0xB1, 0x36, 0x16, 0x5D, 0x5C, 0x78, 0xB5, 0x4E,
			0x05, 0x05, 0xBE, 0x74, 0x45, 0x19, 0x43, 0x34, 0xA1, 0x93, 0x60,
			0xB8, 0x72, 0x86, 0x20, 0x4A, 0x02, 0x02, 0x71, 0xAA, 0x0E, 0x0E,
			0x37, 0x89, 0x10, 0x10, 0x6C, 0x61 };

	Gpu_Hal_LoadImageToMemory(s_pHalContext,
			TEST_DIR "/sample1px_214x1_PALETTED565_lut.raw", 0, LOAD);

	Display_Start(s_pHalContext);
	EVE_CoCmd_setBitmap(s_pHalContext, adr, PALETTED565, w, h);
	EVE_Cmd_wr32(s_pHalContext, PALETTE_SOURCE(0));

	EVE_Cmd_wr32(s_pHalContext, BEGIN(BITMAPS));
	EVE_Cmd_wr32(s_pHalContext,
			VERTEX2F((s_pHalContext->Width / 2 - w / 2) * 16,
					(s_pHalContext->Height / 2 - h / 2) * 16));
	Display_End(s_pHalContext);
	int amp = 100;
	int sinwave_s = w;
	SINWAVE* sinwave = malloc(sinwave_s * sizeof(SINWAVE));
	if (!sinwave) {
		APP_ERR("Not enough memory");
		Draw_Text(s_pHalContext, "Not enough memory");
		EVE_sleep(5000);
		exit(0);
	}
	char* colorbuffer = malloc(w * h);
	if (!colorbuffer) {
		APP_ERR("Not enough memory");
		Draw_Text(s_pHalContext, "Not enough memory");
		EVE_sleep(5000);
		exit(0);
	}

	// draw sin for to buffer
#define colorRed    20
#define colorYellow 40
#define colorGreen  60
#define colorBGreen 140
#define colorBlue   214
	int colorTable[] = { colorRed, colorYellow, colorGreen, colorBGreen, colorBlue };
	int weight[] = { 1, 1, 1, 1, 2 };
	static int sOffsetX = 0, sOffsetY = 0, drawX = 0, drawY = 0;

	drawY = s_pHalContext->Height / 2;

	static int sDistance = 0, lastDistance = 0;
	static int zoom = 0;
	while (1) {
		int touch1 = EVE_Hal_rd32(s_pHalContext, REG_CTOUCH_TOUCH0_XY);
		int touch2 = EVE_Hal_rd32(s_pHalContext, REG_CTOUCH_TOUCH1_XY);

		int x1 = touch1 >> 16;
		int x2 = touch2 >> 16;
		int y1 = touch1 & 0x0000FFFF;
		int y2 = touch2 & 0x0000FFFF;
		static int sx1 = 0, sx2 = 0, sy1 = 0, sy2 = 0;

		int distance = sqrt(((x2 - x1) * (x2 - x1)) + ((y2 - y1) * (y2 - y1)));

		if (distance > w || distance < 0) {
			distance = 0;
		}

		if (lastDistance == 0 && distance > 0 && sDistance > 0) {
			sDistance = distance;
		}

		if (distance > sDistance && distance > 0) {
			zoom += (distance - sDistance);
			sDistance = distance;
		} else if (0 < distance && distance < sDistance) {
			zoom -= (sDistance - distance);
			sDistance = distance;
		}

		if (zoom < -100) {
			zoom = -100;
		}
		lastDistance = distance;

		Gesture_Renew(s_pHalContext);
		offsetX = Gesture_Get()->touchX;
		offsetY = Gesture_Get()->touchY;
		if (distance > 0) {
			offsetX = offsetY = 0;
		}

		if (offsetY == 0 || offsetY == 32768) {
			sOffsetY = 0;
		} else {
			if (sOffsetY == 0) {
				sOffsetY = offsetY;
			}
			drawY += offsetY - sOffsetY;
			sOffsetY = offsetY;
		}

		if (offsetX == 0 || offsetX == 32768) {
			sOffsetX = 0;
		} else {
			if (sOffsetX == 0) {
				sOffsetX = offsetX;
			}
			drawX += offsetX - sOffsetX;
			sOffsetX = offsetX;
		}

		// calculate sin form
		for (int x = 0; x < sinwave_s; x++) {
			sinwave[x].y = drawY + ((int32_t) (amp + zoom)
					* Math_Qsin(-65536 * (w + x - drawX) / (zoom + rate))/ 65536);
			sinwave[x].w = 2;
		}
		memset(colorbuffer, rawh[sizeof(rawh) - 1], w * h);
		// loop through screen from left to right
		for (int i = 0; i < w; i++) {
			int column = s_pHalContext->Height - rand() % (s_pHalContext->Height * 2 / 10);
			int colorId = 0;
			float color = 0, step = 1;
			int colorTop = 0;
			colorTop = colorTable[colorId];
			float colorMax = colorTop * weight[colorId] - rand() % (colorTop * 5 / 10);;

			for (int pixelh = 0; pixelh < column; pixelh++) {
				int topPart = pixelPoint(i, sinwave[i].y + pixelh);
				int bottomPart = pixelPoint(i, sinwave[i].y - pixelh);

				if (topPart < w * h && topPart > 0) {
					colorbuffer[topPart] = rawh[(int) color];
				}
				if (bottomPart < w * h && bottomPart > 0) {
					colorbuffer[bottomPart] = rawh[(int) color];
				}
				color += step;
				if (color >= colorMax) {
					colorId++;
					if (colorId > 5) {
						break;
					}
					colorTop = colorTable[colorId];
					colorMax = colorTop * weight[colorId] - rand() % (colorTop * 5 / 10);
					step = 1.0 * colorTop / colorMax;
				}
				if (color > 212)
					color--;
			}
		}
		// write sin buffer to ram_g
		EVE_Hal_wrMem(s_pHalContext, adr, colorbuffer, w * h);
		EVE_sleep(0);
	}
}

void DemoInstrument(EVE_HalContext* pHalContext) {
	s_pHalContext = pHalContext;
#if EVE_CHIPID < EVE_BT815
#error Platform not supported
#endif
#if defined(EVE_SUPPORT_CAPACITIVE)
	EVE_Hal_wr8(s_pHalContext, REG_CTOUCH_EXTENDED, CTOUCH_MODE_EXTENDED);
#endif
	sin_lut();
}

