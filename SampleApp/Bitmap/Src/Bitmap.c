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
#include "Bitmap.h"

// angle and circle                         
#define MAX_ANGLE        360
#define MAX_CIRCLE_UNIT  65536
#define SAMAPP_DELAY     EVE_sleep(2000)

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

static EVE_HalContext s_halContext;
static EVE_HalContext* s_pHalContext;
void SAMAPP_Bitmap();

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
        "This sample demonstrate the using of bitmap", 
        "",
        ""
    }; 

    while (TRUE) {
        WelcomeScreen(s_pHalContext, info);

        SAMAPP_Bitmap();

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
* @brief Draw ASTC image on screen
*
* @param str Image label
* @param x Image X
* @param y Image Y
* @param w Image W
* @param h Image H
* @param margin Image margin
* @param source Image source
* @param numcell Cell number
*/
void helperDrawASTC(const char* title, uint16_t source, uint16_t fmt, uint16_t x, uint16_t y, 
    uint16_t w, uint16_t h, uint16_t margin, uint16_t numcell)
{
#if defined(BT81X_ENABLE) // BT81X
    int m1 = 30;

    if (title != 0 ) {
        EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0, 0, 0));
        EVE_CoCmd_text(s_pHalContext, x, y, 16, 0, title);
        EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0xFF, 0xFF, 0xFF));
    }
    EVE_CoCmd_setBitmap(s_pHalContext, source, fmt, w, h);
    EVE_Cmd_wr32(s_pHalContext, BEGIN(BITMAPS));
    for (int i=0; i< numcell; i++)
    {
        EVE_Cmd_wr32(s_pHalContext, CELL(i));
        EVE_Cmd_wr32(s_pHalContext, VERTEX2F((x + m1 + margin*i) * 16, y * 16));
    }
#endif // BT81X
}

/**
* @brief Rotate image by CMD_ROTATEAROUND
*
* @param address Address of image
* @param format Image format
* @param x Image X
* @param y Image Y
* @param w Image W
* @param h Image H
* @param rotation_angle Rotate angle
*/
void helperRotateAroundOne(uint32_t address, uint32_t format, uint32_t x, uint32_t y,
    uint32_t w, uint32_t h, uint32_t rotation_angle)
{
#if defined (BT81X_ENABLE) && (defined(MSVC_PLATFORM) || defined(BT8XXEMU_PLATFORM)) // Win32 BT81X only
    int16_t lw;
    int16_t lh;
    const uint32_t TRANSLATE_XY = 100;
    const uint32_t precision = 16;

    EVE_Cmd_wr32(s_pHalContext, SAVE_CONTEXT());

    EVE_Cmd_wr32(s_pHalContext, BEGIN(BITMAPS));
    EVE_CoCmd_setBitmap(s_pHalContext, (int16_t) address, (int16_t) format, (int16_t) w,
        (int16_t) h);

    lw = (int16_t) (w + TRANSLATE_XY);
    lh = (int16_t) (h + TRANSLATE_XY);

    EVE_Cmd_wr32(s_pHalContext, BITMAP_SIZE(NEAREST, BORDER, BORDER, lw, lh));
    EVE_Cmd_wr32(s_pHalContext, BITMAP_SIZE_H(lw >> 8, lh >> 8));

    EVE_CoCmd_loadIdentity(s_pHalContext);
    EVE_CoCmd_translate(s_pHalContext, TRANSLATE_XY * MAX_CIRCLE_UNIT,
        TRANSLATE_XY * MAX_CIRCLE_UNIT);
    EVE_CoCmd_rotateAround(s_pHalContext, w / 2, h / 2,
        rotation_angle * MAX_CIRCLE_UNIT / MAX_ANGLE, MAX_CIRCLE_UNIT);
    EVE_CoCmd_setMatrix(s_pHalContext);

    EVE_Cmd_wr32(s_pHalContext, VERTEX2F(x * precision, y * precision));

    EVE_Cmd_wr32(s_pHalContext, END());

    EVE_Cmd_wr32(s_pHalContext, RESTORE_CONTEXT());
#endif // Win32 BT81X only
}

/**
* @brief Rotate image by CMD_ROTATEA and CMD_TRANSLATE
*
* @param address Address of image
* @param format Image format
* @param x Image X
* @param y Image Y
* @param w Image W
* @param h Image H
* @param rotation_angle Rotate angle
*/
void helperRotateAndTranslateOne(uint32_t address, uint32_t format, uint32_t x, uint32_t y,
    uint32_t w, uint32_t h, uint32_t rotation_angle)
{
#if defined (BT81X_ENABLE) && (defined(MSVC_PLATFORM) || defined(BT8XXEMU_PLATFORM)) // Win32 BT81X only
    int16_t lw;
    int16_t lh;
    const uint32_t TRANSLATE_XY = 100;
    const uint32_t precision = 16;

    EVE_Cmd_wr32(s_pHalContext, SAVE_CONTEXT());

    EVE_Cmd_wr32(s_pHalContext, BEGIN(BITMAPS));
    EVE_CoCmd_setBitmap(s_pHalContext, (int16_t) address, (int16_t) format, (int16_t) w,
        (int16_t) h);

    lw = (int16_t) (w * 2);
    lh = (int16_t) (h * 2);

    EVE_Cmd_wr32(s_pHalContext, BITMAP_SIZE(NEAREST, BORDER, BORDER, lw, lh));
    EVE_Cmd_wr32(s_pHalContext, BITMAP_SIZE_H(lw >> 8, lh >> 8));

    EVE_CoCmd_loadIdentity(s_pHalContext);

    EVE_CoCmd_translate(s_pHalContext, (w / 2 + TRANSLATE_XY) * MAX_CIRCLE_UNIT,
        (w / 2 + TRANSLATE_XY) * MAX_CIRCLE_UNIT);
    EVE_CoCmd_rotate(s_pHalContext, rotation_angle * MAX_CIRCLE_UNIT / MAX_ANGLE);
    EVE_CoCmd_translate(s_pHalContext, -((int32_t) (w / 2)) * MAX_CIRCLE_UNIT,
        -((int32_t) (h / 2)) * MAX_CIRCLE_UNIT);

    EVE_CoCmd_setMatrix(s_pHalContext);

    EVE_Cmd_wr32(s_pHalContext, VERTEX2F(x * precision, y * precision));

    EVE_Cmd_wr32(s_pHalContext, END());

    EVE_Cmd_wr32(s_pHalContext, RESTORE_CONTEXT());
#endif // Win32 BT81X only
}

/**
* @brief API to demonstrate load a raw file to Coprocessor
*
* @param fileName File to load
* @param ramOffset Offset on RAM_G
* @return int32_t Always 0
*/
int32_t helperLoadRawFromFile(const char8_t* fileName, int32_t ramOffset)
{
#if defined(FT81X_ENABLE) && (defined(MSVC_PLATFORM) || defined(BT8XXEMU_PLATFORM)) // Win32 FT81X only
    int32_t FileLen = 0;
    int32_t FileSz = 0;
    uint8_t* pbuff = NULL;
    FILE* file = fopen(fileName, "rb");
    if (NULL == file)
    {
        printf("Error while opening file.\n");
        return 0;
    }
    else
    {
        fseek(file, 0, SEEK_END);
        FileSz = FileLen = ftell(file);
        fseek(file, 0, SEEK_SET);
        pbuff = (uint8_t*) malloc(8192);
        while (FileLen > 0)
        {
            /* download the data into the command buffer by 2kb one shot */
            uint16_t blocklen = FileLen > 8192 ? 8192 : (uint16_t) FileLen;

            /* copy the data into pbuff and then transfter it to command buffer */
            fread(pbuff, 1, blocklen, file);
            FileLen -= blocklen;
            /* copy data continuously into command memory */
            EVE_Hal_wrMem(s_pHalContext, ramOffset, pbuff, blocklen);
            ramOffset += blocklen;
        }
        fclose(file);
        free(pbuff);
    }
    return FileSz;
#endif
}

/**
* @brief Load image to RAM_G
*
*/
static void helperloadDefaultImage() {
    uint32_t fileSize = 0;
    const uint32_t bytePerTrans = 1000;
    uint8_t buff[1000];

    const uint8_t* image = TEST_DIR "\\mandrill256.jpg";
    fileSize = FileIO_File_Open(image, FILEIO_E_FOPEN_READ);

    /* Decode jpg output into location 0 and output color format as RGB565 */
    EVE_Cmd_wr32(s_pHalContext, CMD_LOADIMAGE);
    EVE_Cmd_wr32(s_pHalContext, 0);//destination address of jpg decode
    EVE_Cmd_wr32(s_pHalContext, 0);//output format of the bitmap

    while (fileSize > 0) {
        /* copy the data into pbuff and then transfter it to command buffer */
        int bytes = FileIO_File_Read(buff, bytePerTrans);
        EVE_Cmd_wrMem(s_pHalContext, buff, bytes);
        fileSize -= bytes;
    }
    /* close the opened jpg file */
    FileIO_File_Close();
}

/**
* @brief API to demonstrate CMD_GETIMAGE
*
*/
void SAMAPP_Bitmap_getImage()
{
#if EVE_SUPPORT_GEN == EVE4
    uint32_t source;
    uint32_t fmt;
    uint32_t w;
    uint32_t h;
    uint32_t palette;
    source = 0;
    fmt = 4;
    w = 8;
    h = 12;
    palette = 16;

    Draw_Text(s_pHalContext, "Example for: CMD_GETIMAGE");

    // Now try to update the allocation pointer
    uint16_t format = RGB565;
    uint8_t type = LOADIMAGE;
    EVE_CoCmd_dlStart(s_pHalContext);
    EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1));
    EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 255, 255));

    Gpu_Hal_LoadImageToMemory(s_pHalContext, TEST_DIR "/CR_003_dithering.png", RAM_G, type);
    EVE_Cmd_waitFlush(s_pHalContext);
    EVE_CoCmd_setBitmap(s_pHalContext, RAM_G, format, 800, 600);

    //Start drawing bitmap
    EVE_Cmd_wr32(s_pHalContext, BEGIN(BITMAPS));
    EVE_Cmd_wr32(s_pHalContext, VERTEX2II(0, 0, 0, 0));
    EVE_Cmd_wr32(s_pHalContext, END());
    EVE_Cmd_wr32(s_pHalContext, RESTORE_CONTEXT());
    EVE_Cmd_wr32(s_pHalContext, DISPLAY());
    EVE_CoCmd_swap(s_pHalContext);
    EVE_Cmd_waitFlush(s_pHalContext);

    EVE_sleep(500);

    // Now getImage properties
    EVE_CoCmd_getImage(s_pHalContext, &source, &fmt, &w, &h, &palette);

    printf("Loaded image: \nsource: %u, format: %u, width: %u, height: %u, palette: %u", 
        source, fmt,
        w, h, palette);
#endif // EVE_SUPPORT_GEN == EVE4
}

/**
* @brief API to demonstrate for Non Square pixel
*
*/
void SAMAPP_Bitmap_nonSquareDisplay()
{
#if EVE_SUPPORT_GEN == EVE4
#ifdef DISPLAY_RESOLUTION_1280x800
#define LCD_W 218
#define LCD_H 136
#define EXPECT_W 1280
#define EXPECT_H 800
#else
#define LCD_W 153
#define LCD_H 86
#define EXPECT_W 800
#define EXPECT_H 480
#endif

    const uint32_t precision = 16;

    EVE_Util_clearScreen(s_pHalContext);

    // Non-square pixel panel support in EVE4
    // EVE_Hal_wr8(s_pHalContext, REG_PCLK, 1);
    Ft_Gpu_HorizontalScanoutFilter(s_pHalContext, LCD_W, LCD_H);

    Draw_Text_Format(s_pHalContext, "Example for non square pixel LCD: Draw Bitmap");
    Draw_Image(s_pHalContext, TEST_DIR "/TC_NF_24_001_862x480_862x480_RGB565_Converted.png",
        RGB565);
    EVE_sleep(2000);

    Draw_Text_Format(s_pHalContext, "Example for non square pixel LCD: Draw primitive");
    Display_Start(s_pHalContext);
    EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0, 128, 0));
    Draw_Point(s_pHalContext, s_pHalContext->Height / 2, s_pHalContext->Height / 2,
        s_pHalContext->Height / 2);

    EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 128, 0));
    EVE_Cmd_wr32(s_pHalContext, BEGIN(LINES));
    EVE_Cmd_wr32(s_pHalContext, VERTEX2F(0, s_pHalContext->Height / 2 * precision));
    EVE_Cmd_wr32(s_pHalContext,
        VERTEX2F(s_pHalContext->Height * precision, s_pHalContext->Height / 2 * precision));
    EVE_CoCmd_text(s_pHalContext, 10, (uint16_t) (s_pHalContext->Height / 2 + 10), 30, 0, "line x");

    EVE_Cmd_wr32(s_pHalContext, BEGIN(LINES));
    EVE_Cmd_wr32(s_pHalContext, VERTEX2F(s_pHalContext->Height / 2 * precision, 0));
    EVE_Cmd_wr32(s_pHalContext,
        VERTEX2F(s_pHalContext->Height / 2 * precision, s_pHalContext->Height * precision));
    EVE_CoCmd_text(s_pHalContext, (uint16_t) (s_pHalContext->Height / 2 + 10), 10, 30, 0, "line y");
    EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 255, 255));
    Display_End(s_pHalContext);
    EVE_sleep(2 * 1000);

    // Non-square pixel panel support in BT815A
    EVE_Hal_wr8(s_pHalContext, REG_PCLK, 1); /* after this display is visible on the LCD */

    // Now draw a circle
    Display_Start(s_pHalContext);
    EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0, 128, 0));

    uint32_t r[] = { s_pHalContext->Height * 4 / 13, s_pHalContext->Height * 2 / 13, s_pHalContext
        ->Height * 4 / 13, s_pHalContext->Height * 2 / 18, 0 };
    uint32_t y[5];
    y[0] = s_pHalContext->Height - r[0] - 10;
    y[1] = y[0] - r[1] - 10;
    y[2] = s_pHalContext->Height - r[2];
    y[3] = s_pHalContext->Height - r[2] - 10;

    uint32_t x = r[0];
    uint32_t x1;
    uint32_t y1;
    uint32_t y2;
    uint32_t distance = 0;
    for (int i = 0; i < 4; i++)
    {
        y2 = y[i];
        distance += r[i];

        if (i > 0)
            x = Math_Points_Nearby_NextX(x1, y1, y2, distance);
        Draw_Point(s_pHalContext, x, y[i], r[i]);
        x1 = x;
        y1 = y[i];
        distance = r[i];
    }
    uint32_t r5 = s_pHalContext->Height * 2 / 11;
    Draw_Point(s_pHalContext, s_pHalContext->Width - r5 - 10, r5 + 10, r5);
    Display_End(s_pHalContext);

    SAMAPP_DELAY;
#endif // EVE_SUPPORT_GEN == EVE4
}

/**
* @brief API to demonstrate CMD_LOADIMAGE
* For PNG decoder, the dithering is enabled
*
*/
void SAMAPP_Bitmap_dithering()
{
#if EVE_SUPPORT_GEN == EVE4
    uint16_t format = RGB565;
    uint16_t w = 800;
    uint16_t h = 480;
    uint16_t otp = OPT_NODL;
    uint16_t address = RAM_G;

    EVE_Util_clearScreen(s_pHalContext);


    for (int i = 0; i < 2; i++)
    {
        if (i == 0)
        {
            Draw_Text(s_pHalContext, "Example for PNG dithering: Dithering disable");
        }
        else
        {
            Draw_Text(s_pHalContext, "Example for PNG dithering: Dithering enable");

            /// Now enable dithering support
            otp |= OPT_DITHER;
        }

        EVE_Util_loadImageFile(s_pHalContext, address, TEST_DIR "\\loadimage-dither-testcase.png", 0);
        EVE_Cmd_waitFlush(s_pHalContext);

        //Start drawing bitmap
        Display_Start(s_pHalContext);
        EVE_CoCmd_setBitmap(s_pHalContext, address, format, w, h);
        EVE_Cmd_wr32(s_pHalContext, BEGIN(BITMAPS));
        EVE_Cmd_wr32(s_pHalContext, VERTEX2II(0, 0, 0, 0));
        Display_End(s_pHalContext);

        SAMAPP_DELAY;
    }
#endif // EVE_SUPPORT_GEN == EVE4
}

/**
* @brief API to demonstrate ASTC image on flash
*
*/
void SAMAPP_Bitmap_ASTC()
{
#if defined (BT81X_ENABLE) && (defined(MSVC_PLATFORM) || defined(BT8XXEMU_PLATFORM))
    Draw_Text(s_pHalContext, "Example for: ASTC bitmap");
    EVE_Cmd_waitFlush(s_pHalContext);

    EVE_CoCmd_dlStart(s_pHalContext);
    EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1));
    EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 255, 255));

    uint16_t iw = 32;
    uint16_t ih = 32;
    uint16_t format = COMPRESSED_RGBA_ASTC_4x4_KHR;

#define BITMAP_ADDRESS_ON_FLASH 5774720 //address of bitmap file from Flash map after generating Flash
    /* Switch Flash to FULL Mode */
    EVE_CoCmd_setBitmap(s_pHalContext, (0x800000 | BITMAP_ADDRESS_ON_FLASH / 32), format, iw, ih);
    //Start drawing bitmap
    EVE_Cmd_wr32(s_pHalContext, BEGIN(BITMAPS));
    EVE_Cmd_wr32(s_pHalContext, VERTEX2II(0, 0, 0, 0));
    EVE_Cmd_wr32(s_pHalContext, END());
    EVE_Cmd_wr32(s_pHalContext, RESTORE_CONTEXT());
    EVE_Cmd_wr32(s_pHalContext, DISPLAY());
    EVE_CoCmd_swap(s_pHalContext);
    EVE_Cmd_waitFlush(s_pHalContext);
    SAMAPP_DELAY;
#endif // Win32 BT81X
}

/**
* @brief API to demonstrateASTC layout
*
*/
void SAMAPP_Bitmap_ASTCLayoutRAMG()
{
#if defined (BT81X_ENABLE) && (defined(MSVC_PLATFORM) || defined(BT8XXEMU_PLATFORM))
    const char* files = TEST_DIR "\\numbers_astc12x10.raw";
    int16_t x = 0;
    int16_t y = 0;

    Ftf_Write_File_To_RAM_G(s_pHalContext, files, 0);

    Draw_Text(s_pHalContext, "Example for: ASTC bitmap on RAM_G");

    Display_Start(s_pHalContext);
    x = 20;
    y = 20;
    helperDrawASTC("1x1", 0, COMPRESSED_RGBA_ASTC_12x10_KHR, x, y, 12, 10, 20, 2);
    y += 30;
    helperDrawASTC("1x2", 0, COMPRESSED_RGBA_ASTC_12x10_KHR, x, y, 12, 20, 20, 2);
    y += 30;
    helperDrawASTC("1x3", 0, COMPRESSED_RGBA_ASTC_12x10_KHR, x, y, 12, 30, 20, 2);
    y += 40;
    helperDrawASTC("1x4", 0, COMPRESSED_RGBA_ASTC_12x10_KHR, x, y, 12, 40, 20, 2);
    y += 50;
    helperDrawASTC("1x5", 0, COMPRESSED_RGBA_ASTC_12x10_KHR, x, y, 12, 50, 20, 2);

    x += 90;
    y = 20;
    helperDrawASTC("2x1", 0, COMPRESSED_RGBA_ASTC_12x10_KHR, x, y, 24, 10, 40, 2);
    y += 30;
    helperDrawASTC("2x2", 0, COMPRESSED_RGBA_ASTC_12x10_KHR, x, y, 24, 20, 40, 2);
    y += 30;
    helperDrawASTC("2x3", 0, COMPRESSED_RGBA_ASTC_12x10_KHR, x, y, 24, 30, 40, 2);
    y += 40;
    helperDrawASTC("2x4", 0, COMPRESSED_RGBA_ASTC_12x10_KHR, x, y, 24, 40, 40, 2);
    y += 50;
    helperDrawASTC("2x5", 0, COMPRESSED_RGBA_ASTC_12x10_KHR, x, y, 24, 50, 40, 2);

    x += 110;
    y = 20;
    helperDrawASTC("3x1", 0, COMPRESSED_RGBA_ASTC_12x10_KHR, x, y, 36, 10, 50, 2);
    y += 30;
    helperDrawASTC("3x2", 0, COMPRESSED_RGBA_ASTC_12x10_KHR, x, y, 36, 20, 50, 2);
    y += 30;
    helperDrawASTC("3x3", 0, COMPRESSED_RGBA_ASTC_12x10_KHR, x, y, 36, 30, 50, 2);
    y += 40;
    helperDrawASTC("3x4", 0, COMPRESSED_RGBA_ASTC_12x10_KHR, x, y, 36, 40, 50, 2);
    y += 50;
    helperDrawASTC("3x5", 0, COMPRESSED_RGBA_ASTC_12x10_KHR, x, y, 36, 50, 50, 2);

    x += 130;
    y = 20;
    helperDrawASTC("4x1", 0, COMPRESSED_RGBA_ASTC_12x10_KHR, x, y, 48, 10, 60, 2);
    y += 30;
    helperDrawASTC("4x2", 0, COMPRESSED_RGBA_ASTC_12x10_KHR, x, y, 48, 20, 60, 2);
    helperDrawASTC("4x3", 0, COMPRESSED_RGBA_ASTC_12x10_KHR, x, y + 30, 48, 30, 60, 2);
    y += 70;
    helperDrawASTC("4x4", 0, COMPRESSED_RGBA_ASTC_12x10_KHR, x, y, 48, 40, 60, 2);
    y += 50;
    helperDrawASTC("4x5", 0, COMPRESSED_RGBA_ASTC_12x10_KHR, x, y, 48, 50, 60, 2);

    x += 150;
    y = 20;
    helperDrawASTC("5x1", 0, COMPRESSED_RGBA_ASTC_12x10_KHR, x, y, 60, 10, 70, 2);
    y += 30;
    helperDrawASTC("5x2", 0, COMPRESSED_RGBA_ASTC_12x10_KHR, x, y, 60, 20, 70, 2);
    y += 30;
    helperDrawASTC("5x3", 0, COMPRESSED_RGBA_ASTC_12x10_KHR, x, y, 60, 30, 70, 2);
    y += 40;
    helperDrawASTC("5x4", 0, COMPRESSED_RGBA_ASTC_12x10_KHR, x, y, 60, 40, 70, 2);
    y += 50;
    helperDrawASTC("5x5", 0, COMPRESSED_RGBA_ASTC_12x10_KHR, x, y, 60, 50, 70, 2);

    y += 70;
    EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 0, 0));
    EVE_CoCmd_text(s_pHalContext, 20, y, 28, OPT_FILL,
        "Note: Multi-celled bitmaps must have a size which is a multiple of 4 blocks (See programming guide)");

    Display_End(s_pHalContext);
    SAMAPP_DELAY;
#endif // Win32 BT81X
}

/**
* @brief API to demonstrate ASTC layout on flash
*
*/
void SAMAPP_Bitmap_ASTCLayoutFlash()
{
#if defined (BT81X_ENABLE) && (defined(MSVC_PLATFORM) || defined(BT8XXEMU_PLATFORM))
    char* files = TEST_DIR "\\numbers_astc12x10.raw";
    int16_t x = 0;
    int16_t y = 0;
    uint32_t astc_flash_addr = 4096;

    Draw_Text(s_pHalContext, "Example for: ASTC bitmap on Flash");

    if (0 == Ftf_Write_File_To_Flash_By_RAM_G(s_pHalContext, files, astc_flash_addr))
    {
        APP_ERR("Error when write raw file to flash");
        return;
    }

    Display_Start(s_pHalContext);
    x = 20;
    y = 20;
    helperDrawASTC("1x1", ATFLASH(astc_flash_addr), COMPRESSED_RGBA_ASTC_12x10_KHR, x, y, 12, 10, 20, 1);
    y += 30;
    helperDrawASTC("1x2", ATFLASH(astc_flash_addr), COMPRESSED_RGBA_ASTC_12x10_KHR, x, y, 12, 20, 20, 1);
    y += 30;
    helperDrawASTC("1x3", ATFLASH(astc_flash_addr), COMPRESSED_RGBA_ASTC_12x10_KHR, x, y, 12, 30, 20, 1);
    y += 40;
    helperDrawASTC("1x4", ATFLASH(astc_flash_addr), COMPRESSED_RGBA_ASTC_12x10_KHR, x, y, 12, 40, 20, 2);
    y += 50;
    helperDrawASTC("1x5", ATFLASH(astc_flash_addr), COMPRESSED_RGBA_ASTC_12x10_KHR, x, y, 12, 50, 20, 1);

    x += 90;
    y = 20;
    helperDrawASTC("2x1", ATFLASH(astc_flash_addr), COMPRESSED_RGBA_ASTC_12x10_KHR, x, y, 24, 10, 40, 1);
    y += 30;
    helperDrawASTC("2x2", ATFLASH(astc_flash_addr), COMPRESSED_RGBA_ASTC_12x10_KHR, x, y, 24, 20, 40, 2);
    y += 30;
    helperDrawASTC("2x3", ATFLASH(astc_flash_addr), COMPRESSED_RGBA_ASTC_12x10_KHR, x, y, 24, 30, 40, 1);
    y += 40;
    helperDrawASTC("2x4", ATFLASH(astc_flash_addr), COMPRESSED_RGBA_ASTC_12x10_KHR, x, y, 24, 40, 40, 2);
    y += 50;
    helperDrawASTC("2x5", ATFLASH(astc_flash_addr), COMPRESSED_RGBA_ASTC_12x10_KHR, x, y, 24, 50, 40, 1);

    x += 110;
    y = 20;
    helperDrawASTC("3x1", ATFLASH(astc_flash_addr), COMPRESSED_RGBA_ASTC_12x10_KHR, x, y, 36, 10, 50, 1);
    y += 30;
    helperDrawASTC("3x2", ATFLASH(astc_flash_addr), COMPRESSED_RGBA_ASTC_12x10_KHR, x, y, 36, 20, 50, 1);
    y += 30;
    helperDrawASTC("3x3", ATFLASH(astc_flash_addr), COMPRESSED_RGBA_ASTC_12x10_KHR, x, y, 36, 30, 50, 1);
    y += 40;
    helperDrawASTC("3x4", ATFLASH(astc_flash_addr), COMPRESSED_RGBA_ASTC_12x10_KHR, x, y, 36, 40, 50, 2);
    y += 50;
    helperDrawASTC("3x5", ATFLASH(astc_flash_addr), COMPRESSED_RGBA_ASTC_12x10_KHR, x, y, 36, 50, 50, 1);

    x += 130;
    y = 20;
    helperDrawASTC("4x1", ATFLASH(astc_flash_addr), COMPRESSED_RGBA_ASTC_12x10_KHR, x, y, 48, 10, 60, 2);
    y += 30;
    helperDrawASTC("4x2", ATFLASH(astc_flash_addr), COMPRESSED_RGBA_ASTC_12x10_KHR, x, y, 48, 20, 60, 2);
    y += 30;
    helperDrawASTC("4x3", ATFLASH(astc_flash_addr), COMPRESSED_RGBA_ASTC_12x10_KHR, x, y, 48, 30, 60, 2);
    y += 40;
    helperDrawASTC("4x4", ATFLASH(astc_flash_addr), COMPRESSED_RGBA_ASTC_12x10_KHR, x, y, 48, 40, 60, 2);
    y += 50;
    helperDrawASTC("4x5", ATFLASH(astc_flash_addr), COMPRESSED_RGBA_ASTC_12x10_KHR, x, y, 48, 50, 60, 2);

    x += 150;
    y = 20;
    helperDrawASTC("5x1", ATFLASH(astc_flash_addr), COMPRESSED_RGBA_ASTC_12x10_KHR, x, y, 60, 10, 70, 1);
    y += 30;
    helperDrawASTC("5x2", ATFLASH(astc_flash_addr), COMPRESSED_RGBA_ASTC_12x10_KHR, x, y, 60, 20, 70, 1);
    y += 30;
    helperDrawASTC("5x3", ATFLASH(astc_flash_addr), COMPRESSED_RGBA_ASTC_12x10_KHR, x, y, 60, 30, 70, 1);
    y += 40;
    helperDrawASTC("5x4", ATFLASH(astc_flash_addr), COMPRESSED_RGBA_ASTC_12x10_KHR, x, y, 60, 40, 70, 2);
    y += 50;
    helperDrawASTC("5x5", ATFLASH(astc_flash_addr), COMPRESSED_RGBA_ASTC_12x10_KHR, x, y, 60, 50, 70, 1);

    y += 70;
    EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 0, 0));
    EVE_CoCmd_text(s_pHalContext, 20, y, 28, OPT_FILL,
        "Note: Multi-celled bitmaps must have a size which is a multiple of 4 blocks (See programming guide)");

    Display_End(s_pHalContext);
    SAMAPP_DELAY;
#endif // Win32 BT81X
}

/**
* @brief API to demonstrate ASTC layout of cell 3x2
*
*/
void SAMAPP_Bitmap_ASTCLayoutCell_1_3x2()
{
#if defined (BT81X_ENABLE) && (defined(MSVC_PLATFORM) || defined(BT8XXEMU_PLATFORM))
    char* files[] = { TEST_DIR "\\ASTC_layout_cell_1_3x2_casei_36x240_ASTC_12x10_KHR.raw", 0 };
    const int32_t astc_flash_addr = 4096;
    int32_t source = ATFLASH(astc_flash_addr);
    int16_t x = 0;
    int16_t y = 0;

    Draw_Text(s_pHalContext, "Example for: ASTC layout of cell 3x2");

    if (0 == Ftf_Write_FileArr_To_Flash_By_RAM_G(s_pHalContext, files, astc_flash_addr))
    {
        APP_ERR("Error when write raw file to flash");
        return;
    }

    Ftf_Write_FileArr_To_RAM_G(s_pHalContext, files, 0);

    Display_Start(s_pHalContext);
    source = 0;
    x = 30;
    y = 30;
    EVE_CoCmd_fillWidth(s_pHalContext, s_pHalContext->Width - 20);
    EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0, 0, 0));
    EVE_CoCmd_text(s_pHalContext, x, y, 28, OPT_FILL,
        "Display CELL 1 for 3x2 case for ASTC in RAM_G is ok:");
    EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 255, 255));
    EVE_CoCmd_setBitmap(s_pHalContext, source, COMPRESSED_RGBA_ASTC_12x10_KHR, 12 * 3, 10 * 2);
    EVE_Cmd_wr32(s_pHalContext, BEGIN(BITMAPS));

    y += 55;
    EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0, 0, 0));
    EVE_CoCmd_text(s_pHalContext, x + 50, y, 22, 0, "CELL 0:");
    EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 255, 255));
    EVE_Cmd_wr32(s_pHalContext, CELL(0)); //#CELL 0
    EVE_Cmd_wr32(s_pHalContext, VERTEX2F((x + 50 + 70) * 16, (y - 2) * 16));

    x += 200;
    EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0, 0, 0));
    EVE_CoCmd_text(s_pHalContext, x, y, 22, 0, "CELL 1:");
    EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 255, 255));
    EVE_Cmd_wr32(s_pHalContext, CELL(1)); //#CELL 1
    EVE_Cmd_wr32(s_pHalContext, VERTEX2F((x + 70) * 16, (y - 2) * 16));

    x = 30;
    y += 80;
    source = ATFLASH(astc_flash_addr);
    EVE_CoCmd_fillWidth(s_pHalContext, s_pHalContext->Width - 20);
    EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0, 0, 0));
    EVE_CoCmd_text(s_pHalContext, x, y, 28, OPT_FILL,
        "Display CELL 1 for 3x2 case for ASTC in Flash shall show some artifacts:");
    EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 255, 255));
    EVE_CoCmd_setBitmap(s_pHalContext, source, COMPRESSED_RGBA_ASTC_12x10_KHR, 12 * 3, 10 * 2);
    EVE_Cmd_wr32(s_pHalContext, BEGIN(BITMAPS));

    y += 50;
    EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0, 0, 0));
    EVE_CoCmd_text(s_pHalContext, x + 50, y, 22, 0, "CELL 0:");
    EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 255, 255));
    EVE_Cmd_wr32(s_pHalContext, CELL(0)); //#CELL 0
    EVE_Cmd_wr32(s_pHalContext, VERTEX2F((x + 50 + 70) * 16, (y - 2) * 16));

    x += 150;
    EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0, 0, 0));
    EVE_CoCmd_text(s_pHalContext, x + 50, y, 22, 0, "CELL 1:");
    EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 255, 255));
    EVE_Cmd_wr32(s_pHalContext, CELL(1)); //#CELL 1
    EVE_Cmd_wr32(s_pHalContext, VERTEX2F((x + 50 + 70) * 16, (y - 2) * 16));

    y += 70;
    EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 0, 0));
    EVE_CoCmd_text(s_pHalContext, 30, y, 28, OPT_FILL,
        "Note: Multi-celled bitmaps must have a size which is a multiple of 4 blocks (See programming guide)");

    Display_End(s_pHalContext);
    SAMAPP_DELAY;
#endif // Win32 BT81X
}

/**
* @brief API to demonstrate multicell ASTC bitmap on RAMG
* 
* On 81x ASTC bitmaps cells must start on a multiple of 4 blocks.
* So, if using 5x4 texels on an image 20x100:
* - The legal cell size is 20x20 (5 cells, 20 blocks / cell), 20x25 (4 cells, 24 blocks / cell) ...
* - The illegal cell size is 10x20(10 cells. 10 blocks / cell), 10x25(8 cells. 14 blocks / cell) ...
* 
* https://github.com/BRTSG-FOSS/EveApps/issues/21
*/
void SAMAPP_Bitmap_ASTCMultiCellRAMG()
{
#if defined (BT81X_ENABLE)
    const char* astc_file = TEST_DIR "\\cell_100x672_COMPRESSED_RGBA_ASTC_4x4_KHR.raw";
    int16_t x = 20;
    int16_t y = 20;
    uint32_t astc_addr = 0;

    Draw_Text(s_pHalContext, "Example for: Multicell ASTC bitmap on RAM_G");

    uint32_t fs = Ftf_Write_File_To_RAM_G(s_pHalContext, astc_file, astc_addr);
    if (fs == 0)
    {
        APP_ERR("Error when write raw file to RAM_G");
        return;
    }

    Display_Start(s_pHalContext);
    EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0x33, 0x33, 0x33));
    EVE_CoCmd_text(s_pHalContext, 20, y, 28, 0,
        "Example for: Multicell ASTC bitmap on RAM_G:\n\n\n"\
        "On 81x ASTC bitmaps cells must start on a multiple of 4 blocks.\n"\
        "So, if using ASTC 4x4 texels on an image 100x672:\n\n"\
        ""\
        "  - The legal cell size is 100x96 (7 cells, 600 blocks / cell):\n");

    y += 180;
    EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0xff, 0xff, 0xff));
    helperDrawASTC(0, astc_addr, COMPRESSED_RGBA_ASTC_4x4_KHR, x, y, 100, 96, 105, 7);

    y += 140;
    EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0x33, 0x33, 0x33));
    EVE_CoCmd_text(s_pHalContext, 20, y, 28, OPT_FILL,
        "  - The illegal cell size is 100x84 (8 cells, 525 blocks / cell):\n");
    y += 40;
    EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 255, 255));
    helperDrawASTC(0, astc_addr, COMPRESSED_RGBA_ASTC_4x4_KHR, x, y, 100, 84, 105, 7);

    Display_End(s_pHalContext);
    SAMAPP_DELAY;
#endif
}

/**
* @brief API to demonstrate multicell ASTC bitmap on Flash
* 
* On 81x ASTC bitmaps cells must start on a multiple of 4 blocks.
* So, if using 5x4 texels on an image 20x100:
* - The legal cell size is 20x20 (5 cells, 20 blocks / cell), 20x25 (4 cells, 24 blocks / cell) ...
* - The illegal cell size is 10x20(10 cells. 10 blocks / cell), 10x25(8 cells. 14 blocks / cell) ...
* 
* https://github.com/BRTSG-FOSS/EveApps/issues/21
*/
void SAMAPP_Bitmap_ASTCMultiCellFlash()
{
#if defined (BT81X_ENABLE)
    const char* astc_file = TEST_DIR "\\cell_100x672_COMPRESSED_RGBA_ASTC_4x4_KHR.raw";
    int16_t x = 20;
    int16_t y = 20;
    uint32_t astc_addr = 0;

    Draw_Text(s_pHalContext, "Example for: Multicell ASTC bitmap on Flash");

    uint32_t fs = Ftf_Write_File_To_Flash_By_RAM_G(s_pHalContext, astc_file, astc_addr);
    if (fs == 0)
    {
        APP_ERR("Error when write raw file to Flash");
        return;
    }

    Display_Start(s_pHalContext);
    EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0x33, 0x33, 0x33));
    EVE_CoCmd_text(s_pHalContext, 20, y, 28, 0,
        "Example for: Multicell ASTC bitmap on Flash:\n\n\n"\
        "On 81x ASTC bitmaps cells must start on a multiple of 4 blocks.\n"\
        "So, if using ASTC 4x4 texels on an image 100x672:\n\n"\
        ""\
        "  - The legal cell size is 100x96 (7 cells, 600 blocks / cell):\n");

    y += 180;
    EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0xff, 0xff, 0xff));
    helperDrawASTC(0, ATFLASH(astc_addr), COMPRESSED_RGBA_ASTC_4x4_KHR, x, y, 100, 96, 105, 7);

    y += 140;
    EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0x33, 0x33, 0x33));
    EVE_CoCmd_text(s_pHalContext, 20, y, 28, OPT_FILL,
        "  - The illegal cell size is 100x84 (8 cells, 525 blocks / cell):\n");
    y += 40;
    EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 255, 255));
    helperDrawASTC(0, ATFLASH(astc_addr), COMPRESSED_RGBA_ASTC_4x4_KHR, x, y, 100, 84, 105, 7);

    Display_End(s_pHalContext);
    SAMAPP_DELAY;
#endif
}

/**
* @brief API to demonstrate image rotate
*
*/
void SAMAPP_Bitmap_rotate()
{
#if defined (BT81X_ENABLE) && (defined(MSVC_PLATFORM) || defined(BT8XXEMU_PLATFORM))
    uchar8_t imagefile[] = TEST_DIR "\\lenaface_COMPRESSED_RGBA_ASTC_4x4_KHR.raw";
    uint16_t image_w = 32;
    uint16_t image_h = 32;

    Draw_Text(s_pHalContext, "Example for: Bitmap rotate");
    Gpu_Hal_LoadImageToMemory(s_pHalContext, imagefile, RAM_G, LOAD);

    for (uint16_t i = 0; i <= 360; i++)
    {
        EVE_CoCmd_dlStart(s_pHalContext); // start
        EVE_Cmd_wr32(s_pHalContext, CLEAR_COLOR_RGB(0, 0, 255));
        EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1));
        EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 255, 255));
        EVE_Cmd_wr32(s_pHalContext, BITMAP_HANDLE((0)));

        EVE_CoCmd_setBitmap(s_pHalContext, RAM_G, COMPRESSED_RGBA_ASTC_4x4_KHR, image_w, image_h);
        EVE_Cmd_wr32(s_pHalContext, BITMAP_SIZE(BILINEAR, BORDER, BORDER, image_w, image_h));
        EVE_Cmd_wr32(s_pHalContext, BITMAP_SIZE_H(image_w >> 9, image_h >> 9));
        EVE_CoCmd_text(s_pHalContext, 10, 0, 24, 0, "CMD_ROTATEAROUND"); //text info
        EVE_Cmd_waitFlush(s_pHalContext);

        EVE_Cmd_wr32(s_pHalContext, BEGIN(BITMAPS));

        EVE_CoCmd_loadIdentity(s_pHalContext);
        EVE_CoCmd_rotateAround(s_pHalContext, image_w / 2, image_h / 2, i * 65536 / 360, 65536 * 1);
        EVE_CoCmd_setMatrix(s_pHalContext);
        EVE_Cmd_wr32(s_pHalContext,
            VERTEX2F((s_pHalContext->Width / 2 - image_w / 2) * 16,
                (s_pHalContext->Height / 2 - image_h / 2) * 16));
        EVE_Cmd_wr32(s_pHalContext, END());

        EVE_CoCmd_loadIdentity(s_pHalContext);
        EVE_CoCmd_setMatrix(s_pHalContext);

        EVE_Cmd_wr32(s_pHalContext, DISPLAY());
        EVE_CoCmd_swap(s_pHalContext);
        EVE_Cmd_waitFlush(s_pHalContext);
        EVE_sleep(5);
    }
    SAMAPP_DELAY;
#endif // Win32 BT81X
}

/**
* @brief Draw set 12
*
*/
void SAMAPP_Bitmap_rotateAndTranslate()
{
#if defined (BT81X_ENABLE) && (defined(MSVC_PLATFORM) || defined(BT8XXEMU_PLATFORM))
    int count = 0;
    const uint32_t TRANSLATE_XY = 100;

    // tiles
    uint16_t tile_w = 256;
    uint16_t tile_h = 256;
    uint16_t tile1_x = -30;
    uint16_t tile1_y = 50;
    uint16_t tile2_x = (uint16_t)(s_pHalContext->Width - tile1_x - tile_w - 200);
    uint16_t tile2_y = tile1_y;

    Draw_Text(s_pHalContext, "Example for: Bitmap rotate and translate");
    helperloadDefaultImage();

    while (count++ < 60 * 10)
    { // wait 10 seconds, 60 FPS
      /*Display List start*/
        EVE_CoCmd_dlStart(s_pHalContext);
        EVE_Cmd_wr32(s_pHalContext, CLEAR_COLOR_RGB(0, 0, 0));
        EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1));

        static uint16_t rotation_angle = 0;
        static uint16_t font_size = 29;
        static uint16_t option = 0;

        rotation_angle = (rotation_angle + 1) % MAX_ANGLE;

        helperRotateAroundOne(RAM_G, RGB565, tile1_x, tile1_y, tile_w, tile_h, rotation_angle);
        helperRotateAndTranslateOne(RAM_G, RGB565, tile2_x, tile2_y, tile_w, tile_h, rotation_angle);

        EVE_CoCmd_text(s_pHalContext, tile1_x + TRANSLATE_XY / 2, tile1_y, font_size, option, "Rotate by RotateAround");
        EVE_CoCmd_text(s_pHalContext, tile2_x + TRANSLATE_XY / 2, tile2_y, font_size, option, "Rotate by Rotate and Translate");

        EVE_Cmd_wr32(s_pHalContext, DISPLAY()); //send command display - to end display commands
        EVE_CoCmd_swap(s_pHalContext);// draw the new screen
        EVE_Cmd_waitFlush(s_pHalContext);// Wait until EVE is free
    }
    SAMAPP_DELAY;
#endif
}

/**
* @brief Load image to RAM_G
*
*/
void SAMAPP_Bitmap_loadImage()
{
    uint32_t fileSize = 0;
    const uint32_t bytePerTrans = 1000;
    uint8_t buff[1000];

    const uint8_t* image = TEST_DIR "\\mandrill256.jpg";

    Draw_Text(s_pHalContext, "Example for: CMD_LOADIMAGE");

    fileSize = FileIO_File_Open(image, FILEIO_E_FOPEN_READ);

    /******************* Decode jpg output into location 0 and output color format as RGB565 *********************/
    Display_Start(s_pHalContext);
    EVE_Cmd_wr32(s_pHalContext, CMD_LOADIMAGE);
    EVE_Cmd_wr32(s_pHalContext, 0); //destination address of jpg decode
    EVE_Cmd_wr32(s_pHalContext, 0); //output format of the bitmap

    while (fileSize > 0)
    {
        /* copy the data into pbuff and then transfter it to command buffer */
        int bytes = FileIO_File_Read(buff, bytePerTrans);
        EVE_Cmd_wrMem(s_pHalContext, buff, bytes);
        fileSize -= bytes;
    }
    /* close the opened jpg file */
    FileIO_File_Close();

    int32_t ImgW = 256, ImgH = 256;
    int16_t xoffset = (int16_t) ((s_pHalContext->Width - ImgW) / 2);
    int16_t yoffset = (int16_t) ((s_pHalContext->Height - ImgH) / 2);
    EVE_Cmd_wr32(s_pHalContext, BEGIN(BITMAPS));
    EVE_Cmd_wr32(s_pHalContext, VERTEX2F(xoffset * 16, yoffset * 16));
    Display_End(s_pHalContext);

    SAMAPP_DELAY;
}

/**
* @brief demonstrates the usage of loadimage function
* Download the jpg data into command buffer and in turn coprocessor decodes and dumps into location 0 with rgb565 format
*
*/
void SAMAPP_Bitmap_loadImageMono()
{
#define BUFFERSIZE 8192
    uint8_t* pbuff;
    int16_t ImgW;
    int16_t ImgH;
    int32_t xoffset;
    int32_t yoffset;
    const char* file = TEST_DIR "\\mandrill256.jpg";

    ImgW = ImgH = 256;
    xoffset = (s_pHalContext->Width - ImgW) / 2;
    yoffset = (s_pHalContext->Height - ImgH) / 2;

    Draw_Text(s_pHalContext, "Example for: Load image and display as monochromic image");

    /* decode the jpeg data */
    if (0 >= FileIO_File_Open(file, FILEIO_E_FOPEN_READ))
    {
        printf("Error in opening file %s \n", "mandrill256.jpg");
        return;
    }
    pbuff = (uint8_t*) malloc(8192);

    /// Decode jpg output into location 0 and output color format as L8

    EVE_CoCmd_dlStart(s_pHalContext);
    EVE_Cmd_wr32(s_pHalContext, CMD_LOADIMAGE);
    EVE_Cmd_wr32(s_pHalContext, 0); //destination address of jpg decode
    EVE_Cmd_wr32(s_pHalContext, OPT_MONO); //output format of the bitmap

    int bytes = FileIO_File_Read(pbuff, BUFFERSIZE);
    while (bytes)
    {
        uint16_t blocklen = bytes > BUFFERSIZE ? BUFFERSIZE : (uint16_t) bytes;

        /* copy data continuously into command memory */
        EVE_Cmd_wrMem(s_pHalContext, pbuff, blocklen); //alignment is already taken care by this api
        bytes = FileIO_File_Read(pbuff, BUFFERSIZE);
    }
    free(pbuff);
    FileIO_File_Close();
    EVE_Cmd_waitFlush(s_pHalContext);

    EVE_Cmd_wr32(s_pHalContext, BEGIN(BITMAPS));
    EVE_Cmd_wr32(s_pHalContext, VERTEX2F((int16_t )(xoffset * 16), (int16_t )(yoffset * 16)));
    EVE_Cmd_wr32(s_pHalContext, END());

    xoffset = s_pHalContext->Width / 2;
    yoffset = s_pHalContext->Height / 2;
    EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0, 0, 255));
    EVE_CoCmd_text(s_pHalContext, (int16_t) xoffset, (int16_t) yoffset, 26, OPT_CENTER,
        "Display bitmap by jpg decode L8");

    Display_End(s_pHalContext);
    SAMAPP_DELAY;
}

/**
* @brief Load image to Coprocessor full color
*
*/
void SAMAPP_Bitmap_loadImageFullColor()
{
#define BUFFERSIZE 8192
    uint8_t* pbuff;
    int16_t ImgW;
    int16_t ImgH;
    int16_t xoffset;
    int16_t yoffset;
    const char* file = TEST_DIR "\\mandrill256.jpg";

    ImgW = ImgH = 256;
    xoffset = (int16_t) ((s_pHalContext->Width - ImgW) / 2);
    yoffset = (int16_t) ((s_pHalContext->Height - ImgH) / 2);

    Draw_Text(s_pHalContext, "Example for: Load image full color ");

    /* decode the jpeg data */
    if (0 >= FileIO_File_Open(file, FILEIO_E_FOPEN_READ))
    {
        printf("Error in opening file %s \n", "mandrill256.jpg");
        return;
    }
    pbuff = (uint8_t*) malloc(8192);

    /// Decode jpg output into location 0 and output color format as L8
    Display_Start(s_pHalContext);
    EVE_Cmd_wr32(s_pHalContext, CMD_LOADIMAGE);
    EVE_Cmd_wr32(s_pHalContext, 0); //destination address of jpg decode
    EVE_Cmd_wr32(s_pHalContext, 0); //output format of the bitmap

    int bytes = FileIO_File_Read(pbuff, BUFFERSIZE);
    while (bytes)
    {
        uint16_t blocklen = bytes > BUFFERSIZE ? BUFFERSIZE : (uint16_t) bytes;

        /* copy data continuously into command memory */
        EVE_Cmd_wrMem(s_pHalContext, pbuff, blocklen); //alignment is already taken care by this api
        bytes = FileIO_File_Read(pbuff, BUFFERSIZE);
    }
    free(pbuff);
    EVE_Cmd_waitFlush(s_pHalContext);

    EVE_Cmd_wr32(s_pHalContext, BEGIN(BITMAPS));
    EVE_Cmd_wr32(s_pHalContext, VERTEX2F(xoffset * 16, yoffset * 16));
    EVE_Cmd_wr32(s_pHalContext, END());

    xoffset = (int16_t) ((s_pHalContext->Width) / 2);
    yoffset = (int16_t) ((s_pHalContext->Height) / 2);
    EVE_CoCmd_text(s_pHalContext, xoffset, yoffset, 26, OPT_CENTER, "Display bitmap by jpg decode");

    Display_End(s_pHalContext);
    SAMAPP_DELAY;
}

/**
* @brief Load DXT1 compressed image. The BRIDGETEK DXT1 conversion utility outputs 4 seperate files: c0,c1,b0,b1.
* The 4 files should be combined to create the final image.  The bitmap size can be reduced up to 4 folds of the original size.
*
*/
void SAMAPP_Bitmap_DXT1()
{
#if defined(FT81X_ENABLE) // FT81X only
    //RAM_G is starting address in graphics RAM, for example 00 0000h
    uint16_t imgWidth = 128;
    uint16_t imgHeight = 128;
    uint16_t c0_c1_width = 32;
    uint16_t c0_c1_height = 32;
    uint16_t c0_c1_stride = c0_c1_width * 2;
    uint16_t b0_b1_width = imgWidth;
    uint16_t b0_b1_height = imgHeight;
    uint16_t b0_b1_stride = b0_b1_width / 8;
    uint16_t szPerFile = 2048;

    Draw_Text(s_pHalContext, "Example for: Load DXT1 compressed image");

    Ftf_Write_File_To_RAM_G(s_pHalContext, TEST_DIR "\\SAMAPP_Tomato_DXT1_C0_Data_Raw.bin", RAM_G);
    Ftf_Write_File_To_RAM_G(s_pHalContext, TEST_DIR "\\SAMAPP_Tomato_DXT1_C1_Data_Raw.bin",
        RAM_G + szPerFile);
    Ftf_Write_File_To_RAM_G(s_pHalContext, TEST_DIR "\\SAMAPP_Tomato_DXT1_B0_Data_Raw.bin",
        RAM_G + szPerFile * 2);
    Ftf_Write_File_To_RAM_G(s_pHalContext, TEST_DIR "\\SAMAPP_Tomato_DXT1_B1_Data_Raw.bin",
        RAM_G + szPerFile * 3);

    EVE_CoCmd_dlStart(s_pHalContext);
    EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1));
    EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 255, 255));

    EVE_CoCmd_loadIdentity(s_pHalContext);
    EVE_CoCmd_setMatrix(s_pHalContext);

    EVE_Cmd_wr32(s_pHalContext, SAVE_CONTEXT());
    // B0&B1 handle
    EVE_Cmd_wr32(s_pHalContext, BITMAP_HANDLE(1));
    EVE_Cmd_wr32(s_pHalContext, BITMAP_SOURCE(RAM_G + szPerFile * 2));
    EVE_Cmd_wr32(s_pHalContext, BITMAP_LAYOUT(L1, b0_b1_stride, b0_b1_height));//L1 format stride is 1 bit per pixel
    EVE_Cmd_wr32(s_pHalContext, BITMAP_SIZE(NEAREST, BORDER, BORDER, imgWidth, imgHeight));//draw in full size
    
    // C0&C1 handle
    EVE_Cmd_wr32(s_pHalContext, BITMAP_HANDLE(2));
    EVE_Cmd_wr32(s_pHalContext, BITMAP_SOURCE(RAM_G));
    EVE_Cmd_wr32(s_pHalContext, BITMAP_LAYOUT(RGB565, c0_c1_stride, c0_c1_height));//RGB565 format stride is 2 bytes per pixel
    EVE_Cmd_wr32(s_pHalContext, BITMAP_SIZE(NEAREST, BORDER, BORDER, imgWidth, imgHeight));//draw in full size
                                                                                           
    // start drawing bitmaps
    EVE_Cmd_wr32(s_pHalContext, BEGIN(BITMAPS));
    EVE_Cmd_wr32(s_pHalContext, BLEND_FUNC(ONE, ZERO));
    EVE_Cmd_wr32(s_pHalContext, COLOR_A(0x55));
    EVE_Cmd_wr32(s_pHalContext,
        VERTEX2II(s_pHalContext->Width / 2 - b0_b1_width / 2,
            s_pHalContext->Height / 2 - b0_b1_height / 2, 1, 0));
    EVE_Cmd_wr32(s_pHalContext, BLEND_FUNC(ONE, ONE));
    EVE_Cmd_wr32(s_pHalContext, COLOR_A(0xAA));
    EVE_Cmd_wr32(s_pHalContext,
        VERTEX2II(s_pHalContext->Width / 2 - b0_b1_width / 2,
            s_pHalContext->Height / 2 - b0_b1_height / 2, 1, 1));
    EVE_Cmd_wr32(s_pHalContext, COLOR_MASK(1, 1, 1, 0));
    EVE_CoCmd_scale(s_pHalContext, 4 * 65536, 4 * 65536);
    EVE_CoCmd_setMatrix(s_pHalContext);
    EVE_Cmd_wr32(s_pHalContext, BLEND_FUNC(DST_ALPHA, ZERO));
    EVE_Cmd_wr32(s_pHalContext,
        VERTEX2II(s_pHalContext->Width / 2 - b0_b1_width / 2,
            s_pHalContext->Height / 2 - b0_b1_height / 2, 2, 1));
    EVE_Cmd_wr32(s_pHalContext, BLEND_FUNC(ONE_MINUS_DST_ALPHA, ONE));
    EVE_Cmd_wr32(s_pHalContext,
        VERTEX2II(s_pHalContext->Width / 2 - b0_b1_width / 2,
            s_pHalContext->Height / 2 - b0_b1_height / 2, 2, 0));
    EVE_Cmd_wr32(s_pHalContext, END());
    EVE_Cmd_wr32(s_pHalContext, RESTORE_CONTEXT());

    //reset the transformation matrix because its not part of the context, RESTORE_CONTEXT() command will not revert the command.
    EVE_CoCmd_loadIdentity(s_pHalContext);
    EVE_CoCmd_setMatrix(s_pHalContext);

    EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 0, 0));
    EVE_CoCmd_text(s_pHalContext, (int16_t) (s_pHalContext->Width / 2), 50, 31, OPT_CENTER,
        "DXT1: 8KB.");
    EVE_CoCmd_text(s_pHalContext, (int16_t) (s_pHalContext->Width / 2), 80, 31, OPT_CENTER,
        "Original: 32KB.");

    EVE_Cmd_wr32(s_pHalContext, DISPLAY());
    //swap the current display list with the new display list
    EVE_CoCmd_swap(s_pHalContext);
    //write to the FT800 FIFO command buffer - bitmap will appear after this command
    EVE_Cmd_waitFlush(s_pHalContext);
    SAMAPP_DELAY;
#endif
}

/**
* @brief API to demonstrate Paletted8 format
*
*/
void SAMAPP_Bitmap_paletted8()
{
    const SAMAPP_Bitmap_header_t* p_bmhdr;

    int32_t ft800_memaddr = RAM_G;
    int32_t pal_mem_addr = 900 * 1024;

    Draw_Text(s_pHalContext, "Example for: Paletted8 format");

    Gpu_Hal_LoadImageToMemory(s_pHalContext, TEST_DIR "\\Background_index.raw", RAM_G, LOAD);
    Gpu_Hal_LoadImageToMemory(s_pHalContext, TEST_DIR "\\Background_lut.raw", pal_mem_addr, LOAD);
#ifndef FT81X_ENABLE
    pal_mem_addr = RAM_PAL;
#else
    ft800_memaddr = pal_mem_addr;
#endif
    p_bmhdr = &SAMAPP_Bitmap_RawData_Header[3];
    EVE_Cmd_waitFlush(s_pHalContext);
    App_WrDl_Buffer(s_pHalContext, CLEAR(1, 1, 1)); // clear screen
    App_WrDl_Buffer(s_pHalContext, BEGIN(EDGE_STRIP_B));// clear screen
    App_WrDl_Buffer(s_pHalContext, VERTEX2II(0, 0, 0, 0));
    App_WrDl_Buffer(s_pHalContext, VERTEX2F(s_pHalContext->Width * 16, 0));

    App_WrDl_Buffer(s_pHalContext, BITMAP_SOURCE(RAM_G));
#ifdef FT81X_ENABLE
    App_WrDl_Buffer(s_pHalContext, BITMAP_LAYOUT(PALETTED8, p_bmhdr->Stride, p_bmhdr->Height));
    App_WrDl_Buffer(s_pHalContext, BITMAP_LAYOUT_H(p_bmhdr->Stride >> 10, p_bmhdr->Height >> 9));
#else
    App_WrDl_Buffer(s_pHalContext, BITMAP_LAYOUT(p_bmhdr->Format, p_bmhdr->Stride, p_bmhdr->Height));
#endif

    App_WrDl_Buffer(s_pHalContext,
        BITMAP_SIZE(NEAREST, BORDER, BORDER, p_bmhdr->Width, p_bmhdr->Height));
    App_WrDl_Buffer(s_pHalContext, BITMAP_SIZE_H(p_bmhdr->Width >> 9, p_bmhdr->Height >> 9));
    App_WrDl_Buffer(s_pHalContext, BEGIN(BITMAPS)); // start drawing bitmaps

#ifdef FT81X_ENABLE
    App_WrDl_Buffer(s_pHalContext, BLEND_FUNC(ONE, ZERO));

    App_WrDl_Buffer(s_pHalContext, COLOR_MASK(0, 0, 0, 1));
    App_WrDl_Buffer(s_pHalContext, PALETTE_SOURCE(ft800_memaddr + 3));
    App_WrDl_Buffer(s_pHalContext, VERTEX2II(0, 0, 0, 0));

    App_WrDl_Buffer(s_pHalContext, BLEND_FUNC(DST_ALPHA, ONE_MINUS_DST_ALPHA));
    App_WrDl_Buffer(s_pHalContext, COLOR_MASK(1, 0, 0, 0));
    App_WrDl_Buffer(s_pHalContext, PALETTE_SOURCE(ft800_memaddr + 2));
    App_WrDl_Buffer(s_pHalContext, VERTEX2II(0, 0, 0, 0));

    App_WrDl_Buffer(s_pHalContext, COLOR_MASK(0, 1, 0, 0));
    App_WrDl_Buffer(s_pHalContext, PALETTE_SOURCE(ft800_memaddr + 1));
    App_WrDl_Buffer(s_pHalContext, VERTEX2II(0, 0, 0, 0));

    App_WrDl_Buffer(s_pHalContext, COLOR_MASK(0, 0, 1, 0));
    App_WrDl_Buffer(s_pHalContext, PALETTE_SOURCE(ft800_memaddr + 0));
    App_WrDl_Buffer(s_pHalContext, VERTEX2II(0, 0, 0, 0));

#else
    App_WrDl_Buffer(s_pHalContext, PALETTE_SOURCE(ft800_memaddr));
    App_WrDl_Buffer(s_pHalContext, VERTEX2II(0, 0, 0, 0));
#endif

    App_WrDl_Buffer(s_pHalContext, END());
    App_WrDl_Buffer(s_pHalContext, DISPLAY());

    /* Download the DL into DL RAM */
    App_Flush_DL_Buffer(s_pHalContext);

    /* Do a swap */
    GPU_DLSwap(s_pHalContext, DLSWAP_FRAME);
    SAMAPP_DELAY;
}

/**
* @brief this function demonstrates the usage of the paletted bitmap converted by the BRIDGETEK palette converter
*
*/
void SAMAPP_Bitmap_paletted8Simple()
{
    uint16_t bitmapHeight = 128;
    uint16_t bitmapWidth = 128;

    Draw_Text(s_pHalContext, "Example for: Paletted8 format");

    EVE_CoCmd_dlStart(s_pHalContext);
    EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1));
    EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 255, 255));

    helperLoadRawFromFile(TEST_DIR "\\Tomato_lut.raw", RAM_G);
    helperLoadRawFromFile(TEST_DIR "\\Tomato_index.raw", 1024);

    EVE_Cmd_wr32(s_pHalContext, BEGIN(BITMAPS));
    EVE_Cmd_wr32(s_pHalContext, PALETTE_SOURCE(RAM_G));
    EVE_Cmd_wr32(s_pHalContext, BITMAP_SOURCE(1024));
    EVE_Cmd_wr32(s_pHalContext, BITMAP_LAYOUT(PALETTED4444, bitmapWidth, bitmapHeight));
    EVE_Cmd_wr32(s_pHalContext, BITMAP_LAYOUT_H(((bitmapWidth * 2L) >> 10), ((bitmapHeight) >> 9)));
    EVE_Cmd_wr32(s_pHalContext, BITMAP_SIZE(NEAREST, BORDER, BORDER, bitmapWidth, bitmapHeight));
    EVE_Cmd_wr32(s_pHalContext,
        VERTEX2F((s_pHalContext->Width / 2 - bitmapWidth / 2) * 16,
            (s_pHalContext->Height / 2 - bitmapHeight / 2 - bitmapHeight) * 16));

    EVE_Cmd_wr32(s_pHalContext, DISPLAY());
    EVE_CoCmd_swap(s_pHalContext);
    EVE_Cmd_waitFlush(s_pHalContext);
    SAMAPP_DELAY;
}

/**
* @brief support bitmap resolutions up to 2048x2048
* FT80x can only display bitmaps no larger than 512x512, FT81x
* If the bitmap dimensions are bigger than 512 in either direction
*/
void SAMAPP_Bitmap_higherResolutionBitmap()
{
#if defined(FT81X_ENABLE) // FT81X only
    Draw_Text(s_pHalContext, "Example for: Bitmap resolutions up to 2048x2048");
    EVE_Cmd_waitFlush(s_pHalContext);

    EVE_CoCmd_dlStart(s_pHalContext);
    EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1));
    EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 255, 255));

    uint16_t iw = 800;
    uint16_t ih = 480;
    uint16_t format = RGB565;

    //load bitmap file into graphics RAM
    //RAM_G is starting address in graphics RAM, for example 00 0000h
    Gpu_Hal_LoadImageToMemory(s_pHalContext, TEST_DIR "\\flower_800x480_800x480_RGB565.raw", RAM_G,
        LOAD);
    EVE_CoCmd_setBitmap(s_pHalContext, RAM_G, format, iw, ih);

    //Start drawing bitmap
    EVE_Cmd_wr32(s_pHalContext, BITMAP_HANDLE(1));
    EVE_Cmd_wr32(s_pHalContext, SAVE_CONTEXT());
    EVE_Cmd_wr32(s_pHalContext, BEGIN(BITMAPS));
    EVE_Cmd_wr32(s_pHalContext, VERTEX2II(0, 0, 0, 0));
    EVE_Cmd_wr32(s_pHalContext, END());
    EVE_Cmd_wr32(s_pHalContext, RESTORE_CONTEXT());
    EVE_Cmd_wr32(s_pHalContext, DISPLAY());
    EVE_CoCmd_swap(s_pHalContext);
    EVE_Cmd_waitFlush(s_pHalContext);

    SAMAPP_DELAY;
#endif
}

/**
* @brief API to demonstrate scale, rotate and translate functionality
*
*/
void SAMAPP_Bitmap_matrix()
{
    /*************************************************************************/
    /* Below code demonstrates the usage of bitmap matrix processing apis.   */
    /* Mainly matrix apis consists if scale, rotate and translate.           */
    /* Units of translation and scale are interms of 1/65536, rotation is in */
    /* degrees and in terms of 1/65536. +ve theta is anticlock wise, and -ve  */
    /* theta is clock wise rotation                                          */
    /*************************************************************************/

    /* Lena image with 40x40 rgb565 is used as an example */
    int32_t imagewidth;
    int32_t imagestride;
    int32_t imageheight;
    int32_t imagexoffset;
    int32_t imageyoffset;

    Draw_Text(s_pHalContext, "Example for: Scale, rotate and translate functionality");

    /* Download the bitmap data */
    Ftf_Write_File_nBytes_To_RAM_G(s_pHalContext, TEST_DIR "\\SAMAPP_Bitmap_RawData.bin", RAM_G,
        SAMAPP_Bitmap_RawData_Header[0].Stride * SAMAPP_Bitmap_RawData_Header[0].Height,
        SAMAPP_Bitmap_RawData_Header[0].Arrayoffset);

    EVE_CoCmd_dlStart(s_pHalContext);
    EVE_Cmd_wr32(s_pHalContext, CLEAR_COLOR_RGB(0xff, 0xff, 0xff));
    EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1));
    EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(32, 32, 32));
    EVE_CoCmd_text(s_pHalContext, 10, 5, 16, 0, "BM with rotation");
    EVE_CoCmd_text(s_pHalContext, 10, 20 + 40 + 5, 16, 0, "BM with scaling");
    EVE_CoCmd_text(s_pHalContext, 10, 20 + 40 + 20 + 80 + 5, 16, 0, "BM with flip");

    imagewidth = SAMAPP_Bitmap_RawData_Header[0].Width;
    imageheight = SAMAPP_Bitmap_RawData_Header[0].Height;
    imagestride = SAMAPP_Bitmap_RawData_Header[0].Stride;
    imagexoffset = 10 * 16;
    imageyoffset = 20 * 16;

    EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0xff, 0xff, 0xff));
    EVE_Cmd_wr32(s_pHalContext, BEGIN(BITMAPS));
    EVE_Cmd_wr32(s_pHalContext, BITMAP_SOURCE(0));
    EVE_Cmd_wr32(s_pHalContext,
        BITMAP_LAYOUT(SAMAPP_Bitmap_RawData_Header[0].Format, imagestride, imageheight));
    EVE_Cmd_wr32(s_pHalContext, BITMAP_SIZE(BILINEAR, BORDER, BORDER, imagewidth, imageheight));
    /******************************************* Perform display of plain bitmap ************************************/
    EVE_Cmd_wr32(s_pHalContext, VERTEX2F(imagexoffset, imageyoffset));

    /* Perform display of plain bitmap with 45 degrees anti clock wise and the rotation is performed on top left coordinate */
    imagexoffset += (imagewidth + 10) * 16;
    EVE_CoCmd_loadIdentity(s_pHalContext);
    EVE_CoCmd_rotate(s_pHalContext, (-45 * 65536 / 360)); //rotate by 45 degrees anticlock wise
    EVE_CoCmd_setMatrix(s_pHalContext);
    EVE_Cmd_wr32(s_pHalContext, VERTEX2F(imagexoffset, imageyoffset));

    /* Perform display of plain bitmap with 30 degrees clock wise and the rotation is performed on top left coordinate */
    imagexoffset += (int32_t) (imagewidth * 1.42 + 10) * 16; //add the width*1.41 as diagonal is new width and extra 10 pixels
    EVE_CoCmd_loadIdentity(s_pHalContext);
    EVE_CoCmd_rotate(s_pHalContext, 30 * 65536 / 360); //rotate by 33 degrees clock wise
    EVE_CoCmd_setMatrix(s_pHalContext);
    EVE_Cmd_wr32(s_pHalContext, VERTEX2F(imagexoffset, imageyoffset));

    /* Perform display of plain bitmap with 45 degrees anti clock wise and the rotation is performed wrt centre of the bitmap */
    imagexoffset += (int32_t) (imagewidth * 1.42 + 10) * 16; //add the width*1.41 as diagonal is new width and extra 10 pixels
    EVE_CoCmd_loadIdentity(s_pHalContext);
    EVE_CoCmd_translate(s_pHalContext, 65536 * imagewidth / 2, 65536 * imageheight / 2); //make the rotation coordinates at the center
    EVE_CoCmd_rotate(s_pHalContext, -45 * 65536 / 360); //rotate by 45 degrees anticlock wise
    EVE_CoCmd_translate(s_pHalContext, -65536 * imagewidth / 2, -65536 * imageheight / 2);
    EVE_CoCmd_setMatrix(s_pHalContext);
    EVE_Cmd_wr32(s_pHalContext, VERTEX2F(imagexoffset, imageyoffset));

    /* Perform display of plain bitmap with 45 degrees clock wise and the rotation is performed so that whole bitmap is viewable */
    imagexoffset += (int32_t) (imagewidth * 1.42 + 10) * 16; //add the width*1.41 as diagonal is new width and extra 10 pixels
    EVE_CoCmd_loadIdentity(s_pHalContext);
    EVE_CoCmd_translate(s_pHalContext, 65536 * imagewidth / 2, 65536 * imageheight / 2); //make the rotation coordinates at the center
    EVE_CoCmd_rotate(s_pHalContext, 45 * 65536 / 360); //rotate by 45 degrees clock wise
    EVE_CoCmd_translate(s_pHalContext, -65536 * imagewidth / 10, -65536 * imageheight / 2);
    EVE_CoCmd_setMatrix(s_pHalContext);
    EVE_Cmd_wr32(s_pHalContext,
        BITMAP_SIZE(BILINEAR, BORDER, BORDER, imagewidth * 2, imageheight * 2));
    EVE_Cmd_wr32(s_pHalContext, VERTEX2F(imagexoffset, imageyoffset));

    /* Perform display of plain bitmap with 90 degrees anti clock wise and the rotation is performed so that whole bitmap is viewable */
    imagexoffset += (imagewidth * 2 + 10) * 16;
    EVE_CoCmd_loadIdentity(s_pHalContext);
    EVE_CoCmd_translate(s_pHalContext, 65536 * imagewidth / 2, 65536 * imageheight / 2); //make the rotation coordinates at the center
    EVE_CoCmd_rotate(s_pHalContext, -90 * 65536 / 360); //rotate by 90 degrees anticlock wise
    EVE_CoCmd_translate(s_pHalContext, -65536 * imagewidth / 2, -65536 * imageheight / 2);
    EVE_CoCmd_setMatrix(s_pHalContext);
    EVE_Cmd_wr32(s_pHalContext, BITMAP_SIZE(BILINEAR, BORDER, BORDER, imagewidth, imageheight));
    EVE_Cmd_wr32(s_pHalContext, VERTEX2F(imagexoffset, imageyoffset));

    /* Perform display of plain bitmap with 180 degrees clock wise and the rotation is performed so that whole bitmap is viewable */
    imagexoffset += (imagewidth + 10) * 16;
    EVE_CoCmd_loadIdentity(s_pHalContext);
    EVE_CoCmd_translate(s_pHalContext, 65536 * imagewidth / 2, 65536 * imageheight / 2); //make the rotation coordinates at the center
    EVE_CoCmd_rotate(s_pHalContext, -180 * 65536 / 360); //rotate by 180 degrees anticlock wise
    EVE_CoCmd_translate(s_pHalContext, -65536 * imagewidth / 2, -65536 * imageheight / 2);
    EVE_CoCmd_setMatrix(s_pHalContext);
    EVE_Cmd_wr32(s_pHalContext, VERTEX2F(imagexoffset, imageyoffset));
    /******************************************* Perform display of bitmap with scale ************************************/
    /* Perform display of plain bitmap with scale factor of 2x2 in x & y direction */
    imagexoffset = (10) * 16;
    imageyoffset += (imageheight + 20) * 16;
    EVE_CoCmd_loadIdentity(s_pHalContext);
    EVE_CoCmd_scale(s_pHalContext, 2 * 65536, 2 * 65536); //scale by 2x2
    EVE_CoCmd_setMatrix(s_pHalContext);
    EVE_Cmd_wr32(s_pHalContext,
        BITMAP_SIZE(BILINEAR, BORDER, BORDER, imagewidth * 2, imageheight * 2));
    EVE_Cmd_wr32(s_pHalContext, VERTEX2F(imagexoffset, imageyoffset));

    /* Perform display of plain bitmap with scale factor of .5x.25 in x & y direction, rotate by 45 degrees clock wise wrt top left */
    imagexoffset += (imagewidth * 2 + 10) * 16;
    EVE_CoCmd_loadIdentity(s_pHalContext);
    EVE_CoCmd_translate(s_pHalContext, 65536 * imagewidth / 2, 65536 * imageheight / 2); //make the rotation coordinates at the center

    EVE_CoCmd_rotate(s_pHalContext, 45 * 65536 / 360); //rotate by 45 degrees clock wise
    EVE_CoCmd_scale(s_pHalContext, 65536 / 2, 65536 / 4); //scale by .5x.25
    EVE_CoCmd_translate(s_pHalContext, -65536 * imagewidth / 2, -65536 * imageheight / 2);
    EVE_CoCmd_setMatrix(s_pHalContext);
    EVE_Cmd_wr32(s_pHalContext, VERTEX2F(imagexoffset, imageyoffset));

    /* Perform display of plain bitmap with scale factor of .5x2 in x & y direction, rotate by 75 degrees anticlock wise wrt center of the image */
    imagexoffset += (imagewidth + 10) * 16;
    EVE_CoCmd_loadIdentity(s_pHalContext);
    EVE_CoCmd_translate(s_pHalContext, 65536 * imagewidth / 2, 65536 * imageheight / 2); //make the rotation coordinates at the center
    EVE_CoCmd_rotate(s_pHalContext, -75 * 65536 / 360); //rotate by 75 degrees anticlock wise
    EVE_CoCmd_scale(s_pHalContext, 65536 / 2, 2 * 65536); //scale by .5x2
    EVE_CoCmd_translate(s_pHalContext, -65536 * imagewidth / 2, -65536 * imageheight / 8);
    EVE_CoCmd_setMatrix(s_pHalContext);
    EVE_Cmd_wr32(s_pHalContext,
        BITMAP_SIZE(BILINEAR, BORDER, BORDER, imagewidth * 5 / 2, imageheight * 5 / 2));
    EVE_Cmd_wr32(s_pHalContext, VERTEX2F(imagexoffset, imageyoffset));
    /******************************************* Perform display of bitmap flip ************************************/
    /* perform display of plain bitmap with 1x1 and flip right */
    imagexoffset = (10) * 16;
    imageyoffset += (imageheight * 2 + 20) * 16;
    EVE_CoCmd_loadIdentity(s_pHalContext);
    EVE_CoCmd_translate(s_pHalContext, 65536 * imagewidth / 2, 65536 * imageheight / 2); //make the rotation coordinates at the center
    EVE_CoCmd_scale(s_pHalContext, -1 * 65536, 1 * 65536); //flip right
    EVE_CoCmd_translate(s_pHalContext, -65536 * imagewidth / 2, -65536 * imageheight / 2);
    EVE_CoCmd_setMatrix(s_pHalContext);
    EVE_Cmd_wr32(s_pHalContext, BITMAP_SIZE(BILINEAR, BORDER, BORDER, imagewidth, imageheight));
    EVE_Cmd_wr32(s_pHalContext, VERTEX2F(imagexoffset, imageyoffset));

    /* Perform display of plain bitmap with 2x2 scaling, flip bottom */
    imagexoffset += (imagewidth + 10) * 16;
    EVE_CoCmd_loadIdentity(s_pHalContext);
    EVE_CoCmd_translate(s_pHalContext, 65536 * imagewidth / 2, 65536 * imageheight / 2); //make the rotation coordinates at the center
    EVE_CoCmd_scale(s_pHalContext, 2 * 65536, -2 * 65536); //flip bottom and scale by 2 on both sides
    EVE_CoCmd_translate(s_pHalContext, -65536 * imagewidth / 4,
        (int32_t) (-65536 * imageheight / 1.42));
    EVE_CoCmd_setMatrix(s_pHalContext);
    EVE_Cmd_wr32(s_pHalContext,
        BITMAP_SIZE(BILINEAR, BORDER, BORDER, imagewidth * 4, imageheight * 4));
    EVE_Cmd_wr32(s_pHalContext, VERTEX2F(imagexoffset, imageyoffset));

    /* Perform display of plain bitmap with 2x1 scaling, rotation and flip right and make sure whole image is viewable */
    imagexoffset += (imagewidth * 2 + 10) * 16;
    EVE_CoCmd_loadIdentity(s_pHalContext);
    EVE_CoCmd_translate(s_pHalContext, 65536 * imagewidth / 2, 65536 * imageheight / 2); //make the rotation coordinates at the center

    EVE_CoCmd_rotate(s_pHalContext, -45 * 65536 / 360); //rotate by 45 degrees anticlock wise
    EVE_CoCmd_scale(s_pHalContext, -2 * 65536, 1 * 65536); //flip right and scale by 2 on x axis
    EVE_CoCmd_translate(s_pHalContext, -65536 * imagewidth / 2, -65536 * imageheight / 8);
    EVE_CoCmd_setMatrix(s_pHalContext);
    EVE_Cmd_wr32(s_pHalContext,
        BITMAP_SIZE(BILINEAR, BORDER, BORDER, (imagewidth * 5), (imageheight * 5)));
    EVE_Cmd_wr32(s_pHalContext, VERTEX2F(imagexoffset, imageyoffset));

    EVE_Cmd_wr32(s_pHalContext, END());
    EVE_Cmd_wr32(s_pHalContext, DISPLAY());
    EVE_CoCmd_swap(s_pHalContext);

    /* Wait till coprocessor completes the operation */
    EVE_Cmd_waitFlush(s_pHalContext);
    SAMAPP_DELAY;
}

void SAMAPP_Bitmap() 
{
    SAMAPP_Bitmap_getImage();
    SAMAPP_Bitmap_nonSquareDisplay();
    SAMAPP_Bitmap_dithering();
    SAMAPP_Bitmap_ASTC();
    SAMAPP_Bitmap_ASTCLayoutRAMG();
    SAMAPP_Bitmap_ASTCLayoutFlash();
    SAMAPP_Bitmap_ASTCMultiCellRAMG();
    SAMAPP_Bitmap_ASTCMultiCellFlash(); 
    SAMAPP_Bitmap_ASTCLayoutCell_1_3x2();
    SAMAPP_Bitmap_rotate();
    SAMAPP_Bitmap_rotateAndTranslate();
    SAMAPP_Bitmap_loadImage();
    SAMAPP_Bitmap_loadImageMono();
    SAMAPP_Bitmap_loadImageFullColor();
    SAMAPP_Bitmap_DXT1();
    SAMAPP_Bitmap_paletted8();
    SAMAPP_Bitmap_paletted8Simple();
    SAMAPP_Bitmap_higherResolutionBitmap();
}


