/*
 * Copyright (c) Bridgetek Pte Ltd
 *
 * THIS SOFTWARE IS PROVIDED BY BRIDGETEK PTE LTD "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL BRIDGETEK PTE LTD BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 * TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES LOSS OF USE, DATA, OR PROFITS OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. BRIDGETEK DRIVERS MAY BE USED ONLY
 * IN CONJUNCTION WITH PRODUCTS BASED ON BRIDGETEK PARTS. BRIDGETEK DRIVERS MAY BE DISTRIBUTED
 * IN ANY FORM AS LONG AS LICENSE INFORMATION IS NOT MODIFIED. IF A CUSTOM VENDOR ID AND/OR PRODUCT
 * ID OR DESCRIPTION STRING ARE USED, IT IS THE RESPONSIBILITY OF THE PRODUCT MANUFACTURER TO
 * MAINTAIN ANY CHANGES AND SUBSEQUENT WHQL RE-CERTIFICATION AS A RESULT OF MAKING THESE CHANGES.
 */

 /* Abstract: Application to demonstrate function of EVE.
  * Author : Bridgetek
  */

#include "Common.h"
#include "Platform.h"
#include "EVE_CoCmd.h"
#include "Animation.h"

#define SAMAPP_DELAY EVE_sleep(2000)

static EVE_HalContext s_halContext;
static EVE_HalContext* s_pHalContext;
void SAMAPP_Animation();

int main(int argc, char* argv[])
{
    s_pHalContext = &s_halContext;
    Gpu_Init(s_pHalContext);

    // read and store calibration setting
#if !defined(BT8XXEMU_PLATFORM) && GET_CALIBRATION == 1
    Esd_Calibrate(s_pHalContext);
    Calibration_Save(s_pHalContext);
#endif

    Flash_Init(s_pHalContext, TEST_DIR "/Flash/BT81X_Flash.bin", "BT81X_Flash.bin");
    EVE_Util_clearScreen(s_pHalContext);

    char *info[] =
    {  "EVE Sample Application",
        "This sample demonstrate the using of animation", 
        "",
        ""
    }; 

    while (TRUE) {
        WelcomeScreen(s_pHalContext, info);

        SAMAPP_Animation();

        EVE_Util_clearScreen(s_pHalContext);

        EVE_Hal_close(s_pHalContext);
        EVE_Hal_release();

        /* Init HW Hal for next loop*/
        Gpu_Init(s_pHalContext);
#if !defined(BT8XXEMU_PLATFORM) && GET_CALIBRATION == 1
        Calibration_Restore(s_pHalContext);
#endif
    }
    return 0;
}

/**
* @brief API to demonstrate CMD_RUNANIM without host control on RAM_G
*/
void SAMAPP_Animation_RAM_G() {
#if EVE_SUPPORT_GEN == EVE4
#define ANIM_ADDR     (19584) // start address of .object part in battery.ramg.anim.bin

    uint32_t cmd;
    uint32_t waitmask = -1;
    uint32_t play = 1024 * 512;
    uint32_t cmdwrite = 0;
    const int16_t xAnim = 350;
    const int16_t yAnim = 260;
    uint32_t ch = 0;

    Draw_Text(s_pHalContext, "Example for: Run animation without host control on RAM_G");
    if (!FlashHelper_SwitchFullMode(s_pHalContext))
    {
        APP_ERR("Flash is not able to switch full mode");
        return;
    }
    EVE_CoCmd_flashRead(s_pHalContext, 0, 8937216, 19712);
    EVE_Cmd_waitFlush(s_pHalContext);

    /// Check Cmd_RunAnim with play control
    EVE_CoCmd_memZero(s_pHalContext, play, 1);
    EVE_CoCmd_animStartRam(s_pHalContext, ch, ANIM_ADDR, ANIM_LOOP);
    EVE_CoCmd_animXY(s_pHalContext, ch, xAnim, yAnim);

    EVE_Cmd_waitFlush(s_pHalContext);
    cmdwrite = EVE_Cmd_wp(s_pHalContext);
    EVE_CoCmd_runAnim(s_pHalContext, waitmask, play);
    EVE_Hal_flush(s_pHalContext);

    cmd = EVE_Hal_rd32(s_pHalContext, (RAM_CMD + cmdwrite + 0) & EVE_CMD_FIFO_MASK);
    waitmask = EVE_Hal_rd32(s_pHalContext, (RAM_CMD + cmdwrite + 4) & EVE_CMD_FIFO_MASK);
    uint32_t p = EVE_Hal_rd32(s_pHalContext, (RAM_CMD + cmdwrite + 8) & EVE_CMD_FIFO_MASK);

    APP_INF("cmd: 0x%08x, waitmask:%u, play:%u", cmd, waitmask, p);
    SAMAPP_DELAY;
    EVE_Hal_wr32(s_pHalContext, play, 1);
#endif
}

/**
* @brief API to demonstrate CMD_RUNANIM
* Run animation without host control on Flash
*/
void SAMAPP_Animation_flash()
{
#if EVE_SUPPORT_GEN == EVE4
#define ANIM_ADDR     (8609600) // offset of abstract.anim.object
#define FRAME_COUNT   (40)

    uint32_t waitmask = -1;
    uint32_t play = 1024 * 512;
    const int16_t xAnim = 350;
    const int16_t yAnim = 260;
    uint32_t ch = 0;

    Draw_Text(s_pHalContext, "Example for: Run animation without host control on Flash");
    if (!FlashHelper_SwitchFullMode(s_pHalContext))
    {
        APP_ERR("Flash is not able to switch full mode");
        return;
    }

    EVE_CoCmd_memZero(s_pHalContext, play, 1);
    EVE_CoCmd_animStart(s_pHalContext, ch, ANIM_ADDR, ANIM_ONCE);
    EVE_CoCmd_animXY(s_pHalContext, ch, xAnim, yAnim);
    EVE_Cmd_waitFlush(s_pHalContext);
    EVE_CoCmd_runAnim(s_pHalContext, waitmask, play);
    EVE_Hal_flush(s_pHalContext);

    SAMAPP_DELAY;
    EVE_Hal_wr32(s_pHalContext, play, 1);
#endif // EVE_SUPPORT_GEN == EVE4
}

/**
* @brief API to demonstrate animation from Flash by AnimDraw
*/
void SAMAPP_Animation_control()
{
#if defined (BT81X_ENABLE) && (defined(MSVC_PLATFORM) || defined(BT8XXEMU_PLATFORM)) // Win32 BT81X only
#define ANIM_ADDR 8609600 //address of abstract.anim.object
#define FRAME_COUNT 40
    uint8_t txtAnim_ONCE[] = "Playing Animation by ANIM_ONCE";
    uint8_t txtAnim_LOOP[] = "Playing Animation by ANIM_LOOP";
    uint8_t txtAnim_HOLD[] = "Playing Animation by ANIM_HOLD";

    uint8_t txtPress2Stop[] = "Press \"Stop Anim\" again to exit";

    uint32_t prAnimLoop = ANIM_ONCE;
    const uint8_t* txtAnimLabel = txtAnim_ONCE;

    int i = 0;
    const int timeOut = 80 * 500; // exit after 500*80 = 40.000 frames, about 400s (7min)
    int countStop = 0; // countStop = 2 to exit while loop

    Draw_Text(s_pHalContext, "Example for: ANIM_ONCE, ANIM_LOOP, ANIM_HOLD");

    /* Switch Flash to FULL Mode */
    FlashHelper_SwitchFullMode(s_pHalContext);

    uint16_t channel = 30;
    uint8_t tag = 0;
    uint8_t oldTag = 0;
    uint8_t isPlaying = 0;
    const int16_t xAnim = 350;
    const int16_t yAnim = 260;
    const int xLbl = 200;
    const int yLbl = 20;
    const int xStart = 650;
    const int yStart = 50;
    const int xStop = 650;
    const int yStop = 160;
    const int w = 130;
    const int h = 100;

    while (i++ < timeOut && countStop < 2) {
        EVE_CoCmd_dlStart(s_pHalContext);
        EVE_Cmd_wr32(s_pHalContext, CLEAR_COLOR_RGB(0, 0, 0));
        EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1));

        int8_t newTag = EVE_Hal_rd8(s_pHalContext, REG_TOUCH_TAG);

        if (newTag != oldTag) {
            tag = newTag;
            oldTag = newTag;
        }
        else {
            tag = 0;
        }

        // Show text information
        if (isPlaying) {
            EVE_CoCmd_text(s_pHalContext, xLbl, yLbl, 27, 0, txtAnimLabel);
            EVE_CoCmd_animDraw(s_pHalContext, channel);

            EVE_Cmd_wr32(s_pHalContext, TAG(1));
            EVE_CoCmd_button(s_pHalContext, xStart, yStart, w, h, 27, 0, "Change Anim");
        }
        else {
            if (countStop == 0) {
                EVE_CoCmd_text(s_pHalContext, xLbl, yLbl, 27, 0, "Press \"Start Anim\" to start animation");
            }
            else if (countStop > 0) {
                EVE_CoCmd_text(s_pHalContext, xLbl, yLbl, 27, 0, txtPress2Stop);
            }

            EVE_Cmd_wr32(s_pHalContext, TAG(1));
            EVE_CoCmd_button(s_pHalContext, xStart, yStart, w, h, 27, 0, "Start Anim");
        }

        EVE_Cmd_wr32(s_pHalContext, TAG(2));
        EVE_CoCmd_button(s_pHalContext, xStop, yStop, w, h, 27, 0, "Stop Anim");

        if (tag == 1) {
            if (isPlaying == 1) {
                EVE_CoCmd_animStop(s_pHalContext, channel);
            }

            isPlaying = 1;
            countStop = 0;

            EVE_CoCmd_animStart(s_pHalContext, channel, ANIM_ADDR, prAnimLoop);
            EVE_CoCmd_animXY(s_pHalContext, channel, xAnim, yAnim);

            // change AnimLoop attribute for the next user button press
            // change text lable for the current press
            // ONCE -> HOLD -> LOOP -> ONCE -> ...
            switch (prAnimLoop) {
            case ANIM_ONCE:
                prAnimLoop = ANIM_HOLD;
                txtAnimLabel = txtAnim_ONCE;
                break;
            case ANIM_HOLD:
                prAnimLoop = ANIM_LOOP;
                txtAnimLabel = txtAnim_HOLD;
                break;
            case ANIM_LOOP:
                prAnimLoop = ANIM_ONCE;
                txtAnimLabel = txtAnim_LOOP;
                break;
            default: break;
            }
        }
        if (tag == 2) {
            isPlaying = 0;
            EVE_CoCmd_animStop(s_pHalContext, channel);
            countStop++;
        }

        EVE_Cmd_wr32(s_pHalContext, DISPLAY());
        EVE_CoCmd_swap(s_pHalContext);
        EVE_Cmd_waitFlush(s_pHalContext);
    }
#endif // Win32 BT81X only
}

/**
* @brief API to demonstrate 32-bit register REG_ANIM_ACTIVE
*/
void SAMAPP_Animation_animeAtive() 
{
    // This example using below asset on flash:
    //  battery.ramg.anim.bin : 8937216 : 19712

#if EVE_SUPPORT_GEN == EVE4
#define ANIM_ADDR     (19584) // start address of .object part in battery.ramg.anim.bin

    char str[1000];
    Draw_Text_Format(s_pHalContext, "Example for: REG_ANIM_ACTIVE usage");
    if (!FlashHelper_SwitchFullMode(s_pHalContext))
    {
        APP_ERR("Flash is not able to switch full mode");
        return;
    }
    EVE_CoCmd_flashRead(s_pHalContext, 0, 8937216, 19712);
    EVE_Cmd_waitFlush(s_pHalContext);

    uint16_t channel = 1;
    const int16_t xAnim = (uint16_t)(s_pHalContext->Width / 2);
    const int16_t yAnim = (uint16_t)(s_pHalContext->Height/ 2);;

    EVE_CoCmd_animStartRam(s_pHalContext, channel, ANIM_ADDR, ANIM_ONCE);
    EVE_CoCmd_animXY(s_pHalContext, channel, xAnim, yAnim);
    const int MAX_LOOP = 100;
    for (int i = 0; i < MAX_LOOP; i++) {
        EVE_CoCmd_dlStart(s_pHalContext);
        EVE_Cmd_wr32(s_pHalContext, CLEAR_COLOR_RGB(255, 255, 255));
        EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1));
        EVE_CoCmd_animDraw(s_pHalContext, channel);

        uint32_t reg_anim_active = EVE_Hal_rd32(s_pHalContext, REG_ANIM_ACTIVE);
        EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0, 0, 0));
        snprintf(str, 1000, "REG_ANIM_ACTIVE: %u\n", reg_anim_active);
        EVE_CoCmd_text(s_pHalContext, 
            xAnim, 30, 30, OPT_CENTER, str);

        EVE_Cmd_wr32(s_pHalContext, DISPLAY());
        EVE_CoCmd_swap(s_pHalContext);
        EVE_Hal_flush(s_pHalContext);

        if (0 == reg_anim_active) {
            break;
        }

        EVE_sleep(500);
    }

    SAMAPP_DELAY;
#endif
}

/**
* @brief API to demonstrate animation from Flash by AnimFrame
*/
void SAMAPP_Animation_animFrame() {
#if defined (BT81X_ENABLE) && (defined(MSVC_PLATFORM) || defined(BT8XXEMU_PLATFORM)) // Win32 BT81X only
#define ANIM_ADDR 8609600 //address of abstract.anim.object
#define FRAME_COUNT   40
    uint32_t prAnimLoop = ANIM_ONCE;

    Draw_Text(s_pHalContext, "Example for: ANIMATION from Flash by AnimFrame");
    if (!FlashHelper_SwitchFullMode(s_pHalContext))
    {
        APP_ERR("Flash is not able to switch full mode");
        return;
    }

    uint16_t channel = 30;
    const int16_t xAnim = 400;
    const int16_t yAnim = 240;

    EVE_CoCmd_animStart(s_pHalContext, channel, ANIM_ADDR, prAnimLoop);
    EVE_CoCmd_animXY(s_pHalContext, channel, xAnim, yAnim);

    for (uint16_t frame = 0; frame < FRAME_COUNT; frame++) {
        EVE_CoCmd_dlStart(s_pHalContext);
        EVE_Cmd_wr32(s_pHalContext, CLEAR_COLOR_RGB(0, 0, 0));
        EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1));

        EVE_CoCmd_animFrame(s_pHalContext, xAnim, yAnim, ANIM_ADDR, frame);

        EVE_Cmd_wr32(s_pHalContext, DISPLAY());
        EVE_CoCmd_swap(s_pHalContext);
        EVE_Cmd_waitFlush(s_pHalContext);
    }

    EVE_CoCmd_animStop(s_pHalContext, channel);
    SAMAPP_DELAY;
#endif // Win32 BT81X only
}

/**
* @brief API to demonstrate CMD_ANIMFRAMERAM
*/
void SAMAPP_Animation_animFrameRam()
{
#if EVE_SUPPORT_GEN == EVE4
#define ANIM_ADDR     (19584) // start address of .object part in battery.ramg.anim.bin
#define FRAME_COUNT   4

    Draw_Text(s_pHalContext, "Example for: ANIMATION from RAMG by Cmd_animFrameRam");
    if (!FlashHelper_SwitchFullMode(s_pHalContext))
    {
        APP_ERR("Flash is not able to switch full mode");
        return;
    }

    for (uint16_t frame = 0; frame < FRAME_COUNT; frame++)
    {
        EVE_CoCmd_dlStart(s_pHalContext);
        EVE_Cmd_wr32(s_pHalContext, CLEAR_COLOR_RGB(0, 0, 0));
        EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1));

        EVE_CoCmd_animFrameRam(s_pHalContext, (uint16_t) (s_pHalContext->Width / 2),
            (uint16_t) (s_pHalContext->Height / 2), ANIM_ADDR, frame);

        EVE_Cmd_wr32(s_pHalContext, DISPLAY());
        EVE_CoCmd_swap(s_pHalContext);
        EVE_Cmd_waitFlush(s_pHalContext);

        uint32_t reg_anim_active = EVE_Hal_rd32(s_pHalContext, REG_ANIM_ACTIVE);
        APP_DBG("frame: %u, REG_ANIM_ACTIVE: %u (channel mask)\n", frame, reg_anim_active);

        EVE_sleep(500);
    }
    SAMAPP_DELAY;
#endif // EVE_SUPPORT_GEN == EVE4
}

void SAMAPP_Animation() 
{
    SAMAPP_Animation_RAM_G();
    SAMAPP_Animation_flash();
    SAMAPP_Animation_control();
    SAMAPP_Animation_animeAtive();
    SAMAPP_Animation_animFrame();
    SAMAPP_Animation_animFrameRam();
}


