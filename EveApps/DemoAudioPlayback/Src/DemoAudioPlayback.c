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

static EVE_HalContext *s_pHalContext;

#define ENABLE_ALL_WAV_IN_RAM_G 0

#define BUTTON_MARGIN  250

Img_t g_Img[] = {
    //index addressFlash addressRamg size      x    y  w  h    bitmapLayout extFormat    tag isFlash
    { 0     ,12187264    ,0          ,6400   ,0   ,0 ,80 ,80 ,COMPRESSED_RGBA_ASTC_4x4_KHR       ,0           ,1  ,1 },
    { 1     ,12193664    ,0          ,6400   ,0   ,0 ,80 ,80 ,COMPRESSED_RGBA_ASTC_4x4_KHR       ,0           ,2  ,1 },
};

#define ANIM_ADDR     (10529664) // address of "output.anim.object" from Flash map after generating Flash
#define FRAME_COUNT   (99)

#define BTN_STATE_PLAY   0
#define BTN_STATE_PAUSE  1

typedef struct _MEDIA_BTN {
    int x, y;
    int tag;
    int state; //BTN_STATE_...
    int isPausing;
}MEDIA_BTN;

#define BTN_NUM 3
MEDIA_BTN btn[BTN_NUM];

char* media_info[] = {
    "8 Bit signed PCM", 
    "8 Bit u-Law", 
    "4 Bit IMA ADPCM", 
};

typedef struct _AUDIO {
    int addr, len, freq, format;
}AUDIO;
AUDIO wav[3] = {
    {10530496, 662208, 22050, LINEAR_SAMPLES},
    {11192704, 661632, 22050, ULAW_SAMPLES},
    {11854336, 332928, 22050, ADPCM_SAMPLES},
};

#define TAG_SLIDER (BTN_NUM+1)
static uint32_t gs_volume = 100;

void changeAudio(AUDIO* ad) {
    // Mute audio
    EVE_Hal_wr8(s_pHalContext, REG_VOL_PB, 0);

#if !ENABLE_ALL_WAV_IN_RAM_G
    // Copy data
    EVE_CoCmd_flashRead(s_pHalContext, 0, ad->addr, ad->len > RAM_G_SIZE? RAM_G_SIZE:ad->len);
    EVE_Cmd_waitFlush(s_pHalContext);
#endif

    // Start playback
    EVE_Hal_wr32(s_pHalContext, REG_PLAYBACK_START, 0);
    EVE_Hal_wr32(s_pHalContext, REG_PLAYBACK_LENGTH, ad->len);
    EVE_Hal_wr16(s_pHalContext, REG_PLAYBACK_FREQ, ad->freq);
    EVE_Hal_wr8(s_pHalContext, REG_PLAYBACK_FORMAT, ad->format);
    EVE_Hal_wr8(s_pHalContext, REG_PLAYBACK_PAUSE, 0);
    EVE_Hal_wr8(s_pHalContext, REG_PLAYBACK_LOOP, 0);
    EVE_Hal_wr8(s_pHalContext, REG_PLAYBACK_PLAY, 1);
    EVE_Cmd_waitFlush(s_pHalContext);
    
    /// Volume increase from 0
    uint32_t vol = 0;
    int ins = 1;
    while (vol < gs_volume) {
        EVE_Hal_wr8(s_pHalContext, REG_VOL_PB, vol);
        vol += ins;
        ins++;
        EVE_sleep(1);
    }
}

void process_event() {
    Gesture_Renew(s_pHalContext);
    Gesture_Touch_t* touch = Gesture_Get(s_pHalContext);
    uint32_t tracker = touch->tagTrackTouched & 0xff;
    uint32_t val = touch->tagTrackTouched >> 16;

    if (touch->tagReleased > 0 && touch->tagReleased <= BTN_NUM) {
        if (btn[touch->tagReleased - 1].state == BTN_STATE_PAUSE) {
            /// Pause playback if playing
            btn[touch->tagReleased - 1].state = BTN_STATE_PLAY;
            btn[touch->tagReleased - 1].isPausing = true;

            EVE_Hal_wr8(s_pHalContext, REG_PLAYBACK_PAUSE, 1);
            EVE_Cmd_waitFlush(s_pHalContext);
            return;
        }
        else if (btn[touch->tagReleased - 1].isPausing == true) {
            /// resume playback if pausing
            EVE_Hal_wr8(s_pHalContext, REG_PLAYBACK_PAUSE, 0);
            btn[touch->tagReleased - 1].state = BTN_STATE_PAUSE;
            btn[touch->tagReleased - 1].isPausing = false;
            return;
        }
        else {
            /// Start new playback otherwise
            for (int i = 0; i < BTN_NUM; i++) {
                btn[i].state = BTN_STATE_PLAY;
                btn[i].isPausing = false;
            }

            btn[touch->tagReleased - 1].state = BTN_STATE_PAUSE;
            btn[touch->tagReleased - 1].isPausing = false;
            changeAudio(&wav[touch->tagReleased - 1]);
        }
    }
    else {
        uint32_t playback = EVE_Hal_rd32(s_pHalContext, REG_PLAYBACK_PLAY);

        /// Played to the end, stop.
        if (playback == 0) {
            for (int i = 0; i < BTN_NUM; i++) {
                btn[i].state = BTN_STATE_PLAY;
                btn[i].isPausing = false;
            }
        }
    }

    /// Adjust volume
    if (tracker == TAG_SLIDER) {
        gs_volume = val * 255 / 65535;
        EVE_Hal_wr8(s_pHalContext, REG_VOL_PB, gs_volume);
    }

}

void drawUi() {
    uint16_t channel = 30;
    static uint32_t frame = 0;

    const int16_t xAnim = 400;
    const int16_t yAnim = 240;

    Display_Start(s_pHalContext);

    EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 255, 255));
    
    // Disable animation on WSVGA and WXGA
    if (s_pHalContext->Width < 1024) {
        EVE_CoCmd_animFrame(s_pHalContext, xAnim, yAnim, ANIM_ADDR, frame);
    }

    frame++;
    if (frame > FRAME_COUNT) frame = 0;

    for (int i = 0; i < BTN_NUM; i++) {
        Img_t* img = & g_Img[1];
        if (btn[i].state == BTN_STATE_PLAY) {
        }
        else {
            img = & g_Img[0];
        }

        img->x = btn[i].x;
        img->y = btn[i].y;
        img->tag = btn[i].tag;
        Image_Draw(s_pHalContext, img);

        EVE_CoCmd_text(s_pHalContext, img->x - (strlen(media_info[i]) * 5), img->y + 100, 30, 0, media_info[i]);
    
    }

    char cvol[300];
    sprintf(cvol, "Volume %d", gs_volume);

    EVE_CoCmd_text(s_pHalContext, 100, 60, 30, 0, cvol);

    EVE_Cmd_wr32(s_pHalContext, TAG(TAG_SLIDER));
    EVE_CoCmd_slider(s_pHalContext, 100, 100, s_pHalContext->Width - 200, 20, OPT_FLAT, gs_volume, 255);

    /* set the tracker to track the slider for frequency */
    EVE_CoCmd_track(s_pHalContext,100, 100, s_pHalContext->Width - 200, 100, TAG_SLIDER);
    Display_End(s_pHalContext);

}

void DemoAudioPlayback(EVE_HalContext* pHalContext) {
    s_pHalContext = pHalContext;
    FlashHelper_SwitchFullMode(s_pHalContext);

    // To avoid a pop sound on reset or power state change
    // EVE_Hal_wr16(s_pHalContext, REG_SOUND,0x0);//configure “mute” sound to be played 
    // EVE_Hal_wr8(s_pHalContext, REG_PLAY,1);//play sound 
    // while (1 == EVE_Hal_rd8(s_pHalContext, REG_PLAY));//Wait for the completion of sound play 

    /// Button positioning
    int padding = (s_pHalContext->Width - ((BTN_NUM-1) * BUTTON_MARGIN) - ((BTN_NUM-2) * 80)) / 2 ;
    for (int i = 0; i < BTN_NUM; i++) {
        btn[i].x = padding + (i * BUTTON_MARGIN);
        btn[i].y = s_pHalContext->Height / 2;
        btn[i].tag = i + 1;
    }

    /// Copy wav data to ram_g
#if ENABLE_ALL_WAV_IN_RAM_G
    uint32_t dest = 0;
    for (int i = 0; i < BTN_NUM; i++) {
        EVE_CoCmd_flashRead(s_pHalContext, dest, wav[i].addr, wav[i].len);
        wav[i].addr = dest;
        dest += wav[i].len;
        EVE_Cmd_waitFlush(s_pHalContext);
    }
#endif

    while (1) {
        process_event();
        drawUi();
    }
}

