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
  *  Description: This is sample demo for MainWindow
  */

#include "Platform.h"
#include "Gpu_Hal.h"

#include "Samapp_Enable_API.h"
#include "Common.h"
#include "App.h"

#ifdef SAMAPP_ENABLE_APIS_SET6

static EVE_HalContext *s_pHalContext;
/* Beginning MainWindow section */
PROGMEM SAMAPP_Logo_Img_t Main_Icons[1] =
{
	{ TEST_DIR "\\tile3.bin",ImH,RGB565,NEAREST,ImW,ImH,ImW * 2,0L },
};

/**
 * @brief Transfer file to RAM_G
 *
 * @param add Address on RAM_G
 * @param sectors Number of sector
 * @param afile File pointer
 */
void FilePtr_to_Ramg(uint32_t add, uint8_t sectors, FILE* afile) {
	uint8_t pbuff[512];

	for (int z = 0; z < sectors; z++) {
		fread(pbuff, 1, 512, afile);
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
void load_inflate_image(uint32_t address, const char* filename)
{
	FILE* afile;
	uint32_t ftsize = 0;
	uint8_t pbuff[8192];
	uint16_t blocklen;

	EVE_Cmd_wr32(s_pHalContext, CMD_INFLATE);
	EVE_Cmd_wr32(s_pHalContext, address * 1024L);
	afile = fopen(filename, "rb");		// read Binary (rb)
	if (!afile) {
		return;
	}
	fseek(afile, 0, SEEK_END);
	ftsize = ftell(afile);
	fseek(afile, 0, SEEK_SET);
	while (ftsize > 0)
	{
		blocklen = ftsize > 8192 ? 8192 : (uint16_t)ftsize;
		fread(pbuff, 1, blocklen, afile);/* copy the data into pbuff and then transfter it to command buffer */
		ftsize -= blocklen;
		EVE_Cmd_wrMem(s_pHalContext, pbuff, blocklen);/* copy data continuously into command memory */
	}
	fclose(afile);/* close the opened jpg file */
}

/**
 * @brief Setup logo
 *
 * @param sptr Logo image
 * @param num Number of image
 */
void Logo_Intial_setup(const SAMAPP_Logo_Img_t sptr[], uint8_t num)
{
	uint8_t z;
	for (z = 0; z < num; z++)
	{
		load_inflate_image(sptr[z].gram_address, sptr[z].name);
	}

	EVE_CoCmd_dlStart(s_pHalContext);// start
	EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1));
	for (z = 0; z < num; z++)
	{
		EVE_Cmd_wr32(s_pHalContext, BITMAP_HANDLE(z));
		EVE_Cmd_wr32(s_pHalContext, BITMAP_SOURCE(sptr[z].gram_address * 1024L));
		EVE_Cmd_wr32(s_pHalContext, BITMAP_LAYOUT(sptr[z].image_format, sptr[z].linestride, sptr[z].image_height));
		EVE_Cmd_wr32(s_pHalContext, BITMAP_SIZE(sptr[z].filter, BORDER, BORDER, sptr[z].linestride / 2, sptr[z].image_height));

	}
	EVE_Cmd_wr32(s_pHalContext, DISPLAY());
	EVE_CoCmd_swap(s_pHalContext);
	EVE_Cmd_waitFlush(s_pHalContext);
}

int32_t RowNo[5];
int32_t ColNo[5];
int32_t Volume;

/**
 * @brief API to demonstrate touch test
 *
 * @param Sq Square positions
 * @param TouchXY TouchXY value
 * @param TouchNo Touch number order
 */
void CheckTouch_tile(SAMAPP_Squares_t* Sq, int32_t TouchXY, uint8_t TouchNo)
{
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

	EVE_Cmd_wr32(s_pHalContext, VERTEX2F(((ImW + 2) * ColNo[TouchNo]) * 16, ((ImH + 2) * RowNo[TouchNo]) * 16));
}

/**
 * @brief Draw set 6
 *
 */
void App_Set_06_Draw() {
	Draw_Text(s_pHalContext, "Touch to play song");
	uint32_t val[6];

	FILE* afile;
	int32_t ftsize = 0;
	int32_t AddrOffset;

	int32_t rp = 0;
	int32_t audioval;
	int32_t wp = 0;

	SAMAPP_Squares_t SqCall;

	Logo_Intial_setup(Main_Icons, 1);
	EVE_Hal_wr8(s_pHalContext, REG_CTOUCH_EXTENDED, CTOUCH_MODE_EXTENDED);
	EVE_sleep(30);
	AddrOffset = 102400L;

	/*Audio*/

	afile = fopen(TEST_DIR "\\Devil_Ride_30_44100_ulaw.wav", "rb");// read Binary (rb)
	if (!afile) {
		return;
	}
	fseek(afile, 0, SEEK_END);
	ftsize = ftell(afile);
	fseek(afile, 0, SEEK_SET);

	FilePtr_to_Ramg(AddrOffset + 0UL, 64 * 2, afile);

	wp = 1024;
	ftsize -= 1024;

	EVE_Hal_wr32(s_pHalContext, REG_PLAYBACK_FREQ, 44100);
	EVE_Hal_wr32(s_pHalContext, REG_PLAYBACK_START, AddrOffset);
	EVE_Hal_wr32(s_pHalContext, REG_PLAYBACK_FORMAT, ULAW_SAMPLES);
	EVE_Hal_wr32(s_pHalContext, REG_PLAYBACK_LENGTH, APPBUFFERSIZE);
	EVE_Hal_wr32(s_pHalContext, REG_PLAYBACK_LOOP, 1);
	EVE_Hal_wr8(s_pHalContext, REG_VOL_PB, (uint8_t)Volume);
	EVE_Hal_wr8(s_pHalContext, REG_PLAYBACK_PLAY, 1);

	for (int j = 0; j < 1500; j++) {
		EVE_Cmd_wr32(s_pHalContext, CMD_DLSTART);
		EVE_Cmd_wr32(s_pHalContext, CLEAR_COLOR_RGB(0, 0, 0));
		EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1));
		EVE_CoCmd_text(s_pHalContext, (int16_t)(s_pHalContext->Width / 2), 30, 26, OPT_CENTER, "Touch to play song");//text info

		val[0] = EVE_Hal_rd32(s_pHalContext, REG_CTOUCH_TOUCH0_XY);
		val[1] = EVE_Hal_rd32(s_pHalContext, REG_CTOUCH_TOUCH1_XY);
		val[2] = EVE_Hal_rd32(s_pHalContext, REG_CTOUCH_TOUCH2_XY);
		val[3] = EVE_Hal_rd32(s_pHalContext, REG_CTOUCH_TOUCH3_XY);
		val[4] = (EVE_Hal_rd16(s_pHalContext, REG_CTOUCH_TOUCH4_X) << 16) | (EVE_Hal_rd16(s_pHalContext, REG_CTOUCH_TOUCH4_Y));

		for (int8_t i = 0; i < NO_OF_TOUCH; i++) {
			CheckTouch_tile(&SqCall, (int32_t)val[i], i);
		}
		if ((val[0] == 2147516416) && (val[1] == 2147516416) && (val[2] == 2147516416) && (val[3] == 2147516416) && (val[4] == 2147516416))
			Volume = 0;
		EVE_Cmd_wr32(s_pHalContext, DISPLAY());
		EVE_CoCmd_swap(s_pHalContext);
		EVE_Cmd_waitFlush(s_pHalContext);

		rp = EVE_Hal_rd16(s_pHalContext, REG_PLAYBACK_READPTR);
		audioval = APPBUFFERSIZEMINUSONE & (rp - wp);
		if (audioval > 1024) {
			uint16_t n = min(1024, (uint16_t)ftsize);
			FilePtr_to_Ramg(AddrOffset + wp, 2, afile);
			wp = (wp + 1024) & APPBUFFERSIZEMINUSONE;
			ftsize -= n;
			EVE_Hal_wr8(s_pHalContext, REG_VOL_PB, (uint8_t)Volume);

		}
		if (wp > APPBUFFERSIZE) break;//Add to prevent over buffer
	}
	EVE_Hal_wr8(s_pHalContext, REG_VOL_PB, 0);
	EVE_Hal_wr8(s_pHalContext, REG_PLAYBACK_PLAY, 0);
	fclose(afile);

	EVE_Hal_wr8(s_pHalContext, REG_CTOUCH_EXTENDED, CTOUCH_MODE_COMPATIBILITY);
}

/**
 * @brief Init set 6
 *
 */
void App_Set_06_Init(EVE_HalContext* pHalContext) {
	s_pHalContext = pHalContext;
	Draw_Text(s_pHalContext, "Set6   Start!");
}

/**
 * @brief Deinit set 6
 *
 */
void App_Set_06_Deinit() {
	Draw_Text(s_pHalContext, "Set6   End!");
}

#endif // SAMAPP_ENABLE_APIS_SET6
