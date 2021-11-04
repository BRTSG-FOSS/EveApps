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
#include "Widget.h"

#define SAMAPP_INFO_TEXT(str)  Draw_TextColor(s_pHalContext, str, (uint8_t[]) { 0x77, 0x77, 0x77 }, (uint8_t[]) { 255, 255, 255 })
#define SAMAPP_INFO_START      Display_StartColor(s_pHalContext,  (uint8_t[]) { 0x77, 0x77, 0x77 }, (uint8_t[]) { 255, 255, 255 })
#define SAMAPP_INFO_END        Display_End(s_pHalContext);
#define SAMAPP_DELAY_NEXT      EVE_sleep(2000);

static EVE_HalContext s_halContext;
static EVE_HalContext* s_pHalContext;
void SAMAPP_Widget();

/* Header of raw data containing properties of bitmap */
SAMAPP_Bitmap_header_t SAMAPP_Bitmap_RawData_Header[] =
{
    /* format,width,height,stride,arrayoffset */
    { RGB565      ,    40,      40,    40 * 2,    0 },
#ifdef FT81X_ENABLE
    { PALETTED4444,    40,     40 ,    40    ,    0 },
    { PALETTED8   ,    480,    272,    480   ,    0 },
    { PALETTED8   ,    802,    520,    802   ,    0 },
#else
    { PALETTED    ,    40 ,    40 ,    40    ,    0 },
    { PALETTED    ,    480,    272,    480   ,    0 },
#endif
};

int main(int argc, char* argv[])
{
    s_pHalContext = &s_halContext;
    Gpu_Init(s_pHalContext);

    // read and store calibration setting
#if !defined(BT8XXEMU_PLATFORM) && GET_CALIBRATION == 1
    Esd_Calibrate(s_pHalContext);
    Calibration_Save(s_pHalContext);
#endif

    EVE_Util_clearScreen(s_pHalContext);

    char *info[] =
    {  "EVE Sample Application",
        "This sample demonstrate the using of basic widgets", 
        "",
        ""
    }; 

    while (TRUE) {
        WelcomeScreen(s_pHalContext, info);

        SAMAPP_Widget();

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
* @brief Show logo
*
*/
void SAMAPP_Widget_logo()
{
    Draw_Text(s_pHalContext, "Example for: Showing logo");

    Logo_Showup(s_pHalContext);

    printf("Logo animation has finished playing.\n");

    SAMAPP_DELAY_NEXT;
    Display_Start(s_pHalContext);
    Display_End(s_pHalContext);
    EVE_Util_clearScreen(s_pHalContext);
}

/**
* @brief demonstrate clock widget
*
*/
void SAMAPP_Widget_clock()
{
    Draw_Text(s_pHalContext, "Example for: clock widget");

    /*************************************************************************/
    /* Below code demonstrates the usage of clock function. Clocks can be    */
    /* constructed using flat or 3d effect. Clock background and foreground  */
    /* colors can be set by gbcolor and colorrgb. Clock can be constructed   */
    /* with multiple options such as no background, no needles, no pointer.  */
    /*************************************************************************/
    int16_t xOffset;
    int16_t yOffset;
    int16_t radius;
    int16_t xDistBtwClocks;

    xDistBtwClocks = (int16_t) (s_pHalContext->Width / 5);
    radius = xDistBtwClocks / 2 - (int16_t) (s_pHalContext->Width / 64);

    /* Download the bitmap data for lena faced clock */
    Ftf_Write_File_nBytes_To_RAM_G(s_pHalContext, TEST_DIR "\\SAMAPP_Bitmap_RawData.bin", RAM_G,
        SAMAPP_Bitmap_RawData_Header[0].Stride * SAMAPP_Bitmap_RawData_Header[0].Height,
        SAMAPP_Bitmap_RawData_Header[0].Arrayoffset);

    /* Draw clock with blue as background and read as needle color */
    SAMAPP_INFO_START;
    /* flat effect and default color background */
    xOffset = xDistBtwClocks / 2;
    yOffset = radius + 5;
    EVE_CoCmd_bgColor(s_pHalContext, 0x0000ff);
    EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0xff, 0x00, 0x00));
    EVE_CoCmd_clock(s_pHalContext, xOffset, yOffset, radius, OPT_FLAT, 30, 100, 5, 10);
    EVE_CoCmd_text(s_pHalContext, xOffset, (yOffset + radius + 6), 26, OPT_CENTER, "Flat effect"); //text info
    /* no seconds needle */
    EVE_CoCmd_bgColor(s_pHalContext, 0x00ff00);
    EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0xff, 0x00, 0x00));
    EVE_CoCmd_fgColor(s_pHalContext, 0xff0000);
    xOffset += xDistBtwClocks;
    EVE_CoCmd_clock(s_pHalContext, xOffset, yOffset, radius, OPT_NOSECS, 10, 10, 5, 10);
    EVE_Cmd_wr32(s_pHalContext, COLOR_A(255));
    EVE_CoCmd_text(s_pHalContext, xOffset, (yOffset + radius + 6), 26, OPT_CENTER, "No Secs"); //text info
    /* no background color */
    EVE_CoCmd_bgColor(s_pHalContext, 0x00ffff);
    EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0xff, 0xff, 0x00));
    xOffset += xDistBtwClocks;
    EVE_CoCmd_clock(s_pHalContext, xOffset, yOffset, radius, OPT_NOBACK, 10, 10, 5, 10);
    EVE_CoCmd_text(s_pHalContext, xOffset, (yOffset + radius + 6), 26, OPT_CENTER, "No BG"); //text info
    /* No ticks */
    EVE_CoCmd_bgColor(s_pHalContext, 0xff00ff);
    EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0x00, 0xff, 0xff));
    xOffset += xDistBtwClocks;
    EVE_CoCmd_clock(s_pHalContext, xOffset, yOffset, radius, OPT_NOTICKS, 10, 10, 5, 10);
    EVE_CoCmd_text(s_pHalContext, xOffset, (yOffset + radius + 6), 26, OPT_CENTER, "No Ticks"); //text info
    /* No hands */
    EVE_CoCmd_bgColor(s_pHalContext, 0x808080);
    EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0x00, 0xff, 0x00));
    xOffset += xDistBtwClocks;
    EVE_CoCmd_clock(s_pHalContext, xOffset, yOffset, radius, OPT_NOHANDS, 10, 10, 5, 10);
    EVE_CoCmd_text(s_pHalContext, xOffset, (yOffset + radius + 6), 26, OPT_CENTER, "No Hands"); //text info
    /* Bigger clock */
    yOffset += (radius + 10);
    radius = (int16_t) (s_pHalContext->Height - (2 * radius + 5 + 10)); //calculate radius based on remaining height
    radius = (radius - 5 - 20) / 2;
    xOffset = radius + 10;
    yOffset += radius + 5;
    EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0x00, 0x00, 0xff));
    EVE_CoCmd_clock(s_pHalContext, xOffset, yOffset, radius, 0, 10, 10, 5, 10);

    xOffset += 2 * radius + 10;
    /* Lena clock with no background and no ticks */
    EVE_Cmd_wr32(s_pHalContext, LINE_WIDTH(10 * 16));
    EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0xff, 0xff, 0xff));
    EVE_Cmd_wr32(s_pHalContext, BEGIN(RECTS));
    EVE_Cmd_wr32(s_pHalContext,
        VERTEX2F((xOffset - radius + 10) * 16, (yOffset - radius + 10) * 16));
    EVE_Cmd_wr32(s_pHalContext,
        VERTEX2F((xOffset + radius - 10) * 16, (yOffset + radius - 10) * 16));
    EVE_Cmd_wr32(s_pHalContext, END());
    EVE_Cmd_wr32(s_pHalContext, COLOR_A(0xff));
    EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0xff, 0xff, 0xff));
    EVE_Cmd_wr32(s_pHalContext, COLOR_MASK(0, 0, 0, 1));
    EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1));
    EVE_Cmd_wr32(s_pHalContext, BEGIN(RECTS));
    EVE_Cmd_wr32(s_pHalContext,
        VERTEX2F((xOffset - radius + 12) * 16, (yOffset - radius + 12) * 16));
    EVE_Cmd_wr32(s_pHalContext,
        VERTEX2F((xOffset + radius - 12) * 16, (yOffset + radius - 12) * 16));
    EVE_Cmd_wr32(s_pHalContext, END());
    EVE_Cmd_wr32(s_pHalContext, COLOR_MASK(1, 1, 1, 1));
    EVE_Cmd_wr32(s_pHalContext, BLEND_FUNC(DST_ALPHA, ONE_MINUS_DST_ALPHA));
    /* Lena bitmap - scale proportionately wrt output resolution */
    EVE_CoCmd_loadIdentity(s_pHalContext);
    EVE_CoCmd_scale(s_pHalContext, 65536 * 2 * radius / SAMAPP_Bitmap_RawData_Header[0].Width,
        65536 * 2 * radius / SAMAPP_Bitmap_RawData_Header[0].Height);
    EVE_CoCmd_setMatrix(s_pHalContext);
    EVE_Cmd_wr32(s_pHalContext, BEGIN(BITMAPS));
    EVE_Cmd_wr32(s_pHalContext, BITMAP_SOURCE(0));
    EVE_Cmd_wr32(s_pHalContext,
        BITMAP_LAYOUT(SAMAPP_Bitmap_RawData_Header[0].Format,
            SAMAPP_Bitmap_RawData_Header[0].Stride, SAMAPP_Bitmap_RawData_Header[0].Height));
    EVE_Cmd_wr32(s_pHalContext, BITMAP_SIZE(BILINEAR, BORDER, BORDER, 2 * radius, 2 * radius));
    EVE_Cmd_wr32(s_pHalContext, VERTEX2F((xOffset - radius) * 16, (yOffset - radius) * 16));
    EVE_Cmd_wr32(s_pHalContext, END());
    EVE_Cmd_wr32(s_pHalContext, BLEND_FUNC(SRC_ALPHA, ONE_MINUS_SRC_ALPHA));
    EVE_CoCmd_loadIdentity(s_pHalContext);
    EVE_CoCmd_setMatrix(s_pHalContext);
    EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0xff, 0xff, 0xff));
    EVE_CoCmd_clock(s_pHalContext, xOffset, yOffset, radius, OPT_NOTICKS | OPT_NOBACK, 10, 10, 5,
        10);
    SAMAPP_INFO_END;
    SAMAPP_DELAY_NEXT;
}

/**
* @brief demonstrate a gauge widget
*/
void SAMAPP_Widget_gauge()
{
    Draw_Text(s_pHalContext, "Example for: gauge widget");

    /*************************************************************************/
    /* Below code demonstrates the usage of gauge function. Gauge can be     */
    /* constructed using flat or 3d effect. Gauge background and foreground  */
    /* colors can be set by gbcolor and colorrgb. Gauge can be constructed   */
    /* with multiple options such as no background, no minors/majors and     */
    /* no pointer.                                                           */
    /*************************************************************************/
    int16_t xOffset;
    int16_t yOffset;
    int16_t cRadius;
    int16_t xDistBtwClocks;

    xDistBtwClocks = (int16_t) (s_pHalContext->Width / 5);
    cRadius = xDistBtwClocks / 2 - (int16_t) (s_pHalContext->Width / 64);
    yOffset = cRadius + 5;

    /* Download the bitmap data */
    Ftf_Write_File_nBytes_To_RAM_G(s_pHalContext, TEST_DIR "\\SAMAPP_Bitmap_RawData.bin", RAM_G,
        SAMAPP_Bitmap_RawData_Header[0].Stride * SAMAPP_Bitmap_RawData_Header[0].Height,
        SAMAPP_Bitmap_RawData_Header[0].Arrayoffset);

    /* Draw gauge with blue as background and read as needle color */
    SAMAPP_INFO_START;

    /* flat effect and default color background */
    xOffset = xDistBtwClocks / 2;
    EVE_CoCmd_bgColor(s_pHalContext, 0x0000ff);
    EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0xff, 0x00, 0x00));
    EVE_CoCmd_gauge(s_pHalContext, xOffset, yOffset, cRadius, OPT_FLAT, 5, 4, 45, 100);
    EVE_CoCmd_text(s_pHalContext, xOffset, (yOffset + cRadius + 6), 26, OPT_CENTER, "Flat effect"); //text info

    /* 3d effect */
    EVE_CoCmd_bgColor(s_pHalContext, 0x00ff00);
    EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0xff, 0x00, 0x00));
    EVE_CoCmd_fgColor(s_pHalContext, 0xff0000);
    xOffset += xDistBtwClocks;
    EVE_CoCmd_gauge(s_pHalContext, xOffset, yOffset, cRadius, 0, 5, 1, 60, 100);
    EVE_CoCmd_text(s_pHalContext, xOffset, (yOffset + cRadius + 6), 26, OPT_CENTER, "3d effect"); //text info

    /* no background color */
    EVE_CoCmd_bgColor(s_pHalContext, 0x00ffff);
    EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0xff, 0xff, 0x00));
    xOffset += xDistBtwClocks;
    EVE_CoCmd_gauge(s_pHalContext, xOffset, yOffset, cRadius, OPT_NOBACK, 1, 6, 90, 100);
    EVE_Cmd_wr32(s_pHalContext, COLOR_A(255));
    EVE_CoCmd_text(s_pHalContext, xOffset, (yOffset + cRadius + 6), 26, OPT_CENTER, "No BG"); //text info

    /* No ticks */
    EVE_CoCmd_bgColor(s_pHalContext, 0xff00ff);
    EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0x00, 0xff, 0xff));
    xOffset += xDistBtwClocks;
    EVE_CoCmd_gauge(s_pHalContext, xOffset, yOffset, cRadius, OPT_NOTICKS, 5, 4, 20, 100);
    EVE_CoCmd_text(s_pHalContext, xOffset, (yOffset + cRadius + 6), 26, OPT_CENTER, "No Ticks"); //text info

    /* No hands */
    EVE_CoCmd_bgColor(s_pHalContext, 0x808080);
    EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0x00, 0xff, 0x00));
    xOffset += xDistBtwClocks;
    EVE_CoCmd_gauge(s_pHalContext, xOffset, yOffset, cRadius, OPT_NOTICKS, 5, 4, 55, 100);
    EVE_Cmd_wr32(s_pHalContext, COLOR_A(255));
    EVE_CoCmd_text(s_pHalContext, xOffset, (yOffset + cRadius + 6), 26, OPT_CENTER, "No Hands"); //text info

    /* Bigger gauge */
    yOffset += cRadius + 10;
    cRadius = (int16_t) (s_pHalContext->Height - (2 * cRadius + 5 + 10)); //calculate radius based on remaining height
    cRadius = (cRadius - 5 - 20) / 2;
    xOffset = cRadius + 10;
    yOffset += cRadius + 5;
    EVE_CoCmd_bgColor(s_pHalContext, 0x808000);
    EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0xff, 0xff, 0xff));
    EVE_CoCmd_gauge(s_pHalContext, xOffset, yOffset, cRadius, OPT_NOBACK, 5, 4, 80, 100);
    EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0xff, 0x00, 0x00));
    EVE_CoCmd_gauge(s_pHalContext, xOffset, yOffset, cRadius, OPT_NOTICKS | OPT_NOBACK, 5, 4, 30,
        100);

    xOffset += 2 * cRadius + 10;
    /* Lena guage with no background and no ticks */
    EVE_Cmd_wr32(s_pHalContext, POINT_SIZE(cRadius * 16));
    EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0xff, 0xff, 0xff));
    EVE_Cmd_wr32(s_pHalContext, BEGIN(FTPOINTS));
    EVE_Cmd_wr32(s_pHalContext, VERTEX2F(xOffset * 16, yOffset * 16));
    EVE_Cmd_wr32(s_pHalContext, END());
    EVE_Cmd_wr32(s_pHalContext, COLOR_A(0xff));
    EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0xff, 0xff, 0xff));
    EVE_Cmd_wr32(s_pHalContext, COLOR_MASK(0, 0, 0, 1));
    EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1));
    EVE_Cmd_wr32(s_pHalContext, BEGIN(FTPOINTS));
    EVE_Cmd_wr32(s_pHalContext, POINT_SIZE((cRadius - 2) * 16));
    EVE_Cmd_wr32(s_pHalContext, VERTEX2F(xOffset * 16, yOffset * 16));
    EVE_Cmd_wr32(s_pHalContext, END());
    EVE_Cmd_wr32(s_pHalContext, COLOR_MASK(1, 1, 1, 1));
    EVE_Cmd_wr32(s_pHalContext, BLEND_FUNC(DST_ALPHA, ONE_MINUS_DST_ALPHA));
    /* Lena bitmap - scale proportionately wrt output resolution */
    EVE_CoCmd_loadIdentity(s_pHalContext);
    EVE_CoCmd_scale(s_pHalContext, 65536 * 2 * cRadius / SAMAPP_Bitmap_RawData_Header[0].Width,
        65536 * 2 * cRadius / SAMAPP_Bitmap_RawData_Header[0].Height);
    EVE_CoCmd_setMatrix(s_pHalContext);
    EVE_Cmd_wr32(s_pHalContext, BEGIN(BITMAPS));
    EVE_Cmd_wr32(s_pHalContext, BITMAP_SOURCE(0));
    EVE_Cmd_wr32(s_pHalContext,
        BITMAP_LAYOUT(SAMAPP_Bitmap_RawData_Header[0].Format,
            SAMAPP_Bitmap_RawData_Header[0].Stride, SAMAPP_Bitmap_RawData_Header[0].Height));
    EVE_Cmd_wr32(s_pHalContext, BITMAP_SIZE(BILINEAR, BORDER, BORDER, 2 * cRadius, 2 * cRadius));
    EVE_Cmd_wr32(s_pHalContext, VERTEX2F((xOffset - cRadius) * 16, (yOffset - cRadius) * 16));
    EVE_Cmd_wr32(s_pHalContext, END());
    EVE_Cmd_wr32(s_pHalContext, BLEND_FUNC(SRC_ALPHA, ONE_MINUS_SRC_ALPHA));
    EVE_CoCmd_loadIdentity(s_pHalContext);
    EVE_CoCmd_setMatrix(s_pHalContext);

    EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0xff, 0xff, 0xff));
    EVE_CoCmd_gauge(s_pHalContext, xOffset, yOffset, cRadius, OPT_NOTICKS | OPT_NOBACK, 5, 4, 30,
        100);

    SAMAPP_INFO_END;
    SAMAPP_DELAY_NEXT;
}

/**
* @brief demonstrate gradient widget
*
*/
void SAMAPP_Widget_gradient()
{
    Draw_Text(s_pHalContext, "Example for: gradient widget");

    /*************************************************************************/
    /* Below code demonstrates the usage of gradient function. Gradient func */
    /* can be used to construct three effects - horizontal, vertical and     */
    /* diagonal effects.                                                      */
    /*************************************************************************/
    int16_t wScissor;
    int16_t hScissor;
    int16_t xOffset;
    int16_t yOffset;

    xOffset = 10;
    yOffset = 20;
    hScissor = (int16_t) ((s_pHalContext->Height - 3 * 20) / 2);
    wScissor = (int16_t) ((s_pHalContext->Width - 4 * 10) / 3);

    /* Draw gradient  */
    SAMAPP_INFO_START;
    EVE_Cmd_wr32(s_pHalContext, SCISSOR_SIZE(wScissor, hScissor));
    /* Horizontal gradient effect */
    EVE_Cmd_wr32(s_pHalContext, SCISSOR_XY(xOffset, yOffset)); //clip the display
    EVE_CoCmd_gradient(s_pHalContext, xOffset, yOffset, 0x808080, (xOffset + wScissor), yOffset,
        0xffff00);
    /* Vertical gradient effect */
    xOffset += wScissor + 10;
    EVE_Cmd_wr32(s_pHalContext, SCISSOR_XY(xOffset, yOffset)); //clip the display
    EVE_CoCmd_gradient(s_pHalContext, xOffset, yOffset, 0xff0000, xOffset, (yOffset + hScissor),
        0x00ff00);
    /* diagonal gradient effect */
    xOffset += wScissor + 10;
    EVE_Cmd_wr32(s_pHalContext, SCISSOR_XY(xOffset, yOffset)); //clip the display
    EVE_CoCmd_gradient(s_pHalContext, xOffset, yOffset, 0x800000, (xOffset + wScissor),
        (yOffset + hScissor), 0xffffff);
    /* Diagonal gradient with text info */
    xOffset = 10;
    yOffset += hScissor + 20;
    EVE_Cmd_wr32(s_pHalContext, SCISSOR_SIZE(wScissor, 30));
    EVE_Cmd_wr32(s_pHalContext, SCISSOR_XY(xOffset, (yOffset + hScissor / 2 - 15))); //clip the display
    EVE_CoCmd_gradient(s_pHalContext, xOffset, (yOffset + hScissor / 2 - 15), 0x606060,
        (xOffset + wScissor), (yOffset + hScissor / 2 + 15), 0x404080);
    EVE_CoCmd_text(s_pHalContext, (xOffset + wScissor / 2), (yOffset + hScissor / 2), 28,
        OPT_CENTER, "Heading 1"); //text info

    /* Draw horizontal, vertical and diagonal with alpha */
    xOffset += wScissor + 10;
    EVE_Cmd_wr32(s_pHalContext, SCISSOR_SIZE(wScissor, hScissor));
    EVE_Cmd_wr32(s_pHalContext, SCISSOR_XY(xOffset, yOffset)); //clip the display
    EVE_CoCmd_gradient(s_pHalContext, xOffset, yOffset, 0x808080, (xOffset + wScissor), yOffset,
        0xffff00);
    wScissor -= 30;
    hScissor -= 30;
    EVE_Cmd_wr32(s_pHalContext, SCISSOR_SIZE(wScissor, hScissor));
    xOffset += 15;
    yOffset += 15;
    EVE_Cmd_wr32(s_pHalContext, SCISSOR_XY(xOffset, yOffset)); //clip the display
    EVE_CoCmd_gradient(s_pHalContext, xOffset, yOffset, 0x800000, xOffset, (yOffset + hScissor),
        0xffffff);
    wScissor -= 30;
    hScissor -= 30;
    EVE_Cmd_wr32(s_pHalContext, SCISSOR_SIZE(wScissor, hScissor));
    xOffset += 15;
    yOffset += 15;
    EVE_Cmd_wr32(s_pHalContext, SCISSOR_XY(xOffset, yOffset)); //clip the display
    EVE_CoCmd_gradient(s_pHalContext, xOffset, yOffset, 0x606060, (xOffset + wScissor),
        (yOffset + hScissor), 0x404080);

    /* Display the text wrt gradient */
    wScissor = (int16_t) ((s_pHalContext->Width - 4 * 10) / 3);
    hScissor = (int16_t) ((s_pHalContext->Height - 3 * 20) / 2);
    xOffset = 10 + wScissor / 2;
    yOffset = 20 + hScissor + 5;
    EVE_Cmd_wr32(s_pHalContext, SCISSOR_XY(0, 0)); //set to default values
#ifndef FT81X_ENABLE
    EVE_Cmd_wr32(s_pHalContext, SCISSOR_SIZE(512, 512));
#else
    EVE_Cmd_wr32(s_pHalContext, SCISSOR_SIZE(2048, 2048));
#endif
    EVE_CoCmd_text(s_pHalContext, xOffset, yOffset, 26, OPT_CENTER, "Horizontal grad"); //text info
    xOffset += wScissor + 10;
    EVE_CoCmd_text(s_pHalContext, xOffset, yOffset, 26, OPT_CENTER, "Vertical grad"); //text info
    xOffset += wScissor + 10;
    EVE_CoCmd_text(s_pHalContext, xOffset, yOffset, 26, OPT_CENTER, "Diagonal grad"); //text info

    SAMAPP_INFO_END;
    SAMAPP_DELAY_NEXT;
}

/**
* @brief Key interactive
*
*/
void SAMAPP_Widget_keysInteractive()
{
    Draw_Text(s_pHalContext, "Example for: Key interactive");

    /*************************************************************************/
    /* Below code demonstrates the usage of keys function. keys function     */
    /* draws buttons with characters given as input parameters. Keys support */
    /* Flat and 3D effects, draw at (x,y) coordinates or center of the display*/
    /* , inbuilt or custom fonts can be used for key display                 */
    /*************************************************************************/
    int32_t loopflag = 600;
    int16_t TextFont = 29;
    int16_t ButtonW = 30;
    int16_t ButtonH = 30;
    int16_t yBtnDst = 5;
    int16_t yOffset;
#define SAMAPP_COPRO_WIDGET_KEYS_INTERACTIVE_TEXTSIZE (512)
    char8_t DispText[SAMAPP_COPRO_WIDGET_KEYS_INTERACTIVE_TEXTSIZE];
    char8_t CurrChar = '|';
    uint8_t CurrTag = 0;
    uint8_t PrevTag = 0;
    uint8_t Pendown = 1;
    int32_t CurrTextIdx = 0;
#ifdef SAMAPP_DISPLAY_QVGA
    TextFont = 27;
    ButtonW = 22;
    ButtonH = 22;
    yBtnDst = 3;
#endif

#ifdef SAMAPP_DISPLAY_WVGA
    TextFont = 30;
    ButtonW = 38;
    ButtonH = 38;
    yBtnDst = 15;
#endif

    while (loopflag--)
    {
        /* Check the user input and then add the characters into array */
        CurrTag = EVE_Hal_rd8(s_pHalContext, REG_TOUCH_TAG);

        CurrChar = CurrTag;
        if (0 == CurrTag)
        {
            CurrChar = '|';
        }

        /* check whether pwndown has happened */
        if (!CurrTag && PrevTag && 1 == Pendown && 0 != PrevTag)
        {
            CurrTextIdx++;
            /* clear all the charaters after 100 are pressed */
            if (CurrTextIdx > 24)
            {
                CurrTextIdx = 0;
            }
        }

        SAMAPP_INFO_START;
        /* Draw text entered by user */
        /* make sure the array is a string */
        DispText[CurrTextIdx] = CurrChar;
        DispText[CurrTextIdx + 1] = '\0';

        EVE_Cmd_wr32(s_pHalContext, TAG_MASK(0));
        EVE_CoCmd_text(s_pHalContext, (int16_t) (s_pHalContext->Width / 2), 40, TextFont,
            OPT_CENTER, DispText); //text info
        EVE_Cmd_wr32(s_pHalContext, TAG_MASK(1));

        yOffset = ButtonW * 2 + 10;
        /* Construct a simple keyboard - note that the tags associated with the keys are the character values given in the arguments */
        EVE_CoCmd_fgColor(s_pHalContext, 0x404080);
        EVE_CoCmd_gradColor(s_pHalContext, 0x00ff00);
        EVE_CoCmd_keys(s_pHalContext, yBtnDst, yOffset, 10 * ButtonW, ButtonH, TextFont,
            (OPT_CENTER | CurrTag), "qwertyuiop");
        EVE_CoCmd_gradColor(s_pHalContext, 0x00ffff);
        yOffset += ButtonH + yBtnDst;
        EVE_CoCmd_keys(s_pHalContext, yBtnDst, yOffset, 10 * ButtonW, ButtonH, TextFont,
            (OPT_CENTER | CurrTag), "asdfghjkl");
        EVE_CoCmd_gradColor(s_pHalContext, 0xffff00);
        yOffset += ButtonH + yBtnDst;
        EVE_CoCmd_keys(s_pHalContext, yBtnDst, yOffset, 10 * ButtonW, ButtonH, TextFont,
            (OPT_CENTER | CurrTag), "zxcvbnm"); //hilight button z
        yOffset += ButtonH + yBtnDst;
        EVE_Cmd_wr32(s_pHalContext, TAG(' '));
        if (' ' == CurrTag)
        {
            EVE_CoCmd_button(s_pHalContext, yBtnDst, yOffset, 10 * ButtonW, ButtonH, TextFont,
                OPT_FLAT, " "); //mandatory to give '\0' at the end to make sure coprocessor understands the string end
        }
        else
        {
            EVE_CoCmd_button(s_pHalContext, yBtnDst, yOffset, 10 * ButtonW, ButtonH, TextFont, 0,
                " "); //mandatory to give '\0' at the end to make sure coprocessor understands the string end
        }
        yOffset = ButtonW * 2 + 10;
        EVE_CoCmd_keys(s_pHalContext, 11 * ButtonW, yOffset, 3 * ButtonW, ButtonH, TextFont,
            (0 | CurrTag), "789");
        yOffset += ButtonH + yBtnDst;
        EVE_CoCmd_keys(s_pHalContext, 11 * ButtonW, yOffset, 3 * ButtonW, ButtonH, TextFont,
            (0 | CurrTag), "456");
        yOffset += ButtonH + yBtnDst;
        EVE_CoCmd_keys(s_pHalContext, 11 * ButtonW, yOffset, 3 * ButtonW, ButtonH, TextFont,
            (0 | CurrTag), "123");
        yOffset += ButtonH + yBtnDst;
        EVE_Cmd_wr32(s_pHalContext, COLOR_A(255));
        EVE_CoCmd_keys(s_pHalContext, 11 * ButtonW, yOffset, 3 * ButtonW, ButtonH, TextFont,
            (0 | CurrTag), "0."); //hilight button 0
        SAMAPP_INFO_END;
        EVE_sleep(10);
        PrevTag = CurrTag;
    }
}

/**
* @brief  keys widget
*
*/
void SAMAPP_Widget_keys()
{
    Draw_Text(s_pHalContext, "Example for: Keys widget");

    /*************************************************************************/
    /* Below code demonstrates the usage of keys function. keys function     */
    /* draws buttons with characters given as input parameters. Keys support */
    /* Flat and 3D effects, draw at (x,y) coordinates or center of the display*/
    /* , inbuilt or custom fonts can be used for key display                 */
    /*************************************************************************/
    int16_t TextFont = 29;
    int16_t ButtonW = 30;
    int16_t ButtonH = 30;
    int16_t yBtnDst = 5;
    int16_t yOffset;
    int16_t xOffset;
#ifdef SAMAPP_DISPLAY_QVGA
    TextFont = 27;
    ButtonW = 22;
    ButtonH = 22;
    yBtnDst = 3;
#endif

#ifdef SAMAPP_DISPLAY_WVGA
    TextFont = 30;
    ButtonW = 38;
    ButtonH = 38;
    yBtnDst = 15;
#endif
    SAMAPP_INFO_START;
    EVE_Cmd_wr32(s_pHalContext, LINE_WIDTH(1 * 16));
    /* Draw keys with flat effect */
    yOffset = ButtonW + 10 + yBtnDst;
    EVE_CoCmd_fgColor(s_pHalContext, 0x404080);
    EVE_CoCmd_keys(s_pHalContext, 10, 10, 4 * ButtonW, 30, TextFont, OPT_FLAT, "ABCD");
    EVE_CoCmd_text(s_pHalContext, 10, yOffset, 26, 0, "Flat effect"); //text info
    /* Draw keys with 3d effect */
    EVE_CoCmd_fgColor(s_pHalContext, 0x800000);
    xOffset = 4 * ButtonW + 20;
    EVE_CoCmd_keys(s_pHalContext, xOffset, 10, 4 * ButtonW, 30, TextFont, 0, "ABCD");
    EVE_CoCmd_text(s_pHalContext, xOffset, yOffset, 26, 0, "3D effect"); //text info
    /* Draw keys with center option */
    EVE_CoCmd_fgColor(s_pHalContext, 0xffff000);
    xOffset += 4 * ButtonW + 20;
    EVE_CoCmd_keys(s_pHalContext, xOffset, 10, (int16_t) (s_pHalContext->Width - 230), 30, TextFont,
        OPT_CENTER, "ABCD");
    EVE_CoCmd_text(s_pHalContext, xOffset, yOffset, 26, 0, "Option Center"); //text info

    yOffset = ButtonW * 2 + 10;
    /* Construct a simple keyboard - note that the tags associated with the keys are the character values given in the arguments */
    EVE_CoCmd_fgColor(s_pHalContext, 0x404080);
    EVE_CoCmd_gradColor(s_pHalContext, 0x00ff00);
    EVE_CoCmd_keys(s_pHalContext, yBtnDst, yOffset, 10 * ButtonW, ButtonH, TextFont, OPT_CENTER,
        "qwertyuiop");
    EVE_CoCmd_gradColor(s_pHalContext, 0x00ffff);
    yOffset += ButtonH + yBtnDst;
    EVE_CoCmd_keys(s_pHalContext, yBtnDst, yOffset, 10 * ButtonW, ButtonH, TextFont, OPT_CENTER,
        "asdfghjkl");
    EVE_CoCmd_gradColor(s_pHalContext, 0xffff00);
    yOffset += ButtonH + yBtnDst;
    EVE_CoCmd_keys(s_pHalContext, yBtnDst, yOffset, 10 * ButtonW, ButtonH, TextFont,
        (OPT_CENTER | 'z'), "zxcvbnm"); //hilight button z
    yOffset += ButtonH + yBtnDst;
    EVE_CoCmd_button(s_pHalContext, yBtnDst, yOffset, 10 * ButtonW, ButtonH, TextFont, 0, " "); //mandatory to give '\0' at the end to make sure coprocessor understands the string end
    yOffset = 80 + 10;
    EVE_CoCmd_keys(s_pHalContext, 11 * ButtonW, yOffset, 3 * ButtonW, ButtonH, TextFont, 0, "789");
    yOffset += ButtonH + yBtnDst;
    EVE_CoCmd_keys(s_pHalContext, 11 * ButtonW, yOffset, 3 * ButtonW, ButtonH, TextFont, 0, "456");
    yOffset += ButtonH + yBtnDst;
    EVE_CoCmd_keys(s_pHalContext, 11 * ButtonW, yOffset, 3 * ButtonW, ButtonH, TextFont, 0, "123");
    yOffset += ButtonH + yBtnDst;
    EVE_Cmd_wr32(s_pHalContext, COLOR_A(255));
    EVE_CoCmd_keys(s_pHalContext, 11 * ButtonW, yOffset, 3 * ButtonW, ButtonH, TextFont, (0 | '0'),
        "0."); //hilight button 0
    SAMAPP_INFO_END;
    SAMAPP_DELAY_NEXT;
}

/**
* @brief demonstrate progress bar widget
*
*/
void SAMAPP_Widget_progressbar()
{
    Draw_Text(s_pHalContext, "Example for: Progress bar widget");

    /*************************************************************************/
    /* Below code demonstrates the usage of progress function. Progress func */
    /* draws process bar with fgcolor for the % completion and bgcolor for   */
    /* % remaining. Progress bar supports flat and 3d effets                 */
    /*************************************************************************/
    int16_t xOffset;
    int16_t yOffset;
    int16_t yDist = (int16_t) (s_pHalContext->Width / 12);
    int16_t ySz = (int16_t) (s_pHalContext->Width / 24);

    SAMAPP_INFO_START;
    /* Draw progress bar with flat effect */
    EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0xff, 0xff, 0xff));
    EVE_CoCmd_bgColor(s_pHalContext, 0x404080);
    EVE_CoCmd_progress(s_pHalContext, 20, 10, 120, 20, OPT_FLAT, 50, 100); //note that h/2 will be added on both sides of the progress bar
    EVE_Cmd_wr32(s_pHalContext, COLOR_A(255));
    EVE_CoCmd_text(s_pHalContext, 20, 40, 26, 0, "Flat effect"); //text info
    /* Draw progress bar with 3d effect */
    EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0x00, 0xff, 0x00));
    EVE_CoCmd_bgColor(s_pHalContext, 0x800000);
    EVE_CoCmd_progress(s_pHalContext, 180, 10, 120, 20, 0, 75, 100);
    EVE_Cmd_wr32(s_pHalContext, COLOR_A(255));
    EVE_CoCmd_text(s_pHalContext, 180, 40, 26, 0, "3D effect"); //text info
    /* Draw progress bar with 3d effect and string on top */
    EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0xff, 0x00, 0x00));
    EVE_CoCmd_bgColor(s_pHalContext, 0x000080);
    EVE_CoCmd_progress(s_pHalContext, 30, 60, 120, 30, 0, 19660, 65535);
    EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0xff, 0xff, 0xff));
    EVE_CoCmd_text(s_pHalContext, 78, 68, 26, 0, "30 %"); //text info

    xOffset = 20;
    yOffset = 120;
    EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0x00, 0xa0, 0x00));
    EVE_CoCmd_bgColor(s_pHalContext, 0x800000);
    EVE_CoCmd_progress(s_pHalContext, xOffset, yOffset, 150, ySz, 0, 10, 100);
    EVE_CoCmd_bgColor(s_pHalContext, 0x000080);
    yOffset += yDist;
    EVE_CoCmd_progress(s_pHalContext, xOffset, yOffset, 150, ySz, 0, 40, 100);
    EVE_CoCmd_bgColor(s_pHalContext, 0xffff00);
    yOffset += yDist;
    EVE_CoCmd_progress(s_pHalContext, xOffset, yOffset, 150, ySz, 0, 70, 100);
    EVE_CoCmd_bgColor(s_pHalContext, 0x808080);
    yOffset += yDist;
    EVE_CoCmd_progress(s_pHalContext, xOffset, yOffset, 150, ySz, 0, 90, 100);

    EVE_CoCmd_text(s_pHalContext, xOffset + 180, 80, 26, 0, "40 % TopBottom"); //text info
    EVE_CoCmd_progress(s_pHalContext, xOffset + 180, 100, ySz, 150, 0, 40, 100);

    SAMAPP_INFO_END;
    SAMAPP_DELAY_NEXT;
}

/**
* @brief demonstrate scroll widget
*
*/
void SAMAPP_Widget_scroll()
{
    Draw_Text(s_pHalContext, "Example for: scroll widget");

    /*************************************************************************/
    /* Below code demonstrates the usage of scroll function. Scroll function */
    /* draws scroll bar with fgcolor for inner color and current location and*/
    /* can be given by val parameter */
    /*************************************************************************/
    int16_t xOffset;
    int16_t yOffset;
    int16_t xDist = (int16_t) (s_pHalContext->Width / 12);
    int16_t yDist = (int16_t) (s_pHalContext->Width / 12);
    int16_t wSz;

    SAMAPP_INFO_START;
    /* Draw scroll bar with flat effect */
    EVE_CoCmd_fgColor(s_pHalContext, 0xffff00);
    EVE_CoCmd_bgColor(s_pHalContext, 0x404080);
    EVE_CoCmd_scrollbar(s_pHalContext, 20, 10, 120, 8, OPT_FLAT, 20, 30, 100); //note that h/2 size will be added on both sides of the progress bar
    EVE_Cmd_wr32(s_pHalContext, COLOR_A(255));
    EVE_CoCmd_text(s_pHalContext, 20, 40, 26, 0, "Flat effect"); //text info
    /* Draw scroll bar with 3d effect */
    EVE_CoCmd_fgColor(s_pHalContext, 0x00ff00);
    EVE_CoCmd_bgColor(s_pHalContext, 0x800000);
    EVE_CoCmd_scrollbar(s_pHalContext, 180, 10, 120, 8, 0, 20, 30, 100);
    EVE_Cmd_wr32(s_pHalContext, COLOR_A(255));
    EVE_CoCmd_text(s_pHalContext, 180, 40, 26, 0, "3D effect"); //text info

    xOffset = 20;
    yOffset = 120;
    wSz = (int16_t) ((s_pHalContext->Width / 2) - 40);
    /* Draw horizontal scroll bars */
    EVE_CoCmd_fgColor(s_pHalContext, 0x00a000);
    EVE_CoCmd_bgColor(s_pHalContext, 0x800000);
    EVE_CoCmd_scrollbar(s_pHalContext, xOffset, yOffset, wSz, 8, 0, 10, 30, 100);
    EVE_CoCmd_bgColor(s_pHalContext, 0x000080);
    yOffset += yDist;
    EVE_CoCmd_scrollbar(s_pHalContext, xOffset, yOffset, wSz, 8, 0, 30, 30, 100);
    EVE_Cmd_wr32(s_pHalContext, COLOR_A(255));
    EVE_CoCmd_bgColor(s_pHalContext, 0xffff00);
    yOffset += yDist;
    EVE_CoCmd_scrollbar(s_pHalContext, xOffset, yOffset, wSz, 8, 0, 50, 30, 100);
    EVE_CoCmd_bgColor(s_pHalContext, 0x808080);
    yOffset += yDist;
    EVE_CoCmd_scrollbar(s_pHalContext, xOffset, yOffset, wSz, 8, 0, 70, 30, 100);

    xOffset = (int16_t) (s_pHalContext->Width / 2 + 40);
    yOffset = 80;
    wSz = (int16_t) (s_pHalContext->Height - 100);
    /* draw vertical scroll bars */
    EVE_CoCmd_bgColor(s_pHalContext, 0x800000);
    EVE_CoCmd_scrollbar(s_pHalContext, xOffset, yOffset, 8, wSz, 0, 10, 30, 100);
    EVE_CoCmd_bgColor(s_pHalContext, 0x000080);
    xOffset += xDist;
    EVE_CoCmd_scrollbar(s_pHalContext, xOffset, yOffset, 8, wSz, 0, 30, 30, 100);
    EVE_Cmd_wr32(s_pHalContext, COLOR_A(255));
    EVE_CoCmd_bgColor(s_pHalContext, 0xffff00);
    xOffset += xDist;
    EVE_CoCmd_scrollbar(s_pHalContext, xOffset, yOffset, 8, wSz, 0, 50, 30, 100);
    EVE_CoCmd_bgColor(s_pHalContext, 0x808080);
    xOffset += xDist;
    EVE_CoCmd_scrollbar(s_pHalContext, xOffset, yOffset, 8, wSz, 0, 70, 30, 100);

    SAMAPP_INFO_END;
    SAMAPP_DELAY_NEXT;
}

/**
* @brief demonstrate slider widget
*
*/
void SAMAPP_Widget_slider()
{
    Draw_Text(s_pHalContext, "Example for: Slider widget");
    
    /*************************************************************************/
    /* Below code demonstrates the usage of slider function. Slider function */
    /* draws slider bar with fgcolor for inner color and bgcolor for the knob*/
    /* , contains input parameter for position of the knob                   */
    /*************************************************************************/
    int16_t xOffset;
    int16_t yOffset;
    int16_t xDist = (int16_t) (s_pHalContext->Width / 12);
    int16_t yDist = (int16_t) (s_pHalContext->Width / 12);
    int16_t wSz;

    SAMAPP_INFO_START;
    /* Draw scroll bar with flat effect */
    EVE_CoCmd_fgColor(s_pHalContext, 0xffff00);
    EVE_CoCmd_bgColor(s_pHalContext, 0x000080);
    EVE_CoCmd_slider(s_pHalContext, 20, 10, 120, 10, OPT_FLAT, 30, 100); //note that h/2 size will be added on both sides of the progress bar
    EVE_Cmd_wr32(s_pHalContext, COLOR_A(255));
    EVE_CoCmd_text(s_pHalContext, 20, 40, 26, 0, "Flat effect"); //text info
    /* Draw scroll bar with 3d effect */
    EVE_CoCmd_fgColor(s_pHalContext, 0x00ff00);
    EVE_CoCmd_bgColor(s_pHalContext, 0x800000);
    EVE_CoCmd_slider(s_pHalContext, 180, 10, 120, 10, 0, 50, 100);
    EVE_Cmd_wr32(s_pHalContext, COLOR_A(255));
    EVE_CoCmd_text(s_pHalContext, 180, 40, 26, 0, "3D effect"); //text info

    xOffset = 20;
    yOffset = 120;
    wSz = (int16_t) ((s_pHalContext->Width / 2) - 40);
    /* Draw horizontal slider bars */
    EVE_CoCmd_fgColor(s_pHalContext, 0x00a000);
    EVE_CoCmd_bgColor(s_pHalContext, 0x800000);
    EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(41, 1, 5));
    EVE_CoCmd_slider(s_pHalContext, xOffset, yOffset, wSz, 10, 0, 10, 100);
    EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(11, 7, 65));
    EVE_CoCmd_bgColor(s_pHalContext, 0x000080);
    yOffset += yDist;
    EVE_CoCmd_slider(s_pHalContext, xOffset, yOffset, wSz, 10, 0, 30, 100);
    EVE_Cmd_wr32(s_pHalContext, COLOR_A(255));
    EVE_CoCmd_bgColor(s_pHalContext, 0xffff00);
    EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(87, 94, 9));
    yOffset += yDist;
    EVE_CoCmd_slider(s_pHalContext, xOffset, yOffset, wSz, 10, 0, 50, 100);
    EVE_CoCmd_bgColor(s_pHalContext, 0x808080);
    EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(51, 50, 52));
    yOffset += yDist;
    EVE_CoCmd_slider(s_pHalContext, xOffset, yOffset, wSz, 10, 0, 70, 100);

    xOffset = (int16_t) (s_pHalContext->Width / 2) + 40;
    yOffset = 80;
    wSz = (int16_t) (s_pHalContext->Height - 100);
    /* draw vertical slider bars */
    EVE_CoCmd_bgColor(s_pHalContext, 0x800000);
    EVE_CoCmd_slider(s_pHalContext, xOffset, yOffset, 10, wSz, 0, 10, 100);
    EVE_CoCmd_bgColor(s_pHalContext, 0x000080);
    xOffset += xDist;
    EVE_CoCmd_slider(s_pHalContext, xOffset, yOffset, 10, wSz, 0, 30, 100);
    EVE_Cmd_wr32(s_pHalContext, COLOR_A(255));
    EVE_CoCmd_bgColor(s_pHalContext, 0xffff00);
    xOffset += xDist;
    EVE_CoCmd_slider(s_pHalContext, xOffset, yOffset, 10, wSz, 0, 50, 100);
    EVE_CoCmd_bgColor(s_pHalContext, 0x808080);
    xOffset += xDist;
    EVE_CoCmd_slider(s_pHalContext, xOffset, yOffset, 10, wSz, 0, 70, 100);

    SAMAPP_INFO_END;
    SAMAPP_DELAY_NEXT;
}

/**
* @brief demonstrate dial widget
*
*/
void SAMAPP_Widget_dial()
{
    Draw_Text(s_pHalContext, "Example for: Dial widget");

    /*************************************************************************/
    /* Below code demonstrates the usage of dial function. Dial function     */
    /* draws rounded bar with fgcolor for knob color and colorrgb for pointer*/
    /* , contains input parameter for angle of the pointer                   */
    /*************************************************************************/
    SAMAPP_INFO_START;
    /* Draw dial with flat effect */
    EVE_CoCmd_fgColor(s_pHalContext, 0xffff00);
    EVE_CoCmd_bgColor(s_pHalContext, 0x000080);
    EVE_CoCmd_dial(s_pHalContext, 50, 50, 40, OPT_FLAT, (uint16_t) (0.2 * 65535)); //20%
    EVE_Cmd_wr32(s_pHalContext, COLOR_A(255));
    EVE_CoCmd_text(s_pHalContext, 15, 90, 26, 0, "Flat effect"); //text info
    /* Draw dial with 3d effect */
    EVE_CoCmd_fgColor(s_pHalContext, 0x00ff00);
    EVE_CoCmd_bgColor(s_pHalContext, 0x800000);
    EVE_CoCmd_dial(s_pHalContext, 140, 50, 40, 0, (uint16_t) (0.45 * 65535)); //45%
    EVE_Cmd_wr32(s_pHalContext, COLOR_A(255));
    EVE_CoCmd_text(s_pHalContext, 105, 90, 26, 0, "3D effect"); //text info

    /* Draw increasing dials horizontally */
    EVE_CoCmd_fgColor(s_pHalContext, 0x800000);
    EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(41, 1, 5));
    EVE_CoCmd_dial(s_pHalContext, 30, 160, 20, 0, (uint16_t) (0.30 * 65535));
    EVE_CoCmd_text(s_pHalContext, 20, 180, 26, 0, "30 %"); //text info
    EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(11, 7, 65));
    EVE_CoCmd_fgColor(s_pHalContext, 0x000080);
    EVE_CoCmd_dial(s_pHalContext, 100, 160, 40, 0, (uint16_t) (0.45 * 65535));
    EVE_Cmd_wr32(s_pHalContext, COLOR_A(255));
    EVE_CoCmd_text(s_pHalContext, 90, 200, 26, 0, "45 %"); //text info
    EVE_CoCmd_fgColor(s_pHalContext, 0xffff00);
    EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(87, 94, 9));
    EVE_CoCmd_dial(s_pHalContext, 210, 160, 60, 0, (uint16_t) (0.60 * 65535));
    EVE_CoCmd_text(s_pHalContext, 200, 220, 26, 0, "60 %"); //text info
    EVE_CoCmd_fgColor(s_pHalContext, 0x808080);

#ifndef SAMAPP_DISPLAY_QVGA
    EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(51, 50, 52));
    EVE_CoCmd_dial(s_pHalContext, 360, 160, 80, 0, (uint16_t) (0.75 * 65535));
    EVE_CoCmd_text(s_pHalContext, 350, 240, 26, 0, "75 %"); //text info
#endif
    SAMAPP_INFO_END;
    SAMAPP_DELAY_NEXT;
}

/**
* @brief demonstrate toggle widget
*
*/
void SAMAPP_Widget_toggle()
{
    Draw_Text(s_pHalContext, "Example for: Toggle widget");

    /*************************************************************************/
    /* Below code demonstrates the usage of toggle function. Toggle function */
    /* draws line with inside knob to choose between left and right. Toggle  */
    /* inside color can be changed by bgcolor and knob color by fgcolor. Left*/
    /* right texts can be written and the color of the text can be changed by*/
    /* colorrgb graphics function                                            */
    /*************************************************************************/
    int16_t xOffset;
    int16_t yOffset;
    int16_t yDist = 40;

    SAMAPP_INFO_START;
    /* Draw toggle with flat effect */
    EVE_CoCmd_fgColor(s_pHalContext, 0xffff00);
    EVE_CoCmd_bgColor(s_pHalContext, 0x000080);

    EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0xff, 0xff, 0xff));

    EVE_CoCmd_toggle(s_pHalContext, 40, 10, 30, 27, OPT_FLAT, (uint16_t) (0.5 * 65535),
        "no\xffyes"); //circle radius will be extended on both the horizontal sides
    EVE_Cmd_wr32(s_pHalContext, COLOR_A(255));
    EVE_CoCmd_text(s_pHalContext, 40, 40, 26, 0, "Flat effect"); //text info
    /* Draw toggle bar with 3d effect */
    EVE_CoCmd_fgColor(s_pHalContext, 0x00ff00);
    EVE_CoCmd_bgColor(s_pHalContext, 0x800000);
    EVE_CoCmd_toggle(s_pHalContext, 140, 10, 30, 27, 0, 1 * 65535, "stop\xffrun");
    EVE_Cmd_wr32(s_pHalContext, COLOR_A(255));
    EVE_CoCmd_text(s_pHalContext, 140, 40, 26, 0, "3D effect"); //text info

    xOffset = 40;
    yOffset = 80;
    /* Draw horizontal toggle bars */
    EVE_CoCmd_bgColor(s_pHalContext, 0x800000);
    EVE_CoCmd_fgColor(s_pHalContext, 0x410105);
    EVE_CoCmd_toggle(s_pHalContext, xOffset, yOffset, 30, 27, 0, 0 * 65535, "-ve\xff+ve");
    EVE_CoCmd_fgColor(s_pHalContext, 0x0b0721);
    EVE_CoCmd_bgColor(s_pHalContext, 0x000080);
    yOffset += yDist;
    EVE_CoCmd_toggle(s_pHalContext, xOffset, yOffset, 30, 27, 0, (uint16_t) (0.25 * 65535),
        "zero\xffone");
    EVE_CoCmd_bgColor(s_pHalContext, 0xffff00);
    EVE_CoCmd_fgColor(s_pHalContext, 0x575e1b);
    EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0, 0, 0));
    yOffset += yDist;
    EVE_CoCmd_toggle(s_pHalContext, xOffset, yOffset, 30, 27, 0, (uint16_t) (0.5 * 65535),
        "exit\xffinit");
    EVE_CoCmd_bgColor(s_pHalContext, 0x808080);
    EVE_CoCmd_fgColor(s_pHalContext, 0x333234);
    EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0xff, 0xff, 0xff));
    yOffset += yDist;
    EVE_CoCmd_toggle(s_pHalContext, xOffset, yOffset, 30, 27, 0, (uint16_t) (0.75 * 65535),
        "off\xffon");
    SAMAPP_INFO_END;

    SAMAPP_DELAY_NEXT;
}

/**
* @brief demonstrate a spinner widget
*
*/
void SAMAPP_Widget_spinner()
{
    Draw_Text(s_pHalContext, "Example for: Spinner widget");

    /*************************************************************************/
    /* Below code demonstrates the usage of spinner function. Spinner func   */
    /* will wait untill stop command is sent from the mcu. Spinner has option*/
    /* for displaying points in circle fashion or in a line fashion.         */
    /*************************************************************************/
    EVE_CoCmd_dlStart(s_pHalContext);
    EVE_Cmd_wr32(s_pHalContext, CLEAR_COLOR_RGB(64, 64, 64));
    EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1));
    EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0xff, 0xff, 0xff));
    EVE_CoCmd_text(s_pHalContext, (int16_t) (s_pHalContext->Width / 2), 20, 27, OPT_CENTER,
        "Spinner circle");
    EVE_CoCmd_text(s_pHalContext, (int16_t) (s_pHalContext->Width / 2), 80, 27, OPT_CENTER,
        "Please Wait ...");
    EVE_CoCmd_spinner(s_pHalContext, (int16_t) (s_pHalContext->Width / 2),
        (int16_t) (s_pHalContext->Height / 2), 0, 1); //style 0 and scale 0

    /* Wait till coprocessor completes the operation */
    EVE_Cmd_waitFlush(s_pHalContext);

    EVE_sleep(1000);

    /* spinner with style 1 and scale 1 */
    EVE_CoCmd_dlStart(s_pHalContext);
    EVE_Cmd_wr32(s_pHalContext, CLEAR_COLOR_RGB(64, 64, 64));
    EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1));
    EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0xff, 0xff, 0xff));
    EVE_CoCmd_text(s_pHalContext, (int16_t) (s_pHalContext->Width / 2), 20, 27, OPT_CENTER,
        "Spinner line");
    EVE_CoCmd_text(s_pHalContext, (int16_t) (s_pHalContext->Width / 2), 80, 27, OPT_CENTER,
        "Please Wait ...");
    EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0x00, 0x00, 0x80));
    EVE_CoCmd_spinner(s_pHalContext, (int16_t) (s_pHalContext->Width / 2),
        (int16_t) (s_pHalContext->Height / 2), 1, 1); //style 1 and scale 1

    /* Wait till coprocessor completes the operation */
    EVE_Cmd_waitFlush(s_pHalContext);
    EVE_sleep(1000);

    EVE_CoCmd_dlStart(s_pHalContext);
    EVE_Cmd_wr32(s_pHalContext, CLEAR_COLOR_RGB(64, 64, 64));
    EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1));
    EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0xff, 0xff, 0xff));
    EVE_CoCmd_text(s_pHalContext, (int16_t) (s_pHalContext->Width / 2), 20, 27, OPT_CENTER,
        "Spinner clockhand");
    EVE_CoCmd_text(s_pHalContext, (int16_t) (s_pHalContext->Width / 2), 80, 27, OPT_CENTER,
        "Please Wait ...");
    EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0x80, 0x00, 0x00));
    EVE_CoCmd_spinner(s_pHalContext, (int16_t) (s_pHalContext->Width / 2),
        (int16_t) ((s_pHalContext->Height / 2) + 20), 2, 1); //style 2 scale 1

    /* Wait till coprocessor completes the operation */
    EVE_Cmd_waitFlush(s_pHalContext);
    EVE_sleep(1000);

    EVE_CoCmd_dlStart(s_pHalContext);
    EVE_Cmd_wr32(s_pHalContext, CLEAR_COLOR_RGB(64, 64, 64));
    EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1));
    EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0xff, 0xff, 0xff));
    EVE_CoCmd_text(s_pHalContext, (int16_t) (s_pHalContext->Width / 2), 20, 27, OPT_CENTER,
        "Spinner two dots");
    EVE_CoCmd_text(s_pHalContext, (int16_t) (s_pHalContext->Width / 2), 80, 27, OPT_CENTER,
        "Please Wait ...");
    EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0x80, 0x00, 0x00));
    EVE_CoCmd_spinner(s_pHalContext, (int16_t) (s_pHalContext->Width / 2),
        (int16_t) ((s_pHalContext->Height / 2) + 20), 3, 1); //style 3 scale 0

    /* Wait till coprocessor completes the operation */
    EVE_Cmd_waitFlush(s_pHalContext);
    EVE_sleep(1000);

    /* Send the stop command */
    EVE_Cmd_wr32(s_pHalContext, CMD_STOP);
    /* Update the command buffer pointers - both read and write pointers */
    EVE_Cmd_waitFlush(s_pHalContext);

    SAMAPP_DELAY_NEXT;
}

/**
* @brief API to demonstrate text widget
*
*/
void SAMAPP_Widget_text()
{
    Draw_Text(s_pHalContext, "Example for: text widget");

    /*************************************************************************/
    /* Below code demonstrates the usage of text function. Text function     */
    /* draws text with either in built or externally configured text. Text   */
    /* color can be changed by colorrgb and text function supports display of*/
    /* texts on left, right, top, bottom and center respectively             */
    /*************************************************************************/
    
    SAMAPP_INFO_START;
    /* Draw text at 0,0 location */
    EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0x00, 0x00, 0x80));
    EVE_CoCmd_text(s_pHalContext, 0, 0, 29, 0, "Bridgetek!");
    EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0xff, 0xff, 0xff));
    EVE_CoCmd_text(s_pHalContext, 0, 40, 26, 0, "Text29 at 0,0"); //text info
    /* text with centerx */
    EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0x80, 0x00, 0x00));
    EVE_CoCmd_text(s_pHalContext, (int16_t) (s_pHalContext->Width / 2), 50, 29, OPT_CENTERX,
        "Bridgetek!");
    EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0xff, 0xff, 0xff));
    EVE_CoCmd_text(s_pHalContext, (int16_t) ((s_pHalContext->Width / 2) - 30), 90, 26, 0,
        "Text29 centerX"); //text info
    /* text with centery */
    EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0x41, 0x01, 0x05));
    EVE_CoCmd_text(s_pHalContext, (int16_t) (s_pHalContext->Width / 5), 120, 29, OPT_CENTERY,
        "Bridgetek!");
    EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0xff, 0xff, 0xff));
    EVE_CoCmd_text(s_pHalContext, (int16_t) (s_pHalContext->Width / 5), 140, 26, 0,
        "Text29 centerY"); //text info
    /* text with center */
    EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0x0b, 0x07, 0x21));
    EVE_CoCmd_text(s_pHalContext, (int16_t) (s_pHalContext->Width / 2), 180, 29, OPT_CENTER,
        "Bridgetek!");
    EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0xff, 0xff, 0xff));
    EVE_CoCmd_text(s_pHalContext, (int16_t) ((s_pHalContext->Width / 2) - 50), 200, 26, 0,
        "Text29 center"); //text info
    /* text with rightx */
    EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0x57, 0x5e, 0x1b));
    EVE_CoCmd_text(s_pHalContext, (int16_t) s_pHalContext->Width, 60, 29, OPT_RIGHTX, "Bridgetek!");
    EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0xff, 0xff, 0xff));
    EVE_CoCmd_text(s_pHalContext, (int16_t) (s_pHalContext->Width - 90), 100, 26, 0,
        "Text29 rightX"); //text info
    SAMAPP_INFO_END;
    SAMAPP_DELAY_NEXT;
}

/**
* @brief API to demonstrate number widget
*
*/
void SAMAPP_Widget_number()
{
    Draw_Text(s_pHalContext, "Example for: number widget");

    /*************************************************************************/
    /* Below code demonstrates the usage of number function. Number function */
    /* draws text with only 32bit decimal number, signed or unsigned can also*/
    /* be specified as input parameter. Options like centerx, centery, center*/
    /* and rightx are supported                                              */
    /*************************************************************************/
    SAMAPP_INFO_START;
    /* Draw number at 0,0 location */
    EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0x00, 0x00, 0x80));
    EVE_CoCmd_number(s_pHalContext, 0, 0, 29, 0, 1234);
    EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0xff, 0xff, 0xff));
    EVE_CoCmd_text(s_pHalContext, 0, 40, 26, 0, "Number29 at 0,0"); //text info
    /* number with centerx */
    EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0x80, 0x00, 0x00));
    EVE_CoCmd_number(s_pHalContext, (int16_t) (s_pHalContext->Width / 2), 50, 29,
        OPT_CENTERX | OPT_SIGNED, -1234);
    EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0xff, 0xff, 0xff));
    EVE_CoCmd_text(s_pHalContext, (int16_t) ((s_pHalContext->Width / 2) - 30), 90, 26, 0,
        "Number29 centerX"); //text info
    /* number with centery */
    EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0x41, 0x01, 0x05));
    EVE_CoCmd_number(s_pHalContext, (int16_t) (s_pHalContext->Width / 5), 120, 29, OPT_CENTERY,
        1234);
    EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0xff, 0xff, 0xff));
    EVE_CoCmd_text(s_pHalContext, (int16_t) (s_pHalContext->Width / 5), 140, 26, 0,
        "Number29 centerY"); //text info
    /* number with center */
    EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0x0b, 0x07, 0x21));
    EVE_CoCmd_number(s_pHalContext, (int16_t) (s_pHalContext->Width / 2), 180, 29,
        OPT_CENTER | OPT_SIGNED, -1234);
    EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0xff, 0xff, 0xff));
    EVE_CoCmd_text(s_pHalContext, (int16_t) ((s_pHalContext->Width / 2) - 50), 200, 26, 0,
        "Number29 center"); //text info
    /* number with rightx */
    EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0x57, 0x5e, 0x1b));
    EVE_CoCmd_number(s_pHalContext, (int16_t) s_pHalContext->Width, 60, 29, OPT_RIGHTX, 1234);
    EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0xff, 0xff, 0xff));
    EVE_CoCmd_text(s_pHalContext, (int16_t) (s_pHalContext->Width - 100), 100, 26, 0,
        "Number29 rightX"); //text info
    SAMAPP_INFO_END;
    SAMAPP_DELAY_NEXT;
}

/**
* @brief API to demonstrate button functionality
*
*/
void SAMAPP_Widget_button()
{
    Draw_Text(s_pHalContext, "Example for: button functionality");

    /*************************************************************************/
    /* Below code demonstrates the usage of button function. Buttons can be  */
    /* constructed using flat or 3d effect. Button color can be changed by   */
    /* fgcolor command and text color is set by COLOR_RGB graphics command   */
    /*************************************************************************/
    int16_t xOffset;
    int16_t yOffset;
    int16_t bWidth;
    int16_t bHeight;
    int16_t bDistx;
    int16_t bDisty;

    bWidth = 60;
    bHeight = 30;
    bDistx = bWidth + (int16_t) ((s_pHalContext->Width - 4 * bWidth) / 5);
    bDisty = bHeight + 5;
    xOffset = 10;
    /*  Construct a buttons with "ONE/TWO/THREE" text and default background */
    /* Draw buttons 60x30 resolution at 10x40,10x75,10x110 */
    SAMAPP_INFO_START;
    /* flat effect and default color background */
    EVE_CoCmd_fgColor(s_pHalContext, 0x0000ff);
    yOffset = (int16_t) (s_pHalContext->Height / 2 - 2 * bDisty);
    EVE_CoCmd_button(s_pHalContext, xOffset, yOffset, bWidth, bHeight, 28, OPT_FLAT, "ABC");
    yOffset += bDisty;
    EVE_CoCmd_button(s_pHalContext, xOffset, yOffset, bWidth, bHeight, 28, OPT_FLAT, "ABC");
    yOffset += bDisty;
    EVE_CoCmd_button(s_pHalContext, xOffset, yOffset, bWidth, bHeight, 28, OPT_FLAT, "ABC");
    EVE_CoCmd_text(s_pHalContext, xOffset, (yOffset + 40), 26, 0, "Flat effect"); //text info
    /* 3D effect */
    xOffset += bDistx;
    yOffset = (int16_t) (s_pHalContext->Height / 2 - 2 * bDisty);
    EVE_CoCmd_button(s_pHalContext, xOffset, yOffset, bWidth, bHeight, 28, 0, "ABC");
    yOffset += bDisty;
    EVE_CoCmd_button(s_pHalContext, xOffset, yOffset, bWidth, bHeight, 28, 0, "ABC");
    yOffset += bDisty;
    EVE_CoCmd_button(s_pHalContext, xOffset, yOffset, bWidth, bHeight, 28, 0, "ABC");
    EVE_CoCmd_text(s_pHalContext, xOffset, (yOffset + 40), 26, 0, "3D Effect"); //text info
    /* 3d effect with background color */
    EVE_CoCmd_fgColor(s_pHalContext, 0xffff00);
    xOffset += bDistx;
    yOffset = (int16_t) (s_pHalContext->Height / 2 - 2 * bDisty);
    EVE_CoCmd_button(s_pHalContext, xOffset, yOffset, bWidth, bHeight, 28, 0, "ABC");
    yOffset += bDisty;
    EVE_CoCmd_fgColor(s_pHalContext, 0x00ffff);
    EVE_CoCmd_button(s_pHalContext, xOffset, yOffset, bWidth, bHeight, 28, 0, "ABC");
    yOffset += bDisty;
    EVE_CoCmd_fgColor(s_pHalContext, 0xff00ff);
    EVE_CoCmd_button(s_pHalContext, xOffset, yOffset, bWidth, bHeight, 28, 0, "ABC");
    EVE_CoCmd_text(s_pHalContext, xOffset, (yOffset + 40), 26, 0, "3D Color"); //text info
    /* 3d effect with gradient color */
    EVE_CoCmd_fgColor(s_pHalContext, 0x101010);
    EVE_CoCmd_gradColor(s_pHalContext, 0xff0000);
    xOffset += bDistx;
    yOffset = (int16_t) (s_pHalContext->Height / 2 - 2 * bDisty);
    EVE_CoCmd_button(s_pHalContext, xOffset, yOffset, bWidth, bHeight, 28, 0, "ABC");
    yOffset += bDisty;
    EVE_CoCmd_gradColor(s_pHalContext, 0x00ff00);
    EVE_CoCmd_button(s_pHalContext, xOffset, yOffset, bWidth, bHeight, 28, 0, "ABC");
    yOffset += bDisty;
    EVE_CoCmd_gradColor(s_pHalContext, 0x0000ff);
    EVE_CoCmd_button(s_pHalContext, xOffset, yOffset, bWidth, bHeight, 28, 0, "ABC");
    EVE_CoCmd_text(s_pHalContext, xOffset, (yOffset + 40), 26, 0, "3D Gradient"); //text info
    SAMAPP_INFO_END;
    SAMAPP_DELAY_NEXT;
}

/**
* @brief API to demonstrate screen saver widget - playing of bitmap via macro0
*
*/
void SAMAPP_Widget_screensaver()
{
    Draw_Text(s_pHalContext, "Example for: screen saver with an image");

    /*************************************************************************/
    /* Below code demonstrates the usage of screensaver function. Screen     */
    /* saver modifies macro0 with the vertex2f command.                      */
    /* MCU can display any static display list at the background with the    */
    /* changing bitmap                                                       */
    /*************************************************************************/
    /* Download the bitmap data */
    Ftf_Write_File_nBytes_To_RAM_G(s_pHalContext, TEST_DIR "\\SAMAPP_Bitmap_RawData.bin", RAM_G,
        SAMAPP_Bitmap_RawData_Header[0].Stride * SAMAPP_Bitmap_RawData_Header[0].Height,
        SAMAPP_Bitmap_RawData_Header[0].Arrayoffset);

    /* Send command screen saver */
    EVE_Cmd_wr32(s_pHalContext, CMD_SCREENSAVER); //screen saver command will continuously update the macro0 with vertex2f command
    /* Copy the display list */
    SAMAPP_INFO_START;
    /* lena bitmap */
    EVE_CoCmd_loadIdentity(s_pHalContext);
    EVE_CoCmd_scale(s_pHalContext, 3 * 65536, 3 * 65536); //scale the bitmap 3 times on both sides
    EVE_CoCmd_setMatrix(s_pHalContext);
    EVE_Cmd_wr32(s_pHalContext, BEGIN(BITMAPS));
    EVE_Cmd_wr32(s_pHalContext, BITMAP_SOURCE(0));
    EVE_Cmd_wr32(s_pHalContext,
        BITMAP_LAYOUT(SAMAPP_Bitmap_RawData_Header[0].Format,
            SAMAPP_Bitmap_RawData_Header[0].Stride, SAMAPP_Bitmap_RawData_Header[0].Height));
    EVE_Cmd_wr32(s_pHalContext,
        BITMAP_SIZE(BILINEAR, BORDER, BORDER, SAMAPP_Bitmap_RawData_Header[0].Width * 3,
            SAMAPP_Bitmap_RawData_Header[0].Height * 3));
    EVE_Cmd_wr32(s_pHalContext, MACRO(0));
    EVE_Cmd_wr32(s_pHalContext, END());
    /* Display the text */
    EVE_CoCmd_loadIdentity(s_pHalContext);
    EVE_CoCmd_setMatrix(s_pHalContext);
    EVE_CoCmd_text(s_pHalContext, (int16_t) (s_pHalContext->Width / 2),
        (int16_t) (s_pHalContext->Height / 2), 27, OPT_CENTER, "Screen Saver ...");
    EVE_CoCmd_memSet(s_pHalContext, (RAM_G + 3200), 0xff, (160L * 2 * 120));
    SAMAPP_INFO_END;
    EVE_sleep(3000);

    /* Send the stop command */
    EVE_Cmd_wr32(s_pHalContext, CMD_STOP);
    /* Update the command buffer pointers - both read and write pointers */
    EVE_Cmd_waitFlush(s_pHalContext);
}

/**
* @brief API to demonstrate sketch widget
*
*/
void SAMAPP_Widget_sketch()
{
    Draw_Text(s_pHalContext, "Example for: sketch widget");

    /*************************************************************************/
    /* Below code demonstrates the usage of sketch function. Sketch function */
    /* draws line for pen movement. Skecth supports L1 and L8 output formats */
    /* Sketch draws till stop command is executed.                           */
    /*************************************************************************/
    int16_t BorderSz = 40;
    uint32_t MemZeroSz;
    /* Send command sketch */

    MemZeroSz = 1L * (s_pHalContext->Width - 2 * BorderSz) * (s_pHalContext->Height - 2 * BorderSz);
    EVE_CoCmd_memZero(s_pHalContext, RAM_G, MemZeroSz);
#ifdef FT801_ENABLE
    EVE_CoCmd_cSketch(s_pHalContext, BorderSz, BorderSz, 
        (int16_t)(s_pHalContext->Width - 2 * BorderSz), 
        (int16_t)(s_pHalContext->Height - 2 * BorderSz), 0, L1, 1500L); //sketch in L1 format
#else
    EVE_CoCmd_sketch(s_pHalContext, BorderSz, BorderSz,
        (int16_t) (s_pHalContext->Width - 2 * BorderSz),
        (int16_t) (s_pHalContext->Height - 2 * BorderSz), RAM_G, L1); //sketch in L1 format
#endif
    /* Display the sketch */
    SAMAPP_INFO_START;
    EVE_Cmd_wr32(s_pHalContext,
        SCISSOR_SIZE((s_pHalContext->Width - 2 * BorderSz),
            (s_pHalContext->Height - 2 * BorderSz)));
    EVE_Cmd_wr32(s_pHalContext, SCISSOR_XY(BorderSz, BorderSz));
    EVE_Cmd_wr32(s_pHalContext, CLEAR_COLOR_RGB(255, 255, 255));
    EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1));

    /* default the scissor size */
#ifdef FT81X_ENABLE
    EVE_Cmd_wr32(s_pHalContext, SCISSOR_SIZE(2048, 2048));
#else
    EVE_Cmd_wr32(s_pHalContext, SCISSOR_SIZE(512, 512));
#endif

    EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0, 0, 0));
    EVE_Cmd_wr32(s_pHalContext, SCISSOR_XY(0, 0));
    /* L1 bitmap display */
    EVE_Cmd_wr32(s_pHalContext, BEGIN(BITMAPS));
    EVE_Cmd_wr32(s_pHalContext, BITMAP_SOURCE(0));
#ifdef FT81X_ENABLE

    EVE_Cmd_wr32(s_pHalContext,
        BITMAP_LAYOUT(L1, (s_pHalContext->Width - 2 * BorderSz) / 8,
            (s_pHalContext->Height - 2 * BorderSz)));
    EVE_Cmd_wr32(s_pHalContext,
        BITMAP_LAYOUT_H((((s_pHalContext->Width - 2 * BorderSz) / 8) >> 10),
            ((s_pHalContext->Height - 2 * BorderSz) >> 9)));

    EVE_Cmd_wr32(s_pHalContext,
        BITMAP_SIZE(BILINEAR, BORDER, BORDER, (s_pHalContext->Width - 2 * BorderSz),
            (s_pHalContext->Height - 2 * BorderSz)));
    EVE_Cmd_wr32(s_pHalContext,
        BITMAP_SIZE_H(((s_pHalContext->Width - 2 * BorderSz) >> 9),
            ((s_pHalContext->Height - 2 * BorderSz) >> 9)));

#else
    EVE_Cmd_wr32(s_pHalContext, BITMAP_LAYOUT(L1, 
        (s_pHalContext->Width - 2 * BorderSz) / 8, 
        (s_pHalContext->Height - 2 * BorderSz)));
    EVE_Cmd_wr32(s_pHalContext, BITMAP_SIZE(BILINEAR, BORDER, BORDER, 
        (s_pHalContext->Width - 2 * BorderSz), (s_pHalContext->Height - 2 * BorderSz)));
#endif
    EVE_Cmd_wr32(s_pHalContext, VERTEX2F(BorderSz * 16, BorderSz * 16));
    EVE_Cmd_wr32(s_pHalContext, END());
    /* Display the text */
    EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0xff, 0xff, 0xff));
    EVE_CoCmd_text(s_pHalContext, (int16_t) (s_pHalContext->Width / 2), 20, 27, OPT_CENTER,
        "Sketch L1");
    SAMAPP_INFO_END;

    EVE_sleep(10 * 1000); //sleep for 10 seconds

    /* Send the stop command */
    EVE_Cmd_wr32(s_pHalContext, CMD_STOP);
    /* Update the command buffer pointers - both read and write pointers */
    EVE_Cmd_waitFlush(s_pHalContext);

    /* Sketch L8 format */
    /* Send command sketch */
    EVE_CoCmd_memZero(s_pHalContext, RAM_G, MemZeroSz);
#ifdef FT801_ENABLE
    EVE_CoCmd_cSketch(s_pHalContext, BorderSz, BorderSz, 
        (int16_t)(s_pHalContext->Width - 2 * BorderSz), 
        (int16_t)(s_pHalContext->Height - 2 * BorderSz), 0, L8, 1500L); //sketch in L8 format
#else
    EVE_CoCmd_sketch(s_pHalContext, BorderSz, BorderSz,
        (int16_t) (s_pHalContext->Width - 2 * BorderSz),
        (int16_t) (s_pHalContext->Height - 2 * BorderSz), 0, L8); //sketch in L8 format
#endif
    /* Display the sketch */
    SAMAPP_INFO_START;
    EVE_Cmd_wr32(s_pHalContext,
        SCISSOR_SIZE((s_pHalContext->Width - 2 * BorderSz),
            (s_pHalContext->Height - 2 * BorderSz)));
    EVE_Cmd_wr32(s_pHalContext, SCISSOR_XY(BorderSz, BorderSz));
    EVE_Cmd_wr32(s_pHalContext, CLEAR_COLOR_RGB(0xff, 0xff, 0xff));
    EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1));

#ifdef FT81X_ENABLE
    EVE_Cmd_wr32(s_pHalContext, SCISSOR_SIZE(2048, 2048));
#else
    EVE_Cmd_wr32(s_pHalContext, SCISSOR_SIZE(512, 512));
#endif
    EVE_Cmd_wr32(s_pHalContext, SCISSOR_XY(0, 0));
    EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0, 0, 0));
    /* L8 bitmap display */
    EVE_Cmd_wr32(s_pHalContext, BEGIN(BITMAPS));
    EVE_Cmd_wr32(s_pHalContext, BITMAP_SOURCE(0));
#ifdef FT81X_ENABLE

    EVE_Cmd_wr32(s_pHalContext,
        BITMAP_LAYOUT(L8, (s_pHalContext->Width - 2 * BorderSz),
            (s_pHalContext->Height - 2 * BorderSz)));
    EVE_Cmd_wr32(s_pHalContext,
        BITMAP_LAYOUT_H((((s_pHalContext->Width - 2 * BorderSz)) >> 10),
            ((s_pHalContext->Height - 2 * BorderSz) >> 9)));

    EVE_Cmd_wr32(s_pHalContext,
        BITMAP_SIZE(BILINEAR, BORDER, BORDER, (s_pHalContext->Width - 2 * BorderSz),
            (s_pHalContext->Height - 2 * BorderSz)));
    EVE_Cmd_wr32(s_pHalContext,
        BITMAP_SIZE_H(((s_pHalContext->Width - 2 * BorderSz) >> 9),
            ((s_pHalContext->Height - 2 * BorderSz) >> 9)));

#else
    EVE_Cmd_wr32(s_pHalContext, BITMAP_LAYOUT(L8, 
        (s_pHalContext->Width - 2 * BorderSz), (s_pHalContext->Height - 2 * BorderSz)));
    EVE_Cmd_wr32(s_pHalContext, BITMAP_SIZE(BILINEAR, BORDER, BORDER, 
        (s_pHalContext->Width - 2 * BorderSz), (s_pHalContext->Height - 2 * BorderSz)));
#endif
    EVE_Cmd_wr32(s_pHalContext, VERTEX2F(BorderSz * 16, BorderSz * 16));
    EVE_Cmd_wr32(s_pHalContext, END());
    /* Display the text */
    EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0xff, 0xff, 0xff));
    EVE_CoCmd_text(s_pHalContext, (int16_t) (s_pHalContext->Width / 2), 20, 27, OPT_CENTER,
        "Sketch L8");
    SAMAPP_INFO_END;
    EVE_sleep(3 * 1000); //sleep for 3 seconds

    /* Send the stop command */
    EVE_Cmd_wr32(s_pHalContext, CMD_STOP);
    /* Update the command buffer pointers - both read and write pointers */
    EVE_Cmd_waitFlush(s_pHalContext);
}

void SAMAPP_Widget() {
    SAMAPP_Widget_logo();
    SAMAPP_Widget_clock();
    SAMAPP_Widget_gauge();
    SAMAPP_Widget_gradient();
    SAMAPP_Widget_keysInteractive();
    SAMAPP_Widget_keys();
    SAMAPP_Widget_progressbar();
    SAMAPP_Widget_scroll();
    SAMAPP_Widget_slider();
    SAMAPP_Widget_dial();
    SAMAPP_Widget_toggle();
    SAMAPP_Widget_spinner();
    SAMAPP_Widget_text();
    SAMAPP_Widget_number();
    SAMAPP_Widget_button();
    SAMAPP_Widget_screensaver();
    SAMAPP_Widget_sketch();
}


