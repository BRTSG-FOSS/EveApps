#include "Choose_Language.h"
#include "Def.h"
#include "App.h"

// Global variables
extern Gpu_Hal_Context_t* g_pHalContext;
extern E_LANG g_language;

static uint32_t frame_addr = SS_FLASH_ADDR_FRAME_0;
static uint32_t frame_index = 0;
int8_t CL_TAGS[3] = {CL_TAG_CIRCLE_EN, CL_TAG_CIRCLE_DE, CL_TAG_CIRCLE_CN};

extern const char * s_languageEnglish;
extern const char * s_languageGerman;
extern const char * s_languageChinese;
extern const char * s_chooseLanguage;

const char * LANG[3];

uint32_t num = 0;

static int16_t x_ani;

const int8_t X_STEP = 10;
const int8_t Y_STEP = 7;

int16_t y_ani;

static bool_t start_ani_out;

void cl_init() {
    LANG[0] = s_languageEnglish;
    LANG[1] = s_languageGerman;
    LANG[2] = s_languageChinese;

    x_ani = 0;
    y_ani = 0;

    start_ani_out = FALSE;
}

void cl_load_animation() {

    Gpu_CoCmd_FlashRead(g_pHalContext, RAM_DL_SIZE - SS_FRAME_SIZE, frame_addr, SS_FRAME_SIZE);
    Gpu_CoCmd_SetFont2(g_pHalContext, HF_TITLE, FontTitle.xf_addr - FontBegin.xf_addr, 0);
    Gpu_CoCmd_SetFont2(g_pHalContext, HF_LANG_CN, FontLangCH.xf_addr - FontBegin.xf_addr, 0);

    App_WrCoCmd_Buffer(g_pHalContext, COLOR_A(85));
    App_WrCoCmd_Buffer(g_pHalContext, BITMAP_HANDLE(1));
    Gpu_CoCmd_SetBitmap(g_pHalContext, RAM_DL_SIZE - SS_FRAME_SIZE, SS_ASTC_FORMAT, SCREEN_WIDTH, SCREEN_HEIGHT);
    App_WrCoCmd_Buffer(g_pHalContext, BEGIN(BITMAPS));
    App_WrCoCmd_Buffer(g_pHalContext, VERTEX2F(0, 0));
    App_WrCoCmd_Buffer(g_pHalContext, END());
    App_WrCoCmd_Buffer(g_pHalContext, COLOR_A(255));

    frame_index++;
    frame_addr += SS_FRAME_SIZE;

    if (frame_index >= SS_FRAME_COUNT) {
        frame_index = 0;
        frame_addr = SS_FLASH_ADDR_FRAME_0;
    }

    // Draw text
    App_WrCoCmd_Buffer(g_pHalContext, COLOR_RGB(0, 0, 0 ));
    Gpu_CoCmd_Text(g_pHalContext, X_HEADER, Y_HEADER, HF_TITLE, 0, s_chooseLanguage);
    App_WrCoCmd_Buffer(g_pHalContext, COLOR_RGB(255, 255, 255));

    // Draw circle
    const int GAP = 100;
    const int x = CL_CIRCLE[0].width * 3 + GAP * 2;

    if (!start_ani_out) {
        x_ani += X_STEP;
        y_ani += Y_STEP;

        if (x_ani - CL_CIRCLE[0].width >= 120) {
            x_ani = CL_CIRCLE[0].width + 120;

            App_WrCoCmd_Buffer(g_pHalContext, COLOR_RGB(0, 0, 0));
            for (uint32_t i = 0; i < CIRCLE_NUM; i++) {
                Gpu_CoCmd_Text(g_pHalContext, H_CENTER(x) + i * (CL_CIRCLE[i].width + GAP) + CL_CIRCLE[i].width * 0.5,
                               V_CENTER(CL_CIRCLE[i].height) + CL_CIRCLE[i].height,
                               i != 2 ? 31 : HF_LANG_CN, OPT_CENTERX, LANG[i]);
            }
            App_WrCoCmd_Buffer(g_pHalContext, COLOR_RGB(255, 255, 255));
        }

        if (y_ani >= V_CENTER(CL_CIRCLE[2].height)) {
            y_ani = V_CENTER(CL_CIRCLE[2].height);
        }
    } else {
        x_ani -= X_STEP;
        y_ani += Y_STEP * 2;
    }

    draw_image_with_tag(g_pHalContext, CL_CIRCLE[0], ADDR(CL_CIRCLE[0], CL_BEGIN),
                        x_ani - CL_CIRCLE[0].width, V_CENTER(CL_CIRCLE[0].height),
                        CL_TAGS[0]);
    draw_image_with_tag(g_pHalContext, CL_CIRCLE[2], ADDR(CL_CIRCLE[2], CL_BEGIN),
                        SCREEN_WIDTH - x_ani, V_CENTER(CL_CIRCLE[2].height),
                        CL_TAGS[2]);
    draw_image_with_tag(g_pHalContext, CL_CIRCLE[1], ADDR(CL_CIRCLE[1], CL_BEGIN),
                        H_CENTER(CL_CIRCLE[1].width), y_ani,
                        CL_TAGS[1]);    
}

void cl_drawing() {
    cl_load_animation();
}

void cl_process_event() {
    uint8_t tag = Gesture_Get(g_pHalContext)->tagReleased;

    switch (tag) {
        case CL_TAG_CIRCLE_EN: g_language = LANG_EN; break;
        case CL_TAG_CIRCLE_DE: g_language = LANG_DE; break;
        case CL_TAG_CIRCLE_CN: g_language = LANG_CN; break;
        default: break;
    }

    if (tag == CL_TAG_CIRCLE_EN || tag == CL_TAG_CIRCLE_DE || tag == CL_TAG_CIRCLE_CN) {
        start_ani_out = TRUE;
        restart_screen_saver();
    }

    if (start_ani_out && x_ani == -CL_CIRCLE[0].width * 0.5) {
        switch_language();
        switch_next_page();
    }
}
