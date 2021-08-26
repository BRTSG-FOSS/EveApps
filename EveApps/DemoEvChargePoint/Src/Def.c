#include "Def.h"
#include "Hal_Config.h"

int32_t PIXEL(double x) {
    int32_t res = x * PRECISION;
    return res;
}

void sync_up(uint8_t p) {
#ifdef  FT900_PLATFORM
	uart_writen(UART0, &p, 1);
#endif //  FT900_PLATFORM
}

void draw_image(Gpu_Hal_Context_t *s_pHalContext, Image img, uint32_t addr, int32_t x, int32_t y) {
    App_WrCoCmd_Buffer(s_pHalContext, BITMAP_HANDLE(1));
    Gpu_CoCmd_SetBitmap(s_pHalContext, addr, img.format, img.width, img.height);    
    App_WrCoCmd_Buffer(s_pHalContext, BEGIN(BITMAPS));
    App_WrCoCmd_Buffer(s_pHalContext, VERTEX2F(PIXEL(x), PIXEL(y)));
    App_WrCoCmd_Buffer(s_pHalContext, END());
}

void draw_image_with_tag(Gpu_Hal_Context_t *s_pHalContext, Image img, uint32_t addr, int32_t x, int32_t y, uint8_t tag) {
    App_WrCoCmd_Buffer(s_pHalContext, BITMAP_HANDLE(1));
    Gpu_CoCmd_SetBitmap(s_pHalContext, addr, img.format, img.width, img.height);
    App_WrCoCmd_Buffer(s_pHalContext, TAG(tag));
    App_WrCoCmd_Buffer(s_pHalContext, BEGIN(BITMAPS));
    App_WrCoCmd_Buffer(s_pHalContext, VERTEX2F(PIXEL(x), PIXEL(y)));
    App_WrCoCmd_Buffer(s_pHalContext, END());
    App_WrCoCmd_Buffer(s_pHalContext, TAG(0));
}


Image CL_BEGIN = {4096};
Image CL_CIRCLE[CIRCLE_NUM] = {
    {117568, 35392, 0, 0, 280, 280, COMPRESSED_RGBA_ASTC_6x6_KHR},
    {152960, 35392, 0, 0, 280, 280, COMPRESSED_RGBA_ASTC_6x6_KHR},
    {188352, 35392, 0, 0, 280, 280, COMPRESSED_RGBA_ASTC_6x6_KHR},
};


Image ATH_BEGIN = {4096};
Image ATH_TAP_POINT = {4096, 47872, 0, 0, 520, 368, COMPRESSED_RGBA_ASTC_8x8_KHR};
Image ATH_CARD = {51968, 32832, 0, 0, 456, 288, COMPRESSED_RGBA_ASTC_8x8_KHR};

Image ATH_PAY[PAY_NUM] = {
    {84800, 7104, 0, 0, 148, 48, COMPRESSED_RGBA_ASTC_4x4_KHR},
    {91904, 4608, 0, 0, 76, 60, COMPRESSED_RGBA_ASTC_4x4_KHR},
    {96512, 5568, 0, 0, 116, 48, COMPRESSED_RGBA_ASTC_4x4_KHR},
    {102080, 8128, 0, 0, 184, 44, COMPRESSED_RGBA_ASTC_4x4_KHR},
    {110208, 7360, 0, 0, 108, 68, COMPRESSED_RGBA_ASTC_4x4_KHR},
};


Image PL_BEGIN = {4096};
Image PL_MALE = {223744, 45760, 0, 0, 325, 220, COMPRESSED_RGBA_ASTC_5x5_KHR};
Image PL_FEMALE = {269504, 25344, 0, 0, 180, 220, COMPRESSED_RGBA_ASTC_5x5_KHR};
Image PL_FEMALE_GREEN = {294848, 25344, 0, 0, 180, 220, COMPRESSED_RGBA_ASTC_5x5_KHR};


Image TR_BEGIN = {4096};
Image TR_CAR = {320192, 81600, 0, 0, 600, 306, COMPRESSED_RGBA_ASTC_6x6_KHR};
Image TR_BATTERY = {401792, 45632, 0, 0, 300, 152, COMPRESSED_RGBA_ASTC_4x4_KHR};
Image TR_BAT_GREEN = {447424, 4672, 0, 0, 40, 116, COMPRESSED_RGBA_ASTC_4x4_KHR};
Image TR_BAT_RED = {452096, 4672, 0, 0, 40, 116, COMPRESSED_RGBA_ASTC_4x4_KHR};

Image TR_CHARGE_BUTTON = {456768, 63552, 0, 0, 252, 252, COMPRESSED_RGBA_ASTC_4x4_KHR};
Image TR_CHARGE_TEXT = {520320, 896, 0, 0, 24, 36, COMPRESSED_RGBA_ASTC_4x4_KHR};
Image TR_ROUND_ENERGY = {521216, 7104, 0, 0, 84, 84, COMPRESSED_RGBA_ASTC_4x4_KHR};
Image TR_EXIT_BUTTON = {528320, 25344, 0, 0, 352, 72, COMPRESSED_RGBA_ASTC_4x4_KHR};
Image TR_QR_CODE = {553664, 23104, 0, 0, 150, 150, COMPRESSED_RGBA_ASTC_4x4_KHR};

Font FontBegin, FontTitle, FontSmall, FontBottom, FontNumber;
Font FontBegin =    {4096,   0, 4096, 0};
Font FontTitleEN =  {576768, 39744, 616512, 128};
Font FontBottomEN = {616640, 22080, 638720, 128};
Font FontSmallEN =  {638848, 22080, 660928, 128};
Font FontNumber =   {661056, 40960, 702016, 192};
Font FontTitleCH =  {702208, 44928, 747136, 128};
Font FontBottomCH = {747264, 44928, 792192, 128};
Font FontSmallCH =  {792320, 19968, 812288, 128};
Font FontLangCH =   {812416, 1728,  814144, 64};

uint16_t BATTERY_CELL_X[5] = {39, 90, 139, 191, 240};
uint16_t BATTERY_CELL_Y = 19;
