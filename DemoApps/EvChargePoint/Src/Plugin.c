#include "Plugin.h"
#include "Def.h"

#include "App.h"
#include "Event.h"


// Global variables
extern Gpu_Hal_Context_t* g_pHalContext;

typedef enum {
    PLUG_PREPARE = 0,
    PLUGGING,
    PLUGGED,
    PLUG_END
} PL_STATE;

static PL_STATE page_state = PLUG_PREPARE;

static int x = 0;
static int x_start = 0;
static int LEFT_STONE = 0;
static int RIGHT_STONE = 0;
static int x_move = 0;

static int f_x = 0;
static int f_x_start = 0;
static int F_LEFT_STONE = 0;
static int F_RIGHT_STONE = 0;
static int f_x_move = 0;

static bool_t female_ani_flag = FALSE;

extern const char * s_plugIn;
extern const char * s_pleaseTouch;
extern const char * s_connected;

static int x_car = 0;
static const uint8_t STEP_X = 10;
static int y_ani = 0;
static int count_frame = 0;

static int16_t alpha_in = 0;
static int16_t alpha_out = 255;

void pl_init() {

    page_state = PLUG_PREPARE;

    LEFT_STONE = SCREEN_WIDTH * 0.5 - PL_MALE.width - PL_MALE.width * 0.35;
    RIGHT_STONE = SCREEN_WIDTH * 0.5 - PL_MALE.width;

    x_start = LEFT_STONE;
    x = x_start;
    x_move = 5;

    f_x_start = SCREEN_WIDTH * 0.5;
    f_x = f_x_start;
    f_x_move = 2;

    F_LEFT_STONE = f_x_start;
    F_RIGHT_STONE = f_x_start + f_x_move * 5;

    female_ani_flag = FALSE;

    x = -PL_MALE.width;
    f_x = SCREEN_WIDTH;
    y_ani = 0;

    count_frame = 0;
    alpha_in = 0;
    alpha_out = 255;
}

void pl_drawing() {
    
    Gpu_CoCmd_SetFont2(g_pHalContext, HF_TITLE, FontTitle.xf_addr - FontBegin.xf_addr, 0);
    Gpu_CoCmd_SetFont2(g_pHalContext, HF_BOTTOM, FontBottom.xf_addr - FontBegin.xf_addr, 0);

    // Draw Text: footer
    if (page_state != PLUG_PREPARE && page_state != PLUG_END) {
        App_WrCoCmd_Buffer(g_pHalContext, COLOR_RGB(0, 0, 0));
        Gpu_CoCmd_Text(g_pHalContext, SCREEN_WIDTH * 0.5, Y_FOOTER, HF_BOTTOM, OPT_CENTERX, s_pleaseTouch);
        App_WrCoCmd_Buffer(g_pHalContext, COLOR_RGB(255, 255, 255));
    }
        
    // Draw Plug Male and Female
    if (page_state == PLUG_PREPARE) {

    	App_WrCoCmd_Buffer(g_pHalContext, COLOR_A(alpha_in));
        draw_image(g_pHalContext, PL_MALE, ADDR(PL_MALE, PL_BEGIN), x, V_CENTER(PL_MALE.height));
        draw_image(g_pHalContext, PL_FEMALE, ADDR(PL_FEMALE, PL_BEGIN), f_x, V_CENTER(PL_FEMALE.height));
        App_WrCoCmd_Buffer(g_pHalContext, COLOR_A(255));

        alpha_in += 5;
        if (alpha_in > 255) {
        	alpha_in = 255;
        }

        x += STEP_X;
        f_x -= STEP_X * 1.2;

        if (x >= x_start ) {
            x = x_start;
            if (f_x <= f_x_start) {
                page_state = PLUGGING;
                f_x = f_x_start;
            }
        }
    } else if (page_state == PLUGGING) {
        App_WrCoCmd_Buffer(g_pHalContext, COLOR_RGB(0, 0, 0));
        Gpu_CoCmd_Text(g_pHalContext, X_HEADER, Y_HEADER, HF_TITLE, 0, s_plugIn);
        App_WrCoCmd_Buffer(g_pHalContext, COLOR_RGB(255, 255, 255));

        // Male
        draw_image(g_pHalContext, PL_MALE, ADDR(PL_MALE, PL_BEGIN), x, V_CENTER(PL_MALE.height));
        
        x += x_move;
        if (x >= RIGHT_STONE || x <= LEFT_STONE) {
            x_move = -x_move;
        }

        if (x >= RIGHT_STONE) {
            female_ani_flag = TRUE;
        }

        // Female
        Image f_img = PL_FEMALE;
        if (female_ani_flag) {
            f_img = PL_FEMALE_GREEN;
        }

        draw_image(g_pHalContext, f_img, ADDR(f_img, PL_BEGIN), f_x_start, V_CENTER(f_img.height));
        if (female_ani_flag) {
            f_x_start += f_x_move;
        }

        if (f_x_start >= F_RIGHT_STONE) {
            f_x_move = -f_x_move;
        }

        if (f_x_start <= F_LEFT_STONE) {
            f_x_move = abs(f_x_move);
            f_x_start = F_LEFT_STONE;
            female_ani_flag = FALSE;
        }
    } else if (page_state == PLUGGED) {
        App_WrCoCmd_Buffer(g_pHalContext, COLOR_BLUE);
        Gpu_CoCmd_Text(g_pHalContext, X_HEADER, Y_HEADER, HF_TITLE, 0, s_connected);
        App_WrCoCmd_Buffer(g_pHalContext, COLOR_RGB(255, 255, 255));

        // Male
        draw_image(g_pHalContext, PL_MALE, ADDR(PL_MALE, PL_BEGIN), RIGHT_STONE + 48, V_CENTER(PL_MALE.height));

        // Female
        draw_image(g_pHalContext, PL_FEMALE_GREEN, ADDR(PL_FEMALE_GREEN, PL_BEGIN), SCREEN_WIDTH * 0.5, V_CENTER(PL_FEMALE_GREEN.height));
        
        count_frame++;
        if (count_frame >= 30) {
            page_state = PLUG_END;
            x_car = -TR_CAR.width;
            y_ani = V_CENTER(PL_MALE.height);
        }
    } else {

        App_WrCoCmd_Buffer(g_pHalContext, COLOR_A(alpha_out));

        alpha_out -= 5;
        if (alpha_out < 0)
        	alpha_out = 0;

        // Male
        draw_image(g_pHalContext, PL_MALE, ADDR(PL_MALE, PL_BEGIN), RIGHT_STONE + 48, y_ani);
        // Female
        draw_image(g_pHalContext, PL_FEMALE_GREEN, ADDR(PL_FEMALE_GREEN, PL_BEGIN), SCREEN_WIDTH * 0.5, y_ani);
        App_WrCoCmd_Buffer(g_pHalContext, COLOR_A(255));

        // Car
        draw_image(g_pHalContext, TR_CAR, ADDR(TR_CAR, TR_BEGIN), x_car, Y_CENTER(TR_CAR, 510));

        y_ani += 15;
        x_car += STEP_X;

        if (x_car >= 0) {
            switch_next_page();
        }
    }

}

void pl_process_event() {   
    if (is_touch(g_pHalContext) && page_state == PLUGGING) {
        page_state = PLUGGED;
    }
}
