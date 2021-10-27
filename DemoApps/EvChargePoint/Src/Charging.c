#include "Charging.h"
#include "Def.h"

#include "App.h"
#include "Transaction.h"


// Global variables
extern Gpu_Hal_Context_t* g_pHalContext;
extern E_LANG g_language;

static int SLIDER_X = 660;

static const int MAX_ENERGY = 250;
static const int MAX_TIME = 120;
static const int MAX_COST = 32;
static const int MAX_FACTOR = 8;

static float charge_energy = 0;
static float charge_time = 0;
static float charge_cost = 0;
static float charge_factor = 0;

static int factor_count = 0;

static const int round_energy_x = 400;
static float energy_x = 0;

extern const char * s_charging;
extern const char * s_energy;
extern const char * s_kWh;

extern const char * s_time;
extern const char * s_minutes;

extern const char * s_cost;
extern const char * s_currency;

int16_t x_ani;
extern bool_t start_ani;
extern int8_t X_ANI_STEP;

static int16_t y_ani = 0;
bool_t start_y_ani = FALSE;
static float r_scale;
static float r_scale_step;
static const float RE_SCALE = 0.02;

void ch_init() {
    charge_energy = 25;
    charge_time = MAX_TIME * 0.09;
    charge_cost = MAX_COST * 0.095;
    charge_factor = 0.15;
    factor_count = 0;

    energy_x = round_energy_x + TR_ROUND_ENERGY.width;

    x_ani = 0;
    start_ani = FALSE;

    y_ani = 0;
    start_y_ani = FALSE;
    r_scale = 1.0;
    r_scale_step = RE_SCALE;
}

void draw_round_energy() {

	r_scale += r_scale_step;
	if (r_scale >= 1.35) {
		r_scale = 1.35;
		r_scale_step = -RE_SCALE;
	} else if (r_scale <= 1.0) {
		r_scale = 1.0;
		r_scale_step = RE_SCALE;
	}

	if (start_ani) {
		r_scale_step = 0.0;
	}

	uint32_t w = TR_ROUND_ENERGY.width * 2;
	uint32_t h = (uint32_t)(TR_ROUND_ENERGY.height * (1 + r_scale * 0.5));

	Gpu_CoCmd_SetBitmap(g_pHalContext, ADDR(TR_ROUND_ENERGY, TR_BEGIN), COMPRESSED_RGBA_ASTC_4x4_KHR,
			TR_ROUND_ENERGY.width, TR_ROUND_ENERGY.height);
	App_WrCoCmd_Buffer(g_pHalContext, BITMAP_SIZE(BILINEAR, BORDER, BORDER, w, h));
	App_WrCoCmd_Buffer(g_pHalContext, BITMAP_SIZE_H(w >> 9, h >> 9));

	App_WrCoCmd_Buffer(g_pHalContext, BEGIN(BITMAPS));
	App_WrCoCmd_Buffer(g_pHalContext, SAVE_CONTEXT());

	Gpu_CoCmd_LoadIdentity(g_pHalContext);
	Gpu_CoCmd_Scale(g_pHalContext, r_scale * 65536, r_scale * 65536);
	Gpu_CoCmd_Translate(g_pHalContext, 65536 , 65536);
	Gpu_CoCmd_SetMatrix(g_pHalContext);
	App_WrCoCmd_Buffer(g_pHalContext, VERTEX2F(PIXEL(round_energy_x - TR_ROUND_ENERGY.width * (r_scale - 1) * 0.5),
									   PIXEL(y_ani + Y_BATTERY + (TR_BATTERY.height - TR_ROUND_ENERGY.height) * 0.5 - TR_ROUND_ENERGY.height * (r_scale - 1) * 0.5)));
	App_WrCoCmd_Buffer(g_pHalContext, END());
	App_WrCoCmd_Buffer(g_pHalContext, RESTORE_CONTEXT());
}

void ch_drawing() {

    Gpu_CoCmd_SetFont2(g_pHalContext, HF_TITLE, FontTitle.xf_addr - FontBegin.xf_addr, 0);
    Gpu_CoCmd_SetFont2(g_pHalContext, HF_SMALL, FontSmall.xf_addr - FontBegin.xf_addr, 0);
    Gpu_CoCmd_SetFont2(g_pHalContext, HF_NUMBER, FontNumber.xf_addr - FontBegin.xf_addr, 0);

    // Draw Text: header and footer
    App_WrCoCmd_Buffer(g_pHalContext, COLOR_RGB(0, 0, 0));
    Gpu_CoCmd_Text(g_pHalContext, X_HEADER, Y_HEADER, HF_TITLE, 0, s_charging);
    App_WrCoCmd_Buffer(g_pHalContext, COLOR_RGB(255, 255, 255));

    // Draw Car
    draw_image(g_pHalContext, TR_CAR, ADDR(TR_CAR, TR_BEGIN), x_ani + 296 - TR_CAR.width * 0.5, y_ani + Y_CENTER(TR_CAR, 510));

    // Battery background
    const int bat_x = SCREEN_WIDTH - TR_BATTERY.width - TR_RIGHT_MARGIN;
    draw_image(g_pHalContext, TR_BATTERY, ADDR(TR_BATTERY, TR_BEGIN), bat_x, y_ani + Y_BATTERY);

    // Battery Red
    draw_image(g_pHalContext, TR_BAT_RED, ADDR(TR_BAT_RED, TR_BEGIN), SCREEN_WIDTH - TR_BATTERY.width - TR_RIGHT_MARGIN + BATTERY_CELL_X[4], y_ani + Y_BATTERY + BATTERY_CELL_Y);

    
    // Draw line between Round Energy and Battery
    App_WrCoCmd_Buffer(g_pHalContext, COLOR_RGB(104, 167, 22));
    App_WrCoCmd_Buffer(g_pHalContext, LINE_WIDTH(2 *16));
    App_WrCoCmd_Buffer(g_pHalContext, BEGIN(LINES));
    App_WrCoCmd_Buffer(g_pHalContext, VERTEX2F(PIXEL(round_energy_x + TR_ROUND_ENERGY.width), PIXEL(y_ani + Y_BATTERY + TR_BATTERY.height * 0.5)));
    App_WrCoCmd_Buffer(g_pHalContext, VERTEX2F(PIXEL(bat_x), PIXEL(y_ani + Y_BATTERY + TR_BATTERY.height * 0.5)));
    App_WrCoCmd_Buffer(g_pHalContext, END());
    App_WrCoCmd_Buffer(g_pHalContext, COLOR_RGB(255, 255, 255));

    // Draw Round Energy
    draw_round_energy();

    // Draw charging button
    if (!start_ani) {

        App_WrCoCmd_Buffer(g_pHalContext, COLOR_RGB(255, 255, 255));
        App_WrCoCmd_Buffer(g_pHalContext, LINE_WIDTH(2 * 16));
        App_WrCoCmd_Buffer(g_pHalContext, BEGIN(LINES));
        App_WrCoCmd_Buffer(g_pHalContext, VERTEX2F(PIXEL(energy_x), PIXEL(y_ani + Y_BATTERY + TR_BATTERY.height * 0.5)));
        App_WrCoCmd_Buffer(g_pHalContext, VERTEX2F(PIXEL(energy_x + 10), PIXEL(y_ani + Y_BATTERY + TR_BATTERY.height * 0.5)));
        App_WrCoCmd_Buffer(g_pHalContext, END());

    
        energy_x += charge_factor * 20;
        if (energy_x >= (bat_x - 10)) {
            energy_x = round_energy_x + TR_ROUND_ENERGY.width;
        }
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Draw Slider
    int16_t x_ani_slider = x_ani * 2;

    App_WrCoCmd_Buffer(g_pHalContext, COLOR_RGB(219, 219, 219));
    App_WrCoCmd_Buffer(g_pHalContext, LINE_WIDTH(SLIDER_HEIGHT));
    App_WrCoCmd_Buffer(g_pHalContext, BEGIN(LINES));

    for (int i = 0; i < 3; i++) {
        App_WrCoCmd_Buffer(g_pHalContext, VERTEX2F(PIXEL(x_ani_slider + SLIDER_X), PIXEL(SLIDER_Y + i * SLIDER_GAP)));
        App_WrCoCmd_Buffer(g_pHalContext, VERTEX2F(PIXEL(x_ani_slider + SLIDER_X + SLIDER_WIDTH), PIXEL(SLIDER_Y + i * SLIDER_GAP)));
    }

    int slider_x = 0;
    charge_energy += charge_factor;
    charge_time += charge_factor * MAX_TIME / MAX_ENERGY;
    charge_cost += charge_factor * MAX_COST / MAX_ENERGY;

    if (charge_energy > MAX_ENERGY) {
        charge_energy = MAX_ENERGY;
    }
    if (charge_time > MAX_TIME) {
        charge_time = MAX_TIME;
    }
    if (charge_cost > MAX_COST) {
        charge_cost = MAX_COST;
    }

    // sync with Bar Type LCD
    sync_up( (uint8_t)(charge_energy * 100 / MAX_ENERGY) );

    slider_x = SLIDER_X + charge_energy * SLIDER_WIDTH / MAX_ENERGY;
    App_WrCoCmd_Buffer(g_pHalContext, COLOR_RGB(104, 167, 22));
    App_WrCoCmd_Buffer(g_pHalContext, VERTEX2F(PIXEL(x_ani_slider + SLIDER_X), PIXEL(SLIDER_Y)));
    App_WrCoCmd_Buffer(g_pHalContext, VERTEX2F(PIXEL(x_ani_slider + slider_x), PIXEL(SLIDER_Y)));

    slider_x = SLIDER_X + charge_time * SLIDER_WIDTH / MAX_TIME;
    App_WrCoCmd_Buffer(g_pHalContext, COLOR_RGB(0, 85, 255));
    App_WrCoCmd_Buffer(g_pHalContext, VERTEX2F(PIXEL(x_ani_slider + SLIDER_X), PIXEL(SLIDER_Y + SLIDER_GAP)));
    App_WrCoCmd_Buffer(g_pHalContext, VERTEX2F(PIXEL(x_ani_slider + slider_x), PIXEL(SLIDER_Y + SLIDER_GAP)));


    slider_x = SLIDER_X + charge_cost * SLIDER_WIDTH / MAX_COST;
    App_WrCoCmd_Buffer(g_pHalContext, COLOR_RGB(255, 0, 127));
    App_WrCoCmd_Buffer(g_pHalContext, VERTEX2F(PIXEL(x_ani_slider + SLIDER_X), PIXEL(SLIDER_Y + 2 * SLIDER_GAP)));
    App_WrCoCmd_Buffer(g_pHalContext, VERTEX2F(PIXEL(x_ani_slider + slider_x), PIXEL(SLIDER_Y + 2 * SLIDER_GAP)));

    App_WrCoCmd_Buffer(g_pHalContext, END());

    // Battery Green
    App_WrCoCmd_Buffer(g_pHalContext, COLOR_RGB(255, 255, 255));
    for (int i = 0; i < (int)charge_energy/50; i++) {
        draw_image(g_pHalContext, TR_BAT_GREEN, ADDR(TR_BAT_GREEN, TR_BEGIN),
                    SCREEN_WIDTH - TR_BATTERY.width - TR_RIGHT_MARGIN + BATTERY_CELL_X[4 - i], y_ani + Y_BATTERY + BATTERY_CELL_Y);
    }

    // Draw Number
    const int16_t HF_VALUE = HF_NUMBER;
    const int16_t HF_UNIT = 31;

    

    App_WrCoCmd_Buffer(g_pHalContext, COLOR_RGB(0, 0, 0));
    // Energy
    Gpu_CoCmd_Number(g_pHalContext, x_ani_slider + SLIDER_X, SLIDER_Y - 55, HF_VALUE, 0, charge_energy);
    Gpu_CoCmd_Number(g_pHalContext, x_ani_slider + SLIDER_X + SLIDER_WIDTH, SLIDER_Y - 55, HF_UNIT, OPT_RIGHTX, MAX_ENERGY);

    // Time
    Gpu_CoCmd_Number(g_pHalContext, x_ani_slider + SLIDER_X, SLIDER_Y + SLIDER_GAP - 55, HF_VALUE, 0, charge_time);
    Gpu_CoCmd_Number(g_pHalContext, x_ani_slider + SLIDER_X + SLIDER_WIDTH, SLIDER_Y + SLIDER_GAP - 55, HF_UNIT, OPT_RIGHTX, MAX_TIME);

    // Cost
    Gpu_CoCmd_Number(g_pHalContext, x_ani_slider + SLIDER_X, SLIDER_Y + 2 * SLIDER_GAP - 55, HF_VALUE, 0, charge_cost);
    Gpu_CoCmd_Number(g_pHalContext, x_ani_slider + SLIDER_X + SLIDER_WIDTH, SLIDER_Y + 2 * SLIDER_GAP - 55, HF_UNIT, OPT_RIGHTX, MAX_COST);

    // Draw Unit
    const int font_size = HF_SMALL;
    int unit_gap = 12;
    if (g_language == LANG_CN) {
        	unit_gap = 15;
    }

    App_WrCoCmd_Buffer(g_pHalContext, COLOR_GREY);
    Gpu_CoCmd_Text(g_pHalContext, x_ani_slider + SLIDER_X, SLIDER_Y + unit_gap, font_size, 0, s_energy);
    Gpu_CoCmd_Text(g_pHalContext, x_ani_slider + SLIDER_X + SLIDER_WIDTH, SLIDER_Y + unit_gap, font_size, OPT_RIGHTX, s_kWh);

    Gpu_CoCmd_Text(g_pHalContext, x_ani_slider + SLIDER_X, SLIDER_Y + SLIDER_GAP + unit_gap, font_size, 0, s_time);
    Gpu_CoCmd_Text(g_pHalContext, x_ani_slider + SLIDER_X + SLIDER_WIDTH, SLIDER_Y + SLIDER_GAP + unit_gap, font_size, OPT_RIGHTX, s_minutes);

    Gpu_CoCmd_Text(g_pHalContext, x_ani_slider + SLIDER_X, SLIDER_Y + 2 * SLIDER_GAP + unit_gap, font_size, 0, s_cost);
    Gpu_CoCmd_Text(g_pHalContext, x_ani_slider + SLIDER_X + SLIDER_WIDTH, SLIDER_Y + 2 * SLIDER_GAP + unit_gap, font_size, OPT_RIGHTX, s_currency);

    if (start_ani && !start_y_ani) {
        x_ani += X_ANI_STEP;
    }

    if (start_y_ani) {
        y_ani -= X_ANI_STEP;
    }
}


void ch_process_event() {

    restart_screen_saver();

    if (is_touch(g_pHalContext)) {
        if (factor_count <= MAX_FACTOR) {
            factor_count++;
            charge_factor += 0.025 * factor_count;
        }
    }

    if (charge_energy >= MAX_ENERGY && charge_time >= MAX_TIME && charge_cost >= MAX_COST &&
        !start_ani) {
        Display_End(g_pHalContext);
        Gpu_CoCmd_Sync(g_pHalContext);
        start_ani = TRUE;
    }

    if (x_ani >= H_CENTER(TR_CAR.width)) {
        start_y_ani = TRUE;
    }

    if (y_ani + Y_CENTER(TR_CAR, 510) <= 100) {
        switch_next_page();
    }
}
