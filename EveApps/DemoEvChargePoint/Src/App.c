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

#include "Def.h"
#include "Screen_Saver.h"
#include "Choose_Language.h"
#include "Authenticate.h"
#include "Plugin.h"
#include "Transaction.h"
#include "Charging.h"
#include "Report.h"

EVE_HalContext *g_pHalContext;
E_LANG g_language = LANG_EN;

static EVE_HalContext s_halContext;

static const E_PAGE PAGE_HOME = PAGE_BEGIN;
static E_PAGE s_currLanguage = PAGE_MAX;
static E_PAGE s_lastLanguage = PAGE_BEGIN;

typedef void(*P_FUNC)();

static P_FUNC fp_draw = 0;
static P_FUNC fp_process = 0;

static uint32_t s_startTime;

static P_FUNC ARR_FUNC_INIT[] = { &ss_init, &cl_init, &ath_init, &pl_init, &tr_init, &ch_init, &rp_init };
static P_FUNC ARR_FUNC_DRAW[] = { &ss_drawing, &cl_drawing, &ath_drawing, &pl_drawing, &tr_drawing, &ch_drawing, &rp_drawing };
static P_FUNC ARR_FUNC_PROCESS[] = { &ss_process_event, &cl_process_event, &ath_process_event, &pl_process_event, &tr_process_event, &ch_process_event, &rp_process_event };

int s_isSwitchPage =-1 ;

void restart_screen_saver() {
    s_startTime = GET_CURR_MILLIS();
}

void switch_page(E_PAGE page) {
    if (page < PAGE_BEGIN && page >= PAGE_MAX)
        return;
    s_currLanguage = page;
    s_isSwitchPage++;

    Display_End(g_pHalContext);

    int index = s_currLanguage - PAGE_BEGIN;
    fp_draw = ARR_FUNC_DRAW[index];
    fp_process = ARR_FUNC_PROCESS[index];
    ARR_FUNC_INIT[index]();
    restart_screen_saver();

    sync_up((uint8_t)page);
}

void switch_next_page() {
    s_currLanguage++;

    if (s_currLanguage >= PAGE_MAX) {
        s_currLanguage = PAGE_BEGIN;
}
    s_lastLanguage = s_currLanguage;
    switch_page(s_currLanguage);
}

void switch_page_from_screen_saver() {
    s_currLanguage = s_lastLanguage;

    if (s_currLanguage == PAGE_BEGIN) {
        s_currLanguage++;
    }
    else if (s_currLanguage >= PAGE_MAX) {
        s_currLanguage = PAGE_BEGIN;
    }
    switch_page(s_currLanguage);
}

void processEvents() {
    uint32_t time_lap = GET_CURR_MILLIS() - s_startTime;

    Gesture_Renew(g_pHalContext);

    fp_process();
    if (time_lap > SS_TIME_OUT) {
        s_startTime = GET_CURR_MILLIS();
        switch_page(PAGE_SCREEN_SAVER);
    }
}

static void mainLoop() {
    FlashHelper_SwitchFullMode(g_pHalContext);
    Gpu_CoCmd_FlashRead(g_pHalContext, RAM_G, 4096, RAM_DL_SIZE);
    App_WrCoCmd_Buffer(g_pHalContext, VERTEX_FORMAT(FRACTION));
    switch_language();
    switch_page(PAGE_HOME);
    s_startTime = GET_CURR_MILLIS();

    char* s[] = {
        "page_screensaver ",
        "page_language    ",
        "page_authenticate",
        "page_plugin      ",
        "page_transition  ",
        "page_charge      ",
        "page_report      ",
    };

    EVE_Util_clearScreen(g_pHalContext);
    int rc = 0;
    int grc = 0;
	

    uint32_t gt0 = EVE_millis();
    uint32_t t0 = EVE_millis();
    int i = 0;
    while (TRUE) {
        Display_Start(g_pHalContext);
        App_WrCoCmd_Buffer(g_pHalContext, VERTEX_FORMAT(FRACTION));

        fp_draw();
        processEvents();
        Display_End(g_pHalContext);
        rc++;
        grc++;

        if (s_isSwitchPage == 1) {
            s_isSwitchPage = 0;
            uint32_t t1 = EVE_millis();
            float fps = 1000* 1.0 * rc / (t1 - t0);

            printf("%s FPS=%0.2f (%u frames in %u miliseconds)\n", s[i], fps, rc, t1-t0);
            i++;

            if (i == 7) {
                i = 0;
                fps = 1000*1.0 * grc / (t1 - gt0);
                printf("Program FPS=%0.2f(%u frames in %u miliseconds)\n", fps, grc, t1-gt0);
            }

            rc = 0;
            t0 = EVE_millis();
        }
    }

}

int32_t main(int32_t argc, char8_t *argv[])
{
    g_pHalContext = &s_halContext;
	Gpu_Init(g_pHalContext);

    // Store calibration setting
    Gpu_Hal_Wr32(g_pHalContext, REG_TOUCH_TRANSFORM_A, 0xfffefefc);
    Gpu_Hal_Wr32(g_pHalContext, REG_TOUCH_TRANSFORM_B, 0xfffffcbf);
    Gpu_Hal_Wr32(g_pHalContext, REG_TOUCH_TRANSFORM_C, 0x506adb4);
    Gpu_Hal_Wr32(g_pHalContext, REG_TOUCH_TRANSFORM_D, 0xfffffed1);
    Gpu_Hal_Wr32(g_pHalContext, REG_TOUCH_TRANSFORM_E, 0xfffefc79);
    Gpu_Hal_Wr32(g_pHalContext, REG_TOUCH_TRANSFORM_F, 0x32c3211);

    Flash_Init(g_pHalContext, TEST_DIR "/Flash/BT81X_Flash.bin", "BT81X_Flash.bin");

	mainLoop();

#if defined(MSVC_PLATFORM) || defined(FT900_PLATFORM) || defined(FT93X_PLATFORM)
	return 0;
#endif
}
