#include "Screen_Saver.h"
#include "Def.h"
#include "App.h"

// Global variables
extern Gpu_Hal_Context_t* g_pHalContext;
extern E_LANG g_language;

static uint32_t frame_addr = SS_FLASH_ADDR_FRAME_0;
static uint32_t frame_index = 0;
static int16_t ani_color = 0;
static int8_t ani_color_step = 5;
extern const char * s_pleaseTouch;

#define ANI_STEP        (10)

void ss_init() {
    ani_color = 0;
    ani_color_step = ANI_STEP;
}

void load_animation() {
    Gpu_CoCmd_FlashRead(g_pHalContext, RAM_DL_SIZE - SS_FRAME_SIZE, frame_addr, SS_FRAME_SIZE);

    if (g_language == LANG_CN) {
        Gpu_CoCmd_SetFont2(g_pHalContext, HF_BOTTOM, FontBottomCH.xf_addr - FontBegin.xf_addr, 0);
    } else {
        Gpu_CoCmd_SetFont2(g_pHalContext, HF_BOTTOM, FontBottom.xf_addr - FontBegin.xf_addr, 0);
    }

    App_WrCoCmd_Buffer(g_pHalContext, BITMAP_HANDLE(0));
    Gpu_CoCmd_SetBitmap(g_pHalContext, RAM_DL_SIZE - SS_FRAME_SIZE, SS_ASTC_FORMAT, SCREEN_WIDTH, SCREEN_HEIGHT);
    App_WrCoCmd_Buffer(g_pHalContext, TAG(SS_TAG));
    App_WrCoCmd_Buffer(g_pHalContext, BEGIN(BITMAPS));
    App_WrCoCmd_Buffer(g_pHalContext, VERTEX2F(0, 0));
    App_WrCoCmd_Buffer(g_pHalContext, END());

    App_WrCoCmd_Buffer(g_pHalContext, COLOR_RGB(0, 0, 0));
    App_WrCoCmd_Buffer(g_pHalContext, COLOR_A(ani_color));
    Gpu_CoCmd_Text(g_pHalContext, SCREEN_WIDTH * 0.5, Y_FOOTER, HF_BOTTOM, OPT_CENTERX, s_pleaseTouch);
    App_WrCoCmd_Buffer(g_pHalContext, COLOR_RGB(255, 255, 255));

    ani_color += ani_color_step;
    if (ani_color >= 255) {
        ani_color = 255;
        ani_color_step = -ANI_STEP;
    } else if (ani_color <= 0) {
        ani_color = 0;
        ani_color_step = ANI_STEP * 0.5;
    }

    frame_index++;
    frame_addr += SS_FRAME_SIZE;

    if (frame_index >= SS_FRAME_COUNT) {
        frame_index = 0;
        frame_addr = SS_FLASH_ADDR_FRAME_0;
    }
}

void ss_drawing() {
    load_animation();
}

void ss_process_event() {

    restart_screen_saver();
    uint8_t tag = Gesture_Get(g_pHalContext)->tagReleased;

    if (tag == SS_TAG) {
        switch_page_from_screen_saver();
    }
}
