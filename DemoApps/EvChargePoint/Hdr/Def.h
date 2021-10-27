#ifndef DEF_H_
#define DEF_H_

#include <time.h>
#include "Platform.h"
#include "Gpu.h"
#include "Event.h"

#define RAM_DL_SIZE                 (1024 * 1024)
// Precision
#define PRECISION					(8)
#define FRACTION                    (3)

#define ADDR(X, Y)                  (X.addr - Y.addr)

#ifdef FT900_PLATFORM
#define SS_TIME_OUT                 (30 * 50 * 1000)       // 5s
#else
#define SS_TIME_OUT                 (30 * 1000)            // 5s
#endif

typedef enum ePage {
    PAGE_BEGIN = 101,
    PAGE_SCREEN_SAVER = PAGE_BEGIN,
    PAGE_CHOOSE_LANGUAGE,
    PAGE_AUTHENTICATE,
    PAGE_PLUGIN,
    PAGE_TRANSACTION,
    PAGE_CHARGING,
    PAGE_REPORT,
    PAGE_MAX
} E_PAGE;

typedef enum eLang {
    LANG_EN = PAGE_MAX,
    LANG_DE,
    LANG_CN,
    LANG_MAX
} E_LANG;

enum eTag {
    SS_TAG = 1,
    CL_TAG_CIRCLE_EN,
    CL_TAG_CIRCLE_DE,
    CL_TAG_CIRCLE_CN,
    ATH_TAG_TAP_POINT,
    TR_TAG_CHARGE_BUTTON,
    RE_TAG_EXIT_BUTTON,
};

typedef struct _Image {
    uint32_t addr;
    uint32_t size;
    uint16_t x;
    uint16_t y;
    uint16_t width;
    uint16_t height;
    uint32_t format;
} Image;

typedef struct _Font {
    uint32_t gl_addr;
    uint32_t gl_size;
    uint32_t xf_addr;
    uint32_t xf_size;
} Font;

///////////////////////////////////////////////////////////////////////////////
// font
#define HF_TITLE            (14)    // font handle
#define HF_SMALL            (13)
#define HF_BOTTOM           (12)
#define HF_NUMBER           (11)
#define HF_LANG_CN          (10)

void font_init();

///////////////////////////////////////////////////////////////////////////////
// color
#define COLOR_BLUE           COLOR_RGB(0, 170, 255)
#define COLOR_GREY           COLOR_RGB(150, 150, 150)

///////////////////////////////////////////////////////////////////////////////
// screen text

void switch_english();
void switch_german();
void switch_chinese();
void switch_language();

///////////////////////////////////////////////////////////////////////////////
// screen
#define SCREEN_WIDTH               (1280)   // Target screen
#define SCREEN_HEIGHT              (800)    // Target screen

#define H_CENTER(x)                ((SCREEN_WIDTH - (x)) * 0.5)
#define V_CENTER(x)                ((SCREEN_HEIGHT - (x)) * 0.5)

#define Y_CENTER(IMG, y)           ((y) - IMG.height * 0.5)
#define X_CENTER(IMG, x)           ((x) - IMG.width * 0.5)

#define X_HEADER                   (50)
#define Y_HEADER                   (50)
#define Y_BATTERY                  (60)
#define Y_FOOTER                   (700)

#define SLIDER_WIDTH                (570)
#define SLIDER_HEIGHT               (10 * 16)
#define SLIDER_Y                    (350)
#define SLIDER_GAP                  (145)

// Screen saver
//#define SS_FRAME_COUNT              (377) // 64Mb Flash
#define SS_FRAME_COUNT              (97) // 16Mb Flash
#define SS_FRAME_SIZE               (163840)
#define SS_ASTC_FORMAT              COMPRESSED_RGBA_ASTC_10x10_KHR
#define SS_FLASH_ADDR_FRAME_0       (814208)

// Choose language
#define CIRCLE_NUM                  (3)

// Authenticate
#define ATH_ASTC_FORMAT              COMPRESSED_RGBA_ASTC_4x4_KHR
#define PAY_NUM                      (5)

#define ATH_PAYWITH_X                (X_HEADER)
#define ATH_PAYWITH_Y                (670)
#define ATH_PAY_VERTICAL_CENTER      (750)
#define ATH_PAY_ROOM                 (SCREEN_WIDTH / PAY_NUM)

// Plugin
#define PL_ASTC_FORMAT              COMPRESSED_RGBA_ASTC_4x4_KHR

// Transaction
#define TR_ASTC_FORMAT              COMPRESSED_RGBA_ASTC_4x4_KHR
#define TR_RIGHT_MARGIN             (150)
#define TR_RIGHT_BUTTON_MARGIN      (50)

// Report

void draw_image(Gpu_Hal_Context_t *s_pHalContext, Image img, uint32_t addr, int32_t x, int32_t y);
void draw_image_with_tag(Gpu_Hal_Context_t *s_pHalContext, Image img, uint32_t addr, int32_t x, int32_t y, uint8_t tag);

void sync_up(uint8_t p);

extern Image CL_BEGIN, CL_CIRCLE[CIRCLE_NUM];
extern Image ATH_BEGIN, ATH_TAP_POINT, ATH_CARD, ATH_PAY[PAY_NUM];
extern Image PL_BEGIN, PL_MALE, PL_FEMALE, PL_FEMALE_GREEN;
extern Image TR_BEGIN, TR_CAR, TR_BATTERY, TR_BAT_RED, TR_BAT_GREEN, TR_ROUND_ENERGY, TR_CHARGE_BUTTON, TR_CHARGE_TEXT, TR_EXIT_BUTTON, TR_QR_CODE;
extern uint16_t BATTERY_CELL_X[5];
extern uint16_t BATTERY_CELL_Y;
extern int32_t PIXEL(double x);
extern Font FontBegin, FontTitle, FontSmall, FontBottom, FontNumber;
extern Font FontTitleEN, FontSmallEN, FontBottomEN, FontNumber;
extern Font FontTitleCH, FontSmallCH, FontBottomCH, FontLangCH;


#endif // DEF_H_
