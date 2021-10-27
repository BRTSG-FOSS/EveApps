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
  * Description: Demo application for BT815A platform
  */

#include "Platform.h"
#include "EVE_CoCmd.h"
#include "Common.h"
#include "App.h"

static EVE_HalContext *s_pHalContext;
#include "FTGesture.h"

#define IMAGE_SELECTIONMENU	101
#define MAX_IMAGES	18

typedef signed short  int16;
typedef struct FTImageViewer
{
	int16_t Ox;
	int16_t Oy;
	uint8_t dx;
	uint8_t dy;
	uint8_t imw;
	uint8_t imh;
	uint8_t imageno;
}FTImageViewer;

static FTImageViewer mainMenu, subMenu;
static uint32_t CmdBuffer_Index;
static uint32_t DlBuffer_Index;

typedef struct SAMAPP_Bitmap_Tile {
	int index, has_tag, format, addressFlash, addressRamg, w, h;
}SAMAPP_Bitmap_Tile_t;

SAMAPP_Bitmap_Tile_t tile[] = {
	//id  has_tag?  format                        flash      ramg      w      h
	{ 0  , 0,        COMPRESSED_RGBA_ASTC_4x4_KHR, 0         ,0        ,0     ,0 },     // unified.blob                                    : 0       : 4096
	{ 1  , 1,        COMPRESSED_RGBA_ASTC_4x4_KHR, 4096      ,4096     ,120   ,60 },    // F0_120x60_COMPRESSED_RGBA_ASTC_4x4_KHR.raw      : 4096    : 7232
	{ 2  , 0,        COMPRESSED_RGBA_ASTC_4x4_KHR, 11328     ,4096     ,120   ,60 },    // F1_120x60_COMPRESSED_RGBA_ASTC_4x4_KHR.raw      : 11328   : 7232
	{ 3  , 0,        COMPRESSED_RGBA_ASTC_4x4_KHR, 18560     ,4096     ,120   ,60 },    // F2_120x60_COMPRESSED_RGBA_ASTC_4x4_KHR.raw      : 18560   : 7232
	{ 4  , 0,        COMPRESSED_RGBA_ASTC_4x4_KHR, 25792     ,4096     ,120   ,60 },    // F3_120x60_COMPRESSED_RGBA_ASTC_4x4_KHR.raw      : 25792   : 7232
	{ 5  , 0,        COMPRESSED_RGBA_ASTC_4x4_KHR, 33024     ,4096     ,120   ,60 },    // F4_120x60_COMPRESSED_RGBA_ASTC_4x4_KHR.raw      : 33024   : 7232
	{ 6  , 0,        COMPRESSED_RGBA_ASTC_4x4_KHR, 40256     ,4096     ,120   ,60 },    // F5_120x60_COMPRESSED_RGBA_ASTC_4x4_KHR.raw      : 40256   : 7232
	{ 7  , 0,        COMPRESSED_RGBA_ASTC_4x4_KHR, 47488     ,4096     ,120   ,60 },    // F6_120x60_COMPRESSED_RGBA_ASTC_4x4_KHR.raw      : 47488   : 7232
	{ 8  , 0,        COMPRESSED_RGBA_ASTC_4x4_KHR, 54720     ,4096     ,120   ,60 },    // F7_120x60_COMPRESSED_RGBA_ASTC_4x4_KHR.raw      : 54720   : 7232
	{ 9  , 0,        COMPRESSED_RGBA_ASTC_4x4_KHR, 61952     ,4096     ,120   ,60 },    // F8_120x60_COMPRESSED_RGBA_ASTC_4x4_KHR.raw      : 61952   : 7232
	{ 10 , 0,        COMPRESSED_RGBA_ASTC_4x4_KHR, 69184     ,4096     ,120   ,60 },    // F9_120x60_COMPRESSED_RGBA_ASTC_4x4_KHR.raw      : 69184   : 7232
	{ 11 , 0,        COMPRESSED_RGBA_ASTC_4x4_KHR, 76416     ,4096     ,120   ,60 },    // F10_120x60_COMPRESSED_RGBA_ASTC_4x4_KHR.raw     : 76416   : 7232
	{ 12 , 0,        COMPRESSED_RGBA_ASTC_4x4_KHR, 83648     ,4096     ,120   ,60 },    // F11_120x60_COMPRESSED_RGBA_ASTC_4x4_KHR.raw     : 83648   : 7232
	{ 13 , 0,        COMPRESSED_RGBA_ASTC_4x4_KHR, 90880     ,4096     ,120   ,60 },    // F12_120x60_COMPRESSED_RGBA_ASTC_4x4_KHR.raw     : 90880   : 7232
	{ 14 , 0,        COMPRESSED_RGBA_ASTC_4x4_KHR, 98112     ,4096     ,120   ,60 },    // F13_120x60_COMPRESSED_RGBA_ASTC_4x4_KHR.raw     : 98112   : 7232
	{ 15 , 0,        COMPRESSED_RGBA_ASTC_4x4_KHR, 105344    ,4096     ,120   ,60 },    // F14_120x60_COMPRESSED_RGBA_ASTC_4x4_KHR.raw     : 105344  : 7232
	{ 16 , 0,        COMPRESSED_RGBA_ASTC_4x4_KHR, 112576    ,4096     ,120   ,60 },    // F15_120x60_COMPRESSED_RGBA_ASTC_4x4_KHR.raw     : 112576  : 7232
	{ 17 , 0,        COMPRESSED_RGBA_ASTC_4x4_KHR, 119808    ,4096     ,120   ,60 },    // F16_120x60_COMPRESSED_RGBA_ASTC_4x4_KHR.raw     : 119808  : 7232
	{ 18 , 0,        COMPRESSED_RGBA_ASTC_4x4_KHR, 127040    ,4096     ,120   ,60 },    // F17_120x60_COMPRESSED_RGBA_ASTC_4x4_KHR.raw     : 127040  : 7232
	{ 19 , 0,        COMPRESSED_RGBA_ASTC_4x4_KHR, 134272    ,4096     ,120   ,60 },    // F18_120x60_COMPRESSED_RGBA_ASTC_4x4_KHR.raw     : 134272  : 7232
	{ 20 , 0,        COMPRESSED_RGBA_ASTC_4x4_KHR, 141504    ,4096     ,120   ,60 },    // F19_120x60_COMPRESSED_RGBA_ASTC_4x4_KHR.raw     : 141504  : 7232
	{ 21 , 0,        COMPRESSED_RGBA_ASTC_4x4_KHR, 148736    ,0        ,800   ,480 },   // S_F0_800x480_COMPRESSED_RGBA_ASTC_4x4_KHR.raw   : 148736  : 384000
	{ 22 , 0,        COMPRESSED_RGBA_ASTC_4x4_KHR, 532736    ,0        ,800   ,480 },   // S_F1_800x480_COMPRESSED_RGBA_ASTC_4x4_KHR.raw   : 532736  : 384000
	{ 23 , 0,        COMPRESSED_RGBA_ASTC_4x4_KHR, 916736    ,0        ,800   ,480 },   // S_F2_800x480_COMPRESSED_RGBA_ASTC_4x4_KHR.raw   : 916736  : 384000
	{ 24 , 0,        COMPRESSED_RGBA_ASTC_4x4_KHR, 1300736   ,0        ,800   ,480 },   // S_F3_800x480_COMPRESSED_RGBA_ASTC_4x4_KHR.raw   : 1300736 : 384000
	{ 25 , 0,        COMPRESSED_RGBA_ASTC_4x4_KHR, 1684736   ,0        ,800   ,480 },   // S_F4_800x480_COMPRESSED_RGBA_ASTC_4x4_KHR.raw   : 1684736 : 384000
	{ 26 , 0,        COMPRESSED_RGBA_ASTC_4x4_KHR, 2068736   ,0        ,800   ,480 },   // S_F5_800x480_COMPRESSED_RGBA_ASTC_4x4_KHR.raw   : 2068736 : 384000
	{ 27 , 0,        COMPRESSED_RGBA_ASTC_4x4_KHR, 2452736   ,0        ,800   ,480 },   // S_F6_800x480_COMPRESSED_RGBA_ASTC_4x4_KHR.raw   : 2452736 : 384000
	{ 28 , 0,        COMPRESSED_RGBA_ASTC_4x4_KHR, 2836736   ,0        ,800   ,480 },   // S_F7_800x480_COMPRESSED_RGBA_ASTC_4x4_KHR.raw   : 2836736 : 384000
	{ 29 , 0,        COMPRESSED_RGBA_ASTC_4x4_KHR, 3220736   ,0        ,800   ,480 },   // S_F8_800x480_COMPRESSED_RGBA_ASTC_4x4_KHR.raw   : 3220736 : 384000
	{ 30 , 0,        COMPRESSED_RGBA_ASTC_4x4_KHR, 3604736   ,0        ,800   ,480 },   // S_F9_800x480_COMPRESSED_RGBA_ASTC_4x4_KHR.raw   : 3604736 : 384000
	{ 31 , 0,        COMPRESSED_RGBA_ASTC_4x4_KHR, 3988736   ,0        ,800   ,480 },   // S_F10_800x480_COMPRESSED_RGBA_ASTC_4x4_KHR.raw  : 3988736 : 384000
	{ 32 , 0,        COMPRESSED_RGBA_ASTC_4x4_KHR, 4372736   ,0        ,800   ,480 },   // S_F11_800x480_COMPRESSED_RGBA_ASTC_4x4_KHR.raw  : 4372736 : 384000
	{ 33 , 0,        COMPRESSED_RGBA_ASTC_4x4_KHR, 4756736   ,0        ,800   ,480 },   // S_F12_800x480_COMPRESSED_RGBA_ASTC_4x4_KHR.raw  : 4756736 : 384000
	{ 34 , 0,        COMPRESSED_RGBA_ASTC_4x4_KHR, 5140736   ,0        ,800   ,480 },   // S_F13_800x480_COMPRESSED_RGBA_ASTC_4x4_KHR.raw  : 5140736 : 384000
	{ 35 , 0,        COMPRESSED_RGBA_ASTC_4x4_KHR, 5524736   ,0        ,800   ,480 },   // S_F14_800x480_COMPRESSED_RGBA_ASTC_4x4_KHR.raw  : 5524736 : 384000
	{ 36 , 0,        COMPRESSED_RGBA_ASTC_4x4_KHR, 5908736   ,0        ,800   ,480 },   // S_F15_800x480_COMPRESSED_RGBA_ASTC_4x4_KHR.raw  : 5908736 : 384000
	{ 37 , 0,        COMPRESSED_RGBA_ASTC_4x4_KHR, 6292736   ,0        ,800   ,480 },   // S_F16_800x480_COMPRESSED_RGBA_ASTC_4x4_KHR.raw  : 6292736 : 384000
	{ 38 , 0,        COMPRESSED_RGBA_ASTC_4x4_KHR, 6676736   ,0        ,800   ,480 },   // S_F17_800x480_COMPRESSED_RGBA_ASTC_4x4_KHR.raw  : 6676736 : 384000
	{ 39 , 0,        COMPRESSED_RGBA_ASTC_4x4_KHR, 7060736   ,0        ,800   ,480 },   // S_F18_800x480_COMPRESSED_RGBA_ASTC_4x4_KHR.raw  : 7060736 : 384000
	{ 40 , 0,        COMPRESSED_RGBA_ASTC_4x4_KHR, 7444736   ,0        ,800   ,480 },   // S_F19_800x480_COMPRESSED_RGBA_ASTC_4x4_KHR.raw  : 7444736 : 384000
	{ 41 , 0,        COMPRESSED_RGBA_ASTC_4x4_KHR, 7444736 + 384000, 0, 0, 0 },
};

#define _ID_TILE_FIRST 1
#define _ID_TILE_LAST 41

#define _TILE_SIZE  (tile[_ID_TILE_LAST].addressFlash)
#define _TILE_SIZE_120x60  (tile[20].addressFlash + 7232 - tile[_ID_TILE_FIRST].addressFlash)

#define PRECISION 16

/* Boot up for FT800  */
/* Initial boot up DL - make the back ground green color */
const uint8_t FT_DLCODE_BOOTUP[12] =
{
	255,255,255,2,//GPU instruction CLEAR_COLOR_RGB
	7,0,0,38, //GPU instruction CLEAR
	0,0,0,0,  //GPU instruction DISPLAY
};
void dxt1Image(int16_t Ox, int16_t Oy, uint8_t handle, uint8_t cell, uint8_t tagno);
void Info()
{
	uint16_t dloffset = 0, z;
	CmdBuffer_Index = 0;
	EVE_CoCmd_dlStart(s_pHalContext);
	EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1));
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 255, 255));
	EVE_CoCmd_text(s_pHalContext, s_pHalContext->Width / 2, s_pHalContext->Height / 2, 28, OPT_CENTERX | OPT_CENTERY, "Please tap on a dot");
	EVE_CoCmd_calibrate(s_pHalContext);
	EVE_Cmd_wr32(s_pHalContext, DISPLAY());
	EVE_CoCmd_swap(s_pHalContext);
	EVE_Cmd_waitFlush(s_pHalContext);
}

void Load_file2gram(uint32_t add, uint16_t size, FILE *afile)
{
	uint8_t pbuff[512];
	uint16_t z = 0;
	int16_t _size = size;
	while (_size > 0)
	{
		fread(pbuff, 1, 512, afile);
		EVE_Hal_wrMem(s_pHalContext, add, pbuff, 512L);
		add += 512;
		_size -= 512;
	}
}

void Load_Thumbnails() {
	EVE_CoCmd_flashRead(s_pHalContext, RAM_G, tile[_ID_TILE_FIRST].addressFlash, _TILE_SIZE_120x60);
}

void Load_Thumbnails_old()
{
	uint8_t sectors = 0, color = 0, image;
	uint16_t temp_address, address = 0, w = 120, h = 60;
	char filename[120];
	int filesize = 0;
	FILE *pfile;
	int image_no = 1;
	uint16_t length = 0;
	uint8_t tempbuff[1024];
	address = 0;
	do
	{
		sprintf(filename, TEST_DIR "S%d_b%d.raw", image_no, color);
		if (color < 1) color++; else color = 0;
		if (image_no <= MAX_IMAGES && color == 0) image_no++;
		pfile = fopen(filename, "rb");            // read Binary (rb)
		fseek(pfile, 0, SEEK_END);
		filesize = ftell(pfile);
		fseek(pfile, 0, SEEK_SET);
		Load_file2gram(address, filesize, pfile);
		address += (w*h / 8);
	} while (image_no <= MAX_IMAGES);
	temp_address = address;
	image_no = 1;
	do
	{
		sprintf(filename, TEST_DIR "S%d_c%d.raw", image_no, color);
		if (color < 1) color++; else color = 0;
		if (image_no <= MAX_IMAGES && color == 0) image_no++;
		pfile = fopen(filename, "rb");            // read Binary (rb)
		fseek(pfile, 0, SEEK_END);
		filesize = ftell(pfile);
		fseek(pfile, 0, SEEK_SET);
		Load_file2gram(address, filesize, pfile);
		address += (w*h / 8);
	} while (image_no <= MAX_IMAGES);
	EVE_CoCmd_dlStart(s_pHalContext);
	EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1));

	EVE_Cmd_wr32(s_pHalContext, BITMAP_HANDLE(13));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_SOURCE(0));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_LAYOUT(L1, w / 8, h));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_SIZE(NEAREST, BORDER, BORDER, 120, 60));

	EVE_Cmd_wr32(s_pHalContext, BITMAP_HANDLE(14));                         // handle for background stars
	EVE_Cmd_wr32(s_pHalContext, BITMAP_SOURCE(temp_address));            // Starting address in gram
	EVE_Cmd_wr32(s_pHalContext, BITMAP_LAYOUT(RGB565, w / 4 * 2, h / 4));  // format 
	EVE_Cmd_wr32(s_pHalContext, BITMAP_SIZE(NEAREST, BORDER, BORDER, 120, 60));

	EVE_Cmd_wr32(s_pHalContext, DISPLAY());
	EVE_CoCmd_swap(s_pHalContext);
	EVE_Cmd_waitFlush(s_pHalContext);

	EVE_sleep(100000);
}

//Application Interface
void Load_Image(uint8_t image_no)
{
	int imgid = image_no + 20;

	if (imgid < 20 || imgid > 40) {
		return;
	}
	SAMAPP_Bitmap_Tile_t t = tile[imgid];

	EVE_CoCmd_flashRead(s_pHalContext, tile[21].addressFlash, t.addressFlash, 384000);

	return;

	uint32_t address = t.addressRamg ? (t.addressFlash - t.addressRamg) : (FLASH_ADDRESS | t.addressFlash / 32);
	EVE_CoCmd_setBitmap(s_pHalContext, address, t.format, t.w, t.h);

	return;
}

void dxt1Image(int16_t Ox, int16_t Oy, uint8_t handle, uint8_t cell, uint8_t tagno) {
	SAMAPP_Bitmap_Tile_t t = tile[tagno];
	uint32_t address = t.addressRamg ? (t.addressFlash - t.addressRamg) : (FLASH_ADDRESS | t.addressFlash / 32);

	address = (FLASH_ADDRESS | t.addressFlash / 32);

	EVE_CoCmd_setBitmap(s_pHalContext, address, t.format, t.w, t.h);
	EVE_Cmd_wr32(s_pHalContext, TAG(t.index));
	EVE_Cmd_wr32(s_pHalContext, BEGIN(BITMAPS));
	EVE_Cmd_wr32(s_pHalContext, VERTEX2F(Ox * PRECISION, Oy * PRECISION));
}
void dxt1Image_old(int16_t Ox, int16_t Oy, uint8_t handle, uint8_t cell, uint8_t tagno)
{
	int i = 0;
	float_t zin = 1;

	EVE_Cmd_wr32(s_pHalContext, RESTORE_CONTEXT());
	EVE_CoCmd_loadIdentity(s_pHalContext);
	EVE_CoCmd_scale(s_pHalContext, F16(zin), F16(zin));
	EVE_CoCmd_setMatrix(s_pHalContext);

	EVE_Cmd_wr32(s_pHalContext, BLEND_FUNC(ONE, ZERO));
	EVE_Cmd_wr32(s_pHalContext, COLOR_A(0x55));

	EVE_Cmd_wr32(s_pHalContext, BITMAP_HANDLE(handle));
	EVE_Cmd_wr32(s_pHalContext, CELL(cell));

	EVE_Cmd_wr32(s_pHalContext, VERTEX2F(Ox * 16, Oy * 16));

	EVE_Cmd_wr32(s_pHalContext, BLEND_FUNC(ONE, ONE));

	EVE_Cmd_wr32(s_pHalContext, COLOR_A(0xaa));
	EVE_Cmd_wr32(s_pHalContext, CELL(cell + 1));
	EVE_Cmd_wr32(s_pHalContext, VERTEX2F(Ox * 16, Oy * 16));

	EVE_Cmd_wr32(s_pHalContext, COLOR_MASK(1, 1, 1, 0));

	EVE_CoCmd_loadIdentity(s_pHalContext);
	EVE_CoCmd_scale(s_pHalContext, F16(zin * 4), F16(zin * 4));
	EVE_CoCmd_setMatrix(s_pHalContext);

	EVE_Cmd_wr32(s_pHalContext, BLEND_FUNC(DST_ALPHA, ZERO));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_HANDLE(handle + 1));
	EVE_Cmd_wr32(s_pHalContext, CELL(cell + 1));
	EVE_Cmd_wr32(s_pHalContext, VERTEX2F(Ox * 16, Oy * 16));

	EVE_Cmd_wr32(s_pHalContext, BLEND_FUNC(ONE_MINUS_DST_ALPHA, ONE));
	EVE_Cmd_wr32(s_pHalContext, CELL(cell));
	EVE_Cmd_wr32(s_pHalContext, TAG(tagno));
	EVE_Cmd_wr32(s_pHalContext, VERTEX2F(Ox * 16, Oy * 16));
}

void mainScreen_Init()
{
	mainMenu.Ox = 45;
	mainMenu.Oy = 66;
	mainMenu.dx = 20;
	mainMenu.dy = 10;
	mainMenu.imw = 120;
	mainMenu.imh = 60;
}

void readtouch(int16_t *x, int16_t *y)
{
	uint32_t sxy;
	{
		sxy = EVE_Hal_rd32(s_pHalContext, REG_CTOUCH_TOUCH0_XY);
		x[0] = (sxy >> 16);
		y[0] = (sxy);

		sxy = EVE_Hal_rd32(s_pHalContext, REG_CTOUCH_TOUCH1_XY);
		x[1] = (sxy >> 16);
		y[1] = (sxy);

		sxy = EVE_Hal_rd32(s_pHalContext, REG_CTOUCH_TOUCH2_XY);
		x[2] = (sxy >> 16);
		y[2] = (sxy);

		sxy = EVE_Hal_rd32(s_pHalContext, REG_CTOUCH_TOUCH3_XY);
		x[3] = (sxy >> 16);
		y[3] = (sxy);

		x[4] = EVE_Hal_rd16(s_pHalContext, REG_CTOUCH_TOUCH4_X);
		y[4] = EVE_Hal_rd16(s_pHalContext, REG_CTOUCH_TOUCH4_Y);

	}
}

float_t linear(float_t p1, float_t p2, uint16_t t, uint16_t rate)
{
	float_t st = (float_t)t / rate;
	return p1 + (st*(p2 - p1));
}

void Image_ROI(uint8_t tchs, int16_t min_x, int16 max_x, int16_t min_y, int16_t max_y)
{
	int16 phx, phy, pwx, pwy;

	phy = min_y;
	phx = min_x + (max_x - min_x) / 2;
	pwx = max_x;
	pwy = min_y + (max_y - min_y) / 2;

	EVE_Cmd_wr32(s_pHalContext, SAVE_CONTEXT());
	EVE_Cmd_wr32(s_pHalContext, COLOR_A(0));
	EVE_Cmd_wr32(s_pHalContext, BEGIN(EDGE_STRIP_L));
	EVE_Cmd_wr32(s_pHalContext, STENCIL_OP(INCR, INCR));
	EVE_Cmd_wr32(s_pHalContext, VERTEX2F(max_x * 16, min_y * 16));
	EVE_Cmd_wr32(s_pHalContext, VERTEX2F(max_x * 16, max_y * 16));
	EVE_Cmd_wr32(s_pHalContext, VERTEX2F(min_x * 16, max_y * 16));
	EVE_Cmd_wr32(s_pHalContext, VERTEX2F(min_x * 16, min_y * 16));
	EVE_Cmd_wr32(s_pHalContext, STENCIL_OP(KEEP, KEEP));
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(55, 55, 55));
	EVE_Cmd_wr32(s_pHalContext, COLOR_A(180));
	EVE_Cmd_wr32(s_pHalContext, BEGIN(EDGE_STRIP_A));
	EVE_Cmd_wr32(s_pHalContext, STENCIL_FUNC(NOTEQUAL, 1, 1));
	EVE_Cmd_wr32(s_pHalContext, VERTEX2F(0, 272 * 16));
	EVE_Cmd_wr32(s_pHalContext, VERTEX2F(480 * 16, 272 * 16));
	EVE_Cmd_wr32(s_pHalContext, RESTORE_CONTEXT());

	EVE_Cmd_wr32(s_pHalContext, BEGIN(FTPOINTS));
	EVE_Cmd_wr32(s_pHalContext, POINT_SIZE(30 * 16));
	EVE_Cmd_wr32(s_pHalContext, COLOR_A(0));
	EVE_Cmd_wr32(s_pHalContext, TAG_MASK(1));
	EVE_Cmd_wr32(s_pHalContext, TAG('H'));
	EVE_Cmd_wr32(s_pHalContext, VERTEX2F(phx * 16, phy * 16));
	EVE_Cmd_wr32(s_pHalContext, TAG('W'));
	EVE_Cmd_wr32(s_pHalContext, VERTEX2F(pwx * 16, pwy * 16));
	EVE_Cmd_wr32(s_pHalContext, TAG_MASK(0));
	EVE_Cmd_wr32(s_pHalContext, RESTORE_CONTEXT());
	EVE_Cmd_wr32(s_pHalContext, POINT_SIZE(10 * 16));
	EVE_Cmd_wr32(s_pHalContext, VERTEX2F(phx * 16, phy * 16));
	EVE_Cmd_wr32(s_pHalContext, VERTEX2F(pwx * 16, pwy * 16));
}

void Tab_Menu(char *op1, char *op2, char *op3, char *op4)
{
	EVE_CoCmd_button(s_pHalContext, 0, 240, 120, 32, 28, OPT_FLAT, op1);
	EVE_CoCmd_button(s_pHalContext, 120, 240, 120, 32, 28, OPT_FLAT, op2);
	EVE_CoCmd_button(s_pHalContext, 240, 240, 120, 32, 28, OPT_FLAT, op3);
	EVE_CoCmd_button(s_pHalContext, 360, 240, 120, 32, 28, OPT_FLAT, op4);
}

int16_t GetMax(int16_t d[], uint8_t cts)
{
	int16_t max = 0;
	uint8_t i;
	for (i = 0; i < cts; i++)
	{
		if (d[i] != -32768)
			max = d[i];
	}
	for (i = 0; i < cts; i++)
	{
		if (d[i] != -32768) {
			if (max < d[i])
				max = d[i];
		}
	}
	return max;
}

int16_t GetMin(int16_t d[], uint8_t cts)
{
	int16_t min = 240;
	uint8_t i;
	for (i = 0; i < cts; i++)
	{
		if (d[i] != -32768) {
			if (min > d[i])
				min = d[i];
		}
	}
	return min;
}

uint8_t Image_Selector()
{
	unsigned char tag = 0, temp_tag = 0;
	unsigned char tap = 0, tchs = 0;
	unsigned int gesture;
	int  i;
	int16_t Ox = 20, Oy = 0;
	int16_t dy = 20;
	int32_t scy = 0;
	int16_t scrollery = 0;
	int16_t x[5], y[5];
	FTGLInit();
	FTGLSetScrollerMode(0, 1);
	FTGLSetScrollerRange(0, 0, 0, 1188);
	while (1)
	{
		tag = EVE_Hal_rd8(s_pHalContext, REG_TOUCH_TAG);
		if (tag > 0 && tag <= 18)
			temp_tag = tag;

		readtouch(x, y);
		gesture = FTGLRun(x, y);
		if (gesture & FTGL_SCROLLERCHANGE)
		{
			FTGLGetScrollerY(&scy);
			scrollery = scy >> 4;
		}
		if (gesture & FTGL_TAPCHANGE)
		{
			FTGLGetTaptype(&tap, &tchs);
			if (tap == FTGL_TTAP_ONETAP)
			{
				if (temp_tag > 0 && temp_tag <= 18)
				{
					return temp_tag;
				}
			}
		}
		EVE_CoCmd_dlStart(s_pHalContext);
		EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1));
		EVE_Cmd_wr32(s_pHalContext, BEGIN(BITMAPS));
		EVE_Cmd_wr32(s_pHalContext, VERTEX2II(0, 36, 0, 0));
		EVE_Cmd_wr32(s_pHalContext, LINE_WIDTH(2 * 16));
		EVE_Cmd_wr32(s_pHalContext, BEGIN(RECTS));
		for (i = 0; i < MAX_IMAGES; i++)
		{
			Oy = (i*mainMenu.imh) + ((i + 1)*dy);
			Oy -= scrollery;
			if (Oy<512 && Oy>-256)
			{
				EVE_Cmd_wr32(s_pHalContext, VERTEX2F(Ox * 16, Oy * 16));
				EVE_Cmd_wr32(s_pHalContext, VERTEX2F((Ox + mainMenu.imw) * 16, (Oy + mainMenu.imh) * 16));
			}
		}
		EVE_Cmd_wr32(s_pHalContext, BEGIN(BITMAPS));
		for (i = 0; i < MAX_IMAGES; i++)
		{
			Oy = i*mainMenu.imh + ((i + 1)*dy);
			Oy -= scrollery;
			if (Oy<512 && Oy>-256)
				dxt1Image(Ox, Oy, 13, i * 2, i + 1);
		}
		EVE_Cmd_wr32(s_pHalContext, DISPLAY());
		EVE_CoCmd_swap(s_pHalContext);
		EVE_Cmd_waitFlush(s_pHalContext);
	}
}

int16_t Imageviewer_submenu()
{
	unsigned char dtap = 0, i, tag = 0, roi = 0;
	unsigned char tap_tchs = 0, tap = 0, tchs = 0;
	unsigned char alpha = 255, alpha_cts = 0;
	unsigned char animation_cts = 0;
	float_t fsx, fsy;
	int16_t sx[5], sy[5], px[5], py[5], cx[5], cy[5];
	int16 max_x = 0, min_x = 0, min_y = 0, max_y = 0;
	uint8_t nooftchs = 0;
	char filename[150];
	uint32_t gesture = 0;
	int16_t x[5], y[5];
	int16_t dx = 0, dy = 0;
	int16_t ctx = 0, cty = 0;
	int16_t scx = 100, scy = 100;
	int16_t angle = 0;
	EVE_CoCmd_dlStart(s_pHalContext);
	EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1));
	EVE_CoCmd_memSet(s_pHalContext, 64800L, 255L, 480L * 200 * 2);
	EVE_Cmd_wr32(s_pHalContext, BITMAP_HANDLE(0));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_SOURCE(64800L));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_LAYOUT(RGB565, 480 * 2L, 200));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_SIZE(BILINEAR, BORDER, BORDER, 480, 272));
	EVE_Cmd_wr32(s_pHalContext, BEGIN(BITMAPS));
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0xff, 0xff, 0xff));
	EVE_Cmd_wr32(s_pHalContext, VERTEX2II(0, 36, 0, 0));
	EVE_Cmd_wr32(s_pHalContext, DISPLAY());
	EVE_CoCmd_swap(s_pHalContext);
	EVE_Cmd_waitFlush(s_pHalContext);
	//Load_Image(subMenu.imageno);
	sprintf(filename, TEST_DIR "F%d.jpg", subMenu.imageno);
	FTGLInit();
	FTGLSetScrollerMode(0, 0);
	FTGLSetDragRange(0, 0, 0, 0);
	FTGLSetScaleRange(50, 400, 50, 400);
	FTGLSetTransform(0, 100, 100, 0, 0);
	while (1)
	{
		if (roi)
			tag = EVE_Hal_rd8(s_pHalContext, REG_TOUCH_TAG);
		if (tag != 0 && tag != 255)
		{
			int16_t cx[5], cy[5], px[5], py[5];
			FTGLGetCoordinates(sx, sy, cx, cy, px, py);
			if (tag == 'W')
			{
				if (px[0] != -32768L && cx[0] != -32768L)
					max_x += (cx[0] - px[0]);
			}
			if (tag == 'H')
			{
				if (py[0] != -32768L && cy[0] != -32768L)
					min_y += (cy[0] - py[0]);

			}
		}
		if (alpha_cts < 200)alpha_cts++;
		else { roi = 0; tap = 0; }

		readtouch(x, y);
		gesture = FTGLRun(x, y);

		if (gesture)
		{
			if (gesture & FTGL_TAPCHANGE)
			{
				alpha_cts = 0;
				FTGLGetTaptype(&tap, &tchs);
				if (tchs >= 3)
				{
					roi ^= 1;
					FTGLGetTapCoordinates(sx, sy);
					max_x = GetMax(sx, tchs);
					max_y = GetMax(sy, tchs);
					min_x = GetMin(sx, tchs);
					min_y = GetMin(sy, tchs);
				}
			}
			else if (gesture & FTGL_FLICKCHANGE)
			{
				if (!dtap && !roi)
				{
					unsigned char flick, ftchs;
					FTGLGetFlicktype(&flick, &ftchs);
					tap = 0;
					return ftchs;
				}
			}
		}

		if (FTGLIsTouch())
		{
			if (roi)
				alpha_cts = 0;
			FTGLGetNoOfTchs(&nooftchs);
			if (nooftchs == 1 && roi == 0)
				FTGLGetDrag(&dx, &dy);
			else if (nooftchs > 1)
			{
				if (gesture&FTGL_TRANSFORMCHANGE)
				{
					tap = 0;
					if (!roi) {
						FTGLGetTransform(&angle, &scx, &scy, &dx, &dy);
						FTGLSetDragRange((scx - 100)*-2.4, (scx - 100)*2.4, -(scy - 100)*1.24, (scy - 100)*.546);
					}
				}
			}
		}

		if (scx < 100)
			dx = 0;
		if (scy < 100)
			dy = 0;

		EVE_CoCmd_dlStart(s_pHalContext);
		EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1));
		{
			int imgid = subMenu.imageno + 20;

			if (imgid < 20 || imgid > 40) {
				return;
			}
			SAMAPP_Bitmap_Tile_t t = tile[imgid];
			EVE_CoCmd_setBitmap(s_pHalContext, FLASH_ADDRESS | t.addressFlash / 32, t.format, t.w, t.h);
		}

		EVE_Cmd_wr32(s_pHalContext, BEGIN(BITMAPS));
		EVE_Cmd_wr32(s_pHalContext, SAVE_CONTEXT());

		fsx = (float_t)(scx) / 100.0;
		fsy = (float_t)(scy) / 100.0;

		EVE_CoCmd_loadIdentity(s_pHalContext);
		EVE_CoCmd_translate(s_pHalContext, F16(240 - dx), F16(172 - dy));
		EVE_CoCmd_scale(s_pHalContext, F16(fsx), F16(fsy));
		EVE_CoCmd_rotate(s_pHalContext, -angle * 65536L / 360);
		EVE_CoCmd_translate(s_pHalContext, F16(-240), F16(-136));
		EVE_CoCmd_setMatrix(s_pHalContext);

		EVE_Cmd_wr32(s_pHalContext, BITMAP_HANDLE(0));
		EVE_Cmd_wr32(s_pHalContext, VERTEX2II(0, 0, 0, 0));
		EVE_Cmd_wr32(s_pHalContext, RESTORE_CONTEXT());

		switch (tap)
		{
		case FTGL_TTAP_ONETAP:
			if (tchs == FTGL_TPOINTS_ONE)
			{
				EVE_CoCmd_text(s_pHalContext, 5, 248, 28, 0, filename);
				EVE_CoCmd_text(s_pHalContext, 480, 248, 28, OPT_RIGHTX, "480x200px");
			}
			else if (tchs == FTGL_TPOINTS_TWO)
				EVE_CoCmd_text(s_pHalContext, 240, 136, 28, OPT_CENTER, "2 Touch Points are used for post processing");
			else if (roi)
				Image_ROI(tchs, min_x, max_x, min_y, max_y);
			break;

		case FTGL_TTAP_TWOTAP:
			if (!dtap)
			{
				dtap = 1;
				scx = scy = 400;
				FTGLSetScale(scx, scy);
				FTGLSetDragRange(-s_pHalContext->Width, s_pHalContext->Width, -s_pHalContext->Height, s_pHalContext->Height);
			}
			else
			{
				dtap = 0;
				scx = scy = 100;
				FTGLSetTransform(0, 100, 100, 0, 0);
				FTGLSetDragRange(0, 0, 0, 0);
			}
			dx = dy = 0;
			tap = 0;
			break;

		case FTGL_TTAP_LONGTAP:
			EVE_CoCmd_fgColor(s_pHalContext, 0x555555);
			if (tchs == FTGL_TPOINTS_ONE)
				Tab_Menu("Edit", "Share", "Copy", "Delete");
			else if (tchs == FTGL_TPOINTS_TWO)
				Tab_Menu("L8", "RGB332", "ARGB4", "ARGB2");
			else if (tchs == FTGL_TPOINTS_THREE)
				Tab_Menu("Message", "Mail", "Bluetooth", "AirDrop");
			else if (tchs == FTGL_TPOINTS_FOUR)
				Tab_Menu("Play", "Shuffle", "Repeat", "Back");
			else if (tchs == FTGL_TPOINTS_FIVE)
				return IMAGE_SELECTIONMENU;
			break;
		}

		EVE_Cmd_wr32(s_pHalContext, DISPLAY());
		EVE_CoCmd_swap(s_pHalContext);
		EVE_Cmd_waitFlush(s_pHalContext);

	}
}

void ImageViewer()
{
	unsigned char tchs = 0, animation_cts = 0, animation = 0;
	unsigned char tag = 0, tap = 0, fg = 0, temp_tag = 0;
	short zoom_out, zoom_inout;
	unsigned int gesture;
	short rangle = 0;
	int16_t x[5], y[5];
	int16_t i;
	int32_t scx;
	uint8_t row = 0;
	int16_t Ox = 0, Oy = 0, temp_Ox, temp_Oy, scrollerx = 0, temp_scroller = 0;

	// enable multi-touch
	EVE_Hal_wr32(s_pHalContext, REG_CTOUCH_EXTENDED, 0);

	EVE_CoCmd_dlStart(s_pHalContext);
	EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1));
	EVE_CoCmd_text(s_pHalContext, 240, 200, 29, OPT_CENTERX | OPT_CENTERY, "Loading...");
	EVE_CoCmd_spinner(s_pHalContext, 240, 136, OPT_CENTER, 0);
	EVE_Cmd_wr32(s_pHalContext, DISPLAY());
	EVE_CoCmd_swap(s_pHalContext);
	EVE_Cmd_waitFlush(s_pHalContext);
	Load_Thumbnails();
	mainScreen_Init();
	FTGLInit();
	FTGLSetScrollerMode(1, 0);
	FTGLSetScrollerRange(0, 2 * s_pHalContext->Height, 0, 0);
	while (1)
	{
		tag = EVE_Hal_rd8(s_pHalContext, REG_TOUCH_TAG);
		if (tag > 0 && tag <= 18)
			temp_tag = tag;

		readtouch(x, y);
		gesture = FTGLRun(x, y);

		if (gesture & FTGL_SCROLLERCHANGE)
		{
			if (!animation) {
				FTGLGetScrollerX(&scx);
				scrollerx = scx >> 4;
			}
		}
		else if (gesture & FTGL_TAPCHANGE)
		{
			FTGLGetTaptype(&tap, &tchs);
			if (tap == FTGL_TTAP_ONETAP)
			{
				if (temp_tag > 0 && temp_tag <= 18)
				{
					subMenu.imageno = temp_tag;
					temp_tag = 0;
					temp_scroller = scrollerx;
					do
					{
						animation = Imageviewer_submenu();
						if (animation == IMAGE_SELECTIONMENU)
						{
							subMenu.imageno = Image_Selector();
							temp_Ox = mainMenu.Ox + mainMenu.imw*((subMenu.imageno - 1) % 3) + mainMenu.dx*((subMenu.imageno - 1) % 3);
							temp_Ox += (s_pHalContext->Width*((subMenu.imageno - 1) / 6));
							if (temp_Ox > 480) temp_Ox = 480;
							if (!((subMenu.imageno - 1) % 3)) row ^= 1;
							temp_Oy = (row == 0) ? mainMenu.Oy : (mainMenu.imh + mainMenu.Oy + 2 * mainMenu.dy);
							temp_scroller = 0;
						}

					} while (animation == IMAGE_SELECTIONMENU);
					FTGLInit();
					FTGLSetScrollerMode(1, 0);
					FTGLSetScrollerRange(0, s_pHalContext->Width, 0, 0);
					FTGLSetScrollerXY(temp_scroller, 0);
					animation_cts = 0;
					fg = 0;
					rangle = 0;
				}
			}
		}

		EVE_CoCmd_dlStart(s_pHalContext);
		EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1));
		EVE_Cmd_wr32(s_pHalContext, LINE_WIDTH(2 * 16));
		EVE_Cmd_wr32(s_pHalContext, BEGIN(RECTS));
		EVE_Cmd_wr32(s_pHalContext, COLOR_A(0));
		EVE_Cmd_wr32(s_pHalContext, TAG(255));
		EVE_Cmd_wr32(s_pHalContext, VERTEX2F(0 * 16, 0 * 16));
		EVE_Cmd_wr32(s_pHalContext, VERTEX2F(480 * 16, 272 * 16));
		EVE_Cmd_wr32(s_pHalContext, TAG('X'));
		EVE_Cmd_wr32(s_pHalContext, VERTEX2F(0 * 16, 36 * 16));
		EVE_Cmd_wr32(s_pHalContext, VERTEX2F(480 * 16, 236 * 16));
		EVE_Cmd_wr32(s_pHalContext, COLOR_A(255));

		for (i = 0; i < MAX_IMAGES; i++)
		{
			Ox = mainMenu.Ox + mainMenu.imw*(i % 3) + mainMenu.dx*(i % 3);
			Ox += (s_pHalContext->Width*(i / 6));
			Ox -= scrollerx;
			if (!(i % 3)) row ^= 1;
			Oy = (row == 0) ? mainMenu.Oy : (mainMenu.imh + mainMenu.Oy + 2 * mainMenu.dy);
			if (Ox < 512)
			{
				EVE_Cmd_wr32(s_pHalContext, VERTEX2F(Ox * 16, Oy * 16));
				EVE_Cmd_wr32(s_pHalContext, VERTEX2F((Ox + mainMenu.imw) * 16, (Oy + mainMenu.imh) * 16));
			}
		}
		EVE_Cmd_wr32(s_pHalContext, BEGIN(BITMAPS));
		row = 1;
		for (i = 0; i < MAX_IMAGES; i++)
		{
			Ox = mainMenu.Ox + mainMenu.imw*(i % 3) + mainMenu.dx*(i % 3);
			Ox += (s_pHalContext->Width*(i / 6));
			Ox -= scrollerx;
			if (!(i % 3)) row ^= 1;
			Oy = (row == 0) ? mainMenu.Oy : (mainMenu.imh + mainMenu.Oy + 2 * mainMenu.dy);
			if (Ox < 512)
			{
				if ((subMenu.imageno - 1) != i)
					dxt1Image(Ox, Oy, 13, i * 2, i + 1);
			}
			if (tag == i + 1)
			{
				temp_Oy = Oy;
				temp_Ox = Ox;
			}
		}
		EVE_Cmd_wr32(s_pHalContext, RESTORE_CONTEXT());
		row = 1;
		
		if (animation == 1)
		{
			Ox = linear(0, temp_Ox, animation_cts, 200);
			Oy = linear(36, temp_Oy, animation_cts, 200);
			zoom_out = linear(256, 1024, animation_cts, 200);
			zoom_out = linear(256, 854, animation_cts, 200);
			EVE_Cmd_wr32(s_pHalContext, VERTEX2II(Ox, Oy, 0, 0));
			if (animation_cts < 200)    animation_cts += 10;
			else if (animation_cts >= 200) {
				subMenu.imageno = 0;
				animation = 0;
			}
		}
		else if (animation == 2)
		{
			if (!fg)
			{
				zoom_out = linear(256, 1024, animation_cts, 200);
				EVE_Cmd_wr32(s_pHalContext, BITMAP_TRANSFORM_A(zoom_out));
				zoom_out = linear(256, 854, animation_cts, 200);
				EVE_Cmd_wr32(s_pHalContext, BITMAP_TRANSFORM_E(zoom_out));
				Ox = linear(0, 360, animation_cts, 200);
				Oy = linear(36, 106, animation_cts, 200);
			}
			else
			{
				Ox = linear(360, temp_Ox, animation_cts, 200);
				Oy = linear(106, temp_Oy, animation_cts, 200);
				EVE_Cmd_wr32(s_pHalContext, BITMAP_TRANSFORM_A(1024));
				EVE_Cmd_wr32(s_pHalContext, BITMAP_TRANSFORM_E(854));
			}
			EVE_Cmd_wr32(s_pHalContext, VERTEX2II(Ox, Oy, 0, 0));
			if (animation_cts < 200)    animation_cts += 10;
			else if (animation_cts >= 200) {
				if (fg == 0)
				{
					fg = 1;
					animation_cts = 0;
				}
				else
				{
					animation = 0;
					subMenu.imageno = 0;
				}
			}
		}
		else if (animation == 3)
		{
			zoom_inout = 0;
			if (!fg)
			{
				Ox = linear(0, -720, animation_cts, 200);
				Oy = linear(36, -264, animation_cts, 200);
				zoom_inout = linear(256, 64, animation_cts, 200);
				EVE_Cmd_wr32(s_pHalContext, BITMAP_TRANSFORM_A(zoom_inout));
				zoom_inout = linear(256, 64, animation_cts, 200);
				EVE_Cmd_wr32(s_pHalContext, BITMAP_TRANSFORM_E(zoom_inout));
			}
			else
			{
				Ox = linear(-720, temp_Ox, animation_cts, 200);
				Oy = linear(-264, temp_Oy, animation_cts, 200);
				zoom_inout = linear(256, 1024, animation_cts, 200);
				EVE_Cmd_wr32(s_pHalContext, BITMAP_TRANSFORM_A(zoom_inout));
				zoom_inout = linear(256, 854, animation_cts, 200);
				EVE_Cmd_wr32(s_pHalContext, BITMAP_TRANSFORM_E(zoom_inout));
			}
			EVE_Cmd_wr32(s_pHalContext, BITMAP_HANDLE(0));
			EVE_Cmd_wr32(s_pHalContext, CELL(0));
			EVE_Cmd_wr32(s_pHalContext, VERTEX2F(Ox * 16, Oy * 16));
			if (animation_cts < 200)    animation_cts += 10; else if (animation_cts >= 200) {
				if (fg == 0)
				{
					fg = 1;
					animation_cts = 0;
				}
				else
				{
					animation = 0;
					subMenu.imageno = 0;
				}
			}
		}
		else if (animation == 4)
		{
			zoom_inout = 0;
			if (!fg) {
				rangle += 1; if (animation_cts > 100) fg = 1;
			}
			else if (fg) rangle -= 1;
			Ox = linear(0, temp_Ox, animation_cts, 200);
			Oy = linear(36, temp_Oy, animation_cts, 200);
			EVE_CoCmd_loadIdentity(s_pHalContext);
			EVE_CoCmd_translate(s_pHalContext, F16(Ox), F16(66));
			EVE_CoCmd_scale(s_pHalContext, F16(.25), F16(.303));
			EVE_CoCmd_rotate(s_pHalContext, rangle * 65536L / 360);
			EVE_CoCmd_translate(s_pHalContext, F16(-Ox), F16(-30));
			EVE_CoCmd_setMatrix(s_pHalContext);
			EVE_Cmd_wr32(s_pHalContext, BITMAP_HANDLE(0));
			EVE_Cmd_wr32(s_pHalContext, CELL(0));
			EVE_Cmd_wr32(s_pHalContext, VERTEX2F((Ox - 140) * 16, (Oy - 60) * 16));
			if (animation_cts < 200)    animation_cts += 5; else
				if (animation_cts >= 200) {
					subMenu.imageno = 0;
					animation = 0;
				}

		}
		else if (animation == 5)
		{
			zoom_inout = 0;
			if (!fg) {
				rangle += 1; if (animation_cts > 100) fg = 1;
			}
			else if (fg) rangle -= 1;
			Ox = linear(0, temp_Ox, animation_cts, 200);
			Oy = linear(36, temp_Oy, animation_cts, 200);
			EVE_CoCmd_loadIdentity(s_pHalContext);
			EVE_CoCmd_translate(s_pHalContext, F16(Ox), F16(66));
			EVE_CoCmd_scale(s_pHalContext, F16(.25), F16(.303));
			EVE_CoCmd_rotate(s_pHalContext, rangle * 65536L / 360);
			EVE_CoCmd_translate(s_pHalContext, F16(-Ox), F16(-30));
			EVE_CoCmd_setMatrix(s_pHalContext);
			EVE_Cmd_wr32(s_pHalContext, BITMAP_HANDLE(0));
			EVE_Cmd_wr32(s_pHalContext, CELL(0));
			EVE_Cmd_wr32(s_pHalContext, VERTEX2F((Ox - 140) * 16, (Oy - 60) * 16));
			if (animation_cts < 200)    animation_cts += 5; else
				if (animation_cts >= 200) {
					subMenu.imageno = 0;
					animation = 0;
				}

		}
		EVE_Cmd_wr32(s_pHalContext, DISPLAY());
		EVE_CoCmd_swap(s_pHalContext);
		EVE_Cmd_waitFlush(s_pHalContext);
	}
}

void DemoImageviewer2(EVE_HalContext* pHalContext) {
#if !defined(EVE_FLASH_AVAILABLE)
    #error Platform not supported
#endif
	s_pHalContext = pHalContext;
	Display_Start(s_pHalContext);
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0x80, 0x80, 0x00));
	EVE_CoCmd_text(s_pHalContext, (s_pHalContext->Width / 2), 100, 31, OPT_CENTERX, "The App_ImageViewer 2");

	FlashHelper_SwitchFullMode(s_pHalContext);

	Display_End(s_pHalContext);
	EVE_sleep(2000);

	ImageViewer();
}

