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
  *  Description: This is sample demo for BULK transfer
  */

#include "Platform.h"
#include "Gpu_Hal.h"
#include "Common.h"
#include "App.h"
#include "Samapp_Enable_API.h"

#include "Set_11.h"

#ifdef SAMAPP_ENABLE_APIS_SET11

static EVE_HalContext *s_pHalContext;

#define CMD_BUFFER_SIZE (1024 * 5)

/**
 * @brief Set the 11 QueueCmd objectQueue command
 *
 * @param cmd Command to queue
 * @param CmdBuffer Command buffer
 * @param CmdBuffer_Index Command index
 */
static void set11_QueueCmd(uint32_t cmd, uint8_t* CmdBuffer, uint32_t* CmdBuffer_Index) {
	uint32_t* pBuffcmd;

	/* Prevent buffer overflow */
	if (*CmdBuffer_Index >= CMD_BUFFER_SIZE) {
		printf("CmdBuffer overflow\n");
		return;
	}

	/* Copy the command instruction into buffer */
	pBuffcmd = (uint32_t*)&CmdBuffer[*CmdBuffer_Index];
	*pBuffcmd = cmd;

	/* Increment the command index */
	*CmdBuffer_Index += CMD_SIZE;
}

/**
 * @brief Queue string
 *
 * @param s String to queue
 * @param CmdBuffer Command buffer
 * @param CmdBuffer_Index Command index
 */
void set11_QueueString(const char8_t* s, uint8_t* CmdBuffer, uint32_t* CmdBuffer_Index) {
	uint16_t length = 0;

	/* Prevent buffer overflow */
	if (*CmdBuffer_Index >= CMD_BUFFER_SIZE) {
		printf("CmdBuffer overflow\n");
		return;
	}

	/* Copy the command instruction into buffer */
	length = (uint16_t)strlen(s) + 1;//last for the null termination
	strcpy((char*)&CmdBuffer[*CmdBuffer_Index], s);

	/* increment the length and align it by 4 bytes */
	*CmdBuffer_Index += ((length + 3) & ~3);
}

/**
 * @brief Flush command to Coprocessor
 *
 * @param CmdBuffer Command buffer
 * @param CmdBuffer_Index Command index
 */
static void set11_FlushCmd(const uint8_t* CmdBuffer, uint32_t* CmdBuffer_Index) {
	uint32_t length = 0;
	uint32_t byteTransfered = 0;
	uint32_t byteAvailable = 0;

	while (TRUE) {
		byteAvailable = EVE_Cmd_space(s_pHalContext);

		while (byteAvailable == 0) {
			EVE_sleep(10);
			byteAvailable = EVE_Cmd_space(s_pHalContext);
		}

		length = *CmdBuffer_Index;

		if (length > byteAvailable) {
			length = byteAvailable;
		}

		EVE_Cmd_wrMem(s_pHalContext, CmdBuffer + byteTransfered, length);
		byteTransfered += length;

		if (*CmdBuffer_Index > length) {
			*CmdBuffer_Index -= length;
		}
		else {
			*CmdBuffer_Index = 0;
			break;
		}
	}
}

/**
 * @brief Load image via comand buffer
 *
 * @param CmdBuffer Command buffer
 * @param CmdBuffer_Index Command index
 */
static void set11_load_image(uint8_t* CmdBuffer, uint32_t* CmdBuffer_Index) {
	uint32_t fileSize = 0;

	fileSize = FileIO_File_Open(TEST_DIR "\\mandrill256.jpg", FILEIO_E_FOPEN_READ);

	/******************* Decode jpg output into location 0 and output color format as RGB565 *********************/
	set11_QueueCmd(CMD_LOADIMAGE, CmdBuffer, CmdBuffer_Index);
	set11_QueueCmd(0, CmdBuffer, CmdBuffer_Index);//destination address of jpg decode
	set11_QueueCmd(0, CmdBuffer, CmdBuffer_Index);//output format of the bitmap
	set11_FlushCmd(CmdBuffer, CmdBuffer_Index);

	/* Copy the deflated/jpeg encoded data into coprocessor fifo */
	while (fileSize > 0) {
		uint16_t freeBuffer = (uint16_t)(CMD_BUFFER_SIZE - *CmdBuffer_Index);
		/* download the data into the command buffer by 2kb one shot */
		uint16_t blocklen = fileSize > freeBuffer ? freeBuffer : (uint16_t)fileSize;

		/* copy the data into pbuff and then transfter it to command buffer */
		FileIO_File_Read(CmdBuffer + *CmdBuffer_Index, blocklen);
		*CmdBuffer_Index += blocklen;

		fileSize -= blocklen;
		set11_FlushCmd(CmdBuffer, CmdBuffer_Index);
	}
	FileIO_File_Close();
}

/**
 * @brief Draw set 11
 *
 */

void App_Set_11_Draw() {
	SAMAPP_Circle_t circles[100];
	static uint8_t  CmdBuffer[CMD_BUFFER_SIZE];
	uint32_t CmdBuffer_Index = 0;
	uint32_t precision = 16;
	uint32_t CNUM = 0;// Disable circles
	uint32_t ImgW = 256;
	uint32_t ImgH = 256;
	uint32_t xoffset = (s_pHalContext->Width - ImgW) / 2;
	uint32_t yoffset = (s_pHalContext->Height - ImgH) / 2;
	int count = 0;

	for (int i = 0; i < (int)CNUM; i++) {
		circles[i].visible = 0;
		circles[i].opacity = 0;
		circles[i].radius = 0;
		circles[i].step = 0;
		circles[i].visible = 0;
		circles[i].x = 0;
		circles[i].y = 0;
		circles[i].color.b = 0;
		circles[i].color.g = 0;
		circles[i].color.r = 0;
	}

	for (int i = 0; i < EVE_CMD_FIFO_SIZE; i++) {
		CmdBuffer[i] = 0;
	}
	set11_load_image(CmdBuffer, &CmdBuffer_Index);

	while (count++ < 60 * 10) {// wait 10 seconds, 60 FPS
		/*Display List start*/
		set11_QueueCmd(CMD_DLSTART, CmdBuffer, &CmdBuffer_Index);
		set11_QueueCmd(CLEAR_COLOR_RGB(0, 0, 0), CmdBuffer, &CmdBuffer_Index);
		set11_QueueCmd(CLEAR(1, 1, 1), CmdBuffer, &CmdBuffer_Index);
		// draw circles
		for (int i = 0; i < (int)CNUM; i++) {
			int visible = rand() % 3;
			int x = rand() % s_pHalContext->Width;
			int y = rand() % s_pHalContext->Height;
			int radius = rand() % 20;
			int r = rand() % 255;
			int g = rand() % 255;
			int b = rand() % 255;
			int step = rand() % 100 / 20;

			if (visible != 1) {
				visible = 0;
			}

			if (circles[i].visible == 0
				&& visible == 1) {
				circles[i].visible = 1;
				circles[i].x = x;
				circles[i].y = y;
				circles[i].radius = radius;
				circles[i].color.r = r;
				circles[i].color.g = g;
				circles[i].color.b = b;
				circles[i].opacity = 0;
				circles[i].step = step;
			}
			else if (circles[i].opacity >= 255) {
				circles[i].opacity += circles[i].step;
				circles[i].opacity = 255;
				circles[i].step = -circles[i].step;
			}
			else if (circles[i].opacity <= 0) {
				circles[i].opacity += circles[i].step;
				circles[i].visible = 0;
			}
			
			if (circles[i].visible) {
				set11_QueueCmd(COLOR_A(circles[i].opacity), CmdBuffer, &CmdBuffer_Index);
				set11_QueueCmd(COLOR_RGB(circles[i].color.r, circles[i].color.g, circles[i].color.b), CmdBuffer, &CmdBuffer_Index);

				set11_QueueCmd(BEGIN(FTPOINTS), CmdBuffer, &CmdBuffer_Index);
				set11_QueueCmd(POINT_SIZE(circles[i].radius * precision), CmdBuffer, &CmdBuffer_Index);
				set11_QueueCmd(VERTEX2F(circles[i].x * precision, circles[i].y * precision), CmdBuffer, &CmdBuffer_Index);
				set11_QueueCmd(END(), CmdBuffer, &CmdBuffer_Index);
			}
		}
		// reset coloring
		set11_QueueCmd(COLOR_A(255), CmdBuffer, &CmdBuffer_Index);
		set11_QueueCmd(COLOR_RGB(255, 255, 255), CmdBuffer, &CmdBuffer_Index);

		// draw image
		set11_QueueCmd(BITMAP_HANDLE(2), CmdBuffer, &CmdBuffer_Index);
		set11_QueueCmd(BEGIN(BITMAPS), CmdBuffer, &CmdBuffer_Index);
		set11_QueueCmd(BITMAP_SOURCE(0), CmdBuffer, &CmdBuffer_Index);
		set11_QueueCmd(BITMAP_LAYOUT(RGB565, ImgW * 2, ImgH * 2), CmdBuffer, &CmdBuffer_Index);
		set11_QueueCmd(BITMAP_SIZE(BILINEAR, BORDER, BORDER, ImgW, ImgH), CmdBuffer, &CmdBuffer_Index);
		set11_QueueCmd(VERTEX2F(xoffset * precision, yoffset * precision), CmdBuffer, &CmdBuffer_Index);
		set11_QueueCmd(END(), CmdBuffer, &CmdBuffer_Index);

		// Draw the text in top of screen
		uint32_t x = s_pHalContext->Width / 2 - 120;
		uint32_t y = 10;
		uint32_t font = 28;
		uint32_t opt = 0;
		set11_QueueCmd(CMD_TEXT, CmdBuffer, &CmdBuffer_Index);
		set11_QueueCmd(((y << 16) | (x & 0xffff)), CmdBuffer, &CmdBuffer_Index);
		set11_QueueCmd(((opt << 16) | (font & 0xffff)), CmdBuffer, &CmdBuffer_Index);
		set11_QueueString("Bulk transfer demostration", CmdBuffer, &CmdBuffer_Index);

		y = 50;
		x -= 200;
		set11_QueueCmd(CMD_TEXT, CmdBuffer, &CmdBuffer_Index);
		set11_QueueCmd(((y << 16) | (x & 0xffff)), CmdBuffer, &CmdBuffer_Index);
		set11_QueueCmd(((opt << 16) | (font & 0xffff)), CmdBuffer, &CmdBuffer_Index);
		set11_QueueString("Commands are transferd into REG_CMDB_WRITE instead of RAM_CMD", CmdBuffer, &CmdBuffer_Index);

		set11_QueueCmd(DISPLAY(), CmdBuffer, &CmdBuffer_Index); //send command display - to end display commands
		set11_QueueCmd(CMD_SWAP, CmdBuffer, &CmdBuffer_Index); // draw the new screen
		// Flush all the command
		set11_FlushCmd(CmdBuffer, &CmdBuffer_Index); // flush command buffer to EVE
		EVE_Cmd_waitFlush(s_pHalContext); // Wait until EVE is free
	}
}

/**
 * @brief Init set 11
 *
 */
void App_Set_11_Init(EVE_HalContext* pHalContext) {
	s_pHalContext = pHalContext;
	Draw_Text(s_pHalContext, "Set11   Start!");
}

/**
 * @brief Deinit set 11
 *
 */
void App_Set_11_Deinit() {
	Draw_Text(s_pHalContext, "Set11   End!");
}

#endif // SAMAPP_ENABLE_APIS_SET11
