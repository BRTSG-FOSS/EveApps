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
 * Abstract: Initialize EVE and start sample SETs
 * Author : Bridgetek
 *
 * Revision History:
 * 1.0 - date 2018.07.30 - Initial for BT816
 */

#include "Platform.h"
#include "Common.h"
#include "App.h"

#include "Set_00.h"
#include "Set_01.h"
#include "Set_02.h"
#include "Set_03.h"
#include "Set_04.h"
#include "Set_05.h"
#include "Set_06.h"
#include "Set_07.h"
#include "Set_08.h"
#include "Set_09.h"
#include "Set_10.h"
#include "Set_11.h"
#include "Set_12.h"
#include "Set_13.h"
#include "Set_14.h"

static EVE_HalContext s_halContext;
static EVE_HalContext *s_pHalContext;
static App_Slots *s_slot;
static const App_Slots s_slots[][3] = {

#ifdef SAMAPP_ENABLE_APIS_SET0
	APP_SLOTS_SET_(00),
#endif
#ifdef SAMAPP_ENABLE_APIS_SET1
	APP_SLOTS_SET_(01),
#endif
#ifdef SAMAPP_ENABLE_APIS_SET2
	APP_SLOTS_SET_(02),
#endif
#ifdef SAMAPP_ENABLE_APIS_SET3
	APP_SLOTS_SET_(03),
#endif
#ifdef SAMAPP_ENABLE_APIS_SET4
	APP_SLOTS_SET_(04),
#endif
 #ifdef SAMAPP_ENABLE_APIS_SET5
	APP_SLOTS_SET_(05),
 #endif
 #ifdef SAMAPP_ENABLE_APIS_SET6
	APP_SLOTS_SET_(06),
 #endif
  #ifdef SAMAPP_ENABLE_APIS_SET7
	APP_SLOTS_SET_(07),
  #endif
 #ifdef SAMAPP_ENABLE_APIS_SET8
 	APP_SLOTS_SET_(08),
 #endif
 #ifdef SAMAPP_ENABLE_APIS_SET9
 	APP_SLOTS_SET_(09),
 #endif
#ifdef SAMAPP_ENABLE_APIS_SET10
	APP_SLOTS_SET_(10),
#endif
#ifdef SAMAPP_ENABLE_APIS_SET11
	APP_SLOTS_SET_(11),
#endif
#ifdef SAMAPP_ENABLE_APIS_SET12
	APP_SLOTS_SET_(12),
#endif
#ifdef SAMAPP_ENABLE_APIS_SET13
	APP_SLOTS_SET_(13),
#endif
#ifdef SAMAPP_ENABLE_APIS_SET14
	APP_SLOTS_SET_(14),
#endif
};

/* Header of raw data containing properties of bitmap */
SAMAPP_Bitmap_header_t SAMAPP_Bitmap_RawData_Header[] =
{
	/* format,width,height,stride,arrayoffset */
	{RGB565,	40,	40,	40 * 2,	0	},
#ifdef FT81X_ENABLE
	{PALETTED4444,	40,	40,	40,	0	},
	{PALETTED8,	480,	272,	480,	0	},
	{ PALETTED8,	802,	520,	802,	0 },
#else
	{PALETTED,	40,	40,	40,	0	},
	{PALETTED,	480,	272,	480,	0	},
#endif
};

/**
 * @brief Switch to next set
 *
 */
static void switchPageNext() {
	int numSlot = sizeof(s_slots) / sizeof(App_Slots[3]);
	for (int i = 0; i < numSlot; i++) {
		if (s_slot == s_slots[i]) {
			if (i < (numSlot - 1)) {
				s_slot = s_slots[i + 1];
			}
			else {
				s_slot = s_slots[0];
			}
			break;
		}
	}
}

int32_t mainLoop() {
	int numSlot = sizeof(s_slots) / sizeof(App_Slots[3]);

	// exit if no SET to run
	if (numSlot < 1) {
		APP_INF("No SET enabled, nothing to run");
		EVE_sleep(1000);
		exit(0);
	}
	s_slot = s_slots[0];

	EVE_Util_clearScreen(s_pHalContext);

	while (TRUE) {
#if defined(EVE_FLASH_AVAILABLE)
		FlashHelper_SwitchFullMode(s_pHalContext);
#endif
		s_slot->Init(s_pHalContext);
		s_slot->Draw();
		s_slot->DeInit();

		EVE_Util_clearScreen(s_pHalContext);

#if defined(EVE_FLASH_AVAILABLE)
		FlashHelper_SwitchState(s_pHalContext, FLASH_STATUS_DETACHED);
#endif
		EVE_Hal_close(s_pHalContext);
		EVE_Hal_release();

		/* Init HW Hal for next loop*/
		Gpu_Init(s_pHalContext);
#if !defined(BT8XXEMU_PLATFORM) && GET_CALIBRATION == 1
		Calibration_Restore(s_pHalContext);
#endif

		EVE_Util_loadSdCard(s_pHalContext);
#if defined(EVE_FLASH_AVAILABLE)
		FlashHelper_SwitchState(s_pHalContext, FLASH_STATUS_FULL);
#endif

#if defined(FT9XX_PLATFORM)
		sdhost_card_detect();
		sdhost_card_init();
#endif

		switchPageNext();
	}

	return 0;
}

#if defined(ARDUINO_PLATFORM)
void EVE_emuMain(int argc, char *argv[])
#else
int main(int argc, char *argv[])
#endif
{
	s_pHalContext = &s_halContext;
	Gpu_Init(s_pHalContext);

	if (s_pHalContext->Width > 1023)
		Setup_Precision(3); // -2048 to 2047. Note: Use VP with vertex

	// read and store calibration setting
#if !defined(BT8XXEMU_PLATFORM) && GET_CALIBRATION == 1
	Esd_Calibrate(s_pHalContext);
	Calibration_Save(s_pHalContext);
#endif

	Flash_Init(s_pHalContext, TEST_DIR "/Flash/BT81X_Flash.bin", "BT81X_Flash.bin");

	mainLoop();

#if defined(MSVC_PLATFORM) || defined(FT900_PLATFORM) || defined(FT93X_PLATFORM)
	return 0;
#endif
}
