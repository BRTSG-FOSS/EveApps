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
  *     > Flash_State
  *     > Program_Flash
  *     > ASTC
  *     > Mediafifo_CmdInflate
  *     > GradientA
  *     > Cmd_FillWidth
  *     > Rotate
  *     > Animation
  *     > Video_Flash
  *     > ExtendedFormat_Font
  *     > ResetFont
  *     > PrintfType
  *     > CoprocessorFaults
  */

#include "Platform.h"
#include "Gpu_Hal.h"
#include "Common.h"
#include "App.h"
#include "Samapp_Enable_API.h"

#ifdef SAMAPP_ENABLE_APIS_SET10

static EVE_HalContext *s_pHalContext;

#define REG_COPRO_PATCH_PTR  3182946UL
#if EVE_CHIPID <= EVE_FT801
#define ROMFONT_TABLEADDRESS (0xFFFFC)
#else
#define ROMFONT_TABLEADDRESS 3145724UL // 2F FFFCh
#endif

const char* flash_status[4] = { "INIT", "DETACHED", "BASIC", "FULL" };

static uint32_t save_REG_PCLK;

/**
 * @brief API to demonstrate flash states
 *
 */
void SAMAPP_Flash_State() {
	Draw_Text(s_pHalContext, "FLASH STATES");
	printf("\n\nFLASH STATES\n");
	uint8_t status = EVE_Hal_rd8(s_pHalContext, REG_FLASH_STATUS);
	printf("\nFlash State: %s (%d)\n", flash_status[status], status);

	printf("\nCMD_FLASHDETACH\n");
	EVE_CoCmd_flashDetach(s_pHalContext);
	EVE_Cmd_waitFlush(s_pHalContext);

	status = EVE_Hal_rd8(s_pHalContext, REG_FLASH_STATUS);
	if (status != FLASH_STATUS_DETACHED)
	{
		printf("Flash is not able to detach\n");
	}
	else
	{
		printf("Flash is detached\n");
	}
	printf("Flash State: %s (%d)\n", flash_status[status], status);

	printf("\nCMD_FLASHATTACH\n");
	EVE_CoCmd_flashAttach(s_pHalContext);
	EVE_Cmd_waitFlush(s_pHalContext);

	status = EVE_Hal_rd8(s_pHalContext, REG_FLASH_STATUS);

	if (status != FLASH_STATUS_BASIC)
	{
		printf("Flash is not able to attach\n");
	}
	else
	{
		printf("Flash is attached\n");
	}
	printf("Flash State: %s (%d)\n", flash_status[status], status);

	printf("\nCMD_FLASHFAST\n");
	EVE_CoCmd_flashFast(s_pHalContext, 0);
	EVE_Cmd_waitFlush(s_pHalContext);
	status = EVE_Hal_rd8(s_pHalContext, REG_FLASH_STATUS);

	if (status != FLASH_STATUS_FULL)
	{
		printf("Flash is not able to get into full mode\n");
	}
	else
	{
		printf("Flash gets into full mode\n");
	}
	printf("Flash State: %s (%d)\n", flash_status[status], status);
	EVE_sleep(100);
	printf("\nCheck Flash Size\n");
	status = EVE_Hal_rd8(s_pHalContext, REG_FLASH_SIZE);
	printf("Flash size : %d Mbytes (%d Mbits)\n", status, 8 * status);
}

/**
 * @brief API to demonstrate flash programming
 *
 */
void SAMAPP_Program_Flash() {
	/* Erase the flash */
	FlashHelper_SwitchState(s_pHalContext, FLASH_STATUS_FULL);
	EVE_Cmd_waitFlush(s_pHalContext);
	Draw_Text(s_pHalContext, "ERASE FLASH");
	EVE_CoCmd_flashErase(s_pHalContext);
	EVE_CoCmd_nop(s_pHalContext);
	EVE_Cmd_waitFlush(s_pHalContext);
	Draw_Text(s_pHalContext, "ERASE FLASH Done");

	Gpu_Release(s_pHalContext);
	Gpu_Init(s_pHalContext);
#if !defined(BT8XXEMU_PLATFORM) && GET_CALIBRATION == 1
	Calibration_Restore(s_pHalContext);
#endif

	// write flash
	Draw_Text(s_pHalContext, "Writing BT81X_Flash.bin to Flash...");
	Ftf_Write_File_To_Flash_With_Progressbar(s_pHalContext, TEST_DIR "/Flash/BT81X_Flash.bin", "BT81X_Flash.bin", 0);
}

/**
 * @brief API to demonstrate ASTC image on flash
 *
 */
void SAMAPP_ASTC() {
	Draw_Text(s_pHalContext, "ASTC format");
	EVE_Cmd_waitFlush(s_pHalContext);

	EVE_CoCmd_dlStart(s_pHalContext);
	EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1));
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 255, 255));

	uint16_t iw = 32;
	uint16_t ih = 32;
	uint16_t format = COMPRESSED_RGBA_ASTC_4x4_KHR;

#define BITMAP_ADDRESS_ON_FLASH 5774720 //address of bitmap file from Flash map after generating Flash
	/* Switch Flash to FULL Mode */
	FlashHelper_SwitchFullMode(s_pHalContext);
	EVE_CoCmd_setBitmap(s_pHalContext, (0x800000 | BITMAP_ADDRESS_ON_FLASH / 32), format, iw, ih);
	//Start drawing bitmap
	EVE_Cmd_wr32(s_pHalContext, BEGIN(BITMAPS));
	EVE_Cmd_wr32(s_pHalContext, VERTEX2II(0, 0, 0, 0));
	EVE_Cmd_wr32(s_pHalContext, END());
	EVE_Cmd_wr32(s_pHalContext, RESTORE_CONTEXT());
	EVE_Cmd_wr32(s_pHalContext, DISPLAY());
	EVE_CoCmd_swap(s_pHalContext);
	EVE_Cmd_waitFlush(s_pHalContext);
	delay(2000);
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
static void ASTC_screen(const char* str, int16_t x, int16_t y,
	int16_t w, int16_t h, int16_t margin, int16_t numcell) {
	int m1 = 30;
	uint32_t source = 0;

	EVE_CoCmd_text(s_pHalContext, x, y, 16, 0, str);
	EVE_CoCmd_setBitmap(s_pHalContext, source, COMPRESSED_RGBA_ASTC_12x10_KHR, w, h);
	EVE_Cmd_wr32(s_pHalContext, BEGIN(BITMAPS));
	EVE_Cmd_wr32(s_pHalContext, CELL(0));  //#CELL 0
	EVE_Cmd_wr32(s_pHalContext, VERTEX2F((x + m1) * 16, y * 16));

	if (2 == numcell) {
		EVE_Cmd_wr32(s_pHalContext, CELL(1));  //#CELL 1
		EVE_Cmd_wr32(s_pHalContext, VERTEX2F((x + m1 + margin) * 16, y * 16));
	}
}

/**
 * @brief API to demonstrateASTC layout
 *
 */
static void SAMAPP_ASTC_layout_ramg() {
	const char* files = TEST_DIR "\\numbers_astc12x10.raw";
	int16_t x = 0;
	int16_t y = 0;

	Ftf_Write_File_To_RAM_G(s_pHalContext, files, 0);

	Draw_Text(s_pHalContext, "ASTC layout, bitmap on RAM_G");

	Display_Start(s_pHalContext);
	x = 20; y = 20;
	ASTC_screen("1x1", x, y, 12, 10, 20, 2);
	y += 30;
	ASTC_screen("1x2", x, y, 12, 20, 20, 2);
	y += 30;
	ASTC_screen("1x3", x, y, 12, 30, 20, 2);
	y += 40;
	ASTC_screen("1x4", x, y, 12, 40, 20, 2);
	y += 50;
	ASTC_screen("1x5", x, y, 12, 50, 20, 2);

	x += 90; y = 20;
	ASTC_screen("2x1", x, y, 24, 10, 40, 2);
	y += 30;
	ASTC_screen("2x2", x, y, 24, 20, 40, 2);
	y += 30;
	ASTC_screen("2x3", x, y, 24, 30, 40, 2);
	y += 40;
	ASTC_screen("2x4", x, y, 24, 40, 40, 2);
	y += 50;
	ASTC_screen("2x5", x, y, 24, 50, 40, 2);

	x += 110; y = 20;
	ASTC_screen("3x1", x, y, 36, 10, 50, 2);
	y += 30;
	ASTC_screen("3x2", x, y, 36, 20, 50, 2);
	y += 30;
	ASTC_screen("3x3", x, y, 36, 30, 50, 2);
	y += 40;
	ASTC_screen("3x4", x, y, 36, 40, 50, 2);
	y += 50;
	ASTC_screen("3x5", x, y, 36, 50, 50, 2);

	x += 130; y = 20;
	ASTC_screen("4x1", x, y, 48, 10, 60, 2);
	y += 30;
	ASTC_screen("4x2", x, y, 48, 20, 60, 2);
	ASTC_screen("4x3", x, y + 30, 48, 30, 60, 2);
	y += 70;
	ASTC_screen("4x4", x, y, 48, 40, 60, 2);
	y += 50;
	ASTC_screen("4x5", x, y, 48, 50, 60, 2);

	x += 150; y = 20;
	ASTC_screen("5x1", x, y, 60, 10, 70, 2);
	y += 30;
	ASTC_screen("5x2", x, y, 60, 20, 70, 2);
	y += 30;
	ASTC_screen("5x3", x, y, 60, 30, 70, 2);
	y += 40;
	ASTC_screen("5x4", x, y, 60, 40, 70, 2);
	y += 50;
	ASTC_screen("5x5", x, y, 60, 50, 70, 2);

	Display_End(s_pHalContext);
	EVE_sleep(5000);
}

/**
 * @brief API to demonstrate ASTC layout of cell 3x2
 *
 */
static void SAMAPP_ASTC_layout_cell_1_3x2_case() {
	char* files[] = { TEST_DIR "\\ASTC_layout_cell_1_3x2_casei_36x240_ASTC_12x10_KHR.raw", 0 };
	const int32_t astc_flash_addr = 4096;
	int32_t source = ATFLASH(astc_flash_addr);
	int16_t x = 0;
	int16_t y = 0;

	if (0 == Ftf_Write_FileArr_To_Flash_By_RAM_G(s_pHalContext, files, astc_flash_addr)) {
		APP_ERR("Error when write raw file to flash");
		return;
	}

	FlashHelper_SwitchFullMode(s_pHalContext);

	Ftf_Write_FileArr_To_RAM_G(s_pHalContext, files, 0);

	Display_Start(s_pHalContext);
	source = 0;
	x = 30; y = 30;
	EVE_CoCmd_fillWidth(s_pHalContext, s_pHalContext->Width - 20);
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0, 0, 0));
	EVE_CoCmd_text(s_pHalContext, x, y, 28, OPT_FILL, "Display CELL 1 for 3x2 case for ASTC in RAM_G is ok:");
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 255, 255));
	EVE_CoCmd_setBitmap(s_pHalContext, source, COMPRESSED_RGBA_ASTC_12x10_KHR, 12 * 3, 10 * 2);
	EVE_Cmd_wr32(s_pHalContext, BEGIN(BITMAPS));

	y += 55;
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0, 0, 0));
	EVE_CoCmd_text(s_pHalContext, x + 50, y, 22, 0, "CELL 0:");
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 255, 255));
	EVE_Cmd_wr32(s_pHalContext, CELL(0));  //#CELL 0
	EVE_Cmd_wr32(s_pHalContext, VERTEX2F((x + 50 + 70) * 16, (y - 2) * 16));

	x += 200;
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0, 0, 0));
	EVE_CoCmd_text(s_pHalContext, x, y, 22, 0, "CELL 1:");
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 255, 255));
	EVE_Cmd_wr32(s_pHalContext, CELL(1));  //#CELL 1
	EVE_Cmd_wr32(s_pHalContext, VERTEX2F((x + 70) * 16, (y - 2) * 16));

	x = 30; y += 80;
	source = ATFLASH(astc_flash_addr);
	EVE_CoCmd_fillWidth(s_pHalContext, s_pHalContext->Width - 20);
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0, 0, 0));
	EVE_CoCmd_text(s_pHalContext, x, y, 28, OPT_FILL, "Display CELL 1 for 3x2 case for ASTC in Flash shall show some artifacts:");
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 255, 255));
	EVE_CoCmd_setBitmap(s_pHalContext, source, COMPRESSED_RGBA_ASTC_12x10_KHR, 12 * 3, 10 * 2);
	EVE_Cmd_wr32(s_pHalContext, BEGIN(BITMAPS));

	y += 50;
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0, 0, 0));
	EVE_CoCmd_text(s_pHalContext, x + 50, y, 22, 0, "CELL 0:");
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 255, 255));
	EVE_Cmd_wr32(s_pHalContext, CELL(0));  //#CELL 0
	EVE_Cmd_wr32(s_pHalContext, VERTEX2F((x + 50 + 70) * 16, (y - 2) * 16));

	x += 150;
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0, 0, 0));
	EVE_CoCmd_text(s_pHalContext, x + 50, y, 22, 0, "CELL 1:");
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 255, 255));
	EVE_Cmd_wr32(s_pHalContext, CELL(1));  //#CELL 1
	EVE_Cmd_wr32(s_pHalContext, VERTEX2F((x + 50 + 70) * 16, (y - 2) * 16));

	Display_End(s_pHalContext);
	EVE_sleep(10000);
}

/**
 * @brief API to demonstrate ASTC layout on flash
 *
 */
static void SAMAPP_ASTC_layout_flash() {
	char* files[] = { TEST_DIR "\\numbers_astc12x10.raw", 0 };
	int16_t x = 0;
	int16_t y = 0;

	Ftf_Write_FileArr_To_RAM_G(s_pHalContext, files, 0);

	Draw_Text(s_pHalContext, "ASTC layout, bitmap on Flash");

	Display_Start(s_pHalContext);
	x = 20; y = 20;
	ASTC_screen("1x1", x, y, 12, 10, 20, 1);
	y += 30;
	ASTC_screen("1x2", x, y, 12, 20, 20, 1);
	y += 30;
	ASTC_screen("1x3", x, y, 12, 30, 20, 1);
	y += 40;
	ASTC_screen("1x4", x, y, 12, 40, 20, 2);
	y += 50;
	ASTC_screen("1x5", x, y, 12, 50, 20, 1);

	x += 90; y = 20;
	ASTC_screen("2x1", x, y, 24, 10, 40, 1);
	y += 30;
	ASTC_screen("2x2", x, y, 24, 20, 40, 2);
	y += 30;
	ASTC_screen("2x3", x, y, 24, 30, 40, 1);
	y += 40;
	ASTC_screen("2x4", x, y, 24, 40, 40, 2);
	y += 50;
	ASTC_screen("2x5", x, y, 24, 50, 40, 1);

	x += 110; y = 20;
	ASTC_screen("3x1", x, y, 36, 10, 50, 1);
	y += 30;
	ASTC_screen("3x2", x, y, 36, 20, 50, 1);
	y += 30;
	ASTC_screen("3x3", x, y, 36, 30, 50, 1);
	y += 40;
	ASTC_screen("3x4", x, y, 36, 40, 50, 2);
	y += 50;
	ASTC_screen("3x5", x, y, 36, 50, 50, 1);

	x += 130; y = 20;
	ASTC_screen("4x1", x, y, 48, 10, 60, 2);
	y += 30;
	ASTC_screen("4x2", x, y, 48, 20, 60, 2);
	y += 30;
	ASTC_screen("4x3", x, y, 48, 30, 60, 2);
	y += 40;
	ASTC_screen("4x4", x, y, 48, 40, 60, 2);
	y += 50;
	ASTC_screen("4x5", x, y, 48, 50, 60, 2);

	x += 150; y = 20;
	ASTC_screen("5x1", x, y, 60, 10, 70, 1);
	y += 30;
	ASTC_screen("5x2", x, y, 60, 20, 70, 1);
	y += 30;
	ASTC_screen("5x3", x, y, 60, 30, 70, 1);
	y += 40;
	ASTC_screen("5x4", x, y, 60, 40, 70, 2);
	y += 50;
	ASTC_screen("5x5", x, y, 60, 50, 70, 1);

	Display_End(s_pHalContext);
	EVE_sleep(5000);
}

/**
 * @brief Check Coprocesor fault
 *
 */
void CoprocessorFault_Report()
{
	char8_t DiagMessage[128];
	if (EVE_Hal_rd16(s_pHalContext, REG_CMD_READ) == 0xfff) //Fault reported
	{
		EVE_Hal_rdMem(s_pHalContext, DiagMessage, 0x309800, 128);
		printf("%s", DiagMessage);
		printf("\n");
		DiagMessage[0] = '\0'; //Reset message
	}
}

/**
 * @brief Restore Coprocessor after fault
 *
 */
void CoprocessorFault_Recover() {
	/* 3 steps of recovery coprocessor sequence */
	/* Set REG_CPURESET to 1, to hold the coprocessor in the reset condition */
	uint16_t original_patch = EVE_Hal_rd16(s_pHalContext, REG_COPRO_PATCH_PTR);
	EVE_Hal_wr32(s_pHalContext, REG_CPURESET, 1);
	/* Set REG_CMD_READ and REG_CMD_WRITE to zero */
	EVE_Hal_wr32(s_pHalContext, REG_CMD_READ, 0);
	EVE_Hal_wr32(s_pHalContext, REG_CMD_WRITE, 0);
	EVE_Hal_wr32(s_pHalContext, REG_CMD_DL, 0);
	EVE_Hal_wr32(s_pHalContext, REG_PCLK, save_REG_PCLK); /* coprocessor will set the pclk to 0 for that error case */
	Gpu_Hal_ResetCmdFifo(s_pHalContext);
	/* Set REG_CPURESET to 0, to restart the coprocessor */
	EVE_Hal_wr32(s_pHalContext, REG_CPURESET, 0);
	EVE_sleep(100);
	EVE_Hal_wr16(s_pHalContext, REG_COPRO_PATCH_PTR, original_patch);

	App_Set_CmdBuffer_Index(0);
	App_Set_DlBuffer_Index(0);

	EVE_CoCmd_flashFast(s_pHalContext, 0);
	EVE_Cmd_waitFlush(s_pHalContext);
	uint8_t status = EVE_Hal_rd8(s_pHalContext, REG_FLASH_STATUS);
	if (status != FLASH_STATUS_FULL)
	{
		printf("Flash is not able to get into full mode\n");
	}
}

/**
 * @brief API to demonstrate Coprocessor fault and recover
 *
 */
void SAMAPP_CoprocessorFaults() {
	Draw_Text(s_pHalContext, "Coprocessor Faults");
	printf("\n\nCoprocessor Faults\n");
	int32_t FileLen;
	int32_t bitmapWidth = 128;
	int32_t bitmapHeight = 128;
	uchar8_t* pbuff;
#if defined(MSVC_PLATFORM) || defined(BT8XXEMU_PLATFORM)
	FILE* pFile;
#endif
	//Fault case: enable interlace option
	EVE_CoCmd_dlStart(s_pHalContext);
	EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1));
	EVE_Cmd_wr32(s_pHalContext, CMD_LOADIMAGE);
	EVE_Cmd_wr32(s_pHalContext, RAM_G);//destination address of png decode
	EVE_Cmd_wr32(s_pHalContext, 0);
#if defined(MSVC_PLATFORM) || defined(BT8XXEMU_PLATFORM)
	pFile = fopen(TEST_DIR "\\lenaface40_unsupported.png", "rb"); //Fault case: enable interlace option
	if (pFile == NULL) {
		return;
	}
	pbuff = (uchar8_t*)malloc(8192);
	fseek(pFile, 0, SEEK_END);
	FileLen = ftell(pFile);
	fseek(pFile, 0, SEEK_SET);
	while (FileLen > 0)
	{
		/* download the data into the command buffer by 2kb one shot */
		uint16_t blocklen = FileLen > 8192 ? 8192 : (uint16_t)FileLen;

		/* copy the data into pbuff and then transfter it to command buffer */
		fread(pbuff, 1, blocklen, pFile);
		FileLen -= blocklen;
		/* copy data continuously into command memory */
		EVE_Cmd_wrMem(s_pHalContext, pbuff, blocklen);//alignment is already taken care by this api
	}
	/* close the opened binary zlib file */
	free(pbuff);
	fclose(pFile);
	EVE_Cmd_wr32(s_pHalContext, BEGIN(BITMAPS));
	EVE_Cmd_wr32(s_pHalContext, VERTEX2F((s_pHalContext->Width / 2 - bitmapWidth / 2) * 16, (s_pHalContext->Height / 2 - bitmapHeight / 2) * 16));
	EVE_Cmd_wr32(s_pHalContext, END());
#endif
	EVE_Cmd_wr32(s_pHalContext, DISPLAY());
	EVE_CoCmd_swap(s_pHalContext);
	/* Download the commands into fifo */
	EVE_Cmd_waitFlush(s_pHalContext);
	delay(100);
	CoprocessorFault_Report();
	CoprocessorFault_Recover();
	//Fault case: change bit depth into 7
	EVE_CoCmd_dlStart(s_pHalContext);
	EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1));
	EVE_Cmd_waitFlush(s_pHalContext);
	EVE_Cmd_wr32(s_pHalContext, CMD_LOADIMAGE);
	EVE_Cmd_wr32(s_pHalContext, RAM_G);//destination address of png decode
	EVE_Cmd_wr32(s_pHalContext, 0);
#if defined(MSVC_PLATFORM) || defined(BT8XXEMU_PLATFORM)
	pFile = fopen(TEST_DIR "\\lenaface40_corrupted.png", "rb"); //Fault case: change bit depth into 7
	if (pFile == NULL)
		return;
	pbuff = (uchar8_t*)malloc(8192);
	fseek(pFile, 0, SEEK_END);
	FileLen = ftell(pFile);
	fseek(pFile, 0, SEEK_SET);
	while (FileLen > 0)
	{
		/* download the data into the command buffer by 2kb one shot */
		uint16_t blocklen = FileLen > 8192 ? 8192 : (uint16_t)FileLen;

		/* copy the data into pbuff and then transfter it to command buffer */
		fread(pbuff, 1, blocklen, pFile);
		FileLen -= blocklen;
		/* copy data continuously into command memory */
		EVE_Cmd_wrMem(s_pHalContext, pbuff, blocklen);//alignment is already taken care by this api
	}
	/* close the opened binary zlib file */
	fclose(pFile);
	free(pbuff);
	EVE_Cmd_wr32(s_pHalContext, BEGIN(BITMAPS));
	EVE_Cmd_wr32(s_pHalContext, VERTEX2F((s_pHalContext->Width / 2 - bitmapWidth / 2) * 16, (s_pHalContext->Height / 2 - bitmapHeight / 2) * 16));
	EVE_Cmd_wr32(s_pHalContext, END());
#endif
	EVE_CoCmd_text(s_pHalContext, (int16_t)(s_pHalContext->Width / 2), 50, 30, OPT_CENTER, "PNG images can now be loaded directly.");
	EVE_Cmd_wr32(s_pHalContext, DISPLAY());
	EVE_CoCmd_swap(s_pHalContext);
	/* Download the commands into fifo */
	EVE_Cmd_waitFlush(s_pHalContext);
	delay(100);
	CoprocessorFault_Report();
	CoprocessorFault_Recover();
}

/**
 * @brief API to demonstrate display bitmap by inflate
 *
 */
void SAMAPP_Mediafifo_CmdInflate_from_flash() {
	Draw_Text(s_pHalContext, "CMD_INFLATE2 with OPT_FLASH");

#define INFLATED_ADDR (4096)
	/* INFLATED BITMAP information */
#define INFLATED_BITMAP_WIDTH (63)  /* color_.bin */
#define INFLATED_BITMAP_HEIGHT (44)
#define INFLATED_BITMAP_FORMAT (ARGB4)
#define INFLATED_BITMAP_STRIDE (63*2)

	/*Load data to mediafifo */
	uchar8_t* imagefile = TEST_DIR "\\color_.bin";

	Ftf_Write_File_To_Flash_By_RAM_G(s_pHalContext, imagefile, INFLATED_ADDR);
	FlashHelper_SwitchState(s_pHalContext, FLASH_STATUS_FULL); // full mode

	EVE_CoCmd_flashSource(s_pHalContext, INFLATED_ADDR);
	EVE_Hal_flush(s_pHalContext);

	EVE_CoCmd_inflate2(s_pHalContext, RAM_G, OPT_FLASH);
	EVE_Cmd_waitFlush(s_pHalContext);

	EVE_Cmd_wr32(s_pHalContext, CMD_DLSTART);
	EVE_Cmd_wr32(s_pHalContext, CLEAR_COLOR_RGB(0, 0, 0));
	EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1));
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0xff, 0xff, 0xff));
	EVE_Cmd_wr32(s_pHalContext, BEGIN(BITMAPS));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_SOURCE2(0, 0));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_LAYOUT(INFLATED_BITMAP_FORMAT, INFLATED_BITMAP_STRIDE, INFLATED_BITMAP_HEIGHT));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_SIZE(BILINEAR, BORDER, BORDER, INFLATED_BITMAP_WIDTH, INFLATED_BITMAP_HEIGHT));
	EVE_Cmd_wr32(s_pHalContext, VERTEX2F(100 * 16, 100 * 16));
	EVE_Cmd_wr32(s_pHalContext, END());

	/*  Display the text information */
	EVE_Cmd_wr32(s_pHalContext, COLOR_A(255));
	EVE_CoCmd_text(s_pHalContext, 20, 50, 24, 0, "Display bitmap by inflate (OPT_FLASH)");
	EVE_Cmd_wr32(s_pHalContext, DISPLAY());

	EVE_CoCmd_swap(s_pHalContext);
	/* Download the commands into fifo */
	EVE_Cmd_waitFlush(s_pHalContext);
	CoprocessorFault_Report();

	EVE_sleep(3000);
}

void SAMAPP_Mediafifo_CmdInflate()
{
	Draw_Text(s_pHalContext, "CMD_INFLATE2 with media fifo");
	/** Address in RAM_G */
#define MEDIAFIFO_SIZE (100*1024)
#define RAM_MEDIAFIFO_ADDR (RAM_G_SIZE - MEDIAFIFO_SIZE)

	/* INFLATED BITMAP information */
#define INFLATED_BITMAP_WIDTH (63)  /* color_.bin */
#define INFLATED_BITMAP_HEIGHT (44)
#define INFLATED_BITMAP_FORMAT (ARGB4)
#define INFLATED_BITMAP_STRIDE (63*2)

	/*Load data to mediafifo */
	uchar8_t* imagefile[] = { TEST_DIR "\\color_.bin", 0 };

	Ftf_Write_FileArr_To_RAM_G(s_pHalContext, imagefile, RAM_MEDIAFIFO_ADDR);
	EVE_CoCmd_mediaFifo(s_pHalContext, RAM_MEDIAFIFO_ADDR, MEDIAFIFO_SIZE); //address of the media fifo buffer
	EVE_Cmd_waitFlush(s_pHalContext);

	EVE_Hal_wr32(s_pHalContext, REG_MEDIAFIFO_WRITE, 890);//hardcoded for color_.bin file

	CoprocessorFault_Report();

	EVE_CoCmd_inflate2(s_pHalContext, RAM_G, OPT_MEDIAFIFO);
	EVE_Cmd_waitFlush(s_pHalContext);

	CoprocessorFault_Report();

	EVE_Cmd_wr32(s_pHalContext, CMD_DLSTART);
	EVE_Cmd_wr32(s_pHalContext, CLEAR_COLOR_RGB(0, 0, 0));
	EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1));
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 255, 255));
	EVE_Cmd_wr32(s_pHalContext, BEGIN(BITMAPS));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_SOURCE2(0, 0));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_LAYOUT(INFLATED_BITMAP_FORMAT, INFLATED_BITMAP_STRIDE, INFLATED_BITMAP_HEIGHT));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_SIZE(BILINEAR, BORDER, BORDER, INFLATED_BITMAP_WIDTH, INFLATED_BITMAP_HEIGHT));
	EVE_Cmd_wr32(s_pHalContext, VERTEX2F(100 * 16, 100 * 16));
	EVE_Cmd_wr32(s_pHalContext, END());

	/*  Display the text information */
	EVE_Cmd_wr32(s_pHalContext, COLOR_A(255));
	EVE_CoCmd_text(s_pHalContext, 20, 50, 23, 0, "Display bitmap by inflate");
	EVE_Cmd_wr32(s_pHalContext, DISPLAY());

	EVE_CoCmd_swap(s_pHalContext);
	/* Download the commands into fifo */
	EVE_Cmd_waitFlush(s_pHalContext);
	CoprocessorFault_Report();

	EVE_sleep(3000);
}

/**
 * @brief API to demonstrate CMD_GRADIENTA with transparency
 *
 */
void SAMAPP_GradientA()
{
	Draw_Text(s_pHalContext, "CMD_GRADIENTA with transparency");
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

	wScissor = (int16_t)((s_pHalContext->Width - 4 * 10) / 3);
	hScissor = (int16_t)((s_pHalContext->Height - 3 * 20) / 2);
	xOffset = 10;
	yOffset = 20;
	/* Draw gradient  */

	EVE_CoCmd_dlStart(s_pHalContext);
	EVE_Cmd_wr32(s_pHalContext, CLEAR_COLOR_RGB(64, 64, 64));
	EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1));
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 255, 255));
	EVE_Cmd_wr32(s_pHalContext, SCISSOR_SIZE(wScissor, hScissor));
	/* Horizontal gradient effect */
	EVE_Cmd_wr32(s_pHalContext, SCISSOR_XY(xOffset, yOffset));//clip the display
	EVE_CoCmd_gradientA(s_pHalContext, xOffset, yOffset, 0x80808080, (xOffset + wScissor), yOffset, 0xffffff00);
	/* Vertical gradient effect */
	xOffset += wScissor + 10;
	EVE_Cmd_wr32(s_pHalContext, SCISSOR_XY(xOffset, yOffset));//clip the display
	EVE_CoCmd_gradientA(s_pHalContext, xOffset, yOffset, 0x80ff0000, xOffset, (yOffset + hScissor), 0xff00ff00);
	/* diagonal gradient effect */
	xOffset += wScissor + 10;
	EVE_Cmd_wr32(s_pHalContext, SCISSOR_XY(xOffset, yOffset));//clip the display
	EVE_CoCmd_gradientA(s_pHalContext, xOffset, yOffset, 0x80800000, (xOffset + wScissor), (yOffset + hScissor), 0xffffffff);
	/* Diagonal gradient with text info */
	xOffset = 10;
	yOffset += hScissor + 20;
	EVE_Cmd_wr32(s_pHalContext, SCISSOR_SIZE(wScissor, 30));
	EVE_Cmd_wr32(s_pHalContext, SCISSOR_XY(xOffset, (yOffset + hScissor / 2 - 15)));//clip the display
	EVE_CoCmd_gradientA(s_pHalContext, xOffset, (yOffset + hScissor / 2 - 15), 0x00606060, (xOffset + wScissor), (yOffset + hScissor / 2 + 15), 0xff404080);
	EVE_CoCmd_text(s_pHalContext, (xOffset + wScissor / 2), (yOffset + hScissor / 2), 28, OPT_CENTER, "GradientA");//text info

	/* Draw horizontal, vertical and diagonal with alpha */
	xOffset += wScissor + 10;
	EVE_Cmd_wr32(s_pHalContext, SCISSOR_SIZE(wScissor, hScissor));
	EVE_Cmd_wr32(s_pHalContext, SCISSOR_XY(xOffset, yOffset));//clip the display
	EVE_CoCmd_gradientA(s_pHalContext, xOffset, yOffset, 0x80808080, (xOffset + wScissor), yOffset, 0xffffff00);
	wScissor -= 30; hScissor -= 30;
	EVE_Cmd_wr32(s_pHalContext, SCISSOR_SIZE(wScissor, hScissor));
	xOffset += 15; yOffset += 15;
	EVE_Cmd_wr32(s_pHalContext, SCISSOR_XY(xOffset, yOffset));//clip the display
	EVE_CoCmd_gradientA(s_pHalContext, xOffset, yOffset, 0x80800000, xOffset, (yOffset + hScissor), 0xffffffff);
	wScissor -= 30; hScissor -= 30;
	EVE_Cmd_wr32(s_pHalContext, SCISSOR_SIZE(wScissor, hScissor));
	xOffset += 15; yOffset += 15;
	EVE_Cmd_wr32(s_pHalContext, SCISSOR_XY(xOffset, yOffset));//clip the display
	EVE_CoCmd_gradientA(s_pHalContext, xOffset, yOffset, 0x80606060, (xOffset + wScissor), (yOffset + hScissor), 0xFF404080);

	/* Display the text wrt gradient */
	wScissor = (int16_t)((s_pHalContext->Width - 4 * 10) / 3);
	hScissor = (int16_t)((s_pHalContext->Height - 3 * 20) / 2);
	xOffset = 10 + wScissor / 2;
	yOffset = 20 + hScissor + 5;
	EVE_Cmd_wr32(s_pHalContext, SCISSOR_XY(0, 0));//set to default values
#ifdef FT81X_ENABLE
	EVE_Cmd_wr32(s_pHalContext, SCISSOR_SIZE(2048, 2048));
#else
	EVE_Cmd_wr32(s_pHalContext, SCISSOR_SIZE(512, 512));
#endif
	EVE_CoCmd_text(s_pHalContext, xOffset, yOffset, 26, OPT_CENTER, "Horizontal grad");//text info
	xOffset += wScissor + 10;
	EVE_CoCmd_text(s_pHalContext, xOffset, yOffset, 26, OPT_CENTER, "Vertical grad");//text info
	xOffset += wScissor + 10;
	EVE_CoCmd_text(s_pHalContext, xOffset, yOffset, 26, OPT_CENTER, "Diagonal grad");//text info

	EVE_Cmd_wr32(s_pHalContext, DISPLAY());
	EVE_CoCmd_swap(s_pHalContext);

	/* Wait till coprocessor completes the operation */
	EVE_Cmd_waitFlush(s_pHalContext);
	EVE_sleep(3000);
}

/**
 * @brief API to demonstrate CMD_FILLWIDTH
 *
 */
void SAMAPP_Cmd_FillWidth()
{
	int16_t x;
	int16_t y;
	int16_t fill_w;

	Draw_Text(s_pHalContext, "CMD_FILLWIDTH");

	EVE_CoCmd_dlStart(s_pHalContext);
	EVE_Cmd_wr32(s_pHalContext, CLEAR_COLOR_RGB(255, 255, 255));
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0, 0, 0));
	EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1));

	fill_w = 80;
	EVE_CoCmd_fillWidth(s_pHalContext, fill_w);

	x = 100;
	y = 20;
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0, 255, 0));
	EVE_Cmd_wr32(s_pHalContext, BEGIN(RECTS));
	EVE_Cmd_wr32(s_pHalContext, VERTEX2F(x * 16, y * 16));
	EVE_Cmd_wr32(s_pHalContext, VERTEX2F((x + fill_w) * 16, (y + 150) * 16));
	EVE_Cmd_wr32(s_pHalContext, END());
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0, 0, 0));
	EVE_CoCmd_text(s_pHalContext, x, y, 30, OPT_FILL, "one two three four");

	x = 400;
	y = 70;
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0, 255, 0));
	EVE_Cmd_wr32(s_pHalContext, BEGIN(RECTS));
	EVE_Cmd_wr32(s_pHalContext, VERTEX2F((x - fill_w / 2) * 16, (y - 65) * 16));
	EVE_Cmd_wr32(s_pHalContext, VERTEX2F((x + fill_w / 2) * 16, (y + 70) * 16));
	EVE_Cmd_wr32(s_pHalContext, END());
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0, 0, 0));
	EVE_CoCmd_text(s_pHalContext, x, y, 30, OPT_FILL | OPT_CENTER, "one two three four");

	y = 20 + 20 * 10;
	x = 100;
	fill_w = 2;
	EVE_CoCmd_fillWidth(s_pHalContext, fill_w);

	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0, 255, 0));
	EVE_Cmd_wr32(s_pHalContext, BEGIN(RECTS));
	EVE_Cmd_wr32(s_pHalContext, VERTEX2F(x * 16, y * 16));
	EVE_Cmd_wr32(s_pHalContext, VERTEX2F((x + 80) * 16, (y + 150) * 16));
	EVE_Cmd_wr32(s_pHalContext, END());
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0, 0, 0));
	EVE_CoCmd_text(s_pHalContext, x, y, 30, OPT_FILL, "one two three four");

	x = 400;
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0, 255, 0));
	EVE_Cmd_wr32(s_pHalContext, BEGIN(RECTS));
	EVE_Cmd_wr32(s_pHalContext, VERTEX2F(x * 16, y * 16));
	EVE_Cmd_wr32(s_pHalContext, VERTEX2F((x + 160) * 16, (y + 70) * 16));
	EVE_Cmd_wr32(s_pHalContext, END());
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0, 0, 0));
	EVE_CoCmd_text(s_pHalContext, x, y, 30, 0, "one two \nthree four");

	EVE_Cmd_wr32(s_pHalContext, DISPLAY());
	EVE_CoCmd_swap(s_pHalContext);
	EVE_Cmd_waitFlush(s_pHalContext);
	EVE_sleep(4000);

	// Cover the cmd_button,cmd_toggle too. 
	EVE_CoCmd_dlStart(s_pHalContext);
	EVE_Cmd_wr32(s_pHalContext, CLEAR(255, 255, 255));

	y = (int16_t)(s_pHalContext->Height / 2);
	fill_w = 200;
	EVE_CoCmd_fillWidth(s_pHalContext, fill_w);
	EVE_CoCmd_button(s_pHalContext, 100, y, fill_w, 100, 30, OPT_FILL, "one two three four");
	EVE_CoCmd_toggle(s_pHalContext, 400, y, fill_w, 30, OPT_FLAT, 0, "one two three four");

	EVE_Cmd_wr32(s_pHalContext, DISPLAY());
	EVE_CoCmd_swap(s_pHalContext);
	EVE_Cmd_waitFlush(s_pHalContext);
	EVE_sleep(4000);
}

/**
 * @brief API to demonstrate image rotate
 *
 */
void SAMAPP_Rotate()
{
	uchar8_t imagefile[] = TEST_DIR "\\lenaface_COMPRESSED_RGBA_ASTC_4x4_KHR.raw";
	uint16_t image_w = 32;
	uint16_t image_h = 32;
	Gpu_Hal_LoadImageToMemory(s_pHalContext, imagefile, RAM_G, LOAD);

	for (uint16_t i = 0; i <= 360; i++) {
		EVE_CoCmd_dlStart(s_pHalContext);// start
		EVE_Cmd_wr32(s_pHalContext, CLEAR_COLOR_RGB(0, 0, 255));
		EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1));
		EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 255, 255));
		EVE_Cmd_wr32(s_pHalContext, BITMAP_HANDLE((0)));

		EVE_CoCmd_setBitmap(s_pHalContext, RAM_G, COMPRESSED_RGBA_ASTC_4x4_KHR, image_w, image_h);
		EVE_Cmd_wr32(s_pHalContext, BITMAP_SIZE(BILINEAR, BORDER, BORDER, image_w, image_h));
		EVE_Cmd_wr32(s_pHalContext, BITMAP_SIZE_H(image_w >> 9, image_h >> 9));
		EVE_CoCmd_text(s_pHalContext, 10, 0, 24, 0, "CMD_ROTATEAROUND");//text info
		EVE_Cmd_waitFlush(s_pHalContext);

		CoprocessorFault_Report();

		EVE_Cmd_wr32(s_pHalContext, BEGIN(BITMAPS));

		EVE_CoCmd_loadIdentity(s_pHalContext);
		EVE_CoCmd_rotateAround(s_pHalContext, image_w / 2, image_h / 2, i * 65536 / 360, 65536 * 1);
		EVE_CoCmd_setMatrix(s_pHalContext);
		EVE_Cmd_wr32(s_pHalContext, VERTEX2F((s_pHalContext->Width / 2 - image_w / 2) * 16, (s_pHalContext->Height / 2 - image_h / 2) * 16));
		EVE_Cmd_wr32(s_pHalContext, END());

		EVE_CoCmd_loadIdentity(s_pHalContext);
		EVE_CoCmd_setMatrix(s_pHalContext);

		EVE_Cmd_wr32(s_pHalContext, DISPLAY());
		EVE_CoCmd_swap(s_pHalContext);
		EVE_Cmd_waitFlush(s_pHalContext);
		EVE_sleep(50);
	}
	EVE_sleep(1000);
}

/**
 * @brief API to demonstrate animation from Flash by AnimFrame
 *
 */
void SAMAPP_Animation2() {
#define ANIM_ADDR 10785472 //address of "bicycle.anim.object" from Flash map after generating Flash
#define FRAME_COUNT 1636
	uint32_t prAnimLoop = ANIM_ONCE;

	Draw_Text(s_pHalContext, "ANIMATION from Flash by AnimFrame");

	/* Switch Flash to FULL Mode */
	FlashHelper_SwitchFullMode(s_pHalContext);

	uint16_t channel = 30;

	const int16_t xAnim = 400;
	const int16_t yAnim = 240;

	EVE_CoCmd_animStart(s_pHalContext, channel, ANIM_ADDR, prAnimLoop);
	EVE_CoCmd_animXY(s_pHalContext, channel, xAnim, yAnim);

	for (uint16_t frame = 0; frame < FRAME_COUNT; frame++) {
		EVE_CoCmd_dlStart(s_pHalContext);
		EVE_Cmd_wr32(s_pHalContext, CLEAR_COLOR_RGB(0, 0, 0));
		EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1));

		EVE_CoCmd_animFrame(s_pHalContext, xAnim, yAnim, ANIM_ADDR, frame);

		EVE_Cmd_wr32(s_pHalContext, DISPLAY());
		EVE_CoCmd_swap(s_pHalContext);
		EVE_Cmd_waitFlush(s_pHalContext);
	}

	EVE_CoCmd_animStop(s_pHalContext, channel);
}

/**
 * @brief API to demonstrate animation from Flash by AnimDraw
 *
 */
void SAMAPP_Animation()
{
#define ANIM_ADDR 10785472 //address of "bicycle.anim.object" from Flash map after generating Flash
#define FRAME_COUNT 1636
	uint8_t txtAnim_ONCE[] = "Playing Animation by ANIM_ONCE";
	uint8_t txtAnim_LOOP[] = "Playing Animation by ANIM_LOOP";
	uint8_t txtAnim_HOLD[] = "Playing Animation by ANIM_HOLD";

	uint8_t txtPress2Stop[] = "Press \"Stop Anim\" again to exit";

	uint32_t prAnimLoop = ANIM_ONCE;
	const uint8_t* txtAnimLabel = txtAnim_ONCE;

	int i = 0;
	const int timeOut = 80 * 500; // exit after 500*80 = 40.000 frames, about 400s (7min)
	int countStop = 0; // countStop = 2 to exit while loop

	Draw_Text(s_pHalContext, "ANIMATION from Flash by AnimDraw");
	/* Switch Flash to FULL Mode */
	FlashHelper_SwitchFullMode(s_pHalContext);

	uint16_t channel = 30;

	uint8_t tag = 0;
	uint8_t oldTag = 0;

	uint8_t isPlaying = 0;

	const int16_t xAnim = 350;
	const int16_t yAnim = 260;
	const int xLbl = 200;
	const int yLbl = 20;
	const int xStart = 650;
	const int yStart = 50;
	const int xStop = 650;
	const int yStop = 160;
	const int w = 130;
	const int h = 100;

	while (i++ < timeOut && countStop < 2) {
		EVE_CoCmd_dlStart(s_pHalContext);
		EVE_Cmd_wr32(s_pHalContext, CLEAR_COLOR_RGB(0, 0, 0));
		EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1));

		int8_t newTag = EVE_Hal_rd8(s_pHalContext, REG_TOUCH_TAG);

		if (newTag != oldTag) {
			tag = newTag;
			oldTag = newTag;
		}
		else {
			tag = 0;
		}

		// Show text information
		if (isPlaying) {
			EVE_CoCmd_text(s_pHalContext, xLbl, yLbl, 27, 0, txtAnimLabel);
			EVE_CoCmd_animDraw(s_pHalContext, channel);

			EVE_Cmd_wr32(s_pHalContext, TAG(1));
			EVE_CoCmd_button(s_pHalContext, xStart, yStart, w, h, 27, 0, "Change Anim");
		}
		else {
			if (countStop == 0) {
				EVE_CoCmd_text(s_pHalContext, xLbl, yLbl, 27, 0, "Press \"Start Anim\" to start animation");
			}
			else if (countStop > 0) {
				EVE_CoCmd_text(s_pHalContext, xLbl, yLbl, 27, 0, txtPress2Stop);
			}

			EVE_Cmd_wr32(s_pHalContext, TAG(1));
			EVE_CoCmd_button(s_pHalContext, xStart, yStart, w, h, 27, 0, "Start Anim");
		}

		EVE_Cmd_wr32(s_pHalContext, TAG(2));
		EVE_CoCmd_button(s_pHalContext, xStop, yStop, w, h, 27, 0, "Stop Anim");

		if (tag == 1) {
			if (isPlaying == 1) {
				EVE_CoCmd_animStop(s_pHalContext, channel);
			}

			isPlaying = 1;
			countStop = 0;

			EVE_CoCmd_animStart(s_pHalContext, channel, ANIM_ADDR, prAnimLoop);
			EVE_CoCmd_animXY(s_pHalContext, channel, xAnim, yAnim);

			// change AnimLoop attribute for the next user button press
			// change text lable for the current press
			// ONCE -> HOLD -> LOOP -> ONCE -> ...
			switch (prAnimLoop) {
			case ANIM_ONCE:
				prAnimLoop = ANIM_HOLD;
				txtAnimLabel = txtAnim_ONCE;
				break;
			case ANIM_HOLD:
				prAnimLoop = ANIM_LOOP;
				txtAnimLabel = txtAnim_HOLD;
				break;
			case ANIM_LOOP:
				prAnimLoop = ANIM_ONCE;
				txtAnimLabel = txtAnim_LOOP;
				break;
			default: break;
			}
		}
		if (tag == 2) {
			isPlaying = 0;
			EVE_CoCmd_animStop(s_pHalContext, channel);
			countStop++;
		}

		EVE_Cmd_wr32(s_pHalContext, DISPLAY());
		EVE_CoCmd_swap(s_pHalContext);
		EVE_Cmd_waitFlush(s_pHalContext);
	}
}

/**
 * @brief API to demonstrate unicode font
 *
 */
void SAMAPP_ExtendedFormat_Font()
{
	Draw_Text(s_pHalContext, "Unicode Font");
#define UNICODE_HANDLE 30
	uint32_t fontAddr = RAM_G;
	//Load glyph file into BT815's flash
	//Load xfont file into graphics RAM

	/* Switch Flash to FULL Mode */
	FlashHelper_SwitchFullMode(s_pHalContext);
	Gpu_Hal_LoadImageToMemory(s_pHalContext, TEST_DIR "\\mssong_20.xfont", fontAddr, LOAD);
	EVE_Cmd_waitFlush(s_pHalContext);

	EVE_CoCmd_dlStart(s_pHalContext);
	EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1));
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 255, 255));

	EVE_CoCmd_setFont2(s_pHalContext, UNICODE_HANDLE, fontAddr, 0);
	EVE_CoCmd_text(s_pHalContext, 0, 0, UNICODE_HANDLE, 0, u8"BRT的EVE技术是一个革命性的概念，");
	EVE_CoCmd_text(s_pHalContext, 0, 30, UNICODE_HANDLE, 0, u8"利用面向对象的方法创建高质量的人机界面（HMI） 同时支持显示，");
	EVE_CoCmd_text(s_pHalContext, 0, 60, UNICODE_HANDLE, 0, u8"音频和触摸功能。");
	EVE_Cmd_wr32(s_pHalContext, DISPLAY());
	EVE_CoCmd_swap(s_pHalContext);
	EVE_Cmd_waitFlush(s_pHalContext);
	EVE_sleep(4000);
}

/**
 * @brief API to demonstrate CMD_RESETFONTS
 *
 */
void SAMAPP_ResetFont() //must call after SAMAPP_ExtendedFormat_Font
{
	Draw_Text(s_pHalContext, "CMD_RESETFONTS");
	EVE_CoCmd_dlStart(s_pHalContext);
	EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1));
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 255, 255));
	EVE_CoCmd_text(s_pHalContext, 0, 0, 27, 0, "Use custom font with bitmap handle 30 from SAMAPP_ExtendedFormat_Font():");
	EVE_CoCmd_text(s_pHalContext, 0, 200, UNICODE_HANDLE, 0, "The quick brown fox jumps over the lazy dog");
	EVE_Cmd_wr32(s_pHalContext, DISPLAY());
	EVE_CoCmd_swap(s_pHalContext);
	EVE_Cmd_waitFlush(s_pHalContext);
	EVE_sleep(5000);

	EVE_CoCmd_dlStart(s_pHalContext);
	EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1));
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 255, 255));
	EVE_CoCmd_resetFonts(s_pHalContext);
	EVE_CoCmd_text(s_pHalContext, 0, 0, 27, 0, "Call CMD_RESETFONTS and use font 30:");
	EVE_CoCmd_text(s_pHalContext, 0, 200, UNICODE_HANDLE, 0, "The quick brown fox jumps over the lazy dog");
	EVE_Cmd_wr32(s_pHalContext, DISPLAY());
	EVE_CoCmd_swap(s_pHalContext);
	EVE_Cmd_waitFlush(s_pHalContext);
	EVE_sleep(5000);
}

/**
 * @brief API to demonstrate OTP_FORMAT
 *
 */
void SAMAPP_PrintfType() //must call after SAMAPP_ExtendedFormat_Font
{
	Draw_Text(s_pHalContext, "OPT_FORMAT");
	uint8_t c = 51;
	uint32_t a = 0x12a000;
	int16_t mV = 1947;
	int32_t t = 680;

	EVE_CoCmd_dlStart(s_pHalContext);
	EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1));
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 255, 255));
	EVE_CoCmd_text(s_pHalContext, 10, 20, 31, 0, "%");
	EVE_CoCmd_text(s_pHalContext, 10, 20 + 50 * 1, 31, 0, "%%");
	EVE_CoCmd_text(s_pHalContext, 10, 20 + 50 * 2, 31, 0, "%d%%%");
	EVE_CoCmd_text(s_pHalContext, 10, 20 + 50 * 3, 31, OPT_FORMAT, "%d * %d = %d", 100, 200, 20000);
	EVE_CoCmd_text(s_pHalContext, 10, 20 + 50 * 4, 31, OPT_FORMAT, "%3d%% complete", c); /*result:  51 % complete */
	EVE_CoCmd_text(s_pHalContext, 10, 20 + 50 * 5, 31, OPT_FORMAT, "base address %06x", a); /*result:  base address 12a000 */
	EVE_CoCmd_text(s_pHalContext, 10, 20 + 50 * 6, 31, OPT_FORMAT, "%+5.3d mV", mV); /*result:  + 1947 mV */
	EVE_CoCmd_text(s_pHalContext, 10, 20 + 50 * 7, 31, OPT_FORMAT, "Temp. %d.%.1d degrees", t / 10, t % 10); /*result:  Temp. 68.0 degrees */

	EVE_Hal_wrMem(s_pHalContext, RAM_G + 4, "Hello ", 6);
	EVE_CoCmd_text(s_pHalContext, 10, 20 + 50 * 8, 31, OPT_FORMAT, "%s %d times", RAM_G + 4, 5); /*result:  Temp. 68.0 degrees */

	EVE_Cmd_wr32(s_pHalContext, DISPLAY());
	EVE_CoCmd_swap(s_pHalContext);
	EVE_Cmd_waitFlush(s_pHalContext);
	EVE_sleep(5000);
}

/**
 * @brief API to demonstrate Video display from Flash
 *
 */
void SAMAPP_Video_Flash()
{
#define ADDR_VIDEO 4096
	FlashHelper_SwitchFullMode(s_pHalContext);
	EVE_CoCmd_dlStart(s_pHalContext);
	EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1));
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 255, 255));
	EVE_CoCmd_text(s_pHalContext, 0, 150, 30, 0, "Video display from Flash");
	EVE_Cmd_waitFlush(s_pHalContext);
	EVE_CoCmd_flashSource(s_pHalContext, ADDR_VIDEO);
	EVE_Cmd_wr32(s_pHalContext, CMD_PLAYVIDEO);
	EVE_Cmd_wr32(s_pHalContext, OPT_FLASH | OPT_SOUND | OPT_NOTEAR | OPT_OVERLAY);
	EVE_Cmd_waitFlush(s_pHalContext); //Video plays after this
	EVE_CoCmd_nop(s_pHalContext);
	EVE_Cmd_waitFlush(s_pHalContext);
	EVE_sleep(5000);
	EVE_Hal_wr8(s_pHalContext, REG_PLAY_CONTROL, -1);
}

/**
 * @brief Draw set 10
 *
 */
void App_Set_10_Draw() {
	SAMAPP_ASTC_layout_cell_1_3x2_case();
	SAMAPP_Mediafifo_CmdInflate_from_flash();
	SAMAPP_Program_Flash();

	SAMAPP_Video_Flash();
	SAMAPP_ASTC_layout_ramg();
	SAMAPP_ASTC_layout_flash();
	SAMAPP_ASTC();
	SAMAPP_Mediafifo_CmdInflate();
	SAMAPP_GradientA();
	SAMAPP_Cmd_FillWidth();
	SAMAPP_Rotate();
	SAMAPP_Animation();
	SAMAPP_ExtendedFormat_Font();
	SAMAPP_ResetFont();//must call after SAMAPP_ExtendedFormat_Font
	SAMAPP_PrintfType();
	SAMAPP_Flash_State();
	SAMAPP_CoprocessorFaults();
}

/**
 * @brief Init set 10
 *
 */
void App_Set_10_Init(EVE_HalContext* pHalContext) {
	s_pHalContext = pHalContext;
	Draw_Text(s_pHalContext, "Set10   START");
	save_REG_PCLK = EVE_Hal_rd32(s_pHalContext, REG_PCLK);
}

/**
 * @brief Deinit set 10
 *
 */
void App_Set_10_Deinit() {
	Draw_Text(s_pHalContext, "Set10   END");
}

#endif // SAMAPP_ENABLE_APIS_SET10
