/* Video player application specific file */


#ifndef _FT_APP_PLAYVIDEO_H_
#define _FT_APP_PLAYVIDEO_H_




/* Software systems for ring buffer, queue */


/* Ring buffer implementation */

/* AVI parser design */
/*
 * heres the general layout of an AVI riff file (new format)
 *
 * RIFF (3F??????) AVI       <- not more than 1 GB in size
 *     LIST (size) hdrl
 *         avih (0038)
 *         LIST (size) strl
 *             strh (0038)
 *             strf (????)
 *             indx (3ff8)   <- size may vary, should be sector sized
 *         LIST (size) strl
 *             strh (0038)
 *             strf (????)
 *             indx (3ff8)   <- size may vary, should be sector sized
 *         LIST (size) odml
 *             dmlh (????)
 *         JUNK (size)       <- fill to align to sector - 12
 *     LIST (7f??????) movi  <- aligned on sector - 12
 *         00dc (size)       <- sector aligned
 *         01wb (size)       <- sector aligned
 *         ix00 (size)       <- sector aligned
 *     idx1 (00??????)       <- sector aligned
 * RIFF (7F??????) AVIX
 *     JUNK (size)           <- fill to align to sector -12
 *     LIST (size) movi
 *         00dc (size)       <- sector aligned
 * RIFF (7F??????) AVIX      <- not more than 2GB in size
 *     JUNK (size)           <- fill to align to sector - 12
 *     LIST (size) movi
 *         00dc (size)       <- sector aligned
 *
 *-===================================================================*/

/* return types of player */
typedef enum FT_AVIPLAYER_STATUS
{
	FT_AVIPLAYER_OK = 0,
}FT_AVIPLAYER_STATUS;

typedef enum FT_AVIPLAYER_FLAGS
{
	FT_AVIPLAYER_FLAGNONE = 0x00000000,
	FT_AVIPLAYER_MAXFPS = 0x00000001,
	FT_AVIPLAYER_MAXORG = 0x00000002,
}FT_AVIPLAYER_FLAGS;

/* AVI player state machine */
typedef enum FT_AVIPLAYER_STATE
{
	FT_AVIPLAYER_NONE = 0x00000001,
	FT_AVIPLAYER_INIT = 0x00000002,
	FT_AVIPLAYER_CONFIG = 0x00000004,
	FT_AVIPLAYER_SELECT = 0x00000008,
	FT_AVIPLAYER_PLAY = 0x00000010,
	FT_AVIPLAYER_PAUSE = 0x00000020,
	FT_AVIPLAYER_STOP = 0x00000040,
	FT_AVIPLAYER_EXIT = 0x00000080,
	FT_AVIPLAYER_SEEK = 0x00000100,
}FT_AVIPLAYER_STATE;

/* Context of AVI player */
typedef struct FT_AVIPlayer
{
	ft_uint32_t StateCurr;//none, init, configure, play, pause, stop, seek, exit
	ft_uint32_t StateNext;
	ft_uint32_t	AudioCodec;
	ft_uint32_t	VideoCodec;

	/* playback of a avi file context */
	ft_uint32_t	CurrTime;//time interms of ms and time wrt file playback - for pause this time is not changed
	ft_uint32_t CurrFrame;
	/*
	 Possiblely an indication that the audio buffer is too small or audio bitrate is too high for the video if the audio frames
	 are constantly being dropped.  High pitch audio might occur when a frame was dropped which will be silent for the
	 duration of the audio frame before the next audio frame is played.
	 */
	ft_uint32_t AudioDroppedFrames;
	ft_uint32_t AudioLowFifoDroppedFrameCount;
	/*
	constant video frame droppings could be:  1]the storage device access speed is slow and it's unable to keep up with the frame rate.
	 2] The video size is too big where the limited buffer is unable to buffer an effective amount of video data to ensure smooth
	 playback.  Video frames size varies through out the duration of the playback so occasionally frame drops are inevitable.  
	 If frames are being dropped constantly and the smoothness of the playback suffers then the only fix is to resize the video 
	 resolution and/or lower the video bitrate.
	 video .
	 */
	ft_uint32_t VideoDroppedFrames;
	ft_uint32_t OldVideoDroppedFrames;
	ft_uint32_t VideoLackFrames;
	ft_uint8_t noVideoData;
	ft_uint32_t TotalBytes;
	ft_uint32_t DisplayFrameMSinterval; //the micro seconds between each video frame

	ft_uint32_t mediafifo;
	ft_uint32_t mediafifolen;
#if defined(MSVC_PLATFORM) || defined(BT8XXEMU_PLATFORM) 
	volatile long long audTimestamp;
	volatile long long vidTimestamp;
#endif
}FT_AVIPlayer_t;

typedef enum FT_GUI_FLAGS
{
	FT_GUI_NONE = 0x00,
	FT_GUI_REFRESH = 0x01,
	FT_GUI_LINESWAP = 0x02,
	FT_GUI_PENDOWN = 0x04,

}FT_GUI_FLAGS;
typedef struct FT_PGUI
{
	ft_uint16_t CurrX;
	ft_uint16_t CurrY;
	ft_uint8_t PrevTTag;
	ft_uint8_t CurrTTag;
	ft_uint8_t TouchSel;
	ft_uint8_t VRateK;
	ft_uint8_t Flags;		//flags such as update gui,dlswap frame or line,
	volatile ft_int32_t Disappear;	//offset of the touch point
}FT_PGUI_t;

#define OLD_AUDIO_FRAME_THREASHOLD 2
#define NEWER_AUDIO_FRAME_THREASHOLD 5
#define VIDEO_WAIT_AUDIO_FRAME_THREASHOLD 60
#define LOW_MEDIAFIFO_THREASHOLD (100000)
#define TOUCH_LEEWAY (1)

extern volatile ft_uint32_t parsingAudio;
extern volatile ft_uint32_t parsingVideo;

typedef enum FT_VIDEO_ORIENTATION
{
	FT_VIDEO_LANDSCAPE=0,
	FT_VIDEO_INVERTED_LANDSCAPE,
	FT_VIDEO_PORTRAIT,
	FT_VIDEO_INVERTED_PORTRAIT,
	FT_VIDEO_MIRRORED_LANDSCAPE,
	FT_VIDEO_MIRRORED_INVERTED_LANDSCAPE,
	FT_VIDEO_MIRRORED_PORTRAIT,
	FT_VIDEO_MIRROED_INVERTED_PORTRAIT,
}FT_VIDEO_ORIENTATION;

typedef struct FT_GUI_BUTTONS_POS
{
	ft_int16_t volXPos;
	ft_int16_t volYPos;
	ft_int16_t volWidth;
	ft_int16_t statusXPos;
	ft_int16_t statusYPos;
	ft_int16_t statusWidth;
}FT_GUI_BUTTONS_POS;

typedef struct FT_VIDEO_FRAME
{
	ft_uint16_t frameWidth;
	ft_uint16_t frameHeight;
	ft_uint16_t finalFrameWidth;
	ft_uint16_t finalFrameHeight;
	ft_uint32_t scaleFactor;
	ft_uint8_t zoomOut;
	FT_VIDEO_ORIENTATION rotation;
	FT_GUI_BUTTONS_POS *pControlButtons;
}FT_VIDEO_FRAME;

#define FT_VIDEO_TOUCH_THREASHOLD 100000



#if 0
/* GUI player context */
typedef struct FT_AVIP_Gui
{

}FT_AVIP_Gui_t;
#endif







#endif



