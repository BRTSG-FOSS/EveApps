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

#define REPORT_FRAMES   0
#if defined(DISPLAY_RESOLUTION_WQVGA) || defined(DISPLAY_RESOLUTION_QVGA) || defined(DISPLAY_RESOLUTION_HVGA_PORTRAIT)
#define SUBDIV  4
#define YY      (480 / SUBDIV)
#elif defined(DISPLAY_RESOLUTION_WVGA)
#define SUBDIV  12
#define YY      ((800 +SUBDIV)/ SUBDIV)
#define MIN_PIXELS_PER_DIV 55
#endif

float_t transform_m, transform_c;

#if defined(DISPLAY_RESOLUTION_HVGA_PORTRAIT)
float_t m_min = 13.0 / 65536;
#define SUBDIV  4
#define YY      (480 / SUBDIV)
#endif
#if defined(DISPLAY_RESOLUTION_WQVGA) || defined(DISPLAY_RESOLUTION_QVGA)
float_t m_min = 13.0 / 65536;
#elif defined(DISPLAY_RESOLUTION_WVGA)
float_t m_min = 26.0 / 65536;
#else
float_t m_min = 26.0 / 65536;
#define SUBDIV  12
#define YY      (3 * SUBDIV)
#endif

void read_extended(int16_t sx[5], int16_t sy[5])
{
	uint32_t sxy0, sxyA, sxyB, sxyC;
	sxy0 = EVE_Hal_rd32(s_pHalContext, REG_CTOUCH_TOUCH0_XY);
	sxyA = EVE_Hal_rd32(s_pHalContext, REG_CTOUCH_TOUCH1_XY);
	sxyB = EVE_Hal_rd32(s_pHalContext, REG_CTOUCH_TOUCH2_XY);
	sxyC = EVE_Hal_rd32(s_pHalContext, REG_CTOUCH_TOUCH3_XY);

	sx[0] = sxy0 >> 16;
	sy[0] = sxy0;
	sx[1] = sxyA >> 16;
	sy[1] = sxyA;
	sx[2] = sxyB >> 16;
	sy[2] = sxyB;
	sx[3] = sxyC >> 16;
	sy[3] = sxyC;

	sx[4] = EVE_Hal_rd16(s_pHalContext, REG_CTOUCH_TOUCH4_X);
	sy[4] = EVE_Hal_rd16(s_pHalContext, REG_CTOUCH_TOUCH4_Y);
}
/* API used for single touch usecases */
void read_compatible(int16_t sx[5], int16_t sy[5])
{
	uint32_t sxy0, sxyA, sxyB, sxyC;
	sxy0 = EVE_Hal_rd32(s_pHalContext, REG_TOUCH_SCREEN_XY);
	sxyA = 0x80008000;
	sxyB = 0x80008000;
	sxyC = 0x80008000;

	sx[0] = sxy0 >> 16;
	sy[0] = sxy0;
	sx[1] = sxyA >> 16;
	sy[1] = sxyA;
	sx[2] = sxyB >> 16;
	sy[2] = sxyB;
	sx[3] = sxyC >> 16;
	sy[3] = sxyC;

	sx[4] = 0x8000;
	sy[4] = 0x8000;
}

void set(int32_t x0, int16_t y0,
	int32_t x1, int16_t y1) {
	int32_t xd = x1 - x0;
	int16_t yd = y1 - y0;
	transform_m = yd / (float_t)xd;
	if (transform_m < m_min)
		transform_m = m_min;
	transform_c = y0 - transform_m * x0;
}
void sset(int32_t x0, int16_t y0)
{
	transform_c = (float_t)y0 - transform_m * x0;
}
int16_t m2s(int32_t x)
{
	return (int16_t)(transform_m * x + transform_c);
}
int32_t s2m(int16_t y)
{
	return (int32_t)(y - transform_c) / transform_m;
}

PROGMEM prog_uint16_t sintab_x[257] = {
	0, 402, 804, 1206, 1608, 2010, 2412, 2813, 3215, 3617, 4018, 4419, 4821, 5221, 5622, 6023, 6423, 6823, 7223, 7622, 8022, 8421, 8819, 9218, 9615, 10013, 10410, 10807, 11203, 11599, 11995, 12390, 12785, 13179, 13573, 13966, 14358, 14750, 15142, 15533, 15923, 16313, 16702, 17091, 17479, 17866, 18252, 18638, 19023, 19408, 19791, 20174, 20557, 20938, 21319, 21699, 22078, 22456, 22833, 23210, 23585, 23960, 24334, 24707, 25079, 25450, 25820, 26189, 26557, 26924, 27290, 27655, 28019, 28382, 28744, 29105, 29465, 29823, 30181, 30537, 30892, 31247, 31599, 31951, 32302, 32651, 32999, 33346, 33691, 34035, 34378, 34720, 35061, 35400, 35737, 36074, 36409, 36742, 37075, 37406, 37735, 38063, 38390, 38715, 39039, 39361, 39682, 40001, 40319, 40635, 40950, 41263, 41574, 41885, 42193, 42500, 42805, 43109, 43411, 43711, 44010, 44307, 44603, 44896, 45189, 45479, 45768, 46055, 46340, 46623, 46905, 47185, 47463, 47739, 48014, 48287, 48558, 48827, 49094, 49360, 49623, 49885, 50145, 50403, 50659, 50913, 51165, 51415, 51664, 51910, 52155, 52397, 52638, 52876, 53113, 53347, 53580, 53810, 54039, 54265, 54490, 54712, 54933, 55151, 55367, 55581, 55793, 56003, 56211, 56416, 56620, 56821, 57021, 57218, 57413, 57606, 57796, 57985, 58171, 58355, 58537, 58717, 58894, 59069, 59242, 59413, 59582, 59748, 59912, 60074, 60234, 60391, 60546, 60699, 60849, 60997, 61143, 61287, 61428, 61567, 61704, 61838, 61970, 62100, 62227, 62352, 62474, 62595, 62713, 62828, 62941, 63052, 63161, 63267, 63370, 63472, 63570, 63667, 63761, 63853, 63942, 64029, 64114, 64196, 64275, 64353, 64427, 64500, 64570, 64637, 64702, 64765, 64825, 64883, 64938, 64991, 65042, 65090, 65135, 65178, 65219, 65257, 65293, 65326, 65357, 65385, 65411, 65435, 65456, 65474, 65490, 65504, 65515, 65523, 65530, 65533, 65535
};

int16_t rsin(int16_t r, uint16_t th)
{
	int32_t th4; uint16_t s; int16_t p;
	th >>= 6; // angle 0-123

	th4 = th & 511;
	if (th4 & 256)
		th4 = 512 - th4; // 256->256 257->255, etc

	s = pgm_read_word(sintab_x + th4);

	p = ((uint32_t)s * r) >> 16;
	if (th & 512)
		p = -p;
	return p;
}

void plot()
{
	int32_t mm[2], m, j;
	uint8_t fade, h, i;
	int16_t pixels_per_div, x, clock_r, x1;
	int32_t y[YY + 1];
	uint32_t x32;
	uint16_t x2, options;

	EVE_Cmd_wr32(s_pHalContext, STENCIL_OP(ZERO, ZERO));
	EVE_CoCmd_gradient(s_pHalContext, 0, 0, 0x202020, 0, s_pHalContext->Height, 0x107fff);

	mm[0] = s2m(0);
	mm[1] = s2m(s_pHalContext->Width);
	pixels_per_div = m2s(0x4000) - m2s(0);

#if defined(DISPLAY_RESOLUTION_WVGA)
	fade = MIN(255, MAX(0, (pixels_per_div - 60) * 16));
#else
	fade = MIN(255, MAX(0, (pixels_per_div - 32) * 16));
#endif
	EVE_Cmd_wr32(s_pHalContext, LINE_WIDTH(MAX(8, pixels_per_div >> 2)));
	for (m = mm[0] & ~0x3fff; m <= mm[1]; m += 0x4000)
	{
		x = m2s(m);
		if ((-60 <= x) && (x <= (s_pHalContext->Width + 60)))
		{
			h = 3 * (7 & (m >> 14));

			EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0, 0, 0));
			EVE_Cmd_wr32(s_pHalContext, COLOR_A(((h == 0) ? 192 : 64)));
			EVE_Cmd_wr32(s_pHalContext, BEGIN(LINES));
			EVE_Cmd_wr32(s_pHalContext, VERTEX2F(x * 16, 0));
			EVE_Cmd_wr32(s_pHalContext, VERTEX2F(x * 16, s_pHalContext->Height * 16));

			if (fade)
			{
				x -= 1;
				EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0xd0, 0xd0, 0xd0));
				EVE_Cmd_wr32(s_pHalContext, COLOR_A(fade));
#if defined(DISPLAY_RESOLUTION_QVGA) | defined(DISPLAY_RESOLUTION_WQVGA)
				EVE_CoCmd_number(s_pHalContext, x, 0, 26, OPT_RIGHTX | 2, h);
				EVE_CoCmd_text(s_pHalContext, x, 0, 26, 0, ":00");
#elif defined(DISPLAY_RESOLUTION_WVGA)
				EVE_CoCmd_number(s_pHalContext, x, 0, 30, OPT_RIGHTX | 2, h);
				EVE_CoCmd_text(s_pHalContext, x, 0, 30, 0, ":00");
#elif  DISPLAY_RESOLUTION_HVGA_PORTRAIT
				EVE_CoCmd_number(s_pHalContext, x, 0, 28, OPT_RIGHTX | 2, h);
				EVE_CoCmd_text(s_pHalContext, x, 0, 28, 0, ":00");
#else
				EVE_CoCmd_number(s_pHalContext, x, 0, 28, OPT_RIGHTX | 2, h);
				EVE_CoCmd_text(s_pHalContext, x, 0, 28, 0, ":00");
#endif
			}
		}
	}
	EVE_Cmd_wr32(s_pHalContext, COLOR_A(255));

	for (i = 0; i < (YY + 1); i++)
	{
		x32 = s2m(SUBDIV * i);
		x2 = (uint16_t)x32 + rsin(7117, x32);
		y[i] = 130 * 16 + rsin(1200, (217 * x32) >> 8) + rsin(700, 3 * x2);
	}

	EVE_Cmd_wr32(s_pHalContext, STENCIL_OP(INCR, INCR));
	EVE_Cmd_wr32(s_pHalContext, BEGIN(EDGE_STRIP_B));
	for (j = 0; j < (YY + 1); j++)
	{
		EVE_Cmd_wr32(s_pHalContext, VERTEX2F(16 * SUBDIV * j, y[j]));
	}
	EVE_Cmd_wr32(s_pHalContext, STENCIL_FUNC(EQUAL, 1, 255));
	EVE_Cmd_wr32(s_pHalContext, STENCIL_OP(KEEP, KEEP));
	EVE_CoCmd_gradient(s_pHalContext, 0, 0, 0xf1b608, 0, s_pHalContext->Height, 0x98473a);

	EVE_Cmd_wr32(s_pHalContext, STENCIL_FUNC(ALWAYS, 1, 255));
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0xE0, 0xE0, 0xE0));
	EVE_Cmd_wr32(s_pHalContext, LINE_WIDTH(24));
	EVE_Cmd_wr32(s_pHalContext, BEGIN(LINE_STRIP));

	for (j = 0; j < (YY + 1); j++)
	{
		EVE_Cmd_wr32(s_pHalContext, VERTEX2F(16 * SUBDIV * j, y[j]));
	}

#if defined(DISPLAY_RESOLUTION_QVGA) || defined(DISPLAY_RESOLUTION_WQVGA)
	clock_r = MIN(24, pixels_per_div >> 2);
#elif defined(DISPLAY_RESOLUTION_WVGA)
	clock_r = MIN(48, pixels_per_div >> 2);
#elif  DISPLAY_RESOLUTION_HVGA_PORTRAIT
	clock_r = MIN(48, pixels_per_div >> 2);
#else
	clock_r = MIN(48, pixels_per_div >> 2);
#endif

#if defined(FT9XX_PLATFORM)
	if (clock_r > 4 && fade)
#else
	if (clock_r > 4)
#endif
	{
		EVE_Cmd_wr32(s_pHalContext, COLOR_A(200));
		EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0xff, 0xff, 0xff));
		options = OPT_NOSECS | OPT_FLAT;
		if (clock_r < 10)
			options |= OPT_NOTICKS;
		for (m = mm[0] & ~0x3fff; m <= mm[1]; m += 0x4000)
		{
			x1 = m2s(m);
			h = 3 * (3 & (m >> 14));
			if (x1 >= -1024)
				EVE_CoCmd_clock(s_pHalContext, x1, s_pHalContext->Height - clock_r, clock_r, options, h, 0, 0, 0);
		}
	}
}

void DemoGraph(EVE_HalContext* pHalContext) {
	s_pHalContext = pHalContext;
	int16_t sx[5], sy[5];
	int32_t f, j;
	static int32_t _f;
	int16_t cx = 0, cy = 0;
	static bool_t down[2] = { 0,0 };
	static int32_t m[2] = { 0UL,0UL };

	uint8_t n = 0, i = 0;

	set(0, 0, 0x10000, (int16_t)s_pHalContext->Width);
	/* Set the touch mode to extended in case of capacitive */
#if (defined(FT801_ENABLE) || defined(FT811_ENABLE) || defined(FT813_ENABLE) ||defined(BT815_ENABLE) ||defined(BT817_ENABLE))
	EVE_Hal_wr8(s_pHalContext, REG_CTOUCH_EXTENDED, CTOUCH_MODE_EXTENDED);
#elif defined(EVE_SUPPORT_CAPACITIVE)
	EVE_Hal_wr8(s_pHalContext, REG_CTOUCH_EXTENDED, CTOUCH_MODE_COMPATIBILITY);
#else
	EVE_Hal_wr8(s_pHalContext, REG_TOUCH_ADC_MODE , ADC_DIFFERENTIAL);
#endif

	while (1)
	{
		if (REPORT_FRAMES) {
			f = EVE_Hal_rd16(s_pHalContext, REG_FRAMES);

			printf("Error : %d", f - _f);
			_f = f;
		}
#if (defined(FT801_ENABLE) || defined(FT811_ENABLE) || defined(FT813_ENABLE) ||defined(BT815_ENABLE) ||defined(BT817_ENABLE))
		read_extended(sx, sy);
#else
		read_compatible(sx, sy);
#endif
		for (i = 0; i < 2; i++)
		{
			if (sx[i] > -10 && !down[i])
			{
				down[i] = 1;
				m[i] = s2m(sx[i]);
			}
			if (sx[i] < -10)
				down[i] = 0;
		}
		if (down[0] && down[1])
		{
			if (m[0] != m[1])
				set(m[0], sx[0], m[1], sx[1]);
		}
		else if (down[0] && !down[1])
			sset(m[0], sx[0]);
		else if (!down[0] && down[1])
			sset(m[1], sx[1]);

		EVE_Cmd_wr32(s_pHalContext, CMD_DLSTART);
		plot();

		// display touches
		if (0)
		{
			EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0xff, 0xff, 0xff));
			EVE_Cmd_wr32(s_pHalContext, LINE_WIDTH(8));
			EVE_Cmd_wr32(s_pHalContext, BEGIN(LINES));
			for (i = 0; i < 2; i++)
			{
				if (sx[i] > -10)
				{
#if defined(DISPLAY_RESOLUTION_QVGA) | defined(DISPLAY_RESOLUTION_WQVGA)
					EVE_Cmd_wr32(s_pHalContext, VERTEX2II(sx[i], 0, 0, 0));
					EVE_Cmd_wr32(s_pHalContext, VERTEX2II(sx[i], 272, 0, 0));
#elif defined(DISPLAY_RESOLUTION_WVGA)
					EVE_Cmd_wr32(s_pHalContext, VERTEX2F(sx[i] * 16, 0));
					EVE_Cmd_wr32(s_pHalContext, VERTEX2F(sx[i] * 16, s_pHalContext->Height * 16));
#elif  defined(DISPLAY_RESOLUTION_HVGA_PORTRAIT)
					EVE_Cmd_wr32(s_pHalContext, VERTEX2F(sx[i] * 16, 0));
					EVE_Cmd_wr32(s_pHalContext, VERTEX2F(sx[i] * 16, s_pHalContext->Height * 16));
#else
					EVE_Cmd_wr32(s_pHalContext, VERTEX2F(sx[i] * 16, 0));
					EVE_Cmd_wr32(s_pHalContext, VERTEX2F(sx[i] * 16, s_pHalContext->Height * 16));
#endif

				}
			}
		}

		EVE_Cmd_waitFlush(s_pHalContext);

		EVE_Cmd_wr32(s_pHalContext, DISPLAY());
		EVE_CoCmd_swap(s_pHalContext);
		EVE_CoCmd_loadIdentity(s_pHalContext);
		EVE_Cmd_waitFlush(s_pHalContext);
	}
}
