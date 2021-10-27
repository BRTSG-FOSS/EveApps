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
 *
 * Revision History:
 * 1.0 - date 2018.07.30 - Initial for BT816
 */

#include "Common.h"
#include "App.h"
#include "DemoAudioPlayback.h"

static EVE_HalContext s_halContext;
static EVE_HalContext *s_pHalContext;

static void mainLoop() {
	EVE_Util_clearScreen(s_pHalContext);

	while (TRUE) {

		DemoAudioPlayback(s_pHalContext);
		
		EVE_Util_clearScreen(s_pHalContext);

		EVE_Hal_close(s_pHalContext);
		EVE_Hal_release();

		/* Init HW Hal for next loop*/
		Gpu_Init(s_pHalContext);
#if !defined(BT8XXEMU_PLATFORM) && GET_CALIBRATION == 1
		Calibration_Restore(s_pHalContext);
#endif
	}
}

#if defined(ARDUINO_PLATFORM)
void EVE_emuMain(int argc, char *argv[])
#else
int main(int argc, char *argv[])
#endif
{
	s_pHalContext = &s_halContext;
	Gpu_Init(s_pHalContext);

#ifndef EVE_FLASH_AVAILABLE
#error Platform not support
#endif

	if (s_pHalContext->Width > 1023)
		Setup_Precision(3); // -2048 to 2047. Note: Use VP with vertex

    {
        int sz = Ftf_Write_File_To_RAM_G(s_pHalContext, 
            TEST_DIR "/relaxed-inspiration-ig-version-short-30s-6579.raw", 0);
        EVE_Hal_wr8(s_pHalContext, REG_VOL_PB, 0);
        APP_DBG_U(sz);
        // Start playback
        EVE_Hal_wr32(s_pHalContext, REG_PLAYBACK_START, 0);
        EVE_Hal_wr32(s_pHalContext, REG_PLAYBACK_LENGTH, sz);
        EVE_Hal_wr16(s_pHalContext, REG_PLAYBACK_FREQ, 44100);
        EVE_Hal_wr8(s_pHalContext, REG_PLAYBACK_FORMAT, ADPCM_SAMPLES);
        EVE_Hal_wr8(s_pHalContext, REG_PLAYBACK_PAUSE, 0);
        EVE_Hal_wr8(s_pHalContext, REG_PLAYBACK_LOOP, 0);
        EVE_Hal_wr8(s_pHalContext, REG_PLAYBACK_PLAY, 1);
        EVE_Cmd_waitFlush(s_pHalContext);
        /// Volume increase from 0
        uint32_t vol = 0;
        int ins = 1;
        while (vol < 100) {
            EVE_Hal_wr8(s_pHalContext, REG_VOL_PB, vol);
            vol += ins;
            ins++;
            EVE_sleep(1);
        }
    }
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
