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

#if defined(FT9XX_PLATFORM)
#include "ff.h"
#endif

#define SQ(v) (v*v)
#define NOTOUCH		-32768

#define LOOPBACK_METHOD
//#define ANDROID_METHOD
//#define WIN8_METHOD

#if defined(FT80X_ENABLE)
#define RAM_G_END_ADDR (256*1024) //General purpose graphics RAM 256 kB
#elif defined(FT81X_ENABLE)
#define RAM_G_END_ADDR (1024*1024) //General purpose graphics RAM 1024 kB
#else
#error "Should select a GPU chip in Platform.h"
#endif
#define SIZE_HOME_START_ICON (460) 
#define SIZE_LOGO (6703)

#define START_ICON_ADDR (RAM_G_END_ADDR  - SIZE_HOME_START_ICON*10) //*6 to Reserve space for inflate images.
#define LOGO_ADDR       (START_ICON_ADDR - SIZE_LOGO)

#define START_ICON_HANDLE 14
#define LOGO_ICON_HANDLE 15

extern PROGMEM prog_uchar8_t home_start_icon[SIZE_HOME_START_ICON];

#define BACKGROUND_ANIMATION_1
//#define BACKGROUND_ANIMATION_2
//#define BACKGROUND_ANIMATION_3
//#define BACKGROUND_ANIMATION_4
//#define BACKGROUND_ANIMATION_5
//#define BACKGROUND_ANIMATION_6

int16_t xoffset_array[30], yoffset_array[30], dx_pts[20], dy_pts[20], rate_cts[30], iteration_cts[30];
uint16_t point_size[30];
uint8_t color[20], alpha_array[20];

#ifdef FT9XX_PLATFORM

unsigned long crc32file(FIL *Fil, unsigned int *charcnt);	// Calculate CRC32 for a file

#define UPDC32(octet,crc) (crc_32_tab[((crc) ^ ((unsigned char)octet)) & 0xff] ^ ((crc) >> 8))
static unsigned long crc_32_tab[] = { /* CRC polynomial 0xedb88320 */0x00000000, 0x77073096, 0xee0e612c, 0x990951ba, 0x076dc419, 0x706af48f, 0xe963a535, 0x9e6495a3, 0x0edb8832, 0x79dcb8a4,
0xe0d5e91e, 0x97d2d988, 0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91, 0x1db71064, 0x6ab020f2, 0xf3b97148,
0x84be41de, 0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7, 0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec,
0x14015c4f, 0x63066cd9, 0xfa0f3d63, 0x8d080df5, 0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172, 0x3c03e4d1,
0x4b04d447, 0xd20d85fd, 0xa50ab56b, 0x35b5a8fa, 0x42b2986c, 0xdbbbc9d6, 0xacbcf940, 0x32d86ce3, 0x45df5c75,
0xdcd60dcf, 0xabd13d59, 0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116, 0x21b4f4b5, 0x56b3c423, 0xcfba9599,
0xb8bda50f, 0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924, 0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d,
0x76dc4190, 0x01db7106, 0x98d220bc, 0xefd5102a, 0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433, 0x7807c9a2,
0x0f00f934, 0x9609a88e, 0xe10e9818, 0x7f6a0dbb, 0x086d3d2d, 0x91646c97, 0xe6635c01, 0x6b6b51f4, 0x1c6c6162,
0x856530d8, 0xf262004e, 0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457, 0x65b0d9c6, 0x12b7e950, 0x8bbeb8ea,
0xfcb9887c, 0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65, 0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2,
0x4adfa541, 0x3dd895d7, 0xa4d1c46d, 0xd3d6f4fb, 0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0, 0x44042d73,
0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9, 0x5005713c, 0x270241aa, 0xbe0b1010, 0xc90c2086, 0x5768b525, 0x206f85b3,
0xb966d409, 0xce61e49f, 0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4, 0x59b33d17, 0x2eb40d81, 0xb7bd5c3b,
0xc0ba6cad, 0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a, 0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683,
0xe3630b12, 0x94643b84, 0x0d6d6a3e, 0x7a6a5aa8, 0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1, 0xf00f9344,
0x8708a3d2, 0x1e01f268, 0x6906c2fe, 0xf762575d, 0x806567cb, 0x196c3671, 0x6e6b06e7, 0xfed41b76, 0x89d32be0,
0x10da7a5a, 0x67dd4acc, 0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5, 0xd6d6a3e8, 0xa1d1937e, 0x38d8c2c4,
0x4fdff252, 0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b, 0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60,
0xdf60efc3, 0xa867df55, 0x316e8eef, 0x4669be79, 0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236, 0xcc0c7795,
0xbb0b4703, 0x220216b9, 0x5505262f, 0xc5ba3bbe, 0xb2bd0b28, 0x2bb45a92, 0x5cb36a04, 0xc2d7ffa7, 0xb5d0cf31,
0x2cd99e8b, 0x5bdeae1d, 0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a, 0x9c0906a9, 0xeb0e363f, 0x72076785,
0x05005713, 0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38, 0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21,
0x86d3d2d4, 0xf1d4e242, 0x68ddb3f8, 0x1fda836e, 0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777, 0x88085ae6,
0xff0f6a70, 0x66063bca, 0x11010b5c, 0x8f659eff, 0xf862ae69, 0x616bffd3, 0x166ccf45, 0xa00ae278, 0xd70dd2ee,
0x4e048354, 0x3903b3c2, 0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db, 0xaed16a4a, 0xd9d65adc, 0x40df0b66,
0x37d83bf0, 0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9, 0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6,
0xbad03605, 0xcdd70693, 0x54de5729, 0x23d967bf, 0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94, 0xb40bbe37,
0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d };

unsigned long crc32file(FIL *Fil, unsigned int *charcnt) {
	unsigned long oldcrc32 = 0, crc32 = 0;
	unsigned char c;
	unsigned int numBytesRead;
	FRESULT fResult;

	oldcrc32 = 0xFFFFFFFF;
	*charcnt = 0;

	while (!f_eof(Fil)) {
		fResult = f_read(Fil, &c, 1, &numBytesRead);
		if ((fResult != FR_OK) || (numBytesRead == 0)) {
			f_close(Fil);
			*charcnt = -1;
			goto __exit;
		}

		++*charcnt;
		oldcrc32 = UPDC32(c, oldcrc32);
	}

	crc32 = ~oldcrc32;

__exit: return crc32;
}
DWORD get_fattime(void) {
	/* Returns current time packed into a DWORD variable */
	return 0;
}
#endif

#define MAX_MENUS 12
#define THUMBNAIL_ADDRESS (125*1024L)
#define MENU_POINTSIZE  5 // 16bit prec

uint16_t displayImageWidth, displayImageHeight;

#if defined(DISPLAY_RESOLUTION_WVGA)
char *apps[] = { TEST_DIR "\\1_H.jpg", TEST_DIR "\\2_H.jpg", TEST_DIR "\\3_H.jpg", TEST_DIR "\\4_H.jpg", TEST_DIR "\\5_H.jpg",
TEST_DIR "\\6_H.jpg", TEST_DIR "\\7_H.jpg", TEST_DIR "\\8_H.jpg", TEST_DIR "\\9_H.jpg", TEST_DIR "\\10_H.jpg",
TEST_DIR "\\11_H.jpg", TEST_DIR "\\12_H.jpg" };
#else
char *apps[] = { TEST_DIR "\\1.jpg", TEST_DIR "\\2.jpg", TEST_DIR "\\3.jpg", TEST_DIR "\\4.jpg", TEST_DIR "\\5.jpg", TEST_DIR "\\6.jpg",
TEST_DIR "\\7.jpg", TEST_DIR "\\8.jpg", TEST_DIR "\\9.jpg", TEST_DIR "\\10.jpg", TEST_DIR "\\11.jpg", TEST_DIR "\\12.jpg" };
#endif

void Load_Thumbnails() {
	uint8_t i, imn, fnf = 0;

	uint16_t fsize = 0, blklen = 10 * 1024, bytesRead, bufferChunkSz, wsize, imageWidth, imageHeight, imageStride,
		imageSize;
#if defined (MSVC_PLATFORM) || defined(BT8XXEMU_PLATFORM)
	FILE *ifile;
	uint8_t temp[2048];
	char8_t *path = TEST_DIR "\\";
#endif

#if defined(DISPLAY_RESOLUTION_WVGA)
	imageWidth = 167, imageHeight = 111, imageStride = 334, imageSize = 37074;
	displayImageWidth = 167, displayImageHeight = 111;
#else
	imageWidth = 100, imageHeight = 50, imageStride = 200, imageSize = 10000;
	displayImageWidth = 100, displayImageHeight = 50;
#endif

#ifdef  FT9XX_PLATFORM
	SDHOST_STATUS SDHostStatus;
	FATFS FatFs;				// FatFs work area needed for each volume
	FIL FilSrc;			// File object needed for each open file
	FIL FilDes;			// File object needed for each open file
	FRESULT fResult;			// Return value of FatFs APIs
	unsigned int numBytesWritten, numBytesRead;
	unsigned char buffer[32];
	unsigned long crcSrc, crcDes;

	FRESULT ifile;
	uint8_t temp[2048];
	imn = 0;

#endif

#if defined (MSVC_PLATFORM) || defined(BT8XXEMU_PLATFORM)
	imn = 0;
	bufferChunkSz = 1024;
	do
	{
		ifile = fopen(apps[imn], "rb");
		if (ifile == NULL)
		{
			fsize = 0;
			EVE_CoCmd_dlStart(s_pHalContext);        // start
			EVE_CoCmd_memSet(s_pHalContext, (imn*imageSize) + 50 * 1024l, random(10 * 1024L), imageSize);
			EVE_CoCmd_swap(s_pHalContext);
			EVE_Cmd_waitFlush(s_pHalContext);
		}
		else
		{
			fnf = 0;
			fseek(ifile, 0, SEEK_END);
			fsize = ftell(ifile);
			fseek(ifile, 0, SEEK_SET);
			EVE_Cmd_wr32(s_pHalContext, CMD_LOADIMAGE);
			EVE_Cmd_wr32(s_pHalContext, (imn*imageSize) + 50 * 1024L);
			EVE_Cmd_wr32(s_pHalContext, 0);
		}
		while (fsize > 0)
		{
			wsize = fsize > bufferChunkSz ? bufferChunkSz : fsize;
#if 0
			fread(temp, 1, wsize, ifile);
			fsize -= wsize;
			EVE_Cmd_wrMem(s_pHalContext, temp, wsize); /* copy data continuously into command memory */
#else
			bytesRead = fread(temp, 1, wsize, ifile);
#if 0
			if (fsize < bufferChunkSz && bytesRead > 0) {
				fsize = 0;
				bytesRead = ((bytesRead + 3)&~3); //the temp read buffer size has to be 4 bytes aligned
			}
			else {
				fsize -= bytesRead;
			}
#else
			fsize -= wsize;
#endif
			EVE_Cmd_wrMem(s_pHalContext, temp, bytesRead); /* copy data continuously into command memory */
#endif
		}
		imn++;
	} while (imn<12);
	/*Set the bitmap properties of the thumbnails*/
	App_Set_DlBuffer_Index(0);
	App_WrDl_Buffer(s_pHalContext, BITMAP_HANDLE(0));
	App_WrDl_Buffer(s_pHalContext, BITMAP_SOURCE(50 * 1024));
	App_WrDl_Buffer(s_pHalContext, BITMAP_LAYOUT(RGB565, imageStride, imageHeight));
#if defined(FT81X_ENABLE)
	App_WrDl_Buffer(s_pHalContext, BITMAP_LAYOUT_H(imageStride >> 10, imageHeight >> 9));
#endif
	App_WrDl_Buffer(s_pHalContext, BITMAP_SIZE(NEAREST, BORDER, BORDER, imageWidth, imageHeight));
#if defined(FT81X_ENABLE)
	App_WrDl_Buffer(s_pHalContext, BITMAP_SIZE_H(imageWidth >> 9, imageHeight >> 9));
#endif

	App_WrDl_Buffer(s_pHalContext, DISPLAY());
	App_Flush_DL_Buffer(s_pHalContext);
	GPU_DLSwap(s_pHalContext, DLSWAP_FRAME);
#endif

#ifdef FT9XX_PLATFORM
	imn = 0;
	bufferChunkSz = 1024;
	do {
		if (f_open(&FilSrc, apps[imn], FA_READ | FA_OPEN_EXISTING) != FR_OK) {
			fsize = 0;
			EVE_CoCmd_dlStart(s_pHalContext);        // start
			EVE_CoCmd_memSet(s_pHalContext, (imn * imageSize) + 50 * 1024l, random(10 * 1024L), 200 * 50);
			EVE_Cmd_wr32(s_pHalContext, DISPLAY());
			EVE_CoCmd_swap(s_pHalContext);
			EVE_Cmd_waitFlush(s_pHalContext);

		}
		else {
			fnf = 0;
			printf("opened filename:%s\n", apps[imn]);

			fsize = f_size(&FilSrc);

			printf("  Fsize-%d \n", fsize);

			EVE_Cmd_wr32(s_pHalContext, CMD_LOADIMAGE);
			EVE_Cmd_wr32(s_pHalContext, (imn * imageSize) + 50 * 1024l);
			EVE_Cmd_wr32(s_pHalContext, 0);
		}
		while (fsize > 0) {
			wsize = fsize > bufferChunkSz ? bufferChunkSz : fsize;
			f_read(&FilSrc, temp, wsize, &numBytesRead);
			if (fsize < bufferChunkSz && numBytesRead > 0) {
				fsize = 0;
				numBytesRead = ((numBytesRead + 3) & ~3);
				printf("Read: %d , Remaining: %d\n", numBytesRead, fsize);
			}
			else {
				fsize -= numBytesRead;
				printf("Read: %d , Remaining: %d\n", numBytesRead, fsize);
			}
			EVE_Cmd_wrMem(s_pHalContext, temp, numBytesRead);
			/* copy data continuously into command memory */
		}
		imn++;
		f_close(&FilSrc);
	} while (imn < 12);
	/*Set the bitmap properties of the thumbnails*/
	App_Set_DlBuffer_Index(0);
	App_WrDl_Buffer(s_pHalContext, BITMAP_HANDLE(0));
	App_WrDl_Buffer(s_pHalContext, BITMAP_SOURCE(50 * 1024));
	App_WrDl_Buffer(s_pHalContext, BITMAP_LAYOUT(RGB565, imageStride, imageHeight));
#if defined(FT81X_ENABLE)
	App_WrDl_Buffer(s_pHalContext, BITMAP_LAYOUT_H(imageStride >> 10, imageHeight >> 9));
#endif
	App_WrDl_Buffer(s_pHalContext, BITMAP_SIZE(NEAREST, BORDER, BORDER, imageWidth, imageHeight));
#if defined(FT81X_ENABLE)
	App_WrDl_Buffer(s_pHalContext, BITMAP_SIZE_H(imageWidth >> 9, imageHeight >> 9));
#endif
	App_WrDl_Buffer(s_pHalContext, DISPLAY());
	App_Flush_DL_Buffer(s_pHalContext);
	GPU_DLSwap(s_pHalContext, DLSWAP_FRAME);

#endif
	/* Load Home button */

	EVE_Cmd_wr32(s_pHalContext, CMD_INFLATE);
	EVE_Cmd_wr32(s_pHalContext, START_ICON_ADDR);
#if defined(MSVC_PLATFORM) || defined(BT8XXEMU_PLATFORM)
	EVE_Cmd_wrMem(s_pHalContext, home_start_icon, sizeof(home_start_icon)); //Load from RAM
#else
	EVE_Cmd_wrProgMem(s_pHalContext, home_start_icon, sizeof(home_start_icon)); //Load from Flash
#endif
}

static struct {
	signed short dragprev;
	int vel;      // velocity
	long base;    // screen x coordinate, in 1/16ths pixel
	long limit;
} scroller;

static void scroller_init(uint32_t limit) {
	scroller.dragprev = -32768;
	scroller.vel = 0;      // velocity
	scroller.base = 0;     // screen x coordinate, in 1/16ths pixel
	scroller.limit = limit;
}

static void scroller_run() {
	signed short sx;
	static uint16_t _delay = 0;
#ifdef FT801_ENABLE  
	static uint32_t prev_time = 0;
	uint32_t time = GetTickCount();
	if (prev_time != 0)
		_delay += (time - prev_time);
	prev_time = time;
	if (_delay<30)
	{
		scroller.base += scroller.vel;
		scroller.base = MAX(0, MIN(scroller.base, scroller.limit));
		return;
	}
#endif

	sx = EVE_Hal_rd16(s_pHalContext, REG_TOUCH_SCREEN_XY + 2);
	if ((sx != -32768) & (scroller.dragprev != -32768)) {
		scroller.vel = (scroller.dragprev - sx) << 4;
	}
	else {
		int change = MAX(1, abs(scroller.vel) >> 5);
		if (scroller.vel < 0)
			scroller.vel += change;
		if (scroller.vel > 0)
			scroller.vel -= change;
	}
	scroller.dragprev = sx;
	scroller.base += scroller.vel;
	scroller.base = MAX(0, MIN(scroller.base, scroller.limit));
	_delay = 0;
}

/********API to return the assigned TAG value when penup,for the primitives/widgets******/
uint8_t keypressed = 0;
uint8_t keyin_cts = 0;
#define KEYIN_COUNTS	10

float_t linear(float_t p1, float_t p2, float_t t, uint16_t rate) {
	float_t st = (float_t)t / rate;
	return p1 + (st * (p2 - p1));
}

uint16_t smoothstep(float_t p1, float_t p2, float_t t, uint16_t rate) {
	float_t dst = (float_t)t / rate;
	float_t st = SQ(dst) * (3 - 2 * dst);
	return p1 + (st * (p2 - p1));
}

float_t acceleration(float_t p1, float_t p2, uint16_t t, uint16_t rate) {
	float_t dst = (float_t)t / rate;
	float_t st = SQ(dst);
	return p1 + (st * (p2 - p1));
}

float_t deceleration(float_t p1, float_t p2, uint16_t t, uint16_t rate) {
	float_t st, dst = (float_t)t / rate;
	dst = 1 - dst;
	st = 1 - SQ(dst);
	return p1 + (st * (p2 - p1));
}

static uint8_t istouch() {
	return !(EVE_Hal_rd16(s_pHalContext, REG_TOUCH_RAW_XY) & 0x8000);
}

int32_t App_LoadRawFromFile(char8_t *pFileName, uint32_t DstAddr) {
	int32_t FileLen = 0, Ft800_addr = RAM_G;
	uint8_t *pbuff = NULL;
#ifdef FT9XX_PLATFORM
	FIL FilSrc;

	unsigned int numBytesRead;
#endif
#if  defined(MSVC_PLATFORM) || defined(BT8XXEMU_PLATFORM)
	FILE *pFile = fopen(pFileName, "rb");     //TBD - make platform specific
#endif

#ifdef FT9XX_PLATFORM

	if (f_open(&FilSrc, pFileName, FA_READ) != FR_OK)
		printf("Error in opening file");
	else {
#endif

#if  defined(MSVC_PLATFORM) || defined(BT8XXEMU_PLATFORM) || defined(FT900_Platform)

		/* inflate the data read from binary file */
		if (NULL == pFile)
		{
			printf("Error in opening file %s \n", pFileName);
		}
		else
		{
#endif
			Ft800_addr = DstAddr;

#if  defined(MSVC_PLATFORM) || defined(BT8XXEMU_PLATFORM) 

			fseek(pFile, 0, SEEK_END);
			pFile = fopen(pFileName, "rb");     //TBD - make platform specific
			fseek(pFile, 0, SEEK_END);
			FileLen = ftell(pFile);
			fseek(pFile, 0, SEEK_SET);
			pbuff = (uint8_t *)malloc(8192);
			while (FileLen > 0)
			{
				/* download the data into the command buffer by 2kb one shot */
				uint16_t blocklen = FileLen>8192 ? 8192 : FileLen;
				/* copy the data into pbuff and then transfter it to command buffer */
				fread(pbuff, 1, blocklen, pFile);
				FileLen -= blocklen;
				/* copy data continuously into FT800 memory */
				EVE_Hal_wrMem(s_pHalContext, Ft800_addr, pbuff, blocklen);
				Ft800_addr += blocklen;
			}
			/* close the opened binary zlib file */
			fclose(pFile);
			free(pbuff);
		}
#endif

#ifdef FT9XX_PLATFORM

		FileLen = f_size(&FilSrc);
		pbuff = (uint8_t *)malloc(8192);
		while (FileLen > 0) {
			/* download the data into the command buffer by 2kb one shot */
			uint16_t blocklen = FileLen > 8192 ? 8192 : FileLen;

			/* copy the data into pbuff and then transfter it to command buffer */
			f_read(&FilSrc, pbuff, blocklen, &numBytesRead);
			FileLen -= blocklen;
			/* copy data continuously into FT800 memory */
			EVE_Hal_wrMem(s_pHalContext, Ft800_addr, pbuff, blocklen);
			Ft800_addr += blocklen;
		}
		/* close the opened binary zlib file */
		f_close(&FilSrc);
		free(pbuff);
	}
#endif
	return 0;
}

static void polar_draw(int32_t r, float_t th, uint16_t ox, uint16_t oy) {
	int32_t x, y;
	th = (th * 32768L / 180);
	Math_Polarxy(r, th, &x, &y, ox, oy);

#if defined(FT81X_ENABLE)
	EVE_Cmd_wr32(s_pHalContext, VERTEX_FORMAT(0));
	EVE_Cmd_wr32(s_pHalContext, VERTEX2F(x >> 4, y >> 4));
	EVE_Cmd_wr32(s_pHalContext, VERTEX_FORMAT(4));
#else
	EVE_Cmd_wr32(s_pHalContext, VERTEX2F(x, y));
#endif
}

void Sine_wave(uint8_t amp, uint16_t address, uint16_t yoffset) {
	uint32_t x = 0, y = 0;

	for (x = 0; x < s_pHalContext->Width + 10; x += 10) {
		y = (yoffset)+((int32_t) amp * Math_Qsin(-65536 * x / (25 * 10)) / 65536);
		EVE_Hal_wr32(s_pHalContext, address + (x / 10) * 4, VERTEX2F(x * 16, y * 16));
	}
}

#define rotation_rate 2
void Backgroundanimation_1() {
#ifdef DISPLAY_RESOLUTION_WVGA
	static uint16_t bg_cts = 0, fg = 0, wave_cts = 800 + 10;
#else
	static uint16_t bg_cts = 0, fg = 0, wave_cts = 490;
#endif
	static uint8_t init = 0, cell = 0, _cell = 0, VEL;
	static uint8_t bitmap_handle[30], cts = 0;
	uint16_t linestripAddress = 0;
	int16_t i = 0, j = 0, xoff, yoff;
	float_t _xoff = 0;

	if (cts >= 5) {
		cts = 0;
		if (_cell > 0)
			_cell--;
		else
			_cell = 7;
	}
	cts++;
	_cell = 0;
	if (wave_cts < s_pHalContext->Width + 10)
		wave_cts += 10;
	else
		wave_cts = 0;
	if (istouch())
		VEL = 2;
	else
		VEL = 1;

	if (!init) {
		init = 1;
		linestripAddress = ((s_pHalContext->Width + 10) / 10) * 4;
		Sine_wave(15, RAM_G, s_pHalContext->Height / 2);
		Sine_wave(12, RAM_G + linestripAddress, 16 + (s_pHalContext->Height / 2));
		Sine_wave(9, RAM_G + 2 * linestripAddress, 32 + (s_pHalContext->Height / 2));
		Sine_wave(6, RAM_G + 3 * linestripAddress, 48 + (s_pHalContext->Height / 2));
		for (i = 0; i < 30; i++) {
			yoffset_array[i] = random(s_pHalContext->Height);
			bitmap_handle[i] = 4 + random(4);
			rate_cts[i] = 300 + random(200);
			iteration_cts[i] = random(200);
		}
#if defined(FT81X_ENABLE) && defined(DISPLAY_RESOLUTION_WVGA)
		Gpu_Hal_LoadImageToMemory(s_pHalContext, TEST_DIR "\\nts1.raw", 820 * 1024L, LOAD);
		Gpu_Hal_LoadImageToMemory(s_pHalContext, TEST_DIR "\\nts2.raw", 822 * 1024L, LOAD);
		Gpu_Hal_LoadImageToMemory(s_pHalContext, TEST_DIR "\\nts3.raw", 832 * 1024, LOAD);
		Gpu_Hal_LoadImageToMemory(s_pHalContext, TEST_DIR "\\nts4.raw", 842 * 1024, LOAD);
		Gpu_Hal_LoadImageToMemory(s_pHalContext, TEST_DIR "\\hline_H.raw", 855 * 1024L, LOAD);
		EVE_Cmd_wr32(s_pHalContext, BITMAP_HANDLE(4));
		EVE_Cmd_wr32(s_pHalContext, BITMAP_SOURCE(820 * 1024L));
		EVE_Cmd_wr32(s_pHalContext, BITMAP_LAYOUT(L4, 10, 50));
		EVE_Cmd_wr32(s_pHalContext, BITMAP_SIZE(NEAREST, BORDER, BORDER, 40, 100));
		EVE_Cmd_wr32(s_pHalContext, BITMAP_HANDLE(5));
		EVE_Cmd_wr32(s_pHalContext, BITMAP_SOURCE(822 * 1024L));
		EVE_Cmd_wr32(s_pHalContext, BITMAP_LAYOUT(L4, 25, 60));
		EVE_Cmd_wr32(s_pHalContext, BITMAP_SIZE(NEAREST, BORDER, BORDER, 100, 120));
		EVE_Cmd_wr32(s_pHalContext, BITMAP_HANDLE(6));
		EVE_Cmd_wr32(s_pHalContext, BITMAP_SOURCE(832 * 1024L));
		EVE_Cmd_wr32(s_pHalContext, BITMAP_LAYOUT(L4, 10, 40));
		EVE_Cmd_wr32(s_pHalContext, BITMAP_SIZE(NEAREST, BORDER, BORDER, 40, 80));
		EVE_Cmd_wr32(s_pHalContext, BITMAP_HANDLE(7));
		EVE_Cmd_wr32(s_pHalContext, BITMAP_SOURCE(842 * 1024L));
		EVE_Cmd_wr32(s_pHalContext, BITMAP_LAYOUT(L4, 10, 24));
		EVE_Cmd_wr32(s_pHalContext, BITMAP_SIZE(NEAREST, BORDER, BORDER, 40, 48));
		EVE_Cmd_wr32(s_pHalContext, BITMAP_HANDLE(8));
		EVE_Cmd_wr32(s_pHalContext, BITMAP_SOURCE(855 * 1024L));
		EVE_Cmd_wr32(s_pHalContext, BITMAP_LAYOUT(L8, 800, 1));
		EVE_Cmd_wr32(s_pHalContext, BITMAP_SIZE(NEAREST, BORDER, REPEAT, 800, 600));
#if defined(FT81X_ENABLE)
		EVE_Cmd_wr32(s_pHalContext, BITMAP_SIZE_H(800 >> 9, 600 >> 9));
#endif
#else
		Gpu_Hal_LoadImageToMemory(s_pHalContext, TEST_DIR "\\nts1.raw", 220 * 1024L, LOAD);
		Gpu_Hal_LoadImageToMemory(s_pHalContext, TEST_DIR "\\nts2.raw", 222 * 1024L, LOAD);
		Gpu_Hal_LoadImageToMemory(s_pHalContext, TEST_DIR "\\nts3.raw", 232 * 1024, LOAD);
		Gpu_Hal_LoadImageToMemory(s_pHalContext, TEST_DIR "\\nts4.raw", 242 * 1024, LOAD);
		Gpu_Hal_LoadImageToMemory(s_pHalContext, TEST_DIR "\\hline.raw", 255 * 1024L, LOAD);
		EVE_Cmd_wr32(s_pHalContext, BITMAP_HANDLE(4));
		EVE_Cmd_wr32(s_pHalContext, BITMAP_SOURCE(220 * 1024L));
		EVE_Cmd_wr32(s_pHalContext, BITMAP_LAYOUT(L4, 10, 50));
		EVE_Cmd_wr32(s_pHalContext, BITMAP_SIZE(NEAREST, BORDER, BORDER, 40, 100));
		EVE_Cmd_wr32(s_pHalContext, BITMAP_HANDLE(5));
		EVE_Cmd_wr32(s_pHalContext, BITMAP_SOURCE(222 * 1024L));
		EVE_Cmd_wr32(s_pHalContext, BITMAP_LAYOUT(L4, 25, 60));
		EVE_Cmd_wr32(s_pHalContext, BITMAP_SIZE(NEAREST, BORDER, BORDER, 100, 120));
		EVE_Cmd_wr32(s_pHalContext, BITMAP_HANDLE(6));
		EVE_Cmd_wr32(s_pHalContext, BITMAP_SOURCE(232 * 1024L));
		EVE_Cmd_wr32(s_pHalContext, BITMAP_LAYOUT(L4, 10, 40));
		EVE_Cmd_wr32(s_pHalContext, BITMAP_SIZE(NEAREST, BORDER, BORDER, 40, 80));
		EVE_Cmd_wr32(s_pHalContext, BITMAP_HANDLE(7));
		EVE_Cmd_wr32(s_pHalContext, BITMAP_SOURCE(242 * 1024L));
		EVE_Cmd_wr32(s_pHalContext, BITMAP_LAYOUT(L4, 10, 24));
		EVE_Cmd_wr32(s_pHalContext, BITMAP_SIZE(NEAREST, BORDER, BORDER, 40, 48));
		EVE_Cmd_wr32(s_pHalContext, BITMAP_HANDLE(8));
		EVE_Cmd_wr32(s_pHalContext, BITMAP_SOURCE(255 * 1024L));
		EVE_Cmd_wr32(s_pHalContext, BITMAP_LAYOUT(L8, 512, 1));
		EVE_Cmd_wr32(s_pHalContext, BITMAP_SIZE(NEAREST, REPEAT, REPEAT, 512, 480));
#endif
	}

	EVE_Cmd_wr32(s_pHalContext, CLEAR_COLOR_RGB(30, 30, 30));
	EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1));
	EVE_Cmd_wr32(s_pHalContext, COLOR_A(170));
	EVE_Cmd_wr32(s_pHalContext, SAVE_CONTEXT());

	EVE_Cmd_wr32(s_pHalContext, BEGIN(BITMAPS));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_HANDLE(8));
	EVE_Cmd_wr32(s_pHalContext, VERTEX2F((int16_t)0, 0));
	EVE_Cmd_wr32(s_pHalContext, RESTORE_CONTEXT());
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(30, 30, 30));
	EVE_Cmd_wr32(s_pHalContext, COLOR_A(255));

	for (j = 0; j < 3; j++) {
		switch (j) {
		case 0:

			EVE_Cmd_wr32(s_pHalContext, BITMAP_TRANSFORM_A(384));
			EVE_Cmd_wr32(s_pHalContext, BITMAP_TRANSFORM_E(384));
			break;

		case 1:

			EVE_Cmd_wr32(s_pHalContext, BITMAP_TRANSFORM_A(256));
			EVE_Cmd_wr32(s_pHalContext, BITMAP_TRANSFORM_E(256));
			break;

		case 2:

			EVE_Cmd_wr32(s_pHalContext, BITMAP_TRANSFORM_A(200));
			EVE_Cmd_wr32(s_pHalContext, BITMAP_TRANSFORM_E(200));
			break;
		}
		for (i = 0; i < 10; i++) {
			EVE_Cmd_wr32(s_pHalContext, BITMAP_HANDLE(bitmap_handle[j * 10 + i]));
			if (bitmap_handle[j * 10 + i] == 5)
				EVE_Cmd_wr32(s_pHalContext, CELL(_cell));
			else {
				switch (j) {
				case 0:
					EVE_Cmd_wr32(s_pHalContext, CELL(0));

					break;

				case 1:
					EVE_Cmd_wr32(s_pHalContext, CELL(0));

					break;

				case 2:
					EVE_Cmd_wr32(s_pHalContext, CELL(1));
					break;
				}
			}
			xoff = linear(s_pHalContext->Width, -50, iteration_cts[j * 10 + i], rate_cts[j * 10 + i]);
			yoff = linear(s_pHalContext->Height / 2, yoffset_array[j * 10 + i], iteration_cts[j * 10 + i], rate_cts[j * 10 + i]);
			EVE_Cmd_wr32(s_pHalContext, VERTEX2F(xoff * 16, yoff * 16));
		}

	}
	for (i = 0; i < 30; i++) {
		if (iteration_cts[i] == 0) {
			yoffset_array[i] = random(s_pHalContext->Height);
			bitmap_handle[i] = 4 + random(4);
		}
		if (iteration_cts[i] < rate_cts[i])
			iteration_cts[i] += VEL;
		else {
			iteration_cts[i] = 0;
		}
	}
	wave_cts = s_pHalContext->Width + 10;
	linestripAddress = (wave_cts / 10) * 4;
	EVE_Cmd_wr32(s_pHalContext, LINE_WIDTH(1 * 16));
	EVE_Cmd_wr32(s_pHalContext, COLOR_A(255));

	EVE_Cmd_wr32(s_pHalContext, BEGIN(LINE_STRIP));
	EVE_CoCmd_append(s_pHalContext, RAM_G, linestripAddress);
	EVE_Cmd_wr32(s_pHalContext, END());

	EVE_Cmd_wr32(s_pHalContext, BEGIN(LINE_STRIP));
	EVE_CoCmd_append(s_pHalContext, RAM_G + linestripAddress, linestripAddress);
	EVE_Cmd_wr32(s_pHalContext, END());

	EVE_Cmd_wr32(s_pHalContext, BEGIN(LINE_STRIP));
	EVE_CoCmd_append(s_pHalContext, RAM_G + 2 * linestripAddress, linestripAddress);
	EVE_Cmd_wr32(s_pHalContext, END());

	EVE_Cmd_wr32(s_pHalContext, BEGIN(LINE_STRIP));
	EVE_CoCmd_append(s_pHalContext, RAM_G + 3 * linestripAddress, linestripAddress);
	EVE_Cmd_wr32(s_pHalContext, END());

	EVE_Cmd_wr32(s_pHalContext, RESTORE_CONTEXT());
}

void Backgroundanimation_2() {
	static uint8_t init = 0;
	uint8_t ptclrarray[6][3] = { { 0xb9, 0xba, 0xde },{ 0x0c, 0x61, 0xb7 },{ 0x01, 0x18, 0x4e },{ 0xbf, 0x25, 0xbd },
	{ 0x29, 0x07, 0x3a },{ 0xc9, 0x61, 0x22 } };
	int32_t i, ptradius, colorindex;
	static uint8_t fg = 0;
	float_t vel = 2;
	static float_t t = 0, t1 = 0;
	int32_t xoffset, yoffset, x1offset, y1offset, rate = 2000;
	uint8_t pts;
	if (!init) {
		init = 1;
		for (pts = 0; pts < 20; pts++) {
#if defined(DISPLAY_RESOLUTION_WVGA)
			point_size[pts] = 450 * 16 + random(61 * 16);
			xoffset_array[pts] = random(1024) * 16;
			yoffset_array[pts] = random(1024) * 16;
			color[pts] = random(5);
			dx_pts[pts] = 240 * 16 + random(240 * 16);
			dy_pts[pts] = 130 * 16 + random(142 * 16);
#else
			point_size[pts] = 136 * 16 + random(375 * 16);
			xoffset_array[pts] = random(512) * 16;
			yoffset_array[pts] = random(512) * 16;
			color[pts] = random(5);
			dx_pts[pts] = 240 * 16 + random(240 * 16);
			dy_pts[pts] = 130 * 16 + random(142 * 16);
#endif
		}
	}
	if (istouch()) {
		t1 = 0;
		vel = 5;
	}
	else {
		vel = linear(5, 2, t1, 100);
		if (t1 < 100)
			t1++;
	}
	EVE_Cmd_wr32(s_pHalContext, SAVE_CONTEXT());
	EVE_Cmd_wr32(s_pHalContext, COLOR_MASK(1, 1, 1, 1));
	EVE_Cmd_wr32(s_pHalContext, COLOR_A(255));

	//draw 20 points with various radius and additive blending
	EVE_Cmd_wr32(s_pHalContext, BEGIN(FTPOINTS));
	EVE_Cmd_wr32(s_pHalContext, BLEND_FUNC(SRC_ALPHA, ONE));
	EVE_Cmd_wr32(s_pHalContext, COLOR_MASK(1, 1, 1, 0));
	EVE_Cmd_wr32(s_pHalContext, COLOR_A(50));

	/* compute points on top */
	for (i = 0; i < 5; i++) {
		ptradius = point_size[i];
		colorindex = color[i + 0];
		EVE_Cmd_wr32(s_pHalContext,
			COLOR_RGB(ptclrarray[colorindex][0], ptclrarray[colorindex][1], ptclrarray[colorindex][2]));
		yoffset = linear(dy_pts[i + 0], yoffset_array[i + 0], t, 1000);

		xoffset = xoffset_array[i + 0];
		EVE_Cmd_wr32(s_pHalContext, POINT_SIZE(ptradius));
		EVE_Cmd_wr32(s_pHalContext, VERTEX2F(xoffset, yoffset));
	}
	/* compute points on right */
	for (i = 0; i < 5; i++) {
		ptradius = point_size[5 + i];
		colorindex = color[i + 5];
		EVE_Cmd_wr32(s_pHalContext,
			COLOR_RGB(ptclrarray[colorindex][0], ptclrarray[colorindex][1], ptclrarray[colorindex][2]));
		yoffset = linear(dy_pts[i + 5], yoffset_array[i + 5], t, 1000);

		yoffset = yoffset_array[i + 5];
		EVE_Cmd_wr32(s_pHalContext, POINT_SIZE(ptradius));
		EVE_Cmd_wr32(s_pHalContext, VERTEX2F(xoffset, yoffset));
	}
	///* compute points on left */
	for (i = 0; i < 5; i++) {
		ptradius = point_size[10 + i];
		colorindex = color[i + 10];

		EVE_Cmd_wr32(s_pHalContext,
			COLOR_RGB(ptclrarray[colorindex][0], ptclrarray[colorindex][1], ptclrarray[colorindex][2]));
		xoffset = linear(dx_pts[i + 0], xoffset_array[i + 0], t, 1000);

		yoffset = yoffset_array[i + 5];
		EVE_Cmd_wr32(s_pHalContext, POINT_SIZE(ptradius));
		EVE_Cmd_wr32(s_pHalContext, VERTEX2F(xoffset, yoffset));
	}
	/* compute points on bottom */
	for (i = 0; i < 5; i++) {
		ptradius = point_size[15 + i];
		colorindex = color[i + 15];
		EVE_Cmd_wr32(s_pHalContext,
			COLOR_RGB(ptclrarray[colorindex][0], ptclrarray[colorindex][1], ptclrarray[colorindex][2]));
		yoffset = linear(dy_pts[i + 15], yoffset_array[i + 15], t, 1000);
		xoffset = xoffset_array[i + 15];
		EVE_Cmd_wr32(s_pHalContext, POINT_SIZE(ptradius));
		EVE_Cmd_wr32(s_pHalContext, VERTEX2F(xoffset, yoffset));
	}

	//draw additive blend lines diagonally
	EVE_Cmd_wr32(s_pHalContext, COLOR_A(100));
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(32, 32, 32));
	EVE_Cmd_wr32(s_pHalContext, BEGIN(LINES));
#if defined(DISPLAY_RESOLUTION_WVGA)
	for (i = -1; i < 6; i++)
#else
	for (i = -1; i<4; i++)
#endif
	{
		xoffset = i * 136;
		yoffset = 0;
		x1offset = 136 * (2 + i);
		y1offset = s_pHalContext->Height;
		if (x1offset > s_pHalContext->Width) {
			y1offset = s_pHalContext->Height - (x1offset - s_pHalContext->Width);
			x1offset = s_pHalContext->Width;
		}
		if (xoffset < 0) {
			yoffset = -xoffset;
			xoffset = 0;
		}
		EVE_Cmd_wr32(s_pHalContext, VERTEX2F(xoffset * 16, yoffset * 16));
		EVE_Cmd_wr32(s_pHalContext, VERTEX2F(x1offset * 16, y1offset * 16));
	}
#if defined(DISPLAY_RESOLUTION_WVGA)
	for (i = 1; i < 8; i++)
#else
	for (i = 1; i<6; i++)
#endif
	{
		xoffset = i * 136;
		yoffset = 0;
		x1offset = 136 * (i - 2);
		y1offset = s_pHalContext->Height;
		if (x1offset < 0) {
			y1offset = s_pHalContext->Height + x1offset;
			x1offset = 0;
		}
		if (xoffset > s_pHalContext->Width) {
			yoffset = (xoffset - s_pHalContext->Width);
			xoffset = s_pHalContext->Width;
		}
		EVE_Cmd_wr32(s_pHalContext, VERTEX2F(xoffset * 16, yoffset * 16));
		EVE_Cmd_wr32(s_pHalContext, VERTEX2F(x1offset * 16, y1offset * 16));
	}
	if (!fg) {
		if (t < rate)
			t += vel;
		else
			fg = 1;
	}
	else {
		if (t > 0)
			t -= vel;
		else
			fg = 0;
	}
	EVE_Cmd_wr32(s_pHalContext, RESTORE_CONTEXT());
	return;
}

void Backgroundanimation_3() {
	int32_t i = 0, rate = 1000;
	uint8_t alpha = 0, inc = 0;
	static uint16_t ctime = 0, t1 = 0;
	static uint8_t ptsapp = 19;
	int16_t xoff = 0, yoff = 0;
	if (istouch()) {
		t1 = 0;
		inc = 10;
	}
	else {
		inc = linear(10, 2, t1, 100);
		if (t1 < 100)
			t1++;
	}
	//clear the background color
	EVE_Cmd_wr32(s_pHalContext, SAVE_CONTEXT());
	EVE_Cmd_wr32(s_pHalContext, CLEAR_COLOR_RGB(0, 0, 0));
	EVE_Cmd_wr32(s_pHalContext, CLEAR_COLOR_A(0));
	EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1));
	EVE_Cmd_wr32(s_pHalContext, COLOR_MASK(1, 1, 1, 0));
	//draw the cmd gradient with scissors
	EVE_Cmd_wr32(s_pHalContext, SCISSOR_SIZE(s_pHalContext->Width, s_pHalContext->Height));
	EVE_Cmd_wr32(s_pHalContext, SCISSOR_XY(0, 0));
	EVE_CoCmd_gradient(s_pHalContext, 0, 0, 0x708fa1, 0, s_pHalContext->Height / 2, 0xc4cdd2);
	EVE_Cmd_wr32(s_pHalContext, SCISSOR_XY(0, s_pHalContext->Height / 2));
	EVE_CoCmd_gradient(s_pHalContext, 0, s_pHalContext->Height / 2, 0xc4cdd2, 0, s_pHalContext->Height, 0x4f7588);

	EVE_Cmd_wr32(s_pHalContext, SCISSOR_XY(0, 0));
	EVE_Cmd_wr32(s_pHalContext, SCISSOR_SIZE(s_pHalContext->Width, s_pHalContext->Height));	//reprogram with  default values

    //draw 20 points with various radious with additive blending
	EVE_Cmd_wr32(s_pHalContext, BEGIN(FTPOINTS));
	EVE_Cmd_wr32(s_pHalContext, COLOR_MASK(1, 1, 1, 0));
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 255, 255));
	for (i = 0; i < 20; i++) {
		alpha = linear(80, 0, iteration_cts[i], rate_cts[i]);
		if (alpha < 75) {
#if defined(DISPLAY_RESOLUTION_WVGA)
			EVE_Cmd_wr32(s_pHalContext, POINT_SIZE(16 * (60 + (3 * i / 2))));
#else
			EVE_Cmd_wr32(s_pHalContext, POINT_SIZE(16 * (30 + (3 * i / 2))));
#endif
			EVE_Cmd_wr32(s_pHalContext, COLOR_A(alpha));
			xoff = linear(xoffset_array[i], dx_pts[i], iteration_cts[i], rate_cts[i]);
			yoff = linear(yoffset_array[i], dy_pts[i], iteration_cts[i], rate_cts[i]);
			EVE_Cmd_wr32(s_pHalContext, VERTEX2F(xoff * 16, yoff * 16));
		}
	}

	EVE_Cmd_wr32(s_pHalContext, RESTORE_CONTEXT());
	for (i = 0; i < 20; i++) {
		if (iteration_cts[i] == 0) {
			xoffset_array[i] = random(s_pHalContext->Width);
			yoffset_array[i] = 100 + random(s_pHalContext->Height / 4);
			dx_pts[i] = random(s_pHalContext->Width);
			dy_pts[i] = random(s_pHalContext->Height);
			rate_cts[i] = 500 + random(500);
		}
		if (iteration_cts[i] < rate_cts[i])
			iteration_cts[i] += inc;
		else {
			iteration_cts[i] = 0;
		}
	}
	return;
}

void Backgroundanimation_4() {
	static char init = 0;

	int32_t i, linesize;
	int16_t xoff = 0, yoff = 0;
	uint8_t tval = 0, numBlobs = 20, VEL = 0, alpha = 0;

	if (!init) {
		/*Load background raw data*/
#if defined(FT81X_ENABLE) && defined(DISPLAY_RESOLUTION_WVGA)
		Gpu_Hal_LoadImageToMemory(s_pHalContext, TEST_DIR "\\hline_H.raw", 855 * 1024L, LOAD);
#else
		Gpu_Hal_LoadImageToMemory(s_pHalContext, TEST_DIR "\\hline.raw", 255 * 1024L, LOAD);
#endif

		init = 1;
	}

	if (istouch())
		VEL = 8;
	else
		VEL = 2;

	for (i = 0; i < numBlobs; i++) {
		if (iteration_cts[i] == 0) {
			xoffset_array[i] = random(s_pHalContext->Width);
			yoffset_array[i] = random(s_pHalContext->Height);
			dx_pts[i] = random(s_pHalContext->Width);
			dy_pts[i] = random(s_pHalContext->Height);
			rate_cts[i] = 500 + random(500);
		}
		if (iteration_cts[i] < rate_cts[i])
			iteration_cts[i] += VEL;
		else {
			iteration_cts[i] = 0;
		}
	}
	EVE_Cmd_wr32(s_pHalContext, SAVE_CONTEXT());
	//draw the bitmap at location 0 with RGB565
	EVE_Cmd_wr32(s_pHalContext, BITMAP_HANDLE(1));	//bitmap handle 2 is used for background balls
#if defined(FT81X_ENABLE) && defined(DISPLAY_RESOLUTION_WVGA)
	EVE_Cmd_wr32(s_pHalContext, BITMAP_SOURCE(855 * 1024L));
#else
	EVE_Cmd_wr32(s_pHalContext, BITMAP_SOURCE(255 * 1024L));
#endif

#if defined(DISPLAY_RESOLUTION_WVGA)
	EVE_Cmd_wr32(s_pHalContext, BITMAP_LAYOUT(L8, s_pHalContext->Width, 1));
#if defined(FT81X_ENABLE)
	EVE_Cmd_wr32(s_pHalContext, BITMAP_LAYOUT_H(s_pHalContext->Width >> 10, 0));
#endif
	EVE_Cmd_wr32(s_pHalContext, BITMAP_SIZE(NEAREST, BORDER, REPEAT, s_pHalContext->Width, s_pHalContext->Height));
#if defined(FT81X_ENABLE)
	EVE_Cmd_wr32(s_pHalContext, BITMAP_SIZE_H(s_pHalContext->Width >> 9, s_pHalContext->Height >> 9));
#endif
#else
	EVE_Cmd_wr32(s_pHalContext, BITMAP_LAYOUT(L8, 512, 1));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_SIZE(NEAREST, BORDER, REPEAT, 512, 512));
#endif

	//clear the background color
	EVE_Cmd_wr32(s_pHalContext, CLEAR_COLOR_RGB(255, 255, 255));
	EVE_Cmd_wr32(s_pHalContext, CLEAR_COLOR_A(0));
	EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1));
	EVE_Cmd_wr32(s_pHalContext, COLOR_MASK(1, 1, 1, 1));

	EVE_CoCmd_loadIdentity(s_pHalContext);
	EVE_CoCmd_rotate(s_pHalContext, 60 * 65536 / 360);	//rotate by 30 degrees clock wise

	EVE_CoCmd_setMatrix(s_pHalContext);

	EVE_Cmd_wr32(s_pHalContext, COLOR_MASK(0, 0, 0, 1));
	EVE_Cmd_wr32(s_pHalContext, BLEND_FUNC(ONE, ZERO));
	EVE_Cmd_wr32(s_pHalContext, BEGIN(BITMAPS));
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0xff, 0xb4, 0x00));
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 255, 255));
	EVE_Cmd_wr32(s_pHalContext, VERTEX2II(0, 0, 1, 0));

	EVE_Cmd_wr32(s_pHalContext, COLOR_MASK(1, 1, 1, 1));
	EVE_Cmd_wr32(s_pHalContext, BLEND_FUNC(DST_ALPHA, ONE_MINUS_DST_ALPHA));
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0xff, 0xb4, 0x00));
	EVE_Cmd_wr32(s_pHalContext, BEGIN(RECTS));
	EVE_Cmd_wr32(s_pHalContext, VERTEX2F(0, 0));
	EVE_Cmd_wr32(s_pHalContext, VERTEX2F(s_pHalContext->Width * 16, s_pHalContext->Height * 16));

	EVE_Cmd_wr32(s_pHalContext, BEGIN(RECTS));
	EVE_Cmd_wr32(s_pHalContext, BLEND_FUNC(SRC_ALPHA, ONE_MINUS_DST_ALPHA));
	EVE_Cmd_wr32(s_pHalContext, COLOR_MASK(1, 1, 1, 0));
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0x96, 0x6e, 0x0d));
	EVE_Cmd_wr32(s_pHalContext, LINE_WIDTH(16 * 1));
	for (i = 0; i < numBlobs; i++) {
		int32_t xoffset, yoffset;
		if (0 == i % 4) {
#if defined(DISPLAY_RESOLUTION_WVGA)
			linesize = 16 * (40 + (3 * i / 4));
#else
			linesize = 16 * (25 + (3 * i / 4));
#endif
		}
		alpha = linear(80, 0, iteration_cts[i], rate_cts[i]);
		if (alpha < 75) {
			xoff = linear(xoffset_array[i], dx_pts[i], iteration_cts[i], rate_cts[i]);
			yoff = linear(yoffset_array[i], dy_pts[i], iteration_cts[i], rate_cts[i]);
			EVE_Cmd_wr32(s_pHalContext, VERTEX2F(xoff * 16, yoff * 16));
			EVE_Cmd_wr32(s_pHalContext, VERTEX2F(xoff * 16 + linesize, yoff * 16 + linesize));
		}
	}
	EVE_Cmd_wr32(s_pHalContext, RESTORE_CONTEXT());
}

static struct {
	uint8_t init;
	uint16_t xoffset_array[20];
	uint16_t yoffset_array[20];
	int16_t yoffset_array_source[20];
	uint16_t iteration_cts[20];
	uint8_t disable_cts[20][12];
	uint8_t radius_a[12];
	uint8_t radius_b[12];
	uint16_t angle[12];
	uint8_t number_of_firebubbles;
} firebubbles;

void draw_bubbles(uint8_t inc) {
	int16_t i, j, yoff, xoff;
	for (j = 0; j < 3; j++) {
		EVE_Cmd_wr32(s_pHalContext, CELL(j));
		if (j == 2)
			EVE_Cmd_wr32(s_pHalContext, COLOR_A(150));
		else
			EVE_Cmd_wr32(s_pHalContext, COLOR_A(255));
		for (i = 0; i < firebubbles.number_of_firebubbles; i++) {
			if (firebubbles.iteration_cts[j * 5 + i] < firebubbles.yoffset_array[j * 5 + i] + 0) {
				yoff = acceleration(firebubbles.yoffset_array_source[j * 5 + i], firebubbles.yoffset_array[j * 5 + i],
					firebubbles.iteration_cts[j * 5 + i], firebubbles.yoffset_array[j * 5 + i] * 1);
				EVE_Cmd_wr32(s_pHalContext, VERTEX2F(firebubbles.xoffset_array[j * 5 + i] * 16, yoff * 16));

				if (inc) {
					if (firebubbles.iteration_cts[j * 5 + i] < firebubbles.yoffset_array[j * 5 + i] * 1)
						firebubbles.iteration_cts[j * 5 + i] += 1;
				}
			}
		}
	}
}

void collaid_bubbles(uint8_t inc) {
	int16_t i, j, k, yoff, xoff, temp;
	static uint8_t rate = 50;
	EVE_Cmd_wr32(s_pHalContext, CELL(3));
	for (j = 0; j < 3; j++) {
		for (i = 0; i < firebubbles.number_of_firebubbles; i++) {
			if (firebubbles.iteration_cts[j * 5 + i] >= firebubbles.yoffset_array[j * 5 + i]) {
				for (k = 0; k < 12; k++) {
					EVE_Cmd_wr32(s_pHalContext, COLOR_A(200 - firebubbles.disable_cts[j * 5 + i][k] * 10));
					temp = (uint8_t)deceleration(0, firebubbles.radius_a[k], firebubbles.disable_cts[j * 5 + i][k],
						20);
					xoff = firebubbles.xoffset_array[j * 5 + i] + 10 + (temp)* cos(firebubbles.angle[k] * 0.01744); //3.14/180=0.01744
					temp = (uint8_t)deceleration(0, firebubbles.radius_b[k], firebubbles.disable_cts[j * 5 + i][k],
						20);
					yoff = firebubbles.yoffset_array[j * 5 + i] + 10 + (temp)* sin(firebubbles.angle[k] * 0.01744); //3.14/180=0.01744
					EVE_Cmd_wr32(s_pHalContext, VERTEX2F(xoff * 16, yoff * 16));
					if (inc) {
						temp = j * 5 + i;
						if (firebubbles.disable_cts[temp][k] < 20)
							firebubbles.disable_cts[temp][k]++;
						else {
							firebubbles.disable_cts[temp][k] = 0;
							firebubbles.iteration_cts[temp] = 0;
							firebubbles.xoffset_array[temp] = random(s_pHalContext->Width);
							firebubbles.yoffset_array_source[temp] = -50 - random(100);
							if (j == 0)
								firebubbles.yoffset_array[temp] = random(20) + (s_pHalContext->Height - 50);
							else if (j == 1)
								firebubbles.yoffset_array[temp] = random(20) + (s_pHalContext->Height - 75);
							else if (j == 2)
								firebubbles.yoffset_array[temp] = random(20) + (s_pHalContext->Height - 95);

						}
					}
				}
			}
		}
	}
}

void Backgroundanimation_5() {
	int16_t i, j, yoff, xoff;
	firebubbles.number_of_firebubbles = 4;

	if (!firebubbles.init) {
		firebubbles.init = 1;
		for (i = 0; i < 20; i++) {
#if defined(DISPLAY_RESOLUTION_WVGA)
			firebubbles.xoffset_array[i] = random(s_pHalContext->Width);
			firebubbles.yoffset_array[i] = random(85) + s_pHalContext->Height - 100;
#else
			firebubbles.xoffset_array[i] = random(s_pHalContext->Width);
			firebubbles.yoffset_array[i] = random(50) + s_pHalContext->Height - 75;
#endif
			firebubbles.yoffset_array_source[i] = -50 - random(400);
		}
		for (i = 0; i < 12; i++) {
			firebubbles.radius_a[i] = 20 + random(30);
			firebubbles.radius_b[i] = 3 + random(7);
			firebubbles.angle[i] = random(360);
		}
		for (i = 0; i < 12; i++) {
			firebubbles.radius_a[i] = random(70);
			firebubbles.radius_b[i] = random(10);
			firebubbles.angle[i] = random(360);
		}
		Gpu_Hal_LoadImageToMemory(s_pHalContext, TEST_DIR "\\fire.raw", 0 * 1024L, LOAD);
		Gpu_Hal_LoadImageToMemory(s_pHalContext, TEST_DIR "\\floor.raw", 7 * 1024L, LOAD);
		Gpu_Hal_LoadImageToMemory(s_pHalContext, TEST_DIR "\\grad.raw", 17 * 1024L, LOAD);

		EVE_Cmd_wr32(s_pHalContext, BITMAP_HANDLE(5));
		EVE_Cmd_wr32(s_pHalContext, BITMAP_SOURCE(0));
		EVE_Cmd_wr32(s_pHalContext, BITMAP_LAYOUT(L8, 40, 40));
		EVE_Cmd_wr32(s_pHalContext, BITMAP_SIZE(NEAREST, BORDER, BORDER, 40, 40));

		EVE_Cmd_wr32(s_pHalContext, BITMAP_HANDLE(6));
		EVE_Cmd_wr32(s_pHalContext, BITMAP_SOURCE(7 * 1024));
		EVE_Cmd_wr32(s_pHalContext, BITMAP_LAYOUT(L8, 240, 40));
		EVE_Cmd_wr32(s_pHalContext, BITMAP_SIZE(NEAREST, BORDER, BORDER, s_pHalContext->Width, 80));
#if defined(FT81X_ENABLE)
		EVE_Cmd_wr32(s_pHalContext, BITMAP_SIZE_H(s_pHalContext->Width >> 9, 0));
#endif

		EVE_Cmd_wr32(s_pHalContext, BITMAP_HANDLE(8));
		EVE_Cmd_wr32(s_pHalContext, BITMAP_SOURCE(17 * 1024));
		EVE_Cmd_wr32(s_pHalContext, BITMAP_LAYOUT(L8, 60, 30));
		EVE_Cmd_wr32(s_pHalContext, BITMAP_SIZE(NEAREST, BORDER, BORDER, 120, 60));

		for (i = 0; i < 80; i++) {
			EVE_Hal_wr8(s_pHalContext, 19 * 1024L + i, 255 - (i * 2));
		}
		EVE_Cmd_wr32(s_pHalContext, BITMAP_HANDLE(7));
		EVE_Cmd_wr32(s_pHalContext, BITMAP_SOURCE(19 * 1024));
		EVE_Cmd_wr32(s_pHalContext, BITMAP_LAYOUT(L8, 1, 80));
		EVE_Cmd_wr32(s_pHalContext, BITMAP_SIZE(NEAREST, REPEAT, BORDER, s_pHalContext->Width, 80));
#if defined(FT81X_ENABLE)
		EVE_Cmd_wr32(s_pHalContext, BITMAP_SIZE_H(s_pHalContext->Width >> 9, 0));
#endif
	}
	EVE_Cmd_wr32(s_pHalContext, SAVE_CONTEXT());
	EVE_Cmd_wr32(s_pHalContext, BEGIN(BITMAPS));
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(139, 92, 50));

#if defined(DISPLAY_RESOLUTION_WVGA)
	EVE_Cmd_wr32(s_pHalContext, BITMAP_TRANSFORM_A(64));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_TRANSFORM_E(128));
#else
	EVE_Cmd_wr32(s_pHalContext, BITMAP_TRANSFORM_A(128));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_TRANSFORM_E(128));
#endif

	EVE_Cmd_wr32(s_pHalContext, VERTEX2II(0, s_pHalContext->Height - 80, 6, 0));

	EVE_Cmd_wr32(s_pHalContext, BITMAP_TRANSFORM_A(170));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_TRANSFORM_E(200));
	EVE_Cmd_wr32(s_pHalContext, BLEND_FUNC(SRC_ALPHA, ONE));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_TRANSFORM_A(256));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_TRANSFORM_E(256));
	EVE_Cmd_wr32(s_pHalContext, BLEND_FUNC(SRC_ALPHA, ONE_MINUS_SRC_ALPHA));
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0, 0, 0));

	EVE_Cmd_wr32(s_pHalContext, VERTEX2II(0, s_pHalContext->Height - 80, 7, 0));
	EVE_Cmd_wr32(s_pHalContext, BLEND_FUNC(SRC_ALPHA, ONE));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_HANDLE(5));
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(200, 10, 10));
	EVE_Cmd_wr32(s_pHalContext, COLOR_A(200));
	draw_bubbles(0);
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 220, 3));
	EVE_Cmd_wr32(s_pHalContext, COLOR_A(255));
	draw_bubbles(1);
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(200, 10, 10));
	collaid_bubbles(0);
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 220, 3));
	collaid_bubbles(1);
	EVE_Cmd_wr32(s_pHalContext, RESTORE_CONTEXT());
}

#ifdef BACKGROUND_ANIMATION_6

#define ANGLE 18 

void Backgroundanimation_6()
{
	static float_t move = 0;
	static uint8_t init = 0;
	int z = 0, x = 0, y = 0;
	int16_t Ox = 0, Oy = 0;
	uint8_t VEL = 0;
	if (istouch()) VEL = 2; else VEL = 1;
	if (!init)
	{
		for (z = 0; z<20; z++)
		{
#if defined(DISPLAY_RESOLUTION_WVGA)
			point_size[z] = 20 + random(35);
			dx_pts[z] = random(900);
#else
			point_size[z] = 20 + random(20);
			dx_pts[z] = random(600);
#endif
			if (dx_pts[z]<480)
				dy_pts[z] = -random(s_pHalContext->Height);
			else
				dy_pts[z] = random(s_pHalContext->Height);
			rate_cts[z] = 100 + random(155);
		}
		init = 1;
	}
	move += 0.1;
#if defined(DISPLAY_RESOLUTION_WVGA)
	if (move >= 135)
#else
	if (move >= 90)
#endif
		move = 0;
	EVE_Cmd_wr32(s_pHalContext, SAVE_CONTEXT());
#if defined(DISPLAY_RESOLUTION_WVGA)
	EVE_CoCmd_gradient(s_pHalContext, 0, 0, 0x183c78, s_pHalContext->Width, 0, 0x4560dd);
#else
	EVE_CoCmd_gradient(s_pHalContext, 0, 0, 0x183c78, 320, 0, 0x4560dd);
#endif
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 255, 255));
	EVE_Cmd_wr32(s_pHalContext, COLOR_A(120));
#if defined(DISPLAY_RESOLUTION_WVGA)
	EVE_Cmd_wr32(s_pHalContext, LINE_WIDTH(25 * 16));
#else
	EVE_Cmd_wr32(s_pHalContext, LINE_WIDTH(15 * 16));
#endif
	EVE_Cmd_wr32(s_pHalContext, BEGIN(LINES));
	for (z = 0; z<20; z++)
	{
#if defined(DISPLAY_RESOLUTION_WVGA)
		polar_draw((int32_t)(0), move + z*ANGLE, 0, s_pHalContext->Height);
		polar_draw((int32_t)(950), move + z*ANGLE, 0, s_pHalContext->Height);
#else
		polar_draw((int32_t)(0), move + z*ANGLE, 0, s_pHalContext->Height);
		polar_draw((int32_t)(600), move + z*ANGLE, 0, s_pHalContext->Height);
#endif
	}
	EVE_Cmd_wr32(s_pHalContext, COLOR_A(60));
	EVE_Cmd_wr32(s_pHalContext, BEGIN(FTPOINTS));
#if defined(DISPLAY_RESOLUTION_WVGA)
	EVE_Cmd_wr32(s_pHalContext, POINT_SIZE(120 * 16));
#else
	EVE_Cmd_wr32(s_pHalContext, POINT_SIZE(95 * 16));
#endif
	EVE_Cmd_wr32(s_pHalContext, VERTEX2F(0, s_pHalContext->Height * 16));
	EVE_Cmd_wr32(s_pHalContext, COLOR_A(200));
	for (z = 0; z<20; z++)
	{
		EVE_Cmd_wr32(s_pHalContext, POINT_SIZE(point_size[z] * 16));
		Ox = linear(0, dx_pts[z], iteration_cts[z], rate_cts[z]);
		Oy = linear(272, dy_pts[z], iteration_cts[z], rate_cts[z]);
		EVE_Cmd_wr32(s_pHalContext, VERTEX2F(Ox * 16, Oy * 16));
	}
	EVE_Cmd_wr32(s_pHalContext, RESTORE_CONTEXT());
	for (z = 0; z<20; z++)
	{
		if (iteration_cts[z]<rate_cts[z])iteration_cts[z] += VEL; else { iteration_cts[z] = 0; }
	}
}
#endif

/*API to show the icon with background animation*/
void show_icon(uint8_t iconno) {
	Play_Sound(s_pHalContext, 0x51, 100, 108);
	do {
		EVE_CoCmd_dlStart(s_pHalContext);
		EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1));
		/* Save the graphics context before enter into background animatioon*/
		EVE_Cmd_wr32(s_pHalContext, SAVE_CONTEXT());
#ifdef BACKGROUND_ANIMATION_1
		Backgroundanimation_1();
#endif

#ifdef BACKGROUND_ANIMATION_2
		Backgroundanimation_2();
#endif
#ifdef BACKGROUND_ANIMATION_3
		Backgroundanimation_3();
#endif
#ifdef BACKGROUND_ANIMATION_4
		Backgroundanimation_4();
#endif

#ifdef BACKGROUND_ANIMATION_5
		Backgroundanimation_5();
#endif
#ifdef BACKGROUND_ANIMATION_6
		Backgroundanimation_6();
#endif

		/* Restore the graphics context */
		EVE_Cmd_wr32(s_pHalContext, RESTORE_CONTEXT());
		EVE_Cmd_wr32(s_pHalContext, BEGIN(BITMAPS));
		EVE_Cmd_wr32(s_pHalContext, SAVE_CONTEXT());
		EVE_Cmd_wr32(s_pHalContext, BITMAP_HANDLE(0));
		EVE_Cmd_wr32(s_pHalContext, BITMAP_SIZE(NEAREST, BORDER, BORDER, displayImageWidth * 2, displayImageHeight * 2));
		EVE_Cmd_wr32(s_pHalContext, CELL(iconno - 1));
		EVE_Cmd_wr32(s_pHalContext, BITMAP_TRANSFORM_A(128));
		EVE_Cmd_wr32(s_pHalContext, BITMAP_TRANSFORM_E(128));
		EVE_Cmd_wr32(s_pHalContext,
			VERTEX2F(((s_pHalContext->Width - (displayImageWidth * 2)) / 2) * 16,
			((s_pHalContext->Height - (displayImageHeight * 2)) / 2) * 16));
		EVE_Cmd_wr32(s_pHalContext, BITMAP_TRANSFORM_A(256));
		EVE_Cmd_wr32(s_pHalContext, BITMAP_TRANSFORM_E(256));
		EVE_Cmd_wr32(s_pHalContext, RESTORE_CONTEXT());
		EVE_Cmd_wr32(s_pHalContext, TAG('H'));
#ifdef BACKGROUND_ANIMATION_4
		EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0, 0, 0));
#endif

		/* Draw a home button a top left screen */
		EVE_Cmd_wr32(s_pHalContext, BEGIN(BITMAPS));
		EVE_Cmd_wr32(s_pHalContext, BITMAP_HANDLE(START_ICON_HANDLE));
		EVE_Cmd_wr32(s_pHalContext, BITMAP_SOURCE(START_ICON_ADDR));      // Starting address in gram
		EVE_Cmd_wr32(s_pHalContext, BITMAP_LAYOUT(L4, 16, 32));  // format
		EVE_Cmd_wr32(s_pHalContext, BITMAP_SIZE(NEAREST, BORDER, BORDER, 32, 32));

		EVE_Cmd_wr32(s_pHalContext, VERTEX2II(5, 5, START_ICON_HANDLE, 0));
		EVE_Cmd_wr32(s_pHalContext, DISPLAY());
		EVE_CoCmd_swap(s_pHalContext);
		EVE_Cmd_waitFlush(s_pHalContext);
	} while (Gesture_GetTag(s_pHalContext) != 'H');
	Play_Sound(s_pHalContext, 0x51, 100, 108);
	scroller.vel = 0;
}
void android_menu() {
	uint8_t image_height = displayImageHeight/*50*/, image_width = displayImageWidth/*100*/;
	uint16_t dt = 30, dx, dy;
	uint8_t col, row, per_frame, noof_frame, current_frame = 0;
	uint8_t i, key_in = 0, key_in_counts = 0, temp = 0;

	int16_t Ox, Oy, sx, drag = 0, prev = 0, drag_dt = 30, dragth = 0;

	// for points

	uint16_t point_offset, point_dt = 15;

#if defined(DISPLAY_RESOLUTION_WVGA)
	dx = (dt * 3) + image_width;
	dy = (30 * 2) + image_height;
#elif defined(DISPLAY_RESOLUTION_HVGA_PORTRAIT)
	dx = (dt * 2) + image_width;
	dy = (50 * 2) + image_height;
#else
	dx = (dt * 2) + image_width;
	dy = (10 * 2) + image_height;
#endif
	col = s_pHalContext->Width / dx;
	row = 2;
	per_frame = col * row;
	noof_frame = (MAX_MENUS - 1) / per_frame;

	point_offset = (s_pHalContext->Width - (noof_frame + 1) * (MENU_POINTSIZE + point_dt)) / 2;

	scroller_init((s_pHalContext->Width * noof_frame) * 16);

	while (1) {
		/*Read touch screen x varaiation and tag in*/
		sx = EVE_Hal_rd16(s_pHalContext, REG_TOUCH_SCREEN_XY + 2);
		key_in = Gesture_GetTag(s_pHalContext);

		/*Check if any tag in*/
		if (sx != NOTOUCH)
			keyin_cts++;

		/*Move into the particular frame based on dragdt now 30pixels*/
		if (sx == NOTOUCH) {
			keyin_cts = 0;
			if (drag > ((current_frame * s_pHalContext->Width) + drag_dt))
				drag = MIN((current_frame + 1)*s_pHalContext->Width, drag + 15);
			if (drag < ((current_frame * s_pHalContext->Width) - drag_dt))
				drag = MAX((current_frame - 1)*s_pHalContext->Width, drag - 15);
			if (dragth == drag)
				current_frame = drag / s_pHalContext->Width;
			dragth = drag;
			scroller.vel = 0;
			scroller.base = dragth * 16;				// 16bit pre
		}
		/*if tag in but still pendown take a scroller basevalue*/
		else if (keyin_cts > 5) {
			key_in = 0;
			drag = scroller.base >> 4;
		}
		if (key_in == 0)
			scroller_run();

		/*Display list start*/
		EVE_CoCmd_dlStart(s_pHalContext);
		EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1));
#ifdef BACKGROUND_ANIMATION_1
		Backgroundanimation_1();
#endif
#ifdef BACKGROUND_ANIMATION_2
		Backgroundanimation_2();
#endif
#ifdef BACKGROUND_ANIMATION_3
		Backgroundanimation_3();
#endif
#ifdef BACKGROUND_ANIMATION_4
		Backgroundanimation_4();
#endif
#ifdef BACKGROUND_ANIMATION_5
		Backgroundanimation_5();
#endif
#ifdef BACKGROUND_ANIMATION_6
		Backgroundanimation_6();
#endif
		EVE_Cmd_wr32(s_pHalContext, BITMAP_HANDLE(0));
		EVE_Cmd_wr32(s_pHalContext, BITMAP_SIZE(NEAREST, BORDER, BORDER, displayImageWidth, displayImageHeight));
#if defined(FT81X_ENABLE)
		EVE_Cmd_wr32(s_pHalContext, BITMAP_SIZE_H(displayImageWidth >> 9, displayImageHeight >> 9));
#endif
		EVE_Cmd_wr32(s_pHalContext, TAG_MASK(1));
		EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 255, 255));
		EVE_Cmd_wr32(s_pHalContext, LINE_WIDTH(25));				// for rect
		EVE_Cmd_wr32(s_pHalContext, BEGIN(RECTS));
		EVE_Cmd_wr32(s_pHalContext, COLOR_A(150));
		EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(100, 106, 156));
		Oy = 10;
		for (i = 0; i <= noof_frame; i++) {
			Ox = 10;
			Ox += (i * s_pHalContext->Width);
			Ox -= drag;
			if (Ox > 1023)
				Ox = 1023;
			if (i == 0)
				EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(156, 100, 128));
			if (i == 1)
				EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(100, 106, 156));
			if (i == 2)
				EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(156, 152, 100));
			EVE_Cmd_wr32(s_pHalContext, VERTEX2F((Ox) * 16, (Oy) * 16));
#if defined(DISPLAY_RESOLUTION_WVGA)
			if ((Ox + s_pHalContext->Width - 20) > 1023)
				EVE_Cmd_wr32(s_pHalContext, VERTEX2F((1023) * 16, (int16_t)(s_pHalContext->Height * 0.75) * 16));
			else
				EVE_Cmd_wr32(s_pHalContext, VERTEX2F((Ox + s_pHalContext->Width - 20) * 16, (int16_t)(s_pHalContext->Height * 0.75) * 16));
#else
			EVE_Cmd_wr32(s_pHalContext, VERTEX2F((Ox + s_pHalContext->Width - 20) * 16, (int16_t)(s_pHalContext->Height*0.75) * 16));
#endif// i pixels wide than image width +1
		}
		EVE_Cmd_wr32(s_pHalContext, COLOR_A(255));
		EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 255, 255));
		for (i = 0; i < MAX_MENUS; i++) {
			Ox = dt + dx * (i % col);                                          // Calculate the xoffsets
			Ox += ((i / per_frame) * s_pHalContext->Width);
			Ox -= drag;
			Oy = dt + (dy * ((i / col) % row));
			if (Ox >(s_pHalContext->Width + dt))
				0;
			else {
				EVE_Cmd_wr32(s_pHalContext, VERTEX2F((Ox - 1) * 16, (Oy - 1) * 16));
				EVE_Cmd_wr32(s_pHalContext, VERTEX2F((image_width + Ox + 1) * 16, (image_height + Oy + 1) * 16));
			}					// i pixels wide than image width +1
		}
		EVE_Cmd_wr32(s_pHalContext, TAG_MASK(1));
		EVE_Cmd_wr32(s_pHalContext, BEGIN(BITMAPS));
		EVE_Cmd_wr32(s_pHalContext, BITMAP_HANDLE(0));
		for (i = 0; i < MAX_MENUS; i++) {
			Ox = dt + dx * (i % col);                                          // Calculate the xoffsets
			Ox += ((i / per_frame) * s_pHalContext->Width);
			Ox -= drag;
			Oy = dt + (dy * ((i / col) % row));
			if (Ox >(s_pHalContext->Width + dt) || Ox < -dx)
				0;
			else {
				EVE_Cmd_wr32(s_pHalContext, CELL(i));
				EVE_Cmd_wr32(s_pHalContext, TAG(i + 1));
				EVE_Cmd_wr32(s_pHalContext, VERTEX2F(Ox * 16, Oy * 16));
			}
		}
		EVE_Cmd_wr32(s_pHalContext, TAG_MASK(0));

		// frame_no_points

		EVE_Cmd_wr32(s_pHalContext, POINT_SIZE(MENU_POINTSIZE * 16));
		EVE_Cmd_wr32(s_pHalContext, BEGIN(FTPOINTS));
		EVE_Cmd_wr32(s_pHalContext, COLOR_A(50));
		Oy = s_pHalContext->Height - 20;
		for (i = 0; i <= noof_frame; i++) {
			Ox = point_offset + (i * (MENU_POINTSIZE + point_dt));
			EVE_Cmd_wr32(s_pHalContext, VERTEX2F(Ox * 16, Oy * 16));
		}

		Ox = point_offset + (current_frame * (MENU_POINTSIZE + point_dt));
		EVE_Cmd_wr32(s_pHalContext, COLOR_A(255));
		EVE_Cmd_wr32(s_pHalContext, VERTEX2F(Ox * 16, Oy * 16));

		EVE_Cmd_wr32(s_pHalContext, DISPLAY());
		EVE_CoCmd_swap(s_pHalContext);
		EVE_Cmd_waitFlush(s_pHalContext);
		if (key_in > 0 && key_in <= 12 && !istouch())
			show_icon(key_in);
	}
}

void menu_loopback() {
	uint8_t image_height = displayImageHeight/*50*/, image_width = displayImageWidth/*100*/;
	uint8_t dt = 30, dx, dy;
	uint8_t per_frame, no_frames, key_in, current_frame;
	int16_t sx, drag, Oy, Ox, dragth, i;
	dx = (dt * 2) + image_width;
	dy = (10 * 2) + image_height;
	per_frame = s_pHalContext->Width / dx;
	no_frames = (MAX_MENUS - 1) / per_frame;

#if defined(DISPLAY_RESOLUTION_WVGA)
	scroller_init((s_pHalContext->Width * no_frames - displayImageWidth * 3) * 16);
#else
	scroller_init((s_pHalContext->Width*no_frames) * 16);
#endif

	while (1) {
		/*Read touch screen x varaiation and tag in*/
		sx = EVE_Hal_rd16(s_pHalContext, REG_TOUCH_SCREEN_XY + 2);
		key_in = Gesture_GetTag(s_pHalContext);

		/*Check if any tag in*/
		if (sx != NOTOUCH)
			keyin_cts++;
		/*Move into the particular frame based on dragdt now 30pixels*/
		if (sx == NOTOUCH)
			keyin_cts = 0;
		/*if tag in but still pendown take a scroller basevalue*/
		else if (keyin_cts > KEYIN_COUNTS)
			key_in = 0;

		if (key_in == 0)
			scroller_run();
		drag = scroller.base >> 4;

		EVE_CoCmd_dlStart(s_pHalContext);
		EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1));
		EVE_Cmd_wr32(s_pHalContext, BITMAP_HANDLE(0));
#ifdef BACKGROUND_ANIMATION_1
		Backgroundanimation_1();
#endif
#ifdef BACKGROUND_ANIMATION_2
		Backgroundanimation_2();
#endif
#ifdef BACKGROUND_ANIMATION_3
		Backgroundanimation_3();
#endif
#ifdef BACKGROUND_ANIMATION_4
		Backgroundanimation_4();
#endif
#ifdef BACKGROUND_ANIMATION_5
		Backgroundanimation_5();
#endif
#ifdef BACKGROUND_ANIMATION_6
		Backgroundanimation_6();
#endif
		image_height = displayImageHeight;
		image_width = displayImageWidth;
		EVE_Cmd_wr32(s_pHalContext, TAG_MASK(1));
		EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 255, 255));
		EVE_Cmd_wr32(s_pHalContext, LINE_WIDTH(1 * 16)); // for rect

		Oy = (s_pHalContext->Height - image_width) / 2;					//dt+(dy*((i/col)%row));
		current_frame = drag / dx;                       // noof items moved in +/- directions
		dragth = drag % dx;

		EVE_Cmd_wr32(s_pHalContext, BEGIN(RECTS));
#if defined(DISPLAY_RESOLUTION_WVGA)
		for (i = -1; i < (per_frame + 2); i++)
#else
		for (i = -1; i<(per_frame + 1); i++)
#endif
		{
			Ox = dt + dx * i;
			Ox -= dragth;
			if (Ox >(s_pHalContext->Width + dt) || Ox < -dx)
				0;
			else {
				EVE_Cmd_wr32(s_pHalContext, VERTEX2F((Ox - 1) * 16, (Oy - 1) * 16));
				EVE_Cmd_wr32(s_pHalContext,
					VERTEX2F((displayImageWidth + Ox + 1) * 16, (displayImageHeight + Oy + 1) * 16)); // i pixels wide than image width +1
			}
		}

		EVE_Cmd_wr32(s_pHalContext, BEGIN(BITMAPS));											// draw the bitmap
		EVE_Cmd_wr32(s_pHalContext, BITMAP_HANDLE(0));
#if defined(DISPLAY_RESOLUTION_WVGA)
		for (i = -1; i < (per_frame + 2); i++)
#else
		for (i = -1; i<(per_frame + 1); i++)
#endif
		{
			Ox = dt + dx * i;
			Ox -= dragth;
			if (Ox >(s_pHalContext->Width + dt) || Ox < -dx)
				0;
			else {
				EVE_Cmd_wr32(s_pHalContext, CELL((MAX_MENUS + i + current_frame) % 12));
				EVE_Cmd_wr32(s_pHalContext, TAG((1 + i + current_frame) % (MAX_MENUS + 1)));
				EVE_Cmd_wr32(s_pHalContext, VERTEX2F(Ox * 16, Oy * 16));
			}
		}

		EVE_Cmd_wr32(s_pHalContext, DISPLAY());
		EVE_CoCmd_swap(s_pHalContext);
		EVE_Cmd_waitFlush(s_pHalContext);
		if (key_in > 0 && key_in <= 12 && !istouch())
			show_icon(key_in);
	}
}

void menu_win8() {
	uint8_t current_frame = 0, total_frames = 0, key_in = 0;
	int16_t frame_xoffset = 0, frame_xoffset_th = 0;
	uint8_t menus_per_frame = 0;
	uint8_t col = 3, row = 2, option;
	uint16_t image_height = displayImageHeight/*50*/, image_width = displayImageWidth/*100*/, rectangle_width,
		rectangle_height;

	int16_t Ox, Oy, i, sx;
#if defined(DISPLAY_RESOLUTION_WVGA)
	uint8_t frame_xoffset_dt = 30;
#else
	uint8_t frame_xoffset_dt = 30;
#endif

#if defined(DISPLAY_RESOLUTION_HVGA_PORTRAIT)
	uint8_t blockGap = 40, backgroundBlockHeight = 110;
#elif defined(DISPLAY_RESOLUTION_QVGA)
	uint8_t blockGap = 10, backgroundBlockHeight = 110;
#endif

	uint8_t color[12][3] = { 0xE0, 0x01B, 0xA2, 0x1B, 0xE0, 0xA8, 0x9E, 0x9E, 0x73, 0xE0, 0x8E, 0x1B, 0xB8, 0x91, 0xB3,
		0x6E, 0x96, 0x8e, 0x1B, 0x60, 0xE0, 0xC7, 0xE3, 0x7B, 0x8B, 0x1B, 0xE0, 0xE3, 0x91, 0xC1, 0xE0, 0x8E, 0x1B,
		0xAC, 0x12, 0xE3, };

	char *menudetails[] = { "Music", "Gauges ", "Gradient", "Photo", "Metaball", "Notepad", "Signature", "Sketch",
		"Swiss", "Waves", "Player", "Clocks" };

	uint8_t point_offset, frame_point_dt = 15;

	uint16_t dx = (frame_xoffset_dt * 2) + image_width;
	uint16_t dy = (10 * 2) + image_height;
	col = s_pHalContext->Width / dx;
	menus_per_frame = col * row;
	total_frames = (MAX_MENUS - 1) / menus_per_frame;

	point_offset = (s_pHalContext->Width - (total_frames + 1) * (MENU_POINTSIZE + frame_point_dt)) / 2;
	/*Load menu Thumbnails*/
	//Load_Thumbnails();
	/*Intilaize the scroller*/
	scroller_init((s_pHalContext->Width * total_frames) * 16);

	while (1) {
		/*Read touch screen x varaiation and tag in*/
		sx = EVE_Hal_rd16(s_pHalContext, REG_TOUCH_SCREEN_XY + 2);
		key_in = Gesture_GetTag(s_pHalContext);

		/*Check if any tag in*/
		if (sx != NOTOUCH)
			keyin_cts++;

		/*Move into the particular frame based on dragdt now 30pixels*/
		if (sx == NOTOUCH) {
			keyin_cts = 0;
			frame_xoffset = scroller.base >> 4;
		}
		/*if tag in but still pendown take a scroller basevalue*/
		else if (keyin_cts > KEYIN_COUNTS) {
			key_in = 0;
			frame_xoffset = scroller.base >> 4;
		}
		if (key_in == 0)
			scroller_run();

		App_Set_CmdBuffer_Index(0);
		EVE_CoCmd_dlStart(s_pHalContext);
		EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1));
		EVE_Cmd_wr32(s_pHalContext, BITMAP_HANDLE(0));
		EVE_Cmd_wr32(s_pHalContext, BITMAP_SIZE(NEAREST, BORDER, BORDER, displayImageWidth, displayImageHeight));
#if defined(FT81X_ENABLE)
		EVE_Cmd_wr32(s_pHalContext, BITMAP_SIZE_H(displayImageWidth >> 9, displayImageHeight >> 9));
#endif

		EVE_Cmd_wr32(s_pHalContext, SAVE_CONTEXT());
#ifdef BACKGROUND_ANIMATION_1
		Backgroundanimation_1();
#endif

#ifdef BACKGROUND_ANIMATION_2
		Backgroundanimation_2();
#endif
#ifdef BACKGROUND_ANIMATION_3
		Backgroundanimation_3();
#endif
#ifdef BACKGROUND_ANIMATION_4
		Backgroundanimation_4();
#endif

#ifdef BACKGROUND_ANIMATION_5
		Backgroundanimation_5();
#endif
#ifdef BACKGROUND_ANIMATION_6
		Backgroundanimation_6();
#endif
		EVE_Cmd_wr32(s_pHalContext, RESTORE_CONTEXT());

#if defined(DISPLAY_RESOLUTION_WVGA) && defined(FT81X_ENABLE)
		EVE_Cmd_wr32(s_pHalContext, VERTEX_FORMAT(0));
#endif

#if defined(DISPLAY_RESOLUTION_WQVGA) || defined(DISPLAY_RESOLUTION_WVGA)
		for (option = 0; option < 3; option++) {
			switch (option) {
			case 0:
				EVE_Cmd_wr32(s_pHalContext, LINE_WIDTH(1 * 16));
				EVE_Cmd_wr32(s_pHalContext, BEGIN(RECTS));
				break;
			case 1:
				EVE_Cmd_wr32(s_pHalContext, BEGIN(BITMAPS));
				EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 255, 255));
				break;
			case 2:
				EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 255, 255));
				break;
			}
#if defined(DISPLAY_RESOLUTION_WVGA)
			rectangle_width = 380;
			rectangle_height = 200;
#else
			rectangle_width = 220;
			rectangle_height = 100;
#endif
			for (i = 0; i < 4; i += 1) {
				if (i < 2) {
					Ox = 10 + s_pHalContext->Width * i;
					Oy = 10;
				}
				else {
#if defined(DISPLAY_RESOLUTION_WVGA)
					Ox = 400 + s_pHalContext->Width * (i % 2);
					Oy = 220;
#else
					Ox = 250 + s_pHalContext->Width*(i % 2);
					Oy = 120;
#endif
				}
				Ox -= frame_xoffset;
				if (Ox > (int16_t)((int16_t)s_pHalContext->Width + frame_xoffset_dt) || Ox < (int16_t)(-s_pHalContext->Width))
					0;
				else {
					EVE_Cmd_wr32(s_pHalContext, TAG(i + 1));
					switch (option) {
					case 0:
						EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(color[i][0], color[i][1], color[i][2]));
#if defined(DISPLAY_RESOLUTION_WVGA)
						EVE_Cmd_wr32(s_pHalContext, VERTEX2F((Ox), (Oy)));
						EVE_Cmd_wr32(s_pHalContext, VERTEX2F((rectangle_width + Ox), (rectangle_height + Oy)));// i pixels wide than image width +1
#else
						EVE_Cmd_wr32(s_pHalContext, VERTEX2F((Ox) * 16, (Oy) * 16));
						EVE_Cmd_wr32(s_pHalContext, VERTEX2F((rectangle_width + Ox) * 16, (rectangle_height + Oy) * 16));
#endif
						break;
					case 1:
						EVE_Cmd_wr32(s_pHalContext, CELL(i));
#if defined(DISPLAY_RESOLUTION_WVGA)
						EVE_Cmd_wr32(s_pHalContext, VERTEX2F((55 + Ox), (25 + Oy)));
#else
						EVE_Cmd_wr32(s_pHalContext, VERTEX2F((55 + Ox) * 16, (25 + Oy) * 16));
#endif
						break;

					case 2:
#if defined(DISPLAY_RESOLUTION_WVGA)
						EVE_CoCmd_text(s_pHalContext, Ox + 10, Oy + 180, 26, 0, menudetails[i]);
#else
						EVE_CoCmd_text(s_pHalContext, Ox + 10, Oy + 80, 26, 0, menudetails[i]);
#endif
						break;
					}
				}
			}
#if defined(DISPLAY_RESOLUTION_WVGA)
			rectangle_width = 180;
			rectangle_height = 200;
#else
			rectangle_width = 100;
			rectangle_height = 100;
#endif

#if defined(DISPLAY_RESOLUTION_WVGA)
			if (option == 1)
				EVE_Cmd_wr32(s_pHalContext, BITMAP_TRANSFORM_A(512));
#else
			if (option == 1) EVE_Cmd_wr32(s_pHalContext, BITMAP_TRANSFORM_A(512));
#endif

			for (i = 0; i < 8; i += 1) {
				if (i < 4) {
#if defined(DISPLAY_RESOLUTION_WVGA)  
					Ox = 400 + s_pHalContext->Width * (i / 2) + (rectangle_width * (i % 2)) + (20 * (i % 2));
					Oy = 10;
#else
					Ox = 250 + s_pHalContext->Width*(i / 2) + (image_width*(i % 2)) + (20 * (i % 2));	// 20 is space between two icon
					Oy = 10;
#endif
				}
				else {
#if defined(DISPLAY_RESOLUTION_WVGA)

					Ox = 10 + (s_pHalContext->Width * (i / 6)) + (((i - 4) % 2) * rectangle_width) + (((i - 4) % 2) * 20);
					Oy = 220;
#else
					Ox = 10 + s_pHalContext->Width*(i / 6) + (((i - 4) % 2)*image_width) + (((i - 4) % 2) * 20);
					Oy = 120;
#endif
				}
				Ox -= frame_xoffset;
				if (Ox > (int16_t)((int16_t)s_pHalContext->Width + frame_xoffset_dt) || Ox < (int16_t)-s_pHalContext->Width)
					0;
				else {
					EVE_Cmd_wr32(s_pHalContext, TAG(i + 5));
					switch (option) {
					case 0:
						EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(color[i + 5][0], color[i + 5][1], color[i + 5][2]));
#if defined(DISPLAY_RESOLUTION_WVGA)
						EVE_Cmd_wr32(s_pHalContext, VERTEX2F((Ox), (Oy)));
						EVE_Cmd_wr32(s_pHalContext, VERTEX2F((rectangle_width + Ox), (rectangle_height + Oy)));// i pixels wide than image width +1
#else
						EVE_Cmd_wr32(s_pHalContext, VERTEX2F((Ox) * 16, (Oy) * 16));
						EVE_Cmd_wr32(s_pHalContext, VERTEX2F((rectangle_width + Ox) * 16, (rectangle_height + Oy) * 16));
#endif
						break;

					case 1:
						EVE_Cmd_wr32(s_pHalContext, CELL(i + 4));
#if defined(DISPLAY_RESOLUTION_WVGA)
						EVE_Cmd_wr32(s_pHalContext,
							VERTEX2F((Ox + (rectangle_width - displayImageWidth) / 2), (25 + Oy)));
#else
						EVE_Cmd_wr32(s_pHalContext, VERTEX2F((25 + Ox) * 16, (25 + Oy) * 16));
#endif
						break;

					case 2:
#if defined(DISPLAY_RESOLUTION_WVGA)
						EVE_CoCmd_text(s_pHalContext, Ox + 10, Oy + 180, 26, 0, menudetails[i + 4]);
#else
						EVE_CoCmd_text(s_pHalContext, Ox + 10, Oy + 80, 26, 0, menudetails[i + 4]);
#endif
						break;
					}								// i pixels wide than image width +1
				}
			}

#if defined(DISPLAY_RESOLUTION_WVGA)
			if (option == 1)
				EVE_Cmd_wr32(s_pHalContext, BITMAP_TRANSFORM_A(256));
#else
			if (option == 1) EVE_Cmd_wr32(s_pHalContext, BITMAP_TRANSFORM_A(256));
#endif
		}

#elif defined(DISPLAY_RESOLUTION_HVGA_PORTRAIT) || defined(DISPLAY_RESOLUTION_QVGA)

		rectangle_width = 170;
		rectangle_height = 100;
		EVE_Cmd_wr32(s_pHalContext, BEGIN(RECTS));
		for (i = 0; i<12; i += 2)
		{
			Ox = 10 + s_pHalContext->Width*(i / 4);
			Ox -= frame_xoffset;
			Oy = blockGap + ((backgroundBlockHeight + blockGap)*((i / col) % row));
			EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(color[i][0], color[i][1], color[i][2]));
			EVE_Cmd_wr32(s_pHalContext, TAG(i + 1));
			EVE_Cmd_wr32(s_pHalContext, VERTEX2F((Ox) * 16, (Oy) * 16));
			EVE_Cmd_wr32(s_pHalContext, VERTEX2F((rectangle_width + Ox) * 16, (rectangle_height + Oy) * 16));// i pixels wide than image width +1
		}

		rectangle_width = 110;
		rectangle_height = 100;

		for (i = 1; i<12; i += 2)
		{
			Ox = 200 + s_pHalContext->Width*(i / 4);
			Ox -= frame_xoffset;

			Oy = blockGap + ((backgroundBlockHeight + blockGap)*((i / col) % row));
			EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(color[i][0], color[i][1], color[i][2]));
			EVE_Cmd_wr32(s_pHalContext, TAG(i + 1));
			EVE_Cmd_wr32(s_pHalContext, VERTEX2F((Ox) * 16, (Oy) * 16));
			EVE_Cmd_wr32(s_pHalContext, VERTEX2F((rectangle_width + Ox) * 16, (rectangle_height + Oy) * 16));// i pixels wide than image width +1
		}
		EVE_Cmd_wr32(s_pHalContext, TAG_MASK(0));

		EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 255, 255));
		for (i = 0; i<12; i += 2)
		{
			Ox = 10 + s_pHalContext->Width*(i / 4);
			Ox -= frame_xoffset;
			Oy = blockGap + ((backgroundBlockHeight + blockGap)*((i / col) % row));
			EVE_CoCmd_text(s_pHalContext, Ox + 10, Oy + 80, 26, 0, menudetails[i]);
		}
		for (i = 1; i<12; i += 2)
		{
			Ox = 200 + s_pHalContext->Width*(i / 4);
			Ox -= frame_xoffset;
			Oy = blockGap + ((backgroundBlockHeight + blockGap)*((i / col) % row));
			EVE_CoCmd_text(s_pHalContext, Ox + 10, Oy + 80, 26, 0, menudetails[i]);
		}

		rectangle_height = 100;
		rectangle_height = 50;
		EVE_Cmd_wr32(s_pHalContext, BEGIN(BITMAPS));											// draw the bitmap
		EVE_Cmd_wr32(s_pHalContext, BITMAP_HANDLE(0));
		for (i = 0; i<12; i += 2)
		{
			Ox = 75 + s_pHalContext->Width*(i / 4);
			Ox -= frame_xoffset;
			Oy = blockGap + 10 + ((backgroundBlockHeight + blockGap)*((i / col) % row));
			EVE_Cmd_wr32(s_pHalContext, CELL(i));
			EVE_Cmd_wr32(s_pHalContext, TAG(i + 1));
			EVE_Cmd_wr32(s_pHalContext, VERTEX2F(Ox * 16, Oy * 16));
		}
		EVE_Cmd_wr32(s_pHalContext, BITMAP_TRANSFORM_A(512));
		for (i = 1; i<12; i += 2)
		{
			Ox = 230 + s_pHalContext->Width*(i / 4);
			Ox -= frame_xoffset;
			Oy = blockGap + 10 + ((backgroundBlockHeight + blockGap)*((i / col) % row));
			EVE_Cmd_wr32(s_pHalContext, CELL(i));
			EVE_Cmd_wr32(s_pHalContext, TAG(i + 1));
			EVE_Cmd_wr32(s_pHalContext, VERTEX2F(Ox * 16, Oy * 16));
		}
		EVE_Cmd_wr32(s_pHalContext, BITMAP_TRANSFORM_A(256));
#endif
#if defined(DISPLAY_RESOLUTION_WVGA) && defined(FT81X_ENABLE)
		EVE_Cmd_wr32(s_pHalContext, VERTEX_FORMAT(4));
#endif
		EVE_Cmd_wr32(s_pHalContext, TAG_MASK(0));

		EVE_Cmd_wr32(s_pHalContext, DISPLAY());
		EVE_CoCmd_swap(s_pHalContext);
		EVE_Cmd_waitFlush(s_pHalContext);

		if (key_in != 0 && key_in <= 12 && !istouch())
			show_icon(key_in);
	}
}

void DemoMainmenu(EVE_HalContext* pHalContext) {
	s_pHalContext = pHalContext;
	Load_Thumbnails();
#if defined(ANDROID_METHOD)
	android_menu();
#elif defined(LOOPBACK_METHOD)
	menu_loopback();
#elif defined(WIN8_METHOD)
	menu_win8();
#endif
}
