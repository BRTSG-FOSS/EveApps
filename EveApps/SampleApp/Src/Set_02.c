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
  *  Description: This is sample demo for below EVE functions:
  *     > Inflate
  *     > Loadimage
  */

#include "Platform.h"
#include "Gpu_Hal.h"
#include "Common.h"
#include "App.h"
#include "Samapp_Enable_API.h"

#ifdef SAMAPP_ENABLE_APIS_SET2

static EVE_HalContext *s_pHalContext;
/**
 * @brief API to demonstrate the usage of inflate command - compression done via zlib
 *
 */
void SAMAPP_CoPro_Inflate()
{
#define BUFFERSIZE 8192
	char pbuff[BUFFERSIZE];
	const SAMAPP_Bitmap_header_t* pBitmapHdr = NULL;
	char* file = TEST_DIR "\\lenaface40.bin";
	int16_t xoffset;
	int16_t yoffset;

	if (0 >= FileIO_File_Open(file, FILEIO_E_FOPEN_READ)) {
		printf("Error in opening file %s \n", file);
		return;
	}
	/*************************************************************************/
	/* Below code demonstrates the usage of inflate functiona                */
	/* Download the deflated data into command buffer and in turn coprocessor infaltes*/
	/* the deflated data and outputs at 0 location                           */
	/*************************************************************************/
	pBitmapHdr = &SAMAPP_Bitmap_RawData_Header[0];

	xoffset = (int16_t)((s_pHalContext->Width - SAMAPP_Bitmap_RawData_Header[0].Width) / 2);
	yoffset = (int16_t)((s_pHalContext->Height - SAMAPP_Bitmap_RawData_Header[0].Height) / 2);

	/* Clear the memory at location 0 - any previous bitmap data */

	EVE_Cmd_wr32(s_pHalContext, CMD_MEMSET);
	EVE_Cmd_wr32(s_pHalContext, 0L);//starting address of memset
	EVE_Cmd_wr32(s_pHalContext, 255L);//value of memset
	EVE_Cmd_wr32(s_pHalContext, 1L * pBitmapHdr->Stride * pBitmapHdr->Height);//number of elements to be changed

	/* Set the display list for graphics processor */
	/* Bitmap construction by MCU - display lena at 200x90 offset */
	/* Transfer the data into coprocessor memory directly word by word */
	EVE_Cmd_wr32(s_pHalContext, CMD_DLSTART);
	EVE_Cmd_wr32(s_pHalContext, CLEAR_COLOR_RGB(0, 0, 255));
	EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1));
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 255, 255));
	EVE_Cmd_wr32(s_pHalContext, BEGIN(BITMAPS));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_SOURCE(0));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_LAYOUT(SAMAPP_Bitmap_RawData_Header[0].Format,
		SAMAPP_Bitmap_RawData_Header[0].Stride, SAMAPP_Bitmap_RawData_Header[0].Height));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_SIZE(BILINEAR, BORDER, BORDER,
		SAMAPP_Bitmap_RawData_Header[0].Width, SAMAPP_Bitmap_RawData_Header[0].Height));
	EVE_Cmd_wr32(s_pHalContext, VERTEX2F(xoffset * 16, yoffset * 16));
	EVE_Cmd_wr32(s_pHalContext, END());

	/*  Display the text information */
	EVE_Cmd_wr32(s_pHalContext, COLOR_A(255));
	xoffset -= 50;
	yoffset += 40;
	EVE_CoCmd_text(s_pHalContext, xoffset, yoffset, 26, 0, "Display bitmap by inflate");
	EVE_Cmd_wr32(s_pHalContext, DISPLAY());

	EVE_CoCmd_swap(s_pHalContext);
	/* Wait till coprocessor completes the operation */
	EVE_Cmd_waitFlush(s_pHalContext);

	/* inflate the data read from binary file */
	EVE_Cmd_wr32(s_pHalContext, CMD_INFLATE);
	EVE_Cmd_wr32(s_pHalContext, 0);//destination address if inflate

	int bytes = FileIO_File_Read(pbuff, BUFFERSIZE);
	while (bytes)
	{
		/* download the data into the command buffer by 2kb one shot */
		uint16_t blocklen = bytes > BUFFERSIZE ? BUFFERSIZE : (uint16_t)bytes;

		/* copy data continuously into command memory */
		EVE_Cmd_wrMem(s_pHalContext, pbuff, blocklen);//alignment is already taken care by this api
		bytes = FileIO_File_Read(pbuff, BUFFERSIZE);
	}
	EVE_Cmd_waitFlush(s_pHalContext);
	SAMAPP_ENABLE_DELAY();
}

/**
 * @brief demonstrates the usage of loadimage function
 * Download the jpg data into command buffer and in turn coprocessor decodes and dumps into location 0 with rgb565 format
 *
 */
void SAMAPP_CoPro_Loadimage_Mono() {
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

	/* decode the jpeg data */
	if (0 >= FileIO_File_Open(file, FILEIO_E_FOPEN_READ)) {
		printf("Error in opening file %s \n", "mandrill256.jpg");
		return;
	}
	pbuff = (uint8_t*)malloc(8192);

	/// Decode jpg output into location 0 and output color format as L8
	Display_Start(s_pHalContext);
	EVE_Cmd_wr32(s_pHalContext, CMD_LOADIMAGE);
	EVE_Cmd_wr32(s_pHalContext, 0);//destination address of jpg decode
	EVE_Cmd_wr32(s_pHalContext, OPT_MONO);//output format of the bitmap

	int bytes = FileIO_File_Read(pbuff, BUFFERSIZE);
	while (bytes)
	{
		uint16_t blocklen = bytes > BUFFERSIZE ? BUFFERSIZE : (uint16_t)bytes;

		/* copy data continuously into command memory */
		EVE_Cmd_wrMem(s_pHalContext, pbuff, blocklen);//alignment is already taken care by this api
		bytes = FileIO_File_Read(pbuff, BUFFERSIZE);
	}
	free(pbuff);
	EVE_Cmd_waitFlush(s_pHalContext);

	EVE_Cmd_wr32(s_pHalContext, BEGIN(BITMAPS));
	EVE_Cmd_wr32(s_pHalContext, VERTEX2F((int16_t)(xoffset * 16), (int16_t)(yoffset * 16)));
	EVE_Cmd_wr32(s_pHalContext, END());

	xoffset = s_pHalContext->Width / 2;
	yoffset = s_pHalContext->Height / 2;
	EVE_CoCmd_text(s_pHalContext, (int16_t)xoffset, (int16_t)yoffset, 26, OPT_CENTER, "Display bitmap by jpg decode L8");

	Display_End(s_pHalContext);
	SAMAPP_ENABLE_DELAY();
}

/**
 * @brief Load image to Coprocessor full color
 *
 */
void SAMAPP_CoPro_Loadimage_FullColor() {
#define BUFFERSIZE 8192
	uint8_t* pbuff;
	int16_t ImgW;
	int16_t ImgH;
	int16_t xoffset;
	int16_t yoffset;
	const char* file = TEST_DIR "\\mandrill256.jpg";

	ImgW = ImgH = 256;
	xoffset = (int16_t)((s_pHalContext->Width - ImgW) / 2);
	yoffset = (int16_t)((s_pHalContext->Height - ImgH) / 2);

	/* decode the jpeg data */
	if (0 >= FileIO_File_Open(file, FILEIO_E_FOPEN_READ)) {
		printf("Error in opening file %s \n", "mandrill256.jpg");
		return;
	}
	pbuff = (uint8_t*)malloc(8192);

	/// Decode jpg output into location 0 and output color format as L8
	Display_Start(s_pHalContext);
	EVE_Cmd_wr32(s_pHalContext, CMD_LOADIMAGE);
	EVE_Cmd_wr32(s_pHalContext, 0);//destination address of jpg decode
	EVE_Cmd_wr32(s_pHalContext, 0);//output format of the bitmap

	int bytes = FileIO_File_Read(pbuff, BUFFERSIZE);
	while (bytes)
	{
		uint16_t blocklen = bytes > BUFFERSIZE ? BUFFERSIZE : (uint16_t)bytes;

		/* copy data continuously into command memory */
		EVE_Cmd_wrMem(s_pHalContext, pbuff, blocklen);//alignment is already taken care by this api
		bytes = FileIO_File_Read(pbuff, BUFFERSIZE);
	}
	free(pbuff);
	EVE_Cmd_waitFlush(s_pHalContext);

	EVE_Cmd_wr32(s_pHalContext, BEGIN(BITMAPS));
	EVE_Cmd_wr32(s_pHalContext, VERTEX2F(xoffset * 16, yoffset * 16));
	EVE_Cmd_wr32(s_pHalContext, END());

	xoffset = (int16_t)((s_pHalContext->Width) / 2);
	yoffset = (int16_t)((s_pHalContext->Height) / 2);
	EVE_CoCmd_text(s_pHalContext, xoffset, yoffset, 26, OPT_CENTER, "Display bitmap by jpg decode");

	Display_End(s_pHalContext);
	SAMAPP_ENABLE_DELAY();
}

/**
 * @brief Draw set 2
 *
 */
void App_Set_02_Draw() {
	SAMAPP_CoPro_Inflate();
	SAMAPP_CoPro_Loadimage_Mono();
	SAMAPP_CoPro_Loadimage_FullColor();
}

/**
 * @brief Init set 2
 *
 */
void App_Set_02_Init(EVE_HalContext* pHalContext) {
	s_pHalContext = pHalContext;
	Draw_Text(s_pHalContext, "Set2   START");
}

/**
 * @brief Deinit set 2
 *
 */
void App_Set_02_Deinit() {
	Draw_Text(s_pHalContext, "Set2   END!");
}

#endif // SAMAPP_ENABLE_APIS_SET2
