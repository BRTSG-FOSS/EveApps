/*
 * Copyright (c) Bridgetek Pte Ltd
 *
 * THIS SOFTWARE IS PROVIDED BY BRIDGETEK PTE LTD "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 * BRIDGETEK PTE LTD BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES LOSS OF USE, DATA, OR PROFITS OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
 * TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * BRIDGETEK DRIVERS MAY BE USED ONLY IN CONJUNCTION WITH PRODUCTS BASED ON BRIDGETEK PARTS.
 *
 * BRIDGETEK DRIVERS MAY BE DISTRIBUTED IN ANY FORM AS LONG AS LICENSE INFORMATION IS NOT MODIFIED.
 *
 * IF A CUSTOM VENDOR ID AND/OR PRODUCT ID OR DESCRIPTION STRING ARE USED, IT IS THE
 * RESPONSIBILITY OF THE PRODUCT MANUFACTURER TO MAINTAIN ANY CHANGES AND SUBSEQUENT WHQL
 * RE-CERTIFICATION AS A RESULT OF MAKING THESE CHANGES.
 *
 * Abstract: Application to demonstrate function of EVE.
 * Author : Bridgetek
 */

 /*
  *  Description: This is sample demo for Multi-touching
  */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "Platform.h"
#include "Gpu_Hal.h"
#include "Common.h"
#include "App.h"
#include "Samapp_Enable_API.h"

#ifdef SAMAPP_ENABLE_APIS_SET7

static EVE_HalContext *s_pHalContext;

/**
 * @brief Beginning BouncingSquares section
 *
 * @param BRx
 * @param BRy
 * @param MovingRy
 * @param SqNumber
 */
void BouncingSquaresCall(int16_t BRx, int16_t BRy, int16_t MovingRy, uint8_t SqNumber)
{
	int16_t MovingRx;

	int16_t R1;
	int16_t G1;
	int16_t B_1;
	int16_t R2;
	int16_t G2;
	int16_t B2;
	MovingRx = BRx;

	if (BRy <= 60)BRy = 60;
	if (BRy >= 260)BRy = 260;

	//different colours are set for the different rectangles
	if (SqNumber == 0) {
		R1 = 63; G1 = 72;  B_1 = 204;
		R2 = 0;  G2 = 255; B2 = 255;
	}

	if (SqNumber == 1)
	{
		R1 = 255; G1 = 255;  B_1 = 0;
		R2 = 246;  G2 = 89; B2 = 12;
	}

	if (SqNumber == 2)
	{
		R1 = 255; G1 = 0;  B_1 = 0;
		R2 = 237;  G2 = 28; B2 = 36;
	}

	if (SqNumber == 3)
	{
		R1 = 131; G1 = 171;  B_1 = 9;
		R2 = 8;  G2 = 145; B2 = 76;
	}

	if (SqNumber == 4)
	{
		R1 = 141; G1 = 4;  B_1 = 143;
		R2 = 176;  G2 = 3; B2 = 89;
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
}

/**
 * @brief API to demonstrate moving rectangle
 *
 * @param BRy Deprecated
 * @param MovingRy Moving rectangle y
 * @param EndPtReach Is end moving
 * @return int16_t Moving rectangle y
 */
int16_t MovingRect(int16_t MovingRy, uint8_t EndPtReach)
{
	if (MovingRy <= 0)
	{
		EndPtReach = 0;
		MovingRy = 1;
	}

	if (EndPtReach == 1 && MovingRy > 0)
		MovingRy -= 1;//the smaller rectangles are moved behind
	else if (EndPtReach == 0)
		MovingRy += 2;//the smaller rectangles are moved forward slightly faster
	return MovingRy;
}

/**
 * @brief Calculate rectangle limits and positions
 *
 * @param context BouncingSquares context
 * @param Arrayno Rectangle id
 */
void RectangleCalc(SAMAPP_BouncingSquares_t* context, uint8_t Arrayno)
{
	uint8_t Arr;
	int16_t MovingRy1;
	int16_t leap = 0;

	if (context->RectNo[Arrayno] == 1)
	{
		Arr = Arrayno;
		//the limits for the smaller rectangles forward and backward movement is set here
		if (context->My[Arr] == 0 && (context->My[Arr] + 25) < context->BRy[Arr])
			context->E[Arr] = 0;//inc
		else if (context->My[Arr] + 25 >= context->BRy[Arr])
			context->E[Arr] = 1;//dec

		// the smaller rectangles are moved accordingly according to the flags set above ion this function call
		MovingRy1 = MovingRect(context->My[Arr], context->E[Arr]);

		if (context->BRy[Arr] == 0)MovingRy1 = 4;
		context->My[Arr] = MovingRy1;

		if (context->My[Arr] > (context->BRy[Arr] - 15))
		{
			leap = context->My[Arr] - context->BRy[Arr];
			context->My[Arr] = context->My[Arr] - (leap + 25);
		}
	}
}

/**
 * @brief Check user touches
 *
 * @param context BouncingSquares context
 * @param Tx1 Touch x position
 * @param val1 Multi touch value
 */
void CheckTouch(SAMAPP_BouncingSquares_t* context, int16_t Tx1, int32_t val1)
{
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
	if (Arrayno < 255) {
		context->RectNo[Arrayno] = 1;

		//store the vertices of the rectangle selected according to the flag
		if ((val1 >> 16) != -32768)
		{
			context->BRx[Arrayno] = (val1 >> 16) & 0xffff;
			context->BRy[Arrayno] = (val1 & 0xffff);
		}

		//limit the Bigger rectangle's height
		if (context->BRy[Arrayno] <= 60) context->BRy[Arrayno] = 60;
	}

	//According to the bigger rectangle values move the smaller rectangles
	for (int i = 0; i < NO_OF_RECTS; i++)
	{
		RectangleCalc(context, (uint8_t)i);
	}
}

/**
 * @brief Draw Bouncing squares
 *
 */
void BouncingSquares()
{
	int16_t RectX[5];
	int32_t val[5];

	SAMAPP_BouncingSquares_t context;

	//Calculate the X vertices where the five rectangles have to be placed

	for (int i = 1; i < 5; i++)
	{
		RectX[0] = 60;
		RectX[i] = RectX[i - 1] + 80;
	}

	for (int i = 0; i < 5; i++) {
		context.BRy[i] = 0;
		context.My[i] = 0;
		context.RectNo[i] = 0;
		context.E[i] = 0;
	}

#if defined(EVE_SUPPORT_CAPACITIVE)
	EVE_Hal_wr8(s_pHalContext, REG_CTOUCH_EXTENDED, CTOUCH_MODE_EXTENDED);
#endif
	EVE_sleep(30);
	for (int k = 0; k < 300; k++) {
		/* first touch*/
		val[0] = EVE_Hal_rd32(s_pHalContext, REG_CTOUCH_TOUCH0_XY);

		/*second touch*/
		val[1] = EVE_Hal_rd32(s_pHalContext, REG_CTOUCH_TOUCH1_XY);

		/*third touch*/
		val[2] = EVE_Hal_rd32(s_pHalContext, REG_CTOUCH_TOUCH2_XY);

		/*fourth  touch*/
		val[3] = EVE_Hal_rd32(s_pHalContext, REG_CTOUCH_TOUCH3_XY);

		/*fifth  touch*/
		val[4] = ((uint32_t)EVE_Hal_rd16(s_pHalContext, REG_CTOUCH_TOUCH4_X) << 16L) | (EVE_Hal_rd16(s_pHalContext, REG_CTOUCH_TOUCH4_Y) & 0xffffL);

		EVE_CoCmd_dlStart(s_pHalContext);
		EVE_Cmd_wr32(s_pHalContext, CLEAR_COLOR_RGB(0, 0, 0));
		EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1));

		//Check which rectangle is being touched using the coordinates and move the respective smaller rectangle
		for (int8_t i = 0; i < NO_OF_RECTS; i++)
		{
			CheckTouch(&context, (val[i] >> 16) & 0xffffL, val[i]);
			BouncingSquaresCall(RectX[i], context.BRy[i], context.My[i], i);
		}

		EVE_Cmd_wr32(s_pHalContext, DISPLAY());
		EVE_CoCmd_swap(s_pHalContext);
		EVE_Cmd_waitFlush(s_pHalContext);
		context.Count++;

	}
#if defined(EVE_SUPPORT_CAPACITIVE)
	EVE_Hal_wr8(s_pHalContext, REG_CTOUCH_EXTENDED, CTOUCH_MODE_COMPATIBILITY);
#endif
}
/* End BouncingSquares section */

/**
 * @brief Beginning BouncingCircle section
 *
 * @param C1 Point size
 * @param R Deprecated
 * @param G Deprecated
 * @param B Deprecated
 */
void ConcentricCircles(float C1, uint16_t R, uint16_t G, uint16_t B)
{
	EVE_Cmd_wr32(s_pHalContext, CLEAR_COLOR_A(0));

	/* Using ColorMask to disable color buffer updates, and
	set the BlendFunc to a value that writes 	incoming alpha
	directly into the alpha buffer, by specifying a source blend factor
	of ONE */

	EVE_Cmd_wr32(s_pHalContext, COLOR_MASK(0, 0, 0, 1));
	EVE_Cmd_wr32(s_pHalContext, BLEND_FUNC(ONE, ONE_MINUS_SRC_ALPHA));

	/*Draw the Outer circle */

	EVE_Cmd_wr32(s_pHalContext, BEGIN(FTPOINTS));
	EVE_Cmd_wr32(s_pHalContext, POINT_SIZE((uint16_t)(C1 * 16)));		//outer circle
	EVE_Cmd_wr32(s_pHalContext, VERTEX2II(240, 136, 0, 0));

	/* Draw the inner circle in a blend mode that clears any drawn
	pixels to zero, so the source blend factor is ZERO */
	EVE_Cmd_wr32(s_pHalContext, BLEND_FUNC(ZERO, ONE_MINUS_SRC_ALPHA));

	EVE_Cmd_wr32(s_pHalContext, POINT_SIZE((uint16_t)((C1 - 2) * 16)));//inner circle
	EVE_Cmd_wr32(s_pHalContext, VERTEX2II(240, 136, 0, 0));

	/*Enable the color Mask and the source blend factor is set to DST ALPHA, so the
	transparency values come from the alpha buffer*/

	EVE_Cmd_wr32(s_pHalContext, COLOR_MASK(1, 1, 1, 0));

	EVE_Cmd_wr32(s_pHalContext, BLEND_FUNC(DST_ALPHA, ONE));

	/* draw the outer circle again with the color mask enabled andd the blend factor
	is set to SRC_ALPHA */
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(R, G, B));
	EVE_Cmd_wr32(s_pHalContext, POINT_SIZE((uint16_t)(C1 * 16)));
	EVE_Cmd_wr32(s_pHalContext, VERTEX2II(240, 136, 0, 0));

	EVE_Cmd_wr32(s_pHalContext, BLEND_FUNC(SRC_ALPHA, ONE_MINUS_SRC_ALPHA));

	EVE_Cmd_wr32(s_pHalContext, END());
}

/**
 * @brief Draw touch points
 *
 * @param C1X Point X
 * @param C1Y Point Y
 * @param i Point number
 */
void TouchPoints(int16_t C1X, int16_t C1Y, uint8_t i)
{
	/* Draw the five white circles for the Touch areas with their rescpective numbers*/
	EVE_Cmd_wr32(s_pHalContext, BEGIN(FTPOINTS));
	EVE_Cmd_wr32(s_pHalContext, POINT_SIZE((14) * 16));
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 255, 255));
	EVE_Cmd_wr32(s_pHalContext, VERTEX2II(C1X, C1Y, 0, 0));
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(155, 155, 0));
	EVE_CoCmd_number(s_pHalContext, C1X, C1Y, 29, OPT_CENTERX | OPT_CENTERY, i);
}

/**
 * @brief Draw plots
 *
 */
void PlotXY()
{
	uint8_t i = 0;
	uint16_t PlotHt = 0;
	uint16_t PlotWth = 0;
	uint16_t X = 0;
	uint16_t Y = 0;

	PlotHt = (uint16_t)(s_pHalContext->Height / 10);
	PlotWth = (uint16_t)(s_pHalContext->Width / 10);

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
}

/**
 * @brief check which circle has been touched based on the coordinates
 *
 * @param context BouncingSquares context
 * @param val Touch value
 * @param TouchNum Touch number
 * @param i Circle number
 */
void CheckCircleTouchCood(SAMAPP_BouncingCircles_t* context, int32_t val, uint8_t TouchNum, uint8_t i)
{
	double CX = 0;
	double CY = 0;

	uint8_t AllClear = 0;

	if ((val >> 16) == -32768) {
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

	if (AllClear == 1
		&& context->TN[TouchNum].F[i] != 1
		&& (CX > (context->C1X[i] - 15))
		&& (CX < (context->C1X[i] + 15))
		&& (CY > (context->C1Y[i] - 30))
		&& (CY < context->C1Y[i] + 30))
	{
		context->C1X[i] = (float)CX;
		context->C1Y[i] = (float)CY;
		context->TN[TouchNum].F[i] = 1;
	}

	if (context->TN[TouchNum].F[i] == 1)
	{
		context->C1X[i] = (float)CX;
		context->C1Y[i] = (float)CY;
	}

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
uint16_t CirclePlot(SAMAPP_BouncingCircles_t* context, uint16_t X, uint16_t Y, uint8_t Val)
{
	double Xsq1[NO_OF_CIRCLE];
	double Ysq1[NO_OF_CIRCLE];
	Xsq1[Val] = (X - (s_pHalContext->Width / 2)) * (X - (s_pHalContext->Width / 2));
	Ysq1[Val] = (Y - (s_pHalContext->Height / 2)) * (Y - (s_pHalContext->Height / 2));
	context->Tsq1[Val] = (float)(Xsq1[Val] + Ysq1[Val]);
	context->Tsq1[Val] = (float)sqrt(context->Tsq1[Val]);
	return (uint16_t)context->Tsq1[Val];
}

/**
 * @brief Store touches to context
 *
 * @param context BouncingSquares context
 * @param Touchval Touch value
 * @param TouchNo Touch number
 */
void StoreTouch(SAMAPP_BouncingCircles_t* context, int32_t Touchval, uint8_t TouchNo)
{
	if (Touchval >> 16 != -32768)
	{
		context->TouchX[TouchNo] = (float)(Touchval >> 16);
		context->TouchY[TouchNo] = (float)(Touchval & 0xffff);
	}
}

/**
 * @brief Draw Bouncing Squares
 *
 */
void BouncingCircles()
{
	int32_t Touchval[NO_OF_CIRCLE];
	SAMAPP_BouncingCircles_t context;

#if defined(EVE_SUPPORT_CAPACITIVE)
	EVE_Hal_wr8(s_pHalContext, REG_CTOUCH_EXTENDED, CTOUCH_MODE_EXTENDED);
#endif
	EVE_sleep(30);
	/* calculate the intital radius of the circles before the touch happens*/
	context.Tsq1[0] = 50; context.C1X[0] = 190; context.C1Y[0] = 136;
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
		Touchval[4] = ((int32_t)EVE_Hal_rd16(s_pHalContext, REG_CTOUCH_TOUCH4_X) << 16) | (EVE_Hal_rd16(s_pHalContext, REG_CTOUCH_TOUCH4_Y));

		for (int8_t i = 0; i < NO_OF_CIRCLE; i++)
		{
			StoreTouch(&context, Touchval[i], i);
		}
		/* The plot is drawn here */
		PlotXY();

		/* check which circle has been touched based on the coordinates and store the[0] number of the circle touched*/

		for (int8_t i = 0; i < NO_OF_CIRCLE; i++)
		{
			CheckCircleTouchCood(&context, Touchval[0], 0, i);
			CheckCircleTouchCood(&context, Touchval[1], 1, i);
			CheckCircleTouchCood(&context, Touchval[2], 2, i);
			CheckCircleTouchCood(&context, Touchval[3], 3, i);
			CheckCircleTouchCood(&context, Touchval[4], 4, i);
		}
		/* calculate the radius of each circle according to the touch of each individual circle */

		for (int8_t i = 0; i < NO_OF_CIRCLE; i++)
		{
			context.Tsq1[i] = (float)CirclePlot(&context, (uint16_t)context.C1X[i], (uint16_t)context.C1Y[i], i);
		}
		/* with the calculated radius draw the circles as well as the Touch points */

		for (int8_t i = 0; i < (NO_OF_CIRCLE); i++)
		{
			ConcentricCircles(context.Tsq1[i], 255, 0, 0);
			TouchPoints((int16_t)context.C1X[i], (int16_t)context.C1Y[i], i + 1);
		}

		EVE_Cmd_wr32(s_pHalContext, DISPLAY());
		EVE_CoCmd_swap(s_pHalContext);
		EVE_Cmd_waitFlush(s_pHalContext);

	}
}
/* End BouncingCircle section */

/**
 * @brief Beginning BouncingPoints section
 *
 * @param pBInst Blob instance
 * @param TouchXY Touch value
 */
void BlobColor(SAMAPP_BlobsInst_t* pBInst, int32_t TouchXY)
{
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
			EVE_Cmd_wr32(s_pHalContext, VERTEX2F((pBInst->blobs[j].x) * 16, (pBInst->blobs[j].y) * 16));
	}
}

/**
 * @brief Draw Bouncing points
 *
 */
void BouncingPoints()
{
	int32_t val[5];
	SAMAPP_BlobsInst_t gBlobsInst[APP_BLOBS_NUMTOUCH];
	SAMAPP_BlobsInst_t* pBInst;
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
		val[4] = (((int32_t)EVE_Hal_rd16(s_pHalContext, REG_CTOUCH_TOUCH4_X) << 16) | (EVE_Hal_rd16(s_pHalContext, REG_CTOUCH_TOUCH4_Y) & 0xffff));

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
			BlobColor(&gBlobsInst[j], val[j]);
		}

		EVE_Cmd_wr32(s_pHalContext, DISPLAY());
		EVE_CoCmd_swap(s_pHalContext);
		EVE_Cmd_waitFlush(s_pHalContext);
	}

#if defined(EVE_SUPPORT_CAPACITIVE)
	EVE_Hal_wr8(s_pHalContext, REG_CTOUCH_EXTENDED, CTOUCH_MODE_COMPATIBILITY);
#endif
}
/* End BouncingPoints section */

/**
 * @brief Beginning MovingPoints section
 *
 * @param k Point set
 * @param i POint number
 */
void ColorSelection(int16_t k, int16_t i)
{
	if (k == 0)
	{
		if (i & 1)
			EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(128, 0, 255));//purple
		else
			EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 43, 149));//pink
	}
	if (k == 1)
	{
		if (i & 1)
			EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 0, 0));//red
		else
			EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0, 255, 0));//green
	}
	if (k == 2)
	{
		if (i & 1)
			EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 128, 64));//orange
		else
			EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0, 255, 255));//blue
	}
	if (k == 3)
	{
		if (i & 1)
			EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(128, 0, 0));//orange
		else
			EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 255, 128));//blue
	}
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
int16_t linear(float p1, float p2, uint16_t t, uint16_t rate)
{
	float st = (float)t / rate;
	return (int16_t)(p1 + (st * (p2 - p1)));
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
void PointsCalc(SAMAPP_MovingPoints_t* context, uint8_t* t)
{
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

		ColorSelection(pointset, k);

		if (context->X[k] != -32768)
		{
			tempDeltaX = linear(context->X[k], context->SmallX[pointset], context->t[k], NO_OF_POINTS);
			tempDeltaY = linear(context->Y[k], context->SmallY, context->t[k], NO_OF_POINTS);
			EVE_Cmd_wr32(s_pHalContext, VERTEX2F(tempDeltaX * 16L, tempDeltaY * 16L));
		}
		t[k]++;
	}
}

/**
 * @brief Move points
 *
 */
void MovingPoints()
{
	SAMAPP_MovingPoints_t context;
	context.Flag = 1;
#if defined(EVE_SUPPORT_CAPACITIVE)
	EVE_Hal_wr8(s_pHalContext, REG_CTOUCH_EXTENDED, CTOUCH_MODE_EXTENDED);
#endif
	EVE_sleep(30);
	/* Initialize all coordinates */
	for (uint16_t j = 0; j < 4; j++) {
		for (uint16_t i = 0; i < NO_OF_POINTS; i++) {
			context.t[i + j * NO_OF_POINTS] = (uint8_t)i;
			context.X[i + j * NO_OF_POINTS] = -32768;
		}
	}

	context.SmallX[0] = 180; context.SmallY = 20;
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
		PointsCalc(&context, &context.t[0]);
		EVE_Cmd_wr32(s_pHalContext, DISPLAY());
		EVE_CoCmd_swap(s_pHalContext);
		EVE_Cmd_waitFlush(s_pHalContext);
	}
#if defined(EVE_SUPPORT_CAPACITIVE)
	EVE_Hal_wr8(s_pHalContext, REG_CTOUCH_EXTENDED, CTOUCH_MODE_COMPATIBILITY);
#endif
}
/* End MovingPoints section */

/**
 * @brief Draw set 7
 *
 */
void App_Set_07_Draw() {
	Draw_Text(s_pHalContext, "Multi-touch on screen.");
	EVE_sleep(2000);
	Draw_Text(s_pHalContext, "Bouncing Squares");
	BouncingSquares();
#if defined(MSVC_PLATFORM) || defined(BT8XXEMU_PLATFORM)
	Draw_Text(s_pHalContext, "Bouncing Circles");
	BouncingCircles();
#endif
	Draw_Text(s_pHalContext, "Bouncing Points");
	BouncingPoints();

	Draw_Text(s_pHalContext, "Moving Points");
	MovingPoints();
}

/**
 * @brief Init set 7
 *
 */
void App_Set_07_Init(EVE_HalContext* pHalContext) {
	s_pHalContext = pHalContext;
	Draw_Text(s_pHalContext, "Set7   Start!");
}

/**
 * @brief Deinit set 7
 *
 */
void App_Set_07_Deinit() {
	Draw_Text(s_pHalContext, "Set7   End!");
}

#endif // SAMAPP_ENABLE_APIS_SET7
