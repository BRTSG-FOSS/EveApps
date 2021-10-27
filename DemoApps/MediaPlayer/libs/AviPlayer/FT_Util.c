/*

Copyright (c) Future Technology Devices International 2014

THIS SOFTWARE IS PROVIDED BY FUTURE TECHNOLOGY DEVICES INTERNATIONAL LIMITED "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
FUTURE TECHNOLOGY DEVICES INTERNATIONAL LIMITED BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
OF SUBSTITUTE GOODS OR SERVICES LOSS OF USE, DATA, OR PROFITS OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

FTDI DRIVERS MAY BE USED ONLY IN CONJUNCTION WITH PRODUCTS BASED ON FTDI PARTS.

FTDI DRIVERS MAY BE DISTRIBUTED IN ANY FORM AS LONG AS LICENSE INFORMATION IS NOT MODIFIED.

IF A CUSTOM VENDOR ID AND/OR PRODUCT ID OR DESCRIPTION STRING ARE USED, IT IS THE
RESPONSIBILITY OF THE PRODUCT MANUFACTURER TO MAINTAIN ANY CHANGES AND SUBSEQUENT WHQL
RE-CERTIFICATION AS A RESULT OF MAKING THESE CHANGES.

Author : FTDI

AVI Recorder demo application

Revision History:


*/
#include "FT_Platform.h"
#include "Gpu_CoCmd.h"
#if defined(FT900_PLATFORM)
#include "ff.h"
#else
#include <stdio.h>
#endif

#include "FT_Util.h"
#include "bmp.h"
#include "Common.h"

#if defined(MSVC_PLATFORM) || defined(BT8XXEMU_PLATFORM) 
#define __flash__ 
#define iprintf printf
#endif

#define PLAYBACK_BUFFER_SIZE  8192
#define SCREENSHOT_BUFFER_SIZE (32*1024)
#define SCRATCH_PNGLOAD   0xf5800   // EVE needs this for loading PNGs

//Because the all commands are mutual exclusive so we can share this buffer between screenshot and pngload
#define SCRATCH_SCREENSHOT     (0x100000 - SCREENSHOT_BUFFER_SIZE)

#define SCRATCH_VIDEO_BUF (SCRATCH_PNGLOAD - 0x10000)
#define SCRATCH_AUDIO_BUF (SCRATCH_VIDEO_BUF - PLAYBACK_BUFFER_SIZE)
#define USABLE_GRAM       SCRATCH_AUDIO_BUF

#ifdef BUFFER_OPTIMIZATION
ft_uint8_t  Ft_DlBuffer[FT_DL_SIZE];
ft_uint8_t  Ft_CmdBuffer[FT_CMD_FIFO_SIZE];
#endif

#if defined(FT900_PLATFORM)
#define F_SIZE(_file, _sz) do{ _sz = f_size(_file); }while(0)
#else
#define F_SIZE(_file, _sz) do{\
	ft_int32_t _curr = ftell(_file);\
    fseek(_file, 0L, SEEK_END);\
    _sz = ftell(_file);\
    fseek(_file, _curr, SEEK_SET);\
}while(0)
#endif

const __flash__ ft_uint8_t FT_DLCODE_BOOTUP[12] =
{
  0,0,0,2,//GPU instruction CLEAR_COLOR_RGB
  7,0,0,38, //GPU instruction CLEAR
  0,0,0,0,  //GPU instruction DISPLAY
};

#ifdef BT815A_VALIDATION_800x200
ft_int16_t FT_DispWidth      = 800L;
ft_int16_t FT_DispHeight     = 200L;
ft_int16_t FT_DispHCycle     = 880L;
ft_int16_t FT_DispHOffset    = 46L ;
ft_int16_t FT_DispHSync0     = 0L  ;
ft_int16_t FT_DispHSync1     = 3L  ;
ft_int16_t FT_DispVCycle     = 224L;
ft_int16_t FT_DispVOffset    = 23L ;
ft_int16_t FT_DispVSync0     = 0L  ;
ft_int16_t FT_DispVSync1     = 3L  ;
ft_int16_t FT_DispPCLK       = 2L  ;
ft_int16_t FT_DispSwizzle    = 0L  ;
ft_int16_t FT_DispPCLKPol    = 1L  ;
ft_int16_t FT_DispCSpread    = 0L  ;
ft_int16_t FT_DispDither     = 0   ;
ft_int16_t FT_DispOutbits    = 0L;//(RGB 888)
#elif BT815A_VALIDATION_VM816CU      
ft_int16_t FT_DispWidth      = 480L;
ft_int16_t FT_DispHeight     = 272L;
ft_int16_t FT_DispHCycle     = 548L;
ft_int16_t FT_DispHOffset    = 43L ;
ft_int16_t FT_DispHSync0     = 0L  ;
ft_int16_t FT_DispHSync1     = 41L ;
ft_int16_t FT_DispVCycle     = 292L;
ft_int16_t FT_DispVOffset    = 12L ;
ft_int16_t FT_DispVSync0     = 0L  ;
ft_int16_t FT_DispVSync1     = 10L ;
ft_int16_t FT_DispPCLK       = 3   ;
ft_int16_t FT_DispSwizzle    = 0   ;
ft_int16_t FT_DispPCLKPol    = 1   ;
ft_int16_t FT_DispCSpread    = 0   ;
ft_int16_t FT_DispDither     = 0   ;
ft_int16_t FT_DispOutbits    = 0L  ;//(RGB 888)
#else
ft_uint32_t Ft_CmdBuffer_Index;
ft_int16_t FT_DispWidth = 480;
ft_int16_t FT_DispHeight = 272;
ft_int16_t FT_DispHCycle = 548;
ft_int16_t FT_DispHOffset = 43;
ft_int16_t FT_DispHSync0 = 0;
ft_int16_t FT_DispHSync1 = 41;
ft_int16_t FT_DispVCycle = 292;
ft_int16_t FT_DispVOffset = 12;
ft_int16_t FT_DispVSync0 = 0;
ft_int16_t FT_DispVSync1 = 10;
ft_uint8_t FT_DispPCLK = 5;
ft_char8_t FT_DispSwizzle = 0;
ft_char8_t FT_DispPCLKPol = 1;
ft_char8_t FT_DispCSpread = 1;
ft_char8_t FT_DispDither = 1;
#endif

ft_uint32_t Ft_CmdBuffer_Index;
ft_uint32_t Ft_DlBuffer_Index;

#ifdef BUFFER_OPTIMIZATION
ft_uint8_t  Ft_DlBuffer[FT_DL_SIZE];
ft_uint8_t  Ft_CmdBuffer[FT_CMD_FIFO_SIZE];
#endif

#if defined(FT900_PLATFORM)
#define F_READ(f,b,s,ret) f_read((f), b, s, &ret)
#define F_CLOSE(f) f_close(f)
#define F_SEEK(f,pos) f_lseek((f), pos)
#define F_TELL(f) f_tell((f))
#else
#define F_READ(f,b,s,ret) (ret = fread(b, 1, s, (f)))
#define F_CLOSE(f) fclose(f)
#define F_SEEK(f,pos) fseek(f, pos, SEEK_SET)
#define F_TELL(f) ftell(f)
#endif

void FT_Util_StartApp(Ft_Gpu_Hal_Context_t *pHalContext)
{
    /* It is optional to clear the screen here*/
    Ft_Gpu_Hal_WrMem(pHalContext, RAM_DL,(ft_uint8_t *)FT_DLCODE_BOOTUP,sizeof(FT_DLCODE_BOOTUP));
    Ft_Gpu_Hal_Wr8(pHalContext, REG_DLSWAP,DLSWAP_FRAME);
	Ft_Gpu_Hal_Sleep(100); //Show the booting up screen.
#ifdef FT900_PLATFORM
	/* Initialize the context valriables */
//	pHalContext->hal_config.channel_no = 0;
//	pHalContext->hal_config.pdn_pin_no = FT800_PD_N;
//	pHalContext->hal_config.spi_cs_pin_no = FT800_SEL_PIN;
//
//	pHalContext->cmd_fifo_wp = 0;
//	pHalContext->spinumdummy = 2;//by default ft800/801/810/811 goes with single dummy byte for read
//	pHalContext->spichannel = 2;
//	pHalContext->status = FT_GPU_HAL_OPENED;
//	pHalContext->cmd_fifo_wp = Ft_Gpu_Hal_Rd16(pHalContext,REG_CMD_WRITE);
#endif
}

void FT_Util_ExitApp(Ft_Gpu_Hal_Context_t *pHalContext)
{
    /* To ensiure graceful exit is done and also gui manager expects cmd_dlstart */
    Ft_Gpu_Hal_WrMem(pHalContext, RAM_DL,(ft_uint8_t *)FT_DLCODE_BOOTUP,sizeof(FT_DLCODE_BOOTUP));
    Ft_Gpu_Hal_Wr8(pHalContext, REG_DLSWAP,DLSWAP_FRAME);
	Gpu_CoCmd_Dlstart(pHalContext);
}

#define EVE_RESET_COPROCESSORRELEASEALL		(0x0000)
#define EVE_RESET_COPROCESSOR_J1			(0x0001)
#define EVE_RESET_COPROCESSOR_JT			(0x0002)
#define EVE_RESET_COPROCESSOR_JA			(0x0004)

/* Error recovery of coprocessor J1 */
ft_int32_t Ft_CoProErrorRecovery(Ft_Gpu_Hal_Context_t *pHalContext)
{
	Ft_Gpu_Hal_Wr16(pHalContext, REG_CPURESET,EVE_RESET_COPROCESSOR_J1);
	ft_delay(1);//safer side
	Ft_Gpu_Hal_Wr16(pHalContext, REG_CMD_READ,0);
	Ft_Gpu_Hal_Wr16(pHalContext, REG_CMD_WRITE,0);
	Ft_Gpu_Hal_Wr16(pHalContext, REG_CPURESET,EVE_RESET_COPROCESSORRELEASEALL);

	/* TODO - datapath flush, mediafifo and audio PCM buffer flush */
	return 0;
}

#define EF_DETECT_TIME_OUT              (200000)
/* SDHC. */
#define GPIO_SD_CLK                     (19)
#define GPIO_SD_CMD                     (20)
#define GPIO_SD_DAT3                    (21)
#define GPIO_SD_DAT2                    (22)
#define GPIO_SD_DAT1                    (23)
#define GPIO_SD_DAT0                    (24)
#define GPIO_SD_CD                      (25)
#define GPIO_SD_WP                      (26)
#define EF_DETECT_TIME_OUT              (200000)

void init_fatfs(void)
{
#if defined(FT900_PLATFORM)
	//TODO: init fat fs
	static FATFS FatFs;                            //FatFS volume
	int ret __attribute__((__unused__));
	/* Start up the SD Card */
	sys_enable(sys_device_sd_card);

	ret = 0;
	gpio_function(GPIO_SD_CLK, pad_sd_clk);
	gpio_pull(GPIO_SD_CLK, pad_pull_none);
	gpio_function(GPIO_SD_CMD, pad_sd_cmd);
	gpio_pull(GPIO_SD_CMD, pad_pull_pullup);
	gpio_function(GPIO_SD_DAT3, pad_sd_data3);
	gpio_pull(GPIO_SD_DAT3, pad_pull_pullup);
	gpio_function(GPIO_SD_DAT2, pad_sd_data2);
	gpio_pull(GPIO_SD_DAT2, pad_pull_pullup);
	gpio_function(GPIO_SD_DAT1, pad_sd_data1);
	gpio_pull(GPIO_SD_DAT1, pad_pull_pullup);
	gpio_function(GPIO_SD_DAT0, pad_sd_data0);
	gpio_pull(GPIO_SD_DAT0, pad_pull_pullup);
	gpio_function(GPIO_SD_CD, pad_sd_cd);
	gpio_pull(GPIO_SD_CD, pad_pull_pullup);
	gpio_function(GPIO_SD_WP, pad_sd_wp);
	gpio_pull(GPIO_SD_WP, pad_pull_pullup);

	sdhost_init();
	int time_out = EF_DETECT_TIME_OUT;
	while (sdhost_card_detect() != SDHOST_CARD_INSERTED) {
		if (!--time_out) {
			break;
		}
	}
	if (time_out) {
		iprintf("SD Card Inserted\r\n");
		if (f_mount(&FatFs, "", 1) != FR_OK) {
			iprintf("Unable to mount File System\r\n");
			ret = -1;
		}
	} else {
		iprintf("SD Card Detection Time-out\r\n");
		ret = -1;
	}
#endif //FT900_PLATFORM
}

#if defined(FT900_PLATFORM)
ft_uint8_t f_getc(FIL *file);
ft_uint16_t be16(FIL *file);
#elif defined(MSVC_PLATFORM) || defined(BT8XXEMU_PLATFORM) 
ft_uint8_t f_getc(FILE *file);
ft_uint16_t be16(FILE *file);
#endif
int adpcm(void);

struct GuiInternal_t {
	GuiManager ops;
	Ft_Gpu_Hal_Context_t *pdirect;
#define MASK(h)       (1 << (h & 7))
#define NFILES        8
	void* phandle[NFILES];
	ft_uint32_t fatfsHandle;
	struct {
#if defined(FT900_PLATFORM)
		FIL _file;
		FIL* file;
#else
		FILE *file;
#endif
		ft_uint8_t format;
		ft_uint8_t bps;
		ft_uint8_t mode;   // 1 means loop
		ft_uint8_t channels;
		int nsamples;
		int wp;
		int atsample;
		int samplerate;

		ft_uint32_t dataOfs;
		ft_uint32_t prev_rdptr;        // Previous value of REG_PLAYBACK_READPTR
		ft_uint32_t played_samples;    // How many samples has hw played?
		ft_uint32_t started_samples;    // How many samples has hw played?
		ft_uint32_t last_sample;       // When will this audio file end?

		int nBlockAlign;
		int adpcm;
		int adpcmPos;
		int adpcmNibble;
		int adpcmIndex, adpcmPred;
//BIT
#define FT_AUDIO_FLAGS_BIT_PLAYING  0

#define FT_AUDIO_FLAGS_PLAYING  BIT(FT_AUDIO_FLAGS_BIT_PLAYING)
		ft_uint32_t flags;
	} audio;
} g_GuiInternal;

static void service_playback(void)
{
	UINT bytesread;

	unsigned rp = Ft_Gpu_Hal_Rd32(g_GuiInternal.pdirect, REG_PLAYBACK_READPTR) - SCRATCH_AUDIO_BUF;
	unsigned fullness = (g_GuiInternal.audio.wp - rp) & (PLAYBACK_BUFFER_SIZE - 1);

	if (!test_bit(g_GuiInternal.audio.flags, FT_AUDIO_FLAGS_BIT_PLAYING)) {
		return;
	}
	g_GuiInternal.audio.played_samples += (rp - g_GuiInternal.audio.prev_rdptr) & (PLAYBACK_BUFFER_SIZE - 1);
	g_GuiInternal.audio.prev_rdptr = rp;

	if (((ft_int32_t)(g_GuiInternal.audio.played_samples - g_GuiInternal.audio.last_sample) >= 0)
			&& (rp >= (unsigned) g_GuiInternal.audio.wp)) {
		g_GuiInternal.audio.started_samples = g_GuiInternal.audio.last_sample;
		g_GuiInternal.audio.last_sample += g_GuiInternal.audio.nsamples;
		if ((g_GuiInternal.audio.mode == PLAY_ONCE || g_GuiInternal.audio.mode == PLAY_WAIT)) {
			//disable_speaker(AUDIO_SOUND);
			Ft_Gpu_Hal_Wr32(g_GuiInternal.pdirect, REG_PLAYBACK_FREQ, 100);
			Gpu_CoCmd_MemSet(g_GuiInternal.pdirect, SCRATCH_AUDIO_BUF, 0x00000000,
			PLAYBACK_BUFFER_SIZE);
			g_GuiInternal.audio.mode = 255;
		}
	}

	while (fullness < (PLAYBACK_BUFFER_SIZE - 1)) {
		ft_uint8_t sample = 0;

		if (test_bit(g_GuiInternal.audio.flags, FT_AUDIO_FLAGS_BIT_PLAYING)) {
			if (g_GuiInternal.audio.adpcm) {
				// FILE *f = fopen("dump", "w");
				// while (++audio.atsample != audio.nsamples) {
				//   int16_t s = adpcm();
				//   fwrite(&s, 1, sizeof(s), f);
				// }
				// fclose(f);
				// assert(0);
				sample = adpcm() >> 8;
			} else if (g_GuiInternal.audio.bps == 16) {
				ft_int16_t s16[2];
				if (g_GuiInternal.audio.channels == 1) {
					//cmd_FRead(audio.f, (uint8_t*) &s16[0], sizeof(s16[0]), bytesread);
					F_READ(g_GuiInternal.audio.file, (ft_uint8_t*)&s16[0], sizeof(s16[0]), bytesread);
					sample = (s16[0] >> 8);
				} else {
					//cmd_FRead(audio.f, (uint8_t*) &s16, sizeof(s16), bytesread);
					F_READ(g_GuiInternal.audio.file, (ft_uint8_t*) &s16, sizeof(s16), bytesread);
					sample = (s16[0] + s16[1]) >> 9;
				}
			} else if ((g_GuiInternal.audio.format == LINEAR_SAMPLES) && (g_GuiInternal.audio.bps == 8)) {
				ft_uint8_t u8[2];
				if (g_GuiInternal.audio.channels == 1) {
					//cmd_FRead(audio.f, (uint8_t*) &u8[0], sizeof(u8[0]), bytesread);
					F_READ(g_GuiInternal.audio.file, (ft_uint8_t*) &u8[0], sizeof(u8[0]), bytesread);
					sample = u8[0];
				} else {
					//cmd_FRead(audio.f, (uint8_t*) &u8, sizeof(u8), bytesread);
					F_READ(g_GuiInternal.audio.file, (ft_uint8_t*) &u8, sizeof(u8), bytesread);
					sample = (u8[0] + u8[1]) >> 1;
				}
				sample -= 128;
			} else if (g_GuiInternal.audio.format == ULAW_SAMPLES) {
				ft_uint8_t u8[2];
				//cmd_FRead(audio.f, (uint8_t*) &u8, audio.channels, bytesread);
				F_READ(g_GuiInternal.audio.file, (ft_uint8_t*) &u8, g_GuiInternal.audio.channels, bytesread);
				sample = u8[0];
			} else {
				sample = 0;
			}
			if (++g_GuiInternal.audio.atsample == g_GuiInternal.audio.nsamples) {
				if (g_GuiInternal.audio.mode == PLAY_LOOP) {
					//cmd_FSeek(audio.f, audio.dataOfs);
					F_SEEK(g_GuiInternal.audio.file, g_GuiInternal.audio.dataOfs);
					g_GuiInternal.audio.adpcmPos = 0;
					g_GuiInternal.audio.adpcmNibble = 0;
					g_GuiInternal.audio.atsample = 0;
				} else {
					//cmd_FClose(audio.f);
					F_CLOSE(g_GuiInternal.audio.file);
					clear_bit(g_GuiInternal.audio.flags, FT_AUDIO_FLAGS_BIT_PLAYING);
				}
			}
		}
		Ft_Gpu_Hal_Wr8(g_GuiInternal.pdirect, SCRATCH_AUDIO_BUF + g_GuiInternal.audio.wp, sample);
		g_GuiInternal.audio.wp = (g_GuiInternal.audio.wp + 1) & (PLAYBACK_BUFFER_SIZE - 1);
		fullness++;
	}
}

static void cmd_AudioStop(void)
{
	if (g_GuiInternal.audio.flags & FT_AUDIO_FLAGS_PLAYING) {
		F_CLOSE(g_GuiInternal.audio.file);
		//Clear bit
		clear_bit(g_GuiInternal.audio.flags, FT_AUDIO_FLAGS_BIT_PLAYING);
		g_GuiInternal.audio.wp = Ft_Gpu_Hal_Rd32(g_GuiInternal.pdirect, REG_PLAYBACK_READPTR) - SCRATCH_AUDIO_BUF;
		Ft_Gpu_Hal_Wr32(g_GuiInternal.pdirect, REG_PLAYBACK_FREQ, 100);
		Gpu_CoCmd_MemSet(g_GuiInternal.pdirect, SCRATCH_AUDIO_BUF, 0x00000000, PLAYBACK_BUFFER_SIZE);
		//disable_speaker(AUDIO_SOUND);
	}
}

static void end_audio(void)
{
	/* Stop the audio playback */
	cmd_AudioStop();

	/* Disable JT by writting the length register to 0 */
	Ft_Gpu_Hal_Wr32(g_GuiInternal.pdirect, REG_PLAYBACK_LENGTH, ZERO);
	Ft_Gpu_Hal_Wr32(g_GuiInternal.pdirect, REG_PLAYBACK_PLAY, 1);
}

static void init_audio(void)
{
	/* Initialize audio context */
	g_GuiInternal.audio.flags = 0x0;
	g_GuiInternal.audio.wp = 0;
	g_GuiInternal.audio.prev_rdptr = 0;
	g_GuiInternal.audio.played_samples = 0;
	g_GuiInternal.audio.last_sample = 0;
	g_GuiInternal.audio.format = LINEAR_SAMPLES;       //TBD - default shall be NONE
	g_GuiInternal.audio.mode = PLAY_ONCE;       //TBD - change this to -1

	/* Set the audio buffer to 0 */
	Gpu_CoCmd_MemSet(g_GuiInternal.pdirect, SCRATCH_AUDIO_BUF, 0x00000000,
			PLAYBACK_BUFFER_SIZE);

	/* Init audio engine registers */
	Ft_Gpu_Hal_Wr32(g_GuiInternal.pdirect, REG_PLAYBACK_START, SCRATCH_AUDIO_BUF);
	Ft_Gpu_Hal_Wr32(g_GuiInternal.pdirect, REG_PLAYBACK_LENGTH, PLAYBACK_BUFFER_SIZE);
	Ft_Gpu_Hal_Wr32(g_GuiInternal.pdirect, REG_PLAYBACK_READPTR, SCRATCH_AUDIO_BUF);
	Ft_Gpu_Hal_Wr32(g_GuiInternal.pdirect, REG_PLAYBACK_FORMAT, ULAW_SAMPLES);
	Ft_Gpu_Hal_Wr32(g_GuiInternal.pdirect, REG_PLAYBACK_LOOP, 1);
	Ft_Gpu_Hal_Wr32(g_GuiInternal.pdirect, REG_PLAYBACK_FREQ, 100);
	service_playback();
	Ft_Gpu_Hal_Wr32(g_GuiInternal.pdirect, REG_PLAYBACK_PLAY, 1);
}

static int audio_played_samples(void)
{
	return (int)(g_GuiInternal.audio.played_samples - g_GuiInternal.audio.started_samples);
}

static int cmd_AudioPlay(const char* Filename, ft_int16_t mode) {
	ft_uint32_t pos;
	int res;

	cmd_AudioStop();
#if defined(FT900_PLATFORM)
	res = f_open(g_GuiInternal.audio.file, Filename, FA_READ);
	if (res != FR_OK)
		return -1;
#else
	g_GuiInternal.audio.file = fopen(Filename, "rb");
	if (g_GuiInternal.audio.file == NULL) return -1;
#endif

	ft_uint8_t hdr[36];
	//int16_t bytesread;
	UINT bytesread;
	F_READ(g_GuiInternal.audio.file, hdr, sizeof(hdr), bytesread);
	if ((bytesread != sizeof(hdr)) || (memcmp(hdr + 8, "WAVE", 4) != 0)) {
		F_CLOSE(g_GuiInternal.audio.file);
		return -1;
	}

	int subchunk1size = *(ft_uint32_t*) (hdr + 16);
	g_GuiInternal.audio.samplerate = *(ft_uint32_t*) (hdr + 24);
	g_GuiInternal.audio.nBlockAlign = hdr[32] + (hdr[33] << 8);
	switch (hdr[20]) {
	case 0x01:
		g_GuiInternal.audio.format = LINEAR_SAMPLES;
		g_GuiInternal.audio.adpcm = 0;
		break;
	case 0x07:
		g_GuiInternal.audio.format = ULAW_SAMPLES;
		g_GuiInternal.audio.adpcm = 0;
		break;
	case 0x11:
		g_GuiInternal.audio.format = LINEAR_SAMPLES;
		g_GuiInternal.audio.adpcm = 1;
		break;
	default:
		F_CLOSE(g_GuiInternal.audio.file);
		return -1;
	}
	g_GuiInternal.audio.channels = hdr[22];
	g_GuiInternal.audio.bps = hdr[34];

	//cmd_FTell(f, pos);
	pos = F_TELL(g_GuiInternal.audio.file);
	//cmd_FSeek(f, pos + (subchunk1size - 16));
	F_SEEK(g_GuiInternal.audio.file, pos + (subchunk1size - 16));

	struct {
		char id[4];
		ft_uint32_t size;
	} chunk;
	while (//(cmd_FRead(f, (uint8_t*) &chunk, sizeof(chunk), bytesread) == 0)
			(F_READ(g_GuiInternal.audio.file, (ft_uint8_t*) &chunk, sizeof(chunk), bytesread) >=0)
			&& (bytesread == sizeof(chunk))
			&& (memcmp(chunk.id, "data", 4) != 0)) {
		pos = F_TELL(g_GuiInternal.audio.file);
		//cmd_FSeek(f, pos + chunk.size);
		F_SEEK(g_GuiInternal.audio.file, pos + chunk.size);
	}

	if (memcmp(chunk.id, "data", 4) != 0) {
		//cmd_FClose(f);
		F_CLOSE(g_GuiInternal.audio.file);
		return -1;
	}
	//cmd_FTell(f, audio.dataOfs);
	g_GuiInternal.audio.dataOfs = 92;// F_TELL(g_GuiInternal.audio.file);
	if (!g_GuiInternal.audio.adpcm)
		g_GuiInternal.audio.nsamples = (8 * chunk.size) / (g_GuiInternal.audio.channels * g_GuiInternal.audio.bps);
	else {
		int sbp = 1 + 2 * (g_GuiInternal.audio.nBlockAlign - 4);  // samples per block
		int nblocks = chunk.size / g_GuiInternal.audio.nBlockAlign;
		g_GuiInternal.audio.nsamples = g_GuiInternal.audio.channels
						               * ((nblocks * sbp)
								       + 2 * ((chunk.size % g_GuiInternal.audio.nBlockAlign) - 4));
	}
	g_GuiInternal.audio.atsample = 0;
	g_GuiInternal.audio.adpcmPos = 0;
	g_GuiInternal.audio.adpcmNibble = 0;

	g_GuiInternal.audio.started_samples = g_GuiInternal.audio.played_samples;
	g_GuiInternal.audio.last_sample = g_GuiInternal.audio.played_samples + g_GuiInternal.audio.nsamples;

	g_GuiInternal.audio.wp = Ft_Gpu_Hal_Rd32(g_GuiInternal.pdirect, REG_PLAYBACK_READPTR) - SCRATCH_AUDIO_BUF;
	Ft_Gpu_Hal_Wr32(g_GuiInternal.pdirect, REG_PLAYBACK_FREQ, g_GuiInternal.audio.samplerate);
	Ft_Gpu_Hal_Wr32(g_GuiInternal.pdirect, REG_PLAYBACK_FORMAT, g_GuiInternal.audio.format);

	g_GuiInternal.audio.mode = mode;
	//audio.f = f;
	set_bit(g_GuiInternal.audio.flags, FT_AUDIO_FLAGS_BIT_PLAYING);
	//enable_speaker(AUDIO_SOUND);
	service_playback();

	if (g_GuiInternal.audio.mode == PLAY_WAIT) {
		while (g_GuiInternal.audio.mode == PLAY_WAIT)
			service_playback();
		Ft_Gpu_Hal_Wr32(g_GuiInternal.pdirect, REG_PLAYBACK_FREQ, 100);
	}
	return 0;
}

int adpcm(void)
{
	UINT bytesread;

	/* Intel ADPCM step variation table */
	static const int indexTable[16] = { -1, -1, -1, -1, 2, 4, 6, 8, -1, -1, -1,
			-1, 2, 4, 6, 8, };

	static const int stepsizeTable[89] = { 7, 8, 9, 10, 11, 12, 13, 14, 16, 17,
			19, 21, 23, 25, 28, 31, 34, 37, 41, 45, 50, 55, 60, 66, 73, 80, 88,
			97, 107, 118, 130, 143, 157, 173, 190, 209, 230, 253, 279, 307, 337,
			371, 408, 449, 494, 544, 598, 658, 724, 796, 876, 963, 1060, 1166,
			1282, 1411, 1552, 1707, 1878, 2066, 2272, 2499, 2749, 3024, 3327,
			3660, 4026, 4428, 4871, 5358, 5894, 6484, 7132, 7845, 8630, 9493,
			10442, 11487, 12635, 13899, 15289, 16818, 18500, 20350, 22385,
			24623, 27086, 29794, 32767 };

	if (!(g_GuiInternal.audio.adpcmNibble & 0x100)) {
		if ((g_GuiInternal.audio.adpcmPos % g_GuiInternal.audio.nBlockAlign) == 0) {
			struct {
				ft_int16_t predictor;
				ft_uint8_t index;
				ft_uint8_t _dummy;
			} adpcmHdr;
			F_READ(g_GuiInternal.audio.file, (ft_uint8_t*) &adpcmHdr, sizeof(adpcmHdr), bytesread);
			g_GuiInternal.audio.adpcmPos += bytesread;
			g_GuiInternal.audio.adpcmIndex = adpcmHdr.index;
			g_GuiInternal.audio.adpcmPred = adpcmHdr.predictor;
			return g_GuiInternal.audio.adpcmPred;
		}

		g_GuiInternal.audio.adpcmNibble = f_getc(g_GuiInternal.audio.file) | 0x1000;
		g_GuiInternal.audio.adpcmPos++;
	}

	int delta = g_GuiInternal.audio.adpcmNibble & 0xf;
	g_GuiInternal.audio.adpcmNibble >>= 4;

	int step = stepsizeTable[g_GuiInternal.audio.adpcmIndex];

	/* Step 1 - get the delta value and compute next index */

	/* Step 2 - Find new index value (for later) */
	g_GuiInternal.audio.adpcmIndex += indexTable[delta];
	if (g_GuiInternal.audio.adpcmIndex < 0)
		g_GuiInternal.audio.adpcmIndex = 0;
	if (g_GuiInternal.audio.adpcmIndex > 88)
		g_GuiInternal.audio.adpcmIndex = 88;

	/* Step 3 - Separate sign and magnitude */
	int sign = delta & 8;
	delta = delta & 7;

	/* Step 4 - Compute difference and new predicted value */
	/*
	 ** Computes 'vpdiff = (delta+0.5)*step/4', but see comment
	 ** in adpcm_coder.
	 */
	int vpdiff = step >> 3;
	if (delta & 4)
		vpdiff += step;
	if (delta & 2)
		vpdiff += step >> 1;
	if (delta & 1)
		vpdiff += step >> 2;

	if (sign)
		g_GuiInternal.audio.adpcmPred -= vpdiff;
	else
		g_GuiInternal.audio.adpcmPred += vpdiff;

	/* Step 5 - clamp output value */
	if (g_GuiInternal.audio.adpcmPred > 32767)
		g_GuiInternal.audio.adpcmPred = 32767;
	else if (g_GuiInternal.audio.adpcmPred < -32768)
		g_GuiInternal.audio.adpcmPred = -32768;

	/* Step 6 - Update step value */
	step = stepsizeTable[g_GuiInternal.audio.adpcmIndex];

	/* Step 6 - Output value */
	// iprintf("audio.adpcmPred = %04x\n", audio.adpcmPred);
	return g_GuiInternal.audio.adpcmPred;
}

static int audio_seekto(int seek_samples)
{
	if (!(g_GuiInternal.audio.flags & FT_AUDIO_FLAGS_PLAYING) || g_GuiInternal.audio.mode == -1 || seek_samples < 0
			|| seek_samples > g_GuiInternal.audio.nsamples)
		return -1;

	g_GuiInternal.audio.atsample = 0;
	g_GuiInternal.audio.adpcmPos = 0;
	g_GuiInternal.audio.adpcmNibble = 0;
	//TODO: improve this code later.
	F_SEEK(g_GuiInternal.audio.file, g_GuiInternal.audio.dataOfs);
	if (g_GuiInternal.audio.adpcm) {
		while (g_GuiInternal.audio.atsample++ < seek_samples) {
			adpcm();
		}
	} else {
		if (g_GuiInternal.audio.bps == 16) {
			if (g_GuiInternal.audio.channels == 1) {
				F_SEEK(g_GuiInternal.audio.file, 2 * seek_samples);
			} else {
				F_SEEK(g_GuiInternal.audio.file, 4 * seek_samples);
			}
		} else if ((g_GuiInternal.audio.format == LINEAR_SAMPLES) && (g_GuiInternal.audio.bps == 8)) {
			if (g_GuiInternal.audio.channels == 1) {
				F_SEEK(g_GuiInternal.audio.file, seek_samples);
			} else {
				F_SEEK(g_GuiInternal.audio.file, 2 * seek_samples);
			}
		} else if (g_GuiInternal.audio.format == ULAW_SAMPLES) {
			F_SEEK(g_GuiInternal.audio.file, g_GuiInternal.audio.channels * seek_samples);
		}
		g_GuiInternal.audio.atsample += seek_samples;
	}
	g_GuiInternal.audio.last_sample = g_GuiInternal.audio.played_samples + g_GuiInternal.audio.nsamples - seek_samples;
	g_GuiInternal.audio.started_samples = g_GuiInternal.audio.played_samples - seek_samples;
	g_GuiInternal.audio.prev_rdptr = g_GuiInternal.audio.wp = Ft_Gpu_Hal_Rd32(g_GuiInternal.pdirect,
			                                                                  REG_PLAYBACK_READPTR) - SCRATCH_AUDIO_BUF;
	Ft_Gpu_Hal_Wr32(g_GuiInternal.pdirect, REG_PLAYBACK_FREQ, g_GuiInternal.audio.samplerate);
	Ft_Gpu_Hal_Wr32(g_GuiInternal.pdirect, REG_PLAYBACK_FORMAT, g_GuiInternal.audio.format);
	service_playback();
	return 0;
}

#if defined(MSVC_PLATFORM) || defined(BT8XXEMU_PLATFORM) 
ft_uint8_t f_getc(FILE *file)
#else
ft_uint8_t f_getc(FIL *file)
#endif
{
	ft_int8_t buf;
	UINT actual;
	int res;

#if defined(MSVC_PLATFORM) || defined(BT8XXEMU_PLATFORM) 
	res = fread(&buf, 1, 1, file);
	if (res != 1) return -1;
#else
	res = f_read(file, (char*)&buf, 1, &actual);
	if (res != FR_OK)
		return -1;
#endif
	return buf;
}
#if defined(MSVC_PLATFORM) || defined(BT8XXEMU_PLATFORM) 
ft_uint16_t be16(FILE *file)
#else
ft_uint16_t be16(FIL *file)
#endif
{
	ft_uint8_t hi = f_getc(file);
	ft_uint8_t lo = f_getc(file);
	return (hi << 8) + lo;
}

#if defined(MSVC_PLATFORM) || defined(BT8XXEMU_PLATFORM) 
int png_scan(FILE *file, int *has_tRNS, int *palsize)
#else
int png_scan(FIL *file, int *has_tRNS, int *palsize)
#endif
{
	ft_uint32_t pos = 8, total = 0;
	UINT actual;

	*has_tRNS = 0;
	F_SIZE(file, total);
	while (pos < total) {
		struct {
			ft_uint8_t length[4];
			ft_uint8_t typecode[4];
		} chunk;
		F_SEEK(file, pos);
		F_READ(file, (ft_uint8_t*) &chunk, sizeof(chunk), actual);
		if (actual != sizeof(chunk))
			return 0;
		ft_uint32_t length = (chunk.length[0] << 24) | (chunk.length[1] << 16)
				| (chunk.length[2] << 8) | (chunk.length[3] << 0);
		*has_tRNS |= (memcmp(chunk.typecode, "tRNS", 4) == 0);
		if (memcmp(chunk.typecode, "PLTE", 4) == 0)
			*palsize = length / 3;
		pos += (12 + length);
	}
	return 1;
}

#if defined(MSVC_PLATFORM) || defined(BT8XXEMU_PLATFORM) 
static ft_int16_t _raw_draft_image(FILE *file, struct Hbitmap *gm)
#else
static ft_int16_t _raw_draft_image(FIL *file, struct Hbitmap *gm)
#endif
{
	int res;
	UINT bytesread;

	if (F_SEEK(file, 0) != 0) {
		printf("WARN: fail to seek to head!\n");
	}
	ft_uint16_t magic = be16(file);
	switch (magic) {
	case 0x424d:  // .bmp starts with 'B' 'M'
	{
		FILEHEADER fh;
		INFOHEADER ih;

		F_SEEK(file, 0);
		F_READ(file, (ft_uint8_t*) &fh, sizeof(FILEHEADER), bytesread);
		F_READ(file, (ft_uint8_t*) &ih, sizeof(INFOHEADER), bytesread);

		gm->width = ih.width;
		gm->height = ih.height;
		gm->fmt = RGB565;
	}
		break;

	case 0xffd8:  // .jpg
	{
		ft_uint32_t pos;
#if defined(MSVC_PLATFORM) || defined(BT8XXEMU_PLATFORM) 
		{
			int fsz;

			F_SIZE(file, fsz);
			pos = F_TELL(file);
			if (pos != 2) {
				if (F_SEEK(file, 2) != 0) {
					printf("WARN: fail to seek!\n");
				}
				pos = F_TELL(file);
			}
			ft_uint16_t marker;
			while ((marker = be16(file)) != 0xffc0 && pos < fsz) {
				if ((marker & 0xFF00) == 0xFF00) {
					ft_uint16_t o = be16(file);
					pos = F_TELL(file) + o - 2;
					F_SEEK(file, pos);
				}
				else {
					f_getc(file);
				}
			}
			if (marker != 0xffc0) {
				return ERROR_INVALIDFILE;
			}
		}
#else
		while (be16(file) != 0xffc0) {
			ft_uint16_t o = be16(file);
			pos = F_TELL(file);
			F_SEEK(file, pos + o - 2);
		}
#endif
		pos = F_TELL(file);
		F_SEEK(file, pos + 3);
		gm->height = be16(file);
		gm->width = be16(file);
		gm->fmt = RGB565;
	}
		break;

	case 0x8950:  // .png
	{
		F_SEEK(file, 18);
		gm->width = be16(file);
		F_SEEK(file, 22);
		gm->height = be16(file);

		ft_uint8_t bit_depth = f_getc(file);
		ft_uint8_t color_type = f_getc(file);

		if (bit_depth != 8) {
			F_CLOSE(file);
			iprintf("Invalid png");
			return ERROR_INVALIDFILE;
		}
		switch (color_type) {
		case 0:
			gm->fmt = L8;
			break;
		case 2:
			gm->fmt = RGB565;
			break;
		case 3: {
			int has_tRNS, palsize;
			png_scan(file, &has_tRNS, &palsize);
			gm->fmt = has_tRNS ? PALETTED4444 : PALETTED565;
			gm->palsize = (palsize - 1);
			break;
		}
		case 6:
			gm->fmt = ARGB4;
			break;
		default:
			iprintf("NOT supported png file!\n");
			F_CLOSE(file);
			return ERROR_INVALIDFILE;
		}
	}
		break;

	default:
		F_CLOSE(file);
		iprintf("NOT a bmp, jpg, or png");
		return ERROR_INVALIDFILE;
	}
	return 0;
}

ft_int16_t raw_draft_image(const char* filename, struct Hbitmap *gm) 
{
#if defined(FT900_PLATFORM)
	FIL _file;
	FIL *file = &_file;
#else
	FILE *file;
#endif
	int res;

#if defined(FT900_PLATFORM)
	res = f_open(file, filename, FA_READ);
	if (res != FR_OK)
		return -res;
#else
	file = fopen(filename, "r");
	if (file == NULL) return -1;
#endif
	res = _raw_draft_image(file, gm);
	F_CLOSE(file);
	return res;
}

ft_int16_t raw_draft_image_byhandle(int handle, struct Hbitmap *gm)
{
#if defined(MSVC_PLATFORM) || defined(BT8XXEMU_PLATFORM) 
	return _raw_draft_image((FILE*)g_GuiInternal.phandle[handle], gm);
#else
	return _raw_draft_image((FIL*)g_GuiInternal.phandle[handle], gm);
#endif // MSVC_PL
}

/*/////////////////////////////////////// File commands ////////////////////////////
 */

int slot(ft_uint32_t h, int handle_num) {
    // Find the lowest 0 bit in h
	ft_uint32_t sel = ~h & (h + 1);

    if ((sel & ((1 << NFILES) - 1)) == 0)
      return -1;

    int index = handle_num;
    if (sel & 0xf0) index |= 4;
    if (sel & 0xcc) index |= 2;
    if (sel & 0xaa) index |= 1;
    return index;
  }

static ft_int16_t cmd_FOpen(const char* String, int mode)
{
	int h = slot(g_GuiInternal.fatfsHandle, 0);
	if (h < 0)
		return -1;
#if defined(MSVC_PLATFORM) || defined(BT8XXEMU_PLATFORM) 
	FILE *file;
	char* fmode = "rb";

	if (mode == FILE_WRITE) {
		fmode = "wb";
	}
	else if (mode == (FILE_READ | FILE_WRITE)) {
		fmode = "w+b";
	}
	file = fopen(String, fmode);
	if (file == NULL) {
		g_GuiInternal.phandle[h] = NULL;
		return -1;
	}
	else {
		g_GuiInternal.phandle[h] = file;
	}
#else
	FIL *fm = (FIL*)malloc(sizeof(FIL));
	g_GuiInternal.phandle[h] = fm;

	FRESULT res;
	res = f_open(fm, String, mode);
	if (res != FR_OK) {
		free(g_GuiInternal.phandle[h]);
		g_GuiInternal.phandle[h] = NULL;
		return -1;
	}

#endif
	g_GuiInternal.fatfsHandle |= MASK(h);
	return h;
}

ft_int16_t  cmd_FSeek(ft_int16_t Handle, ft_int32_t Offset)
{
#if defined(FT900_PLATFORM)
	return f_lseek((FIL*)g_GuiInternal.phandle[Handle], Offset);
#else
	return fseek((FILE*)g_GuiInternal.phandle[Handle], Offset, SEEK_SET);
#endif
}

ft_int16_t cmd_FRead(ft_int16_t Handle, ft_uint8_t *Buffer, ft_int16_t bytetoread, ft_int16_t *bytesread)
{
	int res;

#if defined(FT900_PLATFORM)
	UINT actual;

	res = f_read((FIL*)g_GuiInternal.phandle[Handle], Buffer, bytetoread, &actual);
	*bytesread = actual;
#else
	res = fread(Buffer, 1, bytetoread, (FILE*)g_GuiInternal.phandle[Handle]);
	if (res > 0) *bytesread = res;
	else *bytesread = 0;
#endif
	return res;
}

ft_int16_t cmd_FClose(ft_int16_t Handle)
{
	int res;

#if defined(FT900_PLATFORM)
	res = f_close((FIL*)g_GuiInternal.phandle[Handle]);
	free(g_GuiInternal.phandle[Handle]);
#else
	if (g_GuiInternal.phandle[Handle] != NULL)
		res = fclose((FILE*)g_GuiInternal.phandle[Handle]);
#endif
	g_GuiInternal.phandle[Handle] = NULL;
	g_GuiInternal.fatfsHandle &= ~MASK(Handle);
	return res;
}

ft_uint16_t cmd_be16(ft_int16_t f)
{
#if defined(FT900_PLATFORM)
	return be16((FIL*)g_GuiInternal.phandle[f]);
#else
	return be16((FILE*)g_GuiInternal.phandle[f]);
#endif
}

void feedfile(ft_int16_t f)
{
	ft_int16_t bytesread;
	ft_uint8_t dd[512];
#if defined(MSVC_PLATFORM) || defined(BT8XXEMU_PLATFORM) 
	ft_uint32_t pos;
	ft_uint32_t sz;

	cmd_FSize(f, &sz);
	cmd_FTell(f, &pos);
	if (pos != 0 && cmd_FSeek(f, 0) != 0) {
		printf("WARN: feedfile fail!\n");
	}
#endif
	
    memset(dd, 0, sizeof(dd));
    do {
#if defined(MSVC_PLATFORM) || defined(BT8XXEMU_PLATFORM) 
#if	1
		bytesread = 0;
		ft_int16_t rdnum = 0;
		do {
			cmd_FRead(f, &dd[bytesread], sizeof(dd) - bytesread, &rdnum);
			bytesread += rdnum;
			cmd_FTell(f, &pos);
		} while (pos < sz && bytesread < sizeof(dd));
#endif
		//cmd_FRead(f, &dd[bytesread], sizeof(dd), &bytesread);
#else
		cmd_FRead(f, dd, sizeof(dd), &bytesread);
#endif
      //direct->sendn(dd, (bytesread + 3) & ~3);
      Ft_Gpu_Hal_WrCmdBuf(g_GuiInternal.pdirect, dd, (bytesread + 3) & ~3);
      service_playback();
    } 
#if defined(FT900_PLATFORM)
	while (!f_eof((FIL*)g_GuiInternal.phandle[f]));
#else
	while (bytesread > 0 && pos < sz);
#endif
}

int cmd_FSize(ft_int16_t Handle, ft_uint32_t *Offset)
{
#if defined(MSVC_PLATFORM) || defined(BT8XXEMU_PLATFORM) 
	//Get current
	ft_int32_t currOffs = ftell((FILE*)g_GuiInternal.phandle[Handle]);
	fseek((FILE*)g_GuiInternal.phandle[Handle], 0L, SEEK_END);
	*Offset = ftell((FILE*)g_GuiInternal.phandle[Handle]);
	fseek((FILE*)g_GuiInternal.phandle[Handle], currOffs, SEEK_SET);
	return 0;
#else
	*Offset = f_size((FIL*)g_GuiInternal.phandle[Handle]);
	return 0;
#endif
}

int cmd_FTell(ft_int16_t Handle, ft_uint32_t *Offset)
{
#if defined(FT900_PLATFORM)
	*Offset = f_tell((FIL*)g_GuiInternal.phandle[Handle]);
	return 0;
#else
	*Offset = ftell((FILE*)g_GuiInternal.phandle[Handle]);
	return 0;
#endif
}

int init_guiobs(Ft_Gpu_Hal_Context_t *pHalContext)
{
	g_GuiInternal.ops.service_playback = &service_playback;
	g_GuiInternal.ops.cmd_AudioPlay = &cmd_AudioPlay;
	g_GuiInternal.ops.cmd_AudioStop = &cmd_AudioStop;
	g_GuiInternal.ops.end_audio = &end_audio;
	g_GuiInternal.ops.raw_draft_image = &raw_draft_image;
	g_GuiInternal.ops.raw_draft_image_byhandle = &raw_draft_image_byhandle;
	g_GuiInternal.ops.audio_played_samples = &audio_played_samples;
	g_GuiInternal.ops.audio_seekto = &audio_seekto;
	g_GuiInternal.ops.init_audio = &init_audio;
	//File commands
	g_GuiInternal.ops.cmd_FOpen = &cmd_FOpen;
	g_GuiInternal.ops.cmd_FClose = &cmd_FClose;
	g_GuiInternal.ops.cmd_FRead = &cmd_FRead;
	g_GuiInternal.ops.cmd_FSeek = &cmd_FSeek;
	g_GuiInternal.ops.feedfile = &feedfile;
	g_GuiInternal.ops.be16 = &cmd_be16;
	g_GuiInternal.ops.cmd_FSize = &cmd_FSize;
	g_GuiInternal.ops.cmd_FTell = &cmd_FTell;

	g_GuiInternal.pdirect = pHalContext;
#if defined(FT900_PLATFORM)
	g_GuiInternal.audio.file = &g_GuiInternal.audio._file;
#endif
	return 0;
}
GuiManager* getGuiInstance(void)
{
	return &g_GuiInternal.ops;
}

#ifdef FT900_ICON_FROM_RAM
const __flash__ ft_uint8_t g_IconData[] = {
    //loadIcon: 173 1152 1047424: fmt 2 48x48 128 ICON_CLOSE
	129, 48,
	0x78, 0xda, 0x63, 0x60, 0x18, 0x89, 0x60, 0x2a, 0x82, 0x19, 0x89, 0x24, 0xcc, 0xf9, 0x2f, 0x00,
	0xc6, 0x64, 0x7d, 0x3f, 0x01, 0x21, 0x3e, 0xff, 0xff, 0x53, 0x18, 0x33, 0xee, 0xff, 0x4f, 0x84,
	0x78, 0xdc, 0x7f, 0x98, 0x06, 0xd6, 0xf7, 0x08, 0x25, 0xc8, 0x3c, 0x84, 0x0a, 0x14, 0x2e, 0xaa,
	0x72, 0x04, 0x1f, 0x4d, 0x39, 0x4c, 0x00, 0x5d, 0x39, 0x4c, 0x04, 0x43, 0x39, 0x44, 0x08, 0x53,
	0x39, 0x44, 0x03, 0x16, 0xe5, 0x60, 0x0d, 0x58, 0x94, 0x83, 0x35, 0x60, 0x53, 0x0e, 0xd2, 0x80,
	0x4d, 0x39, 0x4e, 0x71, 0x5c, 0xe6, 0xe0, 0xb0, 0x17, 0x97, 0x3b, 0x71, 0xf8, 0x0b, 0x57, 0x38,
	0xe0, 0x08, 0x37, 0x5c, 0xe1, 0x8c, 0x23, 0x5e, 0x70, 0xc5, 0x23, 0xae, 0x78, 0xc7, 0x95, 0x4e,
	0x70, 0xa5, 0x2b, 0x5c, 0xe9, 0x70, 0x04, 0x01, 0x00, 0x94, 0xdc, 0x82, 0xd3, 0x20, 0x20, 0x20,

	//loadIcon: 431 1152 1046272: fmt 2 48x48 64 ICON_BACK
	65, 48,
	0x78, 0xda, 0x63, 0x60, 0x18, 0x05, 0xc4, 0x00, 0xf6, 0x02, 0xec, 0xe2, 0xf5, 0xdf, 0xb1, 0x2b,
	0xff, 0xff, 0x1f, 0xab, 0x86, 0xfa, 0xff, 0xff, 0x0c, 0xb0, 0x2b, 0x7f, 0x3c, 0x98, 0x94, 0xa3,
	0x01, 0x98, 0x72, 0xd2, 0xc4, 0x71, 0x99, 0x83, 0xcb, 0x5e, 0x9c, 0xee, 0x1c, 0x58, 0x0d, 0x09,
	0xa4, 0xc4, 0x3b, 0xce, 0x74, 0x32, 0x0a, 0xb0, 0x03, 0x00, 0xb9, 0xbc, 0x74, 0x4d, 0x20, 0x20,

	//loadIcon: 701 1152 1045120: fmt 2 48x48 108 ICON_SDCARD
	109, 48,
	0x78, 0xda, 0x63, 0x60, 0xa0, 0x13, 0x60, 0xff, 0x0f, 0x05, 0xaf, 0x14, 0x50, 0xc4, 0xeb, 0x61,
	0xe2, 0xff, 0x1f, 0x61, 0x55, 0xfe, 0xff, 0xff, 0x2f, 0xec, 0xca, 0xff, 0xff, 0x43, 0x55, 0xce,
	0xc0, 0x00, 0x45, 0xff, 0x51, 0x95, 0x63, 0x13, 0x07, 0x99, 0x8e, 0x4d, 0xbc, 0x1e, 0xbb, 0x38,
	0xd8, 0x31, 0x58, 0xc4, 0xeb, 0xb1, 0x8b, 0x43, 0xdc, 0x8e, 0x29, 0x5e, 0x8f, 0x43, 0xfc, 0xfd,
	0x7f, 0x54, 0x30, 0x2a, 0x3e, 0x34, 0xc5, 0xff, 0x0f, 0x90, 0x38, 0x9a, 0x7b, 0xe0, 0xf9, 0x62,
	0x3d, 0xaa, 0x38, 0x3c, 0x1f, 0xe9, 0xa1, 0x8a, 0xc3, 0xf3, 0x1d, 0xd3, 0x3a, 0x64, 0x61, 0xb4,
	0x7c, 0x4a, 0x23, 0x00, 0x00, 0xa6, 0x1d, 0x21, 0x40, 0x20, 0x20, 0x20,

	//loadIcon: 516 1152 1043968: fmt 2 48x48 88 ICON_EFLASH
	89, 48,
	0x78, 0xda, 0x63, 0x60, 0x18, 0xa4, 0xe0, 0xff, 0x7f, 0x08, 0xa6, 0x85, 0x38, 0xd3, 0xba, 0xff,
	0x70, 0xf0, 0x4a, 0x01, 0x21, 0xae, 0xf7, 0x1f, 0x09, 0x3c, 0x42, 0x88, 0xaf, 0x47, 0x16, 0xff,
	0x85, 0x10, 0x7f, 0x8f, 0x2c, 0xfe, 0x0f, 0xd5, 0x4e, 0xda, 0xb1, 0x41, 0x96, 0x41, 0x48, 0x18,
	0x4d, 0x5d, 0x71, 0x44, 0x98, 0xa1, 0x87, 0x1d, 0x75, 0xc4, 0xe9, 0xe3, 0x7e, 0x5a, 0xb2, 0x71,
	0xc5, 0x3b, 0xae, 0x74, 0x82, 0x2b, 0x5d, 0xe1, 0x4a, 0x87, 0xb4, 0x4e, 0xff, 0x83, 0x08, 0x00,
	0x00, 0xc4, 0x8d, 0x37, 0xa4, 0x20, 0x20, 0x20,

	//loadIcon: 616 1152 1042816: fmt 2 48x48 156 ICON_PICTURE
	157, 48,
	0x78, 0xda, 0x63, 0x60, 0x18, 0x74, 0x80, 0x69, 0xdd, 0x7f, 0x14, 0xf0, 0x4a, 0x01, 0x22, 0xae,
	0xf7, 0x1f, 0x0d, 0x3c, 0x82, 0x88, 0xaf, 0x47, 0x17, 0xff, 0x05, 0x11, 0x7f, 0x8f, 0x2e, 0xfe,
	0x0f, 0x22, 0xfe, 0x1f, 0x03, 0x8c, 0x8a, 0x53, 0x49, 0x7c, 0x26, 0x76, 0xf1, 0x3f, 0x0c, 0xe7,
	0xb1, 0x8a, 0x5f, 0x84, 0xc6, 0x29, 0x92, 0xf8, 0x0c, 0x50, 0x3c, 0x39, 0x30, 0xb0, 0xa2, 0x89,
	0xff, 0x61, 0xd8, 0xff, 0xff, 0xff, 0x77, 0x20, 0x67, 0x3e, 0xaa, 0xf8, 0x45, 0x06, 0x99, 0xff,
	0xff, 0x17, 0x00, 0x39, 0x3c, 0x28, 0xe2, 0x40, 0x13, 0x98, 0xde, 0xff, 0x15, 0x00, 0x25, 0xa6,
	0xf7, 0xc8, 0xe2, 0x20, 0x13, 0xfc, 0x20, 0xc9, 0x26, 0x1e, 0x59, 0x1c, 0x64, 0x02, 0x5b, 0x00,
	0x98, 0xc7, 0x89, 0x24, 0x0e, 0x36, 0x01, 0x06, 0xce, 0x23, 0xc4, 0x1f, 0x21, 0x27, 0x57, 0x3d,
	0x84, 0x78, 0x00, 0xb2, 0x38, 0x2b, 0xb9, 0xe1, 0x89, 0x2b, 0xdd, 0xe2, 0x4a, 0xe7, 0xb8, 0xf2,
	0x05, 0xae, 0x7c, 0x34, 0xa8, 0x00, 0x00, 0xb8, 0x41, 0x29, 0xf0, 0x20,

	//loadIcon: 539 1152 1041664: fmt 2 48x48 96 ICON_AUDIO
	97, 48,
	0x78, 0xda, 0x63, 0x60, 0x18, 0xbc, 0xe0, 0x3f, 0x18, 0x0c, 0x65, 0x71, 0x5c, 0xfe, 0x1a, 0x15,
	0xd7, 0x38, 0xf7, 0xa6, 0x09, 0x8b, 0x38, 0x27, 0x30, 0x20, 0xff, 0x61, 0x11, 0xdf, 0x8f, 0x3d,
	0x8c, 0x39, 0xff, 0x63, 0x17, 0xd7, 0xc7, 0x21, 0xde, 0x8f, 0x43, 0xfc, 0x3c, 0x0e, 0xf1, 0xf7,
	0xf8, 0xc5, 0xff, 0xe1, 0x30, 0xe7, 0x0f, 0x0e, 0x7b, 0x7f, 0xa0, 0x8b, 0xeb, 0x41, 0xc4, 0x1f,
	0xa1, 0x8b, 0x73, 0x40, 0xc4, 0x1b, 0xb0, 0x87, 0xc3, 0x6f, 0xcc, 0xe0, 0xe4, 0x00, 0xba, 0xe8,
	0x5f, 0x03, 0x03, 0xf6, 0x70, 0x1e, 0xc4, 0x59, 0x9c, 0x01, 0x00, 0xe2, 0x13, 0xaf, 0x7e, 0x20,


	//loadIcon: 886 1152 1040512: fmt 2 48x48 216 ICON_SPEAKER
	217, 48,
	0x78, 0xda, 0xcd, 0x93, 0x4d, 0x0e, 0x01, 0x41, 0x10, 0x85, 0x8b, 0x49, 0x48, 0x8c, 0x85, 0x93,
	0xe0, 0x08, 0x8e, 0xc0, 0x11, 0x9c, 0xc4, 0x1c, 0x61, 0x4e, 0xc0, 0x11, 0x88, 0xb0, 0xf5, 0x13,
	0xf6, 0xae, 0x20, 0x2e, 0x20, 0x42, 0x24, 0x32, 0xf4, 0x53, 0x63, 0x74, 0x64, 0xba, 0xab, 0x84,
	0xdd, 0xf4, 0xa2, 0xd3, 0xf9, 0xf2, 0xd2, 0xf5, 0xf7, 0x8a, 0xa8, 0xc8, 0xa7, 0xaf, 0x70, 0xcc,
	0x05, 0x58, 0x65, 0x8e, 0xad, 0xcf, 0x07, 0x29, 0x47, 0xec, 0xc9, 0x41, 0xb4, 0x00, 0x92, 0x86,
	0x2b, 0x67, 0x5e, 0x1e, 0x02, 0x6b, 0x57, 0xce, 0x9c, 0x82, 0x1d, 0x12, 0x57, 0x9e, 0x72, 0xaa,
	0x39, 0x11, 0x58, 0xfe, 0xe2, 0x34, 0xc6, 0xc5, 0x91, 0x67, 0xbc, 0x8e, 0xbb, 0x23, 0x67, 0x3e,
	0xe5, 0x08, 0x47, 0x74, 0xf3, 0x72, 0xe6, 0xa6, 0x4d, 0x34, 0xc2, 0xe4, 0x5d, 0xbc, 0x3d, 0xfc,
	0x3c, 0x10, 0x35, 0x71, 0xf2, 0x39, 0x27, 0x19, 0xe2, 0xea, 0x73, 0x74, 0xa8, 0x82, 0x9b, 0xc0,
	0x63, 0x0a, 0x6c, 0x42, 0x39, 0xbe, 0xa2, 0x12, 0x1e, 0x02, 0xdf, 0xf3, 0x65, 0xfe, 0xe0, 0xda,
	0x3f, 0x5a, 0x5c, 0x25, 0x4f, 0xad, 0x2e, 0xee, 0x43, 0xcb, 0xf6, 0x41, 0xeb, 0xdb, 0xb7, 0x3e,
	0x6b, 0x73, 0xf9, 0xcc, 0x71, 0x89, 0xb3, 0x34, 0xf7, 0x10, 0x88, 0x7e, 0xf2, 0x89, 0xe6, 0x2b,
	0xd5, 0x87, 0x99, 0x6f, 0x4d, 0x24, 0xfb, 0x7c, 0x23, 0xee, 0x85, 0x99, 0xc9, 0xfb, 0xd2, 0x2b,
	0xec, 0x86, 0x3f, 0x01, 0x7c, 0xe4, 0x0d, 0x4f,

	//loadIcon: 630 1152 1039360: fmt 2 48x48 228 ICON_PLAY
	229, 48,
	0x78, 0xda, 0x63, 0x60, 0xa0, 0x13, 0x30, 0x5f, 0x73, 0xef, 0xed, 0xa9, 0x62, 0x74, 0x51, 0xc6,
	0xde, 0xff, 0x60, 0x70, 0x43, 0x00, 0x55, 0x78, 0xde, 0x7f, 0x28, 0x78, 0x81, 0x22, 0x11, 0xf7,
	0x1f, 0x0e, 0xae, 0x22, 0x09, 0x73, 0xfe, 0x47, 0x02, 0x0d, 0x08, 0xf1, 0xf5, 0xc8, 0xe2, 0xbf,
	0xe0, 0xc2, 0x5c, 0xff, 0x51, 0xc0, 0x02, 0x98, 0xf8, 0x7c, 0x54, 0xf1, 0x1f, 0x50, 0x61, 0xd6,
	0xff, 0x68, 0x20, 0x00, 0x22, 0x2e, 0x87, 0x2e, 0x7e, 0x11, 0xab, 0x31, 0x30, 0x83, 0x18, 0xdf,
	0x43, 0x78, 0x08, 0x3f, 0xfc, 0x03, 0xfb, 0x8d, 0x03, 0xca, 0x63, 0x3a, 0x0f, 0x97, 0x28, 0x00,
	0x89, 0xf3, 0x42, 0x39, 0x48, 0xbe, 0xbb, 0x00, 0x12, 0xb7, 0x87, 0x89, 0x23, 0x42, 0xe3, 0x33,
	0x48, 0xbc, 0x1e, 0x2e, 0x0e, 0x37, 0xe9, 0x3b, 0x72, 0x20, 0x20, 0x85, 0x13, 0x38, 0x28, 0xce,
	0x23, 0xc4, 0x61, 0x26, 0xfd, 0x01, 0xb1, 0xef, 0x23, 0x89, 0x43, 0x4d, 0xfa, 0x0b, 0x62, 0xbf,
	0x47, 0x12, 0x87, 0x9a, 0xf4, 0x0f, 0x9f, 0x38, 0x2e, 0x73, 0x70, 0xd9, 0x8b, 0xcb, 0x9d, 0xb8,
	0xfc, 0x85, 0x2b, 0x1c, 0x70, 0x85, 0x1b, 0xae, 0x70, 0xc6, 0x15, 0x2f, 0x0c, 0xfd, 0xd8, 0xe3,
	0x91, 0x41, 0x16, 0x47, 0xbc, 0xe3, 0x4a, 0x27, 0xe8, 0x06, 0xfd, 0x20, 0x94, 0x0e, 0x71, 0xa5,
	0x5b, 0xb8, 0x17, 0xd0, 0xd3, 0x39, 0x43, 0x2c, 0xf6, 0x7c, 0xc1, 0xc0, 0xd8, 0x87, 0x3d, 0x1f,
	0x31, 0x30, 0xd6, 0x42, 0x84, 0xaf, 0x0b, 0x10, 0x97, 0x4f, 0x69, 0x06, 0x00, 0x62, 0x9e, 0x34,
	0x97, 0x20, 0x20, 0x20,

	//loadIcon: 581 1152 1038208: fmt 2 48x48 212 ICON_PAUSE
	213, 48,
	0x78, 0xda, 0x63, 0x60, 0xa0, 0x13, 0x30, 0x5f, 0x73, 0xef, 0xed, 0xa9, 0x62, 0x74, 0x51, 0xc6,
	0xde, 0xff, 0x60, 0x70, 0x43, 0x00, 0x55, 0x78, 0xde, 0x7f, 0x28, 0x78, 0x81, 0x22, 0x11, 0xf7,
	0x1f, 0x0e, 0xae, 0x22, 0x09, 0x73, 0xfe, 0x47, 0x02, 0x0d, 0x08, 0xf1, 0xf5, 0xc8, 0xe2, 0xbf,
	0xe0, 0xc2, 0x5c, 0xff, 0x51, 0xc0, 0x02, 0x98, 0xf8, 0x7c, 0x54, 0xf1, 0x1f, 0x50, 0x61, 0xd6,
	0xff, 0x68, 0x20, 0x00, 0x22, 0x2e, 0x87, 0x2e, 0x7e, 0x11, 0xab, 0x31, 0x30, 0x83, 0x18, 0xdf,
	0xa3, 0x8b, 0xff, 0x03, 0xfb, 0x8d, 0x03, 0xc2, 0x61, 0x60, 0x80, 0x60, 0x10, 0x28, 0x00, 0x89,
	0xf3, 0x62, 0x8a, 0x5f, 0x00, 0x89, 0xdb, 0x63, 0x8a, 0x7f, 0x06, 0x89, 0xd7, 0x63, 0x8a, 0x7f,
	0x47, 0x0a, 0x04, 0x64, 0x71, 0x70, 0x50, 0x9c, 0xc7, 0x14, 0xff, 0x03, 0x12, 0xbf, 0x8f, 0x29,
	0xfe, 0x17, 0x24, 0xfe, 0x1e, 0x53, 0xfc, 0x1f, 0x3e, 0x71, 0x5c, 0xe6, 0xe0, 0xb2, 0x17, 0x97,
	0x3b, 0x71, 0xf9, 0x0b, 0x57, 0x38, 0xe0, 0x0a, 0x37, 0x5c, 0xe1, 0x8c, 0x2b, 0x5e, 0x18, 0xfa,
	0xb1, 0xc7, 0x23, 0x83, 0x2c, 0x8e, 0x78, 0xc7, 0x95, 0x4e, 0xd0, 0x0d, 0xfa, 0x41, 0x28, 0x1d,
	0xe2, 0x4a, 0xb7, 0x30, 0x2f, 0x60, 0xa4, 0x73, 0x86, 0x58, 0xec, 0xf9, 0x82, 0x81, 0xb1, 0x0f,
	0x7b, 0x3e, 0x62, 0x60, 0xac, 0x85, 0x08, 0x5f, 0x17, 0x20, 0x2e, 0x9f, 0xd2, 0x0c, 0x00, 0x00,
	0x60, 0x88, 0x2f, 0x45,

	//loadIcon: 481 648 1037560: fmt 2 36x36 56 ICON_VIDEO
	57, 36,
	0x78, 0xda, 0x63, 0x60, 0xa0, 0x1a, 0x28, 0x2f, 0x60, 0x28, 0x07, 0x03, 0x06, 0xf6, 0x72, 0x88,
	0xc8, 0xff, 0x0f, 0x0c, 0xff, 0xc1, 0x80, 0x81, 0xff, 0x3f, 0x4e, 0x91, 0xef, 0xe5, 0x10, 0x91,
	0xf2, 0x7a, 0x98, 0x08, 0x12, 0xc0, 0x29, 0x82, 0xa9, 0x6b, 0xd4, 0x2e, 0xba, 0xd8, 0x85, 0x19,
	0xcb, 0x54, 0x01, 0x00, 0xd4, 0x7a, 0x14, 0x94,

	//loadIcon: 685 1152 1035904: fmt 2 48x48 268 ICON_ROTATE
	255,  14, 48,
	0x78, 0xda, 0xdd, 0x92, 0xbd, 0x0d, 0xc2, 0x30, 0x10, 0x85, 0x2f, 0x08, 0x44, 0x88, 0x44, 0x94,
	0x11, 0x68, 0xd9, 0x82, 0x92, 0x92, 0x0d, 0xb2, 0x02, 0x1b, 0x00, 0x12, 0x1d, 0x45, 0x22, 0x16,
	0x60, 0x02, 0x7a, 0xe8, 0xd8, 0x00, 0xc4, 0x02, 0x61, 0x83, 0x34, 0x20, 0x40, 0x10, 0x3f, 0xfc,
	0x43, 0x70, 0xe2, 0xc4, 0x05, 0x94, 0x5c, 0x61, 0x47, 0xdf, 0xbd, 0xd8, 0xe7, 0x77, 0x47, 0xf4,
	0x7b, 0xb4, 0x2d, 0x7c, 0x62, 0x91, 0xc3, 0x22, 0x87, 0x45, 0x0e, 0x8b, 0x1c, 0x16, 0x39, 0xb6,
	0xa3, 0x5a, 0x39, 0xc0, 0x96, 0x75, 0x72, 0x91, 0x98, 0x1a, 0xdc, 0x53, 0x1c, 0xcf, 0xc0, 0x48,
	0xac, 0x38, 0x1c, 0x26, 0xc0, 0xd1, 0xe0, 0x1d, 0xce, 0xa9, 0x95, 0x20, 0xeb, 0x19, 0xc6, 0xac,
	0x44, 0x9d, 0x3c, 0xbb, 0x2b, 0xd4, 0x12, 0xab, 0x1f, 0xf8, 0x1a, 0xe1, 0xf6, 0xc1, 0x8d, 0xf4,
	0x4e, 0x12, 0xc9, 0x2c, 0xeb, 0xe9, 0xa3, 0x33, 0xb1, 0xb9, 0xf2, 0xbd, 0x09, 0xe2, 0x9c, 0xfb,
	0x38, 0xcb, 0x3d, 0x12, 0x4b, 0x88, 0x53, 0xce, 0x43, 0x1c, 0xf4, 0xf5, 0x5d, 0x5c, 0x74, 0xe9,
	0x63, 0x5d, 0x83, 0xab, 0x2f, 0xde, 0x63, 0xa0, 0x79, 0x13, 0x8f, 0x5a, 0xbb, 0x1d, 0x55, 0x44,
	0x35, 0xc0, 0xbe, 0xe2, 0x85, 0x73, 0x6c, 0xf7, 0xda, 0xea, 0xcc, 0xdf, 0x45, 0xc6, 0xbb, 0x7c,
	0xfd, 0x59, 0xf2, 0xc1, 0x2d, 0x96, 0x9c, 0xe0, 0xd3, 0x62, 0x47, 0xf9, 0xfc, 0xee, 0x33, 0x0b,
	0xca, 0x7d, 0x51, 0x11, 0xe1, 0x5a, 0x37, 0xe0, 0x5e, 0xa9, 0x8f, 0x3c, 0xd6, 0x72, 0xd4, 0x44,
	0xdf, 0x83, 0xf2, 0xf8, 0x64, 0x73, 0xa2, 0xea, 0x9c, 0x88, 0x29, 0xe4, 0xd6, 0x54, 0xe6, 0xca,
	0x4d, 0x15, 0x67, 0x63, 0xc3, 0xae, 0x7e, 0x2a, 0x38, 0x9b, 0x55, 0x7c, 0x6c, 0x2e, 0x88, 0x36,
	0x03, 0xfa, 0x97, 0x78, 0x01, 0xa5, 0xce, 0x9b, 0xb9, 0x20, 0x20, 0x20,

	//loadIcon: 884 1152 1037056: fmt 2 48x48 72 ICON_VOL_MUTE
	73, 48,
	0x78, 0xda, 0xed, 0xd2, 0x21, 0x0e, 0x00, 0x20, 0x10, 0x03, 0xc1, 0x73, 0x3c, 0x9b, 0xa7, 0x17,
	0x85, 0x21, 0x0c, 0x1e, 0x42, 0x65, 0xb3, 0xaa, 0xdd, 0xaa, 0xa7, 0xd2, 0xd0, 0x77, 0xe0, 0x01,
	0x1e, 0xe0, 0x01, 0x1e, 0xe0, 0x01, 0x1e, 0xe0, 0x01, 0xbe, 0xf6, 0x99, 0xf9, 0xfd, 0xb6, 0xd7,
	0x6e, 0xa7, 0x9d, 0xf5, 0x0b, 0x7f, 0xd4, 0xef, 0xf4, 0x44, 0x5e, 0xd1, 0x43, 0x79, 0x4b, 0xcf,
	0xaf, 0xcb, 0x00, 0xa9, 0x41, 0x98, 0xa9, 0x20,
};
#endif //FT900_ICON_FROM_RAM

int helperLoadIcon(Ft_Gpu_Hal_Context_t *pHalContext, struct gram_t *pGram, const char* filename, struct bm_info_t *pBmInfo)
{
#ifdef FT900_ICON_FROM_RAM
	ft_uint32_t w = 0;
	int idx = 0;
	int len = 0;

	//Hard-code this
	pBmInfo->fmt = L4;
//	iprintf("Load icon: %d\n", pBmInfo->iconId);
	//Find the icon
	for (int i = 0; i <= pBmInfo->iconId; i++) {
		if (g_IconData[idx] == (ft_uint8_t)255) {
			idx++;
			len = 255 + g_IconData[idx];
		} else {
			len = g_IconData[idx];
		}
		idx += len + 1;
	}
	idx -= len;//Minus 1 for type
	w = g_IconData[idx];
	pBmInfo->w = pBmInfo->h = w;
	int size = w / 2 * w;

	pBmInfo->gram_size = size;
	pBmInfo->fmt = L4;
	pBmInfo->w = w;
	pBmInfo->h = w;
	if (pBmInfo->gram_src == 0) {
		size = (size + 3) & ~3;
		pGram->gram_free -= size;
		pBmInfo->gram_src = pGram->gram_start + pGram->gram_free;
	}
	idx++;
#if defined(FT900_PLATFORM)
	Ft_Gpu_CoCmd_Inflate(pHalContext, pBmInfo->gram_src);
	Ft_Gpu_Hal_WrCmdBufFromFlash(pHalContext, &g_IconData[idx], len-1);
#else
	Ft_Gpu_Hal_WrCmd32(pHalContext, CMD_INFLATE);
	Ft_Gpu_Hal_WrCmd32(pHalContext, pBmInfo->gram_src);
	Ft_Gpu_Hal_WrCmdBuf(pHalContext, &g_IconData[idx], len - 1);
#endif
	//iprintf("Icon: %d wxh=%dx%d %d %d %d %d\n\r", pBmInfo->iconId, w, w, len - 1, pBmInfo->gram_src, size, sizeof(g_IconData));
	return 0;
#else
	struct {
	    uint32_t id;
	    uint32_t nicons;
	    uint32_t fmt, w, h;
	} hdr;
	UINT bytesread;
	uint32_t offsets[2];
	FIL file;
	FRESULT res;
	//NOTE: for CleO project, we need this CleO obj for wrapper file access eFlash vs SDCard
	//      but moving to stand-alone app, we use fatfs directly
	res = f_open(&file, filename, FA_READ);

	if (res != FR_OK)
		return 0;
	res = f_read(&file, (uint8_t*)&hdr, sizeof(hdr), &bytesread);
	if (hdr.id != 0xf7d11100) {
		F_CLOSE(&file);
		return -1;
	}
	res = f_lseek(&file, sizeof(hdr) + sizeof(uint32_t) * pBmInfo->iconId);
	res = f_read(&file, (uint8_t*)&offsets, sizeof(offsets), &bytesread);
	res = f_lseek(&file, offsets[0]);

	int size;
	switch (hdr.fmt) {
		case L1:     size = hdr.w / 8 * hdr.h; break;
		case L2:     size = hdr.w / 4 * hdr.h; break;
		case L4:     size = hdr.w / 2 * hdr.h; break;
		case RGB332:
		case ARGB2:
		case L8:     size = hdr.w * hdr.h; break;
		case ARGB4:
		case ARGB1555:
		case RGB565: size = hdr.w * 2 * hdr.h; break;
		default:
		return -1;
	}
	pBmInfo->gram_size = size;
	pBmInfo->fmt = hdr.fmt;
	pBmInfo->w = hdr.w;
	pBmInfo->h = hdr.h;
	if (pBmInfo->gram_src == 0) {
		size = (size + 3) & ~3;
		pGram->gram_free -= size;
		pBmInfo->gram_src = pGram->gram_start + pGram->gram_free;
	}
	uint8_t dd[512];
	unsigned remaining = offsets[1] - offsets[0];

	iprintf("loadIcon: %d %d %d: fmt %d %dx%d %d\n\r", pBmInfo->iconId, size, pBmInfo->gram_src, hdr.fmt, hdr.w, hdr.h, remaining);

	Ft_Gpu_CoCmd_Inflate(pHalContext, pBmInfo->gram_src);
	while (remaining) {
		unsigned n = remaining;
		if (n > sizeof(dd))
		  n = sizeof(dd);
		res = f_read(&file, dd, n, &bytesread);
		//dump_buffer(dd, n);
		Ft_Gpu_Hal_WrCmdBuf(pHalContext, dd, n);
		remaining -= n;
	}
	F_CLOSE(&file);
	return 0;
#endif
}

#if 0
static void dump_buffer(unsigned char *buf, int len)
{
#define BUFF_SIZE 120
	char line[BUFF_SIZE];
	int i;
	char *s = line;

	for (i = 0; i < len; i++) {
		if (!(i % 16) || i == (len-1)) {
			if (i) {
#if 1
				*s++ = ' ';
				for (int j = 0; j < 16; j++) {
					if (isprint(buf[i+j-16])) {
						*s++ = buf[i+j-16];
					} else {
						*s++ = '.';
					}
				}
#endif
				*s++ = '\n';
				*s++ = '\r';
				*s++ = 0;
				uart_puts(UART0, line);
			}
			s = line + sprintf(line, "%03d: ", i);
			//s = line + sprintf(line, "");
		}
		s += sprintf(s, "0x%02x, ", buf[i]);
	}
	*s++ = '\n';
	*s++ = '\r';
	*s++ = 0;
	uart_puts(UART0, line);
#define uart_puts(a,b)
}
#endif
