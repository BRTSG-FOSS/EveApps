#include "Authenticate.h"
#include "Def.h"
#include "App.h"

// Global variables
extern Gpu_Hal_Context_t* g_pHalContext;
extern E_LANG g_language;

typedef enum {
    TAP_POINT = 0,
    WHITE_CARD,
    AUTHENTICATED,
} ATH_STATE;

static ATH_STATE page_state = TAP_POINT;
static int card_x = 0;
static int card_x_start = 0;
static int LEFT_STONE = 0;
static int RIGHT_STONE = 0;
static int card_move = 0;
static int card_ani_count = 0;

#define MAX_CARD_ANI_COUNT           (4)

extern const char * s_tapYourCard;
extern const char * s_checkingTheCard;
extern const char * s_authenticated;
extern const char * s_payWith;

int count_frame = 0;
static int16_t alpha = 0;
static int8_t alpha_step;
uint8_t pay_ani = 0;

void ath_init() {

    page_state = TAP_POINT;
    card_x_start = H_CENTER(ATH_CARD.width);
    card_x = card_x_start;
    card_move = 10;
    card_ani_count = 0;

    LEFT_STONE = SCREEN_WIDTH * 0.5 - ATH_CARD.width * 0.75;
    RIGHT_STONE = SCREEN_WIDTH * 0.5 - ATH_CARD.width * 0.25;

    count_frame = 0;
    alpha = 255;
    alpha_step = -5;
    pay_ani = 0;
}

void ath_drawing() {
    Gpu_CoCmd_SetFont2(g_pHalContext, HF_TITLE, FontTitle.xf_addr - FontBegin.xf_addr, 0);
    Gpu_CoCmd_SetFont2(g_pHalContext, HF_BOTTOM, FontBottom.xf_addr - FontBegin.xf_addr, 0);

    if (page_state == TAP_POINT) {
        // Draw header text
        
        App_WrCoCmd_Buffer(g_pHalContext, COLOR_RGB(0, 0, 0));
        Gpu_CoCmd_Text(g_pHalContext, X_HEADER, Y_HEADER, HF_TITLE, 0, s_tapYourCard);
        App_WrCoCmd_Buffer(g_pHalContext, COLOR_RGB(255, 255, 255));
        
        // Draw Tap Point

        App_WrCoCmd_Buffer(g_pHalContext, COLOR_A(alpha));
        draw_image_with_tag(g_pHalContext, ATH_TAP_POINT, ADDR(ATH_TAP_POINT, ATH_BEGIN),
                   H_CENTER(ATH_TAP_POINT.width), V_CENTER(ATH_TAP_POINT.height), ATH_TAG_TAP_POINT);
        App_WrCoCmd_Buffer(g_pHalContext, COLOR_A(255));
        alpha += alpha_step;
        if (alpha >= 255) {
            alpha = 255;
            alpha_step = -10;
            pay_ani++;
            pay_ani %= PAY_NUM;
        } else if (alpha <= 10) {
            alpha = 10;
            alpha_step = 5;
        }
    } else if (page_state == WHITE_CARD) {

        // Draw header text
        App_WrCoCmd_Buffer(g_pHalContext, COLOR_RGB(0, 0, 0));
        Gpu_CoCmd_Text(g_pHalContext, X_HEADER, Y_HEADER, HF_TITLE, 0, s_checkingTheCard);
        App_WrCoCmd_Buffer(g_pHalContext, COLOR_RGB(255, 255, 255));

        // Draw White Card
        draw_image(g_pHalContext, ATH_CARD, ADDR(ATH_CARD, ATH_BEGIN), card_x, V_CENTER(ATH_CARD.height));
        card_x += card_move;
        if (card_x >= RIGHT_STONE || card_x <= LEFT_STONE) {
            card_move = -card_move;
            card_ani_count++;
        }

        if (card_ani_count >= MAX_CARD_ANI_COUNT && card_x >= H_CENTER(ATH_CARD.width)) {
            page_state = AUTHENTICATED;
            count_frame = 0;
        }
        pay_ani = PAY_NUM;
        restart_screen_saver();
    } else {
        App_WrCoCmd_Buffer(g_pHalContext, COLOR_BLUE);
        Gpu_CoCmd_Text(g_pHalContext, X_HEADER, Y_HEADER, HF_TITLE, 0, s_authenticated);
        App_WrCoCmd_Buffer(g_pHalContext, COLOR_RGB(255, 255, 255));

        // Draw White Card        
        draw_image(g_pHalContext, ATH_CARD, ADDR(ATH_CARD, ATH_BEGIN), card_x, V_CENTER(ATH_CARD.height));
        
        count_frame++;
    }

    // Draw Text "Pay with"
    int16_t hf_paywith = HF_BOTTOM;
    if (g_language == LANG_DE)
        hf_paywith = 30;

    App_WrCoCmd_Buffer(g_pHalContext, COLOR_RGB(0, 0, 0));
    Gpu_CoCmd_Text(g_pHalContext, ATH_PAYWITH_X, ATH_PAYWITH_Y, hf_paywith, 0, s_payWith);
    App_WrCoCmd_Buffer(g_pHalContext, COLOR_RGB(255, 255, 255));

    // Draw Pay Items
    int pay_alpha = 255;
    for (int i = 0; i < PAY_NUM; i++) {
        if (pay_ani == i) {
            pay_alpha = alpha;
        } else {
            pay_alpha = 255;
        }
        App_WrCoCmd_Buffer(g_pHalContext, COLOR_A(pay_alpha));
        draw_image(g_pHalContext, ATH_PAY[i], ADDR(ATH_PAY[i], ATH_BEGIN),
                   ATH_PAY_ROOM * i + (ATH_PAY_ROOM - ATH_PAY[i].width) * 0.5,
                   ATH_PAY_VERTICAL_CENTER - ATH_PAY[i].height * 0.5);
        App_WrCoCmd_Buffer(g_pHalContext, COLOR_A(255));
    }
}

void ath_process_event() {
    uint8_t tag = Gesture_Get(g_pHalContext)->tagReleased;

    if (tag == ATH_TAG_TAP_POINT) {
        page_state = WHITE_CARD;
        restart_screen_saver();
    }

    if (count_frame > 30) {
        switch_next_page();
    }
}
