#ifndef SRC_UTIL_FT_UTIL_H_
#define SRC_UTIL_FT_UTIL_H_

#include "FT_Platform.h"

#define FT_CMD_SIZE          (4)       //4 byte per coprocessor command of EVE
#define FT900_ICON_FROM_RAM						1

/*
 * List of IconID
 */
#ifdef FT900_ICON_FROM_RAM
enum FT_ICON_e {
	FTICON_ID_EXIT = 0,
	FTICON_ID_BACK,
	FTICON_ID_SDCARD,
	FTICON_ID_EFLASH,
	FTICON_ID_PICTURE,
	FTICON_ID_AUDIO,
	FTICON_ID_SPEAKER,
	FTICON_ID_PLAY,
	FTICON_ID_PAUSE,
	FTICON_ID_VIDEO,
	FTICON_ID_ROTATE,
	FTICON_ID_VOL_MUTE
};
#else //ICON from file
#define FTICON_ID_SDCARD  (701)
#define FTICON_ID_EFLASH  (516)
#define FTICON_ID_PICTURE (616) //ICON_PHOTO
#define FTICON_ID_AUDIO   (539)
#define FTICON_ID_BACK    (431)
#define FTICON_ID_SPEAKER (886)
#define FTICON_ID_ROTATE  (685)
#define FTICON_ID_PLAY    (630)
#define FTICON_ID_PAUSE   (581)
#define FTICON_ID_VIDEO   (481) //ICON_LOCAL_MOVIES
#define FTICON_ID_EXIT    (173) //ICON_CLOSE
#define FTICON_ID_VOL_MUTE (884) //ICON_VOLUME_MUTE
#endif //FT900_ICON_FROM_RAM


/***
 * End of Icon Define
 */

#define MAINUI_ELEMENT_HEIGHT 50
#define MEDIAICON_WIDTH       48
#define MEDIAICON_HEIGHT      48

/*
 * Some useful color
 */
#define DARK_GREY_COLOR()   COLOR_RGB(10,10,10)
#define LIGHT_GREY_COLOR()  COLOR_RGB(94,91,92)
#define BLACK_COLOR()  COLOR_RGB(0,0,0)
#define WHITE_COLOR()  COLOR_RGB(255,255,255)
#define NO_COLOR()     (0xFF<<24)

#define FT81x_MAX_ZOOM_LEVEL   128UL
#define FT81x_MAX_VOLUME_LEVEL 255UL
#define FT81x_DEFAULT_VOLUME   32UL

// Modes for AudioPlay
#define PLAY_WAIT             0
#define PLAY_ONCE             1
#define PLAY_LOOP             2

// Error return codes

#define ERROR_GENERAL             -1
#define ERROR_INTERNAL            -2
#define ERROR_NOFILE              -3
#define ERROR_NOPATH              -4
#define ERROR_INVALIDFILE         -5
#define ERROR_INVALIDOBJECT       -6
#define ERROR_NOFILESYSTEM        -7
#define ERROR_FILESYSTEMFULL      -8
#define ERROR_NOTENOUGHCORE       -9
#define ERROR_TOOMANYOPENFILES    -10
#define ERROR_INVALIDPARAMETERS   -11
#define ERROR_NOSDCARD            -12
#define ERROR_INVALIDFILENAME     -13
#define ERROR_ASSERTION           -14
#define ERROR_EXIST               -15

/*
 * Date and time macros
 */
#define SECOND2HOUR(t)   ((t)/3600)
#define SECOND2MIN(t)    (((t)%3600)/60)
#define SECOND2REMAIN(t) ((t)%60)

enum AviPlayerTag_e {
	MEDIAPLAYER_KEY_START_IDX = 200,
	//Common KEY
	MEDIAPLAYER_KEY_BACK,
	MEDIAPLAYER_KEY_EXIT,

	//Specific keys for Audio/Pic
	MEDIAPLAYER_KEY_AUD_PLAY,
	MEDIAPLAYER_KEY_AUD_PROGRESS0,
	MEDIAPLAYER_KEY_AUD_PROGRESS1,
	MEDIAPLAYER_KEY_PIC_EXIT,
	MEDIAPLAYER_KEY_SDCARD,
	MEDIAPLAYER_KEY_EFLASH,
	MEDIAPLAYER_KEY_VOL_ZOOM_CONTROL,
	MEDIAPLAYER_KEY_PIC_SEL,

	//Specific keys for Video
	MEDIAPLAYER_KEY_VOL_CONTROL,
	MEDIAPLAYER_KEY_PAUSE_PLAY,
	MEDIAPLAYER_KEY_ROTATE,
	MEDIAPLAYER_KEY_ZOOM,
	MEDIAPLAYER_KEY_PROGBAR0,
	MEDIAPLAYER_KEY_PROGBAR1,
};

struct bm_info_t {
	ft_uint32_t gram_size;
	ft_uint32_t gram_src;
	ft_uint32_t fmt, w, h;
	ft_uint16_t iconId;
	ft_uint8_t bmId;
};

struct gram_t {
	ft_int32_t	gram_free;
	ft_int32_t	gram_start;
};

struct Hobject {
	ft_uint32_t addr;
	ft_uint32_t size;
	ft_uint16_t flags;
};

#define OBJECT_LOADED       1
#define OBJECT_VISIBLE      2
#define OBJECT_SPILLABLE    4
#define OBJECT_TOFREE       8

struct Hbitmap {
  struct Hobject ai;
  ft_uint16_t width, height;
  ft_uint8_t fmt, palsize;
  char filename[1];
};

// Modes for AudioPlay
#define PLAY_WAIT             0
#define PLAY_ONCE             1
#define PLAY_LOOP             2

typedef struct  {
	void (*service_playback)();
	void (*cmd_AudioStop)();
	void (*end_audio)();
	void (*init_audio)();
	int (*audio_played_samples)();
	int (*cmd_AudioPlay)(const char* filename, ft_int16_t mode);
	int (*audio_seekto)(int seek_samples);
	ft_int16_t (*raw_draft_image)(const char* filename, struct Hbitmap *gm);
	ft_int16_t(*raw_draft_image_byhandle)(int handle, struct Hbitmap *gm);
#if defined(FT900_PLATFORM)
#define FILE_READ FA_READ
#define FILE_WRITE FA_WRITE
#else
#define FILE_READ 0x01
#define FILE_WRITE 0x02
#endif
	//File operations
	ft_int16_t (*cmd_FOpen)(const char* String, int mode);
	ft_int16_t  (*cmd_FSeek)(ft_int16_t Handle, ft_int32_t Offset);
	ft_int16_t (*cmd_FRead)(ft_int16_t Handle, ft_uint8_t *Buffer, ft_int16_t bytetoread, ft_int16_t *bytesread);
	ft_int16_t (*cmd_FClose)(ft_int16_t Handle);
	int (*cmd_FSize)(ft_int16_t Handle, ft_uint32_t *Offset);
	int (*cmd_FTell)(ft_int16_t Handle, ft_uint32_t *Offset);
	ft_uint16_t (*be16)(ft_int16_t f);
	void     (*feedfile)(ft_int16_t f);
} GuiManager;

//Render the font in an area
enum Alignment_e {
	ALIGN_LEFT = 0,
	ALIGN_CENTER,
	ALIGN_RIGHT,
	ALIGN_VIDEO,
	ALIGN_MAX,
};

enum MediaFont_e {
	MEDIA_FONT_TINY = 0,
	MEDIA_FONT_MEDIUM,
	MEDIA_FONT_HUGE,
	MEDIA_FONT_MAX,
};

/*
 * Safe display a string in a region
 */
struct RenderArea_t {
	int x;
	int y;
	int w;
	int h;
	ft_int32_t fgColor;
};

#ifdef __cplusplus
extern "C" {
#endif
void helperRenderRect(Ft_Gpu_Hal_Context_t *pHalContext, int x1Frac16, int y1Frac16, int x2Frac16, int y2Frac16, int color, ft_uint8_t tag);
void helperRenderAlignedString(Ft_Gpu_Hal_Context_t * pHalContext, const char* str, enum MediaFont_e font, struct RenderArea_t *pArea, enum Alignment_e align);
int helperLoadIcon(Ft_Gpu_Hal_Context_t *pHalContext, struct gram_t *pGram, const char* filename, struct bm_info_t *pBmInfo);
void helperRenderBitmapIcon(Ft_Gpu_Hal_Context_t *pHalContext, struct bm_info_t *pBmInfo, int x, int y, int w, int h, ft_uint8_t tag, int isSel);
void helperRenderVolumeBar(Ft_Gpu_Hal_Context_t *pHalContext, int xdist_frac8, int ydist_frac8, int ydist2_frac8, ft_uint8_t tag, int vol_level, int range);
void helperRenderProgressBar(Ft_Gpu_Hal_Context_t *pHalContext, int xdist, int ydist, int xdist2, int curTimeSecs, int endTimeSecs, ft_uint8_t tag, float ratio);
void helperRenderHeaderPanel(Ft_Gpu_Hal_Context_t *pHalContext, int xdist, int ydist, struct bm_info_t *pBackIcon, struct bm_info_t *pExitIcon,
	ft_uint8_t curTag, ft_uint8_t prevTag);
#ifdef __cplusplus
}
#endif

/* Hardware or Module specific macros for gpio line numbers  */
#if (defined(MM900EV1A) || defined(MM900EV2A) || defined(MM900EV3A) || defined(MM900EV_LITE))
#define FT800_SEL_PIN   0
#define FT800_PD_N      43

/* Timer 1 is been utilized in case of FT900 platform */
#define	FT900_FT_MILLIS_TIMER					(timer_select_b)
#define FT900_TIMER_MAX_VALUE 					(65536L)
#define FT900_TIMER_PRESCALE_VALUE 				(100)
#define FT900_TIMER_OVERFLOW_VALUE 				(1000)

#endif

#ifdef PANL35
#define FT800_SEL_PIN   						0
#undef FT800_PD_N
#define FT800_PD_N      						1
#define FT800_INT	      						0

/* Timer 1 is been utilized in case of FT900 platform */
#define	FT900_FT_MILLIS_TIMER					(timer_select_b)
#define FT900_TIMER_MAX_VALUE 					(65536L)
#define FT900_TIMER_PRESCALE_VALUE 				(100)
#define FT900_TIMER_OVERFLOW_VALUE 				(1000)
#endif

#define FT900_ICON_FROM_RAM						1
#define FT900_DISABLE_EFLASH_STORAGE			1

#define BIT(b) (0x1UL << (b))

#define clear_bit(val, bit) do{\
	(val) &= ~(1 << (bit));\
}while(0)
#define set_bit(val, bit) do {\
	(val) |= 1 << (bit);\
}while(0)

#define test_bit(val, bit) ((val) & (1 << (bit)))


#endif /* SRC_UTIL_FT_UTIL_H_ */
