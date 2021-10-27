/* Sample application to demonstrat FT800 primitives, widgets and customized screen shots */


#include "Ft_Gpu_Hal.h"
#include "Gpu_Hal.h"
#include "Gpu_CoCmd.h"

#include "FT_Platform.h"
#include "FT_Buffer.h"
#include "FT_MQueue.h"
#include "FT_AVIParser.h"
#include "FT_App_PlayVideo.h"
#include "FT_Audio.h"
#include "FT_DataTypes.h"

#define _CRT_NONSTDC_NO_WARNINGS 

#if defined(FT900_PLATFORM)
#include "ff.h"
#endif
#define SAMAPP_DELAY_BTW_APIS (1000)
#define SAMAPP_ENABLE_DELAY() Ft_Gpu_Hal_Sleep(SAMAPP_DELAY_BTW_APIS)
#define SAMAPP_ENABLE_DELAY_VALUE(x) Ft_Gpu_Hal_Sleep(x)
#undef F16
#define F16(s)        ((ft_int32_t)((s) * 65536))
#define WRITE2CMD(a) Ft_Gpu_Hal_WrCmdBuf(s_pHalContext,a,sizeof(a))
#define MEDIAFIFOSIZE (10*1024)

#undef SQ
#define SQ(v) (v*v)
#define NOTOUCH		-32768

#if defined(FT900_PLATFORM)
#include <ft900_sys.h>
#include <ft900_delay.h>
#include <registers/ft900_registers.h>
#endif

#include <stdarg.h>
#include <stddef.h>
#include "FT_Util.h"

#undef max
#undef min
#define max(a, b) (((a) < (b)) ? (b) : (a))
#define min(a, b) (((a) > (b)) ? (b) : (a))
#define ft_delay(x) Ft_Gpu_Hal_Sleep(x)

//typedef float 				ft_float_t;

#ifdef WIN32
#define ft_printf printf
#define printf printf
#else
extern int ft_printf(const char* fmt, ...);
#if 0
#define printf(fmt, ...) do {\
	ft_printf(fmt, ##__VA_ARGS__);\
}while(0)
#else
#define printf printf
#define ft_printf printf
#endif
#endif

#define LOG_TAG "AviPlayer  "

#define FT_Error(fmt, ...) do {\
	ft_printf("E/" fmt "\n\r", ##__VA_ARGS__);\
}while(0)


#define FT_Info(fmt, ...) do {\
	ft_printf("I/" fmt "\n\r", ##__VA_ARGS__);\
}while(0)

#define FT_Debug(fmt, ...) do {\
	ft_printf("D/" fmt "\n\r", ##__VA_ARGS__);\
}while(0)

#define intLock()   interrupt_disable_globally()
#define intUnLock() interrupt_enable_globally()

enum AviPlayerIcon_e {
	AVIPLAYER_ICON_BACK,
	AVIPLAYER_ICON_EXIT,
	AVIPLAYER_ICON_SPEAKER,
	AVIPLAYER_ICON_VOL_MUTE,
	AVIPLAYER_ICON_PLAY,
	AVIPLAYER_ICON_PAUSE,
	AVIPLAYER_ICON_ROTATE,
	AVIPLAYER_ICON_MAX
	//No icon after this
};

/////////////////////////////// Some Global Macros ////////////////////////////////////////
#define UpdateFifoRdPtr(_pHalContext, pFifo, pMetaV) do {\
	(pFifo)->fifo_rp = (pMetaV)->DeviceWrtPtr;\
	Ft_Gpu_Hal_Wr32(_pHalContext, (pFifo)->HW_Read_Reg, (pMetaV)->DeviceWrtPtr);\
	EVE_Cmd_waitFlush(s_pHalContext);\
}while(0)

////////////////////////////// End Of Global Macros ///////////////////////////////////////
//method for audio playback, only one method should be enabled at any given time.
#define AUDIO_VIA_FT81X 1
//FIXME: for testing purpose!Will enable audio later
//#define AUDIO_VIA_FT900 1


#define FT_AVIPLAYER_MAX_AUDIN_QE	(20)
#define FT_AVIPLAYER_MAX_AUDOUT_QE	(1)
#define FT_AVIPLAYER_MAX_VIDIN_QE	(20)
#define FT_AVIPLAYER_MAX_VIDOUT_QE	(1)

#define FT_AVIPARSER_TESTSTUB_VIDBUFFMAXSZ	(200*1024)
#define FT_81X_AUDIO_BUFFER_SIZE (30*1024)
//#define FT_EMPTY_AUDIO_DROP_VIDEO_LEEWAY (FT_AVIPLAYER_MAX_VIDIN_QE/2)
#define FT_EMPTY_AUDIO_DROP_VIDEO_LEEWAY (5)

/* files to be tested */
#define FT_PLAYER_TESTNUM	(4)

#if defined(MSVC_PLATFORM) || defined(BT8XXEMU_PLATFORM)
#define FT_GUI_DISAPPEAR_COUNT	(200)
#elif defined(FT900_PLATFORM)
#if defined(USE_TIMER_BASE_TOUCH)
#define FT_GUI_DISAPPEAR_COUNT	(5000)  //5 seconds before making the control UI disappear
#else
//#define FT_GUI_DISAPPEAR_COUNT	(120000)
#define FT_GUI_DISAPPEAR_COUNT	(10000)
#endif
#endif

#define VIDEO_DROP_ALL_OLD_FRAMES
//#define DISPLAY_VIDEO_IN_LOOP


/* Global variables for display resolution to support various display panels */
/* Default is WQVGA - 480x272 */
extern ft_int16_t FT_DispWidth;
extern ft_int16_t FT_DispHeight;

/* Global used for buffer optimization */

typedef struct AviPlayerGlobalCtx_s {
	const char* fname;

#define CurFile			(g_global_ctx->_CurFile)
#if defined(FT900_PLATFORM)
	FIL 	_CurFile;
#elif defined(MSVC_PLATFORM) || defined(BT8XXEMU_PLATFORM)
	FILE* _CurFile;
#endif

	/*  profiling stats, remove me */
#define tempval 		(g_global_ctx->_tempval)
	ft_uint32_t _tempval;

	/* Global definitions - to be migrated to dynamic allocation based on input file */
#define G_S_PGui 		(g_global_ctx->_G_S_PGui)
#define G_S_AviPlayer 	(g_global_ctx->_G_S_AviPlayer)
#define G_S_AviParser 	(g_global_ctx->_G_S_AviParser)
#define G_SVidDataPath 	(g_global_ctx->_G_SVidDataPath)
#define G_SAudDataPath 	(g_global_ctx->_G_SAudDataPath)
	FT_PGUI_t _G_S_PGui;
	FT_AVIPlayer_t _G_S_AviPlayer;
	FT_AVIParser_t _G_S_AviParser;
	FT_VidDataPath_t _G_SVidDataPath;
	FT_AudDataPath_t _G_SAudDataPath;

#define stFifo				(g_global_ctx->_stFifo)
	//#define audioFifo			(g_global_ctx->_audioFifo)
#define srqaudin			(g_global_ctx->_srqaudin)
#define srqvidin			(g_global_ctx->_srqvidin)
#define srbaudin			(g_global_ctx->_srbaudin)
	Fifo_t _stFifo;
	//	Fifo_t _audioFifo;
	FT_RingQueue_t _srqaudin;
	FT_RingQueue_t _srqvidin;
	FT_RingBuffer_t _srbaudin;

#define G_SA_VMetaDInRQ		(g_global_ctx->_G_SA_VMetaDInRQ)
#define G_SA_VMetaDOutRQ	(g_global_ctx->_G_SA_VMetaDOutRQ)
#define G_SA_AMetaDInRQ		(g_global_ctx->_G_SA_AMetaDInRQ)
	/* Ring queue buffer */
	FT_MetaV_t _G_SA_VMetaDInRQ[FT_AVIPLAYER_MAX_VIDIN_QE];
	FT_MetaA_t _G_SA_AMetaDInRQ[FT_AVIPLAYER_MAX_AUDIN_QE];

	/**********
	 * For new UI
	 */
	struct bm_info_t iconList[AVIPLAYER_ICON_MAX];
	struct gram_t    gram;
	ft_uint8_t          volume_level;

	/*volatile ft_uint32_t parserfillmutex = 0, videodecodemutex = 0, guimutex = 0, audioPlaying = 0, audiomutex = 0, updateAudioIndex = 0;*/
#define parserfillmutex		(g_global_ctx->_parserfillmutex)
#define videodecodemutex	(g_global_ctx->_videodecodemutex)
#define guimutex			(g_global_ctx->_guimutex)
#define audioPlaying		(g_global_ctx->_audioPlaying)
#define audiomutex			(g_global_ctx->_audiomutex)
#define updateAudioIndex	(g_global_ctx->_updateAudioIndex)
	volatile ft_uint32_t _parserfillmutex;
	volatile ft_uint32_t _videodecodemutex;
	volatile ft_uint32_t _guimutex;
	volatile ft_uint32_t _audioPlaying;
	volatile ft_uint32_t _audiomutex;
	volatile ft_uint32_t _updateAudioIndex;

	/*volatile ft_uint32_t updateFrame = 1, partialFrameCount = 0, dropSlowVideoFrame = 0, actualISRcount = 0;*/
#define updateFrame				(g_global_ctx->_updateFrame)
#define partialFrameCount		(g_global_ctx->_partialFrameCount)
#define dropSlowVideoFrame		(g_global_ctx->_dropSlowVideoFrame)
#define actualISRcount			(g_global_ctx->_actualISRcount)
	volatile ft_uint32_t _updateFrame;// = 1;
	volatile ft_uint32_t _partialFrameCount;
	volatile ft_uint32_t _dropSlowVideoFrame;
	volatile ft_uint32_t _actualISRcount;

#if defined(AUDIO_VIA_FT81X)
#define lastAudioPtr			(g_global_ctx->_lastAudioPtr)
#define FT800AudioBufferSize	(g_global_ctx->_FT800AudioBufferSize)
#define audioFifoOffset			(g_global_ctx->_audioFifoOffset)
	volatile ft_uint32_t _lastAudioPtr;
	ft_uint32_t _FT800AudioBufferSize;
	ft_uint32_t _audioFifoOffset;
#endif //AUDIO_VIA_FT81X

#define Ft_CmdBuffer_Index		(g_global_ctx->_Ft_CmdBuffer_Index)
#define Ft_DlBuffer_Index		(g_global_ctx->_Ft_DlBuffer_Index)
	ft_uint32_t _Ft_CmdBuffer_Index;
	ft_uint32_t _Ft_DlBuffer_Index;

#ifdef BUFFER_OPTIMIZATION
#define Ft_DlBuffer		(g_global_ctx->_Ft_DlBuffer)
#define Ft_CmdBuffer	(g_global_ctx->_Ft_CmdBuffer)
	ft_uint8_t _Ft_DlBuffer[FT_DL_SIZE];
	ft_uint8_t _Ft_CmdBuffer[FT_CMD_FIFO_SIZE];
#endif

#define fifoEmptyTimeStart				(g_global_ctx->_fifoEmptyTimeStart)
#define fifoEmptyTimeEnd				(g_global_ctx->_fifoEmptyTimeEnd)
#define i2sClockDifferenceCounter		(g_global_ctx->_i2sClockDifferenceCounter)
	ft_uint16_t _fifoEmptyTimeStart;
	ft_uint16_t _fifoEmptyTimeEnd;
	ft_uint16_t _i2sClockDifferenceCounter;

#define lateAudioZeros	(g_global_ctx->_lateAudioZeros)
#define waitAudioZeros	(g_global_ctx->_waitAudioZeros)
	ft_uint32_t _lateAudioZeros;
	ft_uint32_t _waitAudioZeros;

#define keypressed		(g_global_ctx->_keypressed)
#define keyin_cts		(g_global_ctx->_keyin_cts)
#define tpprevval		(g_global_ctx->_tpprevval)
#define timercount		(g_global_ctx->_timercount)
#define timervalue		(g_global_ctx->_timervalue)
	ft_uint8_t _keypressed;
	ft_uint8_t _keyin_cts;
	ft_uint32_t _tpprevval;
	ft_uint32_t _timercount;
	ft_uint32_t _timervalue;

	/* Globals for polling implementation */

#define videoFrame		(g_global_ctx->_videoFrame)
	FT_VIDEO_FRAME _videoFrame;

#define controlButtons	(g_global_ctx->_controlButtons)
	FT_GUI_BUTTONS_POS _controlButtons;

	/* Globals for interrupt implementation */
#define FTPayer_TMicroS				(g_global_ctx->_FTPayer_TMicroS)
	volatile ft_uint32_t _FTPayer_TMicroS;

#define FTPayer_AudioRate		(g_global_ctx->_FTPayer_AudioRate)
#define FTPayer_VideoRate		(g_global_ctx->_FTPayer_VideoRate)
#define FTPayer_AudioCnt		(g_global_ctx->_FTPayer_AudioCnt)
#define FTPayer_VideoCnt		(g_global_ctx->_FTPayer_VideoCnt)
	volatile ft_uint32_t _FTPayer_AudioRate;
	volatile ft_uint32_t _FTPayer_VideoRate;
	volatile ft_uint32_t _FTPayer_AudioCnt;
	volatile ft_uint32_t _FTPayer_VideoCnt;

#define currentPartialFrame			(g_global_ctx->_currentPartialFrame)
#define maxVideoFrameSz				(g_global_ctx->_maxVideoFrameSz)
#define FT81xAudioPlaybackLeeway		(g_global_ctx->_FT81xAudioPlaybackLeeway)
	volatile ft_uint8_t _currentPartialFrame;
	ft_uint32_t _maxVideoFrameSz;
	ft_uint32_t _FT81xAudioPlaybackLeeway;


#define G_PBuff				(g_global_ctx->_G_PBuff)
#ifdef AUDIO_VIA_FT900
#define FT_AVIPARSER_BUFFER_SIZE_NEW (FT_AVIPARSER_BUFFER_SIZE)
#define AudioBuff			(g_global_ctx->_AudioBuff)
	ft_uint8_t _G_PBuff[FT_AVIPARSER_BUFFER_SIZE];
	ft_uint8_t _AudioBuff[AUDIO_BUFFER_SIZE];
#else
#define FT_AVIPARSER_BUFFER_SIZE_NEW (FT_AVIPARSER_BUFFER_SIZE + AUDIO_BUFFER_SIZE)
	ft_uint8_t _G_PBuff[FT_AVIPARSER_BUFFER_SIZE_NEW];
#endif

#ifdef AUDIO_VIA_FT900
#define AudioConfig			(g_global_ctx->_AudioConfig)
	FT_AUDIO_CONFIG	_AudioConfig;
#endif
} AviPlayerGlobalCtx_t;
static AviPlayerGlobalCtx_t* g_global_ctx;
static Ft_Gpu_Hal_Context_t* s_pHalContext;


#if 0
/* Boot up for FT800 followed by graphics primitive sample cases */
/* Initial boot up DL - make the back ground green color */
__flash__ const ft_uint8_t FT_DLCODE_BOOTUP_AVIPlayer[12] =
{ 0, 0, 0, 2, //GPU instruction CLEAR_COLOR_RGB
  7, 0, 0, 38, //GPU instruction CLEAR
  0, 0, 0, 0,  //GPU instruction DISPLAY
};
#endif

ft_void_t ft_timer_interrupt1();
void aviPlayerRenderFrame(void);
void aviPlayerRenderAudio(void);

////////////////////////////// Re-use these functions from CameraStreaming App /////////////////////////////

extern ft_int32_t Ft_CoProErrorRecovery(Ft_Gpu_Hal_Context_t *s_pHalContext);
//extern ft_void_t EVE_Cmd_waitFlush(Ft_Gpu_Hal_Context_t *s_pHalContext);
extern void Ft_Play_Sound(ft_uint8_t sound, ft_uint8_t vol, ft_uint8_t midi);
//extern ft_void_t EVE_Cmd_wr32(Ft_Gpu_Hal_Context_t* pHalContext, ft_uint32_t cmd);
extern ft_uint32_t FT_AVIParser_Seek_To(ft_uint8_t percent, FT_AVIParser_t* pParser);

////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define EVE_RESET_COPROCESSORRELEASEALL		(0x0000)
#define EVE_RESET_COPROCESSOR_J1			(0x0001)
#define EVE_RESET_COPROCESSOR_JT			(0x0002)
#define EVE_RESET_COPROCESSOR_JA			(0x0004)

#define EVE_FRAME_SLOW_DECODE				(4)
/* Error recovery of coprocessor J1 */
ft_int32_t Ft_CoProErrorRecovery_Check(Ft_Gpu_Hal_Context_t *s_pHalContext)
{
	ft_int32_t rdptr, wrptr;
	ft_uint32_t old_mutex = guimutex;

	guimutex = 1;
	rdptr = Ft_Gpu_Hal_Rd16(s_pHalContext, REG_CMD_READ);
	wrptr = Ft_Gpu_Hal_Rd16(s_pHalContext, REG_CMD_WRITE);
	if ((rdptr & 0x3) || (wrptr & 0x3) ||
		(rdptr != wrptr && videodecodemutex > EVE_FRAME_SLOW_DECODE)) {
		FT_Error("EVE hang %x %x! Recover now", rdptr, wrptr);
	}
	else {
		guimutex = old_mutex;
		/*FT_Error("OK %d %d", rdptr, wrptr);*/
		return 0;
	}
	Ft_CoProErrorRecovery(s_pHalContext);
	if (G_S_AviPlayer.mediafifo > 0) {
		Ft_Gpu_CoCmd_MediaFifo(s_pHalContext, G_S_AviPlayer.mediafifo, G_S_AviPlayer.mediafifolen);	//address of the media fifo buffer - init of fifo done by coprocessor
		EVE_Cmd_waitFlush(s_pHalContext);
		Gpu_Hal_WaitCmdfifo_empty(s_pHalContext);
		//	FT_RQueue_ReadE(&srqvidin, NULL);
		Ft_Gpu_Hal_Wr32(s_pHalContext, stFifo.HW_Write_Reg, stFifo.fifo_wp);
	}
	guimutex = old_mutex;
	//	FT_Error("DONE");
		/* TODO - datapath flush, mediafifo and audio PCM buffer flush */
	return 0;
}

ft_void_t Gpu_Hal_WaitCmdfifo_empty_and_recovery(Ft_Gpu_Hal_Context_t* host)
{
	Ft_CoProErrorRecovery_Check(s_pHalContext);
	Gpu_Hal_WaitCmdfifo_empty(s_pHalContext);
}

#define Gpu_Hal_WaitCmdfifo_empty Gpu_Hal_WaitCmdfifo_empty_and_recovery

/*****************************************************************************/
/* Example code to display few points at various offsets with various colors */

/*
 static void polarxy(ft_int32_t r, ft_uint16_t th, ft_int32_t *x, ft_int32_t *y)
 {
 *x = (16 * (FT_DispWidth / (2 * noofch)) + (((long) r * qsin(th)) >> 11) + 16 * ox);
 *y = (16 * 300 - (((long) r * qcos(th)) >> 11));
 }

 void vertex(ft_int32_t x, ft_int32_t y)
 {
 EVE_Cmd_wr32(s_pHalContext, VERTEX2F(x, y));
 }

 static void polar(ft_int32_t r, ft_uint16_t th)
 {
 ft_int32_t x, y;
 polarxy(r, th, &x, &y);
 vertex(x, y);
 }

 ft_uint16_t da(ft_int32_t i)
 {
 return (i - 45) * 32768L / 360;
 }

 static void cs(ft_uint8_t i)
 {
 switch (i)
 {
 case 0:
 EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(200, 255, 200));
 break;
 case 60:
 EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 255, 0));
 break;
 case 80:
 EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 0, 0));
 break;
 }
 }
 */
#ifdef FT900_PLATFORM

 /* Time for each interrupt would be (1000/(frequency/(prescalar*timervalue))). to get 30fps or 33.333ms, 1000/(100000000/(30*1000*100))*/
ft_void_t AviPlayer_TimerInit()
{
	uint16_t value;
#ifdef FT900_PLATFORM
	FTPayer_TMicroS = 0;

	sys_enable(sys_device_timer_wdt);
	timer_prescaler(FT900_TIMER_PRESCALE_VALUE);
	timer_init(FT900_FT_MILLIS_TIMER, FT900_TIMER_OVERFLOW_VALUE, timer_direction_up, timer_prescaler_select_on, timer_mode_continuous);
	//	timer_init(CAM_FLUSH_FIFO_TIMER,CAM_FLUSH_FIFO_TIMER_OVERFLOW,timer_direction_up,timer_prescaler_select_on,timer_mode_continuous);
	interrupt_attach(interrupt_timers, 17, ft_timer_interrupt1);

	/* enabling the interrupts for timer */
	timer_enable_interrupt(FT900_FT_MILLIS_TIMER);
	//	timer_start(FT900_FT_MILLIS_TIMER);
#endif
}

#define ft_timer_interrupt1_init(TPrescalar, TValue) AviPlayer_TimerInit(timer_select_a, TPrescalar, TValue)

//Only 1 prescaler is available for the two possible timers in the FT900.
#define ft_timer_interrupt2_init(TPrescalar, TValue) AviPlayer_TimerInit(timer_select_b, TPrescalar, TValue)

ft_void_t ft_timer_interrupt1_exit()
{
#ifdef FT900_PLATFORM
	timer_stop(FT900_FT_MILLIS_TIMER);
	/*timer_stop(timer_select_b);*/
#endif
}

#endif

#if defined(MSVC_PLATFORM) || defined(BT8XXEMU_PLATFORM) 
#define time_after(a,b) ((signed long long)((b) - (a)) < 0)
static LARGE_INTEGER  CpuFrequency;
void MSVCTimeInit()
{
	QueryPerformanceFrequency(&CpuFrequency);
}

long long MSVCTimeMsGet(void)
{
#if 0
	SYSTEMTIME time;
	GetSystemTime(&time);

	//TODO: should call
	ft_uint32_t time_ms = (time.wHour * 3600 + time.wMinute * 60 + time.wSecond) * 1000 + time.wMilliseconds;

	//printf("Curr time ms is: %d(0x%x) \n", time_ms, time_ms);
	return time_ms;
#else
	//if (CpuFrequency == 0) QueryPerformanceFrequency(&CpuFrequency);
	LARGE_INTEGER now;

	QueryPerformanceCounter(&now);
	return (now.QuadPart * 1000) / CpuFrequency.QuadPart;
#endif
}

void MSVCTimerThread(void)
{
#if 0
	if (FTPayer_AudioCnt > 1) {
		FTPayer_AudioCnt--;
	}
	if (FTPayer_VideoCnt > 1) {
		FTPayer_VideoCnt--;
	}
#endif
	long long now = MSVCTimeMsGet();
	if (G_S_AviPlayer.StateCurr == FT_AVIPLAYER_PAUSE) {
		G_S_AviPlayer.audTimestamp = G_S_AviPlayer.vidTimestamp = now;
		return;
	}
	//TODO:FIXME: check this later!
	//if (FTPayer_AudioCnt == 1)

	if (!(G_S_AviParser.Flags & FT_AVIPARSER_DROPAUDIO) && time_after(now, G_S_AviPlayer.audTimestamp + FTPayer_AudioRate))
	{
		//FTPayer_AudioCnt = FTPayer_AudioRate;
		G_S_AviPlayer.audTimestamp += FTPayer_AudioRate;
		if (G_S_AviPlayer.StateCurr == FT_AVIPLAYER_PAUSE || !G_S_AviParser.AudioStreamFound) {
			goto vid_isr;
		}
#if 1
		if (updateAudioIndex > 40) {
			printf("Warning: audio playback is being delayed with busy operations(%d).\n",updateAudioIndex);
		}
#endif
		if ((1 == parsingVideo) || (1 == guimutex) || (audioPlaying == 1) || (parsingAudio == 1) /*|| (updateAudioIndex)*/) {
			updateAudioIndex++;
			goto vid_isr;
		}
		aviPlayerRenderAudio();
	}
vid_isr:
	if (time_after(G_S_AviPlayer.vidTimestamp + FTPayer_VideoRate, now)) {
		//skip! Not time for rendering
		return;
	}
	G_S_AviPlayer.vidTimestamp += FTPayer_VideoRate;
	/*
	[Decode and display mjpeg frame]

	One of two sections where frame decoding is initialized and frame(s) are dropped.
	*/
	//if (FTPayer_VideoCnt == 1) 
	{
		//FTPayer_VideoCnt = FTPayer_VideoRate;
		//actualISRcount++;
		{
			//videodecodemutex = 0;
			if ((1 == parsingVideo) || (1 == guimutex) || (audioPlaying == 1) || (parsingAudio == 1)) {
				videodecodemutex++;
				if (videodecodemutex > 40) {
					//printf("Warning: frame displaying is being delayed with busy operations(%d).\n", videodecodemutex);
				}
				return; //testing
			}
			else {
				ft_int32_t rdptr, wrptr;

				if (videodecodemutex) {
					return;
				}
				rdptr = Ft_Gpu_Hal_Rd16(s_pHalContext, REG_CMD_READ);
				wrptr = Ft_Gpu_Hal_Rd16(s_pHalContext, REG_CMD_WRITE);
				if (rdptr == wrptr) { ///write the next frame if and only if the previous command has been consumed.
					aviPlayerRenderFrame();
				}
				else {
					videodecodemutex++;
#if defined(SHOWVIDEOPRINTS)
					printf("Device is busy. %x %x\n", rdptr, wrptr);
#endif
				}
			}
		}
	}
}

#endif // defined(MSVC_PLATFORM) || defined(BT8XXEMU_PLATFORM)

ft_uint8_t istouch()
{
	return !(Ft_Gpu_Hal_Rd16(s_pHalContext, REG_TOUCH_RAW_XY) & 0x8000);
}

float linear(float p1, float p2, float t, ft_uint16_t rate)
{
	float st = (float)t / rate;
	return p1 + (st * (p2 - p1));
}

//ft_uint16_t point_size[30];
//ft_uint8_t color[20],alpha_array[20];



ft_int32_t DefaultGUIScreen(Ft_Gpu_Hal_Context_t *s_pHalContext)
{

	Gpu_CoCmd_Dlstart(s_pHalContext);
	EVE_Cmd_wr32(s_pHalContext, CLEAR_COLOR_RGB(0, 0, 0));
	EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1));
	EVE_Cmd_wr32(s_pHalContext, DISPLAY());
	Ft_Gpu_CoCmd_Swap(s_pHalContext);
	EVE_Cmd_waitFlush(s_pHalContext);
	Gpu_Hal_WaitCmdfifo_empty(s_pHalContext);

	return 0;
}

ft_int32_t VideoFrameonly(Ft_Gpu_Hal_Context_t *pHalContext, ft_int32_t aviw, ft_int32_t avih)
{

	Gpu_CoCmd_Dlstart(pHalContext);
	EVE_Cmd_wr32(pHalContext, CLEAR_TAG(0));

	EVE_Cmd_wr32(pHalContext, CLEAR_COLOR_RGB(0, 0, 0));
	EVE_Cmd_wr32(pHalContext, CLEAR(1, 1, 1));

	//decoded jpeg bitmap
	EVE_Cmd_wr32(pHalContext, TAG_MASK(0));
	EVE_Cmd_wr32(pHalContext, TAG(0));
	EVE_Cmd_wr32(pHalContext, COLOR_RGB(255, 255, 255));
	EVE_Cmd_wr32(pHalContext, BITMAP_HANDLE(0));
	EVE_Cmd_wr32(pHalContext, BITMAP_SOURCE(0)); //bitmap locaiton is 4, as 0 is used for return of video decode result
	EVE_Cmd_wr32(pHalContext, BITMAP_LAYOUT(RGB565, aviw * 2L, avih));
	EVE_Cmd_wr32(pHalContext, BITMAP_LAYOUT_H(((aviw * 2L) >> 10), ((avih) >> 9)));
	EVE_Cmd_wr32(pHalContext, BITMAP_SIZE(NEAREST, BORDER, BORDER, aviw, avih));
	EVE_Cmd_wr32(pHalContext, BITMAP_SIZE_H((aviw >> 9), (avih >> 9)));
	EVE_Cmd_wr32(pHalContext, BEGIN(BITMAPS));
	//hack for not to display video frame

	EVE_Cmd_wr32(pHalContext, VERTEX2F(8 * (FT_DispWidth - aviw), 8 * (FT_DispHeight - avih))); //display at the center of the screen

	EVE_Cmd_wr32(pHalContext, DISPLAY());
	Ft_Gpu_CoCmd_Swap(pHalContext);
	EVE_Cmd_waitFlush(pHalContext);

	return 0;
}

#define FT_MAINMENU_NUMFILES	(4)
#define FT_MAINMENU_WIDTH		(240)
#define FT_MAINMENU_HEIGHT		(144)
#define FT_MAINMENU_DISTANCE	(60)

#if !defined(MSVC_PLATFORM) && !defined(BT8XXEMU_PLATFORM)
ft_void_t ft_timer_interrupt1()
{
	timer_disable_interrupt(FT900_FT_MILLIS_TIMER);
	/* Clear the interrupt and increment the counter */
	timer_is_interrupted(FT900_FT_MILLIS_TIMER);
	++FTPayer_TMicroS;
	if (FTPayer_AudioCnt > 1) {
		FTPayer_AudioCnt--;
	}
	if (FTPayer_VideoCnt > 1) {
		FTPayer_VideoCnt--;
	}
	timer_enable_interrupt(FT900_FT_MILLIS_TIMER);

	if (FTPayer_AudioCnt == 1)
	{
		FTPayer_AudioCnt = FTPayer_AudioRate;

#if defined(USEAUDIOINTERRUPT)
		if (audioPlaying || parsingAudio || guimutex)
		{
			audiomutex = 1;
			goto vid_isr;
		}
		playAudioData(&G_S_AviPlayer, &G_S_AviParser, &G_SAudDataPath, &G_SVidDataPath);
#endif

#if defined(AUDIO_VIA_FT81X)
		if (G_S_AviPlayer.StateCurr == FT_AVIPLAYER_PAUSE || !G_S_AviParser.AudioStreamFound) {
			goto vid_isr;
		}
#if 0
		if (updateAudioIndex > 40) {
			//printf("Warning: audio playback is being delayed with busy operations(%d).\n",updateAudioIndex);
		}
#endif

		if ((1 == parsingVideo) || (1 == guimutex) || (audioPlaying == 1) || (parsingAudio == 1) /*|| (updateAudioIndex)*/) {
			updateAudioIndex++;
			goto vid_isr;
		}
		aviPlayerRenderAudio();
#endif
	}

vid_isr:
	/*
	 [Decode and display mjpeg frame]

	 One of two sections where frame decoding is initialized and frame(s) are dropped.
	 */
	if (FTPayer_VideoCnt == 1) {
		FTPayer_VideoCnt = FTPayer_VideoRate;
		actualISRcount++;
		{
			//videodecodemutex = 0;
#if defined(AUDIO_VIA_FT81X)
			if ((1 == parsingVideo) || (1 == guimutex) || (audioPlaying == 1) || (parsingAudio == 1)) {
#elif defined(AUDIO_VIA_FT900)
			if ((1 == parsingVideo) || (1 == guimutex)) {
#else
			if (1 == guimutex) {
#endif
				videodecodemutex++;

				if (videodecodemutex > 40) {
					//printf("Warning: frame displaying is being delayed with busy operations(%d).\n", videodecodemutex);
				}
				return; //testing
			}
			else {
				ft_int32_t rdptr, wrptr;

				if (videodecodemutex) {
					return;
				}
				rdptr = Ft_Gpu_Hal_Rd16(s_pHalContext, REG_CMD_READ);
				wrptr = Ft_Gpu_Hal_Rd16(s_pHalContext, REG_CMD_WRITE);
				if (rdptr == wrptr) { ///write the next frame if and only if the previous command has been consumed.
					aviPlayerRenderFrame();
				}
				else {
					videodecodemutex++;
#if defined(SHOWVIDEOPRINTS)
					printf("Device is busy. %x %x\n", rdptr, wrptr);
#endif
				}
			}
			}
			}
		}
/* [end of decode and display mjpeg frame] */
#endif

/*
#define SHOWVIDEOPRINTS 1
*/
void aviPlayerRenderFrame(void) {
	FT_MetaV_t tempvmeta;

	while (1) {
		if (G_S_AviParser.Flags & FT_AVIPARSER_DROPVIDEO) {
			if (srqvidin.ValidEle >= 1) {
				FT_RQueue_ReadE(&srqvidin, &tempvmeta);
			}
			G_S_AviPlayer.CurrFrame++;
			break;
		}

		if (srqvidin.ValidEle > 0) {

			FT_RQueue_PeekE(&srqvidin, &tempvmeta, srqvidin.ReadIdx);

			if (currentPartialFrame) {
				if (tempvmeta.PartialSz) {
					break;
				}
				else {
					FT_RQueue_ReadE(&srqvidin, &tempvmeta);
					currentPartialFrame = 0;
					continue;
				}
			}

			if (tempvmeta.PartialSz || tempvmeta.Length == 0) {
				partialFrameCount++;
			}
			else {
				partialFrameCount = 0;
			}

			if (dropSlowVideoFrame) {
				if (srqvidin.ValidEle > 0) {
					FT_RQueue_ReadE(&srqvidin, &tempvmeta);
					UpdateFifoRdPtr(s_pHalContext, &stFifo, &tempvmeta);
					G_S_AviPlayer.CurrFrame++;
					dropSlowVideoFrame = 0;
					if (tempvmeta.PartialSz != 0) {
						FT_AVIPFlush(&G_S_AviParser,
							tempvmeta.Length - tempvmeta.PartialSz);
					}
					//printf("Drop frame1.\n");
#if defined(SHOWVIDEOPRINTS)
					printf("Drop frame.\n");
#endif
					continue;
				}
			}

			if (((tempvmeta.Frame < G_S_AviPlayer.CurrFrame)
				|| (tempvmeta.PartialSz != 0))
				&& (partialFrameCount < VIDEO_WAIT_AUDIO_FRAME_THREASHOLD)) {
#if defined(SHOWVIDEOPRINTS)
				printf("DOVF2: %u %u %u %u %u --", tempvmeta.Frame, G_S_AviPlayer.CurrFrame, tempvmeta.PartialSz, tempvmeta.Length, partialFrameCount);
#endif
				if (tempvmeta.PartialSz == 0) {
#if defined(SHOWVIDEOPRINTS)
					printf("Full frame to be dropped.\n");
#endif
					FT_RQueue_ReadE(&srqvidin, &tempvmeta);
					UpdateFifoRdPtr(s_pHalContext, &stFifo, &tempvmeta);
				}
				else {
					if (tempvmeta.Length) {
#if defined(SHOWVIDEOPRINTS)
						printf("Partial frame to be dropped.\n");
#endif
						UpdateFifoRdPtr(s_pHalContext, &stFifo, &tempvmeta);
						FT_RQueue_ReadE(&srqvidin, &tempvmeta);
						FT_AVIPFlush(&G_S_AviParser,
							tempvmeta.Length - tempvmeta.PartialSz);
					}
					else { //zero length chunk, will never come here
#if defined(SHOWVIDEOPRINTS)
						printf("Empty frame to be dropped.\n");
#endif
						FT_RQueue_ReadE(&srqvidin, &tempvmeta);
					}
					G_S_AviPlayer.CurrFrame++; //added

					break;
				}
				G_S_AviPlayer.CurrFrame++; //added

			}
			else if ((tempvmeta.Frame == G_S_AviPlayer.CurrFrame)
				|| ((partialFrameCount >= VIDEO_WAIT_AUDIO_FRAME_THREASHOLD))) {
				if (tempvmeta.PartialSz == 0) {
					FT_RQueue_ReadE(&srqvidin, &tempvmeta);
				}
				else {
					currentPartialFrame = 1;
				}

				if (tempvmeta.Length > 0) {
					//Update read pointer
					if (G_S_AviParser.SAud.NumChannels == 0) {
						Ft_Gpu_Hal_Wr32(s_pHalContext, stFifo.HW_Read_Reg, tempvmeta.Curr);
					}
					Ft_Gpu_CoCmd_LoadImage(s_pHalContext, 0,
						OPT_MEDIAFIFO | OPT_NODL/* | OPT_NOTEAR*/);
					EVE_Cmd_waitFlush(s_pHalContext);
					//Gpu_Hal_WaitCmdfifo_empty(s_pHalContext); //remove me
					G_SVidDataPath.NumDecoded++;
					//G_S_AviParser.SVid.CurrFrame++;
				}
				G_S_AviPlayer.TotalBytes += tempvmeta.Length; //total number of bytes consumed till now by decoder
#if defined(SHOWVIDEOPRINTS)
				printf("PlaVF: %u %u %u %u %u\n", tempvmeta.Frame, G_S_AviPlayer.CurrFrame, tempvmeta.Length, tempvmeta.PartialSz, partialFrameCount);
#endif
				partialFrameCount = 0;
				G_S_AviPlayer.CurrFrame++;
				break;
			}

			else {
				G_S_AviPlayer.CurrFrame++;
#if defined(SHOWVIDEOPRINTS)
				if (tempvmeta.Frame > G_S_AviPlayer.CurrFrame)
				{
					printf("VWTBP: %u %u.\n", tempvmeta.Frame, G_S_AviPlayer.CurrFrame);
				}
				else
				{
					printf("VWUK: %u %u %u %u.\n", tempvmeta.Frame, G_S_AviPlayer.CurrFrame, tempvmeta.PartialSz, partialFrameCount);
				}
#endif
			}
		}
		else {
#if defined(SHOWVIDEOPRINTS)
			printf("VLF.\n");
#endif
			break;
		}
	}
}
/*
#undef SHOWVIDEOPRINTS
#define printf
*/

void aviPlayerRenderAudio(void)
{
	ft_uint32_t curAudioPtr = 0, dataChunkEnd;
	FT_MetaA_t tempameta;
	//ft_int32_t fResult;

	if (!G_S_AviParser.AudioStreamFound) {
		return;
	}
	if (G_S_AviParser.Flags & FT_AVIPARSER_DROPAUDIO) {
		if (srqaudin.ValidEle >= 0) {
			FT_RQueue_ReadE(&srqaudin, NULL);
		}
		return;
	}
	/*#define SHOWAUDIOPRINTS 1
		printf("Aud: enter \n\r");*/
	while (1) {
		if (srqaudin.ValidEle == 0) {
#if defined(SHOWAUDIOPRINTS)
			printf("LNAD2\n");
#endif
			break;
		}
		FT_RQueue_PeekE(&srqaudin, &tempameta, srqaudin.ReadIdx);

		if (tempameta.Frame > (G_S_AviPlayer.CurrFrame + OLD_AUDIO_FRAME_THREASHOLD)) {
			if (dropSlowVideoFrame == 0) {
				dropSlowVideoFrame = 1;
#if defined(SHOWAUDIOPRINTS)
				printf("IAdrop: %u %u %u\n", tempameta.Frame, G_S_AviPlayer.CurrFrame, G_S_AviPlayer.CurrFrame + OLD_AUDIO_FRAME_THREASHOLD);
#endif
			}
		}

		curAudioPtr = Ft_Gpu_Hal_Rd32(s_pHalContext, REG_PLAYBACK_READPTR);
		dataChunkEnd = (tempameta.Curr + tempameta.Length) >= (srbaudin.Start + srbaudin.Length) ? (srbaudin.Start + ((tempameta.Curr + tempameta.Length) - (srbaudin.Start + srbaudin.Length))) : tempameta.Curr + tempameta.Length;

		if (curAudioPtr > lastAudioPtr) {		//audio consumption is moving ahead
			if ((tempameta.Curr > dataChunkEnd) && (curAudioPtr > tempameta.Curr) && (curAudioPtr < (tempameta.Curr + tempameta.Length))) {
#if defined(SHOWAUDIOPRINTS)
				printf("Iconsuming. RDptr: %u last:%u start:%u end:%u\n", curAudioPtr, lastAudioPtr, tempameta.Curr, dataChunkEnd);
#endif
				break;
			}
			else if (curAudioPtr >= dataChunkEnd) {
				FT_RQueue_ReadE(&srqaudin, &tempameta);
#if defined(SHOWAUDIOPRINTS)
				printf("FCC0.\n");
#endif
				/*
				 srbaudin.Read = (srbaudin.Read + tempameta.Length) > (srbaudin.Start + srbaudin.Length) ? srbaudin.Start + ((srbaudin.Read + tempameta.Length) - (srbaudin.Start + srbaudin.Length)) : srbaudin.Read + tempameta.Length;
				 srbaudin.ValidLength -= tempameta.Length;
				 -*/
				FT_RingBuffer_Flush(&srbaudin, tempameta.Length);
			}
			else {
#if defined(SHOWAUDIOPRINTS)
				printf("Iconsuming1. RDptr: RDptr: %u last:%u start:%u end:%u\n", curAudioPtr, lastAudioPtr, tempameta.Curr, dataChunkEnd);
#endif
				break;
			}

		}
		else if (lastAudioPtr > curAudioPtr) {		//ring buffer wrapped around
			if ((srbaudin.Start + ((tempameta.Curr + tempameta.Length + FT81xAudioPlaybackLeeway) - (srbaudin.Start + srbaudin.Length))) > curAudioPtr) {
				FT_RQueue_ReadE(&srqaudin, &tempameta);
#if defined(SHOWAUDIOPRINTS)
				printf("IFCC1.\n");
#endif
				FT_RingBuffer_Flush(&srbaudin, tempameta.Length);
				//break;
			}
			else if (curAudioPtr > dataChunkEnd) {
				FT_RQueue_ReadE(&srqaudin, &tempameta);
#if defined(SHOWAUDIOPRINTS)
				printf("FCC2.\n");
#endif
				FT_RingBuffer_Flush(&srbaudin, tempameta.Length);
				//break;
			}
			else {
#if defined(SHOWAUDIOPRINTS)
				printf("Iconsuming2. RDptr: RDptr: %u last:%u start:%u end:%u\n", curAudioPtr, lastAudioPtr, tempameta.Curr, dataChunkEnd);
#endif
				break;
			}
		}
		else if (lastAudioPtr == curAudioPtr) {		//data is not consuming
#if defined(SHOWAUDIOPRINTS)
			printf("IAudio data is not consuming. RDptr: %u\n", curAudioPtr);
#endif
			break;
		}
		else {
#if defined(SHOWAUDIOPRINTS)
			printf("PCC.\n");
#endif
			break;
		}

		lastAudioPtr = curAudioPtr;
	}
	lastAudioPtr = curAudioPtr;
	/*
		printf("Aud: exit\n\r");
	#undef SHOWAUDIOPRINTS
	*/
}

ft_void_t calculateFrameRotatialScaling(FT_VIDEO_FRAME * pVideoFrame)
{
	ft_float_t horizontalScaleVal = 1, verticalScaleVal = 1;
	ft_uint16_t tempVal;

	if (pVideoFrame->frameWidth > FT_DispWidth) {
		horizontalScaleVal = (ft_float_t)FT_DispWidth / (ft_float_t)pVideoFrame->frameWidth;
	}

	if (pVideoFrame->frameHeight > FT_DispHeight) {
		verticalScaleVal = (ft_float_t)FT_DispHeight / (ft_float_t)pVideoFrame->frameHeight;
	}

	if (horizontalScaleVal == verticalScaleVal) {
		pVideoFrame->scaleFactor = 65535;
		pVideoFrame->finalFrameWidth = pVideoFrame->frameWidth;
		pVideoFrame->finalFrameHeight = pVideoFrame->frameHeight;
	}
	else if (horizontalScaleVal < verticalScaleVal) {
		pVideoFrame->scaleFactor = (ft_float_t)horizontalScaleVal * 65535;
		pVideoFrame->finalFrameWidth = (ft_int16_t)((ft_float_t)pVideoFrame->frameWidth * horizontalScaleVal);
		pVideoFrame->finalFrameHeight = (ft_int16_t)((ft_float_t)pVideoFrame->frameHeight * horizontalScaleVal);
	}
	else if (verticalScaleVal < horizontalScaleVal) {
		pVideoFrame->scaleFactor = (ft_float_t)verticalScaleVal * 65535;
		pVideoFrame->finalFrameWidth = (ft_int16_t)((ft_float_t)pVideoFrame->frameWidth * verticalScaleVal);
		pVideoFrame->finalFrameHeight = (ft_int16_t)((ft_float_t)pVideoFrame->frameHeight * verticalScaleVal);
	}
	pVideoFrame->zoomOut = 0;
}

ft_void_t setControlButtonsPosition(FT_VIDEO_FRAME * pVideoFrame)
{
	FT_GUI_BUTTONS_POS* pControlButtons = pVideoFrame->pControlButtons;
	int xdist, ydist;

	//TODO: Default, maybe change for CLEO50
	ydist = 0;
	if (FT_DispWidth >= 600) {
		xdist = 120;
	}
	else {
		xdist = 80;
	}

	//Status bar
	pControlButtons->statusXPos = xdist;
	pControlButtons->statusYPos = ydist;
	pControlButtons->statusWidth = FT_DispWidth - xdist;
	//Volume control bar
	pControlButtons->volXPos = xdist;
	pControlButtons->volYPos = FT_DispHeight - 5 * MAINUI_ELEMENT_HEIGHT / 2 - 10;
	pControlButtons->volWidth = FT_DispHeight - 4 * MAINUI_ELEMENT_HEIGHT - 10;
}

#define ENABLE_SCREEN_ROTATE
//NOTE NOTE: Cleo35 default is Inverted Landscape
/*
#undef FT_VIDEO_LANDSCAPE
#define FT_VIDEO_LANDSCAPE 1
*/

ft_void_t rotateScreen(FT_VIDEO_FRAME * pVideoFrame, FT_VIDEO_ORIENTATION orientation)
{
	ft_uint16_t tempVal;

	if (pVideoFrame->rotation != orientation) {
#ifdef ENABLE_SCREEN_ROTATE
		tempVal = FT_DispWidth;
		FT_DispWidth = FT_DispHeight;
		FT_DispHeight = tempVal;
#endif
		pVideoFrame->rotation = orientation;
		calculateFrameRotatialScaling(pVideoFrame);
		setControlButtonsPosition(pVideoFrame);
	}
#ifdef ENABLE_SCREEN_ROTATE
	Ft_Gpu_CoCmd_SetRotate(s_pHalContext, orientation);
#endif //ENABLE_SCREEN_ROTATE
}

ft_void_t applyFrameRotationalScaling(FT_VIDEO_FRAME * pVideoFrame, FT_VIDEO_ORIENTATION orientation)
{
	ft_uint32_t old_mutex;

	old_mutex = guimutex;
	guimutex = 1;
	rotateScreen(pVideoFrame, orientation);
	if (pVideoFrame->scaleFactor != 65535)
	{
#ifdef ENABLE_SCREEN_ROTATE
		Ft_Gpu_CoCmd_LoadIdentity(s_pHalContext);
		Ft_Gpu_CoCmd_Scale(s_pHalContext, pVideoFrame->scaleFactor, pVideoFrame->scaleFactor);
		Ft_Gpu_CoCmd_SetMatrix(s_pHalContext);
#endif
	}
	guimutex = old_mutex;
	/*printf("Rot: %d %d %dx%d %dx%d (%dx%d)\n\r",
			pVideoFrame->scaleFactor, orientation,
			FT_DispWidth, FT_DispHeight,
			pVideoFrame->finalFrameWidth, pVideoFrame->finalFrameHeight,
			pVideoFrame->frameWidth, pVideoFrame->frameHeight);*/
}

ft_void_t restoreFrameRotationalScaling(FT_VIDEO_FRAME * pVideoFrame)
{
	ft_uint32_t old_mutex;

	old_mutex = guimutex;
	guimutex = 1;
	DefaultGUIScreen(s_pHalContext);
	pVideoFrame->scaleFactor = 65535;
	applyFrameRotationalScaling(pVideoFrame, FT_VIDEO_LANDSCAPE);

#ifdef ENABLE_SCREEN_ROTATE
	Ft_Gpu_CoCmd_LoadIdentity(s_pHalContext);
	Ft_Gpu_CoCmd_Scale(s_pHalContext, pVideoFrame->scaleFactor, pVideoFrame->scaleFactor);
	Ft_Gpu_CoCmd_SetMatrix(s_pHalContext);
	Ft_Gpu_CoCmd_SetRotate(s_pHalContext, FT_VIDEO_LANDSCAPE);
#endif //ENABLE_SCREEN_ROTATE
	guimutex = old_mutex;
}

void aviplayerUiInit(void)
{
#define INIT_ICON(icon_name) do{\
	g_global_ctx->iconList[AVIPLAYER_ICON_##icon_name].iconId = FTICON_ID_##icon_name;\
}while(0)
	INIT_ICON(PAUSE);
	INIT_ICON(PLAY);
	INIT_ICON(EXIT);
	INIT_ICON(BACK);
	INIT_ICON(SPEAKER);
	INIT_ICON(VOL_MUTE);
	INIT_ICON(ROTATE);
	//Load and render refresh ICON
	int ret;

	printf("aviplayerUiInit(): enter\n");
	for (int i = 0; i < AVIPLAYER_ICON_MAX; i++) {
		//NOTE NOTE!!!!: CleO reserve ~ 72KB at top GRAM for pngload, audio playback buffer ....
		//Loading these icon will overlap with those memory but we are safe because the memory for icons are small, ~10KB
		ret = helperLoadIcon(s_pHalContext, &g_global_ctx->gram, "/Icons/m48.ftico", &g_global_ctx->iconList[i]);
		if (ret < 0) {
			printf("Fail to load icon: %d %d\n\r", i, g_global_ctx->iconList[i].iconId);
			g_global_ctx->iconList[i].bmId = 0xff;
		}
		else {
			printf("Icon loading success: %d %d\n\r", i, g_global_ctx->iconList[i].iconId);
			g_global_ctx->iconList[i].bmId = i + 1;
		}
	}
	printf("gram_free: %d\n\r", g_global_ctx->gram.gram_free);
}

ft_void_t drawControlButtons(FT_GUI_BUTTONS_POS * pControlButtons)
{
	int xdist, ydist;
	//char buffer[16];

	ydist = 0;
	if (FT_DispWidth >= 600) {
		xdist = 120;
	}
	else {
		xdist = 80;
	}
	EVE_Cmd_wr32(s_pHalContext, TAG_MASK(1));

	////////////////////////////////Render header//////////////////////////////////////////////////////////
	helperRenderHeaderPanel(s_pHalContext, xdist, ydist,
		&g_global_ctx->iconList[AVIPLAYER_ICON_BACK],
		&g_global_ctx->iconList[AVIPLAYER_ICON_EXIT],
		G_S_PGui.CurrTTag, G_S_PGui.PrevTTag);
	////////////////////////////////End of Render header////////////////////////////////////////////////////

	////////////////////////////////Render Volume Control///////////////////////////////////////////////////
	//Side bar to display the volume in case background is white!
	helperRenderRect(s_pHalContext, 0, MAINUI_ELEMENT_HEIGHT,
		(xdist * 12) >> 4, FT_DispHeight - 3 * MAINUI_ELEMENT_HEIGHT / 2 - 10,
		LIGHT_GREY_COLOR(), 255);

	ydist = FT_DispHeight - 5 * MAINUI_ELEMENT_HEIGHT / 2;
	if (g_global_ctx->volume_level < 2) {
		helperRenderBitmapIcon(s_pHalContext, &g_global_ctx->iconList[AVIPLAYER_ICON_VOL_MUTE],
			xdist / 2, ydist, xdist, MAINUI_ELEMENT_HEIGHT, MEDIAPLAYER_KEY_VOL_CONTROL, 0);
	}
	else {
		helperRenderBitmapIcon(s_pHalContext, &g_global_ctx->iconList[AVIPLAYER_ICON_SPEAKER],
			xdist / 2, ydist, xdist, MAINUI_ELEMENT_HEIGHT, MEDIAPLAYER_KEY_VOL_CONTROL, 0);
	}
	ydist = pControlButtons->volYPos;
	xdist = pControlButtons->volXPos;

	helperRenderVolumeBar(s_pHalContext, xdist, ydist << 1, (pControlButtons->volYPos - pControlButtons->volWidth) << 1,
		MEDIAPLAYER_KEY_VOL_CONTROL, g_global_ctx->volume_level, FT81x_MAX_VOLUME_LEVEL);
	//////End volume control

	////////////////////////////////End of Render Volume Control////////////////////////////////////////////

	if (g_global_ctx->fname != NULL) {
		struct RenderArea_t area;

		area.w = FT_DispWidth - xdist;
		area.h = MAINUI_ELEMENT_HEIGHT * 2;
		area.x = xdist;
		area.y = FT_DispHeight - 3 * MAINUI_ELEMENT_HEIGHT;
		if (FT_DispWidth >= 600) {
			helperRenderAlignedString(s_pHalContext, g_global_ctx->fname, MEDIA_FONT_HUGE, &area, ALIGN_VIDEO);
		}
		else {
			helperRenderAlignedString(s_pHalContext, g_global_ctx->fname, MEDIA_FONT_MEDIUM, &area, ALIGN_VIDEO);
		}
	}
	//Bottom line
	helperRenderRect(s_pHalContext, 0, FT_DispHeight - 3 * MAINUI_ELEMENT_HEIGHT / 2 - 10,
		FT_DispWidth, FT_DispHeight,
		LIGHT_GREY_COLOR(), 255);
	////////////////////////////////Render Volume Progress bar//////////////////////////////////////////////
	if (FT_DispWidth >= 600) {
		xdist *= 2;
	}
	{

		ydist = FT_DispHeight - MAINUI_ELEMENT_HEIGHT - 5;
		pControlButtons->statusXPos = xdist / 2;
		pControlButtons->statusYPos = ydist;
		pControlButtons->statusWidth = FT_DispWidth - xdist;
		helperRenderProgressBar(s_pHalContext, xdist, ydist, FT_DispWidth - xdist / 2,
			G_S_AviParser.SVid.CurrFrame * FTPayer_VideoRate / 1000,
			G_S_AviParser.SVid.TotFrames * FTPayer_VideoRate / 1000,
			MEDIAPLAYER_KEY_PROGBAR0,
			(float)G_S_AviParser.SVid.CurrFrame / (float)G_S_AviParser.SVid.TotFrames);
	}
	if (FT_DispWidth >= 600) {
		xdist /= 2;
	}
	////////////////////////////////End of Progress bar//////////////////////////////////////////////

	////////////////////////////////Render Rotate/Play/Pause at bottom screen///////////////////////////////
	//Play/Pause icon
	if (G_S_PGui.TouchSel & 0x01) {
		helperRenderBitmapIcon(s_pHalContext, &g_global_ctx->iconList[AVIPLAYER_ICON_PLAY],
			FT_DispWidth / 2, FT_DispHeight - MAINUI_ELEMENT_HEIGHT, xdist, MAINUI_ELEMENT_HEIGHT, MEDIAPLAYER_KEY_PAUSE_PLAY,
			0);
	}
	else {
		helperRenderBitmapIcon(s_pHalContext, &g_global_ctx->iconList[AVIPLAYER_ICON_PAUSE],
			FT_DispWidth / 2, FT_DispHeight - MAINUI_ELEMENT_HEIGHT, xdist, MAINUI_ELEMENT_HEIGHT, MEDIAPLAYER_KEY_PAUSE_PLAY,
			0);
	}
	//Rotate icon
	helperRenderBitmapIcon(s_pHalContext, &g_global_ctx->iconList[AVIPLAYER_ICON_ROTATE],
		FT_DispWidth - xdist / 2, FT_DispHeight - MAINUI_ELEMENT_HEIGHT, xdist, MAINUI_ELEMENT_HEIGHT, MEDIAPLAYER_KEY_ROTATE,
		0);
	printf("AviPlayer render rotate: %d %d %d \n\r", xdist, FT_DispWidth, g_global_ctx->iconList[AVIPLAYER_ICON_ROTATE].w);
	EVE_Cmd_wr32(s_pHalContext, TAG(255));

	////////////////////////////////End of Render Rotate/Play/Pause at bottom screen/////////////////////////
}

#if defined(AUDIO_VIA_FT900)
///the size to be sent has to be greater than 1 and word aligned
ft_int32_t sendToAudioBuffer(FT_RingBuffer_t * prb, ft_uint32_t size, ft_uint32_t * readSize)
{
#if defined(FT900_PLATFORM) && defined(AUDIO_VIA_FT900)
	ft_uint32_t sendBytes = 0;
	ft_uint32_t tempVal;

	if (size <= 1)
		return 0;

	if (size & 0x00000001)
	{
		size -= 1;
	}

	if (size > prb->ValidLength)
	{
		size = prb->ValidLength;
	}

	if ((prb->Read + size) > (prb->Start + prb->Length))
	{ ///split data
		ft_uint32_t firstPartial, secondPartial;
		ft_uint8_t tempChunk[2];
		ft_uint32_t* ringBufferPointer;

		firstPartial = prb->Start + prb->Length - prb->Read;
		if (firstPartial & 0x00000001)
		{
			printf("Odd sized first partial audio chunk.\n");
			firstPartial -= 1;

			sendBytes += FT_Audio_Play((prb->Read), firstPartial); //original
			ringBufferPointer = (ft_uint8_t*)(prb->Start + prb->Length - 1);
			tempChunk[0] = *(ringBufferPointer);
			ringBufferPointer = (ft_uint8_t*)(prb->Start);
			tempChunk[1] = *(ringBufferPointer);
			sendBytes += FT_Audio_Play(tempChunk, 2);

			prb->Read = prb->Start + 1;

			secondPartial = size - firstPartial - 1;

			if (secondPartial > 0)
			{
				sendBytes += FT_Audio_Play((prb->Start + 1), secondPartial);
				prb->Read = prb->Start + secondPartial + 1;
			}
		}
		else
		{
			//sendBytes += FT_Audio_Play((prb->Read), firstPartial);  //original
			sendBytes += FT_Audio_Play((ft_uint8_t*)(prb->Read), firstPartial);
			secondPartial = size - firstPartial;
			if (secondPartial > 0)
			{
				if (secondPartial & 0x00000001)
					printf("Odd sized second partial chunk.\n");
				sendBytes += FT_Audio_Play((ft_uint8_t*)(prb->Start), secondPartial);
			}
			prb->Read = prb->Start + secondPartial;
		}
	}
	else
	{
		sendBytes += FT_Audio_Play((ft_uint8_t*)(prb->Read), size);
		prb->Read += size;
	}
	prb->ValidLength -= size;
	*readSize = sendBytes;
#elif defined(AUDIO_VIA_FT81X)
	ft_uint32_t firstchunk, secondchunk;

	if (size == 0) {
		return 0;
	}

	if ((prb->Read + size) >= (prb->Start + prb->Length)) {
		firstchunk = (prb->Start + prb->Length) - prb->Read;
		secondchunk = size - firstchunk;
		*readSize = Fifo_Write(s_pHalContext, &audioFifo, (ft_uint8_t*)prb->Read, firstchunk);
		prb->Read = prb->Start;
		if (secondchunk > 0) {
			*readSize += Fifo_Write(s_pHalContext, &audioFifo, (ft_uint8_t*)prb->Start, secondchunk);
			prb->Read += secondchunk;
		}
	}
	else {
		*readSize = Fifo_Write(s_pHalContext, &audioFifo, (ft_uint8_t*)prb->Read, size);
		prb->Read += size;
	}
	prb->ValidLength -= size;
#endif
	return 0;
}

/*
 If the audio frames data size within the play time is less than the maximum amount allowed within the display frame
 then pad the beginning of a full display chunk with x amount of zeros to the fifo to make up the time.

 padding will only occur when the audio buffer is able to buffer all the audio chunks for the current frame.
 */
#define AUDIO_CHUNK_PADDING_LEEWAY 2048
ft_uint32_t getAudioPadAmount(FT_AVIPlayer_t * AviPlayer, FT_AVIParser_t * pCtxt, FT_AudDataPath_t * padp)
{
	ft_int32_t curFrame, totalSz = 0, maxDataBetweenFrames, paddingAmount;
	ft_uint8_t i, framesCount = 0, calculatePadding = 0;
	FT_MetaA_t tempameta;
	FT_RingQueue_t* arq = padp->pairq;

	if (arq->ValidEle == 0) {
		//printf("no audio data.\n");
		return 0;
	}

	FT_RQueue_PeekE(arq, &tempameta, arq->ReadIdx);
	curFrame = tempameta.Frame;
	totalSz += tempameta.Length;

	//not an absolute indication that all the audio frames have been buffered.
	FT_RQueue_PeekELatest(arq, &tempameta);
	if (curFrame == tempameta.Frame) {
		//printf("No full audio frames.");
		return 0;
	}

	for (i = 1; i < (arq->ValidEle); i++) {
		FT_RQueue_PeekE(arq, &tempameta, (arq->ReadIdx) + i);

		if (curFrame != tempameta.Frame) {
			calculatePadding = 1;
			break;
		}

		totalSz += tempameta.Length;
		framesCount++;
	}

	if (calculatePadding) {
		maxDataBetweenFrames = (((ft_uint32_t)pCtxt->SAud.NumChannels * pCtxt->SAud.SamplingFreq * G_S_AviParser.SAud.BitsPerSample) * (G_S_AviPlayer.DisplayFrameMSinterval / 1000000.0)) / 8;

		if ((maxDataBetweenFrames - AUDIO_CHUNK_PADDING_LEEWAY) > totalSz) {
			paddingAmount = (maxDataBetweenFrames - totalSz) / framesCount;
			return ((paddingAmount & 0x01) ? paddingAmount - 1 : paddingAmount);
		}
		else {
			//printf("no need to pad\n");
			return 0;
		}
	}
	return 0;
}

ft_int32_t playAudioData(FT_AVIPlayer_t * AviPlayer, FT_AVIParser_t * pCtxt, FT_AudDataPath_t * padp, FT_VidDataPath_t * pvdp)
{
	//debug static variables, remove me
	static ft_uint32_t waitFrameIndex = 0, partialFrameIndex = 0;
	FT_MetaA_t tempameta;
	FT_MetaV_t tempvmeta;
	static ft_uint32_t curAudioFrame = 0, paddingAmount, curPaddingAmount;
	ft_uint8_t matchingVideoFrame = 0;

	ft_uint32_t wroteBytes = 0, writeBytes, writeAmount, audioFreeSpace;
	ft_int32_t fResult;

#if defined(AUDIO_VIA_FT900)
	audioFreeSpace = FT_Audio_Getfreespace();
#endif

	fResult = FT_RQueue_PeekE(&srqaudin, &tempameta, srqaudin.ReadIdx); ///look at the oldest chunk

	if (FT_RINGQUEUE_NOVALIDDATA == fResult || G_S_AviPlayer.StateCurr == FT_AVIPLAYER_PAUSE) {
#if defined(AUDIO_VIA_FT900)
		G_S_AviPlayer.AudioLowFifoDroppedFrameCount++;

		if (FT_RINGQUEUE_NOVALIDDATA == fResult) {
			if (!(pCtxt->Flags & FT_AVIPARSER_DROPAUDIO) && !(pCtxt->Flags & FT_AVIPARSER_DROPVIDEO)) {
				//printf("%u ",pvdp->pvirq->ValidEle); //REMOVE ME - print
				//if (dropSlowVideoFrame == 0) {
				if ((pvdp->pvirq->ValidEle >= FT_EMPTY_AUDIO_DROP_VIDEO_LEEWAY) && dropSlowVideoFrame == 0) {
					dropSlowVideoFrame = 1;
					//printf("-%u\n",pvdp->pvirq->ValidEle);
				}
			}
		}

		FT_Audio_Fill_With_Zeros();
#endif
		return 0;
	}

	if (!(pCtxt->AudioStreamFound)) {

	}
	else if (pCtxt->Flags & FT_AVIPARSER_DROPAUDIO) {
		if (srqaudin.ValidEle >= 1)
			FT_RQueue_ReadE(&srqaudin, &tempameta);
	}
	else if (!(pCtxt->Flags & FT_AVIPARSER_DROPVIDEO)) {

#if 0
		if (tempameta.PartialSz == 0 && tempameta.ReadSz == 0)
		{
			ft_uint32_t leeway = AviPlayer->CurrFrame >= OLD_AUDIO_FRAME_THREASHOLD ? AviPlayer->CurrFrame - OLD_AUDIO_FRAME_THREASHOLD : 0 /*((AviPlayer->CurrFrame - OLD_FRAME_THREASHOLD) * (-1))*/;
			if (tempameta.Frame <= (AviPlayer->CurrFrame + NEWER_AUDIO_FRAME_THREASHOLD))
			{
				matchingVideoFrame = 1;
#if defined(SHOWPRINTS) || defined(SHOWAUDIOPRINTS)
				printf("PA: %u %u %u\n", tempameta.Frame, leeway, AviPlayer->CurrFrame);
#endif
			}
			else
			{ //wait for video
#if defined(SHOWPRINTS) || defined(SHOWAUDIOPRINTS)
				if (tempameta.Frame != waitFrameIndex)
				{
					printf("wa1. p:%u a:%u\n", AviPlayer->CurrFrame, tempameta.Frame);
					waitFrameIndex = tempameta.Frame;
				}
#endif
			}
		}
#else
		ft_uint32_t leeway = AviPlayer->CurrFrame >= OLD_AUDIO_FRAME_THREASHOLD ? AviPlayer->CurrFrame - OLD_AUDIO_FRAME_THREASHOLD : 0 /*((AviPlayer->CurrFrame - OLD_FRAME_THREASHOLD) * (-1))*/;
		if ((tempameta.Frame >= leeway) && (tempameta.Frame <= (AviPlayer->CurrFrame + NEWER_AUDIO_FRAME_THREASHOLD))) {
			matchingVideoFrame = 1;

			if (tempameta.Frame > (AviPlayer->CurrFrame + OLD_AUDIO_FRAME_THREASHOLD)) {
				if (dropSlowVideoFrame == 0) {
					//if ((pvdp->pvirq->ValidEle >= FT_EMPTY_AUDIO_DROP_VIDEO_LEEWAY) && dropSlowVideoFrame == 0) {
					dropSlowVideoFrame = 1;
					//printf("--%u\n",pvdp->pvirq->ValidEle);  //REMOVE ME
#if defined(SHOWAUDIOPRINTS)
					printf("Adrop: %u %u %u\n", tempameta.Frame, AviPlayer->CurrFrame, AviPlayer->CurrFrame + OLD_AUDIO_FRAME_THREASHOLD);
#endif
				}
			}
#if defined(SHOWAUDIOPRINTS)
			printf("PA: %u %u %u %u\n", tempameta.Frame, leeway, AviPlayer->CurrFrame, tempameta.Pts);
#endif
		}
#endif
		else if (tempameta.ReadSz != 0) { //partial audio frame has been read, must read the remaining chunk.
			matchingVideoFrame = 1;
#if defined(SHOWAUDIOPRINTS)
			if (tempameta.Frame != partialFrameIndex)
			{
				partialFrameIndex = tempameta.Frame;
				printf("rrad: %u\n", tempameta.Frame);
			}
#endif
		}
		else if (tempameta.Frame < leeway) {
			FT_RQueue_ReadE(&srqaudin, &tempameta);
			FT_RingBuffer_Flush(padp->pairb, tempameta.Length);
			G_S_AviPlayer.AudioDroppedFrames++;
			lateAudioZeros++;
#if defined(SHOWAUDIOPRINTS)
			printf("DOA: %u %u %u\n", tempameta.Frame, AviPlayer->CurrFrame, leeway);
#endif
		}
		else {
			waitAudioZeros++;
#if defined(SHOWAUDIOPRINTS)
			//printf("wfaf: %u %u %u\n",AviPlayer->CurrFrame, pvdp->pvirq->ValidEle, pvdp->pvirq->ValidEle);
			printf("wfaf:%u %u %u %u \n", tempameta.Frame, AviPlayer->CurrFrame, padp->pairq->ValidEle, pvdp->pvirq->ValidEle);
#endif

		}
	}
	else {
		matchingVideoFrame = 1;
	}

	if (!matchingVideoFrame) {
#if defined(AUDIO_VIA_FT900)
		FT_Audio_Fill_With_Zeros();
#elif defined(AUDIO_VIA_FT81X)
		//fill 1 frame time worth of zeros for videos that do not have full length audio.
#endif
		return 0;
	}

	if (srqaudin.ValidEle >= 1)  ///one whole chunk of data be can loaded
	{
		if (tempameta.Length == 0) {
			FT_RQueue_ReadE(&srqaudin, &tempameta);
		}
		else if (tempameta.ReadSz == 0 && tempameta.PartialSz == 0) {
#if defined(FT900_PLATFORM) && defined(AUDIO_VIA_FT900)
			writeBytes = FT_Audio_Getfreespace();

			if (writeBytes == 0)
				return wroteBytes;

			if (writeBytes & 0x00000001)
			{
				writeBytes -= 1;
			}
			if (writeBytes > tempameta.Length)
			{
				writeBytes = tempameta.Length;
			}
#elif defined(AUDIO_VIA_FT81X)
			writeBytes = Fifo_GetFreeSpace(s_pHalContext, &audioFifo);
			if (writeBytes > tempameta.Length) {
				writeBytes = tempameta.Length;
			}
#else
			writeBytes = tempameta.Length - tempameta.Length % 2;
#endif

#if defined(FT900_PLATFORM) && (defined(AUDIO_VIA_FT900) || defined(AUDIO_VIA_FT81X))
			sendToAudioBuffer(&srbaudin, writeBytes, &wroteBytes);
#else
			wroteBytes = writeBytes;
#endif
			G_S_AviPlayer.TotalBytes += wroteBytes; //total number of bytes consumed till now by decoder

			if ((tempameta.Length == wroteBytes)) {
				FT_RQueue_ReadE(&srqaudin, &tempameta);
			}
			else if ((tempameta.Length - 1) == wroteBytes) {
				printf("Flush dangling audio byte(1)\n");
				FT_RingBuffer_Flush(padp->pairb, 1);
				FT_RQueue_ReadE(&srqaudin, &tempameta);
			}
			else {
				tempameta.ReadSz += wroteBytes;
				tempameta.Curr += wroteBytes;
				FT_RQueue_WriteEInplace_Oldest(&srqaudin, &tempameta);
			}
		}

		else if (tempameta.ReadSz != 0 || tempameta.PartialSz != 0) { //check for partial chunk
			if (tempameta.PartialSz == 0) {
				writeAmount = tempameta.Length - tempameta.ReadSz;
#if defined(FT900_PLATFORM) && defined(AUDIO_VIA_FT900)
				writeBytes = FT_Audio_Getfreespace();

				if (writeBytes == 0)
					return wroteBytes;

				if (writeBytes & 0x00000001)
				{
					writeBytes -= 1;
				}

				if (writeBytes > writeAmount)
				{
					if (writeAmount & 0x00000001)
					{
						writeBytes = (writeAmount - 1);
					}
					else
					{
						writeBytes = writeAmount;
					}
				}
#elif defined(AUDIO_VIA_FT81X)
				writeBytes = Fifo_GetFreeSpace(s_pHalContext, &audioFifo);

				if (writeBytes == 0) {
					return wroteBytes;
				}

				if (writeBytes > writeAmount) {
					writeBytes = writeAmount;
				}
#else
				writeBytes = writeAmount - writeAmount % 2;
				wroteBytes = writeBytes;
#endif

#if defined(FT900_PLATFORM)
				sendToAudioBuffer(&srbaudin, writeBytes, &wroteBytes);
#else
				wroteBytes = writeBytes;
#endif

				tempameta.ReadSz += wroteBytes;
				tempameta.Curr += wroteBytes;
				if ((tempameta.ReadSz == tempameta.Length)) {
					FT_RQueue_ReadE(&srqaudin, &tempameta);  //consume the chunk
				}
				else if ((tempameta.ReadSz == (tempameta.Length - 1))) {
					FT_RingBuffer_Flush(padp->pairb, 1);
					printf("Flush dangling audio byte(2).\n");
					FT_RQueue_ReadE(&srqaudin, &tempameta);
				}
				else {
					FT_RQueue_WriteEInplace_Oldest(&srqaudin, &tempameta); ///partial meta data update
				}
				G_S_AviPlayer.TotalBytes += wroteBytes;

			}
			else {
				if (tempameta.PartialSz == tempameta.ReadSz) ///if reading is faster than writing to the buffer
					return wroteBytes;

				writeAmount = tempameta.PartialSz - tempameta.ReadSz;

#if defined(FT900_PLATFORM) && defined(AUDIO_VIA_FT900)
				writeBytes = FT_Audio_Getfreespace();

				if (writeBytes == 0)
					return wroteBytes;

				if (writeBytes & 0x00000001)
				{
					writeBytes -= 1;
				}

				if (writeBytes > writeAmount)
				{
					if (writeAmount & 0x00000001)
					{
						writeBytes = writeAmount - 1;
					}
					else
					{
						writeBytes = writeAmount;
					}

				}
#elif defined(AUDIO_VIA_FT81X)
				writeBytes = Fifo_GetFreeSpace(s_pHalContext, &audioFifo);

				if (writeBytes > writeAmount) {
					writeBytes = writeAmount;
				}
#else
				writeBytes = writeAmount;
				wroteBytes = writeBytes;
#endif

#if defined(FT900_PLATFORM)
				sendToAudioBuffer(&srbaudin, writeBytes, &wroteBytes);
#else
				wroteBytes = writeBytes;
#endif

				tempameta.ReadSz += wroteBytes;
				tempameta.Curr += wroteBytes;
				G_S_AviPlayer.TotalBytes += wroteBytes;
				FT_RQueue_WriteEInplace_Oldest(&srqaudin, &tempameta); ///partial meta data update
			}
		}
	}

	return wroteBytes;
}
#endif //#if defined(AUDIO_VIA_FT900)

#if !defined(MSVC_PLATFORM) && !defined(BT8XXEMU_PLATFORM)
#if defined(AUDIO_VIA_FT900)
ft_void_t i2s_ISR()
{
	if ((audioPlaying == 0) && G_S_AviParser.AudioStreamFound) {
		if (i2s_is_interrupted(MASK_I2S_PEND_FIFO_TX_HALF_FULL)) {
			playAudioData(&G_S_AviPlayer, &G_S_AviParser, &G_SAudDataPath, &G_SVidDataPath);
			if (FT_Audio_Getfreespace() > I2S_TXRX_FIFO_SPACE_HALF) {
				playAudioData(&G_S_AviPlayer, &G_S_AviParser, &G_SAudDataPath, &G_SVidDataPath);
			}
		}
		else if (i2s_is_interrupted(MASK_I2S_PEND_FIFO_TX_EMPTY)) {
			playAudioData(&G_S_AviPlayer, &G_S_AviParser, &G_SAudDataPath, &G_SVidDataPath);
			if (FT_Audio_Getfreespace() > I2S_TXRX_FIFO_SPACE_HALF) {
				playAudioData(&G_S_AviPlayer, &G_S_AviParser, &G_SAudDataPath, &G_SVidDataPath);
			}
		}
	}

}
#endif //AUDIO_VIA_FT900
#endif

ft_void_t initialBitmapSetup(ft_uint16_t aviw, ft_uint16_t avih, FT_VIDEO_FRAME * pVideoFrame, FT_VIDEO_ORIENTATION curOrientation)
{
	if (!(G_S_AviParser.Flags & FT_AVIPARSER_DROPVIDEO) && G_SVidDataPath.pvirq->ValidEle > 0) {
		FT_MetaV_t tempvmeta;
		FT_RQueue_PeekE(&srqvidin, &tempvmeta, srqvidin.ReadIdx);
		if (tempvmeta.PartialSz == 0) {
			FT_RQueue_ReadE(&srqvidin, &tempvmeta);
		}
		else {
			currentPartialFrame = 1;
		}
		Ft_Gpu_CoCmd_LoadImage(s_pHalContext, 0,
			OPT_MEDIAFIFO | OPT_NODL/* | OPT_NOTEAR*/);
		EVE_Cmd_waitFlush(s_pHalContext);
		if (tempvmeta.PartialSz == 0) {
			//Gpu_Hal_WaitCmdfifo_empty(s_pHalContext);
		}
		G_SVidDataPath.NumDecoded++;
		G_S_AviPlayer.CurrFrame++;

	}
}

#if defined(AUDIO_VIA_FT81X)
ft_void_t FT81x_Audio_Setup(ft_uint32_t ramBufferOffset, ft_uint32_t ramBufferLength, ft_uint8_t channelBitLength, ft_uint32_t samplingFreq, ft_uint32_t audioFormat)
{
	Ft_Gpu_Hal_Wr32(s_pHalContext, REG_PLAYBACK_START, ramBufferOffset);
	printf("Audio offset:%u\n", ramBufferOffset);
	Ft_Gpu_Hal_Wr32(s_pHalContext, REG_PLAYBACK_LENGTH, ramBufferLength);
	//printf("Audio length:%u\n", ramBufferLength);
	Ft_Gpu_Hal_Wr16(s_pHalContext, REG_PLAYBACK_FREQ, samplingFreq);   //Frequency
	//printf("Audio sampling:%u\n", samplingFreq);
	if (audioFormat == FT_WAVE_FORMAT_ADPCM || audioFormat == FT_WAVE_FORMAT_ADPCM_IMA_WAV) {
		Ft_Gpu_Hal_Wr8(s_pHalContext, REG_PLAYBACK_FORMAT, ADPCM_SAMPLES);
		printf(" ADPCM audio ");
	}
	else if (audioFormat == FT_WAVE_FORMAT_PCM) {
		Ft_Gpu_Hal_Wr8(s_pHalContext, REG_PLAYBACK_FORMAT, LINEAR_SAMPLES);
		printf(" LINEAR audio ");
	}
	else if (audioFormat == FT_WAVE_FORMAT_MULAW) {
		Ft_Gpu_Hal_Wr8(s_pHalContext, REG_PLAYBACK_FORMAT, ULAW_SAMPLES);
		printf(" ULAW audio ");
	}
	else {
		printf("Unknown audio format value.");
	}
	printf("\n");
	Ft_Gpu_Hal_Wr8(s_pHalContext, REG_PLAYBACK_LOOP, 1);
	Ft_Gpu_Hal_Wr8(s_pHalContext, REG_VOL_PB, 0);
}
#endif

#if defined(AUDIO_VIA_FT81X)
#if defined(AUDIO_VIA_FT900)
ft_void_t align_buffer(ft_uint32_t start, ft_uint32_t length, ft_uint32_t startingOffset)
{
	ft_uint32_t len = length;
	ft_uint32_t count = 0;
	ft_uint32_t offset = startingOffset;

	while (count < len) {
		ft_uint32_t index = offset;
		ft_uint8_t temp = Ft_Gpu_Hal_Rd8(s_pHalContext, index);
		ft_uint32_t index2 = (start + index) % len + startingOffset;

		while (index2 != offset) {
			Ft_Gpu_Hal_Wr8(s_pHalContext, index, Ft_Gpu_Hal_Rd8(s_pHalContext, index2));
			count++;
			index = index2;
			index2 = (start + index) % len + startingOffset;
		}
		Ft_Gpu_Hal_Wr8(s_pHalContext, index, temp);
		count++;
		offset++;
	}
}
#endif //#if defined(AUDIO_VIA_FT900)

ft_void_t reorderMetaData()
{
	if (srqaudin.ValidEle >= 0) {
		ft_uint8_t i;
		FT_MetaA_t tempameta;
		ft_uint32_t offset = srbaudin.Start;
		for (i = 0; i < srqaudin.ValidEle; i++) {
			FT_RQueue_PeekE(&srqaudin, &tempameta, (srqaudin.ReadIdx + i) % srqaudin.TotEle);
			tempameta.Curr = offset;
			offset += tempameta.Length;
			FT_RQueue_WriteE_Update(&srqaudin, &tempameta, (srqaudin.ReadIdx + i) % srqaudin.TotEle);
		}
	}
}

ft_void_t reorderAudioData(Fifo_t * audiofifo)
{
#if defined(AUDIO_VIA_FT900)
	ft_uint32_t freespace, firstchunk, secondchunk;

	if (audiofifo->fifo_wp >= audiofifo->fifo_rp)
	{
		freespace = audiofifo->fifo_len - audiofifo->fifo_wp + audiofifo->fifo_rp;
	}
	else
	{
		freespace = audiofifo->fifo_rp - audiofifo->fifo_wp;
	}

	if (audiofifo->fifo_Type == FT_GPU_AUDIO_FIFO) {
		if (audiofifo->fifo_rp == 0) {
			return;
		}
		else if (audiofifo->fifo_rp > audiofifo->fifo_wp) {

#if 1
			align_buffer(audiofifo->fifo_buff + audiofifo->fifo_rp, audiofifo->fifo_len, audiofifo->fifo_buff);
#else
			firstchunk = audiofifo->fifo_len - audiofifo->fifo_rp;
			secondchunk = audiofifo->fifo_wp;
			freespace = Fifo_GetFreeSpace(s_pHalContext, audiofifo);
			//copy byte by byte
			if (freespace < firstchunk)
			{
				align_buffer(audiofifo->fifo_buff + audiofifo->fifo_rp, audiofifo->fifo_buff + audiofifo->fifo_len, audiofifo->fifo_buff);
				audiofifo->fifo_rp = 0;
				audiofifo->fifo_wp = audiofifo->fifo_len;
			}
			else
			{
				if (secondchunk)
				{
					Ft_Gpu_CoCmd_Memcpy(s_pHalContext, audiofifo->fifo_buff + audiofifo->fifo_wp, audiofifo->fifo_buff + firstchunk, audiofifo->fifo_wp);
				}

				Ft_Gpu_CoCmd_Memcpy(s_pHalContext, audiofifo->fifo_buff, audiofifo->fifo_buff + firstchunk, audiofifo->fifo_len - audiofifo->fifo_rp);
			}
#endif
		}
		else {
			Ft_Gpu_CoCmd_Memcpy(s_pHalContext, audiofifo->fifo_buff, audiofifo->fifo_buff + audiofifo->fifo_rp, audiofifo->fifo_len);
		}
		audiofifo->fifo_rp = 0;
		audiofifo->fifo_wp = audiofifo->fifo_len - freespace;

	}
	else {
		//the input device fifo is not audio fifo type.
	}
#endif //#if defined(AUDIO_VIA_FT900)
}
#endif

#define TEST_BUFFER (10*1024)

void AviPlayer_BufferInit(int isSeeking)
{
#if defined(AUDIO_VIA_FT900)
	/*
	while(FT_Audio_Getfreespace() != I2S_TXRX_FIFO_SPACE) {
		printf("Wait for audio depletion.\n");
	}
	*/

	if (isSeeking) FT_Audio_Stop_Transmission();
	FT_RingBuffer_Init(&srbaudin, AudioBuff, AUDIO_BUFFER_SIZE);
	srbaudin.BufferType = FT_RINGBUFFER_RAM_BUFFER;

#elif defined(AUDIO_VIA_FT81X)
	//TODO: should check if we have audio chunks
	if (isSeeking) Ft_Gpu_Hal_Wr8(s_pHalContext, REG_PLAYBACK_PLAY, 0);
	FT_RingBuffer_Init(&srbaudin, audioFifoOffset, FT800AudioBufferSize);
	printf("audioFifoOffset:%u  FT800AudioBufferSize:%u\n", audioFifoOffset, FT800AudioBufferSize);
	srbaudin.BufferType = FT_RINGBUFFER_FT81X_RAM_G_BUFFER;
	srbaudin.Option = s_pHalContext;
	if (isSeeking) FT81x_Audio_Setup(audioFifoOffset, FT800AudioBufferSize /*G_S_AviParser.SAud.SamplingFreq / (tempvar / 1000.0)*/, G_S_AviParser.SAud.BitsPerSample, G_S_AviParser.SAud.SamplingFreq, G_S_AviParser.SAud.AudioFormat);
#endif
	FT_RQueue_Init(&srqvidin, FT_AVIPLAYER_MAX_VIDIN_QE, (ft_uint8_t*)G_SA_VMetaDInRQ, sizeof(FT_MetaV_t));

	FT_RQueue_Init(&srqaudin, FT_AVIPLAYER_MAX_AUDIN_QE, (ft_uint8_t*)G_SA_AMetaDInRQ, sizeof(FT_MetaA_t));
}

void AviPlayer_RenderFirstFrames(ft_uint8_t * appliedFirstFrame, ft_uint8_t * applyLastFrame, ft_uint8_t * optionalInitialFrameDecode)
{
	if (G_S_AviParser.VideoStreamFound && !(G_S_AviParser.Flags & FT_AVIPARSER_DROPVIDEO)) {
		while (1) {
			if (srqvidin.ValidEle >= 1) {
				FT_MetaV_t tempvmeta;
				FT_RQueue_PeekE(&srqvidin, &tempvmeta, srqvidin.ReadIdx);
				if (tempvmeta.Length > 0) {
					if (tempvmeta.PartialSz == 0) {
						FT_RQueue_ReadE(&srqvidin, &tempvmeta);
					}
					else {
						currentPartialFrame = 1;
					}
					Ft_Gpu_CoCmd_LoadImage(s_pHalContext, 0, OPT_MEDIAFIFO | OPT_NODL/* | OPT_NOTEAR*/);
					EVE_Cmd_waitFlush(s_pHalContext);
					G_SVidDataPath.NumDecoded++;
					G_S_AviPlayer.TotalBytes += tempvmeta.Length;
					//G_S_AviPlayer.CurrFrame++;
					*appliedFirstFrame = 0;
					printf("Initialized first frame.\n");
					break;
				}
				else {
					FT_RQueue_ReadE(&srqvidin, &tempvmeta);
				}
			}
			else {
				printf("No initial frame data.");
				break;
			}
		}

		if (*appliedFirstFrame == 0) {
			*optionalInitialFrameDecode = 0; //don't attempt to apply the first frame
		}
	}
}

void AviPlayer_FifoInit(int isSeeking, ft_uint32_t mediafifo, ft_uint32_t mediafifolen,
	ft_uint8_t * appliedFirstFrame, ft_uint8_t * applyLastFrame, ft_uint8_t * optionalInitialFrameDecode)
{
	ft_uint32_t resultpar;

	Fifo_Init(&stFifo, mediafifo, mediafifolen, REG_MEDIAFIFO_READ, REG_MEDIAFIFO_WRITE);
	Ft_Gpu_CoCmd_MediaFifo(s_pHalContext, mediafifo, mediafifolen);								//address of the media fifo buffer - init of fifo done by coprocessor
	EVE_Cmd_waitFlush(s_pHalContext);
	Gpu_Hal_WaitCmdfifo_empty(s_pHalContext);
	Ft_Gpu_Hal_Wr32(s_pHalContext, stFifo.HW_Write_Reg, stFifo.fifo_wp);

	/* playback video only */
	if (!isSeeking) {
		FT_AVIParser_SetRiff(&G_S_AviParser, 0);
		G_S_AviParser.CurrStat.MoviOff = G_S_AviParser.Riff[0].MoviOff;
	}
	/* Initial filing of input data */
	resultpar = FT_AVIPARSER_OK;
	for (int i = 0; i < 20; i++) {
		resultpar = FT_AVIParser_FillChunk(&G_S_AviParser, &G_SVidDataPath, &G_SAudDataPath);
		if (resultpar != FT_AVIPARSER_OK) {
			break;
		}
	}
	printf("Seek - buffer filled. video_frames:%u , audio_frames:%u\n", G_SVidDataPath.pvirq->ValidEle, G_SAudDataPath.pairq->ValidEle);

	//reset flag/variables
	dropSlowVideoFrame = parserfillmutex = videodecodemutex = guimutex = audioPlaying = audiomutex = updateAudioIndex = parsingVideo = parsingAudio = 0;
	*applyLastFrame = *optionalInitialFrameDecode = 1; //signal to construct the initial display list
	*appliedFirstFrame = 0;
	G_SVidDataPath.NumDecoded = 0;

	G_S_PGui.TouchSel = 0;
	G_S_PGui.PrevTTag = 0;
	G_S_PGui.Disappear = FT_GUI_DISAPPEAR_COUNT;
	G_S_AviPlayer.StateCurr = FT_AVIPLAYER_PLAY;
	G_S_PGui.Flags |= FT_GUI_REFRESH;
#if defined(FT900_PLATFORM) && defined(AUDIO_VIA_FT900)
	for (i = 0; i < 2; i++)
	{
		playAudioData(&G_S_AviPlayer, &G_S_AviParser, &G_SAudDataPath, &G_SVidDataPath);
	}
	printf("Filled i2s audio buffer.\n");
#endif
}

void AviPlayer_AudioInit(void)
{
#if defined(AUDIO_VIA_FT900)
	i2s_clear_int_flag(0xFFFF);
	interrupt_attach(interrupt_i2s, (uint8_t)interrupt_i2s, i2s_ISR); //original
	FT_Audio_UnMute();
	i2s_enable_int(MASK_I2S_IE_FIFO_TX_EMPTY | MASK_I2S_IE_FIFO_TX_HALF_FULL);
	FT_Audio_Start_Transmission();
#elif defined(AUDIO_VIA_FT81X)
#if defined(USEAUDIOINTERRUPT)
	if (!(G_S_AviParser.Flags & FT_AVIPARSER_DROPAUDIO))
	{
		/*timer_start(timer_select_b);*/
		FTPayer_AudioCnt = FTPayer_AudioRate;
	}
#endif
	if (!(G_S_AviParser.Flags & FT_AVIPARSER_DROPAUDIO)) {
		FT_Audio_UnMute();
		Ft_Gpu_Hal_Wr8(s_pHalContext, REG_VOL_SOUND, 0);
		Ft_Gpu_Hal_Wr8(s_pHalContext, REG_VOL_PB, g_global_ctx->volume_level);
		Ft_Gpu_Hal_Wr8(s_pHalContext, REG_PLAYBACK_PLAY, 1);
	}
#endif

#if defined(FT900_PLATFORM)
#if defined(AUDIO_VIA_FT81X)
	if (!(G_S_AviParser.Flags & FT_AVIPARSER_DROPAUDIO) && G_S_AviParser.AudioStreamFound) {
		/*timer_start(timer_select_b);*/
		FTPayer_AudioCnt = FTPayer_AudioRate;
	}
#endif
	FTPayer_VideoCnt = FTPayer_VideoRate;
	timer_start(FT900_FT_MILLIS_TIMER);
	interrupt_enable_globally();
#endif
}

/* Testing avi parser */
/* Main entry point */
extern void FT_Util_StartApp(Ft_Gpu_Hal_Context_t *s_pHalContext);
extern void FT_Util_ExitApp(Ft_Gpu_Hal_Context_t *s_pHalContext);
extern ft_int32_t AppAviPlayer(Ft_Gpu_Hal_Context_t * pHalCtxt, const char* fname, ft_int16_t Loop, int updateProgress);

#ifdef SUPPORT_MEDIAPLAYER_RETAIN_STATUS
extern int MultimediaExplorer_SetFileProgress(const char* filename, uint8_t percent);
extern int8_t MultimediaExplorer_GetFileProgress(const char* filename);
#endif //SUPPORT_MEDIAPLAYER_RETAIN_STATUS

#if defined(FT900_PLATFORM)
extern size_t xPortGetFreeHeapSize();
#endif

static int aviPlayerSeekTo(ft_int8_t percent)
{
	//update the 'movi' offset
	ft_uint32_t curSeekedOffset = 0;
	curSeekedOffset = FT_AVIParser_Seek_To(percent, &G_S_AviParser); //percentage of the avi file to seek to
	printf("offset after seeking: %u\n", curSeekedOffset);
	G_S_AviParser.CurrRiff = 0;
	G_S_AviParser.CurrStat.MoviOff = curSeekedOffset;
	while (1) {
		if (curSeekedOffset >= (G_S_AviParser.Riff[G_S_AviParser.CurrRiff].MoviOff + G_S_AviParser.Riff[G_S_AviParser.CurrRiff].MoviSize)) {
			G_S_AviParser.CurrRiff++;
			//printf("curRif:%u seekOffset:%x moviOff:%x moviSize:%x\n", G_S_AviParser.CurrRiff, curSeekedOffset, G_S_AviParser.Riff[G_S_AviParser.CurrRiff].MoviOff, G_S_AviParser.Riff[G_S_AviParser.CurrRiff].MoviSize);
			if (G_S_AviParser.CurrRiff >= G_S_AviParser.NumRiff) {
				//printf("Cur:%x %x %x %x %x\n", G_S_AviParser.CurrRiff, G_S_AviParser.CurrStat.MoviOff, G_S_AviParser.Riff[G_S_AviParser.CurrRiff - 1].MoviSize, G_S_AviParser.Riff[G_S_AviParser.CurrRiff - 1].MoviOff, curSeekedOffset);
				G_S_AviParser.CurrRiff = G_S_AviParser.NumRiff; //protection
				break;
			}
		}
		else {
			break;
		}
	}

	if (G_S_AviParser.CurrRiff >= G_S_AviParser.NumRiff) {
		return 1;
	}
	G_S_AviParser.SVid.CurrFrame = G_S_AviPlayer.CurrFrame = G_S_AviParser.curFrame = (G_S_AviParser.SVid.TotFrames * percent) / 100;
	FT_AVIParser_SetBuff(&G_S_AviParser, G_PBuff, FT_AVIPARSER_BUFFER_SIZE_NEW);
	return 0;
}

ft_int32_t AppAviPlayer(Ft_Gpu_Hal_Context_t * pHalCtxt, const char* fname, ft_int16_t Loop, int needUpdProg)
{
		//ft_uint32_t i, j;
	ft_uint32_t mediafifo, mediafifolen;
	ft_float_t audioCheckInterval;
	ft_int32_t aviw, avih;
	//ft_uint32_t tempVal,
	ft_uint32_t returnVal;
	//ft_uint8_t tempCounter = 0;
	ft_uint8_t frameRotationalScalingApplied = 0, applyLastFrame = 0, appliedFirstFrame = 0;
	//ft_uint8_t ignoreFirstTouch, firstTime = 0;
	ft_uint32_t prevSelect = 0;
	ft_uint16_t audioIRQstatus = 0;
	ft_int16_t touchXPos = 0, touchYPos = 0;
	//ft_uint32_t lastTouchCounter, audioFifoLength;
	ft_uint8_t optionalInitialFrameDecode = 0;
	FT_VIDEO_ORIENTATION curOrientation = FT_VIDEO_LANDSCAPE;
	ft_int32_t retVal = MEDIAPLAYER_KEY_BACK;

#if defined(FT900_PLATFORM)
	printf("Free mem: %d, req: %d file %s\n\r", xPortGetFreeHeapSize(), sizeof(*g_global_ctx), fname);
#endif
#if defined(MSVC_PLATFORM) || defined(BT8XXEMU_PLATFORM)
	MSVCTimeInit();
#endif
	s_pHalContext = pHalCtxt;
	//s_pHalContext->cmd_fifo_wp = Ft_Gpu_Hal_Rd16(s_pHalContext, REG_CMD_WRITE);

	//size_t xPortGetFreeHeapSize( void );
	/*
	 * Malloc the internal data structure
	 * NOTE NOTE: the printf library take 1032 bytes after initialized!
	 * NOTE NOTE: The Malloc from heap-4 will align the size for 4 bytes so that we are safe here!
	 */
	 //g_global_ctx = malloc(sizeof(*g_global_ctx));
	g_global_ctx = malloc(sizeof(*g_global_ctx));
	if (g_global_ctx == NULL) {
		//uart_puts(UART0, "AviPlayer: Fail to allocate global data!\n\r");
#if defined(FT900_PLATFORM)
		printf("AviPlayer: Fail to allocate global data! req %d but have %d \n\r", sizeof(*g_global_ctx), xPortGetFreeHeapSize());
#else
		printf("AviPlayer: Fail to allocate global data! req %d\n\r", sizeof(*g_global_ctx));
#endif
		return ERROR_NOTENOUGHCORE;
	}
	memset(g_global_ctx, 0, sizeof(*g_global_ctx));
#if defined(FT900_PLATFORM)
	ft_millis_exit();
#endif
	updateFrame = 1;
	g_global_ctx->gram.gram_start = 0;
	g_global_ctx->gram.gram_free = RAM_G_SIZE;

	videoFrame.pControlButtons = &controlButtons;
	videoFrame.rotation = FT_VIDEO_LANDSCAPE;
	/* Initialize all the global contexts */
	G_S_PGui.CurrTTag = 0;
	G_S_PGui.PrevTTag = 0;
	G_S_PGui.TouchSel = 0;	//max fps
	G_S_PGui.VRateK = 0;
	G_S_PGui.Flags = FT_GUI_NONE;
	G_S_PGui.Disappear = FT_GUI_DISAPPEAR_COUNT;

	/* Player context */
	G_S_AviPlayer.AudioCodec = 0;
	G_S_AviPlayer.CurrTime = 0;
	G_S_AviPlayer.StateCurr = FT_AVIPLAYER_INIT;	//init state
	G_S_AviPlayer.StateNext = 0;	//none
	G_S_AviPlayer.VideoCodec = 0;
	G_S_AviPlayer.TotalBytes = 0;	//total number of bytes consumed till now
	G_S_AviPlayer.CurrFrame = 0;
	G_S_AviPlayer.AudioDroppedFrames = 0;
	G_S_AviPlayer.VideoLackFrames = 0;
	/* Video data path context */
	G_SVidDataPath.pfifo = &stFifo;
	G_SVidDataPath.phalctxt = s_pHalContext;
	G_SVidDataPath.pvirq = &srqvidin;
	G_SVidDataPath.pvorb = NULL;
	G_SVidDataPath.NumDecoded = 0;

	/* Audio data path context */
	G_SAudDataPath.pairb = &srbaudin;
	G_SAudDataPath.pairq = &srqaudin;
	G_SAudDataPath.paorb = NULL;
	g_global_ctx->fname = fname;

	/*It is optional to clear the screen here*/
	FT_Util_StartApp(s_pHalContext);
#ifdef FT900_PLATFORM
	printf("reg_touch_rz =0x%x\n", Ft_Gpu_Hal_Rd16(s_pHalContext, REG_TOUCH_RZ));
	printf("reg_touch_rzthresh =0x%x\n", Ft_Gpu_Hal_Rd32(s_pHalContext, REG_TOUCH_RZTHRESH));
	printf("reg_touch_tag_xy=0x%x\n", Ft_Gpu_Hal_Rd32(s_pHalContext, REG_TOUCH_TAG_XY));
	printf("reg_touch_tag=0x%x\n", Ft_Gpu_Hal_Rd32(s_pHalContext, REG_TOUCH_TAG));
#endif
	//printf("malloc %d bytes\n\r", sizeof(*g_global_ctx));

	//printf("Scroller init %d \n",(((FT_MAINMENU_DISTANCE*2)+FT_MAINMENU_WIDTH)*FT_MAINMENU_NUMFILES - FT_DispWidth)*16);
	aviplayerUiInit();
	/* GUI Init */
//	home_setup();
//	Info();

	if (Loop == 0) Loop = 1;//Play at least 1 time
	{
		parserfillmutex = 0;
		videodecodemutex = 0;
		guimutex = 0;
		g_global_ctx->volume_level = Ft_Gpu_Hal_Rd8(s_pHalContext, REG_VOL_PB);
		while (Loop) {
			ft_uint32_t resultpar, tempvar = 0;

			if (Loop > 0) {
				Loop--;
			}
			//FIXME: Re-init the screen-size, need to detect Cleo50 or Cleo35
#if defined(FT900_PLATFORM)
			/* Initialize few of the global parameters */
			interrupt_disable_globally();
			ft_timer_interrupt1_exit();
#endif

#if defined(AUDIO_VIA_FT900)
			FT_Audio_Stop_Transmission();
			i2s_disable_int(MASK_I2S_IE_FIFO_TX_EMPTY | MASK_I2S_IE_FIFO_TX_HALF_FULL);
#endif

			G_S_AviPlayer.TotalBytes = 0;
			G_S_PGui.Flags &= (~FT_GUI_REFRESH);
			G_SVidDataPath.NumDecoded = 0;

			/* Parser testing */
			FT_AVIParser_Init(&G_S_AviParser);
			FT_AVIParser_SetBuff(&G_S_AviParser, G_PBuff, FT_AVIPARSER_BUFFER_SIZE_NEW);
			resultpar = FT_AVIParser_SetOpenFile(&G_S_AviParser, fname);
			if (resultpar != FT_AVIPARSER_OK) {
				retVal = ERROR_NOFILE;
				break;
			}

			if (FT_AVIParser_ParseAvi(&G_S_AviParser) != FT_AVIPARSER_OK) {
				printf("Unable to parse input video header structure: %s\n", fname);
				retVal = ERROR_INVALIDFILE;
				break;
			}

#if defined(AUDIO_VIA_FT81X)
			Ft_Gpu_Hal_Wr8(s_pHalContext, REG_VOL_PB, 0);
#endif

			/* GUI init */
			/* construct the DL and display */
			DefaultGUIScreen(s_pHalContext);

			/* Video decoder settings */
			/* start video playback, load the data into media fifo */
			aviw = G_S_AviParser.SVid.Width;
			avih = G_S_AviParser.SVid.Height;
			mediafifo = aviw * avih * 2L + 32L;	//size of 4 is due to the result of video decode
			G_S_AviPlayer.mediafifo = mediafifo = ((mediafifo + 3) & ~3);
			if (G_S_AviParser.SVid.Width & 0x01) {
				printf("*****Unable to play input video. Video frame pixel width must be multiple of 2.*****\n");
				break;
			}
			if (mediafifo > RAM_G_SIZE) {
				printf("*****Unable to play input video. Video resolution is outside of the playable range.*****\n");
				break;
			}

			G_S_AviPlayer.CurrFrame = 0;
			G_S_AviPlayer.AudioDroppedFrames = 0;
			G_S_AviPlayer.VideoLackFrames = 0;
			G_S_AviPlayer.AudioLowFifoDroppedFrameCount = 0;
			optionalInitialFrameDecode = 1;
			actualISRcount = 0;
			partialFrameCount = 0;
			mediafifolen = g_global_ctx->gram.gram_free - mediafifo;
			G_S_AviPlayer.mediafifolen = mediafifolen = mediafifolen - (mediafifolen & 3);

#if defined(AUDIO_VIA_FT81X)

			FT800AudioBufferSize = FT_81X_AUDIO_BUFFER_SIZE;
			mediafifolen -= FT800AudioBufferSize;
			audioFifoOffset = g_global_ctx->gram.gram_free - FT800AudioBufferSize;

			printf("audioFifoOffset:%u  FT800AudioBufferSize:%u\n", audioFifoOffset, FT800AudioBufferSize);
			if ((mediafifo + FT800AudioBufferSize) > RAM_G_SIZE) {
				printf("*****Based on the video resolution, device ram does not have the specified buffer size of: %u for audio, after allocated space for the mediafifo buffer.\n", FT800AudioBufferSize);
				break;
			}
#endif
			printf("MediaFifo: %d %d\n\r", mediafifo, mediafifolen);
			if (mediafifolen < LOW_MEDIAFIFO_THREASHOLD) {
				printf("*****Based on the video resolution, only %d bytes, a relatively low amount, in the FT81x are reserved for frame buffering.  Playback might not be able to show video.*****\n");
			}
			maxVideoFrameSz = mediafifolen;
			/*
			 * Init Audio Path
			 */
			AviPlayer_BufferInit(0);
			/*
			 if(G_SVidDataPath.pvirq->ValidEle < 2){
			 printf("The input video appears to contain video frames that are too large for playback.\n");
			 break;
			 }
			 */

			 /* Steady state of avi player */
			 /* start timer for new stream */
			timercount = 0;
			timervalue = 0;
			/* Timer for fps */
#ifdef FT900_PLATFORM
			ft_timer_interrupt1_exit();		//exit the earlier timer
#endif // FT900_PLATFORM
			timercount = 0, timervalue = 0;
			tempvar = G_S_AviParser.SVid.DataRate / G_S_AviParser.SVid.TimeScale;
			G_S_AviPlayer.DisplayFrameMSinterval = tempvar = 1000.0 / tempvar;
			FTPayer_VideoRate = G_S_AviPlayer.DisplayFrameMSinterval + 1;
#ifdef FT900_PLATFORM
			ft_timer_interrupt1_init(100, (tempvar));
#endif // FT900_PLATFORM
			printf("Video frame time interval: %u(us)\n", tempvar);
#if defined(AUDIO_VIA_FT81X)
			if (G_S_AviParser.SAud.NumChannels > 0) {
				audioCheckInterval = (1.0 / (((G_S_AviParser.SAud.SamplingFreq * G_S_AviParser.SAud.NumChannels * G_S_AviParser.SAud.BitsPerSample) / 8.0) / ((G_S_AviParser.SVid.DataRate / (ft_float_t)G_S_AviParser.SVid.TimeScale) * 4.0))) * 1000;
				FT81xAudioPlaybackLeeway = (((G_S_AviParser.SAud.SamplingFreq * G_S_AviParser.SAud.NumChannels * G_S_AviParser.SAud.BitsPerSample) / 8.0) / ((G_S_AviParser.SVid.DataRate / (ft_float_t)G_S_AviParser.SVid.TimeScale) * 2.0));
				//testing for small audio chunks
				if (audioCheckInterval < 3000) {
					FT81xAudioPlaybackLeeway *= 2; //it might still be too small
					audioCheckInterval *= 2;
				}
				printf("Audio check interval(us):%u\n", (ft_uint32_t)audioCheckInterval);
				/*ft_timer_interrupt2_init(100, (ft_uint32_t) audioCheckInterval);*/
				FTPayer_AudioRate = (ft_uint32_t)audioCheckInterval + 1;
#if defined(USEAUDIOINTERRUPT)
				//Play audio from the FT81x can use the device's sampling rate as a form of timer for the video.
				/*ft_timer_interrupt2_init(100, 10000);*/
#endif
			}
#endif //AUDIO_VIA_FT81X
#if defined(MSVC_PLATFORM) || defined(BT8XXEMU_PLATFORM) 
			G_S_AviPlayer.vidTimestamp = G_S_AviPlayer.audTimestamp = MSVCTimeMsGet();
#else
			AviPlayer_TimerInit();
#endif // defined(MSVC_PLATFORM) || defined(BT8XXEMU_PLATFORM)
			videoFrame.frameWidth = G_S_AviParser.SVid.Width;
			videoFrame.frameHeight = G_S_AviParser.SVid.Height;
			videoFrame.finalFrameWidth = G_S_AviParser.SVid.Width;
			videoFrame.finalFrameHeight = G_S_AviParser.SVid.Height;

			videoFrame.zoomOut = 1;
			/*rotateScreen(&videoFrame, FT_VIDEO_LANDSCAPE);
			curOrientation = FT_VIDEO_LANDSCAPE;
			*/
			calculateFrameRotatialScaling(&videoFrame);
			setControlButtonsPosition(&videoFrame);
#if 0
			//FIXME: disable auto rotation first
			if ((FT_DispWidth > FT_DispHeight && videoFrame.frameWidth > videoFrame.frameHeight) ||
				(FT_DispWidth < FT_DispHeight && videoFrame.frameWidth < videoFrame.frameHeight)) {
				curOrientation = FT_VIDEO_LANDSCAPE;
			}
			else {
				curOrientation = FT_VIDEO_PORTRAIT;
			}
			applyFrameRotationalScaling(&videoFrame, curOrientation);
#endif

			applyLastFrame = 1;
			appliedFirstFrame = 0;

			if ((G_S_AviParser.SVid.Flags & FT_AVIF_MUSTUSEINDEX) == FT_AVIF_MUSTUSEINDEX) {
				printf("Unable to play the input video.  The current playback mode only supports videos with chronologically ordered video data chunks.\n");
				break;
			}

#if defined(AUDIO_VIA_FT900)
			if (G_S_AviParser.SAud.NumChannels != 2)
			{
				printf("FT900 audio Device is unable to playback %u audio channels.  Please recode input file with 2 audio channels.", G_S_AviParser.SAud.NumChannels);
				/*break;*/
			}
#elif defined(AUDIO_VIA_FT81X)
			if (G_S_AviParser.SAud.NumChannels != 1) {
				printf("The input video has %u audio channels.  PWM audio playback via the FT81X has to be 1 audio channel.  Please recode!", G_S_AviParser.SAud.NumChannels);
				/*break;*/
			}
#endif
			/*
			printf("AudioFmt: %d %d %d\n\r",
					G_S_AviParser.SAud.AudioFormat,
					G_S_AviParser.SAud.NumChannels,
					G_S_AviParser.SAud.BitsPerSample);
			*/
#if defined(AUDIO_VIA_FT81X)
			if (G_S_AviParser.SAud.NumChannels == 1 && G_S_AviParser.SAud.BitsPerSample <= 16 /*&&
				(G_S_AviParser.SAud.AudioFormat == FT_WAVE_FORMAT_ADPCM ||
				 G_S_AviParser.SAud.AudioFormat == FT_WAVE_FORMAT_ADPCM_IMA_WAV ||
				 G_S_AviParser.SAud.AudioFormat == FT_WAVE_FORMAT_PCM ||
				 G_S_AviParser.SAud.AudioFormat == FT_WAVE_FORMAT_MULAW)*/) {
#elif defined(AUDIO_VIA_FT900)
			if (G_S_AviParser.SAud.NumChannels == 2) {
#else
			if (0) {
#endif

#if defined(AUDIO_VIA_FT81X)
				FT81x_Audio_Setup(audioFifoOffset, FT800AudioBufferSize /*G_S_AviParser.SAud.SamplingFreq / (tempvar / 1000.0)*/,
					G_S_AviParser.SAud.BitsPerSample, G_S_AviParser.SAud.SamplingFreq, G_S_AviParser.SAud.AudioFormat);
				returnVal = FT_Audio_Playback_Setup(NULL, 16, 44100);
				FT_Audio_Use_Analog_In();

#elif defined(AUDIO_VIA_FT900)
				returnVal = FT_Audio_Playback_Setup(&AudioConfig, G_S_AviParser.SAud.BitsPerSample, G_S_AviParser.SAud.SamplingFreq);
				FT_Audio_Use_I2S();
#endif

#if !defined(MSVC_PLATFORM) && !defined(BT8XXEMU_PLATFORM)
				if (returnVal) {
					if (returnVal == FT_AUDIO_ERROR_UNSUPPORTED_CHANNEL_BIT_LENGTH) {
						printf("The audio channel bit length %u bits-per-channel is unsupported.\nSupported channel bits length are: 16, 20, 24, and 32\n", G_S_AviParser.SAud.BitsPerSample);
						break;
					}

					if (returnVal == FT_AUDIO_ERROR_UNSUPPORTED_SAMPLING_FREQ) {
						printf("The audio sampling frequency %uhz is not supported.\nSupported frequencies are: 32K, 44.1K, 48K, 96K\n", G_S_AviParser.SAud.SamplingFreq);
						break;
					}
			}
#endif
		}
			else {
				G_S_AviParser.Flags |= FT_AVIPARSER_DROPAUDIO;
			}
			AviPlayer_FifoInit(0, mediafifo, mediafifolen, &appliedFirstFrame, &applyLastFrame, &optionalInitialFrameDecode);
#if defined(FT900_PLATFORM) && defined(AUDIO_VIA_FT900)
			/*
			 Fill up the is2 fifo with the initial data to avoid brief moments of lack of data.
			 */
			printf("Initial i2s fifo free space: %u\n", FT_Audio_Getfreespace());
			for (i = 0; i < 2; i++)
			{
				playAudioData(&G_S_AviPlayer, &G_S_AviParser, &G_SAudDataPath, &G_SVidDataPath);
			}
			printf("Finished filling audio buffer. Free space: %u\n", FT_Audio_Getfreespace());
#endif
			/*
			 [Decode and display mjpeg frame]

			 decode an initial frame before playing the video.
			 */
			AviPlayer_RenderFirstFrames(&appliedFirstFrame, &applyLastFrame, &optionalInitialFrameDecode);
			/* [End of decode and display mjpeg frame] */
			AviPlayer_AudioInit();

			//If we need to seek
			if (needUpdProg > 0) {
#ifdef SUPPORT_MEDIAPLAYER_RETAIN_STATUS
				int8_t percent = MultimediaExplorer_GetFileProgress(fname);
				if (percent <= 0 || percent >= 99) {
					needUpdProg = -1;
	}
#else
				needUpdProg = -1;
#endif //SUPPORT_MEDIAPLAYER_RETAIN_STATUS
}
			while (1) {
				ft_int32_t parret = 0, tempgui[2];
				ft_uint32_t screenXY;
				ft_uint8_t pendown = 0;
				//ft_uint8_t partialFrameCheck = 0;
				//ft_uint16_t prevTouchXPos = 0;

#if defined(MSVC_PLATFORM) || defined(BT8XXEMU_PLATFORM)
				//UpdateMSVCFlags();
				MSVCTimerThread();
#endif

#if 1
				/* User touches */
				guimutex = 1;
				Ft_Gpu_Hal_RdMem(s_pHalContext, REG_TOUCH_TAG_XY, tempgui, 8);
				screenXY = Ft_Gpu_Hal_Rd32(s_pHalContext, REG_TOUCH_SCREEN_XY);
				guimutex = 0;
				G_S_PGui.CurrX = (ft_uint16_t)((screenXY >> 16) & 0xffff);
				G_S_PGui.CurrY = (ft_uint16_t)((screenXY) & 0xffff);
				if (screenXY != 0x80008000) {
					touchXPos = (ft_int16_t)(screenXY >> 16) & 0xFFFF;
					touchYPos = (ft_int16_t)(screenXY) & 0xFFFF;
				}
				if (tempgui[1] != 0 && tempgui[1] != 255)
					G_S_PGui.CurrTTag = tempgui[1];

				if (0x80000000 != (tempgui[0] & 0x80000000)) {
					G_S_PGui.Flags |= FT_GUI_PENDOWN;
					pendown = 1;
					if (applyLastFrame == 0 ||
						(G_S_PGui.CurrTTag != G_S_PGui.PrevTTag &&
							(MEDIAPLAYER_KEY_EXIT == G_S_PGui.CurrTTag ||
								MEDIAPLAYER_KEY_BACK == G_S_PGui.CurrTTag))) {
						applyLastFrame = 1;
						G_S_PGui.Flags |= FT_GUI_REFRESH;
					}
					G_S_PGui.Disappear = FT_GUI_DISAPPEAR_COUNT; //counter based
				}
				else {
					G_S_PGui.Flags &= ~FT_GUI_PENDOWN;
					pendown = 0;
					G_S_PGui.CurrTTag = 0;
				}

				//pen up and previously a selection happened
				if (((0 == pendown) && (0 == G_S_PGui.CurrTTag) && (0 != G_S_PGui.PrevTTag)) || (needUpdProg > 0)) {
					if (MEDIAPLAYER_KEY_PAUSE_PLAY == G_S_PGui.PrevTTag) {
						G_S_PGui.TouchSel ^= 0x01;
						G_S_PGui.Flags |= (FT_GUI_REFRESH);
						if (0 == (G_S_PGui.TouchSel & 0x01)) {
							G_S_AviPlayer.StateCurr = FT_AVIPLAYER_PLAY;
#if defined(FT900_PLATFORM)
							FTPayer_VideoCnt = FTPayer_VideoRate;
							timer_start(FT900_FT_MILLIS_TIMER);
							if (!(G_S_AviParser.Flags & FT_AVIPARSER_DROPAUDIO)) {
								/*timer_start(timer_select_b);*/
								FTPayer_AudioCnt = FTPayer_AudioRate;
								guimutex = 1;
								FT_Audio_UnMute();
								guimutex = 0;
						}
#endif
#if defined(MSVC_PLATFORM) || defined(BT8XXEMU_PLATFORM) 
							G_S_AviPlayer.vidTimestamp = MSVCTimeMsGet();
#endif

#if defined(AUDIO_VIA_FT81X)
							if (!(G_S_AviParser.Flags & FT_AVIPARSER_DROPAUDIO) && G_S_AviParser.AudioStreamFound) {
								guimutex = 1;
								Ft_Gpu_Hal_Wr8(s_pHalContext, REG_VOL_PB, g_global_ctx->volume_level);
								Ft_Gpu_Hal_Wr8(s_pHalContext, REG_PLAYBACK_PLAY, 1);
								guimutex = 0;
							}
#endif
					}
						else {
							G_S_AviPlayer.StateCurr = FT_AVIPLAYER_PAUSE;
#if defined(FT900_PLATFORM)
							ft_timer_interrupt1_exit();
							//disable the timer to simulate pause condition
							audioPlaying = 1;
							FT_Audio_Mute();
							audioPlaying = 0;

#endif

#if defined(AUDIO_VIA_FT81X)
							if (!(G_S_AviParser.Flags & FT_AVIPARSER_DROPAUDIO) && G_S_AviParser.AudioStreamFound) {
								audioPlaying = 1;
								Ft_Gpu_Hal_Wr8(s_pHalContext, REG_PLAYBACK_PLAY, 0);
								/*reorderAudioData(&audioFifo);*/
								reorderMetaData();
								FT81x_Audio_Setup(audioFifoOffset, FT800AudioBufferSize, G_S_AviParser.SAud.BitsPerSample, G_S_AviParser.SAud.SamplingFreq, G_S_AviParser.SAud.AudioFormat);
								audioPlaying = 0;
							}
#endif
							{
								//consume the current partial frame, in case a seek was requested while pausing.
								//coprocessor expects a full frame data before it will do anything else.
								if (currentPartialFrame) {
									FT_MetaV_t tempvmeta;

									while (1) {
										FT_RQueue_PeekE(&srqvidin, &tempvmeta, srqvidin.ReadIdx);
										if (tempvmeta.PartialSz) {
											FT_AVIParser_FillChunk(&G_S_AviParser, &G_SVidDataPath, &G_SAudDataPath);
											printf("-");
											EVE_sleep(5);
										}
										else {
											guimutex = 1;
											FT_RQueue_ReadE(&srqvidin, &tempvmeta);
											currentPartialFrame = 0;
											guimutex = 0;
											break;
										}
									}
								}
							}
							printf("Pause\n");
						}
					}
					else if (MEDIAPLAYER_KEY_EXIT == G_S_PGui.PrevTTag || MEDIAPLAYER_KEY_BACK == G_S_PGui.PrevTTag) {
#if defined(FT900_PLATFORM)
						ft_timer_interrupt1_exit();
#endif
						if (videodecodemutex > 0) {
							ft_delay(60); //temp delay
						}
						retVal = G_S_PGui.PrevTTag;
						guimutex = 1;
						Gpu_Hal_WaitCmdfifo_empty(s_pHalContext);
						guimutex = 0;
						applyLastFrame = 1;
						G_S_PGui.Flags |= FT_GUI_REFRESH;
						G_S_PGui.Disappear = FT_GUI_DISAPPEAR_COUNT; //counter based
						printf("Exit video\n");
						break;
				}
					else if (MEDIAPLAYER_KEY_ROTATE == G_S_PGui.PrevTTag) {
						if (curOrientation == FT_VIDEO_LANDSCAPE) {
							curOrientation = FT_VIDEO_PORTRAIT;
							applyFrameRotationalScaling(&videoFrame, curOrientation);
							//Changing to portrait mode
						}
						else {
							curOrientation = FT_VIDEO_LANDSCAPE;
							applyFrameRotationalScaling(&videoFrame, curOrientation);
							//Changing to landscape mode
						}

					}
					else if (MEDIAPLAYER_KEY_VOL_CONTROL == G_S_PGui.PrevTTag) {
						//Process volume change
						if (controlButtons.volYPos >= touchYPos &&
							(controlButtons.volYPos - controlButtons.volWidth) <= touchYPos)
						{
							g_global_ctx->volume_level = (controlButtons.volYPos - touchYPos) * FT81x_MAX_VOLUME_LEVEL / controlButtons.volWidth;
							//Setting new level
							Ft_Gpu_Hal_Wr8(s_pHalContext, REG_VOL_PB, g_global_ctx->volume_level);
							applyLastFrame = 1;
							G_S_PGui.Flags |= FT_GUI_REFRESH;
							G_S_PGui.Disappear = FT_GUI_DISAPPEAR_COUNT; //counter based
						}
					}
					else if (10 == G_S_PGui.PrevTTag) {
						videoFrame.zoomOut ^= 1;
						applyFrameRotationalScaling(&videoFrame, curOrientation);
						G_S_PGui.Flags |= (FT_GUI_REFRESH);
					}
					else if ((MEDIAPLAYER_KEY_PROGBAR0 == G_S_PGui.PrevTTag) || (needUpdProg > 0)
						) { //seek position
						if ((G_S_AviParser.VideoStreamFound && !(G_S_AviParser.Flags & FT_AVIPARSER_DROPVIDEO) && G_S_AviParser.SVid.ValidSuperIndexEntries) ||
							(G_S_AviParser.AudioStreamFound && !(G_S_AviParser.Flags & FT_AVIPARSER_DROPAUDIO) && G_S_AviParser.SAud.ValidSuperIndexEntries) ||
							(G_S_AviParser.Riff[0].indxOff != 0)) {
							if (((touchXPos >= controlButtons.statusXPos + 10) && (touchXPos <= (controlButtons.statusXPos + controlButtons.statusWidth)))
								|| (needUpdProg > 0)) {
									{
#if 1
										ft_uint8_t percent;
										//ft_uint8_t seeked = 0;

#ifdef SUPPORT_MEDIAPLAYER_RETAIN_STATUS
										if (needUpdProg > 0) {
											needUpdProg = -1;
											percent = MultimediaExplorer_GetFileProgress(fname);
									}
										else
#endif //SUPPORT_MEDIAPLAYER_RETAIN_STATUS
										{
											percent = (touchXPos - (controlButtons.statusXPos + 10)) * 100 / (/*controlButtons.statusXPos + */controlButtons.statusWidth);
										}
										parsingVideo = 1;
										parsingAudio = 1;
										//printf("Check for seek.\n");
										//ft_timer_interrupt1_exit();
#if defined(FT900_PLATFORM)
										FT_Audio_Mute();
										interrupt_disable_globally();
										ft_timer_interrupt1_exit();
										//clear interrupt flag, if any
										timer_is_interrupted(FT900_FT_MILLIS_TIMER);
										/*timer_is_interrupted(timer_select_b);*/
										EVE_sleep(10);
#endif
										{
											if (currentPartialFrame) {
												FT_MetaV_t tempvmeta;
												while (1) {
													FT_RQueue_PeekE(&srqvidin, &tempvmeta, srqvidin.ReadIdx);
													if (tempvmeta.PartialSz) {
														FT_AVIParser_FillChunk(&G_S_AviParser, &G_SVidDataPath, &G_SAudDataPath);
														printf("-");
														EVE_sleep(5);
													}
													else {
														FT_RQueue_ReadE(&srqvidin, &tempvmeta);
														currentPartialFrame = 0;
														break;
													}
												}
											}
										}

										//printf("Performing seeking.\n");


										while (Ft_Gpu_Hal_Rd16(s_pHalContext, REG_CMD_READ) != Ft_Gpu_Hal_Rd16(s_pHalContext, REG_CMD_WRITE)) {
											//printf("Waiting for device before seeking.\n");
										}

										AviPlayer_BufferInit(1);

										if (aviPlayerSeekTo(percent)) {
											break;
										}
										AviPlayer_FifoInit(1, mediafifo, mediafifolen, &appliedFirstFrame, &applyLastFrame, &optionalInitialFrameDecode);

										/*
										 [Decode and display mjpeg frame]

										 After seeking, attempt to decode an initial frame before continuing video playback.
										 */
										AviPlayer_RenderFirstFrames(&appliedFirstFrame, &applyLastFrame, &optionalInitialFrameDecode);
										applyLastFrame = 0;
										/* [Decode and display mjpeg frame] */
										//
										AviPlayer_AudioInit();
										parsingVideo = 0;
										parsingAudio = 0;
#endif //if 0: for seeking
							}// End of seeking
						}
					}
			}
					applyLastFrame = 1;
					G_S_PGui.Flags |= FT_GUI_REFRESH;
					G_S_PGui.Disappear = FT_GUI_DISAPPEAR_COUNT; //counter based
				}

				if (G_S_PGui.Flags & FT_GUI_PENDOWN) {
					G_S_PGui.Disappear = FT_GUI_DISAPPEAR_COUNT;
				}
				else {
					G_S_PGui.Disappear = max(0, (G_S_PGui.Disappear - 1));
				}

				if (G_SVidDataPath.NumDecoded > 0 && appliedFirstFrame == 0) { //after seeking and if there isn't any frame data, attempt to perform gui construction after data is present.
					appliedFirstFrame = applyLastFrame = optionalInitialFrameDecode = 1;

				}

				if (optionalInitialFrameDecode && (G_S_AviPlayer.CurrFrame > 1)) {
					G_S_PGui.Flags |= (FT_GUI_REFRESH);
				}
#endif //if 0 for GUI processing

				/*
				 [Decode and display mjpeg frame]

				 Construct display lists for the GUI and video frame.
				 */
				{
					/* GUI screen shot construction */
					if ((G_S_PGui.Flags & FT_GUI_REFRESH)) {
						guimutex = 1;
						if (G_S_PGui.Disappear == 0) {
							Gpu_Hal_WaitCmdfifo_empty(s_pHalContext);
						}

						if (optionalInitialFrameDecode && (G_S_AviPlayer.CurrFrame > 1)) {
							Gpu_Hal_WaitCmdfifo_empty(s_pHalContext);
							G_S_PGui.Disappear = 0;
						}
						if (Ft_Gpu_Hal_Rd16(s_pHalContext, REG_CMD_READ) == Ft_Gpu_Hal_Rd16(s_pHalContext, REG_CMD_WRITE)) {
							//ft_int32_t centerx, centery, totalints;
							//ft_int32_t profileval; //TODO: for profiling

							/* construct the DL and display */
							Gpu_CoCmd_Dlstart(s_pHalContext);
							EVE_Cmd_wr32(s_pHalContext, CLEAR_TAG(0));
							EVE_Cmd_wr32(s_pHalContext, CLEAR_COLOR_RGB(0, 0, 0));
							EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1));

							if (!(G_S_AviParser.Flags & FT_AVIPARSER_DROPVIDEO)) {
								//decoded jpeg bitmap
								EVE_Cmd_wr32(s_pHalContext, TAG_MASK(0));
								EVE_Cmd_wr32(s_pHalContext, TAG(0));
								EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 255, 255));
								EVE_Cmd_wr32(s_pHalContext, BITMAP_HANDLE(0));
								EVE_Cmd_wr32(s_pHalContext, BITMAP_SOURCE(0));
								EVE_Cmd_wr32(s_pHalContext, BITMAP_LAYOUT(RGB565, aviw * 2L, avih));
								EVE_Cmd_wr32(s_pHalContext, BITMAP_LAYOUT_H(((aviw * 2L) >> 10), ((avih) >> 9)));
								EVE_Cmd_wr32(s_pHalContext, BITMAP_SIZE(NEAREST, BORDER, BORDER, aviw, avih));
								EVE_Cmd_wr32(s_pHalContext, BITMAP_SIZE_H((aviw >> 9), (avih >> 9)));

								//hack for not to display video frame
								if (G_SVidDataPath.NumDecoded > 0) {
									EVE_Cmd_wr32(s_pHalContext, BEGIN(BITMAPS));
									if (videoFrame.zoomOut) {
										EVE_Cmd_wr32(s_pHalContext, VERTEX2F(8 * (FT_DispWidth - videoFrame.frameWidth), 8 * (FT_DispHeight - videoFrame.frameHeight)));
									}
									else {
										applyFrameRotationalScaling(&videoFrame, curOrientation);
										EVE_Cmd_wr32(s_pHalContext, VERTEX2F(8 * (FT_DispWidth - videoFrame.finalFrameWidth), 8 * (FT_DispHeight - videoFrame.finalFrameHeight)));
									}
									optionalInitialFrameDecode = 0;
								}
							}
							else {
								optionalInitialFrameDecode = 0;
							}
							if (G_S_PGui.Disappear > 0) {
								Ft_Gpu_CoCmd_LoadIdentity(s_pHalContext);
								Ft_Gpu_CoCmd_SetMatrix(s_pHalContext);
								drawControlButtons(&controlButtons);
								if (videoFrame.zoomOut) {
									applyFrameRotationalScaling(&videoFrame, curOrientation);
								}
							}
							EVE_Cmd_wr32(s_pHalContext, DISPLAY());
							Ft_Gpu_CoCmd_Swap(s_pHalContext);
							EVE_Cmd_waitFlush(s_pHalContext);
							//Gpu_Hal_WaitCmdfifo_empty(s_pHalContext);
							G_S_PGui.Flags &= (~FT_GUI_REFRESH); //default the refresh
						}
						guimutex = 0;
					}
				}
				/* [End of decode and display mjpeg frame] */

				parret = 0;

#if defined(AUDIO_VIA_FT900)
				audioPlaying = 1;

				if ((FT_Audio_Getfreespace() == I2S_TXRX_FIFO_SPACE))
				{
					if (!(audioIRQstatus & MASK_I2S_PEND_FIFO_TX_HALF_FULL) && !(audioIRQstatus & MASK_I2S_PEND_FIFO_TX_EMPTY))
					{
						playAudioData(&G_S_AviPlayer, &G_S_AviParser, &G_SAudDataPath, &G_SVidDataPath);
					}
				}
				audioPlaying = 0;
#endif

				/*
								guimutex = 1;
								printf("tempgui: (%d %d %d) (%d %d %d)\n\r",
										Ft_Gpu_Hal_Rd16(s_pHalContext, REG_CMD_READ), Ft_Gpu_Hal_Rd16(s_pHalContext, REG_CMD_WRITE),
										FTPayer_AudioCnt, FTPayer_AudioRate, updateAudioIndex,
										FTPayer_VideoCnt, FTPayer_VideoRate, videodecodemutex);
								guimutex = 0;
				*/

				/*
				 [Decode and display mjpeg frame]

				 One of the two sections where video frames decoding is initialized and frame(s) are dropped.
				 */
				 //if(1 == parret)
				if (videodecodemutex > 0) {
					guimutex = 1;
					if (Ft_Gpu_Hal_Rd16(s_pHalContext, REG_CMD_READ) == Ft_Gpu_Hal_Rd16(s_pHalContext, REG_CMD_WRITE)) {
						aviPlayerRenderFrame();
						videodecodemutex = 0;
						//videodecodemutex--;
					}
					else {
#if defined(SHOWVIDEOPRINTS)
						printf("LDevice is busy.\n");
#endif
					}

					//videodecodemutex = 0;
					guimutex = 0;
				}

				/* [End of decode and display mjpeg frame] */

				//Check if EVE hang
				Ft_CoProErrorRecovery_Check(s_pHalContext);
				if (G_S_AviPlayer.StateCurr != FT_AVIPLAYER_PAUSE) {
					parret = FT_AVIParser_FillChunk(&G_S_AviParser, &G_SVidDataPath, &G_SAudDataPath);
				}

#if defined(AUDIO_VIA_FT81X)

				//update audio
				if (updateAudioIndex > 0) {
					audioPlaying = 1;
					aviPlayerRenderAudio();
					audioPlaying = 0;
					updateAudioIndex = 0;
					//updateAudioIndex--;
				}
#endif

				if (G_S_PGui.Disappear <= 1 && applyLastFrame == 1) {
					G_S_PGui.Disappear = 0;
					applyLastFrame = 0;
					G_S_PGui.Flags |= (FT_GUI_REFRESH);
				}

				if (FT_AVIPARSER_EOF == parret) {
					parsingVideo = 1;
					Gpu_Hal_WaitCmdfifo_empty(s_pHalContext);
					parsingVideo = 0;
					printf("EOF break; %x\n", parret);
					break;
				}
				else if (parret != FT_AVIPARSER_OK) {

				}

				G_S_PGui.PrevTTag = G_S_PGui.CurrTTag;
				G_S_PGui.TouchSel &= (~0x06);
			}

			audioPlaying = 1;
			FT_Audio_Mute();
			audioPlaying = 0;

#if defined(FT900_PLATFORM)

#if defined(AUDIO_VIA_FT900)
			audioPlaying = 1;
			i2s_disable_int(0xFFFF);
			audioPlaying = 0;
			EVE_sleep(10); //wait till all audio data in the buffer has been consumed.
			FT_Audio_Stop_Transmission();
#endif

			guimutex = 1;
#if defined(AUDIO_VIA_FT81X)
			Ft_Gpu_Hal_Wr8(s_pHalContext, REG_PLAYBACK_PLAY, 0);
			Ft_Gpu_Hal_Wr16(s_pHalContext, REG_VOL_PB, 0);
#endif
			timer_disable_interrupt(FT900_FT_MILLIS_TIMER);
			/*timer_disable_interrupt(timer_select_b);*/
			Gpu_Hal_WaitCmdfifo_empty(s_pHalContext);
			guimutex = 0;

#endif
			/* exit avi parser */
			printf("Finished playing video.\n");
			FT_AVIParser_Exit(&G_S_AviParser);
			if ((2 == G_S_PGui.PrevTTag)) {
				break;
			}
			restoreFrameRotationalScaling(&videoFrame);
			//break; //exit to main menu
		}
		//		restoreFrameRotationalScaling(&videoFrame);
	}
	//	while (Loop != 0);
#ifdef SUPPORT_MEDIAPLAYER_RETAIN_STATUS
	if (needUpdProg) {
		int8_t percent = G_S_AviParser.SVid.CurrFrame / (G_S_AviParser.SVid.TotFrames / 100);
		MultimediaExplorer_SetFileProgress(fname, percent);
	}
#endif
	restoreFrameRotationalScaling(&videoFrame);
#ifdef FT900_PLATFORM
	ft_timer_interrupt1_exit();
	timer_disable_interrupt(FT900_FT_MILLIS_TIMER);
	Ft_Gpu_Hal_Wr8(s_pHalContext, REG_VOL_PB, g_global_ctx->volume_level);
	/*timer_disable_interrupt(timer_select_b);*/
#endif //FT900_PLATFORM

	/* Close video data path */
//	printf("Closed video file.\n");
	/* To ensiure graceful exit is done and also gui manager expects cmd_dlstart */
	FT_Util_ExitApp(s_pHalContext);
	free(g_global_ctx);
	g_global_ctx = NULL;
	/*gui_restore();*/
	//TODO: restore context
	return retVal;
}

/* Nothing beyond this */

