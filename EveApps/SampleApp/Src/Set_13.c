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
  *  Description: This is sample demo for extended font format and its glyph data in RAM_G
  */

#include "Platform.h"
#include "Gpu_Hal.h"
#include "Common.h"
#include "App.h"
#include "Samapp_Enable_API.h"

#ifdef SAMAPP_ENABLE_APIS_SET13

static EVE_HalContext *s_pHalContext;

/**
 * @brief Load xfont to RAM_G
 *
 * @param isReindex
 */
void set13_loadXfont(uint8_t isReindex) {
	static uint32_t unicodeHandle = 0;
	static uint32_t XFONT_ADDR = RAM_G;
	static uint32_t GLYPH_ADDR = 20480; // glyph offset from 20kb
	const uint8_t* str_noindex = u8"自來水, 磨蝕性";// it's mean: water wears away the stone
	const uint8_t* str_reindex = u8"\x0043\x003f\x0041\x0002\x0001\x0042\x0044\x0040\x0001";
	const uint8_t* str = str_noindex;

#if defined(MSVC_PLATFORM) || defined(BT8XXEMU_PLATFORM)
	const uint8_t* xfont_path_noindex = TEST_DIR "\\simkai1.xfo";// set13: font simkai, size 50, font's index
	const uint8_t* glyph_path_noindex = TEST_DIR "\\simkai1.gly";// set13: font simkai, size 50, font's index
	const uint8_t* xfont_path_reindex = TEST_DIR "\\simkai2.xfo";// set13: font simkai, size 50, re-indexed
	const uint8_t* glyph_path_reindex = TEST_DIR "\\simkai2.gly";// set13: font simkai, size 50, re-indexed
	const uint8_t* xfont = xfont_path_noindex;
	const uint8_t* glyph = glyph_path_noindex;
#elif defined (ARDUINO_PLATFORM) || defined (FT9XX_PLATFORM) || defined (RP2040_PLATFORM)
	int8_t* xfont_path_noindex = TEST_DIR "\\simkai1.xfo"; // set13: font simkai, size 50, font's index
	int8_t* glyph_path_noindex = TEST_DIR "\\simkai1.gly"; // set13: font simkai, size 50, font's index
	int8_t* xfont_path_reindex = TEST_DIR "\\simkai2.xfo"; // set13: font simkai, size 50, re-indexed
	int8_t* glyph_path_reindex = TEST_DIR "\\simkai2.gly"; // set13: font simkai, size 50, re-indexed
	int8_t* xfont = xfont_path_noindex;
	int8_t* glyph = glyph_path_noindex;
#endif

	if (isReindex) {
		str = str_reindex;
		xfont = xfont_path_reindex;
		glyph = glyph_path_reindex;
	}

#if defined(MSVC_PLATFORM) || defined(BT8XXEMU_PLATFORM)
	//Load xfont file into graphics RAM
	Gpu_Hal_LoadImageToMemory(s_pHalContext, xfont, XFONT_ADDR, LOAD);
	//Load glyph file into graphics RAM
	Gpu_Hal_LoadImageToMemory(s_pHalContext, glyph, GLYPH_ADDR, LOAD);
	EVE_Cmd_waitFlush(s_pHalContext);
#elif defined (ARDUINO_PLATFORM) || defined (FT9XX_PLATFORM) || defined (RP2040_PLATFORM)
	if (str == str_noindex) {
		printf("No index - xfont \n");
	}
	else {
		printf("Re index - xfont \n");
	}
	Gpu_Hal_LoadImageToMemory(s_pHalContext, xfont, XFONT_ADDR, LOAD);

	if (str == str_noindex) {
		printf("No index - glyph \n");
	}
	else {
		printf("Re index - glyph \n");
	}
	Gpu_Hal_LoadImageToMemory(s_pHalContext, glyph, GLYPH_ADDR, LOAD);
	printf("DONE \n");
#endif

	EVE_CoCmd_gradientA(s_pHalContext, 0, 0, 0xff000000, 0, (int16_t)s_pHalContext->Height, 0xff999999);

	EVE_CoCmd_setFont2(s_pHalContext, unicodeHandle, XFONT_ADDR, 0);
	if (isReindex) {
		EVE_CoCmd_text(s_pHalContext, 10, 10, 29, 0, "Display unicode string use re-index font data");
	}
	else {
		EVE_CoCmd_text(s_pHalContext, 10, 10, 29, 0, "Display unicode string use font's index");
	}
	EVE_CoCmd_text(s_pHalContext, 10, 70, (int16_t)unicodeHandle, 0, str);
}

/**
 * @brief Draw set 13
 *
 */
void App_Set_13_Draw() {
	Display_Start(s_pHalContext);
	set13_loadXfont(0);
	Display_End(s_pHalContext);
	EVE_sleep(5000);// display in 5 second

	// clear screen
	Display_Start(s_pHalContext);
	Display_End(s_pHalContext);

	Display_Start(s_pHalContext);
	set13_loadXfont(1);
	Display_End(s_pHalContext);
	EVE_sleep(5000);// display in 5 second
}

/**
 * @brief Init set 13
 *
 */
void App_Set_13_Init(EVE_HalContext* pHalContext) {
	s_pHalContext = pHalContext;
	Draw_Text(s_pHalContext, "Set13   Start!");
}

/**
 * @brief Deinit set 13
 *
 */
void App_Set_13_Deinit() {
	Draw_Text(s_pHalContext, "Set13   End!");
}

#endif // SAMAPP_ENABLE_APIS_SET13
