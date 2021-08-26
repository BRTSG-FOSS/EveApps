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
  *  Description: This is sample demo for EVE functions: Setfont
  */

#include "Platform.h"
#include "Gpu_Hal.h"
#include "Common.h"
#include "App.h"
#include "Samapp_Enable_API.h"

#ifdef SAMAPP_ENABLE_APIS_SET3

static EVE_HalContext *s_pHalContext;
/**
 * @brief API to demonstrate custom font display
 *
 */
void SAMAPP_CoPro_Setfont()
{
	uint8_t* pbuff;
	uint8_t FontIdxTable[148];
	uint32_t fontaddr = (128 + 5 * 4);
	int32_t FileLen = 0;
	uint16_t blocklen = 128 + 5 * 4;//header size

	pbuff = (uint8_t*)malloc(8192);

	/*************************************************************************/
	/* Below code demonstrates the usage of setfont. Setfont function draws  */
	/* custom configured fonts on screen. Download the font table and raw    */
	/* font data in L1/L4/L8 format and disply text                          */
	/*************************************************************************/
#if defined(MSVC_PLATFORM) || defined(BT8XXEMU_PLATFORM)
	memset(FontIdxTable, 16, 128);
	fontaddr = 0x00000003;//l8 format
	memcpy(&FontIdxTable[128], &fontaddr, 4);
	fontaddr = 16;//stride
	memcpy(&FontIdxTable[128 + 4], &fontaddr, 4);
	fontaddr = 16;//max width
	memcpy(&FontIdxTable[128 + 8], &fontaddr, 4);
	fontaddr = 16;//max height
	memcpy(&FontIdxTable[128 + 12], &fontaddr, 4);
	fontaddr = (1024);//data address - starts at location 1024
	memcpy(&FontIdxTable[128 + 16], &fontaddr, 4);

	EVE_Hal_wrProgMem(s_pHalContext, RAM_G, FontIdxTable, (128 + 5 * 4));

	/* download the jpeg image and decode */
	/* Characters from 32 to 128 are present and each character is 16*16 dimention */
	FILE* pFile = fopen(TEST_DIR "\\font16.jpg", "rb");
	if (NULL == pFile)
	{
		printf("Error in opening file %s \n", "font16.jpg");
	}
	else
	{
		/******************* Decode jpg output into location 0 and output color format as RGB565 *********************/
		EVE_Cmd_wr32(s_pHalContext, CMD_LOADIMAGE);
		EVE_Cmd_wr32(s_pHalContext, (9216));//destination address of jpg decode
		EVE_Cmd_wr32(s_pHalContext, OPT_MONO);//output format of the bitmap
		fseek(pFile, 0, SEEK_END);
		FileLen = ftell(pFile);
		fseek(pFile, 0, SEEK_SET);
		while (FileLen > 0)
		{
			/* download the data into the command buffer by 2kb one shot */
			blocklen = FileLen > 8192 ? 8192 : (uint16_t)FileLen;

			/* copy the data into pbuff and then transfter it to command buffer */
			fread(pbuff, 1, blocklen, pFile);
			FileLen -= blocklen;
			/* copy data continuously into command memory */
			EVE_Cmd_wrMem(s_pHalContext, pbuff, blocklen); //alignment is already taken care by this api
		}
		fclose(pFile);
	}

	EVE_CoCmd_dlStart(s_pHalContext);
	EVE_Cmd_wr32(s_pHalContext, CLEAR_COLOR_RGB(0xff, 0xff, 0xff));//set the background to white
	EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1));
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(32, 32, 32));//black color text

	EVE_CoCmd_text(s_pHalContext, (int16_t)(s_pHalContext->Width / 2), 20, 27, OPT_CENTER, "SetFont - format L8");
	EVE_Cmd_wr32(s_pHalContext, BITMAP_HANDLE(7));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_SOURCE(1024));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_LAYOUT(L8, 16, 16));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_SIZE(NEAREST, BORDER, BORDER, 16, 16));

	EVE_CoCmd_setFont(s_pHalContext, 7, 0);

	EVE_CoCmd_text(s_pHalContext, (int16_t)(s_pHalContext->Width / 2), 80, 7, OPT_CENTER, "The quick brown fox jumps");
	EVE_CoCmd_text(s_pHalContext, (int16_t)(s_pHalContext->Width / 2), 120, 7, OPT_CENTER, "over the lazy dog.");
	EVE_CoCmd_text(s_pHalContext, (int16_t)(s_pHalContext->Width / 2), 160, 7, OPT_CENTER, "1234567890");

	EVE_Cmd_wr32(s_pHalContext, DISPLAY());
	EVE_CoCmd_swap(s_pHalContext);

	/* Wait till coprocessor completes the operation */
	EVE_Cmd_waitFlush(s_pHalContext);

	EVE_sleep(1000);//timeout for snapshot to be performed by coprocessor
#endif

	/* Display custom font by reading from the binary file - header of 148 bytes is at the starting followed by font data of 96 characters */
	/*Roboto-BoldCondensed.ttf*/
	fontaddr = RAM_G;//header size
	blocklen = 128 + 5 * 4;//header size
	FileLen = FileIO_File_Open(TEST_DIR "\\Roboto-BoldCondensed_12.bin", FILEIO_E_FOPEN_READ);

	if (0 == FileLen) {
		printf("Error in opening file %s \n", "Roboto-BoldCondensed_12.bin");
		free(pbuff);
		return;
	}
	//first 148 bytes in the file is the header and later is the raw data for ascii 32 to 128 index charaters
	FileIO_File_Read(pbuff, blocklen);
	uint32_t* ptemp = (uint32_t*)&pbuff[128 + 4 * 4];
	*ptemp = 1024;//download the font data at location 1024+32*8*25
	for (int i = 0; i < 32; i++)
	{
		pbuff[i] = 16;
	}

	/* Modify the font data location */
	EVE_Hal_wrMem(s_pHalContext, fontaddr, pbuff, blocklen);

	/* Next download the data at location 32*8*25 - skip the first 32 characters */
	/* each character is 8x25 bytes */
	fontaddr += (1024 + 32 * 8 * 25);//make sure space is left at the starting of the buffer for first 32 characters - TBD manager this buffer so that this buffer can be utilized by other module

	FileLen -= blocklen;
	while (FileLen > 0)
	{
		/* download the data into the command buffer by 8kb one shot */
		blocklen = FileLen > 8192 ? 8192 : (uint16_t)FileLen;

		/* copy the data into pbuff and then transfter it to command buffer */
		FileIO_File_Read(pbuff, blocklen);

		/* copy data continuously into command memory */
		EVE_Hal_wrMem(s_pHalContext, fontaddr, pbuff, blocklen);
		FileLen -= blocklen;
		fontaddr += blocklen;
	}
	free(pbuff);
	FileIO_File_Close();

	EVE_CoCmd_dlStart(s_pHalContext);
	EVE_Cmd_wr32(s_pHalContext, CLEAR_COLOR_RGB(0xff, 0xff, 0xff));//set the background to white
	EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1));
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(32, 32, 32));//black color text

	EVE_CoCmd_text(s_pHalContext, (int16_t)(s_pHalContext->Width / 2), 20, 27, OPT_CENTER, "SetFont - format L4");
	EVE_Cmd_wr32(s_pHalContext, BITMAP_HANDLE(6));//give index table 6
	EVE_Cmd_wr32(s_pHalContext, BITMAP_SOURCE(1024));//make the address to 0
	EVE_Cmd_wr32(s_pHalContext, BITMAP_LAYOUT(L4, 8, 25));//stride is 8 and height is 25
	EVE_Cmd_wr32(s_pHalContext, BITMAP_SIZE(NEAREST, BORDER, BORDER, 16, 25));//width is 16 and height is 25

	EVE_CoCmd_setFont(s_pHalContext, 6, 0);
	EVE_CoCmd_text(s_pHalContext, (int16_t)(s_pHalContext->Width / 2), 80, 6, OPT_CENTER, "The quick brown fox jumps");
	EVE_CoCmd_text(s_pHalContext, (int16_t)(s_pHalContext->Width / 2), 120, 6, OPT_CENTER, "over the lazy dog.");
	EVE_CoCmd_text(s_pHalContext, (int16_t)(s_pHalContext->Width / 2), 160, 6, OPT_CENTER, "1234567890");

	EVE_Cmd_wr32(s_pHalContext, DISPLAY());
	EVE_CoCmd_swap(s_pHalContext);

	/* Wait till coprocessor completes the operation */
	EVE_Cmd_waitFlush(s_pHalContext);

	EVE_sleep(2000);//timeout for snapshot to be performed by coprocessor
}

/**
 * @brief Draw set 3
 *
 */
void App_Set_03_Draw() {
	SAMAPP_CoPro_Setfont();
}

/**
 * @brief Init set 3
 *
 */
void App_Set_03_Init(EVE_HalContext* pHalContext) {
	s_pHalContext = pHalContext;
	Draw_Text(s_pHalContext, "Set3   START");
}

/**
 * @brief Deinit set 3
 *
 */
void App_Set_03_Deinit() {
	Draw_Text(s_pHalContext, "Set3   END!");
}

#endif // SAMAPP_ENABLE_APIS_SET3
