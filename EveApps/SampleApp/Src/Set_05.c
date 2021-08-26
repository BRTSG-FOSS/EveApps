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
  *  Description: This is sample demo for EVE functions: ChineseFont
  */

#include "Platform.h"
#include "Gpu_Hal.h"
#include "Common.h"
#include "App.h"
#include "Samapp_Enable_API.h"

#ifdef SAMAPP_ENABLE_APIS_SET5

static EVE_HalContext *s_pHalContext;
/**
 * @brief API to demonstrate Chinese font
 *
 */
void SAMAPP_ChineseFont()
{
	/// SAMApp_ChineseFont_MetricBlock: 148 bytes
	///   Widths                       128 bytes
	///   Format                       4 bytes
	///   Stride                       4 bytes
	///   Max Width                    4 bytes
	///   Max Height                   4 bytes
	///   Raw Data Address in Decimal: 4 bytes
	int bytes = Ftf_Write_File_To_RAM_G(s_pHalContext, TEST_DIR "\\SAMApp_ChineseFont_MetricBlock.bin", RAM_G + 1000);
	Ftf_Write_File_To_RAM_G(s_pHalContext, TEST_DIR "\\SAMApp_ChineseFont_FontBmpData.bin", RAM_G + 1000 + bytes);

	EVE_CoCmd_dlStart(s_pHalContext);
	EVE_Cmd_wr32(s_pHalContext, CLEAR_COLOR_RGB(0xff, 0xff, 0xff));//set the background to white
	EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1));
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(32, 32, 32));//black color text

	EVE_CoCmd_text(s_pHalContext, (int16_t)(s_pHalContext->Width / 2), 20, 27, OPT_CENTER, "FangSong Font L8 Traditional Chinese");
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 0, 0));//black color text
	EVE_Cmd_wr32(s_pHalContext, BITMAP_HANDLE(7));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_SOURCE(196));

	EVE_Cmd_wr32(s_pHalContext, BITMAP_LAYOUT(L8, 28, 34));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_SIZE(NEAREST, BORDER, BORDER, 28, 34));

	EVE_CoCmd_setFont(s_pHalContext, 7, RAM_G + 1000);

	EVE_CoCmd_text(s_pHalContext, (int16_t)(s_pHalContext->Width / 2), 80, 7, OPT_CENTER, "\x01\x02\x03\x04\x05");
	EVE_CoCmd_text(s_pHalContext, (int16_t)(s_pHalContext->Width / 2), 80 + 34, 7, OPT_CENTER, "\x06\x07\x08\x09\x0A");
	EVE_CoCmd_text(s_pHalContext, (int16_t)(s_pHalContext->Width / 2), 80 + 34 + 34, 7, OPT_CENTER, "\x0B\x0C\x0D\x0E\x0F");
	EVE_CoCmd_text(s_pHalContext, (int16_t)(s_pHalContext->Width / 2), 80 + 34 + 34 + 34, 7, OPT_CENTER, "\x10\x11\x12\x13\x14");

	EVE_CoCmd_button(s_pHalContext, (int16_t)(s_pHalContext->Width / 2), 80 + 34 + 34 + 34 + 34, 80, 34, 7, 0, "\x15\x16");

	EVE_Cmd_wr32(s_pHalContext, DISPLAY());
	EVE_CoCmd_swap(s_pHalContext);

	/* Wait till coprocessor completes the operation */
	EVE_Cmd_waitFlush(s_pHalContext);

	EVE_sleep(3000);//timeout for snapshot to be performed by coprocessor
}

/**
 * @brief Draw set 5
 *
 */
void App_Set_05_Draw() {
#if defined(MSVC_PLATFORM) || defined(BT8XXEMU_PLATFORM) || defined(FT900_PLATFORM) || defined(FT93X_PLATFORM)
	SAMAPP_ChineseFont();
#endif
}

/**
 * @brief Init set 5
 *
 */
void App_Set_05_Init(EVE_HalContext* pHalContext) {
	s_pHalContext = pHalContext;
	Draw_Text(s_pHalContext, "Set5   START");
}

/**
 * @brief Deinit set 5
 *
 */
void App_Set_05_Deinit() {
	Draw_Text(s_pHalContext, "Set5   END!");
}

#endif // SAMAPP_ENABLE_APIS_SET5
