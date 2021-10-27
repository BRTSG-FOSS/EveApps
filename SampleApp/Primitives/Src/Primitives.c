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
#include "Primitives.h"

static EVE_HalContext s_halContext;
static EVE_HalContext* s_pHalContext;
void SAMAPP_Primitives();

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
        "This sample demonstrate the using of", 
        "basic elements, such as lines, rectangle, point",
        "bitmap, text ..."
    }; 

    while (TRUE) {
        WelcomeScreen(s_pHalContext, info);

        SAMAPP_Primitives();

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
* @brief linear interpolation
*
* @param t deltaTime
* @param a transform position
* @param b target position
* @return float
*/
float helperLerp(float t, float a, float b)
{
    return (1 - t) * a + t * b;
}

/**
* @brief Smooth linear interpolation
*
* @param t deltaTime
* @param a transform position
* @param b target position
* @return float
*/
static float helperSmoothLerp(float t, float a, float b)
{
    float lt = 3 * t * t - 2 * t * t * t;
    return helperLerp(lt, a, b);
}

/**
* @brief display few points at various offsets with various colors
*
*/
void SAMAPP_Primitives_points()
{
    Draw_Text(s_pHalContext, "Example for: Points");

    /* Construct DL of points */
    EVE_Hal_wr32(s_pHalContext, RAM_DL + 0, CLEAR_COLOR_RGB(128, 128, 128));
    EVE_Hal_wr32(s_pHalContext, RAM_DL + 4, CLEAR(1, 1, 1));
    EVE_Hal_wr32(s_pHalContext, RAM_DL + 8, COLOR_RGB(128, 0, 0));
    EVE_Hal_wr32(s_pHalContext, RAM_DL + 12, POINT_SIZE(5 * 16));
    EVE_Hal_wr32(s_pHalContext, RAM_DL + 16, BEGIN(FTPOINTS));
    EVE_Hal_wr32(s_pHalContext, RAM_DL + 20,
        VERTEX2F((s_pHalContext->Width / 5) * 16, (s_pHalContext->Height / 2) * 16));
    EVE_Hal_wr32(s_pHalContext, RAM_DL + 24, COLOR_RGB(0, 128, 0));
    EVE_Hal_wr32(s_pHalContext, RAM_DL + 28, POINT_SIZE(15 * 16));
    EVE_Hal_wr32(s_pHalContext, RAM_DL + 32,
        VERTEX2F((s_pHalContext->Width * 2 / 5) * 16, (s_pHalContext->Height / 2) * 16));
    EVE_Hal_wr32(s_pHalContext, RAM_DL + 36, COLOR_RGB(0, 0, 128));
    EVE_Hal_wr32(s_pHalContext, RAM_DL + 40, POINT_SIZE(25 * 16));
    EVE_Hal_wr32(s_pHalContext, RAM_DL + 44,
        VERTEX2F((s_pHalContext->Width * 3 / 5) * 16, (s_pHalContext->Height / 2) * 16));
    EVE_Hal_wr32(s_pHalContext, RAM_DL + 48, COLOR_RGB(128, 128, 0));
    EVE_Hal_wr32(s_pHalContext, RAM_DL + 52, POINT_SIZE(35 * 16));
    EVE_Hal_wr32(s_pHalContext, RAM_DL + 56,
        VERTEX2F((s_pHalContext->Width * 4 / 5) * 16, (s_pHalContext->Height / 2) * 16));
    EVE_Hal_wr32(s_pHalContext, RAM_DL + 60, DISPLAY()); // display the image

                                                         /* Do a swap */
    GPU_DLSwap(s_pHalContext, DLSWAP_FRAME);
    SAMAPP_ENABLE_DELAY();
}

/**
* @brief Display Lines on screen
*
*/
void SAMAPP_Primitives_lines()
{
    Draw_Text(s_pHalContext, "Example for: Lines");

    int16_t LineHeight = 25;

    App_WrDl_Buffer(s_pHalContext, CLEAR(1, 1, 1)); // clear screen
    App_WrDl_Buffer(s_pHalContext, COLOR_RGB(128, 0, 0));
    App_WrDl_Buffer(s_pHalContext, LINE_WIDTH(5 * 16));
    App_WrDl_Buffer(s_pHalContext, BEGIN(LINES));
    App_WrDl_Buffer(s_pHalContext,
        VERTEX2F((s_pHalContext->Width / 4) * 16, ((s_pHalContext->Height - LineHeight) / 2) * 16));
    App_WrDl_Buffer(s_pHalContext,
        VERTEX2F((s_pHalContext->Width / 4) * 16, ((s_pHalContext->Height + LineHeight) / 2) * 16));
    App_WrDl_Buffer(s_pHalContext, COLOR_RGB(0, 128, 0));
    App_WrDl_Buffer(s_pHalContext, LINE_WIDTH(10 * 16));
    LineHeight = 40;
    App_WrDl_Buffer(s_pHalContext,
        VERTEX2F((s_pHalContext->Width * 2 / 4) * 16,
            ((s_pHalContext->Height - LineHeight) / 2) * 16));
    App_WrDl_Buffer(s_pHalContext,
        VERTEX2F((s_pHalContext->Width * 2 / 4) * 16,
            ((s_pHalContext->Height + LineHeight) / 2) * 16));
    App_WrDl_Buffer(s_pHalContext, COLOR_RGB(128, 128, 0));
    App_WrDl_Buffer(s_pHalContext, LINE_WIDTH(20 * 16));
    LineHeight = 55;
    App_WrDl_Buffer(s_pHalContext,
        VERTEX2F((s_pHalContext->Width * 3 / 4) * 16,
            ((s_pHalContext->Height - LineHeight) / 2) * 16));
    App_WrDl_Buffer(s_pHalContext,
        VERTEX2F((s_pHalContext->Width * 3 / 4) * 16,
            ((s_pHalContext->Height + LineHeight) / 2) * 16));
    App_WrDl_Buffer(s_pHalContext, DISPLAY());

    /* Download the DL into DL RAM */
    App_Flush_DL_Buffer(s_pHalContext);

    /* Do a swap */
    GPU_DLSwap(s_pHalContext, DLSWAP_FRAME);
    SAMAPP_ENABLE_DELAY();
}

/**
* @brief Display rectangles on screen
*
*/
void SAMAPP_Primitives_rectangles()
{
    Draw_Text(s_pHalContext, "Example for: Rectangles");

    int16_t RectWidth;
    int16_t RectHeight;

    App_WrDl_Buffer(s_pHalContext, CLEAR(1, 1, 1)); // clear screen
    App_WrDl_Buffer(s_pHalContext, COLOR_RGB(0, 0, 128));
    App_WrDl_Buffer(s_pHalContext, LINE_WIDTH(1 * 16)); //LINE_WIDTH is used for corner curvature
    App_WrDl_Buffer(s_pHalContext, BEGIN(RECTS));
    RectWidth = 5;
    RectHeight = 25;
    App_WrDl_Buffer(s_pHalContext,
        VERTEX2F(((s_pHalContext->Width / 4) - (RectWidth / 2)) * 16,
            ((s_pHalContext->Height - RectHeight) / 2) * 16));
    App_WrDl_Buffer(s_pHalContext,
        VERTEX2F(((s_pHalContext->Width / 4) + (RectWidth / 2)) * 16,
            ((s_pHalContext->Height + RectHeight) / 2) * 16));
    App_WrDl_Buffer(s_pHalContext, COLOR_RGB(0, 128, 0));
    App_WrDl_Buffer(s_pHalContext, LINE_WIDTH(5 * 16));
    RectWidth = 10;
    RectHeight = 40;
    App_WrDl_Buffer(s_pHalContext,
        VERTEX2F(((s_pHalContext->Width * 2 / 4) - (RectWidth / 2)) * 16,
            ((s_pHalContext->Height - RectHeight) / 2) * 16));
    App_WrDl_Buffer(s_pHalContext,
        VERTEX2F(((s_pHalContext->Width * 2 / 4) + (RectWidth / 2)) * 16,
            ((s_pHalContext->Height + RectHeight) / 2) * 16));
    App_WrDl_Buffer(s_pHalContext, COLOR_RGB(128, 128, 0));
    App_WrDl_Buffer(s_pHalContext, LINE_WIDTH(10 * 16));
    RectWidth = 20;
    RectHeight = 55;
    App_WrDl_Buffer(s_pHalContext,
        VERTEX2F(((s_pHalContext->Width * 3 / 4) - (RectWidth / 2)) * 16,
            ((s_pHalContext->Height - RectHeight) / 2) * 16));
    App_WrDl_Buffer(s_pHalContext,
        VERTEX2F(((s_pHalContext->Width * 3 / 4) + (RectWidth / 2)) * 16,
            ((s_pHalContext->Height + RectHeight) / 2) * 16));
    App_WrDl_Buffer(s_pHalContext, DISPLAY());

    /* Download the DL into DL RAM */
    App_Flush_DL_Buffer(s_pHalContext);

    /* Do a swap */
    GPU_DLSwap(s_pHalContext, DLSWAP_FRAME);
    SAMAPP_ENABLE_DELAY();
}

/**
* @brief Display image at different locations with various colors, -ve offsets, alpha blend
*
*/
void SAMAPP_Primitives_bitmap()
{
    Draw_Text(s_pHalContext, "Example for: Display image");

    const SAMAPP_Bitmap_header_t* p_bmhdr;
    uint32_t BMoffsetx;
    uint32_t BMoffsety;

    p_bmhdr = &SAMAPP_Bitmap_RawData_Header[0];
    /* Copy raw data into address 0 followed by generation of bitmap */
    Ftf_Write_File_nBytes_To_RAM_G(s_pHalContext, TEST_DIR "\\SAMAPP_Bitmap_RawData.bin", RAM_G,
        p_bmhdr->Stride * p_bmhdr->Height, p_bmhdr->Arrayoffset);

    App_WrDl_Buffer(s_pHalContext, CLEAR(1, 1, 1)); // clear screen
    App_WrDl_Buffer(s_pHalContext, COLOR_RGB(255, 255, 255));
    App_WrDl_Buffer(s_pHalContext, BITMAP_SOURCE(RAM_G));
    App_WrDl_Buffer(s_pHalContext,
        BITMAP_LAYOUT(p_bmhdr->Format, p_bmhdr->Stride, p_bmhdr->Height));
    App_WrDl_Buffer(s_pHalContext,
        BITMAP_SIZE(NEAREST, BORDER, BORDER, p_bmhdr->Width, p_bmhdr->Height));
    App_WrDl_Buffer(s_pHalContext, BEGIN(BITMAPS)); // start drawing bitmaps
    BMoffsetx = ((s_pHalContext->Width / 4) - (p_bmhdr->Width / 2));
    BMoffsety = ((s_pHalContext->Height / 2) - (p_bmhdr->Height / 2));
    App_WrDl_Buffer(s_pHalContext, VERTEX2II(BMoffsetx, BMoffsety, 0, 0));
    App_WrDl_Buffer(s_pHalContext, COLOR_RGB(255, 64, 64)); // red at (200, 120)
    BMoffsetx = ((s_pHalContext->Width * 2 / 4) - (p_bmhdr->Width / 2));
    BMoffsety = ((s_pHalContext->Height / 2) - (p_bmhdr->Height / 2));
    App_WrDl_Buffer(s_pHalContext, VERTEX2II(BMoffsetx, BMoffsety, 0, 0));
    App_WrDl_Buffer(s_pHalContext, COLOR_RGB(64, 180, 64)); // green at (216, 136)
    BMoffsetx += (p_bmhdr->Width / 2);
    BMoffsety += (p_bmhdr->Height / 2);
    App_WrDl_Buffer(s_pHalContext, VERTEX2II(BMoffsetx, BMoffsety, 0, 0));
    App_WrDl_Buffer(s_pHalContext, COLOR_RGB(255, 255, 64)); // transparent yellow at (232, 152)
    App_WrDl_Buffer(s_pHalContext, COLOR_A(150));
    BMoffsetx += (p_bmhdr->Width / 2);
    BMoffsety += (p_bmhdr->Height / 2);
    App_WrDl_Buffer(s_pHalContext, VERTEX2II(BMoffsetx, BMoffsety, 0, 0));
    App_WrDl_Buffer(s_pHalContext, COLOR_A(255));
    App_WrDl_Buffer(s_pHalContext, COLOR_RGB(255, 255, 255));
    App_WrDl_Buffer(s_pHalContext, VERTEX2F(-10 * 16, -10 * 16)); //for -ve coordinates use vertex2f instruction
    App_WrDl_Buffer(s_pHalContext, END());
    App_WrDl_Buffer(s_pHalContext, DISPLAY());

    /* Download the DL into DL RAM */
    App_Flush_DL_Buffer(s_pHalContext);

    /* Do a swap */
    GPU_DLSwap(s_pHalContext, DLSWAP_FRAME);
    SAMAPP_ENABLE_DELAY();
}

/**
* @brief Bitmap Palette background
*
*/
void SAMAPP_Primitives_bitmapPaletteBackground()
{
#if defined(FT81X_ENABLE) && (defined(MSVC_PLATFORM) || defined(BT8XXEMU_PLATFORM))
    Draw_Text(s_pHalContext, "Example for: Bitmap Palette background");

    const SAMAPP_Bitmap_header_t* p_bmhdr;

    int32_t LutFileLen;
    int32_t FileLen;

    uchar8_t* pbuff;
    uchar8_t* plutbuff;
    int32_t ft800_memaddr = RAM_G;
    int32_t pal_mem_addr;

    FILE* pFile = fopen(TEST_DIR "\\background_paletted.raw", "rb");
    if (!pFile)
    {
        return;
    }
    FILE* pLutFile = fopen(TEST_DIR "\\background_paletted_lut.raw", "rb");
    if (!pLutFile)
    {
        fclose(pFile);
        return;
    }

    fseek(pFile, 0, SEEK_END);
    FileLen = ftell(pFile);
    fseek(pFile, 0, SEEK_SET);

    pbuff = (uchar8_t*)malloc(FileLen);

    fread(pbuff, 1, FileLen, pFile);
    fclose(pFile);

    p_bmhdr = &SAMAPP_Bitmap_RawData_Header[2];

    const int blockSize = 32 * 1024;
    while (FileLen > blockSize)
    {
        EVE_Hal_wrProgMem(s_pHalContext, ft800_memaddr, pbuff, blockSize);
        FileLen -= blockSize;
        ft800_memaddr += blockSize;
        pbuff += blockSize;
    }
    EVE_Hal_wrProgMem(s_pHalContext, ft800_memaddr, pbuff, FileLen);
    ft800_memaddr += FileLen; //last chunk if any

    fseek(pLutFile, 0, SEEK_END);
    LutFileLen = ftell(pLutFile);
    fseek(pLutFile, 0, SEEK_SET);

    plutbuff = (uchar8_t*)malloc(LutFileLen);

    fread(plutbuff, 1, LutFileLen, pLutFile);
    fclose(pLutFile);

#ifndef FT81X_ENABLE
    pal_mem_addr = RAM_PAL;
#else
    pal_mem_addr = ft800_memaddr;
#endif

    EVE_Hal_wrProgMem(s_pHalContext, pal_mem_addr, plutbuff, LutFileLen);

    App_WrDl_Buffer(s_pHalContext, CLEAR(1, 1, 1)); // clear screen
    App_WrDl_Buffer(s_pHalContext, COLOR_RGB(255, 255, 255));

    App_WrDl_Buffer(s_pHalContext, BITMAP_SOURCE(RAM_G));
    App_WrDl_Buffer(s_pHalContext, BITMAP_LAYOUT(p_bmhdr->Format, p_bmhdr->Stride, p_bmhdr->Height));
    App_WrDl_Buffer(s_pHalContext, BITMAP_SIZE(NEAREST, BORDER, BORDER, p_bmhdr->Width, p_bmhdr->Height));
    App_WrDl_Buffer(s_pHalContext, BEGIN(BITMAPS));// start drawing bitmaps
#ifdef FT81X_ENABLE
    App_WrDl_Buffer(s_pHalContext, BLEND_FUNC(ONE, ZERO));
    App_WrDl_Buffer(s_pHalContext, BEGIN(BITMAPS));

    App_WrDl_Buffer(s_pHalContext, COLOR_MASK(0, 0, 0, 1));
    App_WrDl_Buffer(s_pHalContext, PALETTE_SOURCE(ft800_memaddr + 3));
    App_WrDl_Buffer(s_pHalContext, VERTEX2II(0, 0, 0, 0));

    App_WrDl_Buffer(s_pHalContext, BLEND_FUNC(DST_ALPHA, ONE_MINUS_DST_ALPHA));
    App_WrDl_Buffer(s_pHalContext, COLOR_MASK(1, 0, 0, 0));
    App_WrDl_Buffer(s_pHalContext, PALETTE_SOURCE(ft800_memaddr + 2));
    App_WrDl_Buffer(s_pHalContext, VERTEX2II(0, 0, 0, 0));

    App_WrDl_Buffer(s_pHalContext, COLOR_MASK(0, 1, 0, 0));
    App_WrDl_Buffer(s_pHalContext, PALETTE_SOURCE(ft800_memaddr + 1));
    App_WrDl_Buffer(s_pHalContext, VERTEX2F(0, 0));

    App_WrDl_Buffer(s_pHalContext, COLOR_MASK(0, 0, 1, 0));
    App_WrDl_Buffer(s_pHalContext, PALETTE_SOURCE(ft800_memaddr + 0));
    App_WrDl_Buffer(s_pHalContext, VERTEX2II(0, 0, 0, 0));

#else
    App_WrDl_Buffer(s_pHalContext, VERTEX2II(0, 0, 0, 0));
#endif

    App_WrDl_Buffer(s_pHalContext, END());
    App_WrDl_Buffer(s_pHalContext, DISPLAY());

    /* Download the DL into DL RAM */
    App_Flush_DL_Buffer(s_pHalContext);

    /* Do a swap */
    GPU_DLSwap(s_pHalContext, DLSWAP_FRAME);
    SAMAPP_ENABLE_DELAY();
#endif
}

/**
* @brief Bitmap Palette sample
*
*/
void SAMAPP_Primitives_bitmapPalette()
{
#if defined(FT81X_ENABLE) && (defined(MSVC_PLATFORM) || defined(BT8XXEMU_PLATFORM))
    Draw_Text(s_pHalContext, "Example for: Bitmap Palette ");

    const SAMAPP_Bitmap_header_t* p_bmhdr;
    int32_t BMoffsetx;
    int32_t BMoffsety;
    int32_t LutFileLen;
    int32_t FileLen;
    int32_t pal_mem_addr;

    void* pbuff;
    void* plutbuff;
    /* Paletted4444 format */
    FILE* pFile = fopen(TEST_DIR "\\lenaface40_index.raw", "rb");
    if (!pFile)
    {
        return;
    }
    FILE* pLutFile = fopen(TEST_DIR "\\lenaface40_lut.raw", "rb");
    if (!pLutFile)
    {
        fclose(pFile);
        return;
    }
    if (!pFile || !pLutFile)
    {
        return;
    }

    fseek(pFile, 0, SEEK_END);
    FileLen = ftell(pFile);
    fseek(pFile, 0, SEEK_SET);

    pbuff = malloc(FileLen);

    fread(pbuff, 1, FileLen, pFile);
    fclose(pFile);

    p_bmhdr = &SAMAPP_Bitmap_RawData_Header[1];
    EVE_Hal_wrProgMem(s_pHalContext, RAM_G, (uchar8_t*)pbuff, p_bmhdr->Stride * p_bmhdr->Height);

    fseek(pLutFile, 0, SEEK_END);
    LutFileLen = ftell(pLutFile);
    fseek(pLutFile, 0, SEEK_SET);

    plutbuff = malloc(LutFileLen);

    fread(plutbuff, 1, LutFileLen, pLutFile);
    fclose(pLutFile);
#ifdef FT81X_ENABLE
    pal_mem_addr = RAM_G + p_bmhdr->Stride * p_bmhdr->Height;
#else
    pal_mem_addr = RAM_PAL;
#endif

    EVE_Hal_wrProgMem(s_pHalContext, pal_mem_addr, (uchar8_t*)plutbuff, LutFileLen);

    App_WrDl_Buffer(s_pHalContext, CLEAR(1, 1, 1)); // clear screen
    App_WrDl_Buffer(s_pHalContext, COLOR_RGB(255, 255, 255));
#ifdef FT81X_ENABLE
    App_WrDl_Buffer(s_pHalContext, PALETTE_SOURCE(pal_mem_addr));
#endif
    App_WrDl_Buffer(s_pHalContext, BITMAP_SOURCE(RAM_G));
    App_WrDl_Buffer(s_pHalContext, BITMAP_LAYOUT(p_bmhdr->Format, p_bmhdr->Stride, p_bmhdr->Height));
    App_WrDl_Buffer(s_pHalContext, BITMAP_SIZE(NEAREST, BORDER, BORDER, p_bmhdr->Width, p_bmhdr->Height));
    App_WrDl_Buffer(s_pHalContext, BEGIN(BITMAPS)); // start drawing bitmaps
    BMoffsetx = ((s_pHalContext->Width / 4) - (p_bmhdr->Width / 2));
    BMoffsety = ((s_pHalContext->Height / 2) - (p_bmhdr->Height / 2));
    App_WrDl_Buffer(s_pHalContext, VERTEX2II(BMoffsetx, BMoffsety, 0, 0));

    App_WrDl_Buffer(s_pHalContext, END());
    App_WrDl_Buffer(s_pHalContext, DISPLAY());

    /* Download the DL into DL RAM */
    App_Flush_DL_Buffer(s_pHalContext);

    /* Do a swap */
    GPU_DLSwap(s_pHalContext, DLSWAP_FRAME);
    SAMAPP_ENABLE_DELAY();
#endif
}

/**
* @brief Display a Bar Graph
*
*/
void SAMAPP_Primitives_barGraph()
{
#define SAMAPP_BARGRAPH_ARRAY_SIZE (256)
    /* Write the data into RAM_G */
    uint8_t Y_Array[SAMAPP_BARGRAPH_ARRAY_SIZE];
    uint32_t numchunks = 0;
    int32_t String_size;
    uint32_t hoffset = 0;
    uint32_t voffset = 0;
    uint32_t widthaligh;

    Draw_Text(s_pHalContext, "Example for: Display a Bar Graph");

    hoffset = 0;
    voffset = (s_pHalContext->Height - 256) / 2; //centre of the screen

    widthaligh = ALIGN_TWO_POWER_N(s_pHalContext->Width, SAMAPP_BARGRAPH_ARRAY_SIZE);
    numchunks = widthaligh / SAMAPP_BARGRAPH_ARRAY_SIZE;
    String_size = SAMAPP_BARGRAPH_ARRAY_SIZE;
    for (int j = 0; j < numchunks; j++)
    {
        for (int i = 0; i < SAMAPP_BARGRAPH_ARRAY_SIZE; i++)
        {
            Y_Array[i] = random(128) + 64; //within range
        }
        EVE_Hal_wrMem(s_pHalContext, RAM_G + j * SAMAPP_BARGRAPH_ARRAY_SIZE, &Y_Array[0],
            String_size);
    }
    App_WrDl_Buffer(s_pHalContext, CLEAR_COLOR_RGB(255, 255, 255));
    App_WrDl_Buffer(s_pHalContext, CLEAR(1, 1, 1)); // clear screen
    App_WrDl_Buffer(s_pHalContext, BITMAP_SOURCE(RAM_G));
    App_WrDl_Buffer(s_pHalContext, BITMAP_LAYOUT(BARGRAPH, 256, 1));
    App_WrDl_Buffer(s_pHalContext, COLOR_RGB(128, 0, 0));
    App_WrDl_Buffer(s_pHalContext, BITMAP_SIZE(NEAREST, BORDER, BORDER, 256, 256));
    App_WrDl_Buffer(s_pHalContext, BEGIN(BITMAPS));
    App_WrDl_Buffer(s_pHalContext, BITMAP_HANDLE(0));
    /* Display text 8x8 at hoffset, voffset location */
    for (int i = 0; i < numchunks; i++)
    {
        App_WrDl_Buffer(s_pHalContext, CELL(i));
        App_WrDl_Buffer(s_pHalContext, VERTEX2F(hoffset * 16, voffset * 16));
        hoffset += SAMAPP_BARGRAPH_ARRAY_SIZE;
    }

    App_WrDl_Buffer(s_pHalContext, DISPLAY());

    /* Download the DL into DL RAM */
    App_Flush_DL_Buffer(s_pHalContext);

    /* Do a swap */
    GPU_DLSwap(s_pHalContext, DLSWAP_FRAME);
    SAMAPP_ENABLE_DELAY();

    /* drawing of sine wave with rising amplitude */
    String_size = SAMAPP_BARGRAPH_ARRAY_SIZE;
    for (int j = 0; j < numchunks; j++)
    {
        for (int i = 0; i < SAMAPP_BARGRAPH_ARRAY_SIZE; i++)
        {
            int32_t tmpval;
            int16_t tmpidx;
            tmpidx = (int16_t) (i + j * SAMAPP_BARGRAPH_ARRAY_SIZE);
            tmpval = 128 + ((tmpidx / 4) * Math_Qsin(65536 * tmpidx / 48) / 65536); //within range

            Y_Array[i] = tmpval & 0xff;
        }
        EVE_Hal_wrMem(s_pHalContext, RAM_G + j * SAMAPP_BARGRAPH_ARRAY_SIZE, Y_Array, String_size);
    }

    SAMAPP_ENABLE_DELAY();

    App_WrDl_Buffer(s_pHalContext, CLEAR(1, 1, 1)); // clear screen
    App_WrDl_Buffer(s_pHalContext, BITMAP_SOURCE(RAM_G));
    App_WrDl_Buffer(s_pHalContext, BITMAP_LAYOUT(BARGRAPH, 256, 1));
    App_WrDl_Buffer(s_pHalContext, BITMAP_SIZE(NEAREST, BORDER, BORDER, 256, 256));
    App_WrDl_Buffer(s_pHalContext, BEGIN(BITMAPS));
    App_WrDl_Buffer(s_pHalContext, BITMAP_HANDLE(0));

    /* Display bargraph at hoffset, voffset location */
    voffset = (s_pHalContext->Height - 256) / 2; //centre of the screen
    hoffset = 0;
    for (int i = 0; i < numchunks; i++)
    {
        App_WrDl_Buffer(s_pHalContext, COLOR_RGB(0xff, 0, 0));
        App_WrDl_Buffer(s_pHalContext, CELL(i));
        App_WrDl_Buffer(s_pHalContext, VERTEX2F(hoffset * 16, voffset * 16));

        App_WrDl_Buffer(s_pHalContext, COLOR_RGB(0, 0, 0));
        App_WrDl_Buffer(s_pHalContext, VERTEX2F(hoffset * 16, (voffset + 4) * 16));
        hoffset += SAMAPP_BARGRAPH_ARRAY_SIZE;
    }
    App_WrDl_Buffer(s_pHalContext, DISPLAY());

    /* Download the DL into DL RAM */
    App_Flush_DL_Buffer(s_pHalContext);

    /* Do a swap */
    GPU_DLSwap(s_pHalContext, DLSWAP_FRAME);
    SAMAPP_ENABLE_DELAY();
}

/**
* @brief Display Linestrips
*
*/
void SAMAPP_Primitives_lineStrips()
{
    Draw_Text(s_pHalContext, "Example for: Linestrips");

    App_WrDl_Buffer(s_pHalContext, CLEAR_COLOR_RGB(5, 45, 10));
    App_WrDl_Buffer(s_pHalContext, COLOR_RGB(255, 168, 64));
    App_WrDl_Buffer(s_pHalContext, CLEAR(1, 1, 1));
    App_WrDl_Buffer(s_pHalContext, BEGIN(LINE_STRIP));
    App_WrDl_Buffer(s_pHalContext, VERTEX2F(16 * 16, 16 * 16));
    App_WrDl_Buffer(s_pHalContext,
        VERTEX2F((s_pHalContext->Width * 2 / 3) * 16, (s_pHalContext->Height * 2 / 3) * 16));
    App_WrDl_Buffer(s_pHalContext,
        VERTEX2F((s_pHalContext->Width - 80) * 16, (s_pHalContext->Height - 20) * 16));
    App_WrDl_Buffer(s_pHalContext, DISPLAY());

    /* Download the DL into DL RAM */
    App_Flush_DL_Buffer(s_pHalContext);

    /* Do a swap */
    GPU_DLSwap(s_pHalContext, DLSWAP_FRAME);
    SAMAPP_ENABLE_DELAY();
}

/**
* @brief Display Edgestrips
*
*/
void SAMAPP_Primitives_edgeStrips()
{
    Draw_Text(s_pHalContext, "Example for: Edgestrips");

    App_WrDl_Buffer(s_pHalContext, CLEAR_COLOR_RGB(5, 45, 10));
    App_WrDl_Buffer(s_pHalContext, COLOR_RGB(255, 168, 64));
    App_WrDl_Buffer(s_pHalContext, CLEAR(1, 1, 1));
    App_WrDl_Buffer(s_pHalContext, BEGIN(EDGE_STRIP_R));
    App_WrDl_Buffer(s_pHalContext, VERTEX2F(16 * 16, 16 * 16));
    App_WrDl_Buffer(s_pHalContext,
        VERTEX2F((s_pHalContext->Width * 2 / 3) * 16, (s_pHalContext->Height * 2 / 3) * 16));
    App_WrDl_Buffer(s_pHalContext,
        VERTEX2F((s_pHalContext->Width - 80) * 16, (s_pHalContext->Height - 20) * 16));
    App_WrDl_Buffer(s_pHalContext, DISPLAY());

    /* Download the DL into DL RAM */
    App_Flush_DL_Buffer(s_pHalContext);

    /* Do a swap */
    GPU_DLSwap(s_pHalContext, DLSWAP_FRAME);
    SAMAPP_ENABLE_DELAY();
}

/**
* @brief Scissor sample
*
*/
void SAMAPP_Primitives_scissor()
{
    Draw_Text(s_pHalContext, "Example for: Scissor");

    App_WrDl_Buffer(s_pHalContext, CLEAR(1, 1, 1)); // Clear to black
    App_WrDl_Buffer(s_pHalContext, SCISSOR_XY(40, 20)); // Scissor rectangle top left at (40, 20)
    App_WrDl_Buffer(s_pHalContext, SCISSOR_SIZE(40, 40)); // Scissor rectangle is 40 x 40 pixels
    App_WrDl_Buffer(s_pHalContext, CLEAR_COLOR_RGB(255, 255, 0)); // Clear to yellow
    App_WrDl_Buffer(s_pHalContext, CLEAR(1, 1, 1));
    App_WrDl_Buffer(s_pHalContext, DISPLAY());

    /* Download the DL into DL RAM */
    App_Flush_DL_Buffer(s_pHalContext);

    /* Do a swap */
    GPU_DLSwap(s_pHalContext, DLSWAP_FRAME);
    SAMAPP_ENABLE_DELAY();
}

/**
* @brief Stencil sample
*
*/
void SAMAPP_Primitives_stencil()
{
    Draw_Text(s_pHalContext, "Example for: Stencil");

    int16_t PointSize = 50;
    int16_t DispBtwPoints = 60;

    App_WrDl_Buffer(s_pHalContext, CLEAR(1, 1, 1)); // Clear to black
    App_WrDl_Buffer(s_pHalContext, SCISSOR_XY(40, 20)); // Scissor rectangle top left at (40, 20)
    App_WrDl_Buffer(s_pHalContext, STENCIL_OP(INCR, INCR));
    App_WrDl_Buffer(s_pHalContext, POINT_SIZE(PointSize * 16));
    App_WrDl_Buffer(s_pHalContext, BEGIN(FTPOINTS));
    App_WrDl_Buffer(s_pHalContext,
        VERTEX2II(((s_pHalContext->Width - DispBtwPoints) / 2), (s_pHalContext->Height / 2), 0, 0));
    App_WrDl_Buffer(s_pHalContext,
        VERTEX2II(((s_pHalContext->Width + DispBtwPoints) / 2), (s_pHalContext->Height / 2), 0, 0));
    App_WrDl_Buffer(s_pHalContext, STENCIL_FUNC(EQUAL, 2, 255));
    App_WrDl_Buffer(s_pHalContext, COLOR_RGB(128, 0, 0));
    App_WrDl_Buffer(s_pHalContext,
        VERTEX2II((s_pHalContext->Width / 2), (s_pHalContext->Height / 2), 0, 0));
    App_WrDl_Buffer(s_pHalContext, DISPLAY());

    /* Download the DL into DL RAM */
    App_Flush_DL_Buffer(s_pHalContext);

    /* Do a swap */
    GPU_DLSwap(s_pHalContext, DLSWAP_FRAME);
    SAMAPP_ENABLE_DELAY();
}

/**
* @brief Display Polygons
*
*/
void SAMAPP_Gpu_polygon()
{
    Draw_Text(s_pHalContext, "Example for: Polygons");

    App_WrDl_Buffer(s_pHalContext, CLEAR(1, 1, 1)); // clear screen
    App_WrDl_Buffer(s_pHalContext, COLOR_RGB(255, 0, 0));
    App_WrDl_Buffer(s_pHalContext, STENCIL_OP(INCR, INCR));
    App_WrDl_Buffer(s_pHalContext, COLOR_MASK(0, 0, 0, 0)); //mask all the colors
    App_WrDl_Buffer(s_pHalContext, BEGIN(EDGE_STRIP_L));
    App_WrDl_Buffer(s_pHalContext,
        VERTEX2II((s_pHalContext->Width / 2), (s_pHalContext->Height / 4), 0, 0));
    App_WrDl_Buffer(s_pHalContext,
        VERTEX2II((s_pHalContext->Width * 4 / 5), (s_pHalContext->Height * 4 / 5), 0, 0));
    App_WrDl_Buffer(s_pHalContext,
        VERTEX2II((s_pHalContext->Width / 4), (s_pHalContext->Height / 2), 0, 0));
    App_WrDl_Buffer(s_pHalContext,
        VERTEX2II((s_pHalContext->Width / 2), (s_pHalContext->Height / 4), 0, 0));
    App_WrDl_Buffer(s_pHalContext, END());
    App_WrDl_Buffer(s_pHalContext, COLOR_MASK(1, 1, 1, 1)); //enable all the colors
    App_WrDl_Buffer(s_pHalContext, STENCIL_FUNC(EQUAL, 1, 255));
    App_WrDl_Buffer(s_pHalContext, BEGIN(EDGE_STRIP_L));
    App_WrDl_Buffer(s_pHalContext, VERTEX2F(s_pHalContext->Width * 16, 0));
    App_WrDl_Buffer(s_pHalContext, VERTEX2F(s_pHalContext->Width * 16, s_pHalContext->Height * 16));
    App_WrDl_Buffer(s_pHalContext, END());

    /* Draw lines at the borders to make sure anti aliazing is also done */
    App_WrDl_Buffer(s_pHalContext, STENCIL_FUNC(ALWAYS, 0, 255));
    App_WrDl_Buffer(s_pHalContext, LINE_WIDTH(1 * 16));
    App_WrDl_Buffer(s_pHalContext, COLOR_RGB(0, 0, 0));
    App_WrDl_Buffer(s_pHalContext, BEGIN(LINES));
    App_WrDl_Buffer(s_pHalContext,
        VERTEX2II((s_pHalContext->Width / 2), (s_pHalContext->Height / 4), 0, 0));
    App_WrDl_Buffer(s_pHalContext,
        VERTEX2II((s_pHalContext->Width * 4 / 5), (s_pHalContext->Height * 4 / 5), 0, 0));
    App_WrDl_Buffer(s_pHalContext,
        VERTEX2II((s_pHalContext->Width * 4 / 5), (s_pHalContext->Height * 4 / 5), 0, 0));
    App_WrDl_Buffer(s_pHalContext,
        VERTEX2II((s_pHalContext->Width / 4), (s_pHalContext->Height / 2), 0, 0));
    App_WrDl_Buffer(s_pHalContext,
        VERTEX2II((s_pHalContext->Width / 4), (s_pHalContext->Height / 2), 0, 0));
    App_WrDl_Buffer(s_pHalContext,
        VERTEX2II((s_pHalContext->Width / 2), (s_pHalContext->Height / 4), 0, 0));
    App_WrDl_Buffer(s_pHalContext, END());
    App_WrDl_Buffer(s_pHalContext, DISPLAY());

    /* Download the DL into DL RAM */
    App_Flush_DL_Buffer(s_pHalContext);

    /* Do a swap */
    GPU_DLSwap(s_pHalContext, DLSWAP_FRAME);
    SAMAPP_ENABLE_DELAY();
}

/**
* @brief Display a cube
*
*/
void SAMAPP_Gpu_cube()
{
    uint32_t points[6 * 5], x, y, i, z;
    int16_t xoffset, yoffset, CubeEdgeSz;

    // color vertices
    uint16_t colors[6][3] = { 255,0,0,
        255,0,150,
        0,255,0,
        110,120,110,
        0,0,255,
        128,128,0
    };

    // Cube dimention is of 100*100*100
    CubeEdgeSz = 100;
    xoffset = (s_pHalContext->Width / 2 - CubeEdgeSz); 
    yoffset = ((s_pHalContext->Height - CubeEdgeSz) / 2);

    // xy plane(front)
    points[0] = VERTEX2F(xoffset * 16, yoffset * 16);
    points[1] = VERTEX2F((xoffset + CubeEdgeSz) * 16, yoffset * 16);
    points[2] = VERTEX2F((xoffset + CubeEdgeSz) * 16, (yoffset + CubeEdgeSz) * 16);
    points[3] = VERTEX2F(xoffset * 16, (yoffset + CubeEdgeSz) * 16);
    points[4] = points[0];

    //yz plane (left)
    x = (xoffset + (CubeEdgeSz / 2));            //     xoff+w/2
    y = (yoffset - (CubeEdgeSz / 2));            //     yoff-h/2

    points[5] = points[0];
    points[6] = VERTEX2F(x * 16, y * 16);
    points[7] = VERTEX2F(x * 16, (y + CubeEdgeSz) * 16);
    points[8] = points[3];
    points[9] = points[5];

    //xz plane(top)
    points[10] = points[0];
    points[11] = points[1];
    points[12] = VERTEX2F((x + CubeEdgeSz) * 16, (y) * 16);
    points[13] = points[6];
    points[14] = points[10];

    //xz plane(bottom)
    points[15] = points[3];
    points[16] = points[2];
    points[17] = VERTEX2F((x + CubeEdgeSz) * 16, (y + CubeEdgeSz) * 16);
    points[18] = points[7];
    points[19] = points[15];

    //yz plane (right)
    points[20] = points[2];
    points[21] = points[17];
    points[22] = points[12];
    points[23] = points[1];
    points[24] = points[20];

    //yz plane (back)
    points[25] = points[6];
    points[26] = points[7];
    points[27] = points[17];
    points[28] = points[12];
    points[29] = points[25];

    App_WrDl_Buffer(s_pHalContext, CLEAR(1, 1, 1));
    App_WrDl_Buffer(s_pHalContext, LINE_WIDTH(16));
    App_WrDl_Buffer(s_pHalContext, CLEAR_COLOR_RGB(255, 255, 255));
    App_WrDl_Buffer(s_pHalContext, CLEAR(1, 1, 1));
    App_WrDl_Buffer(s_pHalContext, COLOR_RGB(255, 255, 255));

    // Draw a cube
    App_WrDl_Buffer(s_pHalContext, STENCIL_OP(INCR, INCR));
    App_WrDl_Buffer(s_pHalContext, COLOR_A(192));
    for (z = 0; z < 6; z++)
    {
        App_WrDl_Buffer(s_pHalContext, CLEAR(0, 1, 1));//clear stencil buffer
        App_WrDl_Buffer(s_pHalContext, COLOR_MASK(0, 0, 0, 0));//mask all the colors and draw one surface
        App_WrDl_Buffer(s_pHalContext, STENCIL_FUNC(ALWAYS, 0, 255));//stencil function to increment all the values
        App_WrDl_Buffer(s_pHalContext, BEGIN(EDGE_STRIP_L));
        for (i = 0; i < 5; i++)
        {
            App_WrDl_Buffer(s_pHalContext, points[z * 5 + i]);
        }
        App_WrDl_Buffer(s_pHalContext, END());
        /* set the color and draw a strip */
        App_WrDl_Buffer(s_pHalContext, COLOR_MASK(1, 1, 1, 1));
        App_WrDl_Buffer(s_pHalContext, STENCIL_FUNC(EQUAL, 1, 255));
        //App_WrDl_Buffer(s_pHalContext,STENCIL_FUNC(EQUAL,(z+1),255));
        App_WrDl_Buffer(s_pHalContext, COLOR_RGB(colors[z][0], colors[z][1], colors[z][2]));
        App_WrDl_Buffer(s_pHalContext, BEGIN(RECTS));
        App_WrDl_Buffer(s_pHalContext, VERTEX2II(xoffset, 0, 0, 0));
        App_WrDl_Buffer(s_pHalContext, VERTEX2II(xoffset + CubeEdgeSz * 2, yoffset + CubeEdgeSz * 2, 0, 0));
        App_WrDl_Buffer(s_pHalContext, END());
    }
    App_WrDl_Buffer(s_pHalContext, DISPLAY());

    /* Download the DL into DL RAM */
    App_Flush_DL_Buffer(s_pHalContext);

    /* Do a swap */
    GPU_DLSwap(s_pHalContext, DLSWAP_FRAME);
    SAMAPP_ENABLE_DELAY();
}

/**
* @brief draw points followed by lines to create 3d ball kind of effect
*
*/
void SAMAPP_Primitives_ballStencil()
{
    Draw_Text(s_pHalContext, "Example for: Display a ball");

    int32_t xball = (s_pHalContext->Width / 2);
    int32_t yball = 120;
    int32_t rball = (s_pHalContext->Width / 8);
    int32_t numpoints = 6;
    int32_t numlines = 8;
    int32_t asize;
    int32_t aradius;
    int32_t gridsize = 20;
    int32_t asmooth;
    int32_t loopflag = 1;
    int32_t dispr = (s_pHalContext->Width - 10);
    int32_t displ = 10;
    int32_t dispa = 10;
    int32_t dispb = (s_pHalContext->Height - 10);
    int32_t xflag = 1;
    int32_t yflag = 1;

    dispr -= ((dispr - displ) % gridsize);
    dispb -= ((dispb - dispa) % gridsize);
    /* write the play sound */
    EVE_Hal_wr16(s_pHalContext, REG_SOUND, 0x50);
    EVE_Hal_wr8(s_pHalContext, REG_VOL_SOUND, 100);
    loopflag = 100;
    while (loopflag-- > 0)
    {
        if (((xball + rball + 2) >= dispr) || ((xball - rball - 2) <= displ))
        {
            xflag ^= 1;
            EVE_Hal_wr8(s_pHalContext, REG_PLAY, 1);
        }
        if (((yball + rball + 8) >= dispb) || ((yball - rball - 8) <= dispa))
        {
            yflag ^= 1;
            EVE_Hal_wr8(s_pHalContext, REG_PLAY, 1);
        }
        if (xflag)
        {
            xball += 2;
        }
        else
        {
            xball -= 2;
        }
        if (yflag)
        {
            yball += 8;
        }
        else
        {
            yball -= 8;
        }

        App_WrDl_Buffer(s_pHalContext, CLEAR_COLOR_RGB(128, 128, 0));
        App_WrDl_Buffer(s_pHalContext, CLEAR(1, 1, 1)); // clear screen
        App_WrDl_Buffer(s_pHalContext, STENCIL_OP(INCR, INCR));
        App_WrDl_Buffer(s_pHalContext, COLOR_RGB(0, 0, 0));
        /* draw grid */
        App_WrDl_Buffer(s_pHalContext, LINE_WIDTH(16));
        App_WrDl_Buffer(s_pHalContext, BEGIN(LINES));
        for (int i = 0; i <= ((dispr - displ) / gridsize); i++)
        {
            App_WrDl_Buffer(s_pHalContext, VERTEX2F((displ + i * gridsize) * 16, dispa * 16));
            App_WrDl_Buffer(s_pHalContext, VERTEX2F((displ + i * gridsize) * 16, dispb * 16));
        }
        for (int i = 0; i <= ((dispb - dispa) / gridsize); i++)
        {
            App_WrDl_Buffer(s_pHalContext, VERTEX2F(displ * 16, (dispa + i * gridsize) * 16));
            App_WrDl_Buffer(s_pHalContext, VERTEX2F(dispr * 16, (dispa + i * gridsize) * 16));
        }
        App_WrDl_Buffer(s_pHalContext, END());
        App_WrDl_Buffer(s_pHalContext, COLOR_MASK(0, 0, 0, 0)); //mask all the colors
        App_WrDl_Buffer(s_pHalContext, POINT_SIZE(rball * 16));
        App_WrDl_Buffer(s_pHalContext, BEGIN(FTPOINTS));
        App_WrDl_Buffer(s_pHalContext, VERTEX2F(xball * 16, yball * 16));
        App_WrDl_Buffer(s_pHalContext, STENCIL_OP(INCR, ZERO));
        App_WrDl_Buffer(s_pHalContext, STENCIL_FUNC(GEQUAL, 1, 255));
        /* one side points */

        for (int i = 1; i <= numpoints; i++)
        {
            asize = i * rball * 2 / (numpoints + 1);
            asmooth = (int16_t) helperSmoothLerp(((float) (1.0 * asize / (2 * rball))), 0,
                (float) (2 * rball));

            if (asmooth > rball)
            {
                //change the offset to -ve
                int32_t tempsmooth;
                tempsmooth = asmooth - rball;
                aradius = (rball * rball + tempsmooth * tempsmooth) / (2 * tempsmooth);
                App_WrDl_Buffer(s_pHalContext, POINT_SIZE(aradius * 16));
                App_WrDl_Buffer(s_pHalContext,
                    VERTEX2F((xball - aradius + tempsmooth) * 16, yball * 16));
            }
            else
            {
                int32_t tempsmooth;
                tempsmooth = rball - asmooth;
                aradius = (rball * rball + tempsmooth * tempsmooth) / (2 * tempsmooth);
                App_WrDl_Buffer(s_pHalContext, POINT_SIZE(aradius * 16));
                App_WrDl_Buffer(s_pHalContext,
                    VERTEX2F((xball + aradius - tempsmooth) * 16, yball * 16));
            }
        }

        App_WrDl_Buffer(s_pHalContext, END());
        App_WrDl_Buffer(s_pHalContext, BEGIN(LINES));
        /* draw lines - line should be at least radius diameter */
        for (int i = 1; i <= numlines; i++)
        {
            asize = (i * rball * 2 / numlines);
            asmooth = (int16_t) helperSmoothLerp(((float) (1.0 * asize / (2 * rball))), 0,
                (float) (2 * rball));
            App_WrDl_Buffer(s_pHalContext, LINE_WIDTH(asmooth * 16));
            App_WrDl_Buffer(s_pHalContext, VERTEX2F((xball - rball) * 16, (yball - rball) * 16));
            App_WrDl_Buffer(s_pHalContext, VERTEX2F((xball + rball) * 16, (yball - rball) * 16));
        }
        App_WrDl_Buffer(s_pHalContext, END());

        App_WrDl_Buffer(s_pHalContext, COLOR_MASK(1, 1, 1, 1)); // enable all the colors
        App_WrDl_Buffer(s_pHalContext, STENCIL_FUNC(ALWAYS, 1, 255));
        App_WrDl_Buffer(s_pHalContext, STENCIL_OP(KEEP, KEEP));
        App_WrDl_Buffer(s_pHalContext, COLOR_RGB(255, 255, 255));
        App_WrDl_Buffer(s_pHalContext, POINT_SIZE(rball * 16));
        App_WrDl_Buffer(s_pHalContext, BEGIN(FTPOINTS));
        App_WrDl_Buffer(s_pHalContext, VERTEX2F((xball - 1) * 16, (yball - 1) * 16));
        App_WrDl_Buffer(s_pHalContext, COLOR_RGB(0, 0, 0)); //shadow
        App_WrDl_Buffer(s_pHalContext, COLOR_A(160));
        App_WrDl_Buffer(s_pHalContext, VERTEX2F((xball + 16) * 16, (yball + 8) * 16));
        App_WrDl_Buffer(s_pHalContext, COLOR_A(255));
        App_WrDl_Buffer(s_pHalContext, COLOR_RGB(255, 255, 255));
        App_WrDl_Buffer(s_pHalContext, VERTEX2F(xball * 16, yball * 16));
        App_WrDl_Buffer(s_pHalContext, COLOR_RGB(255, 0, 0));
        App_WrDl_Buffer(s_pHalContext, STENCIL_FUNC(GEQUAL, 1, 1));
        App_WrDl_Buffer(s_pHalContext, STENCIL_OP(KEEP, KEEP));
        App_WrDl_Buffer(s_pHalContext, VERTEX2F(xball * 16, yball * 16));

        App_WrDl_Buffer(s_pHalContext, END());

        App_WrDl_Buffer(s_pHalContext, DISPLAY());

        /* Download the DL into DL RAM */
        App_Flush_DL_Buffer(s_pHalContext);

        /* Do a swap */
        GPU_DLSwap(s_pHalContext, DLSWAP_FRAME);

        EVE_sleep(30);
    }
}

/**
* @brief demonstrated display of point and text
*
*/
void SAMAPP_Primitives_string()
{
    int16_t hoffset;
    int16_t voffset;

    Draw_Text(s_pHalContext, "Example for: Display a text");

    voffset = (int16_t) ((s_pHalContext->Height - 49) / 2); //the max height of inbuilt font handle 31
    hoffset = (int16_t) ((s_pHalContext->Width - 4 * 60) / 2);

    EVE_Hal_wr32(s_pHalContext, RAM_DL + 0, CLEAR(1, 1, 1)); // clear screen
    EVE_Hal_wr32(s_pHalContext, RAM_DL + 4, BEGIN(BITMAPS)); // start drawing bitmaps

    EVE_Hal_wr32(s_pHalContext, RAM_DL + 8, VERTEX2II(hoffset, voffset, 31, 'B')); // ascii B in font 31
    hoffset += 24;
    EVE_Hal_wr32(s_pHalContext, RAM_DL + 12, VERTEX2II(hoffset, voffset, 31, 'R')); // ascii R
    hoffset += 26;
    EVE_Hal_wr32(s_pHalContext, RAM_DL + 16, VERTEX2II(hoffset, voffset, 31, 'I')); // ascii I
    hoffset += 10;
    EVE_Hal_wr32(s_pHalContext, RAM_DL + 20, VERTEX2II(hoffset, voffset, 31, 'D')); // ascii D
    hoffset += 24;
    EVE_Hal_wr32(s_pHalContext, RAM_DL + 24, VERTEX2II(hoffset, voffset, 31, 'G')); // ascii G
    hoffset += 24;
    EVE_Hal_wr32(s_pHalContext, RAM_DL + 28, VERTEX2II(hoffset, voffset, 31, 'E')); // ascii E
    hoffset += 24;
    EVE_Hal_wr32(s_pHalContext, RAM_DL + 32, VERTEX2II(hoffset, voffset, 31, 'T')); // ascii T
    hoffset += 24;
    EVE_Hal_wr32(s_pHalContext, RAM_DL + 36, VERTEX2II(hoffset, voffset, 31, 'E')); // ascii E
    hoffset += 24;
    EVE_Hal_wr32(s_pHalContext, RAM_DL + 40, VERTEX2II(hoffset, voffset, 31, 'K')); // ascii K
    EVE_Hal_wr32(s_pHalContext, RAM_DL + 44, END());
    EVE_Hal_wr32(s_pHalContext, RAM_DL + 48, DISPLAY()); // display the image

                                                         /* Do a swap */
    GPU_DLSwap(s_pHalContext, DLSWAP_FRAME);
    SAMAPP_ENABLE_DELAY();
}

/**
* @brief Additive blending of points - 1000 points
*
*/
void SAMAPP_Primitives_additiveBlendPoints()
{
    int32_t hoffset;
    int32_t hoffsetdiff;
    int32_t voffset;
    int32_t flagloop = 1;
    int32_t hdiff;
    int32_t vdiff;
    int32_t PointSz;

    Draw_Text(s_pHalContext, "Example for: Additive blending of points");

#define MSVC_PI (3.141592653)
    PointSz = s_pHalContext->Width / 100;
    flagloop = 10;
    hoffsetdiff = s_pHalContext->Width / 160;
    while (flagloop-- > 0)
    {
        /* Download the DL into DL RAM */
        App_Flush_DL_Buffer(s_pHalContext);

        App_WrDl_Buffer(s_pHalContext, CLEAR(1, 1, 1)); // clear screen
        App_WrDl_Buffer(s_pHalContext, COLOR_RGB(20, 91, 20)); // green color for additive blending
        App_WrDl_Buffer(s_pHalContext, BLEND_FUNC(SRC_ALPHA, ONE)); //input is source alpha and destination is whole color
        App_WrDl_Buffer(s_pHalContext, POINT_SIZE(PointSz * 16));
        App_WrDl_Buffer(s_pHalContext, BEGIN(FTPOINTS));

        /* First 100 random values */
        for (int i = 0; i < 100; i++)
        {
            hoffset = random(s_pHalContext->Width);
            voffset = random(s_pHalContext->Height);
            App_WrDl_Buffer(s_pHalContext, VERTEX2F(hoffset * 16, voffset * 16));
        }

        /* next 480 are sine values of two cycles */
        for (int i = 0; i < 160; i++)
        {
            /* i is x offset, y is sinwave */
            hoffset = i * hoffsetdiff;

            voffset = (s_pHalContext->Height / 2)
                + ((int32_t) (s_pHalContext->Height / 2)
                    * Math_Qsin((uint16_t) (65536 * i / (s_pHalContext->Width / 6)) / 65536));

            App_WrDl_Buffer(s_pHalContext, VERTEX2F(hoffset * 16, voffset * 16));
            for (int j = 0; j < 4; j++)
            {
                hdiff = random(PointSz * 6) - (PointSz * 3);
                vdiff = random(PointSz * 6) - (PointSz * 3);
                App_WrDl_Buffer(s_pHalContext,
                    VERTEX2F((hoffset + hdiff) * 16, (voffset + vdiff) * 16));
            }
        }

        App_WrDl_Buffer(s_pHalContext, END());
        App_WrDl_Buffer(s_pHalContext, DISPLAY());

        /* Download the DL into DL RAM */
        App_Flush_DL_Buffer(s_pHalContext);

        /* Do a swap */
        GPU_DLSwap(s_pHalContext, DLSWAP_FRAME);

        EVE_sleep(10);

    }
}

/**
* @brief display text8x8 of abcdefgh
*
*/
void SAMAPP_Primitives_text8x8()
{
    Draw_Text(s_pHalContext, "Example for: Text8x8");

    /* Write the data into RAM_G */
    const uint8_t Text_Array[] = "abcdefgh";
    int32_t String_size;
    int32_t hoffset = 16;
    int32_t voffset = 16;

    String_size = sizeof(Text_Array) - 1;
    EVE_Hal_wrMem(s_pHalContext, RAM_G, Text_Array, String_size);

    /*
    abcdefgh
    abcdefgh
    */

    App_WrDl_Buffer(s_pHalContext, CLEAR(1, 1, 1)); // clear screen
    App_WrDl_Buffer(s_pHalContext, BITMAP_SOURCE(RAM_G));
    App_WrDl_Buffer(s_pHalContext, BITMAP_LAYOUT(TEXT8X8, 1 * 8, 1)); //L1 format, each input data element is in 1 byte size
    App_WrDl_Buffer(s_pHalContext, BITMAP_SIZE(NEAREST, BORDER, REPEAT, 8 * 8, 8 * 2)); //output is 8x8 format - draw 8 characters in horizontal repeated in 2 line

    App_WrDl_Buffer(s_pHalContext, BEGIN(BITMAPS));
    /* Display text 8x8 at hoffset, voffset location */
    App_WrDl_Buffer(s_pHalContext, VERTEX2F(hoffset * 16, voffset * 16));

    /*
    abcdabcdabcdabcd
    efghefghefghefgh
    */
    App_WrDl_Buffer(s_pHalContext, BITMAP_LAYOUT(TEXT8X8, 1 * 4, 2)); //L1 format and each datatype is 1 byte size
    App_WrDl_Buffer(s_pHalContext, BITMAP_SIZE(NEAREST, REPEAT, BORDER, 8 * 16, 8 * 2)); //each character is 8x8 in size -  so draw 32 characters in horizontal and 32 characters in vertical
    hoffset = s_pHalContext->Width / 2;
    voffset = s_pHalContext->Height / 2;
    App_WrDl_Buffer(s_pHalContext, VERTEX2F(hoffset * 16, voffset * 16));
    App_WrDl_Buffer(s_pHalContext, END());

    App_WrDl_Buffer(s_pHalContext, DISPLAY());

    /* Download the DL into DL RAM */
    App_Flush_DL_Buffer(s_pHalContext);

    /* Do a swap */
    GPU_DLSwap(s_pHalContext, DLSWAP_FRAME);
    SAMAPP_ENABLE_DELAY();
}

/**
* @brief display textVGA of random values
*
*/
void SAMAPP_Primitives_textVGA()
{
    Draw_Text(s_pHalContext, "Example for: TextVGA");

    /* Write the data into RAM_G */
    uint16_t Text_Array[160];
    int32_t String_size;
    int32_t hoffset = 32;
    int32_t voffset = 32;

    for (int i = 0; i < 160; i++)
    {
        Text_Array[i] = (uint16_t) random(65536); //within range
    }

    String_size = 160 * 2;
    EVE_Hal_wrMem(s_pHalContext, RAM_G, (uint8_t*) Text_Array, String_size);

    App_WrDl_Buffer(s_pHalContext, CLEAR(1, 1, 1)); // clear screen
    App_WrDl_Buffer(s_pHalContext, BITMAP_SOURCE(RAM_G));

    /* mandatory for textvga as background color is also one of the parameter in textvga format */
    App_WrDl_Buffer(s_pHalContext, BLEND_FUNC(ONE, ZERO));

    //draw 8x8
    App_WrDl_Buffer(s_pHalContext, BITMAP_LAYOUT(TEXTVGA, 2 * 4, 8)); //L1 format, but each input data element is of 2 bytes in size
    App_WrDl_Buffer(s_pHalContext, BITMAP_SIZE(NEAREST, BORDER, BORDER, 8 * 8, 8 * 8)); //output is 8x8 format - draw 8 characters in horizontal and 8 vertical
    App_WrDl_Buffer(s_pHalContext, BEGIN(BITMAPS));
    App_WrDl_Buffer(s_pHalContext, VERTEX2F(hoffset * 16, voffset * 16));
    App_WrDl_Buffer(s_pHalContext, END());

    //draw textvga
    App_WrDl_Buffer(s_pHalContext, BITMAP_LAYOUT(TEXTVGA, 2 * 16, 8)); //L1 format but each datatype is 16bit size
    App_WrDl_Buffer(s_pHalContext, BITMAP_SIZE(NEAREST, BORDER, REPEAT, 8 * 32, 8 * 32)); //8 pixels per character and 32 rows/colomns
    App_WrDl_Buffer(s_pHalContext, BEGIN(BITMAPS));
    hoffset = s_pHalContext->Width / 2;
    voffset = s_pHalContext->Height / 2;
    /* Display textvga at hoffset, voffset location */
    App_WrDl_Buffer(s_pHalContext, VERTEX2F(hoffset * 16, voffset * 16));
    App_WrDl_Buffer(s_pHalContext, END());

    App_WrDl_Buffer(s_pHalContext, DISPLAY());

    /* Download the DL into DL RAM */
    App_Flush_DL_Buffer(s_pHalContext);

    /* Do a swap */
    GPU_DLSwap(s_pHalContext, DLSWAP_FRAME);
    SAMAPP_ENABLE_DELAY();
}

/**
* @brief usage of additive blending - draw 3 Gs
*
*/
void SAMAPP_Primitives_additiveBlendText()
{
    Draw_Text(s_pHalContext, "Example for: Additive blending");

    App_WrDl_Buffer(s_pHalContext, CLEAR(1, 1, 1)); // clear screen
    App_WrDl_Buffer(s_pHalContext, BEGIN(BITMAPS));
    App_WrDl_Buffer(s_pHalContext, VERTEX2II(50, 30, 31, 0x47));
    App_WrDl_Buffer(s_pHalContext, COLOR_A(128));
    App_WrDl_Buffer(s_pHalContext, VERTEX2II(58, 38, 31, 0x47));
    App_WrDl_Buffer(s_pHalContext, COLOR_A(64));
    App_WrDl_Buffer(s_pHalContext, VERTEX2II(66, 46, 31, 0x47));
    App_WrDl_Buffer(s_pHalContext, END());
    App_WrDl_Buffer(s_pHalContext, DISPLAY());

    /* Download the DL into DL RAM */
    App_Flush_DL_Buffer(s_pHalContext);

    /* Do a swap */
    GPU_DLSwap(s_pHalContext, DLSWAP_FRAME);
    SAMAPP_ENABLE_DELAY();
}

/**
* @brief Usage of macro
*
*/
void SAMAPP_Primitives_macroUsage()
{
    Draw_Text(s_pHalContext, "Example for: Macro");

    int32_t xoffset;
    int32_t yoffset;
    int32_t xflag = 1;
    int32_t yflag = 1;
    int32_t flagloop = 1;
    const SAMAPP_Bitmap_header_t* p_bmhdr;

    xoffset = s_pHalContext->Width / 3;
    yoffset = s_pHalContext->Height / 2;

    /* First write a valid macro instruction into macro0 */
    EVE_Hal_wr32(s_pHalContext, REG_MACRO_0, VERTEX2F(xoffset * 16, yoffset * 16));
    /* update lena face as bitmap 0 */

    p_bmhdr = &SAMAPP_Bitmap_RawData_Header[0];
    /* Copy raw data into address 0 followed by generation of bitmap */

    Ftf_Write_File_nBytes_To_RAM_G(s_pHalContext, TEST_DIR "\\SAMAPP_Bitmap_RawData.bin", RAM_G,
        p_bmhdr->Stride * p_bmhdr->Height, p_bmhdr->Arrayoffset);

    App_WrDl_Buffer(s_pHalContext, CLEAR(1, 1, 1)); // clear screen
    App_WrDl_Buffer(s_pHalContext, BITMAP_SOURCE(RAM_G));
    App_WrDl_Buffer(s_pHalContext,
        BITMAP_LAYOUT(p_bmhdr->Format, p_bmhdr->Stride, p_bmhdr->Height));
    App_WrDl_Buffer(s_pHalContext,
        BITMAP_SIZE(NEAREST, BORDER, BORDER, p_bmhdr->Width, p_bmhdr->Height));
    App_WrDl_Buffer(s_pHalContext, BEGIN(BITMAPS)); // start drawing bitmaps
    App_WrDl_Buffer(s_pHalContext, MACRO(0)); // draw the image at (100,120)
    App_WrDl_Buffer(s_pHalContext, END());
    App_WrDl_Buffer(s_pHalContext, DISPLAY());

    /* Download the DL into DL RAM */
    App_Flush_DL_Buffer(s_pHalContext);

    /* Do a swap */
    GPU_DLSwap(s_pHalContext, DLSWAP_FRAME);
    flagloop = 300;
    while (flagloop-- > 0)
    {
        if (((xoffset + p_bmhdr->Width) >= s_pHalContext->Width) || (xoffset <= 0))
        {
            xflag ^= 1;
        }
        if (((yoffset + p_bmhdr->Height) >= s_pHalContext->Height) || (yoffset <= 0))
        {
            yflag ^= 1;
        }
        if (xflag)
        {
            xoffset++;
        }
        else
        {
            xoffset--;
        }
        if (yflag)
        {
            yoffset++;
        }
        else
        {
            yoffset--;
        }
        /*  update just the macro */
        EVE_Hal_wr32(s_pHalContext, REG_MACRO_0, VERTEX2F(xoffset * 16, yoffset * 16));
        EVE_sleep(10);
    }
}

/**
* @brief API to demonstrate for CMD_CALIBRATESUB
*
*/
void SAMAPP_Primitives_calibratesub() {
#if EVE_SUPPORT_GEN == EVE4
    Draw_Text(s_pHalContext, "Example for: CMD_CALIBRATESUB");

    uint16_t clb_x = 20;
    uint16_t clb_y = 20;
    uint16_t clb_w = (uint16_t)(s_pHalContext->Width / 2);
    uint16_t clb_h = (uint16_t)(s_pHalContext->Height / 2);
    uint16_t format = RGB565;
    uint16_t w = 800;
    uint16_t h = 480;
    uint8_t type = LOADIMAGE;

    Display_Start(s_pHalContext);
    Gpu_Hal_LoadImageToMemory(s_pHalContext, TEST_DIR "\\Mountain_800x480_RGB565_Converted.png", RAM_G, type);
    EVE_CoCmd_setBitmap(s_pHalContext, RAM_G, format, w, h);
    //Start drawing bitmap
    EVE_Cmd_wr32(s_pHalContext, SAVE_CONTEXT());
    EVE_Cmd_wr32(s_pHalContext, BEGIN(BITMAPS));
    EVE_Cmd_wr32(s_pHalContext, VERTEX2II(0, 0, 0, 0));
    EVE_Cmd_wr32(s_pHalContext, END());

    EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0, 255, 255));
    EVE_CoCmd_text(s_pHalContext, clb_x + 260, clb_y + clb_h / 2, 30, 0,
        "Please touch on screen");

    EVE_CoCmd_fgColor(s_pHalContext, 0xb90007);
    EVE_CoCmd_calibrateSub(s_pHalContext, clb_x, clb_y, clb_w, clb_h);

    EVE_Cmd_wr32(s_pHalContext, RESTORE_CONTEXT());
    EVE_Cmd_waitFlush(s_pHalContext);
#endif
}

/**
* @brief API to demonstrate CMD_TESTCARD
*
*/
void SAMAPP_Primitives_testcard() {
#if EVE_SUPPORT_GEN == EVE4
    Draw_Text(s_pHalContext, "Example for: CMD_TESTCARD");

    EVE_CoCmd_testCard(s_pHalContext);
    EVE_Cmd_waitFlush(s_pHalContext);
    EVE_sleep(2000);
#endif
}

/**
* @brief API to demonstrate CMD_GRADIENTA with transparency
*
*/
void SAMAPP_gradientA()
{
#if defined (BT81X_ENABLE) && (defined(MSVC_PLATFORM) || defined(BT8XXEMU_PLATFORM))
    Draw_Text(s_pHalContext, "Example for: CMD_GRADIENTA with transparency");

    /*************************************************************************/
    /* Below code demonstrates the usage of gradient function. Gradient func */
    /* can be used to construct three effects - horizontal, vertical and     */
    /* diagonal effects.                                                      */
    /*************************************************************************/
    /*GradientA */
    int16_t wScissor;
    int16_t hScissor;
    int16_t xOffset;
    int16_t yOffset;

    wScissor = (int16_t) ((s_pHalContext->Width - 4 * 10) / 3);
    hScissor = (int16_t) ((s_pHalContext->Height - 3 * 20) / 2);
    xOffset = 10;
    yOffset = 20;
    /* Draw gradient  */

    EVE_CoCmd_dlStart(s_pHalContext);
    EVE_Cmd_wr32(s_pHalContext, CLEAR_COLOR_RGB(64, 64, 64));
    EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1));
    EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 255, 255));
    EVE_Cmd_wr32(s_pHalContext, SCISSOR_SIZE(wScissor, hScissor));
    /* Horizontal gradient effect */
    EVE_Cmd_wr32(s_pHalContext, SCISSOR_XY(xOffset, yOffset)); //clip the display
    EVE_CoCmd_gradientA(s_pHalContext, xOffset, yOffset, 0x80808080, (xOffset + wScissor), yOffset,
        0xffffff00);
    /* Vertical gradient effect */
    xOffset += wScissor + 10;
    EVE_Cmd_wr32(s_pHalContext, SCISSOR_XY(xOffset, yOffset)); //clip the display
    EVE_CoCmd_gradientA(s_pHalContext, xOffset, yOffset, 0x80ff0000, xOffset, (yOffset + hScissor),
        0xff00ff00);
    /* diagonal gradient effect */
    xOffset += wScissor + 10;
    EVE_Cmd_wr32(s_pHalContext, SCISSOR_XY(xOffset, yOffset)); //clip the display
    EVE_CoCmd_gradientA(s_pHalContext, xOffset, yOffset, 0x80800000, (xOffset + wScissor),
        (yOffset + hScissor), 0xffffffff);
    /* Diagonal gradient with text info */
    xOffset = 10;
    yOffset += hScissor + 20;
    EVE_Cmd_wr32(s_pHalContext, SCISSOR_SIZE(wScissor, 30));
    EVE_Cmd_wr32(s_pHalContext, SCISSOR_XY(xOffset, (yOffset + hScissor / 2 - 15))); //clip the display
    EVE_CoCmd_gradientA(s_pHalContext, xOffset, (yOffset + hScissor / 2 - 15), 0x00606060,
        (xOffset + wScissor), (yOffset + hScissor / 2 + 15), 0xff404080);
    EVE_CoCmd_text(s_pHalContext, (xOffset + wScissor / 2), (yOffset + hScissor / 2), 28,
        OPT_CENTER, "GradientA"); //text info

                                  /* Draw horizontal, vertical and diagonal with alpha */
    xOffset += wScissor + 10;
    EVE_Cmd_wr32(s_pHalContext, SCISSOR_SIZE(wScissor, hScissor));
    EVE_Cmd_wr32(s_pHalContext, SCISSOR_XY(xOffset, yOffset)); //clip the display
    EVE_CoCmd_gradientA(s_pHalContext, xOffset, yOffset, 0x80808080, (xOffset + wScissor), yOffset,
        0xffffff00);
    wScissor -= 30;
    hScissor -= 30;
    EVE_Cmd_wr32(s_pHalContext, SCISSOR_SIZE(wScissor, hScissor));
    xOffset += 15;
    yOffset += 15;
    EVE_Cmd_wr32(s_pHalContext, SCISSOR_XY(xOffset, yOffset)); //clip the display
    EVE_CoCmd_gradientA(s_pHalContext, xOffset, yOffset, 0x80800000, xOffset, (yOffset + hScissor),
        0xffffffff);
    wScissor -= 30;
    hScissor -= 30;
    EVE_Cmd_wr32(s_pHalContext, SCISSOR_SIZE(wScissor, hScissor));
    xOffset += 15;
    yOffset += 15;
    EVE_Cmd_wr32(s_pHalContext, SCISSOR_XY(xOffset, yOffset)); //clip the display
    EVE_CoCmd_gradientA(s_pHalContext, xOffset, yOffset, 0x80606060, (xOffset + wScissor),
        (yOffset + hScissor), 0xFF404080);

    /* Display the text wrt gradient */
    wScissor = (int16_t) ((s_pHalContext->Width - 4 * 10) / 3);
    hScissor = (int16_t) ((s_pHalContext->Height - 3 * 20) / 2);
    xOffset = 10 + wScissor / 2;
    yOffset = 20 + hScissor + 5;
    EVE_Cmd_wr32(s_pHalContext, SCISSOR_XY(0, 0)); //set to default values
#ifdef FT81X_ENABLE
    EVE_Cmd_wr32(s_pHalContext, SCISSOR_SIZE(2048, 2048));
#else
    EVE_Cmd_wr32(s_pHalContext, SCISSOR_SIZE(512, 512));
#endif
    EVE_CoCmd_text(s_pHalContext, xOffset, yOffset, 26, OPT_CENTER, "Horizontal grad"); //text info
    xOffset += wScissor + 10;
    EVE_CoCmd_text(s_pHalContext, xOffset, yOffset, 26, OPT_CENTER, "Vertical grad"); //text info
    xOffset += wScissor + 10;
    EVE_CoCmd_text(s_pHalContext, xOffset, yOffset, 26, OPT_CENTER, "Diagonal grad"); //text info

    EVE_Cmd_wr32(s_pHalContext, DISPLAY());
    EVE_CoCmd_swap(s_pHalContext);

    /* Wait till coprocessor completes the operation */
    EVE_Cmd_waitFlush(s_pHalContext);
    EVE_sleep(2000);
#endif // defined (BT81X_ENABLE) && (defined(MSVC_PLATFORM) || defined(BT8XXEMU_PLATFORM))
}

/**
* @brief API to demonstrate the use of transfer commands
*
*/
void SAMAPP_CoPro_appendCmds()
{
    uint32_t AppendCmds[16];
    int16_t xoffset;
    int16_t yoffset;

    Draw_Text(s_pHalContext, "Example for: Appending DL command");

    /*************************************************************************/
    /* Below code demonstrates the usage of coprocessor append command - to append any*/
    /* mcu specific graphics commands to coprocessor generated graphics commands      */
    /*************************************************************************/

    /* Bitmap construction by MCU - display lena at 200x60 offset */
    /* Construct the bitmap data to be copied in the temp buffer then by using
    coprocessor append command copy it into graphics processor DL memory */
    xoffset = (int16_t) ((s_pHalContext->Width - SAMAPP_Bitmap_RawData_Header[0].Width) / 2);
    yoffset = (int16_t) ((s_pHalContext->Height / 3) - SAMAPP_Bitmap_RawData_Header[0].Height / 2);

    EVE_Cmd_wr32(s_pHalContext, CMD_DLSTART);
    AppendCmds[0] = CLEAR_COLOR_RGB(255, 0, 0);
    AppendCmds[1] = CLEAR(1, 1, 1);
    AppendCmds[2] = COLOR_RGB(255, 255, 255);
    AppendCmds[3] = BEGIN(BITMAPS);
    AppendCmds[4] = BITMAP_SOURCE(0);
    AppendCmds[5] = BITMAP_LAYOUT(SAMAPP_Bitmap_RawData_Header[0].Format,
        SAMAPP_Bitmap_RawData_Header[0].Stride, SAMAPP_Bitmap_RawData_Header[0].Height);
    AppendCmds[6] = BITMAP_SIZE(BILINEAR, BORDER, BORDER, SAMAPP_Bitmap_RawData_Header[0].Width,
        SAMAPP_Bitmap_RawData_Header[0].Height);
    AppendCmds[7] = VERTEX2F(xoffset * 16, yoffset * 16);
    AppendCmds[8] = END();

    /* Download the bitmap data*/
    Ftf_Write_File_nBytes_To_RAM_G(s_pHalContext, TEST_DIR "\\SAMAPP_Bitmap_RawData.bin", RAM_G,
        SAMAPP_Bitmap_RawData_Header[0].Stride * SAMAPP_Bitmap_RawData_Header[0].Height,
        SAMAPP_Bitmap_RawData_Header[0].Arrayoffset);

    /* Download the DL data constructed by the MCU to location 40*40*2 in sram */
    EVE_Hal_wrMem(s_pHalContext,
        RAM_G + SAMAPP_Bitmap_RawData_Header[0].Stride * SAMAPP_Bitmap_RawData_Header[0].Height,
        (uint8_t*) AppendCmds, 9 * 4);

    /* Call the append api for copying the above generated data into graphics processor
    DL commands are stored at location 40*40*2 offset from the starting of the sram */
    EVE_CoCmd_append(s_pHalContext,
        RAM_G + SAMAPP_Bitmap_RawData_Header[0].Stride * SAMAPP_Bitmap_RawData_Header[0].Height, 9 * 4);
    /*  Display the text information */
    EVE_CoCmd_fgColor(s_pHalContext, 0xffff00);
    xoffset -= 50;
    yoffset += 40;
    EVE_CoCmd_text(s_pHalContext, xoffset, yoffset, 26, 0, "Display bitmap by Append");
    EVE_Cmd_wr32(s_pHalContext, DISPLAY());
    EVE_CoCmd_swap(s_pHalContext);
    /* Download the commands into fifo */
    EVE_Cmd_waitFlush(s_pHalContext);
    EVE_sleep(2000);
}

void SAMAPP_Primitives() 
{
    SAMAPP_Primitives_points();
    SAMAPP_Primitives_lines();
    SAMAPP_Primitives_rectangles();
    SAMAPP_Primitives_bitmap();
    SAMAPP_Primitives_bitmapPaletteBackground();
    SAMAPP_Primitives_bitmapPalette();
    SAMAPP_Primitives_barGraph();
    SAMAPP_Primitives_lineStrips();
    SAMAPP_Primitives_edgeStrips();
    SAMAPP_Primitives_scissor();
    SAMAPP_Primitives_stencil();
    SAMAPP_Gpu_polygon();
    SAMAPP_Gpu_cube();
    SAMAPP_Primitives_ballStencil();
    SAMAPP_Primitives_string();
    SAMAPP_Primitives_additiveBlendPoints();
    SAMAPP_Primitives_text8x8();
    SAMAPP_Primitives_textVGA();
    SAMAPP_Primitives_additiveBlendText();
    SAMAPP_Primitives_macroUsage();
    SAMAPP_Primitives_calibratesub();
    SAMAPP_Primitives_testcard();
    SAMAPP_gradientA();
    SAMAPP_CoPro_appendCmds();
}


