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

Multimedia Explorer demo application

Revision History:

*/

#include <ctype.h>

#include "Common.h"
#include "App.h"

#include "DemoMediaPlayer.h"

#include "FT_Buffer.h"
#include "FT_MQueue.h"
#include "FT_App_PlayVideo.h"
#include "FT_Audio.h"

#if defined(FT900_PLATFORM)
#include "container_avi.h"
#else
#include "FT_AVIParser.h"
#endif

#include "FT_Util.h"
#include "bmp.h"

#if defined(MSVC_PLATFORM) || defined(BT8XXEMU_PLATFORM) 
//For file operations
#include <io.h>  
#include <fcntl.h>  
#include <sys/types.h>  
#include <sys/stat.h>  
#include <share.h>  

#include <tchar.h> 
#include <stdio.h>
#include <strsafe.h>
#pragma comment(lib, "User32.lib")

#define _CRT_NONSTDC_NO_WARNINGS 
#define FR_OK 0
#endif // defined(MSVC_PLATFORM) || defined(BT8XXEMU_PLATFORM) 

#define MSVC_MEDIA_ROOT_DIR		TEST_DIR "\\Sdcard"
#define MSVC_MEDIA_DB_FILENAME  MSVC_MEDIA_ROOT_DIR "\\media.db"

#ifdef __cplusplus
#define EXTERN_C extern "C"
#define CMD_FREAD(obj, x, y, z, t) obj->cmd_FRead(x,y,z,t)
#define CMD_FSIZE(obj, x, y) obj->cmd_FSize(x,y)
#define CMD_FTELL(obj, x, y) obj->cmd_FTell(x, y)
#else
#define EXTERN_C extern
#define CMD_FREAD(obj, x, y, z, t) obj->cmd_FRead(x,y,z,&t)
#define CMD_FSIZE(obj, x, y) obj->cmd_FSize(x,&y)
#define CMD_FTELL(obj, x, y) obj->cmd_FTell(x, &y)
#endif

#if defined(FT900_PLATFORM)
#define F_READ(f,b,s,ret) f_read((f), b, s, &ret)
#define F_WRITE(f,b,s,ret) f_write((f), b, s, &ret)
#define F_CLOSE(f) f_close(f)
#define F_SEEK(f,pos) f_lseek((f), pos)
#define F_TELL(f) f_tell((f))
#else
#define F_READ(f,b,s,ret) (ret = fread(b, 1, s, (f)))
#define F_WRITE(f,b,s,ret) (ret = fwrite(b, 1, s, (f)))
#define F_CLOSE(f) fclose(f)
#define F_SEEK(f,pos) fseek(f, pos, SEEK_SET)
#define F_TELL(f) ftell(f)
#endif

#define FILE_SECTOR_SIZE 512
#define MEDIA_COMMAND_BUFFER_SIZE 256

#if defined(FT900_PLATFORM)
#include "ff.h"
#endif
#define FRAC(n)   ((n) >> 3)
#define FIX(n)    ((n) << 3)

#ifndef min
#define min(a,b)  ((a) < (b) ? (a) : (b))
#endif

#ifndef max
#define max(a,b)  ((a) > (b) ? (a) : (b))
#endif

#define FDATE_DAY(fdate) ((fdate)&(0x1F))
#define FDATE_MONTH(fdate) (((fdate)>>5)&(0x0F))
#define FDATE_YEAR(fdate) ((((fdate)>>9)&(0x7F)) + 1980)

#define FTIME_HOUR(ftime) (((ftime)>>11)&(0x1F))
#define FTIME_MIN(ftime) (((ftime)>>5)&(0x3F))

#define MEDIA_DISPLAY_INFO_RESOLUTION  0x0001
#define MEDIA_DISPLAY_INFO_DURATION    0x0002
#define MEDIA_DISPLAY_INFO_FREQUENCY   0x0004
#define MEDIA_DISPLAY_INFO_CODEC       0x0008
#define MEDIA_DISPLAY_INFO_FPS         0x0010

#define MEDIA_UICOLOR_NORM	WHITE
#define MEDIA_UICOLOR_SEL	YELLOW


//#define ENABLE_UT_FILEPROGRESS 1

//#define ENABLE_PROFILING 1
#ifdef ENABLE_PROFILING
#define BEGIN_PROFILE(name) {\
	ft_uint32_t name##_s, name##_e;\
	name##_s = ft_millis();\

#define END_PROFILE(name) \
	name##_e = ft_millis();\
	printf("<Profiling> %s: %d ms\n\r", #name, name##_e - name##_s);\
    }
#else
#define BEGIN_PROFILE(name) 
#define END_PROFILE(name) 
#endif // ENABLE_PROFILING

#ifndef FT9XX_PLATFORM
void gui_preserve(void) {};
void gui_restore(void) {};
uint32_t gui_moduleid() {};
#endif // FT9XX_PLATFORM

//EXTERN_C ft_void_t EVE_Cmd_wr32(Ft_Gpu_Hal_Context_t *pHalContext,ft_uint32_t cmd);
//EXTERN_C ft_void_t EVE_Cmd_waitFlush(Ft_Gpu_Hal_Context_t *pHalContext);
EXTERN_C void FT_Util_StartApp(Ft_Gpu_Hal_Context_t *pHalContext);
EXTERN_C void FT_Util_ExitApp(Ft_Gpu_Hal_Context_t *pHalContext);
EXTERN_C void init_fatfs(void);

EXTERN_C ft_int16_t FT_DispWidth;
EXTERN_C ft_int16_t FT_DispHeight;
EXTERN_C ft_uint32_t Ft_CmdBuffer_Index;

void updateMediaCompactInfo();

#ifdef FT9XX_PLATFORM
const char* MB_DB_FILENAME = ".media.db";
#else
const char* MB_DB_FILENAME = "/System/media.db";
#endif //FT9XX_PLATFORM

/*const char* MB_INDEX_FILENAME = "/System/media.idx";*/

#define PRESSED_COLOR (200<<8|100)
#define UNPRESS_COLOR (100<<24|50<<8)

//TODO: FIXME: use the matrix to move the picture
#define HIGHEST_ZOOM_PIXEL 1500
#define LOWEST_ZOOM_PIXEL  16
#define RANGE_ZOOM_PIXEL   (HIGHEST_ZOOM_PIXEL - LOWEST_ZOOM_PIXEL)
typedef enum {
	MEDIA_SCALE_X,
	MEDIA_SCALE_Y
} ScaleAxis_e;

typedef enum {
	MEDIA_TYPE_VIDEO = 0xE0,
	MEDIA_TYPE_AUDIO = 0xE1,
	MEDIA_TYPE_PIC	 = 0xE2,
	MEDIA_TYPE_INVALID = 0xE3,
} MediaType_e;

typedef enum {
	MEDIA_STATEMACHINE_MAIN = 0,
	MEDIA_STATEMACHINE_DISPLAY_DETAIL,
	MEDIA_STATEMACHINE_VIEW_PICTURE,
	MEDIA_STATEMACHINE_PLAY_AUDIO,
} MediaStateMachine_e;

typedef enum {
	MEDIA_ICON_EXIT = 0,
	MEDIA_ICON_BACK,
	MEDIA_ICON_SDCARD,
	MEDIA_ICON_EFLASH,
	MEDIA_ICON_PICTURE,
	MEDIA_ICON_AUDIO,
	MEDIA_ICON_SPEAKER,
	MEDIA_ICON_PLAY,
	MEDIA_ICON_PAUSE,
	MEDIA_ICON_VIDEO,
	//Add more icon before max
	MEDIA_ICON_MAX,
} MediaIcon_e;

struct font_info_t {
	ft_uint8_t w;
	ft_uint8_t h;
	ft_uint8_t id;
} FONT_LIST[] = {
	{ 8, 15, 26}, //tiny
	{ 12, 20, 28}, //medium
	{ 16, 26, 30}, //huge
};

#define DRAG  3000    // Loss in velocity each time wheel passes slot
#define DRIFT 1000    // Speed of wheel drift after release
#if defined(MSVC_PLATFORM) || defined(BT8XXEMU_PLATFORM) 
#define SCROLL_DELAY 2UL
#else
#define SCROLL_DELAY 100UL
#endif
typedef struct {
	ft_uint8_t scrollDelay;
	ft_int8_t scrollDirection; //2: is scroll up, 1: down, 0: no scroll
	int mytag_start;
	int mytag_num;
	int lim;
	int dragging;
	int prev_drag;
	int prev_pos;
	int elem_size;
	int pos;
	int vel;
	int pgsz;
} scroller;

void scroller_init(scroller *inst, int tag_begin, int tag_num, int p, int d,
		int elem_sz, int _pgsz) {
	inst->mytag_start = tag_begin;
	inst->mytag_num = tag_num;
	inst->prev_pos = inst->pos = p << 16;
	inst->lim = d << 16;
	inst->vel = 0;
	inst->dragging = 0;
	inst->prev_drag = 0;
	inst->elem_size = elem_sz;
	inst->pgsz = (_pgsz - 2) << 16;
	inst->scrollDelay = SCROLL_DELAY;
	/*printf("scroller init: %d %d %d %d (%d %d) %d %d\n\r", tag_begin, tag_num, p, d, pos, lim, elem_size, pgsz);*/
}

ft_bool_t scroller_isScrolling(scroller *inst) {
	return (inst->vel != 0 || inst->scrollDelay != 0);
}

int scroller_mod(scroller *inst, int x) {
	if (x < 0) {
		x = 0;
		inst->vel = 0;
	}
	if (x >= inst->lim || (x + inst->pgsz) >= inst->lim) {
		if (inst->lim > inst->pgsz) {
			x = inst->lim - inst->pgsz;
		} else {
			x = 0;
		}
		inst->vel = 0;
	}
	return x;
}

void scroller_move(scroller *inst) {
	int _prev_pos = inst->pos;
	inst->pos = scroller_mod(inst, inst->pos + inst->vel);

	if (!inst->dragging && inst->vel) {
		if ((inst->pos ^ _prev_pos) & ~0xffff) {
			if (inst->vel > 0) {
				inst->vel -= DRAG;
				if (inst->vel <= 0) {
					inst->vel = 0;
					inst->pos = (inst->pos + 0xffff) & ~0xffff;
				}
			} else {
				inst->vel += DRAG;
				if (inst->vel >= 0) {
					inst->vel = 0;
					inst->pos &= ~0xffff;
				}
			}
		}
	} else if (inst->scrollDelay) {
		inst->scrollDelay--;
		/*if (!scrollDelay) printf("<scroll> delay done!\n");*/
	}
	/*if (inst->vel) {
	 printf("move: v=%d p=%d->%d %d\n\r", vel, pos, frac(elem_size), trunc(0));
	 }*/
}

int scroller_trunc(scroller *inst, int off) {
	return scroller_mod(inst, inst->pos + (off << 16)) >> 16;
}

int scroller_frac(scroller *inst, int ys) {
	if (inst->vel < 0 || inst->scrollDirection == 1) {
		return (ys * (0xffffL - (inst->pos & 0xffff)) >> 16);
	} else {
		return -(ys * (inst->pos & 0xffff) >> 16);
	}
}

void scroller_touch(scroller *inst, ft_int16_t p, ft_int16_t tag) {
	int istouch = (p != -32768);

	if (!istouch) {
		if (inst->dragging) {
			inst->dragging = 0;
			if (inst->vel) {
				inst->vel = DRAG * (inst->vel / DRAG);
				if (inst->vel == 0)
					inst->vel = (inst->pos & 0x8000) ? DRIFT : -DRIFT;
			} else {
				if (inst->scrollDirection == 2)
					inst->pos = (inst->pos + 0xffff) & ~0xffff;
				else
					inst->pos = (inst->pos - 0xffff) & ~0xffff;
			}
			inst->scrollDelay = SCROLL_DELAY;
			inst->scrollDirection = 0;
			printf("<scroll> Done drag! %d %d\n\r", inst->vel, inst->scrollDelay);
		}
		inst->prev_drag = -32768;
	} else if (!inst->dragging && inst->mytag_start <= tag
			&& tag < (inst->mytag_start + inst->mytag_num)) {
		//atlease diff is 10 pixels
		if (inst->prev_drag == -32768) {
			inst->prev_drag = p;
		} else if ((p - inst->prev_drag) >= inst->elem_size / 3
				|| (p - inst->prev_drag) <= -inst->elem_size / 3) {
			inst->dragging = 1;
			inst->prev_drag = p;
			printf("<scroll> start drag! %d %d\n\r", inst->prev_drag, p);
		}
	}
	if (inst->dragging) {
		/*vel = -((p - prev_drag) << 16) / elem_size;*/
		if ((p - inst->prev_drag) > 3 || (p - inst->prev_drag) < -3)
			inst->vel = -((p - inst->prev_drag) << 16) / inst->elem_size;
		else
			inst->vel = 0;
		if (inst->vel > 0)
			inst->scrollDirection = 2;
		else if (inst->vel < 0)
			inst->scrollDirection = 1;
		printf("Touch: %d %d \n\r", inst->vel, -(p - inst->prev_drag));
		inst->prev_drag = p;
	}
}

//Extract media information
struct MediaInfo_t {
	ft_uint32_t fsize;
	ft_uint32_t fdate;
#define MEDIA_INFO_INVAL_FILE  0x0001 //This is invalid file!
#define MEDIA_INFO_EFLASH_FILE 0x0002 //This is eflash file
	ft_uint16_t flag;
	ft_int16_t dur;//in seconds
	union {
		//video
		struct {
			ft_int16_t w;
			ft_int16_t h;
			ft_int16_t fps;
			ft_uint8_t fmt, palsize;
		};
		struct {
			ft_uint32_t aufmt;
			ft_int32_t freq; //for audio
			int nsamples;
			ft_int16_t codec;
			ft_int16_t bitstream; //for audio
			ft_uint8_t bps;
		};
	};
};

struct data_window_t {
	ft_int32_t start; //start of windows
	ft_int32_t size; //size of window
};

union MediaDb_t{
	struct {
#define MEDIA_DB_MAGIC	('M'<<24|'e'<<16|'D'<<8|'i')
		ft_int32_t magic;
#define MEDIA_DB_VERSION	0x00FF0004
		ft_int32_t version;
		ft_int32_t screenId;
		ft_int32_t totMediaFile;
		ft_int32_t endNamePtr;
		ft_int32_t indexListPtr;
		ft_int32_t lastRamIdx;	 //last ram IDX
	};
	ft_uint8_t res[32];
};

enum MediaStorageType_e {
	MEDIA_STORAGE_TYPE_SDCARD = MEDIAPLAYER_KEY_SDCARD,
	MEDIA_STORAGE_TYPE_EFLASH = MEDIAPLAYER_KEY_EFLASH,
	MEDIA_STORAGE_TYPE_INV
};

struct MediaItem_t{
	ft_int32_t row; //tag=row*colNum + col
	ft_int32_t col;
#define MEDIAITEM_FLAG_SELECTED 	0x0001
	ft_uint32_t flags;
	ft_uint8_t  type;//MediaType_e
	char *fname;
};

typedef struct {
	Ft_Gpu_Hal_Context_t *pHalContext;
	MediaStateMachine_e state;
	int tileW;
	int tileH;
	int rowNum;
	int colNum;
	int headerSize;
	volatile enum MediaStorageType_e storageType;
#if defined(FT900_PLATFORM)
	FIL 	dbFile;
#else
	//Win32
	FILE    *dbFile;
#endif
#define MEDIA_GLOBAL_FLAG_SCANNING    0x0001
#define MEDIA_GLOBAL_FLAG_PARSING     0x0002
#define MEDIA_GLOBAL_FLAG_INITED      0x0004
#define MEDIA_GLOBAL_MANUAL_SCROLLING 0x0008
#define MEDIA_GLOBAL_AUTO_SCROLLING   0x0010
#define MEDIA_GLOBAL_NOSDCARD         0x0020
#define MEDIA_GLOBAL_SCANNED          0x0040 //already scanned sdcard ?
#define MEDIA_GLOBAL_UPDATED_INFO     0x0080 //already updated compact info?
#define MEDIA_GLOBAL_SCROLLING_CLEAR  0x0100 //already updated compact info?
#define MEDIA_GLOBAL_AUDIO_PLAYING    0x0200
#define MEDIA_GLOBAL_AUDIO_CONT_PLAYING    0x0400
#define MEDIA_GLOBAL_NOUSBSTORAGE     0x0800
	ft_uint32_t UiFlags;
	//Touch:
	ft_uint32_t    touchTimeStamp;
	ft_uint8_t 	CurrTag;			/* Present TAG value */
	ft_uint8_t 	PrevTag;			/* Previous TAG value */
	ft_uint8_t 	PenDown;			/* Flag to indicate pendown or up */
	ft_uint16_t    CurrX;
	ft_uint16_t    CurrY;

	GuiManager *CleO;
	struct gram_t gram;
	scroller	scrollobj;
#define MEDIABROWSER_ICON_HANDLE_REFRESH (2)
#define MEDIABROWSER_THUMNAIL_HANDLE     (3)
	struct bm_info_t iconList[MEDIA_ICON_MAX];
	char        audioFile[MEDIA_COMMAND_BUFFER_SIZE];
	char        scratchBuffer[1<<10];//8k is enough for 1 sector ?
#if defined(MSVC_PLATFORM) || defined(BT8XXEMU_PLATFORM) 
	char        mediaNameBuffer[128<<10];//36KB is enough for 1 sector ?
#elif defined(FT9XX_PLATFORM)
	char        mediaNameBuffer[36<<10];//36KB is enough for 1 sector ?
#else
	char        mediaNameBuffer[30<<10];//30KB is enough for 1 sector ?
#endif
#if defined(MSVC_PLATFORM) || defined(BT8XXEMU_PLATFORM) 
#define MEDIABROWSER_MAX_INDEX_BUFFER (4095L)
#else
#define MEDIABROWSER_MAX_INDEX_BUFFER (1023L)
#endif
	ft_uint16_t mediaIndex[MEDIABROWSER_MAX_INDEX_BUFFER+1];		//Index to mediaNameBuffer for speed up searching
	union MediaDb_t dbData;
#define MEDIABROWSER_MAX_DISPLAY_ITEM 200 //The FT81x support maximum 255 tag
#define MEDIABROWSER_ITEM_TAG_BEGIN	  10  //use 10 as beginning tag
//	struct MediaItem_t mediaItem[MEDIABROWSER_MAX_DISPLAY_ITEM];
	int         selMedia;
	struct MediaInfo_t mediaInfo;
	struct data_window_t dispWindow; //Current view
	struct data_window_t ramWindow;  //all data on RAM
	ft_uint32_t dispUICnt;
	struct MediaInfo_t compactInfoList[0];
} MultimediaExp_Global_t;

static MultimediaExp_Global_t *g_MultimediExp_Ctx;
static Ft_Gpu_Hal_Context_t host;

static void MediaBrowser_RenderUI();
void helperEndRender(Ft_Gpu_Hal_Context_t *pHalContext);
void helperBeginRender(Ft_Gpu_Hal_Context_t *pHalContext);

static void MediaBrowser_ResetDbData(union MediaDb_t *pData)
{
	memset(pData, 0, sizeof *pData);
	pData->version = MEDIA_DB_VERSION;
	pData->magic = MEDIA_DB_MAGIC;
#ifndef FT9XX_PLATFORM
	pData->screenId = gui_moduleid();
#endif
	pData->lastRamIdx = 0;
}

static void MediaBrowser_ResetMediaItem()
{
	memset(g_MultimediExp_Ctx->mediaIndex, 0xFF, sizeof(g_MultimediExp_Ctx->mediaIndex));
	g_MultimediExp_Ctx->dispWindow.size = 0;
	g_MultimediExp_Ctx->dispWindow.start = -1;
	g_MultimediExp_Ctx->ramWindow.size = 0;
	g_MultimediExp_Ctx->ramWindow.start = -1;
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
				*s++ = ' ';
				for (int j = 0; j < 16; j++) {
					if (isprint(buf[i+j-16])) {
						*s++ = buf[i+j-16];
					} else {
						*s++ = '.';
					}
				}
				*s++ = '\n';
				*s++ = '\r';
				*s++ = 0;
				uart_puts(UART0, line);
			}
			s = line + sprintf(line, "%03d: ", i);
		}
		s += sprintf(s, "%02x ", buf[i]);
	}
	*s++ = '\n';
	*s++ = '\r';
	*s++ = 0;
	uart_puts(UART0, line);
#define uart_puts(a,b)
}
#endif

MediaType_e detectMediaType(const char* fname) {
	char *s;
	MediaType_e type = MEDIA_TYPE_INVALID;

	if (((s = strstr(fname, ".avi")) != NULL && strlen(s) == 4) ||
		((s = strstr(fname, ".jpg")) != NULL && strlen(s) == 4) ||
		((s = strstr(fname, ".jpeg")) != NULL && strlen(s) == 5) ||
		((s = strstr(fname, ".png")) != NULL && strlen(s) == 4) ||
		((s = strstr(fname, ".bmp")) != NULL && strlen(s) == 4) ||
		((s = strstr(fname, ".wav")) != NULL && strlen(s) == 4))
	{
		switch (s[1]) {
		case 'a':
			type = MEDIA_TYPE_VIDEO;
			break;
		case 'j':
		case 'p':
		case 'b':
			type = MEDIA_TYPE_PIC;
			break;
		case 'w':
			type = MEDIA_TYPE_AUDIO;
			break;
		}
	}
	return type;
}

static ft_int32_t convertMediaFilename(const char* prefix, const char* fname, char *buffer, int sep)
{
	int len = strlen(fname);
	char *s;

	if (prefix != NULL) {
		len += strlen(prefix);
	}
	if (len >= 252)
		return -1;
	if (prefix != NULL) {
#if defined(MSVC_PLATFORM) || defined(BT8XXEMU_PLATFORM) 
		sprintf(&buffer[2], "%s%s%s", prefix, sep ? "\\" : "", fname);
#else
		sprintf(&buffer[2], "%s%s%s", prefix, sep ? "/" : "", fname);
#endif
		if (sep) len++;
	} else {
		strcpy(&buffer[2], fname);
	}
	s = &buffer[2];
	while(*s && *s != '.') s++;
	while(*s) {
		if ('A' <= *s && *s <='Z')
			*s += 'a' - 'A';
		s++;
	}
	buffer[1] = len + 3;
	buffer[0] = detectMediaType(fname);
	if (buffer[0] == (char)MEDIA_TYPE_INVALID)
		return -1;
	return (len+3);
}

#if defined(MSVC_PLATFORM) || defined(BT8XXEMU_PLATFORM) 
void DisplayErrorBox(LPTSTR lpszFunction)
{
	// Retrieve the system error message for the last-error code

	LPVOID lpMsgBuf;
	LPVOID lpDisplayBuf;
	DWORD dw = GetLastError();

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dw,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf,
		0, NULL);

	// Display the error message and clean up

	lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT,
		(lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)lpszFunction) + 40) * sizeof(TCHAR));
	StringCchPrintf((LPTSTR)lpDisplayBuf,
		LocalSize(lpDisplayBuf) / sizeof(TCHAR),
		TEXT("%s failed with error %d: %s"),
		lpszFunction, dw, lpMsgBuf);
	MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK);

	LocalFree(lpMsgBuf);
	LocalFree(lpDisplayBuf);
}

typedef struct folderList_s {
	size_t size;
	size_t csize; //current size
	int head;
	int tail;
	char** data;
} folderList_t;

void _folderInit(folderList_t *inst, size_t size)
{
	memset(inst, 0, sizeof(*inst));
	inst->size = size;
	inst->data = malloc(sizeof(char*)*size);
	if (inst->data == NULL) printf("ERROR! Fail to allocate mem for folderList\n");
}

void _folderPush(folderList_t *inst, const char* fname)
{
#define FOLDERLIST_REALLOC_SIZE 512
	if (inst->csize == inst->size) {
#if 0
		//TODO: need memmove!
		//Re-allocate
		char** newdata = realloc(inst->data, sizeof(char*)*(inst->size + FOLDERLIST_REALLOC_SIZE));
		if (newdata == NULL) {
			printf("ERROR! Realloc fail!\n");
			return;
		}
		inst->data = newdata;
#else
		printf("Full folder list!\n");
		return;
#endif
	}
	inst->data[inst->head++] = strdup(fname);
	if (inst->head == inst->size) inst->head = 0;
	inst->csize++;
}

int _folderPop(folderList_t *inst, char* fname, size_t sz)
{
	if (inst->csize == 0) return -1;//empty case
	char* name = inst->data[inst->tail];
	inst->data[inst->tail] = NULL;
	inst->tail++;
	if (inst->tail >= inst->size) inst->tail = 0;
	inst->csize--;
	strncpy(fname, name, sz);
	free(name);
}

void _folderDestroy(folderList_t *inst)
{
	int i = inst->tail;

	while (inst->csize--) {
		free(inst->data[i++]);
		if (i == inst->size) i = 0;
	}
	free(inst->data);
	inst->size = 0;
}

int _folderEmptry(folderList_t *inst)
{
	return (inst->csize == 0);
}

static ft_int32_t MediaBrowser_FolderScan(TCHAR* path)
#else
static ft_int32_t MediaBrowser_FolderScan(char* path)
#endif
{
	//TODO:FIXME
#if defined(FT900_PLATFORM)
#define LOCAL_BUFFER_SIZE 256
	FRESULT res;
	FILINFO info;
	DIR dir;
	char *fname;
	char *s;
	int read_ptr;
	int write_ptr;
	FIL tmpFile;
	UINT num;
	char buffer[LOCAL_BUFFER_SIZE];

#if _USE_LFN
	char longfn[_MAX_LFN+1];

	info.lfname = longfn;
	info.lfsize = sizeof(longfn);
#endif

	if (g_MultimediExp_Ctx->UiFlags & MEDIA_GLOBAL_SCANNED) {
		//Already scanned!
		return 0;
	}
	//TODO: check if sdcard is inserted
	MediaBrowser_ResetDbData(&g_MultimediExp_Ctx->dbData);
	MediaBrowser_ResetMediaItem();
	res = f_open(&g_MultimediExp_Ctx->dbFile, MB_DB_FILENAME, FA_READ|FA_WRITE|FA_OPEN_ALWAYS);
	if (res != F_OK) {
		return -1;
	}
	f_lseek(&g_MultimediExp_Ctx->dbFile, sizeof(g_MultimediExp_Ctx->dbData));

	//Scanning sdcard
	if(strlen(path) == 1 && *path == '/') {
		g_MultimediExp_Ctx->scratchBuffer[0] = 0;
	} else {
		strcpy(g_MultimediExp_Ctx->scratchBuffer, path);
	}
	res = f_opendir(&dir, path);
	printf("Scanning folder %s res=%d eflash=%d ...\n\r", path, res, g_MultimediExp_Ctx->dbData.totMediaFile);
	if (res != F_OK) {
		return -1;
	}
#ifdef FT9XX_PLATFORM
	res = f_open(&tmpFile, ".media5.tmp", FA_READ | FA_WRITE | FA_OPEN_ALWAYS);
#else
	res = f_open(&tmpFile, "/System/.media5.tmp",
				 FA_READ | FA_WRITE | FA_OPEN_ALWAYS);
#endif
	if (res != FR_OK) {
		printf("Fail to open tmpFile\n");
	}
	read_ptr = 0;
	write_ptr = 0;
	f_lseek(&tmpFile, 0);
	do{
		while (1) {
			res = f_readdir(&dir, &info);
#       if _USE_LFN
			if (*(info.lfname))
			{
				fname = info.lfname;
			}
			else
#       endif
			{
				fname = info.fname;
			}
//			fname = info.fname;
			/* Break if we have finished or an error has occured */
			if ((FR_OK != res) || (info.fname[0] == 0))
				break;
			//To upper case
			s = fname;
			while (*s) {
				if ('A' <= *s && *s <= 'Z')
					*s = (*s - 'A') + 'a';
				s++;
			}
			printf("File '%s' '%s' %d \n\r", info.fname, info.lfname, f_tell(&tmpFile));
			if (info.fattrib & AM_DIR) {
				sprintf(buffer,"%s/%s", g_MultimediExp_Ctx->scratchBuffer, fname);
				f_lseek(&tmpFile, write_ptr);
				res = f_write(&tmpFile, buffer, strlen(buffer) + 1, &num);
				if (num != (strlen(buffer) + 1)) {
					printf("Fail to write \'%s\' to tmp file! %d %d %d\n\r", buffer, res, num, write_ptr);
				} else {
					write_ptr = f_tell(&tmpFile);
					//write_ptr += strlen(buffer) + 1;
					printf("Save folder '%s' %d %d \n\r", buffer, write_ptr, read_ptr);
				}
			} else {
				int l = convertMediaFilename(g_MultimediExp_Ctx->scratchBuffer, fname, buffer, 1);
				if(l > 0) {
					printf(">>Insert: [%d] %s l=%d\n\r", g_MultimediExp_Ctx->dbData.totMediaFile, &buffer[2], l);
					f_write(&g_MultimediExp_Ctx->dbFile, buffer, l, &num);
					g_MultimediExp_Ctx->dbData.totMediaFile++;
					g_MultimediExp_Ctx->dbData.endNamePtr = f_tell(&g_MultimediExp_Ctx->dbFile);
				}
			}
			g_MultimediExp_Ctx->CleO->service_playback();
		}
		f_closedir(&dir);
		do {
			printf("tmpFile: %d %d\n", f_size(&tmpFile), f_size(&tmpFile));
			f_lseek(&tmpFile, read_ptr);
			res = f_read(&tmpFile, buffer, LOCAL_BUFFER_SIZE, &num);
			if (res != FR_OK) {
				printf("Fail to read tmpFile: res=%d %d\n\r", res, read_ptr);
				goto out_done;
			}
			printf("Read tmpFile: res=%d %d %d %d\n\r", res, read_ptr, f_tell(&tmpFile), f_size(&tmpFile));
			s = buffer;
			read_ptr += strlen(buffer) + 1;
			printf("------ Scan '%s' %d %d %d\n\r", buffer, read_ptr, f_tell(&tmpFile), f_size(&tmpFile));
			res = f_opendir(&dir, buffer);
			if (res == FR_OK) {
				strcpy(g_MultimediExp_Ctx->scratchBuffer, buffer);
				break;
			}
			printf("opendir '%s' fail %d\n\r", buffer, res);
			goto out_done;
		}while(read_ptr < write_ptr);
		g_MultimediExp_Ctx->CleO->service_playback();
		//Read from tmp file
	} while (read_ptr < write_ptr);
out_done:
	{
		//Write the index
		write_ptr = f_tell(&g_MultimediExp_Ctx->dbFile);
		//Align to sector size
		g_MultimediExp_Ctx->dbData.indexListPtr = write_ptr = (write_ptr + FILE_SECTOR_SIZE - 1) & ~(FILE_SECTOR_SIZE-1);
		f_lseek(&g_MultimediExp_Ctx->dbFile, write_ptr);
		read_ptr = sizeof(g_MultimediExp_Ctx->dbData);
		ft_uint32_t *pIdxBuf = (ft_uint32_t*)g_MultimediExp_Ctx->mediaNameBuffer;
		int numIdxPerBuf = sizeof(g_MultimediExp_Ctx->mediaNameBuffer)/sizeof(*pIdxBuf);
		int wrtIdx = 0;
		ft_uint8_t hdr[2];
		while(wrtIdx < g_MultimediExp_Ctx->dbData.totMediaFile) {
			int maxRdIdx = min(numIdxPerBuf, g_MultimediExp_Ctx->dbData.totMediaFile - wrtIdx);
			for (int j = 0; j < maxRdIdx; j++) {
				if (f_lseek(&g_MultimediExp_Ctx->dbFile, read_ptr) != FR_OK) {
					goto out;
				}
				if (f_read(&g_MultimediExp_Ctx->dbFile, hdr, 2, &num) != FR_OK) {
					goto out;
				}
				pIdxBuf[j] = read_ptr;
				read_ptr += hdr[1];
			}
			//Write to file
			if (f_lseek(&g_MultimediExp_Ctx->dbFile, write_ptr) != FR_OK) {
				goto out;
			}
			if (f_write(&g_MultimediExp_Ctx->dbFile, pIdxBuf, sizeof(*pIdxBuf)*maxRdIdx, &num) != FR_OK) {
				goto out;
			}
			wrtIdx += maxRdIdx;
			write_ptr = f_tell(&g_MultimediExp_Ctx->dbFile);
			g_MultimediExp_Ctx->CleO->service_playback();
		}
out:
		f_lseek(&g_MultimediExp_Ctx->dbFile, write_ptr);
	}
	printf("Detect %d media files\n\r", g_MultimediExp_Ctx->dbData.totMediaFile);
	g_MultimediExp_Ctx->UiFlags |= MEDIA_GLOBAL_SCANNED;
	printf("close tmpFile: %d %d\n", f_tell(&tmpFile), f_size(&tmpFile));
	f_close(&tmpFile);

	//Truncate to current size
	f_truncate(&g_MultimediExp_Ctx->dbFile);
	//Write database header information
	f_lseek(&g_MultimediExp_Ctx->dbFile, 0);
	f_write(&g_MultimediExp_Ctx->dbFile, &g_MultimediExp_Ctx->dbData, sizeof(g_MultimediExp_Ctx->dbData), &num);
	f_close(&g_MultimediExp_Ctx->dbFile);
	return 0;
#undef LOCAL_BUFFER_SIZE
#else
	//WIN32
	WIN32_FIND_DATA ffd;
	LARGE_INTEGER filesize;
	char szDir[MAX_PATH];
	size_t length_of_arg;
	HANDLE hFind = INVALID_HANDLE_VALUE;
	DWORD dwError = 0;
	//char c_szText[MAX_PATH];
	FILE *dbFile;
	char c_szText[MAX_PATH];
	char buffer[MAX_PATH];
	int write_ptr, read_ptr;
	int num;
#define MAX_INDEX_BUFFER (64<<10)
	ft_uint32_t *pIdxBuffer = NULL;
#define MAX_FOLDER_LIST 1024
	folderList_t folderList;
	char folderName[MAX_PATH];

	// Check that the input path plus 3 is not longer than MAX_PATH.
	// Three characters are for the "\*" plus NULL appended below.
	StringCchLength(path, MAX_PATH, &length_of_arg);
	if (g_MultimediExp_Ctx->UiFlags & MEDIA_GLOBAL_SCANNED) {
		//Already scanned!
		return 0;
	}
	//TODO: check if sdcard is inserted
	MediaBrowser_ResetDbData(&g_MultimediExp_Ctx->dbData);
	MediaBrowser_ResetMediaItem();
	dbFile = fopen(MSVC_MEDIA_DB_FILENAME, "w+");
	if (dbFile == NULL) {
		printf("Open db file %s for write fail!\n", MSVC_MEDIA_DB_FILENAME);
		goto out_close;
	}
	fseek(dbFile, sizeof(g_MultimediExp_Ctx->dbData), SEEK_SET);
	if (length_of_arg > (MAX_PATH - 3))
	{
		printf("\nDirectory path is too long.\n");
		goto out_close;
	}
	// Find the first file in the directory.
	// List all the files in the directory with some info about them.
	pIdxBuffer = malloc(MAX_INDEX_BUFFER * 4);
	if (pIdxBuffer == NULL) {
		printf("Out of mem!\n");
		goto out_close;
	}
	_folderInit(&folderList, MAX_FOLDER_LIST);
	_folderPush(&folderList, "");
	do {
		printf("\nTarget directory is %s\n\n", path);
		if (_folderPop(&folderList, folderName, MAX_PATH) < 0)
			break;

		// Prepare string for use with FindFile functions.  First, copy the
		// string to a buffer, then append '\*' to the directory name.
#if 0
		StringCchCopy(szDir, MAX_PATH, path);
		StringCchCat(szDir, MAX_PATH, TEXT("\\*"));
#endif
		if (strlen(folderName) > 0) {
			snprintf(szDir, MAX_PATH, "%s\\%s\\*", path, folderName);
		}
		else {
			snprintf(szDir, MAX_PATH, "%s\\*", path);
		}
		printf("*** Scanning folder %s ***\n", szDir);
		//hFind = FindFirstFileA(TEXT(TEST_DIR), &ffd);
		hFind = FindFirstFileA(szDir, &ffd);
		if (INVALID_HANDLE_VALUE == hFind)
		{
			DisplayErrorBox(TEXT("FindFirstFile"));
			goto out_close;
		}
		do
		{
			//wcstombs(c_szText, ffd.cFileName, wcslen(ffd.cFileName) + 1);
			//Lower case:
			//FIXME: it is not safe if we use unicode
			//Convert TChar to Char
			wcstombs(c_szText, ffd.cFileName, wcslen(ffd.cFileName) + 1);

			if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				int flen = strlen(c_szText);
				//printf("  %s   <DIR>\n", ffd.cFileName);
				if (c_szText[0] != '.') 
				{
					snprintf(buffer, MAX_PATH, "%s\\%s", folderName, c_szText);
					_folderPush(&folderList, buffer);
				}
			}
			else
			{
				char* s = c_szText;
				while (*s) {
					if (*s >= 'A' && *s <= 'Z') *s = *s - 'A' + 'a';
					s++;
				}
				filesize.LowPart = ffd.nFileSizeLow;
				filesize.HighPart = ffd.nFileSizeHigh;
				//printf("  %s   %ld bytes\n", c_szText, filesize.QuadPart);
				//Adding file to database
				//int l = convertMediaFilename(c_szText, fname, buffer, 1);
				int length = convertMediaFilename(folderName, c_szText, buffer, 1);
				if (length > 0) {
					//printf(">>Insert: [%d] %s l=%d\n\r", g_MultimediExp_Ctx->dbData.totMediaFile, &buffer[2], length);
					pIdxBuffer[g_MultimediExp_Ctx->dbData.totMediaFile] = ftell(dbFile);
					num = fwrite(buffer, 1, length, dbFile);
					g_MultimediExp_Ctx->dbData.totMediaFile++;
					g_MultimediExp_Ctx->dbData.endNamePtr = ftell(dbFile);
					if (g_MultimediExp_Ctx->dbData.totMediaFile >= MAX_INDEX_BUFFER) break;
				}
			}
			//TODO: implement recursive parse sub-directories!
		} while (FindNextFile(hFind, &ffd) != 0);
		FindClose(hFind);
	} while (!_folderEmptry(&folderList));

	dwError = GetLastError();
	if (dwError != ERROR_NO_MORE_FILES)
	{
		DisplayErrorBox(TEXT("FindFirstFile"));
	}
	//Update
	{
		//Update write to file
		write_ptr = ftell(dbFile);
		//Align to sector size
		g_MultimediExp_Ctx->dbData.indexListPtr = write_ptr = (write_ptr + FILE_SECTOR_SIZE - 1) & ~(FILE_SECTOR_SIZE - 1);

		printf("Write index path at: %d %d index \n", write_ptr, g_MultimediExp_Ctx->dbData.totMediaFile);
		read_ptr = sizeof(g_MultimediExp_Ctx->dbData);
		fseek(dbFile, write_ptr, SEEK_SET);
		num = fwrite(pIdxBuffer, 4, g_MultimediExp_Ctx->dbData.totMediaFile, dbFile);
		if (num != g_MultimediExp_Ctx->dbData.totMediaFile) {
			printf("ERROR! Fail to write index data!\n");
		}
		free(pIdxBuffer);
		pIdxBuffer = NULL;
	}
	printf("Detect %d media files\n\r", g_MultimediExp_Ctx->dbData.totMediaFile);
	g_MultimediExp_Ctx->UiFlags |= MEDIA_GLOBAL_SCANNED;

	//Truncate to current size
	write_ptr = ftell(dbFile);
	//Write database header information
	fseek(dbFile, 0, SEEK_SET);
	num = fwrite(&g_MultimediExp_Ctx->dbData, 1, sizeof(g_MultimediExp_Ctx->dbData), dbFile);
	fseek(dbFile, write_ptr, SEEK_SET);
	fclose(dbFile);
	dbFile = NULL;
	{
		//Truncate the file
		int fh, result;
		if (_sopen_s(&fh, MSVC_MEDIA_DB_FILENAME, _O_RDWR, _SH_DENYNO, _S_IREAD | _S_IWRITE) == 0) {
			printf("File length before: %ld\n", _filelength(fh));
			if ((result = _chsize(fh, write_ptr)) == 0)
				printf("Size successfully changed\n");
			else
				printf("!!!Problem in changing the size\n");
			printf("File length after:  %ld\n", _filelength(fh));
			_close(fh);
		}
	}
	_folderDestroy(&folderList);
	return 0;
out_close:
	_folderDestroy(&folderList);
	if (pIdxBuffer != NULL) free(pIdxBuffer);
	if (dbFile != NULL) fclose(dbFile);
	return -1;
#endif
}

enum MediaBrowserParseDB_e {
	MEDIA_PARSEDB_REFRESH,
	MEDIA_PARSEDB_FWD,
	MEDIA_PARSEDB_BWD,
};

ft_int32_t MediaBrowser_ParseDb(enum MediaBrowserParseDB_e cmd)
{
	int res;
	int num = 0;
	int ret = 0;
	char *s;
	int i = 0;
	int read_ptr;
	int pgSz = g_MultimediExp_Ctx->rowNum*g_MultimediExp_Ctx->colNum;
	int maxSize;
#if defined(FT900_PLATFORM)
	FIL *dbFile = &g_MultimediExp_Ctx->dbFile;
#else
	FILE *dbFile;
#endif

#if defined(MSVC_PLATFORM) || defined(BT8XXEMU_PLATFORM) 
	if (g_MultimediExp_Ctx->UiFlags & MEDIA_GLOBAL_FLAG_SCANNING) return -1;
#else
#endif
	if (
#if !defined(MSVC_PLATFORM) && !defined(BT8XXEMU_PLATFORM) 
		(g_MultimediExp_Ctx->UiFlags & MEDIA_GLOBAL_NOSDCARD) || (g_MultimediExp_Ctx->storageType != MEDIA_STORAGE_TYPE_SDCARD) ||
#endif
			(cmd == MEDIA_PARSEDB_FWD &&
			(g_MultimediExp_Ctx->ramWindow.start + g_MultimediExp_Ctx->ramWindow.size) >= g_MultimediExp_Ctx->dbData.totMediaFile) //Last page, could not fwd
		|| (cmd == MEDIA_PARSEDB_BWD &&
			(g_MultimediExp_Ctx->ramWindow.start <= 0))
			)
	{
		printf("No parse!\n\r");
		return -1;
	}

#if defined(MSVC_PLATFORM) || defined(BT8XXEMU_PLATFORM) 
	dbFile = fopen(MSVC_MEDIA_DB_FILENAME, "r+b");
	if (dbFile == NULL) {
		printf("Fail to open file db %s \n", MSVC_MEDIA_DB_FILENAME);
		return -1;
	}
#else
	res = f_open(dbFile, MB_DB_FILENAME, FA_READ|FA_WRITE|FA_OPEN_ALWAYS);
	if (res != FR_OK) {
		printf("Read Error!\n\r");
		return -1;
	}
#endif
	res = F_SEEK(dbFile, 0);
	if (res != FR_OK) {
		printf("Seek Error!\n\r");
		return -1;
	}
	g_MultimediExp_Ctx->UiFlags |= MEDIA_GLOBAL_FLAG_PARSING;
	MediaBrowser_RenderUI();
	//Read the header first
	F_READ(dbFile, &g_MultimediExp_Ctx->dbData, sizeof(g_MultimediExp_Ctx->dbData), num);
	if (num <= 0) {
		printf("Read header fail!\n\r");
		ret = -1;
		goto out_close;
	}
	//checking magic
	if (g_MultimediExp_Ctx->dbData.magic != MEDIA_DB_MAGIC ||
		g_MultimediExp_Ctx->dbData.version != MEDIA_DB_VERSION
#ifndef FT9XX_PLATFORM
		|| g_MultimediExp_Ctx->dbData.screenId != gui_moduleid()
#endif
		)
	{
		printf("Mismatching version.\n\r");
		ret = -2;
		goto out_close;
	}

	//Check the command!
	switch(cmd) {
	case MEDIA_PARSEDB_REFRESH:
		//Get the last read pointer
		break;
	case MEDIA_PARSEDB_FWD:
		//Move next
		{
			int lastIdx = g_MultimediExp_Ctx->dbData.lastRamIdx;
			g_MultimediExp_Ctx->dbData.lastRamIdx += (MEDIABROWSER_MAX_INDEX_BUFFER*2/3);
			if ((g_MultimediExp_Ctx->dbData.lastRamIdx + MEDIABROWSER_MAX_INDEX_BUFFER) > g_MultimediExp_Ctx->dbData.totMediaFile) {
				g_MultimediExp_Ctx->dbData.lastRamIdx = g_MultimediExp_Ctx->dbData.totMediaFile - MEDIABROWSER_MAX_INDEX_BUFFER;
			}
			if (lastIdx >= g_MultimediExp_Ctx->dbData.lastRamIdx) {
				//Case not enough memory for all index
				g_MultimediExp_Ctx->dbData.lastRamIdx = lastIdx + pgSz;
			}
		}
		break;
	case MEDIA_PARSEDB_BWD:
		//Move prev
		g_MultimediExp_Ctx->dbData.lastRamIdx -= (MEDIABROWSER_MAX_INDEX_BUFFER*2/3);
		if ((g_MultimediExp_Ctx->dbData.lastRamIdx) < 0) {
			g_MultimediExp_Ctx->dbData.lastRamIdx = 0;
		}
		break;
	default:
		ret = -5;
		printf("Unknown command: %d!\n\r", cmd);
		goto out_close;
	}
	read_ptr = g_MultimediExp_Ctx->dbData.indexListPtr + g_MultimediExp_Ctx->dbData.lastRamIdx*4;
	//Read the index
	if (F_SEEK(dbFile, read_ptr) != FR_OK ||
		(F_READ(dbFile, &read_ptr, 4, num) <= 0 && num<=0)) {
		ret = -5;
		printf("read/seek fail rptr=%d\n\r", read_ptr);
		goto out_close;
	}
	printf("read_ptr: %d\n\r", read_ptr);
	//Read data now
	if (F_SEEK(dbFile, read_ptr) != FR_OK ||
		(F_READ(dbFile, g_MultimediExp_Ctx->mediaNameBuffer, sizeof(g_MultimediExp_Ctx->mediaNameBuffer), num) <= 0 && num <= 0))
	{
		ret = -6;
		goto out_close;
	}

	/*dump_buffer((unsigned char*)g_MultimediExp_Ctx->mediaNameBuffer, num);*/

	g_MultimediExp_Ctx->ramWindow.start = g_MultimediExp_Ctx->dbData.lastRamIdx;
	g_MultimediExp_Ctx->ramWindow.size = -1;
	printf("lastRamIdx: %d indexListPtr: %d %d %d\n\r",
			g_MultimediExp_Ctx->dbData.lastRamIdx,
			g_MultimediExp_Ctx->dbData.indexListPtr,
			g_MultimediExp_Ctx->dbData.totMediaFile,
			g_MultimediExp_Ctx->ramWindow.start);
	if ((g_MultimediExp_Ctx->ramWindow.start + MEDIABROWSER_MAX_INDEX_BUFFER) > g_MultimediExp_Ctx->dbData.totMediaFile)
	{
		maxSize = g_MultimediExp_Ctx->dbData.totMediaFile;
	} else {
		maxSize = g_MultimediExp_Ctx->ramWindow.start + MEDIABROWSER_MAX_INDEX_BUFFER;
	}
//	g_MultimediExp_Ctx->dispWindow.start = g_MultimediExp_Ctx->dbData.lastRamIdx; ???
	//Now read data
	s = &g_MultimediExp_Ctx->mediaNameBuffer[0];
	for (i = 0;
		((i + g_MultimediExp_Ctx->ramWindow.start) < maxSize); i++) {
		g_MultimediExp_Ctx->mediaIndex[i] = s - g_MultimediExp_Ctx->mediaNameBuffer;
		//Check for the next data
		//[0]: type, [1]: length
		ft_uint8_t slen;
		ft_uint8_t type = *s++;
		slen = *s++;
		if (((s - g_MultimediExp_Ctx->mediaNameBuffer) < num))
		{
			if (strlen(s) > (slen - 3)) {
				printf("ERROR!: %x %d '%s' \n\r", type, slen, s);
				ret = -7;
				goto out_close;
			}
		} else {
			//Not enough memory for file name!
			break;
		}
		//printf("[%d]: %x %d %s: %d \n\r", i + g_MultimediExp_Ctx->ramWindow.start, type, slen, s, g_MultimediExp_Ctx->mediaIndex[i]);
		s += slen - 2;
	}
	//The last one is null device
	printf("Parse %d media\n\r", i);
	if (g_MultimediExp_Ctx->colNum > 1) {
		g_MultimediExp_Ctx->ramWindow.size = (i/g_MultimediExp_Ctx->colNum + 1)*g_MultimediExp_Ctx->colNum;
	} else {
		g_MultimediExp_Ctx->ramWindow.size = i;
	}
	if (g_MultimediExp_Ctx->dispWindow.start < 0) {
		g_MultimediExp_Ctx->dispWindow.start = g_MultimediExp_Ctx->ramWindow.start;
	}
	if ((g_MultimediExp_Ctx->dispWindow.start + pgSz) < (g_MultimediExp_Ctx->ramWindow.size + g_MultimediExp_Ctx->ramWindow.start)) {
		g_MultimediExp_Ctx->dispWindow.size = pgSz;
	} else {
		g_MultimediExp_Ctx->dispWindow.size = g_MultimediExp_Ctx->ramWindow.size + g_MultimediExp_Ctx->ramWindow.start - g_MultimediExp_Ctx->dispWindow.start;
	}

	for (; i <= MEDIABROWSER_MAX_INDEX_BUFFER; i++) {
		g_MultimediExp_Ctx->mediaIndex[i] = 0xFFFF;
	}
	printf("Ram Window: %d %d\n\r", g_MultimediExp_Ctx->ramWindow.start, g_MultimediExp_Ctx->ramWindow.size);

	F_SEEK(dbFile, 0);
	//Update pointer if neeeded!
	if (cmd != MEDIA_PARSEDB_REFRESH) {
		F_WRITE(dbFile, &g_MultimediExp_Ctx->dbData, sizeof(g_MultimediExp_Ctx->dbData), num);
	}
out_close:
	F_CLOSE(dbFile);
	g_MultimediExp_Ctx->UiFlags &= ~MEDIA_GLOBAL_FLAG_PARSING;
	printf("Finish parsing\n\r");

	return ret;
#define uart_puts(a,b)
}

ft_int32_t MediaBrowser_Scan(void)
{
	g_MultimediExp_Ctx->UiFlags &= ~MEDIA_GLOBAL_UPDATED_INFO;
	if ((g_MultimediExp_Ctx->UiFlags & MEDIA_GLOBAL_NOSDCARD) ||
		 g_MultimediExp_Ctx->storageType == MEDIA_STORAGE_TYPE_EFLASH) {
		int pgSz = g_MultimediExp_Ctx->rowNum*g_MultimediExp_Ctx->colNum;

		//Just scan eflash to ram database
		MediaBrowser_ResetDbData(&g_MultimediExp_Ctx->dbData);
		MediaBrowser_ResetMediaItem();
#ifndef FT900_DISABLE_EFLASH_STORAGE
		//Scanning eflash
		BEGIN_PROFILE(eFlashScan)
		MediaBrowser_eFlashScan(DBSTORAGE_TYPE_RAM);
		END_PROFILE(eFlashScan)
#endif //FT900_DISABLE_EFLASH_STORAGE
		g_MultimediExp_Ctx->ramWindow.start = 0;
		g_MultimediExp_Ctx->ramWindow.size = g_MultimediExp_Ctx->dbData.totMediaFile;
		g_MultimediExp_Ctx->dispWindow.start = 0;
		if ((g_MultimediExp_Ctx->dispWindow.start + pgSz) < (g_MultimediExp_Ctx->ramWindow.size + g_MultimediExp_Ctx->ramWindow.start)) {
			g_MultimediExp_Ctx->dispWindow.size = pgSz;
		} else {
			g_MultimediExp_Ctx->dispWindow.size = g_MultimediExp_Ctx->ramWindow.size + g_MultimediExp_Ctx->ramWindow.start - g_MultimediExp_Ctx->dispWindow.start;
		}
	} else {
		g_MultimediExp_Ctx->UiFlags |= MEDIA_GLOBAL_FLAG_SCANNING;
		MediaBrowser_RenderUI();
		MediaBrowser_ResetMediaItem();
		//TODO: we can scan through the file to get list of media files and check if we need to refresh!
		//if (MediaBrowser_ParseDb(MEDIA_PARSEDB_REFRESH) != 0)
		{
			BEGIN_PROFILE(SdCardScan)
#if defined(FT900_PLATFORM)
				MediaBrowser_FolderScan("/");
#elif defined(MSVC_PLATFORM) || defined(BT8XXEMU_PLATFORM) 
				MediaBrowser_FolderScan(MSVC_MEDIA_ROOT_DIR);
#endif
			END_PROFILE(SdCardScan)
		}

		g_MultimediExp_Ctx->UiFlags &= ~MEDIA_GLOBAL_FLAG_SCANNING;
		BEGIN_PROFILE(ParseDB)
		MediaBrowser_ParseDb(MEDIA_PARSEDB_REFRESH);
		END_PROFILE(ParseDB)
	}
#ifdef ENABLE_PROFILING
	printf("<Profiling> Parse %d media\n\r", g_MultimediExp_Ctx->dbData.totMediaFile);
#endif //ENABLE_PROFILING

	//Update scroller
	scroller_init(&g_MultimediExp_Ctx->scrollobj, MEDIABROWSER_ITEM_TAG_BEGIN, g_MultimediExp_Ctx->dispWindow.size,
			//g_MultimediExp_Ctx->dispWindow.start/g_MultimediExp_Ctx->colNum,
			g_MultimediExp_Ctx->dispWindow.start,
			//(g_MultimediExp_Ctx->dbData.totMediaFile+1)/g_MultimediExp_Ctx->colNum,
			(g_MultimediExp_Ctx->dbData.totMediaFile+1),
			g_MultimediExp_Ctx->tileH,
			g_MultimediExp_Ctx->rowNum);
	printf("Done scanning!\n");
	return 0;
}

static void MediaBrowser_ReadKeys(Ft_Gpu_Hal_Context_t *pHalContext)
{
	ft_uint32_t xy;
	ft_uint8_t prevPendown = g_MultimediExp_Ctx->PenDown;

	g_MultimediExp_Ctx->PrevTag = g_MultimediExp_Ctx->CurrTag;
	g_MultimediExp_Ctx->CurrTag = Ft_Gpu_Hal_Rd8(pHalContext, REG_TOUCH_TAG);
	xy = Ft_Gpu_Hal_Rd32(g_MultimediExp_Ctx->pHalContext, REG_TOUCH_SCREEN_XY);
	g_MultimediExp_Ctx->CurrX = (ft_uint16_t)((xy>>16)&0xffff);
	g_MultimediExp_Ctx->CurrY = (ft_uint16_t)(xy&0xffff);
	g_MultimediExp_Ctx->PenDown = (xy&0x80000000)?0:1;
	if (g_MultimediExp_Ctx->PenDown) {
		g_MultimediExp_Ctx->dispUICnt = 300;
		if (prevPendown == 0)
		    g_MultimediExp_Ctx->touchTimeStamp = ft_millis();
	} else if (g_MultimediExp_Ctx->dispUICnt){
		g_MultimediExp_Ctx->dispUICnt--;
	}
}

/*static const char* icon_filename = "@Icons/m48.ftico";*/
void MediaBrowser_UIInit()
{
#define INIT_ICON(icon_name) do{\
	g_MultimediExp_Ctx->iconList[MEDIA_ICON_##icon_name].iconId = FTICON_ID_##icon_name;\
}while(0)
	INIT_ICON(SDCARD);
	INIT_ICON(EFLASH);
	INIT_ICON(AUDIO);
	INIT_ICON(PICTURE);
	INIT_ICON(BACK);
	INIT_ICON(SPEAKER);
	INIT_ICON(PLAY);
	INIT_ICON(PAUSE);
	INIT_ICON(VIDEO);
	INIT_ICON(EXIT);

	/* Dummy display list construction for registering the icons */
	printf(">> Start loading icon...\n\r");
	const char* icon_filename = "@Icons/m48.ftico";
	for (int i = 0; i < MEDIA_ICON_MAX; i++) {
		//Load and render refresh ICON
		int ret;

		if (i == MEDIA_ICON_VIDEO) {
			icon_filename = "@Icons/m36.ftico";
		}
		//NOTE NOTE!!!!: CleO reserve ~ 72KB at top GRAM for pngload, audio playback buffer ....
		//Loading these icon will overlap with those memory but we are safe because the memory for icons are small, ~10KB
		ret = helperLoadIcon(g_MultimediExp_Ctx->pHalContext, &g_MultimediExp_Ctx->gram, icon_filename, &g_MultimediExp_Ctx->iconList[i]);
		if (ret < 0) {
			printf("Fail to load icon: %d %d\n\r", i, g_MultimediExp_Ctx->iconList[i].iconId);
			g_MultimediExp_Ctx->iconList[i].bmId = 0xff;
		} else {
			printf("Icon loading success: %d %d\n\r", i, g_MultimediExp_Ctx->iconList[i].iconId);
			g_MultimediExp_Ctx->iconList[i].bmId = i+1;
		}
	}
	//Special for ICON VIDEO
}


void helperRenderAlignedString(Ft_Gpu_Hal_Context_t * pHalContext, const char* str, enum MediaFont_e font, struct RenderArea_t *pArea, enum Alignment_e align)
{
	int len = strlen(str);
	ft_int8_t longfile = 0;
	if (font >= MEDIA_FONT_MAX)
		font = MEDIA_FONT_MEDIUM;
	struct font_info_t *pFont = &FONT_LIST[font];
	int xdist;
	if (len*pFont->w > pArea->w) {
		int newlen = pArea->w/pFont->w - 4;
		str += (len - newlen);
		len = newlen + 3;
		longfile = 1;
	}
	switch(align) {
	case ALIGN_LEFT:
	default:
		xdist = pArea->x;
		break;
	case ALIGN_RIGHT:
		xdist = pArea->x+pArea->w - pFont->w*len;
		break;
	case ALIGN_CENTER:
	case ALIGN_VIDEO:
		//xdist = pArea->x + (pArea->w - len*pFont->w)/2;
		//FIXME: need to count x here!
		xdist = (FT_DispWidth - len*(pFont->w-1))/2;
		break;
	}
	if (align == ALIGN_VIDEO) {
	    EVE_Cmd_wr32(pHalContext, BLEND_FUNC(ONE, ZERO));
	    //file name
	    helperRenderRect(pHalContext, xdist, pArea->y, xdist + len*pFont->w, pArea->y + 4*pFont->h/3, BLACK_COLOR(), 0);
		EVE_Cmd_wr32(pHalContext, BLEND_FUNC(SRC_ALPHA, ONE_MINUS_SRC_ALPHA));
		EVE_Cmd_wr32(pHalContext, WHITE_COLOR());
	} else {
		EVE_Cmd_wr32(pHalContext, COLOR_RGB(pArea->fgColor>>16, pArea->fgColor>>8, pArea->fgColor));
	}
	//TODO: should support animation!
	if (longfile) {
		Ft_Gpu_CoCmd_Text(pHalContext, xdist, pArea->y, pFont->id, 0, "...");
		xdist += pFont->w<<1;
	}
	Ft_Gpu_CoCmd_Text(pHalContext, xdist, pArea->y, pFont->id, 0, str);

}
//FIXME: Should we return the x,y after render ???
ft_int32_t renderStringSafe(Ft_Gpu_Hal_Context_t * pHalContext, const char* str, enum MediaFont_e font, struct RenderArea_t *pArea)
{
	struct font_info_t *pFont;
	const char* s;
	int len;
	int xdisp, ydisp;
	char buffer[128];
	int sz;

	if (font >= MEDIA_FONT_MAX)
		return -1;
	pFont = &FONT_LIST[font];
	len = strlen(str);
	xdisp = pArea->x;
	ydisp = pArea->y;
	s = str;
	EVE_Cmd_wr32(pHalContext, COLOR_RGB(pArea->fgColor>>16, pArea->fgColor>>8, pArea->fgColor));
	while ((s - str) < len &&
		   (ydisp + pFont->h) < (pArea->y + pArea->h))
	{
		//FIXME: must be in buffer size limit
		sz = pArea->w/pFont->w+1;
		strncpy(buffer, s, sz);
		buffer[sz] = 0;
		Ft_Gpu_CoCmd_Text(pHalContext, xdisp, ydisp, pFont->id, 0, buffer);
		s += sz;
		ydisp += pFont->h;
	}
	return 0;
}

ft_int32_t extractJpegInfo(struct MediaItem_t *pItem, struct MediaInfo_t *pMedInfo, int handle)
{
	//Parse the media to extract some header information
	struct Hbitmap hbitmap;

	hbitmap.width = hbitmap.height = -1;
	hbitmap.fmt = 255;
#if defined(MSVC_PLATFORM) || defined(BT8XXEMU_PLATFORM) 
	g_MultimediExp_Ctx->CleO->raw_draft_image_byhandle(handle, &hbitmap);
#else
	g_MultimediExp_Ctx->CleO->raw_draft_image(pItem->fname, &hbitmap);
#endif
	pMedInfo->w = hbitmap.width;
	pMedInfo->h = hbitmap.height;
	pMedInfo->fmt = hbitmap.fmt;
	pMedInfo->palsize = hbitmap.palsize;
	if (pMedInfo->w <=0 || pMedInfo->h <=0) {
		pMedInfo->flag |= MEDIA_INFO_INVAL_FILE;
	}
	return 0;
}

ft_int32_t extractRiffInfo(struct MediaItem_t *pItem, struct MediaInfo_t *pMedInfo, GuiManager *CleO, int fhandle)
{
	int res;
	ft_int16_t num = 0;
	ft_uint32_t offset;

	pMedInfo->flag |= MEDIA_INFO_INVAL_FILE;
#define SAFE_READ() do{\
	res = CMD_FREAD(CleO, fhandle, (ft_uint8_t*)g_MultimediExp_Ctx->scratchBuffer, sizeof(g_MultimediExp_Ctx->scratchBuffer), num);\
	if (num <= 0) {\
		uart_puts(UART0, "Read error\n\r");\
		goto out_close;\
	}\
}while(0)

	do {
		int i;
		//Try to read as much as possible
		SAFE_READ();
		for (i = 0; i < num; i+=4) {
			if (*(ft_uint32_t*)(&g_MultimediExp_Ctx->scratchBuffer[i]) == FT_AVI_4CC_avih) {
				AVIMAINHEADER *pAviHdr;
				uart_puts(UART0, "Found avih\n\r");
#define uart_puts(a,b)
				if ((num - i) < sizeof(*pAviHdr)) {
					//Need to read data
					CMD_FTELL(CleO, fhandle, offset);
					CleO->cmd_FSeek(fhandle, offset - num + 1);
					SAFE_READ();
					pAviHdr = (AVIMAINHEADER*)&g_MultimediExp_Ctx->scratchBuffer[0];
				} else {
					pAviHdr = (AVIMAINHEADER*)&g_MultimediExp_Ctx->scratchBuffer[i];
				}
				pMedInfo->h = pAviHdr->dwHeight;
				pMedInfo->w = pAviHdr->dwWidth;
				pMedInfo->fps = 1000000/pAviHdr->dwMicroSecPerFrame;
				pMedInfo->dur = pAviHdr->dwTotalFrames*(pAviHdr->dwMicroSecPerFrame/1000)/1000;
				pMedInfo->flag &= ~MEDIA_INFO_INVAL_FILE;
				goto out_close;
			}
#if 0
			if (*(uint32_t*)(&g_MultimediExp_Ctx->scratchBuffer[i]) == FT_AVI_4CC_WAVE) {
				WAVEFORMATEX_T *pWaveHdr;
				uart_puts(UART0, "Found WAVE\n\r");
				if ((num - i) < sizeof(*pWaveHdr)) {
					//Need to read data
					f_lseek(&file, f_tell(&file) - num + i);
					SAFE_READ();
					pWaveHdr = (WAVEFORMATEX_T*)&g_MultimediExp_Ctx->scratchBuffer[4];
				} else {
					pWaveHdr = (WAVEFORMATEX_T*)&g_MultimediExp_Ctx->scratchBuffer[i+4];
				}
				pMedInfo->bps = pWaveHdr->wBitsPerSample;
				pMedInfo->freq = pWaveHdr->nSamplesPerSec;
				pMedInfo->flag &= ~MEDIA_INFO_INVAL_FILE;
				printf("bps: %d freq: %d\n\r", pMedInfo->bps, pMedInfo->freq);
				goto out_close;
			}
#endif
		}
		//finding the avi header
	}while(1);
out_close:
	return 0;
#undef SAFE_READ()
}

ft_int32_t extractWavInfo(struct MediaItem_t *pItem, struct MediaInfo_t *pMedInfo, GuiManager *CleO, int fhandle)
{
	struct {
		int f;
		ft_uint8_t format;
		ft_uint8_t bps;
		ft_uint8_t mode;   // 1 means loop
		ft_uint8_t channels;
		int nsamples;

		int nBlockAlign;
		int adpcm;
		int adpcmPos;
		int adpcmNibble;
		int adpcmIndex, adpcmPred;
	} audio;
	ft_uint8_t hdr[36];
	ft_int16_t bytesread = 0;
	ft_uint32_t offset;
	int pos;
	int subchunk1size;
	int samplerate;

	pMedInfo->flag |= MEDIA_INFO_INVAL_FILE;
	CMD_FREAD(CleO, fhandle, (ft_uint8_t*)&hdr, sizeof(hdr), bytesread);
//	dump_buffer((unsigned char*)hdr, bytesread);
	if (bytesread != sizeof(hdr)
		|| memcmp(&hdr[8], "WAVE", 4)) {
		uart_puts(UART0, "Too small!\n\r");
		goto out_close;
	}
	subchunk1size = *(ft_uint32_t*) (hdr + 16);
	samplerate = *(ft_uint32_t*) (hdr + 24);
	audio.nBlockAlign = hdr[32] + (hdr[33] << 8);
	switch (hdr[20]) {
	case 0x01:
		pMedInfo->aufmt = LINEAR_SAMPLES;
		audio.adpcm = 0;
		break;
	case 0x07:
		pMedInfo->aufmt = ULAW_SAMPLES;
		audio.adpcm = 0;
		break;
	case 0x11:
		pMedInfo->aufmt = LINEAR_SAMPLES;
		audio.adpcm = 1;
		break;
	default:
		goto out_close;
	}
	audio.channels = hdr[22];
	audio.bps = hdr[34];
	CleO->cmd_FSeek(fhandle, sizeof(hdr) + subchunk1size - 16);
	//Finding chunck data
	struct {
		char id[4];
		ft_uint32_t size;
	} chunk;
	while ((CMD_FREAD(CleO, fhandle, (ft_uint8_t*) &chunk, sizeof(chunk), bytesread) == 0)
			&& (bytesread == sizeof(chunk))
			&& (memcmp(chunk.id, "data", 4) != 0))
	{
		CMD_FTELL(CleO, fhandle, offset);
		CleO->cmd_FSeek(fhandle, offset + chunk.size);
	}
	if (memcmp(chunk.id, "data", 4) != 0) {
		//goto out_close;
	}

	if (!audio.adpcm)
		audio.nsamples = (8 * chunk.size) / (audio.channels * audio.bps);
	else {
		int sbp = 1 + 2 * (audio.nBlockAlign - 4);  // samples per block
		int nblocks = chunk.size / audio.nBlockAlign;
		audio.nsamples = audio.channels * ((nblocks * sbp)
						+ 2 * ((chunk.size % audio.nBlockAlign) - 4));
	}
	printf("bps %d nsamples %d %d\n", audio.bps, audio.nsamples, samplerate);
	pMedInfo->bps = audio.bps;
	pMedInfo->freq = samplerate;
	pMedInfo->dur = audio.nsamples / samplerate;
	pMedInfo->flag &= ~MEDIA_INFO_INVAL_FILE;
	pMedInfo->nsamples = audio.nsamples;
out_close:
	return 0;
}

static void initMediaInfo(struct MediaItem_t *pMediaItem, int selIdx)
{
	int index;
	char *sbuf;

	//Get tag
	pMediaItem->col = selIdx%g_MultimediExp_Ctx->colNum;
	pMediaItem->row = selIdx/g_MultimediExp_Ctx->colNum;
	pMediaItem->flags = 0;
	index = selIdx + g_MultimediExp_Ctx->dispWindow.start - g_MultimediExp_Ctx->ramWindow.start;
	sbuf = &g_MultimediExp_Ctx->mediaNameBuffer[g_MultimediExp_Ctx->mediaIndex[index]];
	pMediaItem->type = sbuf[0];
	pMediaItem->fname = &sbuf[2];
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Helper functions
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

void helperRenderLine(Ft_Gpu_Hal_Context_t *pHalContext, int x1, int y1, int x2, int y2, int w)
{
	EVE_Cmd_wr32(pHalContext, BEGIN(LINES));
	EVE_Cmd_wr32(pHalContext, LINE_WIDTH(w));
	EVE_Cmd_wr32(pHalContext, VERTEX2F(x1,y1));
	EVE_Cmd_wr32(pHalContext, VERTEX2F(x2,y2));
}

void helperRenderRect(Ft_Gpu_Hal_Context_t *pHalContext, int x1, int y1, int x2, int y2, int color, ft_uint8_t tag)
{
	if (tag) EVE_Cmd_wr32(pHalContext, TAG(tag));
	if (color != NO_COLOR()) EVE_Cmd_wr32(pHalContext, color);
	EVE_Cmd_wr32(pHalContext, BEGIN(RECTS));
	EVE_Cmd_wr32(pHalContext, VERTEX2F(x1<<4,y1<<4));
	EVE_Cmd_wr32(pHalContext, VERTEX2F(x2<<4,y2<<4));
}

void helperRenderInvisibleRect(Ft_Gpu_Hal_Context_t *pHalContext, int x1, int y1, int x2, int y2, ft_uint8_t tag)
{
	//EVE_Cmd_wr32(pHalContext, SAVE_CONTEXT());
	EVE_Cmd_wr32(pHalContext, TAG(tag));
	EVE_Cmd_wr32(pHalContext, COLOR_MASK(0, 0, 0, 0));
	helperRenderRect(pHalContext, x1, y1, x2, y2, NO_COLOR(), tag);
	EVE_Cmd_wr32(pHalContext, COLOR_MASK(1, 1, 1, 1));
	//EVE_Cmd_wr32(pHalContext, RESTORE_CONTEXT());
}

void helperRenderInvisibleLine(Ft_Gpu_Hal_Context_t *pHalContext, int x1, int y1, int x2, int y2, int w, ft_uint8_t tag)
{
	EVE_Cmd_wr32(pHalContext, SAVE_CONTEXT());
	EVE_Cmd_wr32(pHalContext, TAG(tag));
	EVE_Cmd_wr32(pHalContext, COLOR_MASK(0, 0, 0, 0));
	helperRenderLine(pHalContext, x1, y1, x2, y2, w);
	EVE_Cmd_wr32(pHalContext, COLOR_MASK(1, 1, 1, 1));
	EVE_Cmd_wr32(pHalContext, RESTORE_CONTEXT());
}


void helperBeginRender(Ft_Gpu_Hal_Context_t *pHalContext)
{
	Gpu_CoCmd_Dlstart(pHalContext);
	EVE_Cmd_wr32(pHalContext, CLEAR_TAG(1));
	EVE_Cmd_wr32(pHalContext, CLEAR_COLOR_RGB(0, 0, 0));
	EVE_Cmd_wr32(pHalContext, CLEAR(1, 1, 1));
}

void helperEndRender(Ft_Gpu_Hal_Context_t *pHalContext)
{
	EVE_Cmd_wr32(pHalContext,TAG_MASK(0));
	EVE_Cmd_wr32(pHalContext,DISPLAY());
	EVE_Cmd_waitFlush(pHalContext);
	 /*Swap the frame directly*/
	Ft_Gpu_Hal_Wr8(pHalContext,REG_DLSWAP,DLSWAP_FRAME);
	Gpu_Hal_WaitCmdfifo_empty(pHalContext);
}

void helperRenderHeaderPanel(Ft_Gpu_Hal_Context_t *pHalContext, int xdist, int ydist,
							 struct bm_info_t *pBackIcon, struct bm_info_t *pExitIcon,
	                         ft_uint8_t curTag, ft_uint8_t prevTag)
{
	helperRenderRect(pHalContext, 0, 0, FT_DispWidth, MAINUI_ELEMENT_HEIGHT, LIGHT_GREY_COLOR(), 255);
	EVE_Cmd_wr32(pHalContext, WHITE_COLOR());
	//Render header line!
	helperRenderLine(pHalContext, 0, MAINUI_ELEMENT_HEIGHT*16, FT_DispWidth*16, MAINUI_ELEMENT_HEIGHT*16, 1*8);
	helperRenderBitmapIcon(pHalContext, pBackIcon,
						   FT_DispWidth/2 - xdist/2, ydist, xdist, MAINUI_ELEMENT_HEIGHT, MEDIAPLAYER_KEY_BACK,
						   curTag == MEDIAPLAYER_KEY_BACK || prevTag == MEDIAPLAYER_KEY_BACK);
	helperRenderBitmapIcon(pHalContext, pExitIcon,
						   FT_DispWidth/2 + xdist/2, ydist, xdist, MAINUI_ELEMENT_HEIGHT, MEDIAPLAYER_KEY_EXIT,
						   curTag == MEDIAPLAYER_KEY_EXIT || prevTag == MEDIAPLAYER_KEY_EXIT);
}

void helperRenderBitmapIcon(Ft_Gpu_Hal_Context_t *pHalContext, struct bm_info_t *pBmInfo, int x, int y, int w, int h, ft_uint8_t tag, int isSel)
{
	//Render wrapper rect
	if (tag)
		EVE_Cmd_wr32(pHalContext, TAG(tag));
	/*else
		EVE_Cmd_wr32(pHalContext, TAG(255));*/
	if (isSel) {
		helperRenderRect(pHalContext, x - w/2, y,
				         x + w/2, y + h,
						 DARK_GREY_COLOR(), tag);
	}

	//Render bitmap
	EVE_Cmd_wr32(pHalContext, WHITE_COLOR());
	EVE_Cmd_wr32(pHalContext, BITMAP_HANDLE(pBmInfo->bmId));
	Ft_Gpu_CoCmd_SetBitmap(pHalContext, pBmInfo->gram_src, pBmInfo->fmt, pBmInfo->w, pBmInfo->h);
	EVE_Cmd_wr32(pHalContext,BEGIN(BITMAPS));
	EVE_Cmd_wr32(pHalContext, BITMAP_HANDLE(pBmInfo->bmId));
	x -= pBmInfo->w/2;
	//NOTE: must use VERTEX2F because CleO50 has big resolution 800x480 (>512)
	EVE_Cmd_wr32(pHalContext,VERTEX2F(x*16,y*16));
	/*EVE_Cmd_wr32(pHalContext, TAG(255));*/
    //printf("bitmapIcon: [%d %d %dx%d] wxh=%dx%d\n\r", x, y, w, h, pBmInfo->w, pBmInfo->h);
}

/*
 * Note: this function support vertical volume bar only
 * xdist, ydist, ydist2: are frac_8
 * ydist (high) -> ydist2 (low)
 * TODO: should we support render icon ??
 */
void helperRenderVolumeBar(Ft_Gpu_Hal_Context_t *pHalContext, int xdist, int ydist, int ydist2, ft_uint8_t tag, int vol_level, int range)
{
	int y2;

	y2 = ydist - vol_level * (ydist - ydist2)/range;
	EVE_Cmd_wr32(pHalContext, TAG(tag));
	//Invisible status bar for seeking
	/*helperRenderInvisibleLine(pHalContext, xdist * 8, ydist * 8,
			                         xdist * 8, ydist2 * 8, 10 * 16, tag);*/
	//Display volume line
	helperRenderLine(pHalContext, xdist * 8, ydist * 8,
			                xdist * 8, y2 * 8, 2 * 16);
	helperRenderLine(pHalContext, xdist * 8, y2 * 8,
				            xdist * 8, ydist2 * 8, 1 * 16);
	//Selected point
	EVE_Cmd_wr32(pHalContext, COLOR_A(255));
	EVE_Cmd_wr32(pHalContext, BEGIN(FTPOINTS));
	EVE_Cmd_wr32(pHalContext, POINT_SIZE(12 * 16));
	EVE_Cmd_wr32(pHalContext, VERTEX2F(xdist * 8, y2 * 8));
	EVE_Cmd_wr32(pHalContext, COLOR_A(164));
	EVE_Cmd_wr32(pHalContext, TAG(255));

	helperRenderInvisibleRect(pHalContext, (xdist-10)/2, ydist/2,
                             (xdist+30)/2, ydist2/2, tag);
}

void helperRenderProgressBar(Ft_Gpu_Hal_Context_t *pHalContext, int xdist, int ydist, int xdist2, int curTimeSecs, int endTimeSecs, ft_uint8_t tag, float ratio)
{
	char buffer[16];
	int x2 = 0;
	int progbar_start = xdist/2;
	int progbar_end = xdist2;

	EVE_Cmd_wr32(pHalContext, WHITE_COLOR());
	//Render current played time
	sprintf(buffer, "%02d:%02d", SECOND2MIN(curTimeSecs), SECOND2REMAIN(curTimeSecs));
	Ft_Gpu_CoCmd_Text(pHalContext, xdist/2 - 8*3, ydist - 15 - 10, 28, 0, buffer);

	//Render full time
	sprintf(buffer, "%02d:%02d", SECOND2MIN(endTimeSecs), SECOND2REMAIN(endTimeSecs));
	Ft_Gpu_CoCmd_Text(pHalContext, xdist2 - 8*3, ydist - 15 - 10 , 28, 0, buffer);

	//Render progress bar
	if (ratio > 1.0)
		ratio = 1.0;
	if (ratio < 0.0)
		ratio = 0.0;
	x2 = (int)(progbar_start + (float)(progbar_end - progbar_start)*ratio);
	if (x2 < progbar_start) x2 = progbar_start + 1;

	//Draw line
	EVE_Cmd_wr32(pHalContext, TAG(tag));
	EVE_Cmd_wr32(pHalContext, BEGIN(LINES));
	EVE_Cmd_wr32(pHalContext, LINE_WIDTH(3 * 16));
	EVE_Cmd_wr32(pHalContext, VERTEX2F((progbar_start)*16, (ydist)*16));
	EVE_Cmd_wr32(pHalContext, VERTEX2F((x2)*16, (ydist)*16));
	EVE_Cmd_wr32(pHalContext, LINE_WIDTH(1 * 16));
	EVE_Cmd_wr32(pHalContext, VERTEX2F((x2)*16, (ydist)*16));
	EVE_Cmd_wr32(pHalContext, VERTEX2F((progbar_end)*16, (ydist)*16));

	//Invisible status bar for seeking
	helperRenderInvisibleLine(pHalContext, progbar_start*16, (ydist-14)*16, progbar_end*16, (ydist-10)*16, 16*16, tag);

	////Current progress
	EVE_Cmd_wr32(pHalContext,COLOR_A(255));
	EVE_Cmd_wr32(pHalContext,BEGIN(FTPOINTS));
	EVE_Cmd_wr32(pHalContext,POINT_SIZE(10*16));
	//TODO: sample value, should render correct value here!
	EVE_Cmd_wr32(pHalContext,VERTEX2F((x2)*16,(ydist)*16));
}

static void renderMainUI()
{
#define WIDGET_ICON_W        60
#define WIDGET_RESOLUTION_W  10
#define WIDGET_DURATION_W    8
	Ft_Gpu_Hal_Context_t *pHalContext = g_MultimediExp_Ctx->pHalContext;
    int xdist, ydist;
    int i;
    //int pageSz = g_MultimediExp_Ctx->colNum*g_MultimediExp_Ctx->rowNum;
    char buffer[64];
    int col, row;
    char *sbuf;
    int index;
    //Example: Movie.avi
    //         1024x768                                        1:10:20
    int maxlen = g_MultimediExp_Ctx->tileW/FONT_LIST[MEDIA_FONT_MEDIUM].w - 4; //maximum length of string in one row

	{
		//Scrolling processing:
		index = g_MultimediExp_Ctx->dispWindow.start - g_MultimediExp_Ctx->ramWindow.start;
		xdist = FT_DispWidth/4;
		ydist = 0;
		if (FT_DispWidth >= 600) {
			xdist = 120;
		} else {
			xdist = 80;
		}
		if (g_MultimediExp_Ctx->UiFlags & (MEDIA_GLOBAL_FLAG_SCANNING | MEDIA_GLOBAL_FLAG_PARSING)) {
			EVE_Cmd_wr32(pHalContext, WHITE_COLOR());
			Ft_Gpu_CoCmd_Text(pHalContext, FT_DispWidth/2 - FONT_LIST[MEDIA_FONT_HUGE].w*12/2,
					                 FT_DispHeight/2, FONT_LIST[MEDIA_FONT_HUGE].id, 0,
							         "SCANNING ...");
		} else if (g_MultimediExp_Ctx->storageType != MEDIA_STORAGE_TYPE_INV &&
				   g_MultimediExp_Ctx->dbData.totMediaFile > 0 &&
				   g_MultimediExp_Ctx->dispWindow.start >= 0 &&
				   g_MultimediExp_Ctx->dispWindow.size > 0)
		{
			int size, yoffset;
			if (scroller_frac(&g_MultimediExp_Ctx->scrollobj, g_MultimediExp_Ctx->tileH) > 0 &&
					g_MultimediExp_Ctx->dispWindow.start > 0) {
				//scroll down
				index -= g_MultimediExp_Ctx->colNum;
				size = 	g_MultimediExp_Ctx->dispWindow.size + g_MultimediExp_Ctx->colNum;
				yoffset = MAINUI_ELEMENT_HEIGHT - g_MultimediExp_Ctx->tileH;
			} else {
				size = g_MultimediExp_Ctx->dispWindow.size;
				yoffset = MAINUI_ELEMENT_HEIGHT;
			}
			for (i = 0; i < size; i++, index++) {
				row = i / g_MultimediExp_Ctx->colNum;
				col = i % g_MultimediExp_Ctx->colNum;
				if (g_MultimediExp_Ctx->mediaIndex[index] == 0xFFFF)
					continue;
				sbuf = &g_MultimediExp_Ctx->mediaNameBuffer[g_MultimediExp_Ctx->mediaIndex[index]];
				if ((ft_uint8_t)*sbuf == MEDIA_TYPE_INVALID)
					continue;
				xdist = col * g_MultimediExp_Ctx->tileW;
				ydist = row * g_MultimediExp_Ctx->tileH + scroller_frac(&g_MultimediExp_Ctx->scrollobj, g_MultimediExp_Ctx->tileH);//for easier touch first item
				//ydist += MAINUI_ELEMENT_HEIGHT;
				ydist += yoffset;
				//if (ydist < MAINUI_ELEMENT_HEIGHT) continue;

				//Adding tag
				if (ydist < MAINUI_ELEMENT_HEIGHT)
					EVE_Cmd_wr32(pHalContext, TAG(255));
				else
					EVE_Cmd_wr32(pHalContext, TAG(i+MEDIABROWSER_ITEM_TAG_BEGIN));
				//Display background color
				if (g_MultimediExp_Ctx->PenDown
					/*&& (i + MEDIABROWSER_ITEM_TAG_BEGIN) == g_MultimediExp_Ctx->CurrTag
				      && !scroller_isScrolling(&g_MultimediExp_Ctx->scrollobj)*/
					&& g_MultimediExp_Ctx->CurrY >= (ydist+3) && g_MultimediExp_Ctx->CurrY < (ydist + MAINUI_ELEMENT_HEIGHT - 3)
					)
				{
					//Selected line
					EVE_Cmd_wr32(pHalContext, LIGHT_GREY_COLOR());
				} else {
					EVE_Cmd_wr32(pHalContext, BLACK_COLOR());
				}
				helperRenderRect(pHalContext, xdist, ydist, xdist+g_MultimediExp_Ctx->tileW, ydist + g_MultimediExp_Ctx->tileH, NO_COLOR(), 0);
				EVE_Cmd_wr32(pHalContext, WHITE_COLOR());
                //Render separate line
				helperRenderLine(pHalContext, xdist*16, ydist*16, FT_DispWidth*16, ydist*16, 1*8);

				//Render bitmap
				switch((ft_uint8_t)*sbuf) {
				case MEDIA_TYPE_AUDIO:
					helperRenderBitmapIcon(pHalContext, &g_MultimediExp_Ctx->iconList[MEDIA_ICON_AUDIO], 30, ydist, 60, MAINUI_ELEMENT_HEIGHT, 0, 0);
					break;
				case MEDIA_TYPE_PIC:
					helperRenderBitmapIcon(pHalContext, &g_MultimediExp_Ctx->iconList[MEDIA_ICON_PICTURE], 30, ydist, 60, MAINUI_ELEMENT_HEIGHT, 0, 0);
					break;
				case MEDIA_TYPE_VIDEO:
				default:
					helperRenderBitmapIcon(pHalContext, &g_MultimediExp_Ctx->iconList[MEDIA_ICON_VIDEO], 30, ydist, 60, MAINUI_ELEMENT_HEIGHT, 0, 0);
					break;
				}
				xdist += 60;

				//Text has white color
				EVE_Cmd_wr32(pHalContext, WHITE_COLOR());
				//Render file name
				{
					char *s = sbuf+2;
					int len = sbuf[1] - 3;
					struct MediaInfo_t *info = &g_MultimediExp_Ctx->compactInfoList[i];

					if (*s == '@') {
						s++;
					}
					if (len > maxlen) {
						//just render a short string
						s += len - maxlen;
						sprintf(buffer,"...%s",s);
						Ft_Gpu_CoCmd_Text(pHalContext, xdist, ydist, FONT_LIST[MEDIA_FONT_MEDIUM].id, 0, buffer);
					} else {
						//just render the string
						Ft_Gpu_CoCmd_Text(pHalContext, xdist, ydist, FONT_LIST[MEDIA_FONT_MEDIUM].id, 0, s);
					}
					ydist += FONT_LIST[MEDIA_FONT_MEDIUM].h + 5;
					//Display resolution or codec
					if ((ft_uint8_t)*sbuf == MEDIA_TYPE_AUDIO) {
//						sprintf(buffer, "%d Hz, %d bit", info->freq, info->bps);
						sprintf(buffer, "%s", "Unknown Album");
					} else {
						sprintf(buffer,"%dx%d", info->w, info->h);
					}
					Ft_Gpu_CoCmd_Text(pHalContext, xdist, ydist, FONT_LIST[MEDIA_FONT_TINY].id, 0, buffer);
					//Display duration
					if ((ft_uint8_t)*sbuf != MEDIA_TYPE_PIC) {
						sprintf(buffer,"%02d:%02d:%02d",
										SECOND2HOUR(info->dur),
										SECOND2MIN(info->dur),
										SECOND2REMAIN(info->dur));
						Ft_Gpu_CoCmd_Text(pHalContext, g_MultimediExp_Ctx->tileW - FONT_LIST[MEDIA_FONT_TINY].w*strlen(buffer),
								          ydist, FONT_LIST[MEDIA_FONT_TINY].id, 0, buffer);
					}
				}
			}
		} else {
			EVE_Cmd_wr32(pHalContext, WHITE_COLOR());
			Ft_Gpu_CoCmd_Text(pHalContext, FT_DispWidth/2 - FONT_LIST[MEDIA_FONT_HUGE].w*8/2,
									 FT_DispHeight/2, FONT_LIST[MEDIA_FONT_HUGE].id, 0,
									 "NO MEDIA");
		}
		ydist = 0;
		if (FT_DispWidth >= 600) {
			xdist = 120;
		} else {
			xdist = 80;
		}
		EVE_Cmd_wr32(pHalContext, TAG(255));
		EVE_Cmd_wr32(pHalContext, COLOR_A(255));
		EVE_Cmd_wr32(pHalContext, BLEND_FUNC(ONE, ZERO));
		//Display header
		helperRenderRect(pHalContext, 0, 0, FT_DispWidth, MAINUI_ELEMENT_HEIGHT, LIGHT_GREY_COLOR(), 0);
		EVE_Cmd_wr32(pHalContext, BLEND_FUNC(SRC_ALPHA, ONE_MINUS_SRC_ALPHA));
		//Render SDCard!
		int x2 = FT_DispWidth/2 - xdist;
#if !defined(MSVC_PLATFORM) && !defined(BT8XXEMU_PLATFORM) 
		if (!(g_MultimediExp_Ctx->UiFlags & MEDIA_GLOBAL_NOSDCARD)) {
			helperRenderBitmapIcon(pHalContext, &g_MultimediExp_Ctx->iconList[MEDIA_ICON_SDCARD],
					               x2, ydist, xdist, MAINUI_ELEMENT_HEIGHT, MEDIAPLAYER_KEY_SDCARD,
					               g_MultimediExp_Ctx->storageType == MEDIA_STORAGE_TYPE_SDCARD);
			x2 += xdist;
		}
#endif // defined(MSVC_PLATFORM) || defined(BT8XXEMU_PLATFORM) 
#ifndef FT900_DISABLE_EFLASH_STORAGE
		helperRenderBitmapIcon(pHalContext, &g_MultimediExp_Ctx->iconList[MEDIA_ICON_EFLASH],
							   x2, ydist, xdist, MAINUI_ELEMENT_HEIGHT, MEDIAPLAYER_KEY_EFLASH,
							   g_MultimediExp_Ctx->storageType == MEDIA_STORAGE_TYPE_EFLASH);
		x2 += xdist;
#endif

		helperRenderBitmapIcon(pHalContext, &g_MultimediExp_Ctx->iconList[MEDIA_ICON_EXIT],
                               FT_DispWidth/2 + xdist, ydist, xdist, MAINUI_ELEMENT_HEIGHT, MEDIAPLAYER_KEY_EXIT,
				               g_MultimediExp_Ctx->CurrTag == MEDIAPLAYER_KEY_EXIT);
		EVE_Cmd_wr32(pHalContext,COLOR_A(164));
	}
	EVE_Cmd_wr32(pHalContext, TAG(255));
	if (g_MultimediExp_Ctx->dispUICnt > 0) {
		//Render scroller bar
		Ft_Gpu_CoCmd_FgColor(pHalContext, LIGHT_GREY_COLOR());
		Ft_Gpu_CoCmd_BgColor(pHalContext, BLACK_COLOR());
		Ft_Gpu_CoCmd_Scrollbar(pHalContext,
							   FT_DispWidth - 10,
							   MAINUI_ELEMENT_HEIGHT+8,
							   10, FT_DispHeight - MAINUI_ELEMENT_HEIGHT - 15,
							   0,
							   g_MultimediExp_Ctx->dispWindow.start,
							   g_MultimediExp_Ctx->rowNum*g_MultimediExp_Ctx->colNum,
							   g_MultimediExp_Ctx->dbData.totMediaFile);
	}
	if (g_MultimediExp_Ctx->UiFlags & (MEDIA_GLOBAL_FLAG_SCANNING | MEDIA_GLOBAL_FLAG_PARSING)) {
		//Display spinning
		EVE_Cmd_wr32(pHalContext,COLOR_RGB(0, 0, 128));
		Ft_Gpu_CoCmd_Spinner(pHalContext, FT_DispWidth/2, FT_DispHeight/2,0,0);
	}
}

void MediaBrowser_RenderUI()
{
	Ft_Gpu_Hal_Context_t *pHalContext = g_MultimediExp_Ctx->pHalContext;
	//Display a scrollbar
    //int r, c;
#if !defined(MSVC_PLATFORM) && !defined(BT8XXEMU_PLATFORM) 
	if(Ft_Gpu_Hal_Rd16(pHalContext, REG_CMD_READ) == Ft_Gpu_Hal_Rd16(pHalContext, REG_CMD_WRITE))
#endif
	{
		if (g_MultimediExp_Ctx->state == MEDIA_STATEMACHINE_MAIN &&
			!(g_MultimediExp_Ctx->UiFlags & MEDIA_GLOBAL_FLAG_PARSING))
		{
			int nextIdx;

			//nextIdx = trunc(&g_MultimediExp_Ctx->scrollobj, 0)*g_MultimediExp_Ctx->colNum;
			nextIdx = scroller_trunc(&g_MultimediExp_Ctx->scrollobj, 0);
			//need to update the ram window ?
			if (nextIdx < 0 || (nextIdx + g_MultimediExp_Ctx->dispWindow.size) > (g_MultimediExp_Ctx->dbData.totMediaFile+1)) {
				//Do not allow scroll more
				g_MultimediExp_Ctx->UiFlags &= ~(MEDIA_GLOBAL_AUTO_SCROLLING|MEDIA_GLOBAL_MANUAL_SCROLLING);
			} else {
				//check if we need to update internal database
				if (nextIdx < g_MultimediExp_Ctx->ramWindow.start)
				{
					//Call update
					MediaBrowser_ParseDb(MEDIA_PARSEDB_BWD);
				}
				else if ((nextIdx + g_MultimediExp_Ctx->dispWindow.size) > (g_MultimediExp_Ctx->ramWindow.size + g_MultimediExp_Ctx->ramWindow.start))
				{
					if ((g_MultimediExp_Ctx->ramWindow.size + g_MultimediExp_Ctx->ramWindow.start) >= g_MultimediExp_Ctx->dbData.totMediaFile) {
						g_MultimediExp_Ctx->dispWindow.start = nextIdx;
						g_MultimediExp_Ctx->dispWindow.size = g_MultimediExp_Ctx->dbData.totMediaFile - nextIdx;
					} else {
#if defined(MSVC_PLATFORM) || defined(BT8XXEMU_PLATFORM) 
						if ((nextIdx + g_MultimediExp_Ctx->dispWindow.size) < g_MultimediExp_Ctx->dbData.totMediaFile)
							MediaBrowser_ParseDb(MEDIA_PARSEDB_FWD);
#else
						MediaBrowser_ParseDb(MEDIA_PARSEDB_FWD);
#endif
					}
				} else {
					//Still in ram size
					g_MultimediExp_Ctx->dispWindow.start = nextIdx;
					g_MultimediExp_Ctx->dispWindow.size = g_MultimediExp_Ctx->rowNum*g_MultimediExp_Ctx->colNum;
				}
			}

/*
			printf("Scroll: dispWindow [%d %d] [%d %d] tot=%d %d\n\r", g_MultimediExp_Ctx->dispWindow.start,
					g_MultimediExp_Ctx->dispWindow.size,
					g_MultimediExp_Ctx->ramWindow.start,
					g_MultimediExp_Ctx->ramWindow.size,
					g_MultimediExp_Ctx->dbData.totMediaFile,
					g_MultimediExp_Ctx->storageType);
*/

		}

		helperBeginRender(pHalContext);

		/*render_scrollbar(pHalContext, &g_MultimediExp_Ctx->sb);*/
		EVE_Cmd_wr32(pHalContext,COLOR_A(164));
		/*xoff = g_MultimediExp_Ctx->tileW*8;
		yoff = g_MultimediExp_Ctx->tileH*8;*/
		EVE_Cmd_wr32(pHalContext,TAG_MASK(1));
		renderMainUI();
		/*switch(g_MultimediExp_Ctx->state) {
		case MEDIA_STATEMACHINE_MAIN:
			renderMainUI();
			break;
		default:
			break;
		}*/
		helperEndRender(pHalContext);
	}
}

//const char* __flash__ multimedia_icon_filename = "@Icons/m48.ftico";
extern int text_width(int font, const char *s);
/*const char* g_multimedia_icon_filename = "@Icons/m48.ftico";*/

ft_int32_t MultimediaExplorer_AppPlayAudio(Ft_Gpu_Hal_Context_t *pHalContext, GuiManager *CleO, const char* filename)
{
#define MEDIA_AUDIO_UIFLAGS_LOOP 	0x0001
#define MEDIA_AUDIO_UIFLAGS_PLAYING 0x0002

#define MEDIA_AUDIO_FONT FONT_SANS_2
	volatile ft_uint16_t px, py;
	int progbar_start, progbar_end;
	ft_int16_t textStart = 10;
	int animateSpeed = 0;
	int slen = strlen(filename)*FONT_LIST[MEDIA_FONT_MEDIUM].w;
	int xdist, ydist;
	//char buffer[16];
	struct RenderArea_t area;
	int ret = MEDIAPLAYER_KEY_EXIT;
	int playing = 0;
	int cursample = 0;
	int nsamples = 1;
	ft_uint8_t vol_level = FT81x_DEFAULT_VOLUME;

	/* Get handles to icons */
	//Need to reset the audio and set the volume level
	area.fgColor = WHITE_COLOR();
	if (g_MultimediExp_Ctx->UiFlags & MEDIA_GLOBAL_AUDIO_CONT_PLAYING) {
		//Cont. play the audio file, restore the context
		cursample = CleO->audio_played_samples();
		playing = 1;
	} else {
		CleO->init_audio();
		//Ft_Gpu_Hal_Wr8(pHalContext, REG_VOL_PB, vol_level);
		//CleO->cmd_AudioPlay(filename, PLAY_ONCE);
	}
	vol_level = Ft_Gpu_Hal_Rd8(pHalContext, REG_VOL_PB);
	nsamples = g_MultimediExp_Ctx->mediaInfo.nsamples;
	while (1) {
#if 0
		CleO->cmd_Start();
		if (--animateSpeed <= 0) {
			animateSpeed = 1;
			//Display string
			if ((textStart + slen) < 0) {
				textStart = FT_DispWidth;
			}
			/*if (textStart > 0)*/
			{
				textStart--;
			}
		}
		//fix ydisp
		ydisp = 100;
		xdisp = 10;
		CleO->cmd_StringHandle(FONT_SANS_3);
		CleO->cmd_StringColor(LIGHTGREEN);
		CleO->cmd_String(FIX(textStart), FIX(ydisp), s);
#endif

		///////////////////////////////////Main Rendering for Music Player/////////////////
		//Start Rendering
		helperBeginRender(pHalContext);

		/*render_scrollbar(pHalContext, &g_MultimediExp_Ctx->sb);*/
		EVE_Cmd_wr32(pHalContext,COLOR_A(164));
		/*xoff = g_MultimediExp_Ctx->tileW*8;
		yoff = g_MultimediExp_Ctx->tileH*8;*/
		EVE_Cmd_wr32(pHalContext,TAG_MASK(1));

		ydist = 0;
		if (FT_DispWidth >= 600) {
			xdist = 120;
		} else {
			xdist = 80;
		}
		//////Display header
		helperRenderHeaderPanel(pHalContext, xdist, ydist,
                                &g_MultimediExp_Ctx->iconList[MEDIA_ICON_BACK],
                                &g_MultimediExp_Ctx->iconList[MEDIA_ICON_EXIT],
                                g_MultimediExp_Ctx->CurrTag, 0);
		//////End Header

		if (g_MultimediExp_Ctx->mediaInfo.flag & MEDIA_INFO_INVAL_FILE) {
		    area.w = FT_DispWidth - xdist;
		    area.h = MAINUI_ELEMENT_HEIGHT*2;
		    area.x = xdist;
		    area.y = FT_DispHeight/4;
			helperRenderAlignedString(pHalContext, "UNSUPPORT FILE FORMAT", MEDIA_FONT_HUGE, &area, ALIGN_CENTER);
		} else {
			//////Display volume control
			ydist = FT_DispHeight - 5 * MAINUI_ELEMENT_HEIGHT / 2;
			/*if (vol_level < 2) {
				helperRenderBitmapIcon(pHalContext,
						&g_MultimediExp_Ctx->iconList[MEDIA_ICON_VOL_MUTE],
						xdist / 2, ydist, xdist, MAINUI_ELEMENT_HEIGHT,
						MEDIAPLAYER_KEY_VOL_ZOOM_CONTROL, 0);
			} else */{
				helperRenderBitmapIcon(pHalContext,
						&g_MultimediExp_Ctx->iconList[MEDIA_ICON_SPEAKER],
						xdist / 2, ydist, xdist, MAINUI_ELEMENT_HEIGHT,
						255, 0);
			}
			ydist -= 10;
			if (g_MultimediExp_Ctx->PenDown
					&& g_MultimediExp_Ctx->CurrTag == MEDIAPLAYER_KEY_VOL_ZOOM_CONTROL
					&& (g_MultimediExp_Ctx->CurrY >= 3 * MAINUI_ELEMENT_HEIGHT / 2)
					&& (g_MultimediExp_Ctx->CurrY <= ydist)) {
				vol_level = (ydist - g_MultimediExp_Ctx->CurrY) * FT81x_MAX_VOLUME_LEVEL
						    / (ydist - 3 * MAINUI_ELEMENT_HEIGHT / 2);
				//Setting new level
				Ft_Gpu_Hal_Wr8(pHalContext, REG_VOL_PB, vol_level);
			}
			helperRenderVolumeBar(pHalContext, xdist, ydist<<1, 3*MAINUI_ELEMENT_HEIGHT, MEDIAPLAYER_KEY_VOL_ZOOM_CONTROL, vol_level, FT81x_MAX_VOLUME_LEVEL);
			//////Render Progress bar
			//Current value
			if (FT_DispWidth >= 600) {
				xdist *= 2;
			}
			{
				ydist = FT_DispHeight - MAINUI_ELEMENT_HEIGHT - 5;
				if (playing) {
					cursample = CleO->audio_played_samples();
				}
				progbar_start = xdist / 2;
				progbar_end = FT_DispWidth - xdist / 2;
				helperRenderProgressBar(pHalContext, xdist, ydist, FT_DispWidth - xdist/2,
						cursample/g_MultimediExp_Ctx->mediaInfo.freq,
						g_MultimediExp_Ctx->mediaInfo.dur,
						MEDIAPLAYER_KEY_AUD_PROGRESS1,
						(float)cursample/(float)nsamples);
			}
			if (FT_DispWidth >= 600) {
				xdist /= 2;
			}
			//////End of progress bar

			if (playing) {
				helperRenderBitmapIcon(pHalContext,
						&g_MultimediExp_Ctx->iconList[MEDIA_ICON_PAUSE],
						FT_DispWidth / 2, FT_DispHeight - MAINUI_ELEMENT_HEIGHT,
						xdist, MAINUI_ELEMENT_HEIGHT, MEDIAPLAYER_KEY_AUD_PLAY,
						0);
			} else {
				helperRenderBitmapIcon(pHalContext,
						&g_MultimediExp_Ctx->iconList[MEDIA_ICON_PLAY],
						FT_DispWidth / 2, FT_DispHeight - MAINUI_ELEMENT_HEIGHT,
						xdist, MAINUI_ELEMENT_HEIGHT, MEDIAPLAYER_KEY_AUD_PLAY,
						0);
			}
		}
		//////Render file name
		EVE_Cmd_wr32(pHalContext, TAG(255));
		ydist = FT_DispHeight / 2 - MAINUI_ELEMENT_HEIGHT;
		EVE_Cmd_wr32(pHalContext, WHITE_COLOR());
		area.w = FT_DispWidth - xdist;
		area.h = MAINUI_ELEMENT_HEIGHT;
		area.x = xdist;
		area.y = ydist;
		area.fgColor = WHITE_COLOR();
		if (FT_DispWidth >= 600) {
			helperRenderAlignedString(pHalContext, filename, MEDIA_FONT_HUGE, &area,
					ALIGN_CENTER);
		} else {
			helperRenderAlignedString(pHalContext, filename, MEDIA_FONT_MEDIUM, &area,
					ALIGN_CENTER);
		}
		if (!(g_MultimediExp_Ctx->mediaInfo.flag & MEDIA_INFO_INVAL_FILE)) {
			area.y += MAINUI_ELEMENT_HEIGHT;
			if (FT_DispWidth >= 600) {
				helperRenderAlignedString(pHalContext, "Unknown Album", MEDIA_FONT_MEDIUM,
						&area, ALIGN_CENTER);
			} else {
				helperRenderAlignedString(pHalContext, "Unknown Album", MEDIA_FONT_TINY,
						&area, ALIGN_CENTER);
			}
		}
		//////End of Render file name
		helperEndRender(pHalContext);
		///////////////////////////////////DONE Rendering for Music Player/////////////////

		//Play audio in background
		CleO->service_playback();

		//Touch processing
		MediaBrowser_ReadKeys(pHalContext);
		if (g_MultimediExp_Ctx->PenDown) {
			px = g_MultimediExp_Ctx->CurrX;
			py = g_MultimediExp_Ctx->CurrY;
		}
		//Scroll processing
		if (g_MultimediExp_Ctx->PenDown == 0 && g_MultimediExp_Ctx->PrevTag != 0 && g_MultimediExp_Ctx->CurrTag == 0)
        {
			printf("Tag: %d \n\r", g_MultimediExp_Ctx->PrevTag);
			switch (g_MultimediExp_Ctx->PrevTag) {
			case MEDIAPLAYER_KEY_EXIT:
			case MEDIAPLAYER_KEY_BACK:
				ret = g_MultimediExp_Ctx->PrevTag;
				goto out;
			case MEDIAPLAYER_KEY_AUD_PLAY:
				playing = !playing;
				if (playing) {
#if defined(MSVC_PLATFORM) || defined(BT8XXEMU_PLATFORM) 
					{
						char fullFname[MAX_PATH];
						sprintf(fullFname, "%s\\%s", MSVC_MEDIA_ROOT_DIR, filename);
						CleO->cmd_AudioPlay(fullFname, PLAY_LOOP);
					}
#else
					CleO->cmd_AudioPlay(filename, PLAY_LOOP);
#endif
					if (cursample > 0) {
						CleO->audio_seekto(cursample);
					}
					g_MultimediExp_Ctx->UiFlags |= MEDIA_GLOBAL_AUDIO_PLAYING;
				} else {
					cursample = CleO->audio_played_samples();
					CleO->cmd_AudioStop();
					g_MultimediExp_Ctx->UiFlags &= ~MEDIA_GLOBAL_AUDIO_PLAYING;
				}
				break;
			case MEDIAPLAYER_KEY_AUD_PROGRESS0:
			case MEDIAPLAYER_KEY_AUD_PROGRESS1:
				printf("Got touch at: %u %u %d %d%%\n\r", px, py, g_MultimediExp_Ctx->PrevTag,
						(px - progbar_start)*100/(progbar_end - progbar_start));
				if (!(px & 0x8000)) {
					float ratio = (float)(px - progbar_start)/(float)(progbar_end - progbar_start);
					cursample = nsamples*ratio;
					ft_uint16_t newpx = progbar_start+cursample*(progbar_end - progbar_start)/nsamples;
					//Seek audio
					printf("audio_seekto: %d (%d)\n\r", cursample, nsamples);
					if (playing) {
						CleO->audio_seekto(cursample);
					}
				}
				break;
			}
		}
	}
out:
	//Exit now!
	return ret;
}

ft_int32_t MultimediaExplorer_AppViewPicture(Ft_Gpu_Hal_Context_t *pHalContext, GuiManager *CleO, const char* filename)
{
#define MEDIA_VIEW_PIC_UI_CNT 100
#define direct_send32(x) do {\
	Ft_Gpu_Hal_WrCmd32(pHalContext, x);\
}while(0)

	//int16_t im;
	int xdist, ydist;
	int ret = 0;
	struct RenderArea_t area;
	int img_size = -1;
	ft_uint16_t w = g_MultimediExp_Ctx->mediaInfo.w;
	ft_uint16_t h = g_MultimediExp_Ctx->mediaInfo.h;
	int zoomLevel = 1;
	int xscroll, yscroll, scaledW, scaledH;
	float scaleRatio;
	ScaleAxis_e scaleAxis = MEDIA_SCALE_X; //0: x-axis, 1: y-axis
	int X,Y,x,y;
	ft_int16_t BitmapScale;
	ft_int16_t prevX = 0, prevY = 0;
	ft_uint8_t prevPendown;
	int bigScale = 0;

	g_MultimediExp_Ctx->dispUICnt = MEDIA_VIEW_PIC_UI_CNT;
	area.fgColor = WHITE_COLOR();

	//Render spinning
	helperBeginRender(pHalContext);
	EVE_Cmd_wr32(pHalContext,COLOR_RGB(0, 0, 128));
	Ft_Gpu_CoCmd_Spinner(pHalContext, FT_DispWidth/2, FT_DispHeight/2,0,0);
	helperEndRender(pHalContext);

	//Load image first: use icon max
	if (g_MultimediExp_Ctx->mediaInfo.fmt != 255) {
        img_size = w*h*2;
	} else {
		img_size = -1;
	}
	printf("Loading picture %s wxh=%dx%d %d \n\r", filename,
			g_MultimediExp_Ctx->mediaInfo.w, g_MultimediExp_Ctx->mediaInfo.h,
			g_MultimediExp_Ctx->gram.gram_free);

	if (img_size > 0 && img_size <= g_MultimediExp_Ctx->gram.gram_free) {
		ft_int16_t bytesread;
		ft_int16_t f;
		float xscale, yscale;

		xscale = (float)FT_DispWidth/(float)w;
		yscale = (float)FT_DispHeight/(float)h;
		if (xscale < yscale) {
			scaleAxis = MEDIA_SCALE_X;
			zoomLevel = w;
		} else {
			scaleAxis = MEDIA_SCALE_Y;
			zoomLevel = h;
		}
		X = (FT_DispWidth - w)*8;
		Y = (FT_DispHeight - h)*8;
#if defined(MSVC_PLATFORM) || defined(BT8XXEMU_PLATFORM) 
		{
			char fullFname[MAX_PATH];
			sprintf(fullFname, "%s\\%s", MSVC_MEDIA_ROOT_DIR, filename);
			f = CleO->cmd_FOpen(fullFname, FILE_READ);
		}
#else
		f = CleO->cmd_FOpen(filename, FILE_READ);
#endif
		if (f < 0) {
			img_size = -1;
			goto start_render;
		}
		ft_uint16_t magic = CleO->be16(f);
		CleO->service_playback();
		switch (magic) {
		case 0x424d:  // .bmp starts with 'B' 'M'
		{
			FILEHEADER fh;
			INFOHEADER ih;

			CleO->service_playback();
			CleO->cmd_FSeek(f, 0);
			CMD_FREAD(CleO, f, (ft_uint8_t*) &fh, sizeof(FILEHEADER), bytesread);
			CMD_FREAD(CleO, f, (ft_uint8_t*) &ih, sizeof(INFOHEADER), bytesread);

			ft_uint8_t bgr[3];
			int i, j;
			int s = w * 2;
#if defined(MSVC_PLATFORM) || defined(BT8XXEMU_PLATFORM) 
			ft_uint16_t alignW = (w + 1)&~1;
			ft_uint32_t *pRgbData = (ft_uint32_t*)malloc((s + 7)&~3);
			ft_uint8_t *pRawData = (ft_uint8_t*)malloc(alignW*3);
			int datIdx;
#endif
			for (i = 0; i < h; i++) {
				CleO->service_playback();
#if defined(MSVC_PLATFORM) || defined(BT8XXEMU_PLATFORM) 
				Ft_Gpu_Hal_WrCmd32(pHalContext, CMD_MEMWRITE);
				Ft_Gpu_Hal_WrCmd32(pHalContext, (h - i - 1) * s);
				Ft_Gpu_Hal_WrCmd32(pHalContext, s);
#else
				Ft_Gpu_CoCmd_MemWrite(pHalContext, (h - i - 1) * s, s);
#endif
				int occ = 0;
				ft_uint32_t d32 = 0;
#if defined(MSVC_PLATFORM) || defined(BT8XXEMU_PLATFORM) 
				datIdx = 0;
				CMD_FREAD(CleO, f, pRawData, 3*alignW, bytesread);
				if (bytesread != 3 * alignW) printf("WARN: fail to read BMP file!\n");
				//assert(bytesread == 3 * w);
				for (j = 0; j < alignW; j++) {
					pRgbData[datIdx] = ((pRawData[3 * j + 2] >> 3) << 11)
						| ((pRawData[3 * j + 1] >> 2) << 5) | (pRawData[3 * j + 0] >> 3);
					j++;
					pRgbData[datIdx++] |= (((pRawData[3 * j + 2] >> 3) << 11)
						| ((pRawData[3 * j + 1] >> 2) << 5) | (pRawData[3 * j + 0] >> 3))<<16;
				}
				//Sending data
				Ft_Gpu_Hal_WrCmdBuf(pHalContext, pRgbData, datIdx * 4);
#else
				for (j = 0; j < w; j++) {
					CMD_FREAD(CleO, f, bgr, sizeof(bgr), bytesread);
					ft_uint16_t rgb565 = ((bgr[2] >> 3) << 11)
						| ((bgr[1] >> 2) << 5) | (bgr[0] >> 3);
					d32 |= (rgb565 << occ);
					occ += 16;
					if (occ == 32) {
						direct_send32(d32);
						d32 = 0;
						occ = 0;
					}
				}
				if (w & 1)  // padding for odd width
					CMD_FREAD(CleO, f, bgr, sizeof(bgr), bytesread);
				if (occ != 0) {
					direct_send32(d32);
				}
#endif
			}
#if defined(MSVC_PLATFORM) || defined(BT8XXEMU_PLATFORM) 
			free(pRgbData);
			free(pRawData);
#endif
		}
			break;

		case 0xffd8:  // .jpg
		case 0x8950:  // .png
		{
			CleO->service_playback();
			if (FR_OK != CleO->cmd_FSeek(f, 0)) printf("Fail to seek!\n");
#if defined(MSVC_PLATFORM) || defined(BT8XXEMU_PLATFORM) 
			//Ft_Gpu_CoCmd_LoadImage(pHalContext, 0, OPT_NODL);
			Ft_Gpu_Hal_WrCmd32(pHalContext, CMD_LOADIMAGE);
			Ft_Gpu_Hal_WrCmd32(pHalContext, 0);
			Ft_Gpu_Hal_WrCmd32(pHalContext, OPT_NODL);
#else
			Ft_Gpu_CoCmd_LoadImage(pHalContext, 0, OPT_NODL);
#endif //  defined(MSVC_PLATFORM) || defined(BT8XXEMU_PLATFORM) 
			CleO->feedfile(f);
		}
			break;

		default:
			printf("Not support!\n\r");
			img_size = -1;
		}
		int r = CleO->cmd_FClose(f);
		if (r < 0) {
			img_size = -1;
		}
	}

start_render:
    CleO->service_playback();
	printf("Image: %s wxh=%dx%d fmt=%d sz=%d gram_free=%d\n\r",
		filename, w, h, g_MultimediExp_Ctx->mediaInfo.fmt,
		img_size, g_MultimediExp_Ctx->gram.gram_free);
	MediaBrowser_UIInit();
	xscroll = yscroll = 0;
	while (1) {
        /////////////////////////////////Rendering UI///////////////////////////////
		//Start Rendering
		helperBeginRender(pHalContext);

		/*render_scrollbar(pHalContext, &g_MultimediExp_Ctx->sb);*/
		EVE_Cmd_wr32(pHalContext,COLOR_A(164));
		/*xoff = g_MultimediExp_Ctx->tileW*8;
		yoff = g_MultimediExp_Ctx->tileH*8;*/
		EVE_Cmd_wr32(pHalContext,TAG_MASK(1));

		ydist = 0;
		if (FT_DispWidth >= 600) {
			xdist = 120;
		} else {
			xdist = 80;
		}
		if (img_size < 0 || img_size > g_MultimediExp_Ctx->gram.gram_free) {
		    area.w = FT_DispWidth - xdist;
		    area.h = MAINUI_ELEMENT_HEIGHT*2;
		    area.x = xdist;
		    area.y = FT_DispHeight/2;
			helperRenderAlignedString(pHalContext, "UNSUPPORT FILE FORMAT", MEDIA_FONT_HUGE, &area, ALIGN_CENTER);
		} else {
			//Render image now!
			EVE_Cmd_wr32(pHalContext, TAG(MEDIAPLAYER_KEY_PIC_SEL));
			EVE_Cmd_wr32(pHalContext, WHITE_COLOR());
			EVE_Cmd_wr32(pHalContext, BITMAP_HANDLE(MEDIA_ICON_MAX+1));
			//Ft_Gpu_CoCmd_SetBitmap(pHalContext, pBmInfo->gram_src, pBmInfo->fmt, pBmInfo->w, pBmInfo->h);
			if ((g_MultimediExp_Ctx->mediaInfo.fmt == PALETTED565) || (g_MultimediExp_Ctx->mediaInfo.fmt == PALETTED4444)) {
		        Ft_Gpu_CoCmd_SetBitmap(pHalContext, 2 * (g_MultimediExp_Ctx->mediaInfo.palsize + 1),
		        		               g_MultimediExp_Ctx->mediaInfo.fmt,
									   g_MultimediExp_Ctx->mediaInfo.w, g_MultimediExp_Ctx->mediaInfo.h);
		        direct_send32(PALETTE_SOURCE(0));
			} else {
				Ft_Gpu_CoCmd_SetBitmap(pHalContext, 0, g_MultimediExp_Ctx->mediaInfo.fmt, w, h);
			}
#define ZOOM_CONST (1<<16UL-1)
#define SCALE_UNIT  (64)
			if (scaleAxis == MEDIA_SCALE_X) {
				scaleRatio = ((float)(zoomLevel + LOWEST_ZOOM_PIXEL)/(float)w);
			} else {
				scaleRatio = ((float)(zoomLevel + LOWEST_ZOOM_PIXEL)/(float)h);
			}
			if (scaleRatio != 1.0f) {
				//Normalize scale value!
				BitmapScale = scaleRatio*SCALE_UNIT;

				scaledW = w*BitmapScale/SCALE_UNIT;
				scaledH = h*BitmapScale/SCALE_UNIT;
				if (scaledW > HIGHEST_ZOOM_PIXEL) {
					scaledW = HIGHEST_ZOOM_PIXEL;
					BitmapScale = HIGHEST_ZOOM_PIXEL*SCALE_UNIT/w;
					scaledH = h*BitmapScale/SCALE_UNIT;
				}
				if (scaledH > HIGHEST_ZOOM_PIXEL) {
					scaledH = HIGHEST_ZOOM_PIXEL;
					BitmapScale = HIGHEST_ZOOM_PIXEL*SCALE_UNIT/h;
					scaledW = w*BitmapScale/SCALE_UNIT;
				}

				EVE_Cmd_wr32(pHalContext, BEGIN(BITMAPS));
				EVE_Cmd_wr32(pHalContext, BITMAP_HANDLE(MEDIA_ICON_MAX + 1));
				EVE_Cmd_wr32(pHalContext,
						BITMAP_SIZE(BILINEAR, BORDER, BORDER, scaledW, scaledH));
				EVE_Cmd_wr32(pHalContext,
						BITMAP_SIZE_H(scaledW >> 9, scaledH >> 9));
				Ft_Gpu_CoCmd_LoadIdentity(pHalContext);
//				Ft_Gpu_CoCmd_Translate(pHalContext, (d << 16) / 8, (d << 16) / 8);
				Ft_Gpu_CoCmd_Scale(pHalContext, BitmapScale << 10, BitmapScale << 10);
				/*{
					frac8_t jx = 0, jy = 0;

					jx -= (w);
					jy -= (h);
					Ft_Gpu_CoCmd_Translate(pHalContext, (jx << 16), (jy << 16));
				}*/
				Ft_Gpu_CoCmd_SetMatrix(pHalContext);
				x = (FT_DispWidth-scaledW+xscroll*2)*8;
				y = (FT_DispHeight-scaledH+yscroll*2)*8;
				if (g_MultimediExp_Ctx->PenDown) {
					//printf("Render at: %d,%d \n", x, y);
					if (x < -15000 || y < -15000)
						bigScale = 1;
					else
						bigScale = 0;
				}
				if (x < -(1<<14)) x = -(1<<14);
				if (y < -(1<<14)) y = -(1<<14);
			} else {
				x = X;
				y = Y;
				EVE_Cmd_wr32(pHalContext, BEGIN(BITMAPS));
				EVE_Cmd_wr32(pHalContext, BITMAP_HANDLE(MEDIA_ICON_MAX + 1));
			}
			//NOTE: must use VERTEX2F because CleO50 has big resolution 800x480 (>512)
			EVE_Cmd_wr32(pHalContext,VERTEX2F(x,y));
			//Restore
			Ft_Gpu_CoCmd_LoadIdentity(pHalContext);
		    Ft_Gpu_CoCmd_SetMatrix(pHalContext);
			EVE_Cmd_wr32(pHalContext, TAG(255));

#if 0
			//Render boundary recs
			EVE_Cmd_wr32(pHalContext, LIGHT_GREY_COLOR());
			EVE_Cmd_wr32(pHalContext, LINE_WIDTH(1 * 8));
			EVE_Cmd_wr32(pHalContext, BEGIN(RECTS));
			EVE_Cmd_wr32(pHalContext, VERTEX2F(x, y));
			if (scaleRatio != 1.0f) {
				tmpx = x + scaledW*16;
				tmpy = y + scaledH*16;
				if (tmpx >= FT_DispWidth*16)
					tmpx = FT_DispWidth*16;
				if (tmpy >= FT_DispHeight*16)
					tmpy = FT_DispHeight*16;
				EVE_Cmd_wr32(pHalContext, VERTEX2F(tmpx, tmpy));
			} else {
				EVE_Cmd_wr32(pHalContext, VERTEX2F(x + w*16, y + h*16));
			}
#endif
		}
		if (g_MultimediExp_Ctx->dispUICnt) {
			//////Display header
			helperRenderHeaderPanel(pHalContext, xdist, ydist,
                                    &g_MultimediExp_Ctx->iconList[MEDIA_ICON_BACK],
                                    &g_MultimediExp_Ctx->iconList[MEDIA_ICON_EXIT],
                                    g_MultimediExp_Ctx->CurrTag, 0);
			//////End Header

			if (img_size > 0 && img_size < g_MultimediExp_Ctx->gram.gram_free) {
				///////////////////////Display Zoom Level Control //////////////////////////////
				ydist = FT_DispHeight - 2 * MAINUI_ELEMENT_HEIGHT;
				if (g_MultimediExp_Ctx->PenDown
						&& g_MultimediExp_Ctx->CurrTag == MEDIAPLAYER_KEY_VOL_ZOOM_CONTROL
						&& (g_MultimediExp_Ctx->CurrY >= 3 * MAINUI_ELEMENT_HEIGHT / 2)
						&& (g_MultimediExp_Ctx->CurrY <= ydist)) {
					zoomLevel = (ydist - g_MultimediExp_Ctx->CurrY) * RANGE_ZOOM_PIXEL
							    / (ydist - 3 * MAINUI_ELEMENT_HEIGHT / 2);
					xscroll = yscroll = 0;
					/*printf("ZoomLevel: %d -> wxh=%dx%d [%dx%d] \n\r", zoomLevel, w, h, scaledW, scaledH);*/
					//Setting new level
				}
				helperRenderVolumeBar(pHalContext, xdist, ydist<<1, 3*MAINUI_ELEMENT_HEIGHT, MEDIAPLAYER_KEY_VOL_ZOOM_CONTROL, zoomLevel, RANGE_ZOOM_PIXEL);
				///////////////////////////End of Zoom Level Control /////////////////////////////

				//////Render file name///////////////////////////////////////////////////////////
				ydist = FT_DispHeight - 3 * MAINUI_ELEMENT_HEIGHT / 2;
				EVE_Cmd_wr32(pHalContext, WHITE_COLOR());
				area.w = FT_DispWidth - xdist;
				area.h = MAINUI_ELEMENT_HEIGHT * 2;
				area.x = xdist;
				area.y = ydist;
				area.fgColor = WHITE_COLOR();
				if (FT_DispWidth >= 600) {
					helperRenderAlignedString(pHalContext, filename, MEDIA_FONT_HUGE, &area,
							ALIGN_CENTER);
				} else {
					helperRenderAlignedString(pHalContext, filename, MEDIA_FONT_MEDIUM, &area,
							ALIGN_CENTER);
				}
				//////End of Render file name////////////////////////////////////////////////////

				///////////////////Render scroll bar ////////////////////////////////////////////
				//Vertical scroll bar
				Ft_Gpu_CoCmd_FgColor(pHalContext, LIGHT_GREY_COLOR());
				Ft_Gpu_CoCmd_BgColor(pHalContext, BLACK_COLOR());
				if (scaledH > (int) FT_DispHeight) {
					if (y < 0)
						y = -y;
					y /= 16;
					Ft_Gpu_CoCmd_Scrollbar(pHalContext, FT_DispWidth - 10,
					MAINUI_ELEMENT_HEIGHT + 8, 10,
							FT_DispHeight - MAINUI_ELEMENT_HEIGHT - 15, 0, y,
							FT_DispHeight, scaledH);
				}

				//Horizontal scroll bar
				if (scaledW > (int) FT_DispWidth) {
					if (x < 0)
						x = -x;
					x /= 16;
					Ft_Gpu_CoCmd_Scrollbar(pHalContext, 0, FT_DispHeight - 10,
							FT_DispWidth - 10, 10, 0, x, FT_DispWidth, scaledW);
				}
			}
		    ///////////////////End of scroll bar ////////////////////////////////////////////
		}

		helperEndRender(pHalContext);
		//////////////////////////////////////////////// End of UI Rendering //////////////////////////
		CleO->service_playback();
		if (g_MultimediExp_Ctx->PenDown && g_MultimediExp_Ctx->CurrTag == MEDIAPLAYER_KEY_PIC_SEL) {
			prevX = g_MultimediExp_Ctx->CurrX;
			prevY = g_MultimediExp_Ctx->CurrY;
		}
		prevPendown = g_MultimediExp_Ctx->PenDown;
		MediaBrowser_ReadKeys(pHalContext);

		/* pen down */
		if (g_MultimediExp_Ctx->PenDown && prevPendown &&
			((g_MultimediExp_Ctx->CurrTag == MEDIAPLAYER_KEY_PIC_SEL && g_MultimediExp_Ctx->PrevTag == MEDIAPLAYER_KEY_PIC_SEL) /*||
			bigScale*/)) {
			int xdiff, ydiff;

			xdiff = g_MultimediExp_Ctx->CurrX - prevX;
			ydiff = g_MultimediExp_Ctx->CurrY - prevY;
			/*printf("Got picture select! %d %d\n\r", xdiff, ydiff);*/

			if ((scaledH > (int)FT_DispHeight))
			{
				if (((yscroll + ydiff) <= (scaledH - FT_DispHeight)/2) &&
					((yscroll + ydiff) >= (FT_DispHeight - scaledH)/2))
				{
					//Reach limited
					yscroll += ydiff;
					y = (FT_DispHeight-scaledH+yscroll*2)*8;
					/*printf("Y scale: %d %d %d %d -> %d\n\r", scaledH, FT_DispHeight, yscroll, ydiff, y);*/
				}
			}
			if (scaledW > (int)FT_DispWidth) {
				if (((xscroll + xdiff) <= (scaledW - FT_DispWidth)/2) &&
					((xscroll + xdiff) >= (FT_DispWidth - scaledW)/2))
				{
					//reach limited!
					xscroll += xdiff;
					x = (FT_DispWidth-scaledW+xscroll*2)*8;
					/*printf("X scale: %d %d %d %d -> %d\n\r", scaledW, FT_DispWidth, xscroll, xdiff, x);*/
				}
			}

		} else if (g_MultimediExp_Ctx->PenDown == 0 && g_MultimediExp_Ctx->PrevTag != 0 && g_MultimediExp_Ctx->CurrTag == 0) {
			switch (g_MultimediExp_Ctx->PrevTag) {
			case MEDIAPLAYER_KEY_BACK:
				MediaBrowser_UIInit();
				//Fall-through
			case MEDIAPLAYER_KEY_EXIT:
				ret = g_MultimediExp_Ctx->PrevTag;
				goto out;
			default:
				break;
			}
		}
	}
out:
	/*CleO->cmd_Free(im);*/
    //Restore the transform to normal
    Ft_Gpu_CoCmd_LoadIdentity(pHalContext);
    Ft_Gpu_CoCmd_SetMatrix(pHalContext);
	return ret;
}
#define uart_puts(a,b)


void scroll()
{
	//Should detect if scroll up or down
	if (g_MultimediExp_Ctx->state != MEDIA_STATEMACHINE_MAIN) {
		return;
	}
	scroller_touch(&g_MultimediExp_Ctx->scrollobj,
			        Ft_Gpu_Hal_Rd16(g_MultimediExp_Ctx->pHalContext, REG_TOUCH_SCREEN_XY),
					g_MultimediExp_Ctx->CurrTag);
}

int getMediaFileStatus(struct MediaItem_t *mediaItem, struct MediaInfo_t *info, GuiManager *CleO)
{
#if defined(MSVC_PLATFORM) || defined(BT8XXEMU_PLATFORM) 
	char fname[MAX_PATH];
	sprintf(fname, "%s%s", MSVC_MEDIA_ROOT_DIR, mediaItem->fname);
#else
	char* fname = mediaItem->fname;
#endif
	info->h = -1;
	info->w = -1;
	info->flag = 0x0;
	info->fps = -1;
	info->dur = 0;

	int h = CleO->cmd_FOpen(fname, FILE_READ);
	if (h < 0) {
		return h;
	}
	CMD_FSIZE(CleO, h, info->fsize);

	if (mediaItem->fname[0] == '@') {
		info->flag = MEDIA_INFO_EFLASH_FILE;
	}
	switch (mediaItem->type) {
	case MEDIA_TYPE_VIDEO:
		extractRiffInfo(mediaItem, info, CleO, h);
		break;
	case MEDIA_TYPE_AUDIO:
		extractWavInfo(mediaItem, info, CleO, h);
		//extractRiffInfo(&g_MultimediExp_Ctx->mediaItem[idx], &g_MultimediExp_Ctx->mediaInfo);
		break;
	case MEDIA_TYPE_PIC:
		extractJpegInfo(mediaItem, info, h);
		break;
	}
	CleO->cmd_FClose(h);
	return 0;
}


void updateMediaCompactInfo()
{
	struct MediaItem_t mediaItem;
	int i;

	if (g_MultimediExp_Ctx->UiFlags & MEDIA_GLOBAL_UPDATED_INFO)
		return;
	for (i = 0; i < g_MultimediExp_Ctx->dispWindow.size; i++) {
		initMediaInfo(&mediaItem, i);
//		printf("Extract info item: %d %s \n\r", g_MultimediExp_Ctx->dispWindow.start + i, mediaItem.fname);
		//Extract media info
		getMediaFileStatus(&mediaItem, &g_MultimediExp_Ctx->compactInfoList[i], g_MultimediExp_Ctx->CleO);
	}
	g_MultimediExp_Ctx->UiFlags |= MEDIA_GLOBAL_UPDATED_INFO;
	g_MultimediExp_Ctx->UiFlags &= ~MEDIA_GLOBAL_SCROLLING_CLEAR;
	printf("Done update info\n\r");
	return;
}

int helperPlayMedia(struct MediaItem_t *mediaItem, Ft_Gpu_Hal_Context_t *pHalContext, GuiManager *CleO, ft_uint8_t *mediaCmd, char* bufferCmd)
{
	int aret = -1;

	strncpy(bufferCmd, mediaItem->fname, MEDIA_COMMAND_BUFFER_SIZE);
	if (mediaItem->type == MEDIA_TYPE_AUDIO) {
		*mediaCmd = MEDIA_TYPE_INVALID;
		if (strcmp(g_MultimediExp_Ctx->audioFile, mediaItem->fname)) {
			strncpy(g_MultimediExp_Ctx->audioFile, mediaItem->fname, MEDIA_COMMAND_BUFFER_SIZE);
		} else if (g_MultimediExp_Ctx->UiFlags & MEDIA_GLOBAL_AUDIO_PLAYING) {
			g_MultimediExp_Ctx->UiFlags |= MEDIA_GLOBAL_AUDIO_CONT_PLAYING;
		}
		g_MultimediExp_Ctx->state = MEDIA_STATEMACHINE_PLAY_AUDIO;
		aret = MultimediaExplorer_AppPlayAudio(pHalContext, g_MultimediExp_Ctx->CleO, g_MultimediExp_Ctx->audioFile);
		g_MultimediExp_Ctx->state = MEDIA_STATEMACHINE_MAIN;
		g_MultimediExp_Ctx->UiFlags &= ~MEDIA_GLOBAL_AUDIO_CONT_PLAYING;
	} else if (mediaItem->type == MEDIA_TYPE_PIC) {
		g_MultimediExp_Ctx->state = MEDIA_STATEMACHINE_VIEW_PICTURE;
		aret = MultimediaExplorer_AppViewPicture(pHalContext, CleO, bufferCmd);
		g_MultimediExp_Ctx->state = MEDIA_STATEMACHINE_MAIN;
		//Extract media info
	} else {
		*mediaCmd = MEDIA_TYPE_VIDEO;
		aret = MEDIAPLAYER_KEY_EXIT;
	}
	return aret;
}

#ifdef SUPPORT_MEDIAPLAYER_RETAIN_STATUS
union progInfo_t {
	int8_t buf[256];
	struct {
		int8_t marker;
		int8_t length;
		int8_t percent;
		char name[253];
	};
};

const char* fileProgDb = "/System/prog.db";
int findMediaFile(FIL *pFile, const char* filename, union progInfo_t *pInfo)
{
	UINT rnum = 0;
	int read_ptr = 0;
	FRESULT res;
	FILINFO fileinfo;
	int len = strlen(filename);

	if(sdhost_card_detect() != SDHOST_CARD_INSERTED) {
		//No sdcard to saving info now!
		return -1;
	}
	res = f_open(pFile, fileProgDb, FA_READ|FA_WRITE|FA_OPEN_ALWAYS);
	if (res != FR_OK) {
		return -1;
	}
#if _USE_LFN
	fileinfo.lfname = NULL;
	fileinfo.lfsize = 0;
#endif
	f_lseek(pFile, 0);
	read_ptr = 0;
	while(f_read(pFile, (void*)pInfo, sizeof(*pInfo), &rnum) == FR_OK) {
		if (rnum == 0)
			return -1;
		if (pInfo->marker != (int8_t)0xFF) {
			//Invalid file
			f_lseek(pFile, read_ptr);
			f_truncate(pFile);
			break;
		}
		if (!strncmp(pInfo->name, filename, len)) {
			//Found this
			f_lseek(pFile, read_ptr);
			return read_ptr;
		}
		read_ptr += pInfo->length;
		pInfo->name[252] = 0;
		f_lseek(pFile, read_ptr);
	}
	return -1;
}

extern "C" int MultimediaExplorer_SetFileProgress(const char* filename, uint8_t percent)
{
	FRESULT res;
	FIL file;
	union progInfo_t info;
	UINT rnum = 0;
	int read_ptr = 0;

	read_ptr = findMediaFile(&file, filename, &info);
	if (read_ptr < 0) {
		//New file:
		info.marker = 0xff;
		info.length = strlen(filename) + 3;
		strcpy(info.name, filename);
		//info.name[252] = 0;
		info.percent = percent;
	} else {
		info.percent = percent;
	}
	res = f_write(&file, &info, info.length, &rnum);
	f_close(&file);
	if (res != FR_OK) {
		printf("Fail to write!\n\r");
		return -1;
	}
	return 0;
}

extern "C" int8_t MultimediaExplorer_GetFileProgress(const char* filename)
{
	union progInfo_t info;
	FRESULT res;
	int read_ptr;
	FIL file;

	read_ptr = findMediaFile(&file, filename, &info);
	if (read_ptr < 0) {
		return -1;
	}
	f_close(&file);
	return info.percent;
}

#ifdef ENABLE_UT_FILEPROGRESS
void UT_FileProgress(void)
{
#define CHECK_PERCENT(id,name,val) do {\
	int8_t _percent = MultimediaExplorer_GetFileProgress(name);\
	if (_percent != val) printf("TC %d FAILLLL: File %s %d expect %d\n\r", id, name, _percent, val);\
    else printf("TC %d PASS\n\r", id);\
}while(0)
	int id = 0;

	printf("------ Begin Testing --- \n\r");
	CHECK_PERCENT(id++, "/Seed.avi", -1);
	MultimediaExplorer_SetFileProgress("1.avi", 12);
	MultimediaExplorer_SetFileProgress("2.avi", 33);
	MultimediaExplorer_SetFileProgress("5.avi", 80);
	CHECK_PERCENT(id++, "1.avi", 12);
	CHECK_PERCENT(id++, "2.avi", 33);
	CHECK_PERCENT(id++, "5.avi", 80);
	MultimediaExplorer_SetFileProgress("2.avi", 99);
	MultimediaExplorer_SetFileProgress("4.avi", 99);
	CHECK_PERCENT(id++, "2.avi", 99);
	CHECK_PERCENT(id++, "4.avi", 99);
	CHECK_PERCENT(id++, "5.avi", 80);
	CHECK_PERCENT(id++, "5.avi", 80);
	MultimediaExplorer_SetFileProgress("xbasdfjalsdklf123lkasdf-skdfjlsdkf.avi", 99);
	MultimediaExplorer_SetFileProgress("xbasdfjalsdklf123lkasdf-skdfjlsdkf334.avi", 1);
	CHECK_PERCENT(id++, "xbasdfja.avi", -1);
	CHECK_PERCENT(id++, "xbasdfjalsdklf123lkasdf-skdfjlsdkf.avi", 99);
	CHECK_PERCENT(id++, "xbasdfjalsdklf123lkasdf-skdfjlsdkf334.avi", 1);
}
#endif //ENABLE_UT_FILEPROGRESS

#endif //#ifdef SUPPORT_MEDIAPLAYER_RETAIN_STATUS
#define time_after(a,b) ((long)((b) - (a)) < 0)

#if defined(FT900_PLATFORM)
EXTERN_C size_t xPortGetFreeHeapSize();
#endif

EXTERN_C ft_int32_t AppAviPlayer(Ft_Gpu_Hal_Context_t *pHalCtxt, const char* fname, ft_int16_t Loop, int updateProgress);

ft_int32_t MultimediaExplorer_Main(Ft_Gpu_Hal_Context_t *pHalContext, GuiManager *CleO, int command, void *_data)
{
	//Fixme: may cause stack overflow!!!!
	char g_CmdBuffer[MEDIA_COMMAND_BUFFER_SIZE];
	ft_uint8_t mediaCmd = MEDIA_TYPE_INVALID;
	ft_int32_t ret = 0;
#ifdef FT900_DISABLE_EFLASH_STORAGE
	enum MediaStorageType_e lastStorage = MEDIA_STORAGE_TYPE_SDCARD;
#else
	enum MediaStorageType_e lastStorage = MEDIA_STORAGE_TYPE_EFLASH;
#endif
	struct data_window_t lastDispWindow = {-1,-1	};

	//Reserve GUI manager running context
#ifndef FT9XX_PLATFORM
	gui_preserve();
#endif //FT9XX_PLATFORM
#if defined(SUPPORT_MEDIAPLAYER_RETAIN_STATUS) && defined(ENABLE_UT_FILEPROGRESS)
	UT_FileProgress();
#endif //#ifdef ENABLE_UT_FILEPROGRESS

again:
	FT_DispWidth = pHalContext->Width;
	FT_DispHeight = pHalContext->Height;

	//This small buffer for communicate with next command
	g_MultimediExp_Ctx = (MultimediaExp_Global_t*)malloc(sizeof(*g_MultimediExp_Ctx) +
			                                             sizeof(struct MediaInfo_t)*(FT_DispHeight/MAINUI_ELEMENT_HEIGHT));
	if (!g_MultimediExp_Ctx) {
		return ERROR_NOTENOUGHCORE;
	}
	memset(g_MultimediExp_Ctx, 0, sizeof(MultimediaExp_Global_t));
	//////////////////Init meta data/////////////////////////////////
	g_MultimediExp_Ctx->pHalContext = pHalContext;
	g_MultimediExp_Ctx->CleO = CleO;
	//NOTE NOTE!!!!: CleO reserve ~ 72KB at top GRAM for pngload, audio playback buffer ....
	g_MultimediExp_Ctx->gram.gram_free = 1<<20;//Top of RAM is 1MB, reserve 10KB
	///////////////// End of Init meta data /////////////////////////
	g_MultimediExp_Ctx->tileW = FT_DispWidth;
	g_MultimediExp_Ctx->tileH = MAINUI_ELEMENT_HEIGHT;
	g_MultimediExp_Ctx->colNum = FT_DispWidth/g_MultimediExp_Ctx->tileW;
	g_MultimediExp_Ctx->rowNum = (FT_DispHeight)/g_MultimediExp_Ctx->tileH - 1;
	//TODO: should calculate the size of one page
	FT_Util_StartApp(pHalContext);
#if defined(FT900_PLATFORM)
	ft_millis_init();
#endif

	//TODO: Scan the sdcard or read from database file
	g_MultimediExp_Ctx->state = MEDIA_STATEMACHINE_MAIN;
	MediaBrowser_ResetMediaItem();
	MediaBrowser_UIInit();
	g_MultimediExp_Ctx->UiFlags |= MEDIA_GLOBAL_NOUSBSTORAGE;
#if defined(FT900_PLATFORM)
	if (sdhost_card_detect() != SDHOST_CARD_INSERTED) {
		g_MultimediExp_Ctx->UiFlags |= MEDIA_GLOBAL_NOSDCARD;
#ifndef FT900_DISABLE_EFLASH_STORAGE
		g_MultimediExp_Ctx->storageType = MEDIA_STORAGE_TYPE_EFLASH;
#else
		g_MultimediExp_Ctx->storageType = MEDIA_STORAGE_TYPE_INV;
#endif
		g_MultimediExp_Ctx->UiFlags &= ~MEDIA_GLOBAL_SCANNED;
	} else {
		g_MultimediExp_Ctx->storageType = lastStorage;
	}
#endif //FT900_PLATFORM

	//Always scan at beginning!
	MediaBrowser_Scan();
	if (lastDispWindow.size != -1) {
		g_MultimediExp_Ctx->dispWindow = lastDispWindow;
	}

	//Update scroller
    scroller_init(&g_MultimediExp_Ctx->scrollobj,
                   MEDIABROWSER_ITEM_TAG_BEGIN, g_MultimediExp_Ctx->rowNum*g_MultimediExp_Ctx->colNum,
			g_MultimediExp_Ctx->dispWindow.start/g_MultimediExp_Ctx->colNum,
			(g_MultimediExp_Ctx->dbData.totMediaFile+1)/g_MultimediExp_Ctx->colNum,
			g_MultimediExp_Ctx->tileH,
			g_MultimediExp_Ctx->rowNum);
    /*CleO->cmd_SetBackgroundcolor(0x101014);*/
	mediaCmd = MEDIA_TYPE_INVALID;

/*
	printf("tot: %d RamWindow: %d %d->DispWindow: %d %d %d\n\r",
			g_MultimediExp_Ctx->dbData.totMediaFile,
			g_MultimediExp_Ctx->ramWindow.start, g_MultimediExp_Ctx->ramWindow.size,
			g_MultimediExp_Ctx->dispWindow.start, g_MultimediExp_Ctx->dispWindow.size,
			g_MultimediExp_Ctx->storageType);
*/
#ifndef FT9XX_PLATFORM
	/////Process the application command//////
	if (command) {
		switch(command) {
			case 1:
				if (_data != NULL) *((int*)_data) = g_MultimediExp_Ctx->dbData.totMediaFile;
				//Exit now:
				goto media_command;
			case 2:
				if (_data == NULL || strlen((char*)_data) == 0) {
					goto mainui;
				} else {
					struct MediaItem_t mediaItem;
					char *s;

					//Clear this command in-case return
					command = 0;
					memset(&mediaItem, 0, sizeof(mediaItem));
					mediaItem.fname = (char*)_data;
					mediaItem.type = detectMediaType(mediaItem.fname);
					getMediaFileStatus(&mediaItem, &g_MultimediExp_Ctx->mediaInfo, CleO);
					if (mediaItem.type == MEDIA_TYPE_INVALID ||
						(g_MultimediExp_Ctx->mediaInfo.flag & MEDIA_INFO_INVAL_FILE))
					{
						ret = ERROR_INVALIDFILE;
						goto exit_app;
					}
					ret = helperPlayMedia(&mediaItem, pHalContext, CleO, &mediaCmd, g_CmdBuffer);
					if (ret == MEDIAPLAYER_KEY_EXIT) {
						goto media_command;
					}
				}
				break;
		}
	}
mainui:
#endif //FT9XX_PLATFORM
	while(1) {
		{
			ft_uint32_t oldFlags = g_MultimediExp_Ctx->UiFlags;
			lastStorage = g_MultimediExp_Ctx->storageType;

#if defined(FT900_PLATFORM)
			if (sdhost_card_detect() != SDHOST_CARD_INSERTED) {
				g_MultimediExp_Ctx->UiFlags |= MEDIA_GLOBAL_NOSDCARD;
#ifndef FT900_DISABLE_EFLASH_STORAGE
				g_MultimediExp_Ctx->storageType = MEDIA_STORAGE_TYPE_EFLASH;
#else
				g_MultimediExp_Ctx->storageType = MEDIA_STORAGE_TYPE_INV;
#endif
				g_MultimediExp_Ctx->UiFlags &= ~MEDIA_GLOBAL_SCANNED;
			} else {
				g_MultimediExp_Ctx->UiFlags &= ~MEDIA_GLOBAL_NOSDCARD;
			}
#endif

#ifndef FT900_DISABLE_EFLASH_STORAGE
			if ((g_MultimediExp_Ctx->UiFlags & MEDIA_GLOBAL_NOSDCARD) &&
					g_MultimediExp_Ctx->storageType == MEDIA_STORAGE_TYPE_SDCARD)
			{
				g_MultimediExp_Ctx->storageType = MEDIA_STORAGE_TYPE_EFLASH;
				MediaBrowser_Scan();
			}
#endif //FT900_DISABLE_EFLASH_STORAGE
			if (((oldFlags ^ g_MultimediExp_Ctx->UiFlags) & MEDIA_GLOBAL_NOSDCARD) &&
				!(g_MultimediExp_Ctx->UiFlags & MEDIA_GLOBAL_NOSDCARD))
			{
				init_fatfs();
#ifdef  FT900_DISABLE_EFLASH_STORAGE
				g_MultimediExp_Ctx->storageType = MEDIA_STORAGE_TYPE_SDCARD;
				MediaBrowser_Scan();
#endif
			}
		}
		if(Ft_Gpu_Hal_Rd16(pHalContext, REG_CMD_READ) == Ft_Gpu_Hal_Rd16(pHalContext, REG_CMD_WRITE))
		{
			MediaBrowser_RenderUI();
			MediaBrowser_ReadKeys(pHalContext);
			scroll();
			//Scroll processing
/*
			printf("%d %d %d %d\n\r", g_MultimediExp_Ctx->PenDown, g_MultimediExp_Ctx->CurrTag, g_MultimediExp_Ctx->PrevTag, scroller_isScrolling(&g_MultimediExp_Ctx->scrollobj));
*/
			if (g_MultimediExp_Ctx->PenDown == 0 && g_MultimediExp_Ctx->PrevTag != 0 && g_MultimediExp_Ctx->CurrTag == 0)
			{
				if (g_MultimediExp_Ctx->PrevTag < MEDIAPLAYER_KEY_START_IDX)
				{
					int idx = g_MultimediExp_Ctx->PrevTag - MEDIABROWSER_ITEM_TAG_BEGIN;
					if (scroller_isScrolling(&g_MultimediExp_Ctx->scrollobj)) {
						g_MultimediExp_Ctx->PrevTag = 0;
					} else {
						ft_uint32_t now = ft_millis();
						if(idx >= 0 && time_after(g_MultimediExp_Ctx->touchTimeStamp + 200, now)) {
							struct MediaItem_t mediaItem;

							//Get tag
							g_MultimediExp_Ctx->selMedia = idx;
							initMediaInfo(&mediaItem, idx);
							getMediaFileStatus(&mediaItem, &g_MultimediExp_Ctx->mediaInfo, CleO);
/*
							printf("Select Item: %d %d %d %s [%d %d %d]\n\r", idx, g_MultimediExp_Ctx->PrevTag, g_MultimediExp_Ctx->CurrTag,
									mediaItem.fname,
									g_MultimediExp_Ctx->scrollobj.dragging,
									g_MultimediExp_Ctx->scrollobj.pos,
									g_MultimediExp_Ctx->scrollobj.scrollDelay);*/
							ret = helperPlayMedia(&mediaItem, pHalContext, CleO, &mediaCmd, g_CmdBuffer);
							if (ret == MEDIAPLAYER_KEY_EXIT) {
								goto media_command;
							}
						}
					}
				} else {
					switch(g_MultimediExp_Ctx->PrevTag) {
#if !defined(MSVC_PLATFORM) && !defined(BT8XXEMU_PLATFORM) 
#ifndef FT900_DISABLE_EFLASH_STORAGE
					case MEDIAPLAYER_KEY_EFLASH:
#endif
					case MEDIAPLAYER_KEY_SDCARD:
						if ((ft_uint8_t)g_MultimediExp_Ctx->storageType != g_MultimediExp_Ctx->PrevTag) {
							g_MultimediExp_Ctx->storageType = (enum MediaStorageType_e)g_MultimediExp_Ctx->PrevTag;
							MediaBrowser_Scan();
						}
						break;
#endif
					case MEDIAPLAYER_KEY_EXIT:
						goto media_command;
					}
				}
			}
			if (g_MultimediExp_Ctx->state == MEDIA_STATEMACHINE_MAIN) {
				scroller_move(&g_MultimediExp_Ctx->scrollobj);
				if (scroller_isScrolling(&g_MultimediExp_Ctx->scrollobj) || g_MultimediExp_Ctx->PenDown) {
					g_MultimediExp_Ctx->UiFlags &= ~MEDIA_GLOBAL_UPDATED_INFO;
				} else {
					updateMediaCompactInfo();
				}
			}
			if (g_MultimediExp_Ctx->UiFlags & MEDIA_GLOBAL_AUDIO_PLAYING) {
				CleO->service_playback();
			}
		}
	}
media_command:
    /* To ensure graceful exit is done and also gui manager expects cmd_dlstart */
    /*Ft_Gpu_Hal_WrMem(pHalContext, RAM_DL,(ft_uint8_t *)FT_DLCODE_BOOTUP,sizeof(FT_DLCODE_BOOTUP));
    Ft_Gpu_Hal_Wr8(pHalContext, REG_DLSWAP,DLSWAP_FRAME);
	Gpu_CoCmd_Dlstart(pHalContext);*/
	if (g_MultimediExp_Ctx->UiFlags & MEDIA_GLOBAL_AUDIO_PLAYING) {
		CleO->cmd_AudioStop();
	}
	FT_Util_ExitApp(pHalContext);
#if defined(FT900_PLATFORM)
	ft_millis_exit();
#endif
	//Backup some minimal status
	lastStorage = g_MultimediExp_Ctx->storageType;
	lastDispWindow = g_MultimediExp_Ctx->dispWindow;

	//Restore GUI manager running context
	free(g_MultimediExp_Ctx);
	g_MultimediExp_Ctx = NULL;
    if (mediaCmd != MEDIA_TYPE_INVALID) {
		//TODO: check if the file is exist!, if not we raise error trap!
		switch(mediaCmd)
		{
		case MEDIA_TYPE_VIDEO:
			//TODO: return to app or exit!
#if defined(MSVC_PLATFORM) || defined(BT8XXEMU_PLATFORM) 
		{
			char videoPath[MAX_PATH];

			sprintf(videoPath, "%s\\%s", MSVC_MEDIA_ROOT_DIR, g_CmdBuffer);
			ret = AppAviPlayer(pHalContext, videoPath, 1, 1);
			CleO->end_audio();
		}
#else
			ret = AppAviPlayer(pHalContext, g_CmdBuffer, 1, 1);
			CleO->end_audio();
#endif // 
#ifdef FT9XX_PLATFORM
			Gpu_Hal_WaitCmdfifo_empty(pHalContext);
#endif
			break;
		}
		if (ret == MEDIAPLAYER_KEY_BACK) {
			goto again;
		}
    }
/*
    uart_puts(UART0, "Exit MediaPlayer!\n\r");
*/
#ifndef FT9XX_PLATFORM
exit_app:
    gui_restore();
#endif //FT9XX_PLATFORM

	if (ret < 0) {
		return ret;
	}
	return 0;
}

