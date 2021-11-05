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
#include "Touch.h"

#define SAMAPP_DELAY_NEXT      EVE_sleep(2000);

static EVE_HalContext s_halContext;
static EVE_HalContext* s_pHalContext;
void SAMAPP_Touch();
static int32_t Volume;

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
        "This sample demonstrate the using of touch", 
        "",
        ""
    }; 

    while (TRUE) {
        WelcomeScreen(s_pHalContext, info);

        SAMAPP_Touch();

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

SAMAPP_Logo_Img_t Main_Icons[1] = {
    { TEST_DIR "\\tile3.bin", ImH, RGB565, NEAREST, ImW, ImH, ImW * 2, 0L }, 
};

/**
* @brief Transfer file to RAM_G
*
* @param add Address on RAM_G
* @param sectors Number of sector
* @param afile File pointer
*/
void helperAppendToRAMG(uint32_t add, uint8_t sectors)
{
    uint8_t pbuff[512];
    for (int z = 0; z < sectors; z++)
    {
        FileIO_File_Read(pbuff, 512);
        EVE_Hal_wrMem(s_pHalContext, add, pbuff, 512L);
        add += 512;
    }
}

/**
* @brief Load image into Coprocessor by CMD_INFLATE
*
* @param address
* @param filename
*/
void helperLoadInflatImage(uint32_t address, const char* filename)
{
    FILE* afile;
    uint32_t ftsize = 0;
    uint8_t pbuff[8192];
    uint16_t blocklen;

    EVE_Cmd_wr32(s_pHalContext, CMD_INFLATE);
    EVE_Cmd_wr32(s_pHalContext, address * 1024L);
    afile = fopen(filename, "rb"); // read Binary (rb)
    if (!afile)
    {
        return;
    }
    fseek(afile, 0, SEEK_END);
    ftsize = ftell(afile);
    fseek(afile, 0, SEEK_SET);
    while (ftsize > 0)
    {
        blocklen = ftsize > 8192 ? 8192 : (uint16_t) ftsize;
        fread(pbuff, 1, blocklen, afile);/* copy the data into pbuff and then transfter it to command buffer */
        ftsize -= blocklen;
        EVE_Cmd_wrMem(s_pHalContext, pbuff, blocklen);/* copy data continuously into command memory */
    }
    fclose(afile);/* close the opened jpg file */
}

/**
* @brief API to demonstrate moving rectangle
*
* @param BRy Deprecated
* @param MovingRy Moving rectangle y
* @param EndPtReach Is end moving
* @return int16_t Moving rectangle y
*/
int16_t helperMovingRect(int16_t MovingRy, uint8_t EndPtReach)
{
#if (EVE_CHIPID & 0x01) == 0x01 // capacity EVE
    if (MovingRy <= 0)
    {
        EndPtReach = 0;
        MovingRy = 1;
    }

    if (EndPtReach == 1 && MovingRy > 0)
        MovingRy -= 1; //the smaller rectangles are moved behind
    else if (EndPtReach == 0)
        MovingRy += 2; //the smaller rectangles are moved forward slightly faster
    return MovingRy;
#endif // capacity EVE
}

/**
* @brief Calculate rectangle limits and positions
*
* @param context BouncingSquares context
* @param Arrayno Rectangle id
*/
void helperRectangleCalc(SAMAPP_BouncingSquares_t* context, uint8_t Arrayno)
{
#if (EVE_CHIPID & 0x01) == 0x01 // capacity EVE
    uint8_t Arr;
    int16_t MovingRy1;
    int16_t leap = 0;

    if (context->RectNo[Arrayno] == 1)
    {
        Arr = Arrayno;
        //the limits for the smaller rectangles forward and backward movement is set here
        if (context->My[Arr] == 0 && (context->My[Arr] + 25) < context->BRy[Arr])
            context->E[Arr] = 0; //inc
        else if (context->My[Arr] + 25 >= context->BRy[Arr])
            context->E[Arr] = 1; //dec

                                 // the smaller rectangles are moved accordingly according to the flags set above ion this function call
        MovingRy1 = helperMovingRect(context->My[Arr], context->E[Arr]);

        if (context->BRy[Arr] == 0)
            MovingRy1 = 4;
        context->My[Arr] = MovingRy1;

        if (context->My[Arr] > (context->BRy[Arr] - 15))
        {
            leap = context->My[Arr] - context->BRy[Arr];
            context->My[Arr] = context->My[Arr] - (leap + 25);
        }
    }
#endif // capacity EVE
}

/**
* @brief Setup logo
*
* @param sptr Logo image
* @param num Number of image
*/
void helperLogoIntialsetup(const SAMAPP_Logo_Img_t sptr[], uint8_t num)
{
    uint8_t z;
    for (z = 0; z < num; z++)
    {
        helperLoadInflatImage(sptr[z].gram_address, sptr[z].name);
    }

    EVE_CoCmd_dlStart(s_pHalContext); // start
    EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1));
    for (z = 0; z < num; z++)
    {
        EVE_Cmd_wr32(s_pHalContext, BITMAP_HANDLE(z));
        EVE_Cmd_wr32(s_pHalContext, BITMAP_SOURCE(sptr[z].gram_address * 1024L));
        EVE_Cmd_wr32(s_pHalContext,
            BITMAP_LAYOUT(sptr[z].image_format, sptr[z].linestride, sptr[z].image_height));
        EVE_Cmd_wr32(s_pHalContext,
            BITMAP_SIZE(sptr[z].filter, BORDER, BORDER, sptr[z].linestride / 2,
                sptr[z].image_height));

    }
    EVE_Cmd_wr32(s_pHalContext, DISPLAY());
    EVE_CoCmd_swap(s_pHalContext);
    EVE_Cmd_waitFlush(s_pHalContext);
}

/**
* @brief Beginning BouncingCircle section
*
* @param C1 Point size
* @param R Deprecated
* @param G Deprecated
* @param B Deprecated
*/
void helperConcentricCircles(float C1, uint16_t R, uint16_t G, uint16_t B)
{
#if (EVE_CHIPID & 0x01) == 0x01 // capacity EVE
    EVE_Cmd_wr32(s_pHalContext, CLEAR_COLOR_A(0));

    /* Using ColorMask to disable color buffer updates, and
    set the BlendFunc to a value that writes     incoming alpha
    directly into the alpha buffer, by specifying a source blend factor
    of ONE */

    EVE_Cmd_wr32(s_pHalContext, COLOR_MASK(0, 0, 0, 1));
    EVE_Cmd_wr32(s_pHalContext, BLEND_FUNC(ONE, ONE_MINUS_SRC_ALPHA));

    /*Draw the Outer circle */

    EVE_Cmd_wr32(s_pHalContext, BEGIN(FTPOINTS));
    EVE_Cmd_wr32(s_pHalContext, POINT_SIZE((uint16_t )(C1 * 16))); //outer circle
    EVE_Cmd_wr32(s_pHalContext, VERTEX2II(240, 136, 0, 0));

    /* Draw the inner circle in a blend mode that clears any drawn
    pixels to zero, so the source blend factor is ZERO */
    EVE_Cmd_wr32(s_pHalContext, BLEND_FUNC(ZERO, ONE_MINUS_SRC_ALPHA));

    EVE_Cmd_wr32(s_pHalContext, POINT_SIZE((uint16_t )((C1 - 2) * 16))); //inner circle
    EVE_Cmd_wr32(s_pHalContext, VERTEX2II(240, 136, 0, 0));

    /*Enable the color Mask and the source blend factor is set to DST ALPHA, so the
    transparency values come from the alpha buffer*/

    EVE_Cmd_wr32(s_pHalContext, COLOR_MASK(1, 1, 1, 0));

    EVE_Cmd_wr32(s_pHalContext, BLEND_FUNC(DST_ALPHA, ONE));

    /* draw the outer circle again with the color mask enabled andd the blend factor
    is set to SRC_ALPHA */
    EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(R, G, B));
    EVE_Cmd_wr32(s_pHalContext, POINT_SIZE((uint16_t )(C1 * 16)));
    EVE_Cmd_wr32(s_pHalContext, VERTEX2II(240, 136, 0, 0));

    EVE_Cmd_wr32(s_pHalContext, BLEND_FUNC(SRC_ALPHA, ONE_MINUS_SRC_ALPHA));

    EVE_Cmd_wr32(s_pHalContext, END());
#endif // capacity EVE
}

/**
* @brief Draw touch points
*
* @param C1X Point X
* @param C1Y Point Y
* @param i Point number
*/
void helperTouchPoints(int16_t C1X, int16_t C1Y, uint8_t i)
{
#if (EVE_CHIPID & 0x01) == 0x01 // capacity EVE
    /* Draw the five white circles for the Touch areas with their rescpective numbers*/
    EVE_Cmd_wr32(s_pHalContext, BEGIN(FTPOINTS));
    EVE_Cmd_wr32(s_pHalContext, POINT_SIZE((14) * 16));
    EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 255, 255));
    EVE_Cmd_wr32(s_pHalContext, VERTEX2II(C1X, C1Y, 0, 0));
    EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(155, 155, 0));
    EVE_CoCmd_number(s_pHalContext, C1X, C1Y, 29, OPT_CENTERX | OPT_CENTERY, i);
#endif // capacity EVE
}

/**
* @brief Draw plots
*
*/
void helperPlotXY()
{
#if (EVE_CHIPID & 0x01) == 0x01 // capacity EVE
    uint8_t i = 0;
    uint16_t PlotHt = 0;
    uint16_t PlotWth = 0;
    uint16_t X = 0;
    uint16_t Y = 0;

    PlotHt = (uint16_t) (s_pHalContext->Height / 10);
    PlotWth = (uint16_t) (s_pHalContext->Width / 10);

    EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(36, 54, 125));
    /* Horizontal Lines */
    for (i = 1; i < 11; i++)
    {
        Y = i * PlotHt;
        EVE_Cmd_wr32(s_pHalContext, BEGIN(LINES));
        EVE_Cmd_wr32(s_pHalContext, LINE_WIDTH(1 * 16));
        EVE_Cmd_wr32(s_pHalContext, VERTEX2F(0, Y * 16));
        EVE_Cmd_wr32(s_pHalContext, VERTEX2F(s_pHalContext->Width * 16, Y * 16));
    }
    /* Vertical Lines */
    for (i = 1; i < 11; i++)
    {
        X = i * PlotWth;
        EVE_Cmd_wr32(s_pHalContext, BEGIN(LINES));
        EVE_Cmd_wr32(s_pHalContext, LINE_WIDTH(1 * 16));
        EVE_Cmd_wr32(s_pHalContext, VERTEX2F(X * 16, 0));
        EVE_Cmd_wr32(s_pHalContext, VERTEX2F(X * 16, s_pHalContext->Height * 16));
    }
    EVE_Cmd_wr32(s_pHalContext, END());
#endif // capacity EVE
}

/**
* @brief check which circle has been touched based on the coordinates
*
* @param context BouncingSquares context
* @param val Touch value
* @param TouchNum Touch number
* @param i Circle number
*/
void helperCheckCircleTouchCood(SAMAPP_BouncingCircles_t* context, int32_t val, uint8_t TouchNum,
    uint8_t i)
{
#if (EVE_CHIPID & 0x01) == 0x01 // capacity EVE
    double CX = 0;
    double CY = 0;

    uint8_t AllClear = 0;

    if ((val >> 16) == -32768)
    {
        context->TN[TouchNum].F[i] = 0;
        return;
    }

    CX = (val >> 16);
    CY = (val & 0xffff);

    for (int8_t j = 0; j < NO_OF_CIRCLE; j++)
    {
        if (context->TN[TouchNum].F[j] == 0)
        {
            if (AllClear != 10)
                AllClear = j;
        }
        else
            AllClear = 10;
    }

    if (AllClear != 10)
        AllClear = 1;

    if (AllClear == 1 && context->TN[TouchNum].F[i] != 1 && (CX > (context->C1X[i] - 15))
        && (CX < (context->C1X[i] + 15)) && (CY > (context->C1Y[i] - 30))
        && (CY < context->C1Y[i] + 30))
    {
        context->C1X[i] = (float) CX;
        context->C1Y[i] = (float) CY;
        context->TN[TouchNum].F[i] = 1;
    }

    if (context->TN[TouchNum].F[i] == 1)
    {
        context->C1X[i] = (float) CX;
        context->C1Y[i] = (float) CY;
    }
#endif // capacity EVE
}

/**
* @brief calculate the radius of each circle according to the touch
*
* @param context BouncingSquares context
* @param X Touch X
* @param Y Touch Y
* @param Val Circle number
* @return uint16_t radius of circle
*/
uint16_t helperCirclePlot(SAMAPP_BouncingCircles_t* context, uint16_t X, uint16_t Y, uint8_t Val)
{
#if (EVE_CHIPID & 0x01) == 0x01 // capacity EVE
    double Xsq1[NO_OF_CIRCLE];
    double Ysq1[NO_OF_CIRCLE];
    Xsq1[Val] = (X - (s_pHalContext->Width / 2)) * (X - (s_pHalContext->Width / 2));
    Ysq1[Val] = (Y - (s_pHalContext->Height / 2)) * (Y - (s_pHalContext->Height / 2));
    context->Tsq1[Val] = (float) (Xsq1[Val] + Ysq1[Val]);
    context->Tsq1[Val] = (float) sqrt(context->Tsq1[Val]);
    return (uint16_t) context->Tsq1[Val];
#endif // capacity EVE
}

/**
* @brief Store touches to context
*
* @param context BouncingSquares context
* @param Touchval Touch value
* @param TouchNo Touch number
*/
void helperStoreTouch(SAMAPP_BouncingCircles_t* context, int32_t Touchval, uint8_t TouchNo)
{
#if (EVE_CHIPID & 0x01) == 0x01 // capacity EVE
    if (Touchval >> 16 != -32768)
    {
        context->TouchX[TouchNo] = (float) (Touchval >> 16);
        context->TouchY[TouchNo] = (float) (Touchval & 0xffff);
    }
#endif // capacity EVE
}

/**
* @brief Linear function
*
* @param p1
* @param p2
* @param t
* @param rate
* @return int16_t
*/
int16_t helperLinear(float p1, float p2, uint16_t t, uint16_t rate)
{
#if (EVE_CHIPID & 0x01) == 0x01 // capacity EVE
    float st = (float) t / rate;
    return (int16_t) (p1 + (st * (p2 - p1)));
#endif // capacity EVE
}

/**
* @brief Beginning MovingPoints section
*
* @param k Point set
* @param i POint number
*/
void helperColorSelection(int16_t k, int16_t i)
{
#if (EVE_CHIPID & 0x01) == 0x01 // capacity EVE
    if (k == 0)
    {
        if (i & 1)
            EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(128, 0, 255)); //purple
        else
            EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 43, 149)); //pink
    }
    if (k == 1)
    {
        if (i & 1)
            EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 0, 0)); //red
        else
            EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0, 255, 0)); //green
    }
    if (k == 2)
    {
        if (i & 1)
            EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 128, 64)); //orange
        else
            EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0, 255, 255)); //blue
    }
    if (k == 3)
    {
        if (i & 1)
            EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(128, 0, 0)); //orange
        else
            EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 255, 128)); //blue
    }
#endif // capacity EVE
}

/**
* @brief Calculate offset movements
*
* @param context MovingPpoints context
* @param TouchNo Touch number
* @param X Touch X
* @param Y Touch Y
* @param t Point number
*/
void helperPointsCalc(SAMAPP_MovingPoints_t* context, uint8_t* t)
{
#if (EVE_CHIPID & 0x01) == 0x01 // capacity EVE
    int16_t pointset = 0;
    int16_t tempDeltaX;
    int16_t tempDeltaY;

    EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0, 0, 0));
    EVE_Cmd_wr32(s_pHalContext, POINT_SIZE(20 * 16));
    EVE_Cmd_wr32(s_pHalContext, COLOR_A(120));

    /* For total number of points calculate the offsets of movement */
    for (int16_t k = 0; k < NO_OF_POINTS * 4L; k++)
    {
        pointset = k / NO_OF_POINTS;
        if (t[k] > NO_OF_POINTS)
        {
            context->t[k] = 0;
            context->X[k] = (context->val[pointset] >> 16) & 0xffff;
            context->Y[k] = (context->val[pointset] & 0xffff);
        }

        helperColorSelection(pointset, k);

        if (context->X[k] != -32768)
        {
            tempDeltaX = helperLinear(context->X[k], context->SmallX[pointset], context->t[k],
                NO_OF_POINTS);
            tempDeltaY = helperLinear(context->Y[k], context->SmallY, context->t[k], NO_OF_POINTS);
            EVE_Cmd_wr32(s_pHalContext, VERTEX2F(tempDeltaX * 16L, tempDeltaY * 16L));
        }
        t[k]++;
    }
#endif // capacity EVE
}

/**
* @brief Beginning BouncingPoints section
*
* @param pBInst Blob instance
* @param TouchXY Touch value
*/
void helperBlobColor(SAMAPP_BlobsInst_t* pBInst, int32_t TouchXY)
{
#if (EVE_CHIPID & 0x01) == 0x01 // capacity EVE
    uint8_t j = 0;
    // if there is touch store the values
    if ((TouchXY >> 16) != -32768)
    {
        pBInst->blobs[pBInst->CurrIdx].x = (TouchXY >> 16) & 0xffff;
        pBInst->blobs[pBInst->CurrIdx].y = (TouchXY & 0xffff);
    }
    else
    {
        pBInst->blobs[pBInst->CurrIdx].x = OFFSCREEN;
        pBInst->blobs[pBInst->CurrIdx].y = OFFSCREEN;
    }

    //calculate the current index
    pBInst->CurrIdx = (pBInst->CurrIdx + 1) & (NBLOBS - 1);

    EVE_Cmd_wr32(s_pHalContext, BEGIN(FTPOINTS));
    EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(60, 166, 117));
    for (uint8_t i = 0; i < NBLOBS; i++)
    {
        // Blobs fade away and swell as they age
        EVE_Cmd_wr32(s_pHalContext, COLOR_A(i << 1));

        EVE_Cmd_wr32(s_pHalContext, POINT_SIZE((68) + (i << 3)));

        // Random color for each blob, keyed from (blob_i + i)
        j = (pBInst->CurrIdx + i) & (NBLOBS - 1);

        // Draw it!
        if (pBInst->blobs[j].x != OFFSCREEN)
            EVE_Cmd_wr32(s_pHalContext,
                VERTEX2F((pBInst->blobs[j].x) * 16, (pBInst->blobs[j].y) * 16));
    }
#endif // capacity EVE
}

/**
* @brief Check user touches
*
* @param context BouncingSquares context
* @param Tx1 Touch x position
* @param val1 Multi touch value
*/
void helperCheckTouch(SAMAPP_BouncingSquares_t* context, int16_t Tx1, int32_t val1)
{
#if (EVE_CHIPID & 0x01) == 0x01 // capacity EVE
    uint8_t Arrayno = -1;

    // Check which rectangle is being touched according to the coordinates
    if (Tx1 >= 60 && Tx1 <= 105)
        Arrayno = 0;
    if (Tx1 >= 140 && Tx1 <= 185)
        Arrayno = 1;
    if (Tx1 >= 220 && Tx1 <= 265)
        Arrayno = 2;
    if (Tx1 >= 300 && Tx1 <= 345)
        Arrayno = 3;
    if (Tx1 >= 380 && Tx1 <= 425)
        Arrayno = 4;

    //Set the flag for the rectangle being touched
    if (Arrayno < 255)
    {
        context->RectNo[Arrayno] = 1;

        //store the vertices of the rectangle selected according to the flag
        if ((val1 >> 16) != -32768)
        {
            context->BRx[Arrayno] = (val1 >> 16) & 0xffff;
            context->BRy[Arrayno] = (val1 & 0xffff);
        }

        //limit the Bigger rectangle's height
        if (context->BRy[Arrayno] <= 60)
            context->BRy[Arrayno] = 60;
    }

    //According to the bigger rectangle values move the smaller rectangles
    for (int i = 0; i < NO_OF_RECTS; i++)
    {
        helperRectangleCalc(context, (uint8_t) i);
    }
#endif // capacity EVE
}

/**
* @brief touch test
*
* @param Sq Square positions
* @param TouchXY TouchXY value
* @param TouchNo Touch number order
*/
void helperTouchTest(SAMAPP_Squares_t* Sq, int32_t TouchXY, uint8_t TouchNo)
{
    static int32_t RowNo[5];
    static int32_t ColNo[5];

    if ((TouchXY >> 16) != -32768)
    {
        Sq->x = TouchXY >> 16;
        Sq->y = (TouchXY & 0xffff);
        Volume = (TouchNo + 1) * 51;
        for (int i = 0; i < s_pHalContext->Width / ImH; i++)
        {
            /* find row number*/
            if ((Sq->y > i * (ImH + 2)) && (Sq->y < (i + 1) * (ImH + 2)))
                RowNo[TouchNo] = i;
            if (((Sq->x) > (i * (ImW + 2))) && ((Sq->x) < ((i + 1) * (ImW + 2))))
                ColNo[TouchNo] = i;
        }
    }
    else
    {
        RowNo[TouchNo] = -1000;
        ColNo[TouchNo] = -1000;
    }
    EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 255, 255));
    EVE_Cmd_wr32(s_pHalContext, BEGIN(BITMAPS));
    EVE_Cmd_wr32(s_pHalContext, BITMAP_HANDLE(0));

    EVE_Cmd_wr32(s_pHalContext,
        VERTEX2F(((ImW + 2) * ColNo[TouchNo]) * 16, ((ImH + 2) * RowNo[TouchNo]) * 16));
}

/**
* @brief Beginning BouncingSquares section
*
* @param BRx
* @param BRy
* @param MovingRy
* @param SqNumber
*/
void helperBouncingSquaresCall(int16_t BRx, int16_t BRy, int16_t MovingRy, uint8_t SqNumber)
{
#if (EVE_CHIPID & 0x01) == 0x01 // capacity EVE
    int16_t MovingRx;

    int16_t R1;
    int16_t G1;
    int16_t B_1;
    int16_t R2;
    int16_t G2;
    int16_t B2;
    MovingRx = BRx;

    if (BRy <= 60)
        BRy = 60;
    if (BRy >= 260)
        BRy = 260;

    //different colours are set for the different rectangles
    if (SqNumber == 0)
    {
        R1 = 63;
        G1 = 72;
        B_1 = 204;
        R2 = 0;
        G2 = 255;
        B2 = 255;
    }

    if (SqNumber == 1)
    {
        R1 = 255;
        G1 = 255;
        B_1 = 0;
        R2 = 246;
        G2 = 89;
        B2 = 12;
    }

    if (SqNumber == 2)
    {
        R1 = 255;
        G1 = 0;
        B_1 = 0;
        R2 = 237;
        G2 = 28;
        B2 = 36;
    }

    if (SqNumber == 3)
    {
        R1 = 131;
        G1 = 171;
        B_1 = 9;
        R2 = 8;
        G2 = 145;
        B2 = 76;
    }

    if (SqNumber == 4)
    {
        R1 = 141;
        G1 = 4;
        B_1 = 143;
        R2 = 176;
        G2 = 3;
        B2 = 89;
    }

    // Draw the rectanles here
    EVE_Cmd_wr32(s_pHalContext, BEGIN(RECTS));
    EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(R1, G1, B_1));
    EVE_Cmd_wr32(s_pHalContext, LINE_WIDTH(10 * 16));

    EVE_Cmd_wr32(s_pHalContext, VERTEX2F(BRx * 16, (BRy) * 16));
    EVE_Cmd_wr32(s_pHalContext, VERTEX2F((BRx + 45) * 16, (260) * 16));

    EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(R2, G2, B2));
    EVE_Cmd_wr32(s_pHalContext, LINE_WIDTH(5 * 16));

    EVE_Cmd_wr32(s_pHalContext, VERTEX2F(MovingRx * 16, (MovingRy) * 16));
    EVE_Cmd_wr32(s_pHalContext, VERTEX2F((MovingRx + 45) * 16, (MovingRy + 5) * 16));
#endif // capacity EVE
}

/**
* @brief Draw set 6
*
*/
void SAMAPP_Touch_touchToPlaySong()
{
    Draw_Text(s_pHalContext, "Example for: Touch test\n\n\nPlease touch on screen (1-5 fingers)");
    uint32_t val[6];

    int32_t ftsize = 0;
    int32_t AddrOffset;

    int32_t rp = 0;
    int32_t audioval;
    int32_t wp = 0;

    SAMAPP_Squares_t SqCall;

    helperLogoIntialsetup(Main_Icons, 1);
    EVE_Hal_wr8(s_pHalContext, REG_CTOUCH_EXTENDED, CTOUCH_MODE_EXTENDED);
    EVE_sleep(30);
    AddrOffset = 102400L;

    /*Audio*/
    ftsize = FileIO_File_Open(TEST_DIR "\\Devil_Ride_30_44100_ulaw.wav", FILEIO_E_FOPEN_READ);
    helperAppendToRAMG(AddrOffset + 0UL, 64 * 2);

    wp = 1024;
    ftsize -= 1024;

    EVE_Hal_wr32(s_pHalContext, REG_PLAYBACK_FREQ, 44100);
    EVE_Hal_wr32(s_pHalContext, REG_PLAYBACK_START, AddrOffset);
    EVE_Hal_wr32(s_pHalContext, REG_PLAYBACK_FORMAT, ULAW_SAMPLES);
    EVE_Hal_wr32(s_pHalContext, REG_PLAYBACK_LENGTH, APPBUFFERSIZE);
    EVE_Hal_wr32(s_pHalContext, REG_PLAYBACK_LOOP, 1);
    EVE_Hal_wr8(s_pHalContext, REG_VOL_PB, (uint8_t) Volume);
    EVE_Hal_wr8(s_pHalContext, REG_PLAYBACK_PLAY, 1);

    for (int j = 0; j < 1500; j++)
    {
        EVE_Cmd_wr32(s_pHalContext, CMD_DLSTART);
        EVE_Cmd_wr32(s_pHalContext, CLEAR_COLOR_RGB(0, 0, 0));
        EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1));
        EVE_CoCmd_text(s_pHalContext, (int16_t) (s_pHalContext->Width / 2), 30, 26, OPT_CENTER,
            "Touch to play song"); //text info

        val[0] = EVE_Hal_rd32(s_pHalContext, REG_CTOUCH_TOUCH0_XY);
        val[1] = EVE_Hal_rd32(s_pHalContext, REG_CTOUCH_TOUCH1_XY);
        val[2] = EVE_Hal_rd32(s_pHalContext, REG_CTOUCH_TOUCH2_XY);
        val[3] = EVE_Hal_rd32(s_pHalContext, REG_CTOUCH_TOUCH3_XY);
        val[4] = (EVE_Hal_rd16(s_pHalContext, REG_CTOUCH_TOUCH4_X) << 16)
            | (EVE_Hal_rd16(s_pHalContext, REG_CTOUCH_TOUCH4_Y));

        for (int8_t i = 0; i < NO_OF_TOUCH; i++)
        {
            helperTouchTest(&SqCall, (int32_t) val[i], i);
        }
        if ((val[0] == 2147516416) && (val[1] == 2147516416) && (val[2] == 2147516416)
            && (val[3] == 2147516416) && (val[4] == 2147516416))
            Volume = 0;
        EVE_Cmd_wr32(s_pHalContext, DISPLAY());
        EVE_CoCmd_swap(s_pHalContext);
        EVE_Cmd_waitFlush(s_pHalContext);

        rp = EVE_Hal_rd16(s_pHalContext, REG_PLAYBACK_READPTR);
        audioval = APPBUFFERSIZEMINUSONE & (rp - wp);
        if (audioval > 1024)
        {
            uint16_t n = min(1024, (uint16_t )ftsize);
            helperAppendToRAMG(AddrOffset + wp, 2);
            wp = (wp + 1024) & APPBUFFERSIZEMINUSONE;
            ftsize -= n;
            EVE_Hal_wr8(s_pHalContext, REG_VOL_PB, (uint8_t) Volume);

        }
        if (wp > APPBUFFERSIZE)
            break; //Add to prevent over buffer
    }
    EVE_Hal_wr8(s_pHalContext, REG_VOL_PB, 0);
    EVE_Hal_wr8(s_pHalContext, REG_PLAYBACK_PLAY, 0);

    EVE_Hal_wr8(s_pHalContext, REG_CTOUCH_EXTENDED, CTOUCH_MODE_COMPATIBILITY);
    SAMAPP_DELAY_NEXT;
}

/**
* @brief Draw Bouncing squares
*
*/
void SAMAPP_Touch_BouncingSquares()
{
#if (EVE_CHIPID & 0x01) == 0x01 // capacity EVE
    int16_t RectX[5];
    int32_t val[5];

    SAMAPP_BouncingSquares_t context;

    Draw_Text(s_pHalContext, "Example for: Draw Bouncing squares\n\n\nPlease touch on screen (1-5 fingers)");

    //Calculate the X vertices where the five rectangles have to be placed
    for (int i = 1; i < 5; i++)
    {
        RectX[0] = 60;
        RectX[i] = RectX[i - 1] + 80;
    }

    for (int i = 0; i < 5; i++)
    {
        context.BRy[i] = 0;
        context.My[i] = 0;
        context.RectNo[i] = 0;
        context.E[i] = 0;
    }

#if defined(EVE_SUPPORT_CAPACITIVE)
    EVE_Hal_wr8(s_pHalContext, REG_CTOUCH_EXTENDED, CTOUCH_MODE_EXTENDED);
#endif
    EVE_sleep(30);
    for (int k = 0; k < 300; k++)
    {
        /* first touch*/
        val[0] = EVE_Hal_rd32(s_pHalContext, REG_CTOUCH_TOUCH0_XY);

        /*second touch*/
        val[1] = EVE_Hal_rd32(s_pHalContext, REG_CTOUCH_TOUCH1_XY);

        /*third touch*/
        val[2] = EVE_Hal_rd32(s_pHalContext, REG_CTOUCH_TOUCH2_XY);

        /*fourth  touch*/
        val[3] = EVE_Hal_rd32(s_pHalContext, REG_CTOUCH_TOUCH3_XY);

        /*fifth  touch*/
        val[4] = ((uint32_t) EVE_Hal_rd16(s_pHalContext, REG_CTOUCH_TOUCH4_X) << 16L)
            | (EVE_Hal_rd16(s_pHalContext, REG_CTOUCH_TOUCH4_Y) & 0xffffL);

        EVE_CoCmd_dlStart(s_pHalContext);
        EVE_Cmd_wr32(s_pHalContext, CLEAR_COLOR_RGB(0, 0, 0));
        EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1));

        //Check which rectangle is being touched using the coordinates and move the respective smaller rectangle
        for (int8_t i = 0; i < NO_OF_RECTS; i++)
        {
            helperCheckTouch(&context, (val[i] >> 16) & 0xffffL, val[i]);
            helperBouncingSquaresCall(RectX[i], context.BRy[i], context.My[i], i);
        }

        EVE_Cmd_wr32(s_pHalContext, DISPLAY());
        EVE_CoCmd_swap(s_pHalContext);
        EVE_Cmd_waitFlush(s_pHalContext);
        context.Count++;

    }
#if defined(EVE_SUPPORT_CAPACITIVE)
    EVE_Hal_wr8(s_pHalContext, REG_CTOUCH_EXTENDED, CTOUCH_MODE_COMPATIBILITY);
#endif
#endif // capacity EVE
}
/* End BouncingSquares section */

/**
* @brief Draw Bouncing Circles
*
*/
void SAMAPP_Touch_BouncingCircles()
{
#if (EVE_CHIPID & 0x01) == 0x01 // capacity EVE
    int32_t Touchval[NO_OF_CIRCLE];
    SAMAPP_BouncingCircles_t context;

    Draw_Text(s_pHalContext, "Example for: Draw Bouncing Circles\n\n\nPlease touch on screen (1-5 fingers)");

#if defined(EVE_SUPPORT_CAPACITIVE)
    EVE_Hal_wr8(s_pHalContext, REG_CTOUCH_EXTENDED, CTOUCH_MODE_EXTENDED);
#endif
    EVE_sleep(30);
    /* calculate the intital radius of the circles before the touch happens*/
    context.Tsq1[0] = 50;
    context.C1X[0] = 190;
    context.C1Y[0] = 136;
    for (int8_t i = 1; i < NO_OF_CIRCLE; i++)
    {
        context.Tsq1[i] = context.Tsq1[i - 1] + 30;
        context.C1X[i] = context.C1X[i - 1] - 30;
        context.C1Y[i] = 136;
    }

    for (int32_t k = 0; k < 150; k++)
    {
        EVE_CoCmd_dlStart(s_pHalContext);
        EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1));
        EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 255, 255));
        /* values of the five touches are stored here */

        Touchval[0] = EVE_Hal_rd32(s_pHalContext, REG_CTOUCH_TOUCH0_XY);
        Touchval[1] = EVE_Hal_rd32(s_pHalContext, REG_CTOUCH_TOUCH1_XY);
        Touchval[2] = EVE_Hal_rd32(s_pHalContext, REG_CTOUCH_TOUCH2_XY);
        Touchval[3] = EVE_Hal_rd32(s_pHalContext, REG_CTOUCH_TOUCH3_XY);
        Touchval[4] = ((int32_t) EVE_Hal_rd16(s_pHalContext, REG_CTOUCH_TOUCH4_X) << 16)
            | (EVE_Hal_rd16(s_pHalContext, REG_CTOUCH_TOUCH4_Y));

        for (int8_t i = 0; i < NO_OF_CIRCLE; i++)
        {
            helperStoreTouch(&context, Touchval[i], i);
        }
        /* The plot is drawn here */
        helperPlotXY();

        /* check which circle has been touched based on the coordinates and store the[0] number of the circle touched*/

        for (int8_t i = 0; i < NO_OF_CIRCLE; i++)
        {
            helperCheckCircleTouchCood(&context, Touchval[0], 0, i);
            helperCheckCircleTouchCood(&context, Touchval[1], 1, i);
            helperCheckCircleTouchCood(&context, Touchval[2], 2, i);
            helperCheckCircleTouchCood(&context, Touchval[3], 3, i);
            helperCheckCircleTouchCood(&context, Touchval[4], 4, i);
        }
        /* calculate the radius of each circle according to the touch of each individual circle */

        for (int8_t i = 0; i < NO_OF_CIRCLE; i++)
        {
            context.Tsq1[i] = (float) helperCirclePlot(&context, (uint16_t) context.C1X[i],
                (uint16_t) context.C1Y[i], i);
        }
        /* with the calculated radius draw the circles as well as the Touch points */

        for (int8_t i = 0; i < (NO_OF_CIRCLE); i++)
        {
            helperConcentricCircles(context.Tsq1[i], 255, 0, 0);
            helperTouchPoints((int16_t) context.C1X[i], (int16_t) context.C1Y[i], i + 1);
        }

        EVE_Cmd_wr32(s_pHalContext, DISPLAY());
        EVE_CoCmd_swap(s_pHalContext);
        EVE_Cmd_waitFlush(s_pHalContext);
    }
#endif // capacity EVE
}
/* End BouncingCircle section */

/**
* @brief Draw Bouncing points
*
*/
void SAMAPP_Touch_BouncingPoints()
{
#if (EVE_CHIPID & 0x01) == 0x01 // capacity EVE
    int32_t val[5];
    SAMAPP_BlobsInst_t gBlobsInst[APP_BLOBS_NUMTOUCH];
    SAMAPP_BlobsInst_t* pBInst;

    Draw_Text(s_pHalContext, "Example for: Draw Bouncing points\n\n\nPlease touch on screen (1-5 fingers)");

#if defined(EVE_SUPPORT_CAPACITIVE)
    EVE_Hal_wr8(s_pHalContext, REG_CTOUCH_EXTENDED, CTOUCH_MODE_EXTENDED);
#endif
    EVE_sleep(30);
    pBInst = &gBlobsInst[0];

    //set all coordinates to OFFSCREEN position
    for (uint8_t j = 0; j < APP_BLOBS_NUMTOUCH; j++)
    {
        for (uint8_t i = 0; i < NBLOBS; i++)
        {
            pBInst->blobs[i].x = OFFSCREEN;
            pBInst->blobs[i].y = OFFSCREEN;
        }
        pBInst->CurrIdx = 0;
        pBInst++;
    }

    for (uint16_t k = 0; k < 150; k++)
    {
        val[0] = EVE_Hal_rd32(s_pHalContext, REG_CTOUCH_TOUCH0_XY);
        val[1] = EVE_Hal_rd32(s_pHalContext, REG_CTOUCH_TOUCH1_XY);
        val[2] = EVE_Hal_rd32(s_pHalContext, REG_CTOUCH_TOUCH2_XY);
        val[3] = EVE_Hal_rd32(s_pHalContext, REG_CTOUCH_TOUCH3_XY);
        val[4] = (((int32_t) EVE_Hal_rd16(s_pHalContext, REG_CTOUCH_TOUCH4_X) << 16)
            | (EVE_Hal_rd16(s_pHalContext, REG_CTOUCH_TOUCH4_Y) & 0xffff));

        EVE_Cmd_wr32(s_pHalContext, CMD_DLSTART);
#if 1
        EVE_Cmd_wr32(s_pHalContext, CLEAR_COLOR_RGB(43, 73, 59));
        EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1));
        EVE_Cmd_wr32(s_pHalContext, BLEND_FUNC(SRC_ALPHA, ONE));
        EVE_Cmd_wr32(s_pHalContext, COLOR_MASK(1, 1, 1, 0));
#else
        EVE_Cmd_wr32(s_pHalContext, CLEAR_COLOR_RGB(255, 255, 255));
        EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1));
        EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0, 0, 0));
#endif

        // draw blobs according to the number of touches
        for (uint16_t j = 0; j < APP_BLOBS_NUMTOUCH; j++)
        {
            helperBlobColor(&gBlobsInst[j], val[j]);
        }

        EVE_Cmd_wr32(s_pHalContext, DISPLAY());
        EVE_CoCmd_swap(s_pHalContext);
        EVE_Cmd_waitFlush(s_pHalContext);
    }

#if defined(EVE_SUPPORT_CAPACITIVE)
    EVE_Hal_wr8(s_pHalContext, REG_CTOUCH_EXTENDED, CTOUCH_MODE_COMPATIBILITY);
#endif
#endif // capacity EVE
}
/* End BouncingPoints section */

/**
* @brief Move points
*
*/
void SAMAPP_Touch_MovingPoints()
{
#if (EVE_CHIPID & 0x01) == 0x01 // capacity EVE
    SAMAPP_MovingPoints_t context;
    context.Flag = 1;

    Draw_Text(s_pHalContext, "Example for: Draw Moving points\n\n\nPlease touch on screen (1-5 fingers)");

#if defined(EVE_SUPPORT_CAPACITIVE)
    EVE_Hal_wr8(s_pHalContext, REG_CTOUCH_EXTENDED, CTOUCH_MODE_EXTENDED);
#endif
    EVE_sleep(30);
    /* Initialize all coordinates */
    for (uint16_t j = 0; j < 4; j++)
    {
        for (uint16_t i = 0; i < NO_OF_POINTS; i++)
        {
            context.t[i + j * NO_OF_POINTS] = (uint8_t) i;
            context.X[i + j * NO_OF_POINTS] = -32768;
        }
    }

    context.SmallX[0] = 180;
    context.SmallY = 20;
    for (uint16_t i = 0; i < 5; i++)
    {
        context.SmallX[i + 1] = context.SmallX[i] + 50;
    }

#if defined(FT900_PLATFORM) || defined(FT93X_PLATFORM)
    for (uint16_t k = 0; k < 800; k++)
#elif defined(ARDUINO_PLATFORM)
    for (uint16_t i = 0; i < 700; i++)
#else
    for (uint16_t k = 0; k < 150; k++)
#endif
    {
        EVE_Cmd_wr32(s_pHalContext, CMD_DLSTART);
        EVE_Cmd_wr32(s_pHalContext, CLEAR_COLOR_RGB(255, 255, 255));
        EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1));

        context.val[0] = EVE_Hal_rd32(s_pHalContext, REG_CTOUCH_TOUCH0_XY);
        context.val[1] = EVE_Hal_rd32(s_pHalContext, REG_CTOUCH_TOUCH1_XY);
        context.val[2] = EVE_Hal_rd32(s_pHalContext, REG_CTOUCH_TOUCH2_XY);
        context.val[3] = EVE_Hal_rd32(s_pHalContext, REG_CTOUCH_TOUCH3_XY);

        EVE_Cmd_wr32(s_pHalContext, BEGIN(FTPOINTS));

        EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 0, 0));
        helperPointsCalc(&context, &context.t[0]);
        EVE_Cmd_wr32(s_pHalContext, DISPLAY());
        EVE_CoCmd_swap(s_pHalContext);
        EVE_Cmd_waitFlush(s_pHalContext);
    }
#if defined(EVE_SUPPORT_CAPACITIVE)
    EVE_Hal_wr8(s_pHalContext, REG_CTOUCH_EXTENDED, CTOUCH_MODE_COMPATIBILITY);
#endif
#endif // capacity EVE
}
/* End MovingPoints section */

/**
* @brief Multi touch on a single tracked object can be individually tracked to save the MCU calculations on rotary and linear tracking. Maximum of 5 trackers.
*
*/
void SAMAPP_Touch_multiTracker()
{
#if defined(FT81X_ENABLE) // FT81X only
    uint32_t trackers[5];
    uint32_t delayLoop = 300;
    uint32_t trackerVal;
    uint8_t tagval;
    uint8_t RDialTag = 100;
    uint8_t GDialTag = 101;
    uint8_t BDialTag = 102;
    uint8_t ADialTag = 103;
    uint8_t DialR = (uint8_t) (s_pHalContext->Width / 10);
    uint8_t rectRed = 0;
    uint8_t rectGreen = 0;
    uint8_t rectBlue = 0;
    uint8_t rectAlpha = 255;

    uint16_t RDialX = DialR + 20;
    uint16_t RDialY = (uint16_t) s_pHalContext->Height / 4;
    uint16_t GDialX = DialR + 20;
    uint16_t GDialY = RDialY * 3;
    uint16_t BDialX = (uint16_t) (s_pHalContext->Width - 20 - DialR);
    uint16_t BDialY = (uint16_t) (s_pHalContext->Height / 4);
    uint16_t ADialX = (uint16_t) (s_pHalContext->Width - 20 - DialR);
    uint16_t ADialY = BDialY * 3;
    uint16_t rectWidth = (uint16_t) (s_pHalContext->Width / 2.6);
    uint16_t rectHeight = (uint16_t) (s_pHalContext->Height / 2);
    uint16_t rectX = (uint16_t) (s_pHalContext->Width / 2 - rectWidth / 2);
    uint16_t rectY = (uint16_t) (s_pHalContext->Height / 2 - rectHeight / 2);
    uint16_t RDialTrackVal = 0;
    uint16_t GDialTrackVal = 0;
    uint16_t BDialTrackVal = 0;
    uint16_t ADialTrackVal = 65535;

    Draw_Text(s_pHalContext, "Example for: Multi touch on a single tracked object\n\n\nPlease touch on screen (1-5 fingers)");

    EVE_CoCmd_track(s_pHalContext, RDialX, RDialY, 1, 1, RDialTag);
    EVE_CoCmd_track(s_pHalContext, GDialX, GDialY, 1, 1, GDialTag);
    EVE_CoCmd_track(s_pHalContext, BDialX, BDialY, 1, 1, BDialTag);
    EVE_CoCmd_track(s_pHalContext, ADialX, ADialY, 1, 1, ADialTag);
#ifndef RESISTANCE_THRESHOLD
    EVE_Hal_wr8(s_pHalContext, REG_CTOUCH_EXTENDED, CTOUCH_MODE_EXTENDED);
#endif
    while (delayLoop != 0)
    {
        trackers[0] = EVE_Hal_rd32(s_pHalContext, REG_TRACKER);
        trackers[1] = EVE_Hal_rd32(s_pHalContext, REG_TRACKER_1);
        trackers[2] = EVE_Hal_rd32(s_pHalContext, REG_TRACKER_2);
        trackers[3] = EVE_Hal_rd32(s_pHalContext, REG_TRACKER_3);

        for (uint8_t i = 0; i < 4; i++)
        {
            tagval = (trackers[i] & 0xff);
            trackerVal = (trackers[i] >> 16) & 0xffff;
            if (tagval == RDialTag)
            {
                rectRed = (uint8_t) (trackerVal * 255 / 65536);
                RDialTrackVal = (uint16_t) trackerVal;
            }
            else if (tagval == GDialTag)
            {
                rectGreen = (uint8_t) (trackerVal * 255 / 65536);
                GDialTrackVal = (uint16_t) trackerVal;
            }
            else if (tagval == BDialTag)
            {
                rectBlue = (uint8_t) (trackerVal * 255 / 65536);
                BDialTrackVal = (uint16_t) trackerVal;
            }
            else if (tagval == ADialTag)
            {
                rectAlpha = (uint8_t) (trackerVal * 255 / 65536);
                ADialTrackVal = (uint16_t) trackerVal;
            }
        }
        EVE_CoCmd_dlStart(s_pHalContext); // clear screen
        EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1));
        EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 255, 255));
        EVE_Cmd_wr32(s_pHalContext, COLOR_A(255));
        EVE_Cmd_wr32(s_pHalContext, TAG_MASK(1));
        EVE_Cmd_wr32(s_pHalContext, TAG(RDialTag));
        EVE_CoCmd_dial(s_pHalContext, RDialX, RDialY, DialR, 0, RDialTrackVal);
        EVE_Cmd_wr32(s_pHalContext, TAG(GDialTag));
        EVE_CoCmd_dial(s_pHalContext, GDialX, GDialY, DialR, 0, GDialTrackVal);
        EVE_Cmd_wr32(s_pHalContext, TAG(BDialTag));
        EVE_CoCmd_dial(s_pHalContext, BDialX, BDialY, DialR, 0, BDialTrackVal);
        EVE_Cmd_wr32(s_pHalContext, TAG(ADialTag));
        EVE_CoCmd_dial(s_pHalContext, ADialX, ADialY, DialR, 0, ADialTrackVal);
        EVE_Cmd_wr32(s_pHalContext, TAG_MASK(0));

        EVE_CoCmd_text(s_pHalContext, RDialX, RDialY, 28, OPT_CENTER, "Red");//text info
        EVE_CoCmd_text(s_pHalContext, GDialX, GDialY, 28, OPT_CENTER, "Green");//text info
        EVE_CoCmd_text(s_pHalContext, BDialX, BDialY, 28, OPT_CENTER, "Blue");//text info
        EVE_CoCmd_text(s_pHalContext, ADialX, ADialY, 28, OPT_CENTER, "Alpha");//text info

        EVE_Cmd_wr32(s_pHalContext, BEGIN(RECTS));
        EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(rectRed, rectGreen, rectBlue));
        EVE_Cmd_wr32(s_pHalContext, COLOR_A(rectAlpha));
        EVE_Cmd_wr32(s_pHalContext, VERTEX2F(rectX * 16, rectY * 16));
        EVE_Cmd_wr32(s_pHalContext, VERTEX2F((rectX + rectWidth) * 16, (rectY + rectHeight) * 16));

        EVE_Cmd_wr32(s_pHalContext, DISPLAY());
        EVE_CoCmd_swap(s_pHalContext);
        /* Wait till coprocessor completes the operation */
        EVE_Cmd_waitFlush(s_pHalContext);

        delayLoop--;
    }
#endif
}

/**
* @brief explain the usage of touch engine of FT801 and FT811
* 
*/
void SAMAPP_Touch_touchInfoFT801FT811()
{
    Draw_Text(s_pHalContext, "Example for: Touch raw, touch screen, touch tag, raw adc of FT801 and FT811\n\n\nPlease touch on screen");

#if defined(FT801_ENABLE) || defined(FT811_ENABLE)
    int32_t LoopFlag = 0;
    int32_t wbutton;
    int32_t hbutton;
    int32_t tagval;
    int32_t tagoption;
    char8_t StringArray[100], StringArray1[100];
    uint32_t ReadWord;
    int16_t xvalue;
    int16_t yvalue;
    int16_t pendown;


    /*************************************************************************/
    /* Below code demonstrates the usage of touch function. Display info     */
    /* touch raw, touch screen, touch tag, raw adc and resistance values     */
    /*************************************************************************/
    LoopFlag = 300;
    wbutton = s_pHalContext->Width / 8;
    hbutton = s_pHalContext->Height / 8;
    EVE_Hal_wr8(s_pHalContext, REG_CTOUCH_EXTENDED, CTOUCH_MODE_EXTENDED);
    while (LoopFlag--)
    {
        EVE_CoCmd_dlStart(s_pHalContext);
        EVE_Cmd_wr32(s_pHalContext, CLEAR_COLOR_RGB(64, 64, 64));
        EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1));
        EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0xff, 0xff, 0xff));
        EVE_Cmd_wr32(s_pHalContext, TAG_MASK(0));

        StringArray[0] = '\0';
        strcat(StringArray, "Touch Screen XY0 (");
        ReadWord = EVE_Hal_rd32(s_pHalContext, REG_CTOUCH_TOUCH0_XY);
        /*yvalue = (uint16_t)(ReadWord & 0xffff);
        xvalue = (uint16_t)((ReadWord>>16) & 0xffff);*/
        yvalue = (ReadWord & 0xffff);
        xvalue = (ReadWord >> 16);
        Gpu_Hal_Dec2Ascii(StringArray, (int32_t)xvalue);
        strcat(StringArray, ",");
        Gpu_Hal_Dec2Ascii(StringArray, (int32_t)yvalue);
        strcat(StringArray, ")");
        EVE_CoCmd_text(s_pHalContext, (int16_t)(s_pHalContext->Width / 2), 50, 26, OPT_CENTER, StringArray);

        StringArray[0] = '\0';
        strcat(StringArray, "Touch Screen XY1 (");
        ReadWord = EVE_Hal_rd32(s_pHalContext, REG_CTOUCH_TOUCH1_XY);
        yvalue = (ReadWord & 0xffff);
        xvalue = (ReadWord >> 16);
        Gpu_Hal_Dec2Ascii(StringArray, (int32_t)xvalue);
        strcat(StringArray, ",");
        Gpu_Hal_Dec2Ascii(StringArray, (int32_t)yvalue);
        strcat(StringArray, ")");
        EVE_CoCmd_text(s_pHalContext, (int16_t)(s_pHalContext->Width / 2), 70, 26, OPT_CENTER, StringArray);

        StringArray[0] = '\0';
        strcat(StringArray, "Touch Screen XY2 (");
        ReadWord = EVE_Hal_rd32(s_pHalContext, REG_CTOUCH_TOUCH2_XY);
        yvalue = (ReadWord & 0xffff);
        xvalue = (ReadWord >> 16);
        Gpu_Hal_Dec2Ascii(StringArray, (int32_t)xvalue);
        strcat(StringArray, ",");
        Gpu_Hal_Dec2Ascii(StringArray, (int32_t)yvalue);
        strcat(StringArray, ")");
        EVE_CoCmd_text(s_pHalContext, (int16_t)(s_pHalContext->Width / 2), 90, 26, OPT_CENTER, StringArray);

        StringArray[0] = '\0';
        strcat(StringArray, "Touch Screen XY3 (");
        ReadWord = EVE_Hal_rd32(s_pHalContext, REG_CTOUCH_TOUCH3_XY);
        yvalue = (ReadWord & 0xffff);
        xvalue = (ReadWord >> 16);
        Gpu_Hal_Dec2Ascii(StringArray, (int32_t)xvalue);
        strcat(StringArray, ",");
        Gpu_Hal_Dec2Ascii(StringArray, (int32_t)yvalue);
        strcat(StringArray, ")");
        EVE_CoCmd_text(s_pHalContext, (int16_t)(s_pHalContext->Width / 2), 110, 26, OPT_CENTER, StringArray);

        StringArray[0] = '\0';
        StringArray1[0] = '\0';
        strcat(StringArray, "Touch Screen XY4 (");
        xvalue = EVE_Hal_rd16(s_pHalContext, REG_CTOUCH_TOUCH4_X);
        yvalue = EVE_Hal_rd16(s_pHalContext, REG_CTOUCH_TOUCH4_Y);

        Gpu_Hal_Dec2Ascii(StringArray, (int32_t)xvalue);
        strcat(StringArray, ",");
        Gpu_Hal_Dec2Ascii(StringArray1, (int32_t)yvalue);
        strcat(StringArray1, ")");
        strcat(StringArray, StringArray1);
        EVE_CoCmd_text(s_pHalContext, (int16_t)(s_pHalContext->Width / 2), 130, 26, OPT_CENTER, StringArray);

        StringArray[0] = '\0';
        strcat(StringArray, "Touch TAG (");
        ReadWord = EVE_Hal_rd8(s_pHalContext, REG_TOUCH_TAG);
        Gpu_Hal_Dec2Ascii(StringArray, ReadWord);
        strcat(StringArray, ")");
        EVE_CoCmd_text(s_pHalContext, (int16_t)(s_pHalContext->Width / 2), 170, 26, OPT_CENTER, StringArray);
        tagval = ReadWord;

        EVE_CoCmd_fgColor(s_pHalContext, 0x008000);
        EVE_Cmd_wr32(s_pHalContext, TAG_MASK(1));

        EVE_Cmd_wr32(s_pHalContext, TAG(13));
        tagoption = 0;
        if (13 == tagval)
        {
            tagoption = OPT_FLAT;
        }
        EVE_CoCmd_button(s_pHalContext, (s_pHalContext->Width / 2) - (wbutton / 2), (s_pHalContext->Height * 3 / 4) - (hbutton / 2), wbutton, hbutton, 26, tagoption, "Tag13");

        EVE_Cmd_wr32(s_pHalContext, DISPLAY());
        EVE_CoCmd_swap(s_pHalContext);

        /* Wait till coprocessor completes the operation */
        EVE_Cmd_waitFlush(s_pHalContext);
        EVE_sleep(30);

    }

    EVE_Hal_wr8(s_pHalContext, REG_CTOUCH_EXTENDED, CTOUCH_MODE_COMPATIBILITY);
    EVE_sleep(30);
#endif
}

/**
* @brief explain the usage of touch engine of EVE other than FT801 and FT811
* 
*/
void SAMAPP_Touch_touchInfoOther()
{
    Draw_Text(s_pHalContext, "Example for: Touch raw, touch screen, touch tag, raw adc\n\n\nPlease touch on screen");

#if !defined(FT801_ENABLE) && !defined(FT811_ENABLE)
    int32_t LoopFlag = 0;
    int32_t wbutton;
    int32_t hbutton;
    int32_t tagval;
    int32_t tagoption;
    char8_t StringArray[100];
    uint32_t ReadWord;
    uint16_t xvalue;
    uint16_t yvalue;
    uint16_t pendown;

    /*************************************************************************/
    /* Below code demonstrates the usage of touch function. Display info     */
    /* touch raw, touch screen, touch tag, raw adc and resistance values     */
    /*************************************************************************/
    LoopFlag = 300;
    wbutton = s_pHalContext->Width / 8;
    hbutton = s_pHalContext->Height / 8;
    while (LoopFlag--)
    {
        EVE_CoCmd_dlStart(s_pHalContext);
        EVE_Cmd_wr32(s_pHalContext, CLEAR_COLOR_RGB(64, 64, 64));
        EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1));
        EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0xff, 0xff, 0xff));
        EVE_Cmd_wr32(s_pHalContext, TAG_MASK(0));
        /* Draw informative text at width/2,20 location */
        StringArray[0] = '\0';
        strcat(StringArray, "Touch Raw XY (");
        ReadWord = EVE_Hal_rd32(s_pHalContext, REG_TOUCH_RAW_XY);
        yvalue = (uint16_t) (ReadWord & 0xffff);
        xvalue = (uint16_t) ((ReadWord >> 16) & 0xffff);
        Gpu_Hal_Dec2Ascii(StringArray, (uint32_t) xvalue);
        strcat(StringArray, ",");
        Gpu_Hal_Dec2Ascii(StringArray, (uint32_t) yvalue);
        strcat(StringArray, ")");
        EVE_CoCmd_text(s_pHalContext, (int16_t) (s_pHalContext->Width / 2), 10, 26, OPT_CENTER,
            StringArray);

        StringArray[0] = '\0';
        strcat(StringArray, "Touch RZ (");
        ReadWord = EVE_Hal_rd16(s_pHalContext, REG_TOUCH_RZ);
        Gpu_Hal_Dec2Ascii(StringArray, ReadWord);
        strcat(StringArray, ")");
        EVE_CoCmd_text(s_pHalContext, (int16_t) (s_pHalContext->Width / 2), 25, 26, OPT_CENTER,
            StringArray);

        StringArray[0] = '\0';
        strcat(StringArray, "Touch Screen XY (");
        ReadWord = EVE_Hal_rd32(s_pHalContext, REG_TOUCH_SCREEN_XY);
        yvalue = (int16_t) (ReadWord & 0xffff);
        xvalue = (int16_t) ((ReadWord >> 16) & 0xffff);
        Gpu_Hal_Dec2Ascii(StringArray, (int32_t) xvalue);
        strcat(StringArray, ",");
        Gpu_Hal_Dec2Ascii(StringArray, (int32_t) yvalue);
        strcat(StringArray, ")");
        EVE_CoCmd_text(s_pHalContext, (int16_t) (s_pHalContext->Width / 2), 40, 26, OPT_CENTER,
            StringArray);

        StringArray[0] = '\0';
        strcat(StringArray, "Touch TAG (");
        ReadWord = EVE_Hal_rd8(s_pHalContext, REG_TOUCH_TAG);
        Gpu_Hal_Dec2Ascii(StringArray, ReadWord);
        strcat(StringArray, ")");
        EVE_CoCmd_text(s_pHalContext, (int16_t) (s_pHalContext->Width / 2), 55, 26, OPT_CENTER,
            StringArray);
        tagval = ReadWord;
        StringArray[0] = '\0';
        strcat(StringArray, "Touch Direct XY (");
        ReadWord = EVE_Hal_rd32(s_pHalContext, REG_TOUCH_DIRECT_XY);
        yvalue = (int16_t) (ReadWord & 0x03ff);
        xvalue = (int16_t) ((ReadWord >> 16) & 0x03ff);
        Gpu_Hal_Dec2Ascii(StringArray, (int32_t) xvalue);
        strcat(StringArray, ",");
        Gpu_Hal_Dec2Ascii(StringArray, (int32_t) yvalue);
        pendown = (int16_t) ((ReadWord >> 31) & 0x01);
        strcat(StringArray, ",");
        Gpu_Hal_Dec2Ascii(StringArray, (int32_t) pendown);
        strcat(StringArray, ")");
        EVE_CoCmd_text(s_pHalContext, (int16_t) (s_pHalContext->Width / 2), 70, 26, OPT_CENTER,
            StringArray);

        StringArray[0] = '\0';
        strcat(StringArray, "Touch Direct Z1Z2 (");
        ReadWord = EVE_Hal_rd32(s_pHalContext, REG_TOUCH_DIRECT_Z1Z2);
        yvalue = (int16_t) (ReadWord & 0x03ff);
        xvalue = (int16_t) ((ReadWord >> 16) & 0x03ff);
        Gpu_Hal_Dec2Ascii(StringArray, (int32_t) xvalue);
        strcat(StringArray, ",");
        Gpu_Hal_Dec2Ascii(StringArray, (int32_t) yvalue);
        strcat(StringArray, ")");

        EVE_CoCmd_text(s_pHalContext, (int16_t) (s_pHalContext->Width / 2), 85, 26, OPT_CENTER,
            StringArray);

        EVE_CoCmd_fgColor(s_pHalContext, 0x008000);
        EVE_Cmd_wr32(s_pHalContext, TAG_MASK(1));
        tagoption = 0;
        if (12 == tagval)
        {
            tagoption = OPT_FLAT;
        }

        EVE_Cmd_wr32(s_pHalContext, TAG(12));
        EVE_CoCmd_button(s_pHalContext, (int16_t) ((s_pHalContext->Width / 4) - (wbutton / 2)),
            (int16_t) ((s_pHalContext->Height * 2 / 4) - (hbutton / 2)), (int16_t) wbutton,
            (int16_t) hbutton, 26, (int16_t) tagoption, "Tag12");
        EVE_Cmd_wr32(s_pHalContext, TAG(13));
        tagoption = 0;
        if (13 == tagval)
        {
            tagoption = OPT_FLAT;
        }
        EVE_CoCmd_button(s_pHalContext, (int16_t) ((s_pHalContext->Width * 3 / 4) - (wbutton / 2)),
            (int16_t) ((s_pHalContext->Height * 3 / 4) - (hbutton / 2)), (int16_t) wbutton,
            (int16_t) hbutton, 26, (int16_t) tagoption, "Tag13");

        EVE_Cmd_wr32(s_pHalContext, DISPLAY());
        EVE_CoCmd_swap(s_pHalContext);

        /* Wait till coprocessor completes the operation */
        EVE_Cmd_waitFlush(s_pHalContext);
        EVE_sleep(30);
    }
#endif
}

/**
* @brief Sample app api to demonstrate track widget funtionality
*
*/
void SAMAPP_Touch_objectTrack()
{
    /*************************************************************************/
    /* Below code demonstrates the usage of track function. Track function   */
    /* tracks the pen touch on any specific object. Track function supports  */
    /* rotary and horizontal/vertical tracks. Rotary is given by rotation    */
    /* angle and horizontal/vertucal track is offset position.               */
    /*************************************************************************/
    int32_t LoopFlag = 0;
    uint32_t TrackRegisterVal = 0;
    uint16_t angleval = 0;
    uint16_t slideval = 0;
    uint16_t scrollval = 0;

    /* Set the tracker for 3 bojects */

    Draw_Text(s_pHalContext, "Example for: Rotary and horizontal/vertical tracks\n\n\nPlease touch on screen");

    EVE_CoCmd_track(s_pHalContext, (int16_t) (s_pHalContext->Width / 2),
        (int16_t) (s_pHalContext->Height / 2), 1, 1, 10);
    EVE_CoCmd_track(s_pHalContext, 40, (int16_t) (s_pHalContext->Height - 40),
        (int16_t) (s_pHalContext->Width - 80), 8, 11);
    EVE_CoCmd_track(s_pHalContext, (int16_t) (s_pHalContext->Width - 40), 40, 8,
        (int16_t) (s_pHalContext->Height - 80), 12);
    /* Wait till coprocessor completes the operation */
    EVE_Cmd_waitFlush(s_pHalContext);

    LoopFlag = 600;
    /* update the background color continuously for the color change in any of the trackers */
    while (LoopFlag--)
    {
        uint8_t tagval = 0;
        TrackRegisterVal = EVE_Hal_rd32(s_pHalContext, REG_TRACKER);
        tagval = TrackRegisterVal & 0xff;

        if (10 == tagval)
        {
            angleval = TrackRegisterVal >> 16;
        }
        else if (11 == tagval)
        {
            slideval = TrackRegisterVal >> 16;
        }
        else if (12 == tagval)
        {
            scrollval = TrackRegisterVal >> 16;
            if ((scrollval + 65535 / 10) > (9 * 65535 / 10))
            {
                scrollval = (8 * 65535 / 10);
            }
            else if (scrollval < (1 * 65535 / 10))
            {
                scrollval = 0;
            }
            else
            {
                scrollval -= (1 * 65535 / 10);
            }
        }

        /* Display a rotary dial, horizontal slider and vertical scroll */

        EVE_Cmd_wr32(s_pHalContext, CMD_DLSTART);

        int32_t tmpval0;
        int32_t tmpval1;
        int32_t tmpval2;
        uint8_t angval;
        uint8_t sldval;
        uint8_t scrlval;

        tmpval0 = (int32_t) angleval * 255 / 65536;
        tmpval1 = (int32_t) slideval * 255 / 65536;
        tmpval2 = (int32_t) scrollval * 255 / 65536;

        angval = tmpval0 & 0xff;
        sldval = tmpval1 & 0xff;
        scrlval = tmpval2 & 0xff;

        EVE_Cmd_wr32(s_pHalContext, CLEAR_COLOR_RGB(angval, sldval, scrlval));
        EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1));
        EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0xff, 0xff, 0xff));

        /* Draw dial with 3d effect */
        EVE_CoCmd_fgColor(s_pHalContext, 0x00ff00);
        EVE_CoCmd_bgColor(s_pHalContext, 0x800000);
        EVE_Cmd_wr32(s_pHalContext, TAG(10));
        EVE_CoCmd_dial(s_pHalContext, (int16_t) (s_pHalContext->Width / 2),
            (int16_t) (s_pHalContext->Height / 2), (int16_t) (s_pHalContext->Width / 8), 0,
            angleval);

        /* Draw slider with 3d effect */
        EVE_CoCmd_fgColor(s_pHalContext, 0x00a000);
        EVE_CoCmd_bgColor(s_pHalContext, 0x800000);
        EVE_Cmd_wr32(s_pHalContext, TAG(11));
        EVE_CoCmd_slider(s_pHalContext, 40, (int16_t) (s_pHalContext->Height - 40),
            (int16_t) (s_pHalContext->Width - 80), 8, 0, slideval, 65535);

        /* Draw scroll with 3d effect */
        EVE_CoCmd_fgColor(s_pHalContext, 0x00a000);
        EVE_CoCmd_bgColor(s_pHalContext, 0x000080);
        EVE_Cmd_wr32(s_pHalContext, TAG(12));
        EVE_CoCmd_scrollbar(s_pHalContext, (int16_t) (s_pHalContext->Width - 40), 40, 8,
            (int16_t) (s_pHalContext->Height - 80), 0, scrollval, (uint16_t) (65535 * 0.2), 65535);

        EVE_CoCmd_fgColor(s_pHalContext, TAG_MASK(0));
        EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0xff, 0xff, 0xff));
        EVE_CoCmd_text(s_pHalContext, (int16_t) (s_pHalContext->Width / 2),
            (int16_t) ((s_pHalContext->Height / 2) + (s_pHalContext->Width / 8) + 8), 26,
            OPT_CENTER, "Rotary track");
        EVE_CoCmd_text(s_pHalContext, (int16_t) (s_pHalContext->Width / 2),
            (int16_t) (s_pHalContext->Height - 40 + 8 + 8), 26, OPT_CENTER, "Horizontal track");
        EVE_CoCmd_text(s_pHalContext, (int16_t) (s_pHalContext->Width - 50), 20, 26, OPT_CENTER,
            "Vertical track");

        EVE_Cmd_wr32(s_pHalContext, DISPLAY());
        EVE_CoCmd_swap(s_pHalContext);

        /* Wait till coprocessor completes the operation */
        EVE_Cmd_waitFlush(s_pHalContext);

        EVE_sleep(10);
    }

    /* Set the tracker for 3 bojects */

    EVE_CoCmd_track(s_pHalContext, 240, 136, 0, 0, 10);
    EVE_CoCmd_track(s_pHalContext, 40, 232, 0, 0, 11);
    EVE_CoCmd_track(s_pHalContext, 400, 40, 0, 0, 12);

    /* Wait till coprocessor completes the operation */
    EVE_Cmd_waitFlush(s_pHalContext);
}

void SAMAPP_Touch() {
    SAMAPP_Touch_touchToPlaySong();
    SAMAPP_Touch_BouncingSquares();
    SAMAPP_Touch_BouncingCircles();
    SAMAPP_Touch_BouncingPoints();
    SAMAPP_Touch_MovingPoints();
    SAMAPP_Touch_multiTracker();
    SAMAPP_Touch_touchInfoFT801FT811();
    SAMAPP_Touch_touchInfoOther();
    SAMAPP_Touch_objectTrack();
}


