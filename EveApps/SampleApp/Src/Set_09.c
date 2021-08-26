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
  *     > Configurable clock frequency
  *     > Configurable GPIO/IO drive strength
  */

#include "Platform.h"
#include "Gpu_Hal.h"
#include "Common.h"
#include "App.h"
#include "Samapp_Enable_API.h"

#ifdef SAMAPP_ENABLE_APIS_SET9

static EVE_HalContext *s_pHalContext;

/**
 * @brief change the clock frequency for FT81X series chips
 *
 */
void SAMAPP_ChangeFreq() {
	fadeout(s_pHalContext);
	Play_MuteSound(s_pHalContext); //Play mute sound to avoid pop sound
	EVE_Host_powerModeSwitch(s_pHalContext, GPU_SLEEP_M); //put device in sleep mode
	EVE_sleep(50);

	//change the clock frequency to 60mhz during sleep mode to avoid any behaviors during the clock transition period.
	EVE_Host_selectSysClk(s_pHalContext, GPU_SYSCLK_48M); //change system frequency to 48mhz

	EVE_Host_powerModeSwitch(s_pHalContext, GPU_ACTIVE_M); //put device in active mode
	EVE_sleep(50);
	Draw_Text(s_pHalContext, "System clock is now at 48mhz.");
	fadein(s_pHalContext);
	EVE_sleep(3000);

	fadeout(s_pHalContext);
	Play_MuteSound(s_pHalContext); //Play mute sound to avoid pop sound
	EVE_Host_powerModeSwitch(s_pHalContext, GPU_SLEEP_M);
	EVE_sleep(50);

	//change the clock frequency to 60mhz during sleep mode to avoid any behaviors during the clock transition period.
	EVE_Host_selectSysClk(s_pHalContext, GPU_SYSCLK_60M); //change system frequency to 60mhz

	EVE_Host_powerModeSwitch(s_pHalContext, GPU_ACTIVE_M);
	EVE_sleep(50);
	Draw_Text(s_pHalContext, "System clock is now at 60mhz.");
	fadein(s_pHalContext);
	EVE_sleep(3000);
}

/**
 * @brief when some ROMs and ADC are not being used they can be powered off to save power
 *
 */
void SAMAPP_81X_PowerOffComponents() {
	//Application should retain the ROMs and ADCs setting on its own as the state of the ROMs and ADCs are not readable from the chip.
	EVE_CoCmd_dlStart(s_pHalContext);
	EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1));
	EVE_CoCmd_text(s_pHalContext, (int16_t)(s_pHalContext->Width / 2), (int16_t)(s_pHalContext->Height / 2 - 50), 31, OPT_CENTER, "Some ROMs and ADCs can be");
	EVE_CoCmd_text(s_pHalContext, (int16_t)(s_pHalContext->Width / 2), (int16_t)(s_pHalContext->Height / 2 + 50), 31, OPT_CENTER, "powered off individually.");
	EVE_Cmd_wr32(s_pHalContext, DISPLAY());
	EVE_CoCmd_swap(s_pHalContext);
	EVE_Cmd_waitFlush(s_pHalContext);
	SAMAPP_ENABLE_DELAY_VALUE(3000);

	GPU_81X_PowerOffComponents(s_pHalContext, 0x20 | 0x10); //disable audio

	SAMAPP_ENABLE_DELAY_VALUE(1000);

	//Power on all ROMs and ADC
	GPU_81X_PowerOffComponents(s_pHalContext, 0x00);

	SAMAPP_ENABLE_DELAY_VALUE(1000);
}

/**
 * @brief The current of the GPIOs and IOs can be increased upto 20 milliamps or decrease down to 5 milliamps, by default the GPIOs and IOs specified in the GPU_81X_GPIO_GROUP_T table are outputing 5 milliamps.
 *
 */
void SAMAPP_81X_ChangePadDriveStrength() {
	fadeout(s_pHalContext);
	Play_MuteSound(s_pHalContext);  //Play mute sound to avoid pop sound
	GPU_81X_PadDriveStrength(s_pHalContext, GPU_10MA, GPU_GPIO0);  //change GPIO0

	Draw_Text(s_pHalContext, "GPIO0 is now driving 10 milliamps.");
	fadein(s_pHalContext);
	EVE_sleep(3000);
}

/**
 * @brief demonstration to hold the device during the system reset.
 *
 */
void SAMAPP_HoldResetState() {
	fadeout(s_pHalContext);
	Play_MuteSound(s_pHalContext);		//Play mute sound to avoid pop sound
	Draw_Text(s_pHalContext, "Reset state hold.");
	EVE_Host_coreReset(s_pHalContext);		//perform a device reset.
	EVE_Host_resetActive(s_pHalContext);		//hold the device in reset state
	EVE_sleep(3000);

	//during the device reset holding period, the application can perform operations that require the device to be non-operational or pre-operational status.

	EVE_Host_resetRemoval(s_pHalContext);	//exit reset state, the device will power on and enter into its default state.
	BootupConfig(s_pHalContext);		//reconfigure the display
	Draw_Text(s_pHalContext, "Reset state released.");
	fadein(s_pHalContext);
	EVE_sleep(3000);
}

/**
 * @brief Draw set 9
 *
 */
void App_Set_09_Draw() {
	Draw_Text(s_pHalContext, "Configurable clock frequency.");
	SAMAPP_ChangeFreq();

	SAMAPP_81X_PowerOffComponents();

	Draw_Text(s_pHalContext, "Configurable GPIO/IO drive strength..");
	SAMAPP_81X_ChangePadDriveStrength();
}

/**
 * @brief Init set 9
 *
 */
void App_Set_09_Init(EVE_HalContext* pHalContext) {
	s_pHalContext = pHalContext;
	Draw_Text(s_pHalContext, "Set9   Start!");
}

/**
 * @brief Deinit set 9
 *
 */
void App_Set_09_Deinit() {
	Draw_Text(s_pHalContext, "Set9   End!");
}

#endif // SAMAPP_ENABLE_APIS_SET9
