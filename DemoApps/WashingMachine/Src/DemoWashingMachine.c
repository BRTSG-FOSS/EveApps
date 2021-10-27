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

#ifdef DISPLAY_RESOLUTION_HVGA_PORTRAIT
#define DISPLAY_RESOLUTION_HVGA_PORTRAIT 1
#endif
#ifdef DISPLAY_RESOLUTION_WQVGA
#define DISPLAY_RESOLUTION_WQVGA 1
#endif
#ifdef DISPLAY_RESOLUTION_WVGA
#define DISPLAY_RESOLUTION_WVGA 1
#endif

#if EVE_CHIPID <= EVE_FT801
#define ROMFONT_TABLEADDRESS (0xFFFFC)
#else
#define ROMFONT_TABLEADDRESS 3145724UL // 2F FFFCh
#endif

#ifdef FT9XX_PLATFORM
#include "ff.h"
#endif

#define NAMEARRAYSZ 500

int16_t CartX = 480;

float linear(float p1, float p2, uint16_t t, uint16_t rate)
{
	float st = (float)t / rate;
	return p1 + (st*(p2 - p1));
}

uint16_t smoothstep(int16_t p1, int16_t p2, uint16_t t, uint16_t rate)
{
	float dst = (float)t / rate;
	float st = SQ(dst)*(3 - 2 * dst);
	return p1 + (st*(p2 - p1));
}

uint16_t acceleration(uint16_t p1, uint16_t p2, uint16_t t, uint16_t rate)
{
	float dst = (float)t / rate;
	float st = SQ(dst);
	return p1 + (st*(p2 - p1));
}

float deceleration(uint16_t p1, uint16_t p2, uint16_t t, uint16_t rate)
{
	float st, dst = (float)t / rate;
	dst = 1 - dst;
	st = 1 - SQ(dst);
	return p1 + (st*(p2 - p1));
}

static uint8_t keypressed = 0;

static uint8_t istouch()
{
	return !(EVE_Hal_rd16(s_pHalContext, REG_TOUCH_RAW_XY) & 0x8000);
}
void Use_Rgb()
{
	EVE_Cmd_wr32(s_pHalContext, COLOR_A(255));
	EVE_Cmd_wr32(s_pHalContext, COLOR_MASK(1, 1, 1, 1));
}
void Use_Alpha()
{
	EVE_Cmd_wr32(s_pHalContext, COLOR_MASK(0, 0, 0, 1));
	EVE_Cmd_wr32(s_pHalContext, COLOR_A(255));
}

void Clear_Alpha()
{
	EVE_Cmd_wr32(s_pHalContext, COLOR_MASK(0, 0, 0, 1));
	EVE_Cmd_wr32(s_pHalContext, COLOR_A(0));
}
void end_frame()
{
	EVE_Cmd_wr32(s_pHalContext, DISPLAY());
	EVE_CoCmd_swap(s_pHalContext);
}

#if defined(MSVC_PLATFORM) || defined(BT8XXEMU_PLATFORM)
void Load_file2gram(uint32_t add, uint8_t sectors, FILE *afile)
{
	uint8_t pbuff[512], temp[512], tval;
	uint16_t z = 0;
	for (z = 0; z < sectors; z++)
	{
		fread(pbuff, 1, 512, afile);
		EVE_Hal_wrMem(s_pHalContext, add, pbuff, 512L);
		add += 512;
	}
}
#endif

static struct {
	signed short dragprev;
	int vel;      // velocity
	long base;    // screen x coordinate, in 1/16ths pixel
	long limit;
} scroller;

static void scroller_init(uint32_t limit)
{
	scroller.dragprev = -32768;
	scroller.vel = 0;      // velocity
	scroller.base = 0;     // screen x coordinate, in 1/16ths pixel
	scroller.limit = limit;
}

/*
static void scroller_run()
{
  signed short sy = EVE_Hal_rd16(s_pHalContext,REG_TOUCH_SCREEN_XY);

  if ((sy != -32768) & (scroller.dragprev != -32768)) {
	scroller.vel = (scroller.dragprev - sy) << 4;
  } else {
	int change = MAX(1, abs(scroller.vel) >> 5);
	if (scroller.vel < 0)
	  scroller.vel += change;
	if (scroller.vel > 0)
	  scroller.vel -= change;
  }
  scroller.dragprev = sy;

  scroller.base += scroller.vel;

  scroller.base = MAX(0, MIN(scroller.base, scroller.limit));
}
*/

static uint16_t scroller_run(uint8_t touchTag)
{
	signed short sy = -32768;

	if (touchTag == 101) {
		sy = EVE_Hal_rd16(s_pHalContext, REG_TOUCH_SCREEN_XY);
		if ((sy != -32768) && (scroller.dragprev == -32768)) {
			scroller.dragprev = sy;
		}
		else if ((sy != -32768) & (scroller.dragprev != -32768)) {
			if (scroller.dragprev > sy)
				scroller.vel = scroller.dragprev - sy;
			else if (scroller.dragprev < sy)
				scroller.vel = scroller.dragprev - sy;

			scroller.dragprev = sy;

			scroller.base += (scroller.vel / 2);

			scroller.base = MAX(0, MIN(scroller.base, scroller.limit));

		}
	}
	else {
		scroller.dragprev = -32768;
	}

	return scroller.base;
}

typedef struct SAMAPP_Logo_Img {
	char8_t name[NAMEARRAYSZ];
	uint16_t image_height;
	uint8_t image_format;
	uint8_t filter;
	uint16_t sizex;
	uint16_t sizey;
	uint16_t linestride;
	uint16_t gram_address;
}SAMAPP_Logo_Img_t;

#ifdef DISPLAY_RESOLUTION_WQVGA
SAMAPP_Logo_Img_t Main_Icons[10] =
{
	{TEST_DIR "\\shirt.bin",		 40,    ARGB4,  NEAREST,    40,     40 ,    80,   0L      },
	{TEST_DIR "\\Temp.bin",		 40,    ARGB4,  NEAREST,    40,     40 ,    80,   3L      },
	{TEST_DIR "\\SpinW.bin",        40,    ARGB4,  NEAREST,    40,     40 ,    80,   7L      },
	{TEST_DIR "\\Butfly.bin",	     20,    ARGB4,  NEAREST,    20,     20 ,    40,   11L     },
	{TEST_DIR "\\Lock.bin",		 30,    ARGB4,  NEAREST,    30,     30 ,    60,   12L     },
	{TEST_DIR "\\UnLock.bin",		 30,    ARGB4,  NEAREST,    30,     60 ,    60,   14L     },
	{TEST_DIR "\\Set.bin",		     30,    ARGB4,  NEAREST,    30,     30 ,    60,   16L     },
	{TEST_DIR "\\Start.bin",		 30,    ARGB4,  NEAREST,    30,     30 ,    60,   18L     },
	{TEST_DIR "\\home.bin",		 40,    ARGB4,  NEAREST,    40,     40 ,    80,   20L     },//4kb
	{TEST_DIR "\\Bkgd.bin",        136,    L8,     NEAREST,    480,   272 ,   240,   24L     },
},

Wash_Window[7] =
{
	{TEST_DIR "\\Wash.bin",		55,	 ARGB4,	NEAREST,    50,	   50 ,	  100,	 57L	}, // size 22KB
	{TEST_DIR "\\Rinse.bin",		54,	 ARGB4,	NEAREST,    54,	   54 ,	  108,	 79L	}, // size 24KB
	{TEST_DIR "\\Spin.bin",		54,	 ARGB4,	NEAREST,    45,	   45 ,	   90,	103L	}, // size 30KB
	{TEST_DIR "\\WBar.bin",		52,	RGB565,	NEAREST,   400,	  400 ,	  800,	133L	}, //size 42KB
	{TEST_DIR "\\bub.bin",		    30,	 ARGB4,	NEAREST,    30,	   30 ,	   60,	175L	},//2kb
	{TEST_DIR "\\home.bin",		40,	 ARGB4,	NEAREST,    40,	   40 ,	   80,	177L	},//4kb
	{TEST_DIR "\\Bkgd.bin",		136,    L8,	NEAREST,   480,	  272 ,	  240,	181L	},   //33kb
};
#elif DISPLAY_RESOLUTION_WVGA

#ifdef FT9XX_PLATFORM
SAMAPP_Logo_Img_t Main_Icons[10] =
#else
PROGMEM SAMAPP_Logo_Img_t Main_Icons[10] =
#endif
{
	{TEST_DIR "\\shirtH.bin",		 70,    ARGB4,  NEAREST,    70,    70 ,    140,   0},   //10
	{TEST_DIR "\\TempH.bin",		 70,    ARGB4,  NEAREST,    70,     70 ,    140,   10},  //10
	{TEST_DIR "\\SpinWH.bin",       70,    ARGB4,  NEAREST,    70,     70 ,    140,   20},  //10
	{TEST_DIR "\\ButflyH.bin",	     26,    ARGB4,  NEAREST,    26,     26 ,    52,   30},     //2
	{TEST_DIR "\\LockH.bin",		 54,    ARGB4,  NEAREST,    54,     54 ,    108,   32},  //6
	{TEST_DIR "\\UnLockH.bin",		 53,    ARGB4,  NEAREST,    66,     53 ,    132,   38},  //7
	{TEST_DIR "\\SetH.bin",		 52,    ARGB4,  NEAREST,    52,     52 ,    104,   45     },  //6
	{TEST_DIR "\\StartH.bin",		 52,    ARGB4,  NEAREST,    52,     52 ,    104,   51     },  //6
	{TEST_DIR "\\homeH.bin",		 70,    ARGB4,  NEAREST,    70,     70 ,    140,   57     }, //10
	{TEST_DIR "\\BkgdH.bin",        240,      L8,  NEAREST,   800,    480 ,    400,   67     },         //94
};

#ifdef FT9XX_PLATFORM
SAMAPP_Logo_Img_t Wash_Window[7] =
#else
PROGMEM	SAMAPP_Logo_Img_t  Wash_Window[7] =
#endif
{
	{TEST_DIR "\\WashH.bin",		88,	 ARGB4,	NEAREST,    88,	   352,	  176,	161	}, // 61
	{TEST_DIR "\\RinseH.bin",		96,	 ARGB4,	NEAREST,    96,	   384,	  192,	222	}, // 72
	{TEST_DIR "\\SpinH.bin",		80,	 ARGB4,	NEAREST,    80,	   480,	  160,	294	}, // 75
	{TEST_DIR "\\WBarH.bin",		94,	RGB565,	NEAREST,   664,	   94 ,	  1328,	369	}, //122
	{TEST_DIR "\\bubH.bin",		54,	 ARGB4,	NEAREST,    54,	   54 ,	  108,	491	}, //6
	{TEST_DIR "\\homeH.bin",		70,	 ARGB4,	NEAREST,    70,	   70 ,	  140,	497	}, //10
	{TEST_DIR "\\BkgdH.bin",	   240,    L8,	NEAREST,   800,	  480 ,	  400,	591	}, //94
};
#elif DISPLAY_RESOLUTION_HVGA_PORTRAIT
#else
#error Display resolution is not supported
#endif

uint8_t Stflag, OptionsSet, MainFlag, Tagval, Pf = 0;
uint16_t BrightNess, SoundLev = 255, TimeSet, IterCount, Px;
#ifdef DISPLAY_RESOLUTION_WQVGA
static uint16_t ButX = 330;
#elif DISPLAY_RESOLUTION_WVGA
static uint16_t ButX = 592;
#elif DISPLAY_RESOLUTION_HVGA_PORTRAIT
static uint16_t ButX = 30;
#endif

#ifdef DISPLAY_RESOLUTION_HVGA_PORTRAIT
SAMAPP_Logo_Img_t Main_Icons[10] =
{
	{TEST_DIR "\\shirt.bin",		 40,    ARGB4,  NEAREST,    40,     40 ,    80,   0L      },
	{TEST_DIR "\\Temp.bin",		 40,    ARGB4,  NEAREST,    40,     40 ,    80,   3L      },
	{TEST_DIR "\\SpinW.bin",        40,    ARGB4,  NEAREST,    40,     40 ,    80,   7L      },
	{TEST_DIR "\\Butfly.bin",	     20,    ARGB4,  NEAREST,    20,     20 ,    40,   11L     },
	{TEST_DIR "\\Lock.bin",		 30,    ARGB4,  NEAREST,    30,     30 ,    60,   12L     },
	{TEST_DIR "\\UnLock.bin",		 30,    ARGB4,  NEAREST,    30,     60 ,    60,   14L     },
	{TEST_DIR "\\Set.bin",		     30,    ARGB4,  NEAREST,    30,     30 ,    60,   16L     },
	{TEST_DIR "\\Start.bin",		 30,    ARGB4,  NEAREST,    30,     30 ,    60,   18L     },
	{TEST_DIR "\\home.bin",		 40,    ARGB4,  NEAREST,    40,     40 ,    80,   20L     },//4kb
	{TEST_DIR "\\Bkgd.bin",        136,    L8,     NEAREST,    320,   480 ,   240,   24L     },
},

Wash_Window[7] =
{
	{TEST_DIR "\\Wash.bin",		55,	 ARGB4,	NEAREST,    50,	   50 ,	  100,	 57L	}, // size 22KB
	{TEST_DIR "\\Rinse.bin",		54,	 ARGB4,	NEAREST,    54,	   54 ,	  108,	 79L	}, // size 24KB
	{TEST_DIR "\\Spin.bin",		54,	 ARGB4,	NEAREST,    45,	   45 ,	   90,	103L	}, // size 30KB
	{TEST_DIR "\\WBar.bin",		52,	RGB565,	NEAREST,   320,	  400 ,	  800,	133L	}, //size 42KB
	{TEST_DIR "\\bub.bin",		    30,	 ARGB4,	NEAREST,    30,	   30 ,	   60,	175L	},//2kb
	{TEST_DIR "\\home.bin",		40,	 ARGB4,	NEAREST,    40,	   40 ,	   80,	177L	},//4kb
	{TEST_DIR "\\Bkgd.bin",		136,    L8,	NEAREST,   320,	  480 ,	  240,	181L	},   //33kb
};

#ifdef FT9XX_PLATFORM
SAMAPP_Logo_Img_t Wash_Window[7] =
#else
PROGMEM	SAMAPP_Logo_Img_t  Wash_Window[7] =
#endif
{
	{TEST_DIR "\\WashH.bin",		88,	 ARGB4,	NEAREST,    88,	   352,	  176,	161	}, // 61
	{TEST_DIR "\\RinseH.bin",		96,	 ARGB4,	NEAREST,    96,	   384,	  192,	222	}, // 72
	{TEST_DIR "\\SpinH.bin",		80,	 ARGB4,	NEAREST,    80,	   480,	  160,	294	}, // 75
	{TEST_DIR "\\WBarH.bin",		94,	RGB565,	NEAREST,   664,	   94 ,	  1328,	369	}, //122
	{TEST_DIR "\\bubH.bin",		54,	 ARGB4,	NEAREST,    54,	   54 ,	  108,	491	}, //6
	{TEST_DIR "\\homeH.bin",		70,	 ARGB4,	NEAREST,    70,	   70 ,	  140,	497	}, //10
	{TEST_DIR "\\BkgdH.bin",	   240,    L8,	NEAREST,   800,	  480 ,	  400,	591	}, //94
};

#endif

void Logo_Intial_setup(SAMAPP_Logo_Img_t sptr[], uint8_t num)
{
	uint8_t z;

	for (z = 0; z < num; z++)
	{
		Gpu_Hal_LoadImageToMemory(s_pHalContext, sptr[z].name, sptr[z].gram_address * 1024, INFLATE);
	}

	EVE_CoCmd_dlStart(s_pHalContext);        // start
	EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1));
	for (z = 0; z < num; z++)
	{
		EVE_Cmd_wr32(s_pHalContext, BITMAP_HANDLE(z));
		EVE_Cmd_wr32(s_pHalContext, BITMAP_SOURCE(sptr[z].gram_address * 1024L));
		EVE_Cmd_wr32(s_pHalContext, BITMAP_LAYOUT(sptr[z].image_format, sptr[z].linestride, sptr[z].image_height));
		EVE_Cmd_wr32(s_pHalContext, BITMAP_SIZE(sptr[z].filter, BORDER, BORDER, sptr[z].sizex, sptr[z].sizey));
#if defined(DISPLAY_RESOLUTION_WVGA)
		EVE_Cmd_wr32(s_pHalContext, BITMAP_LAYOUT_H(sptr[z].linestride >> 10, sptr[z].image_height >> 9));
		EVE_Cmd_wr32(s_pHalContext, BITMAP_SIZE_H(sptr[z].sizex >> 9, sptr[z].sizey >> 9));
#endif

	}
	EVE_Cmd_wr32(s_pHalContext, DISPLAY());
	EVE_CoCmd_swap(s_pHalContext);
	EVE_Cmd_waitFlush(s_pHalContext);
}

static void rotate_around(int16_t x, int16_t y, int16_t a)
{
	EVE_CoCmd_loadIdentity(s_pHalContext);
	EVE_CoCmd_translate(s_pHalContext, F16(x), F16(y));
	EVE_CoCmd_rotate(s_pHalContext, a);
	EVE_CoCmd_translate(s_pHalContext, F16(-x), F16(-y));
	EVE_CoCmd_setMatrix(s_pHalContext);
}

static char *Array_Cycle_Name[9] = { 0,"Eco Cold", "Normal", "Heavy","PermPress",  "Wool", "Bedding", "ActiveWear" };
char *Array_Cycle_Options[3] = { "Soil Level", "Temperature", "Spin Speed" };
char *Array_Menu_Options[3] = { "Child Lock", "Settings", "Start" };

char *BrightLevel[6] = { 0,"Level1", "Level2", "Level3", "Level4", "Level5" };

char *SoundLevel[6] = { 0,"Mute", "Level1", "Level2", "Level3", "Level4" };

typedef struct Item_Prop
{
	char *SoilLev;
	char *Temp;
	char *SpinSpeed;
}Item_Properties;

Item_Properties Item_Property[] =
{
		{"Light",  "Cold", "Low"},//Eco cold
		{"Normal", "Warm", "Medium"}, //Normal
		{"Heavy",  "Hot",  "High"}, // HEavy Duty
		{"Light",  "Warm", "Medium"},//Perm Press
		{"Normal", "Hot",  "Medium"},//Active Wear
		{"Heavy",  "Warm", "High"},//Bedding
		{"Heavy",  "Cold", "Low"},//Wool
};

typedef struct Bub
{
	int16_t xOffset;
	int16_t yOffset;
	char8_t xDiff;
	char8_t yDiff;
}Bubbles;

static uint8_t ProcFinish;
uint16_t WashWindow()
{
	uint16_t BitmapTr = 0, BubY = 0, i = 0, th = 0, PwY = 0, PwY1 = 0, PwY2 = 0;
	int16_t RotX = 0, RotY = 0;
	uint32_t ItCt = 0;

#if defined(DISPLAY_RESOLUTION_WQVGA)
	uint16_t pauseX = 205, pauseY = 190, progressCoverX2 = 435, progressCoverY1 = 134, progressCoverY2 = 178, processImageCX = 225, processImageCY = 85, processImageCSz = 45, cycleStringX = 40, cycleStringY = 190, cycleDoneX = 197, cycleDoneY = 73, washCycleImgX = 200, washSpinImgY = 60;
	uint8_t cycleFont = 23, PwYLimit = 61;
#elif defined(DISPLAY_RESOLUTION_WVGA)
	uint16_t pauseX = 372, pauseY = 372, progressCoverX2 = 737, progressCoverY1 = 237, progressCoverY2 = 323, processImageCX = 374, processImageCY = 142, processImageCSz = 80, cycleStringX = 72, cycleStringY = 340, cycleDoneX = 340, cycleDoneY = 130, washCycleImgX = 330, washSpinImgY = 99;
	uint8_t cycleFont = 29, PwYLimit = 85;
#endif

#ifdef DISPLAY_RESOLUTION_HVGA_PORTRAIT
	uint16_t pauseX = 160, pauseY = 280, progressCoverX2 = 320, progressCoverY1 = 134, progressCoverY2 = 178, processImageCX = 225, processImageCY = 85, processImageCSz = 45, cycleStringX = 40, cycleStringY = 190, cycleDoneX = 197, cycleDoneY = 73, washCycleImgX = 200, washSpinImgY = 60;
	uint8_t cycleFont = 23, PwYLimit = 61;
#endif
	static uint8_t ProcessFlag;
	uint8_t Read_tag = 0, Sdflag = 0, Sd1 = 0, Sd2 = 0, SoundFlag = 0;

	Bubbles W_Bubble[40];

	Logo_Intial_setup(Wash_Window, 7);

	if (MainFlag == 1)
	{
		TimeSet = 36;
#if defined(DISPLAY_RESOLUTION_WQVGA)
		Px = 44;
#elif defined(DISPLAY_RESOLUTION_WVGA)
		Px = 76;
#endif
#ifdef DISPLAY_RESOLUTION_HVGA_PORTRAIT
		Px = 44;
#endif

	}
	BubY = 110;

	for (i = 0; i < 40; i++)
	{
		W_Bubble[i].xOffset = random(s_pHalContext->Width);
		W_Bubble[i].yOffset = random(s_pHalContext->Height);

		W_Bubble[i].xDiff = random(16);
		W_Bubble[i].yDiff = random(8);
	}

	do
	{
		EVE_CoCmd_dlStart(s_pHalContext);        // start
		EVE_Cmd_wr32(s_pHalContext, CLEAR_COLOR_RGB(0, 0, 255));
		EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1));

		Read_tag = Gesture_GetTag(s_pHalContext);
		keypressed = EVE_Hal_rd8(s_pHalContext, REG_TOUCH_TAG);

		/* Draw all the background bitmaps - balls with various resolutions */
		if (ProcessFlag == 2)
		{
			EVE_Cmd_wr32(s_pHalContext, BEGIN(BITMAPS));
			EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 255, 255)); // brown outside circle
			if (Pf == 0)
			{
				for (i = 0; i < 25; i++)
				{
					EVE_Cmd_wr32(s_pHalContext, BITMAP_HANDLE(4));
					EVE_Cmd_wr32(s_pHalContext, VERTEX2F(W_Bubble[i].xOffset * 16, W_Bubble[i].yOffset * 16));

					W_Bubble[i].xOffset += W_Bubble[i].xDiff;
					W_Bubble[i].yOffset += W_Bubble[i].yDiff;

					//restrict the position of the background bitmaps to the visible region
					if (W_Bubble[i].xOffset < -80)
					{
						W_Bubble[i].xOffset = s_pHalContext->Width + 80;
					}
					else if (W_Bubble[i].xOffset > s_pHalContext->Width + 80)
					{
						W_Bubble[i].xOffset = -80;
					}

					if (W_Bubble[i].yOffset < -80)
					{
						W_Bubble[i].yOffset = s_pHalContext->Height + 80;
					}
					else if (W_Bubble[i].yOffset > s_pHalContext->Height + 80)
					{
						W_Bubble[i].yOffset = -80;
					}
				}
			}
			W_Bubble[random(40)].xDiff = random(16) - 6;
			W_Bubble[random(40)].yDiff = random(8) - 4;
		}
		// Progress bar Image
		EVE_Cmd_wr32(s_pHalContext, BEGIN(BITMAPS));
		EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 255, 255));

		EVE_Cmd_wr32(s_pHalContext, COLOR_A(180));
		EVE_Cmd_wr32(s_pHalContext, BITMAP_TRANSFORM_A(128));
		EVE_Cmd_wr32(s_pHalContext, BITMAP_TRANSFORM_E(128));
		EVE_Cmd_wr32(s_pHalContext, VERTEX2II(0, 0, 6, 0));// backgnd tag -13

		EVE_Cmd_wr32(s_pHalContext, RESTORE_CONTEXT());
#ifdef DISPLAY_RESOLUTION_WQVGA
		EVE_Cmd_wr32(s_pHalContext, VERTEX2II(40, 130, 3, 0));
#elif DISPLAY_RESOLUTION_WVGA
		EVE_Cmd_wr32(s_pHalContext, VERTEX2II(75, 233, 3, 0));
#endif
#ifdef DISPLAY_RESOLUTION_HVGA_PORTRAIT
		EVE_Cmd_wr32(s_pHalContext, VERTEX2II(0, 130, 3, 0));
#endif
		// Pause Button

		EVE_Cmd_wr32(s_pHalContext, TAG_MASK(1));
		EVE_Cmd_wr32(s_pHalContext, TAG('P'));
		if (ItCt % 100 > 0 && ItCt % 100 < 50 && Pf == 1 && ProcessFlag < 5)
			EVE_CoCmd_text(s_pHalContext, pauseX, pauseY, 31, 0, "I I");
		else if (Pf == 0 && ProcessFlag < 5)
			EVE_CoCmd_text(s_pHalContext, pauseX, pauseY, 31, 0, "I I");

		EVE_Cmd_wr32(s_pHalContext, TAG_MASK(0));

		if (Read_tag == 'P')
		{
			if (Pf == 1)Pf = 0;
			else if (Pf == 0)	Pf = 1;
		}

		//Progress bar cover up
		if (Px < progressCoverX2)
		{
			EVE_Cmd_wr32(s_pHalContext, BEGIN(RECTS));
			EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(129, 178, 218));
			EVE_Cmd_wr32(s_pHalContext, LINE_WIDTH(5 * 16));
			EVE_Cmd_wr32(s_pHalContext, VERTEX2F(Px * 16, progressCoverY1 * 16));
			EVE_Cmd_wr32(s_pHalContext, VERTEX2F(progressCoverX2 * 16, progressCoverY2 * 16));

		}
		// Circle behind Process images
		EVE_Cmd_wr32(s_pHalContext, BEGIN(FTPOINTS));
		EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 255, 255));
		EVE_Cmd_wr32(s_pHalContext, COLOR_A(100));
		EVE_Cmd_wr32(s_pHalContext, POINT_SIZE(processImageCSz * 16));
		EVE_Cmd_wr32(s_pHalContext, VERTEX2F(processImageCX * 16, processImageCY * 16));

		// Wash image
		EVE_Cmd_wr32(s_pHalContext, COLOR_A(255));
		EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 255, 255));

		if (Pf == 0)// if Pause button not set
		{
			if (TimeSet > 30)
			{
				ProcessFlag = 1;
				EVE_CoCmd_text(s_pHalContext, cycleStringX, cycleStringY, cycleFont, 0, "Pre Wash");
				ProcFinish = 1;
			}
			else if (TimeSet > 20 && TimeSet <= 30)
			{
				EVE_Hal_wr8(s_pHalContext, REG_VOL_PB, 0);
				if (Sdflag == 0)
				{
					Play_Sound(s_pHalContext, 0x56, SoundLev, 0);
					Sdflag = 1;
				}
				ProcessFlag = 2;
				EVE_CoCmd_text(s_pHalContext, cycleStringX, cycleStringY, cycleFont, 0, "Washing");
				ProcFinish = 1;
			}
			else if (TimeSet <= 20 && TimeSet > 10)
			{
				if (Sd1 == 0)
				{
					Play_Sound(s_pHalContext, 0x56, SoundLev, 0);
					Sd1 = 1;
				}
				ProcessFlag = 3;
				EVE_CoCmd_text(s_pHalContext, cycleStringX, cycleStringY, cycleFont, 0, "Rinsing");
				ProcFinish = 1;
			}
			else if (TimeSet <= 10 && TimeSet > 0)
			{
				if (Sd2 == 0)
				{
					Play_Sound(s_pHalContext, 0x56, SoundLev, 0);
					Sd2 = 1;
				}
				ProcessFlag = 4;
				EVE_CoCmd_text(s_pHalContext, cycleStringX, cycleStringY, cycleFont, 0, "Spinning");
				ProcFinish = 1;
			}
			else if (TimeSet == 0)
			{
				ProcessFlag = 5;
				EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 0, 0));
				if (ItCt % 20 > 0 && ItCt % 20 < 10)
					EVE_CoCmd_text(s_pHalContext, cycleDoneX, cycleDoneY, cycleFont, 0, "DONE");

				MainFlag = 0;
				ProcFinish = 2;
				Stflag = 0;
#if defined(DISPLAY_RESOLUTION_WQVGA)
				ButX = 330;
#elif defined(DISPLAY_RESOLUTION_WVGA)
				ButX = 592;
#endif
#if defined(DISPLAY_RESOLUTION_HVGA_PORTRAIT)
				ButX = 300;
#endif
			}

			{
				if (ItCt % 25 == 0)
					BitmapTr = 0;
				else	if (ItCt % 40 == 0)
					BitmapTr = 1;
				else	if (ItCt % 60 == 0)
					BitmapTr = 2;
				else	if (ItCt % 90 == 0)
					BitmapTr = 3;
			}
		}
		// Process Images

		EVE_Cmd_wr32(s_pHalContext, BEGIN(BITMAPS));
		EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 255, 255));
		// Pre Wash Image
		if (ProcessFlag == 1)// Images loading during pre wash stage
		{
			EVE_Cmd_wr32(s_pHalContext, VERTEX2II(washCycleImgX, PwY, 1, 0));
			if (PwY == PwYLimit)
			{
				EVE_Cmd_wr32(s_pHalContext, VERTEX2II(washCycleImgX, PwY1, 0, 0));
			}
			if (PwY1 == PwYLimit)
			{
				EVE_Cmd_wr32(s_pHalContext, VERTEX2II(washCycleImgX, PwY2, 2, 0));
			}

		}
		else if (ProcessFlag < 5)
		{
			if (ProcessFlag == 2 || ProcessFlag == 3)// washing and rinsing images
			{
#if defined(DISPLAY_RESOLUTION_WQVGA)
				RotX = 27; RotY = 27;
#elif defined(DISPLAY_RESOLUTION_WVGA)
				RotX = 48; RotY = 48;
#endif
#if defined(DISPLAY_RESOLUTION_HVGA_PORTRAIT)
				RotX = 27; RotY = 27;
#endif
				EVE_Cmd_wr32(s_pHalContext, CMD_LOADIDENTITY);
				if (Pf == 0)
					rotate_around(RotX, RotY, th);
				EVE_Cmd_wr32(s_pHalContext, BITMAP_HANDLE(ProcessFlag - 2));
				EVE_Cmd_wr32(s_pHalContext, CELL(BitmapTr));
				EVE_Cmd_wr32(s_pHalContext, VERTEX2F(washCycleImgX * 16, washSpinImgY * 16));
				EVE_Cmd_wr32(s_pHalContext, RESTORE_CONTEXT());
			}

			else if (ProcessFlag == 4)// Spinning images
			{
				EVE_Cmd_wr32(s_pHalContext, BITMAP_HANDLE(ProcessFlag - 2));
				EVE_Cmd_wr32(s_pHalContext, CELL(BitmapTr));
				EVE_Cmd_wr32(s_pHalContext, VERTEX2F(washCycleImgX * 16, washSpinImgY * 16));

			}
		}

		// Home Button
		EVE_Cmd_wr32(s_pHalContext, TAG_MASK(1));
		EVE_Cmd_wr32(s_pHalContext, TAG('H'));
#if defined(DISPLAY_RESOLUTION_WQVGA)
		EVE_Cmd_wr32(s_pHalContext, VERTEX2II(430, 10, 5, 0));
#elif defined(DISPLAY_RESOLUTION_WVGA)
		EVE_Cmd_wr32(s_pHalContext, BITMAP_HANDLE(5));
		EVE_Cmd_wr32(s_pHalContext, CELL(0));
		EVE_Cmd_wr32(s_pHalContext, VERTEX2F((s_pHalContext->Width - Wash_Window[5].sizex - 16) * 16, 16));
#endif
#if defined(DISPLAY_RESOLUTION_HVGA_PORTRAIT)
		EVE_Cmd_wr32(s_pHalContext, VERTEX2II(270, 10, 5, 0));
#endif
		//fix the home button on the top right corner, position is the image width plus some space to the right
		EVE_Cmd_wr32(s_pHalContext, TAG_MASK(0));

		// Time remaining
#if defined(DISPLAY_RESOLUTION_WQVGA)
		EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 255, 255));
		EVE_CoCmd_number(s_pHalContext, 40, 60, 31, OPT_CENTERX | OPT_CENTERY, 0);
		EVE_CoCmd_number(s_pHalContext, 60, 60, 31, OPT_CENTERX | OPT_CENTERY, 0);
		EVE_CoCmd_text(s_pHalContext, 75, 67, 21, OPT_CENTERX | OPT_CENTERY, "H");
		EVE_CoCmd_text(s_pHalContext, 85, 57, 31, OPT_CENTERX | OPT_CENTERY, ":");

		if (TimeSet < 10)
		{
			EVE_CoCmd_number(s_pHalContext, 105, 60, 31, OPT_CENTERX | OPT_CENTERY, 0);
			EVE_CoCmd_number(s_pHalContext, 128, 60, 31, OPT_CENTERX | OPT_CENTERY, TimeSet);
		}
		else {
			EVE_CoCmd_number(s_pHalContext, 115, 60, 31, OPT_CENTERX | OPT_CENTERY, TimeSet);
		}

		EVE_CoCmd_text(s_pHalContext, 150, 67, 21, OPT_CENTERX | OPT_CENTERY, "Min");

		EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 255, 255));
		EVE_CoCmd_text(s_pHalContext, 300, 190, 23, 0, "Est. Finish Time");

		EVE_CoCmd_number(s_pHalContext, 315, 212, 25, 0, 0);
		EVE_CoCmd_number(s_pHalContext, 335, 212, 25, 0, 0);
		EVE_CoCmd_text(s_pHalContext, 355, 210, 25, 0, ":");
		if (TimeSet < 10)
		{
			EVE_CoCmd_number(s_pHalContext, 370, 212, 25, 0, 0);
			EVE_CoCmd_number(s_pHalContext, 390, 212, 25, 0, TimeSet);
			EVE_CoCmd_number(s_pHalContext, 390, 212, 25, 0, TimeSet);
		}
		else {
			EVE_CoCmd_number(s_pHalContext, 370, 212, 25, 0, TimeSet);
		}
#elif defined(DISPLAY_RESOLUTION_WVGA)
		EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 255, 255));
		EVE_CoCmd_number(s_pHalContext, 74, 122, 31, OPT_CENTERX | OPT_CENTERY, 0);
		EVE_CoCmd_number(s_pHalContext, 97, 122, 31, OPT_CENTERX | OPT_CENTERY, 0);
		EVE_CoCmd_text(s_pHalContext, 115, 130, 28, OPT_CENTERX | OPT_CENTERY, "H");
		EVE_CoCmd_text(s_pHalContext, 130, 118, 31, OPT_CENTERX | OPT_CENTERY, ":");

		if (TimeSet < 10)
		{
			EVE_CoCmd_number(s_pHalContext, 155, 122, 31, OPT_CENTERX | OPT_CENTERY, 0);
			EVE_CoCmd_number(s_pHalContext, 179, 123, 31, OPT_CENTERX | OPT_CENTERY, TimeSet);
		}
		else {
			EVE_CoCmd_number(s_pHalContext, 167, 123, 31, OPT_CENTERX | OPT_CENTERY, TimeSet);
		}

		EVE_CoCmd_text(s_pHalContext, 209, 131, 28, OPT_CENTERX | OPT_CENTERY, "Min");

		EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 255, 255));
		EVE_CoCmd_text(s_pHalContext, 598, 355, 28, OPT_CENTERX | OPT_CENTERY, "Est. Finish Time");

		EVE_CoCmd_number(s_pHalContext, 552, 395, 31, OPT_CENTERX | OPT_CENTERY, 0);
		EVE_CoCmd_number(s_pHalContext, 576, 395, 31, OPT_CENTERX | OPT_CENTERY, 0);
		EVE_CoCmd_text(s_pHalContext, 601, 391, 31, OPT_CENTERX | OPT_CENTERY, ":");
		if (TimeSet < 10)
		{
			EVE_CoCmd_number(s_pHalContext, 629, 395, 31, OPT_CENTERX | OPT_CENTERY, 0);
			EVE_CoCmd_number(s_pHalContext, 654, 395, 31, OPT_CENTERX | OPT_CENTERY, TimeSet);
		}
		else {
			EVE_CoCmd_number(s_pHalContext, 642, 395, 31, OPT_CENTERX | OPT_CENTERY, TimeSet);
		}
#endif

#if defined(DISPLAY_RESOLUTION_HVGA_PORTRAIT)
		EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 255, 255));
		EVE_CoCmd_number(s_pHalContext, 40, 60, 31, OPT_CENTERX | OPT_CENTERY, 0);
		EVE_CoCmd_number(s_pHalContext, 60, 60, 31, OPT_CENTERX | OPT_CENTERY, 0);
		EVE_CoCmd_text(s_pHalContext, 75, 67, 21, OPT_CENTERX | OPT_CENTERY, "H");
		EVE_CoCmd_text(s_pHalContext, 85, 57, 31, OPT_CENTERX | OPT_CENTERY, ":");

		if (TimeSet < 10)
		{
			EVE_CoCmd_number(s_pHalContext, 105, 60, 31, OPT_CENTERX | OPT_CENTERY, 0);
			EVE_CoCmd_number(s_pHalContext, 128, 60, 31, OPT_CENTERX | OPT_CENTERY, TimeSet);
		}
		else {
			EVE_CoCmd_number(s_pHalContext, 115, 60, 31, OPT_CENTERX | OPT_CENTERY, TimeSet);
		}

		EVE_CoCmd_text(s_pHalContext, 150, 67, 21, OPT_CENTERX | OPT_CENTERY, "Min");

		EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 255, 255));
		EVE_CoCmd_text(s_pHalContext, 170, 190, 23, 0, "Est. Finish Time");

		EVE_CoCmd_number(s_pHalContext, 185, 212, 25, 0, 0);
		EVE_CoCmd_number(s_pHalContext, 200, 212, 25, 0, 0);
		EVE_CoCmd_text(s_pHalContext, 217, 210, 25, 0, ":");
		if (TimeSet < 10)
		{
			EVE_CoCmd_number(s_pHalContext, 230, 212, 25, 0, 0);
			EVE_CoCmd_number(s_pHalContext, 245, 212, 25, 0, TimeSet);
		}
		else {
			EVE_CoCmd_number(s_pHalContext, 230, 212, 25, 0, TimeSet);
		}
#endif
		// play sound during every stage of process
		if (TimeSet % 10 != 0)
		{
			if (TimeSet % 2 == 0)
			{
				if (SoundFlag == 0)
				{
					Play_Sound(s_pHalContext, 0x50, SoundLev, 0);
					SoundFlag = 1;
				}
			}
			else
				SoundFlag = 0;
		}

		EVE_Cmd_wr32(s_pHalContext, END());
		EVE_Cmd_wr32(s_pHalContext, DISPLAY());
		EVE_CoCmd_swap(s_pHalContext);
		EVE_Cmd_waitFlush(s_pHalContext);

		ItCt += 1;
		if (ItCt % 50 == 0 && Pf == 0)	TimeSet--;
		if (TimeSet > 60)	TimeSet = 0;
		// Process rectangle cover up co ordinates
		if (IterCount % 50 == 0 && Pf == 0)
#ifdef DISPLAY_RESOLUTION_WQVGA
			Px += 11;
#elif DISPLAY_RESOLUTION_WVGA
			Px += 19;

#endif
#ifdef DISPLAY_RESOLUTION_HVGA_PORTRAIT
		Px += 11;
#endif
		//Butterfly Line
		if (IterCount % 15 == 0 && Pf == 0) {
			ButX += 1;
		}

		// Rinsing process - image roation
		if (ProcessFlag == 3)
		{
			if ((ItCt % 100 > 0) && (ItCt % 100 < 50))
				th -= 1000;
			if ((ItCt % 100 >= 50) && (ItCt % 100 < 100))
				th += 1000;
		}

		// Images loading during pre wash stage
		if (PwY < PwYLimit)PwY += 1;
		if (PwY1 < PwYLimit && PwY == PwYLimit)PwY1 += 1;
		if (PwY2 < PwYLimit && PwY1 == PwYLimit)PwY2 += 1;
		IterCount += 1;

	} while (Read_tag != 'H');
	OptionsSet = 1;

	return  Read_tag;
}

uint8_t Gpu_Rom_Font_WH(uint8_t Char, uint8_t font)
{
	uint32_t ptr, Wptr;
	uint8_t Width = 0;
	ptr = EVE_Hal_rd32(s_pHalContext, ROMFONT_TABLEADDRESS);

	// read Width of the character
	Wptr = (ptr + (148L * (font - 16L))) + Char;	// (table starts at font 16)
	Width = EVE_Hal_rd8(s_pHalContext, Wptr);
	return Width;
}

uint8_t fontPixelHeight(uint8_t font) {
	uint32_t ptr, hPtr;
	uint8_t height = 0;
	ptr = EVE_Hal_rd32(s_pHalContext, ROMFONT_TABLEADDRESS);

	// the height is at the 140th byte
	hPtr = (ptr + (148 * (font - 16))) + 140;	// (table starts at font 16)
	height = EVE_Hal_rd8(s_pHalContext, hPtr);
	return height;
}

uint16_t stringPixelWidth(const uchar8_t* text, uint8_t font) {
	char8_t tempChar;
	uint16_t length = 0, index;
	if (text == NULL) {
		return 0;
	}

	if (text[0] == '\0') {
		return 0;
	}

	index = 0;
	tempChar = text[index];
	while (tempChar != '\0') {
		length += Gpu_Rom_Font_WH(tempChar, font);
		tempChar = text[++index];
	}

	return length;
}

static uint8_t BrightKey, VolKey;

uint8_t Settings()
{
	//new
#ifdef DISPLAY_RESOLUTION_WQVGA
	uint16_t SettingBlockHeight = 30, SettingBlockWidth = 50, SettingBlockX = 115, SettingBlockY = 25, SettingBlockGap = 10;
	static uint16_t SettingBackgroundHeight = 215, DisplayXPos = 60, DisplayYPos = 80, SoundXPos = 60, SoundYPos = 145;
	uint8_t TextFont = 23, SettingFont = 18;
#elif DISPLAY_RESOLUTION_WVGA
	uint16_t SettingBlockHeight = 50, SettingBlockWidth = 80, SettingBlockX = 200, SettingBlockY = 100, SettingBlockGap = 10;
	static uint16_t SettingBackgroundHeight, DisplayXPos = 100, DisplayYPos = 100, SoundXPos = 100, SoundYPos = 200;
	uint8_t TextFont = 30, SettingFont = 23;
#endif
#ifdef DISPLAY_RESOLUTION_HVGA_PORTRAIT
	uint16_t SettingBlockHeight = 30, SettingBlockWidth = 50, SettingBlockX = 30, SettingBlockY = 35, SettingBlockGap = 10;
	static uint16_t SettingBackgroundHeight = 400, DisplayXPos = 80, DisplayYPos = 80, SoundXPos = 180, SoundYPos = 80;
	uint8_t TextFont = 23, SettingFont = 18;
#endif

	static uint16_t SettingBackgroundWidth;
	static uint8_t firstTime = 1;
	uint8_t Read_tag = 0, i = 0, SoundFlag = 0;
	uint16_t Rx1 = 0, Ry1 = 0, Rx2 = 0, Ry2 = 0, startingX, startingY;
	char buffer[10];

	if (firstTime) {
		SettingBackgroundWidth = SettingBlockWidth * 8 + SettingBlockGap * 6;;   //5 setting blocks and 6 gaps between the setting blocks
		SettingBackgroundHeight = 380;
		DisplayYPos = SettingBlockY + 3 * SettingBlockHeight / 2 + SettingBlockGap * 2;  //align the "Display:" text with the setting blocks
		SoundYPos = SettingBlockY + 7 * SettingBlockHeight / 2 + SettingBlockGap * 4; //align the "Sound:" text with the setting blocks.
	}

	if (BrightKey == 0)BrightKey = 5;
	if (VolKey == 0)VolKey = 10;
	do {
		Read_tag = Gesture_GetTag(s_pHalContext);
		keypressed = EVE_Hal_rd8(s_pHalContext, REG_TOUCH_TAG);
		EVE_CoCmd_dlStart(s_pHalContext);

		EVE_Cmd_wr32(s_pHalContext, CLEAR_COLOR_RGB(0, 0, 255));
		EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1));

		EVE_Cmd_wr32(s_pHalContext, SAVE_CONTEXT());
		EVE_Cmd_wr32(s_pHalContext, BEGIN(BITMAPS));
		EVE_Cmd_wr32(s_pHalContext, COLOR_A(180));

		EVE_Cmd_wr32(s_pHalContext, BITMAP_TRANSFORM_A(128));
		EVE_Cmd_wr32(s_pHalContext, BITMAP_TRANSFORM_E(128));

		EVE_Cmd_wr32(s_pHalContext, VERTEX2II(0, 0, 9, 0));// backgnd tag -8

		EVE_Cmd_wr32(s_pHalContext, RESTORE_CONTEXT());

		EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0, 200, 255));

		EVE_Cmd_wr32(s_pHalContext, TAG_MASK(1));
		EVE_Cmd_wr32(s_pHalContext, TAG('V'));
		EVE_CoCmd_text(s_pHalContext, DisplayXPos, DisplayYPos, TextFont, OPT_CENTERX | OPT_CENTERY, "Display:");

		EVE_Cmd_wr32(s_pHalContext, TAG('B'));
		EVE_CoCmd_text(s_pHalContext, SoundXPos, SoundYPos, TextFont, OPT_CENTERX | OPT_CENTERY, "Sound:");

		EVE_Cmd_wr32(s_pHalContext, TAG_MASK(0));

		//rectangle enclosing the selectable setting blocks
		EVE_Cmd_wr32(s_pHalContext, BEGIN(RECTS));
		EVE_Cmd_wr32(s_pHalContext, LINE_WIDTH(16));
		EVE_Cmd_wr32(s_pHalContext, COLOR_A(100));
		EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 255, 255));
		EVE_Cmd_wr32(s_pHalContext, VERTEX2F(SettingBlockX * 16, SettingBlockY * 16));
		EVE_Cmd_wr32(s_pHalContext, VERTEX2F(SettingBackgroundWidth * 16, SettingBackgroundHeight * 16));

		EVE_Cmd_wr32(s_pHalContext, COLOR_A(255));
		EVE_Cmd_wr32(s_pHalContext, LINE_WIDTH(4 * 16));

		EVE_Cmd_wr32(s_pHalContext, TAG_MASK(1));

		// Brightness
		startingX = SettingBlockX + SettingBlockGap;
		startingY = SettingBlockY + SettingBlockHeight + SettingBlockGap * 2;
		for (i = 1; i < 6; i++)
		{
			if (i == 1)EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(177, 176, 180));

			if (i == 2)EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(122, 154, 171));

			if (i == 3)EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(70, 149, 186));

			if (i == 4)EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(28, 140, 202));

			if (i == 5)EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(135, 183, 71));

			if (i == BrightKey)
			{
				EVE_Hal_wr8(s_pHalContext, REG_PWM_DUTY, BrightNess);
				EVE_Cmd_wr32(s_pHalContext, TAG(i));
				EVE_Cmd_wr32(s_pHalContext, VERTEX2F((startingX - 3) * 16, (startingY - 3) * 16));
				EVE_Cmd_wr32(s_pHalContext, VERTEX2F((startingX + SettingBlockWidth + 3) * 16, (startingY + SettingBlockHeight + 3) * 16));
			}
			else
			{
				EVE_Cmd_wr32(s_pHalContext, TAG(i));
				EVE_Cmd_wr32(s_pHalContext, VERTEX2F((startingX) * 16, (startingY) * 16));
				EVE_Cmd_wr32(s_pHalContext, VERTEX2F((startingX + SettingBlockWidth) * 16, (startingY + SettingBlockHeight) * 16));
			}
			startingX += (SettingBlockWidth + SettingBlockGap);
		}

		startingX = SettingBlockX + SettingBlockWidth / 2 + SettingBlockGap;
		startingY = SettingBlockY + 3 * SettingBlockHeight / 2 + SettingBlockGap * 2;
		EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0, 0, 0));

		for (i = 1; i < 6; i++)
		{
			EVE_Cmd_wr32(s_pHalContext, TAG(i));
			EVE_CoCmd_text(s_pHalContext, startingX, startingY, SettingFont, OPT_CENTERX | OPT_CENTERY, BrightLevel[i]);
			startingX += (SettingBlockWidth + SettingBlockGap);
		}

		EVE_Cmd_wr32(s_pHalContext, TAG_MASK(0));

		EVE_Cmd_wr32(s_pHalContext, END());

		// Volume

		EVE_Cmd_wr32(s_pHalContext, BEGIN(RECTS));
		EVE_Cmd_wr32(s_pHalContext, COLOR_A(255));
		EVE_Cmd_wr32(s_pHalContext, LINE_WIDTH(4 * 16));

		EVE_Cmd_wr32(s_pHalContext, TAG_MASK(1));

		startingX = SettingBlockX + SettingBlockGap;
		startingY = SettingBlockY + SettingBlockHeight * 3 + SettingBlockGap * 4;

		for (i = 1; i < 6; i++)
		{
			if (i == 1)EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(177, 176, 180));

			if (i == 2)EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(122, 154, 171));

			if (i == 3)EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(70, 149, 186));

			if (i == 4)EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(28, 140, 202));

			if (i == 5)EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(135, 183, 71));

			if (i == VolKey - 5)
			{
				if (SoundFlag == 0)
				{
					Play_Sound(s_pHalContext, 0x56, SoundLev, 0);
					SoundFlag = 1;
				}
				EVE_Cmd_wr32(s_pHalContext, TAG(i + 5));
				EVE_Cmd_wr32(s_pHalContext, VERTEX2F((startingX - 3) * 16, (startingY - 3) * 16));
				EVE_Cmd_wr32(s_pHalContext, VERTEX2F((startingX + SettingBlockWidth + 3) * 16, (startingY + SettingBlockHeight + 3) * 16));

			}
			else
			{
				EVE_Cmd_wr32(s_pHalContext, TAG(i + 5));
				EVE_Cmd_wr32(s_pHalContext, VERTEX2F((startingX) * 16, (startingY) * 16));
				EVE_Cmd_wr32(s_pHalContext, VERTEX2F((startingX + SettingBlockWidth) * 16, (startingY + SettingBlockHeight) * 16));

			}
			startingX += (SettingBlockWidth + SettingBlockGap);
		}

		startingX = SettingBlockX + SettingBlockGap + SettingBlockWidth / 2;
		startingY = SettingBlockY + 7 * SettingBlockHeight / 2 + SettingBlockGap * 4;

		EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0, 0, 0));

		for (i = 1; i < 6; i++)
		{
			EVE_Cmd_wr32(s_pHalContext, TAG(i + 5));
			EVE_CoCmd_text(s_pHalContext, startingX, startingY, SettingFont, OPT_CENTERX | OPT_CENTERY, SoundLevel[i]);
			startingX += (SettingBlockWidth + SettingBlockGap);
		}

		EVE_Cmd_wr32(s_pHalContext, TAG_MASK(0));
		EVE_Cmd_wr32(s_pHalContext, END());

		if ((Read_tag > 0) && (Read_tag < 6))
		{
			BrightKey = Read_tag;
		}
		if ((Read_tag > 5) && (Read_tag < 11))
		{
			VolKey = Read_tag;
			SoundFlag = 0;
		}
		if (Read_tag > 0 && Read_tag < 6)
			BrightNess = Read_tag * 128 / 5;
		if (Read_tag > 5 && Read_tag < 11)
			SoundLev = (Read_tag - 6) * 63;

		// Home Button
		EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 255, 255));
		EVE_Cmd_wr32(s_pHalContext, BEGIN(BITMAPS));
		EVE_Cmd_wr32(s_pHalContext, TAG_MASK(1));
		EVE_Cmd_wr32(s_pHalContext, TAG('H'));
#ifdef DISPLAY_RESOLUTION_WQVGA
		EVE_Cmd_wr32(s_pHalContext, VERTEX2II(s_pHalContext->Width - Main_Icons[8].sizex - 10, 10, 8, 0));
#elif DISPLAY_RESOLUTION_WVGA
		EVE_Cmd_wr32(s_pHalContext, BITMAP_HANDLE(8));
		EVE_Cmd_wr32(s_pHalContext, VERTEX2F((s_pHalContext->Width - Main_Icons[8].sizex - 10) * 16, 10 * 16));
#endif
#ifdef DISPLAY_RESOLUTION_HVGA_PORTRAIT
		EVE_Cmd_wr32(s_pHalContext, VERTEX2II(s_pHalContext->Width - Main_Icons[8].sizex - 10, 10, 8, 0));
#endif
		EVE_Cmd_wr32(s_pHalContext, TAG_MASK(0));

		EVE_Cmd_wr32(s_pHalContext, END());

		EVE_Cmd_wr32(s_pHalContext, DISPLAY());
		EVE_CoCmd_swap(s_pHalContext);
		EVE_Cmd_waitFlush(s_pHalContext);

	} while (Read_tag != 'H');

	return Read_tag;
}

static  uint8_t MenuFlag;
uint8_t MainWindow()
{
#if defined(DISPLAY_RESOLUTION_WQVGA) || defined(DISPLAY_RESOLUTION_QVGA)
	uint16_t scrollPaneXMark = 120, Ty = 0, washingTypeXoffset = 60, washingTypeTextFont = 23, washingTypeTextSpace = 50;
	uint16_t centerPaneX0 = 140, centerPaneY0 = 25, centerPaneX1 = 320, centerPaneY1 = 240, optionsTextX = 220, optionsTexty = 32, optionsTextFont = 23, centerPaneIconsYOffset = 60, centerPaneIconsXOffset = 150, centerPaneTextXOffset = 240, centerPaneTextFont = 18, centerPaneTextSpace = 60, centerPaneLineYOffset = 55, centerPaneLineX0 = 140, centerPaneLineX1 = 320, centerPaneLineSpacing = 60;
	uint16_t itemPropYOffset = 90, itemPropXOffset = 240, itemPropFont = 29, itemPropSpacing = 60, soilLevX = 150, soilLevY = 60, tempLevPointSz = 8, tempLevPointX = 170, tempLevPointY = 150, tempLevLineWidth = 1, tempLevLineX = 170, tempLevLineY0 = 145, tempLevLineY1 = 135, spinSpeedTextX = 150, spinSpeedTexty = 180;
#elif defined(DISPLAY_RESOLUTION_WVGA)
	uint16_t scrollPaneXMark = 250, Ty = 0, washingTypeXoffset = 118, washingTypeTextFont = 29, washingTypeTextSpace = 85;
	uint16_t centerPaneX0 = 250, centerPaneY0 = 45, centerPaneX1 = 550, centerPaneY1 = 425, optionsTextX = 370, optionsTexty = 60, optionsTextFont = 30, centerPaneIconsYOffset = 90, centerPaneIconsXOffset = 285, centerPaneTextXOffset = 427, centerPaneTextFont = 29, centerPaneTextSpace = 120, centerPaneLineYOffset = 80, centerPaneLineX0 = 250, centerPaneLineX1 = 540, centerPaneLineSpacing = 118;
	uint16_t itemPropYOffset = 140, itemPropXOffset = 427, itemPropFont = 30, itemPropSpacing = 118, soilLevX = 285, soilLevY = 90, tempLevPointSz = 8, tempLevPointX = 320, tempLevPointY = 265, tempLevLineWidth = 2, tempLevLineX = 320, tempLevLineY0 = 265, tempLevLineY1 = 235, spinSpeedTextX = 285, spinSpeedTexty = 334;
#endif

#if defined(DISPLAY_RESOLUTION_HVGA_PORTRAIT)
	uint16_t scrollPaneXMark = 300, Ty = 0, washingTypeXoffset = 60, washingTypeTextFont = 23, washingTypeTextSpace = 50;
	uint16_t centerPaneX0 = 140, centerPaneY0 = 25, centerPaneX1 = 320, centerPaneY1 = 240, optionsTextX = 220, optionsTexty = 32, optionsTextFont = 23, centerPaneIconsYOffset = 60, centerPaneIconsXOffset = 150, centerPaneTextXOffset = 240, centerPaneTextFont = 18, centerPaneTextSpace = 60, centerPaneLineYOffset = 55, centerPaneLineX0 = 140, centerPaneLineX1 = 320, centerPaneLineSpacing = 60;
	uint16_t itemPropYOffset = 90, itemPropXOffset = 240, itemPropFont = 29, itemPropSpacing = 60, soilLevX = 150, soilLevY = 60, tempLevPointSz = 8, tempLevPointX = 170, tempLevPointY = 150, tempLevLineWidth = 1, tempLevLineX = 170, tempLevLineY0 = 145, tempLevLineY1 = 135, spinSpeedTextX = 150, spinSpeedTexty = 180;
#endif
	uint16_t By = 0, Ly = 0, th = 0, Addth = 0;
	uint8_t PrevTag = 0, Read_Tag = 0, temp = 0, dg_count = 0, LockFlag = 0, PlayFlag = 0, Opt = 0, i = 0;
	int16_t sy = 0, drag = 0;

	static uint8_t j;
	Ty = 0;
#if defined(FT9XX_PLATFORM)
	scroller_init(160);
#else
	scroller_init(100);
#endif

	Logo_Intial_setup(Main_Icons, 10);

	do
	{
		EVE_Hal_wr8(s_pHalContext, REG_VOL_SOUND, 0);
		EVE_Hal_wr8(s_pHalContext, REG_VOL_PB, 0);

		sy = EVE_Hal_rd16(s_pHalContext, REG_TOUCH_SCREEN_XY);

		Read_Tag = Gesture_GetTag(s_pHalContext);
		keypressed = EVE_Hal_rd8(s_pHalContext, REG_TOUCH_TAG);

		drag = scroller_run(keypressed);

		EVE_CoCmd_dlStart(s_pHalContext);

		EVE_Cmd_wr32(s_pHalContext, CLEAR_COLOR_RGB(0, 0, 255));
		EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1));

		// BackGround Image
		EVE_Cmd_wr32(s_pHalContext, BEGIN(BITMAPS));

		EVE_Cmd_wr32(s_pHalContext, COLOR_A(180));
		EVE_Cmd_wr32(s_pHalContext, BITMAP_TRANSFORM_A(128));
		EVE_Cmd_wr32(s_pHalContext, BITMAP_TRANSFORM_E(128));

		EVE_Cmd_wr32(s_pHalContext, VERTEX2II(0, 0, 9, 0));// backgnd tag -8

		EVE_Cmd_wr32(s_pHalContext, RESTORE_CONTEXT());

		// Draw rectanglw with zero alpha value to have scrolling option on left pane
		EVE_Cmd_wr32(s_pHalContext, BEGIN(RECTS));
		EVE_Cmd_wr32(s_pHalContext, COLOR_A(0));
		EVE_Cmd_wr32(s_pHalContext, TAG_MASK(1));
		EVE_Cmd_wr32(s_pHalContext, TAG(101));
		EVE_Cmd_wr32(s_pHalContext, VERTEX2F(0 * 16, 0 * 16));
		EVE_Cmd_wr32(s_pHalContext, VERTEX2F(scrollPaneXMark * 16, s_pHalContext->Height * 16));

		EVE_Cmd_wr32(s_pHalContext, TAG_MASK(0));
		EVE_Cmd_wr32(s_pHalContext, END());
		EVE_Cmd_wr32(s_pHalContext, COLOR_A(255));

		Ty = 30;

		Ty = Ty - drag;
		// Options on the left pane

		//draw the washing type
		for (i = 1; i < 8; i++)
		{
			if (i == j)
			{
				EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 255, 255));
			}
			else
				EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0, 200, 255));

			EVE_Cmd_wr32(s_pHalContext, TAG_MASK(1));
			EVE_Cmd_wr32(s_pHalContext, TAG(i));

			EVE_CoCmd_text(s_pHalContext, washingTypeXoffset, Ty, washingTypeTextFont, OPT_CENTERX | OPT_CENTERY, Array_Cycle_Name[i]);
			EVE_Cmd_wr32(s_pHalContext, TAG_MASK(0));

			Ty += washingTypeTextSpace;

		}

		// Draw rectangle in the centre to show the options
		EVE_Cmd_wr32(s_pHalContext, BEGIN(RECTS));

		EVE_Cmd_wr32(s_pHalContext, LINE_WIDTH(4 * 16));
		EVE_Cmd_wr32(s_pHalContext, COLOR_A(100));
		EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 255, 255));

		EVE_Cmd_wr32(s_pHalContext, VERTEX2F(centerPaneX0 * 16, centerPaneY0 * 16));
		EVE_Cmd_wr32(s_pHalContext, VERTEX2F(centerPaneX1 * 16, centerPaneY1 * 16));
		EVE_Cmd_wr32(s_pHalContext, COLOR_A(255));
		EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0, 0, 0));
		EVE_CoCmd_text(s_pHalContext, optionsTextX, optionsTexty, optionsTextFont, OPT_CENTERX | OPT_CENTERY, "Options");

		// Display icons in vertical order
		EVE_Cmd_wr32(s_pHalContext, BEGIN(BITMAPS));

		By = centerPaneIconsYOffset;

		for (i = 0; i < 3; i++)
		{
			EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 255, 255));
			if (i < 2) {
				EVE_Cmd_wr32(s_pHalContext, BITMAP_HANDLE(i));
				EVE_Cmd_wr32(s_pHalContext, VERTEX2F(centerPaneIconsXOffset * 16, (By) * 16));
			}

			EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0, 0, 0));
			EVE_CoCmd_text(s_pHalContext, centerPaneTextXOffset, By + 10, centerPaneTextFont, OPT_CENTERX | OPT_CENTERY, Array_Cycle_Options[i]);
			By += centerPaneTextSpace;
		}

		// Lines draw after each icon
		EVE_Cmd_wr32(s_pHalContext, BEGIN(LINES));
		EVE_Cmd_wr32(s_pHalContext, LINE_WIDTH(1 * 16));
		EVE_Cmd_wr32(s_pHalContext, COLOR_A(100));
		Ly = centerPaneLineYOffset;

		for (i = 0; i < 3; i++)
		{
			EVE_Cmd_wr32(s_pHalContext, VERTEX2F(centerPaneLineX0 * 16, Ly * 16));
			EVE_Cmd_wr32(s_pHalContext, VERTEX2F(centerPaneLineX1 * 16, Ly * 16));
			Ly += 118;
		}
		EVE_Cmd_wr32(s_pHalContext, COLOR_A(255));
		By = itemPropYOffset;
		if (keypressed > 0 && keypressed < 9)
		{
			if (ProcFinish == 0 || ProcFinish == 2 || TimeSet == 0)
			{
				Tagval = keypressed;
				OptionsSet = 1;
				temp = 0;
				j = keypressed;

			}
		}

		if (Tagval != 0)
		{
			for (i = 0; i < 3; i++)
			{
				EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0, 0, 255));
				if (i == 0)
					EVE_CoCmd_text(s_pHalContext, itemPropXOffset, By, itemPropFont, OPT_CENTERX | OPT_CENTERY, Item_Property[Tagval - 1].SoilLev);
				if (i == 1)
					EVE_CoCmd_text(s_pHalContext, itemPropXOffset, By, itemPropFont, OPT_CENTERX | OPT_CENTERY, Item_Property[Tagval - 1].Temp);
				if (i == 2)
					EVE_CoCmd_text(s_pHalContext, itemPropXOffset, By, itemPropFont, OPT_CENTERX | OPT_CENTERY, Item_Property[Tagval - 1].SpinSpeed);
				By += itemPropSpacing;

			}

		}

		// for soil icon
		if (Item_Property[Tagval - 1].SoilLev == "Light")
			EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(200, 200, 200));
		else if (Item_Property[Tagval - 1].SoilLev == "Normal")
			EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(150, 150, 150));
		else if (Item_Property[Tagval - 1].SoilLev == "Heavy")
			EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(80, 80, 80));
		else
			EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 255, 255));
		EVE_Cmd_wr32(s_pHalContext, BEGIN(BITMAPS));
		EVE_Cmd_wr32(s_pHalContext, VERTEX2II(soilLevX, soilLevY, 0, 0));

		// for temp icon - draw point and line inside icon
		if (Item_Property[Tagval - 1].Temp == "Cold")
			EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0, 0, 255));//blue
		else if (Item_Property[Tagval - 1].Temp == "Warm")
			EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 140, 0));//orange
		else if (Item_Property[Tagval - 1].Temp == "Hot")
			EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 0, 0));//red

		///this is the filling of the temperature icon
		EVE_Cmd_wr32(s_pHalContext, BEGIN(FTPOINTS));
		EVE_Cmd_wr32(s_pHalContext, POINT_SIZE(tempLevPointSz * 16));
		EVE_Cmd_wr32(s_pHalContext, VERTEX2F(tempLevPointX * 16, tempLevPointY * 16));
		EVE_Cmd_wr32(s_pHalContext, BEGIN(LINES));
		EVE_Cmd_wr32(s_pHalContext, LINE_WIDTH(tempLevLineWidth * 16));
		EVE_Cmd_wr32(s_pHalContext, VERTEX2F(tempLevLineX * 16, tempLevLineY0 * 16));
		EVE_Cmd_wr32(s_pHalContext, VERTEX2F(tempLevLineX * 16, tempLevLineY1 * 16));

		// Spin Speed rotation
		if (Item_Property[Tagval - 1].SpinSpeed == "Low")
			Addth = 1000;
		else if (Item_Property[Tagval - 1].SpinSpeed == "Medium")
			Addth = 2000;
		else if (Item_Property[Tagval - 1].SpinSpeed == "High")
			Addth = 3000;
		EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0, 0, 255));
		EVE_Cmd_wr32(s_pHalContext, BEGIN(BITMAPS));
		EVE_Cmd_wr32(s_pHalContext, CMD_LOADIDENTITY);
		//rotate_around(20,20,th);	  //Main_Icons[8].sizex
		rotate_around(Main_Icons[2].sizex / 2, Main_Icons[2].sizey / 2, th);
		EVE_Cmd_wr32(s_pHalContext, VERTEX2II(spinSpeedTextX, spinSpeedTexty, 2, 0));
		EVE_Cmd_wr32(s_pHalContext, RESTORE_CONTEXT());

		// Right side Pane

		// Estimated TIme
		if (Stflag == 0)
			TimeSet = 0;
		//else TimeSet = 0;
		if (TimeSet > 36)TimeSet = 0;
		EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0, 200, 255));
#if defined(DISPLAY_RESOLUTION_WQVGA)
		EVE_CoCmd_text(s_pHalContext, 400, 30, 18, OPT_CENTERX | OPT_CENTERY, "Estimated Time");
		EVE_CoCmd_number(s_pHalContext, 355, 60, 31, OPT_CENTERX | OPT_CENTERY, 0);
		EVE_CoCmd_text(s_pHalContext, 372, 67, 21, OPT_CENTERX | OPT_CENTERY, "H");
#elif defined(DISPLAY_RESOLUTION_WVGA)
		EVE_CoCmd_text(s_pHalContext, 684, 37, 28, OPT_CENTERX | OPT_CENTERY, "Estimated Time");
		EVE_CoCmd_number(s_pHalContext, 622, 82, 31, OPT_CENTERX | OPT_CENTERY, 0);
		EVE_CoCmd_text(s_pHalContext, 641, 88, 27, OPT_CENTERX | OPT_CENTERY, "H");
#endif

#if defined(DISPLAY_RESOLUTION_HVGA_PORTRAIT)
		EVE_CoCmd_text(s_pHalContext, 60, 400, 18, OPT_CENTERX | OPT_CENTERY, "Estimated Time");
		EVE_CoCmd_number(s_pHalContext, 10, 430, 31, OPT_CENTERX | OPT_CENTERY, 0);
		EVE_CoCmd_text(s_pHalContext, 30, 435, 21, OPT_CENTERX | OPT_CENTERY, "H");
#endif

#if defined(DISPLAY_RESOLUTION_WQVGA)
		if (TimeSet < 10)
		{
			EVE_CoCmd_number(s_pHalContext, 395, 60, 31, OPT_CENTERX | OPT_CENTERY, 0);
			EVE_CoCmd_number(s_pHalContext, 415, 60, 31, OPT_CENTERX | OPT_CENTERY, TimeSet);
		}
		else
			EVE_CoCmd_number(s_pHalContext, 405, 60, 31, OPT_CENTERX | OPT_CENTERY, TimeSet);
		EVE_CoCmd_text(s_pHalContext, 442, 67, 21, OPT_CENTERX | OPT_CENTERY, "Min");
#elif defined(DISPLAY_RESOLUTION_WVGA)
		if (TimeSet < 10)
		{
			EVE_CoCmd_number(s_pHalContext, 673, 82, 31, OPT_CENTERX | OPT_CENTERY, 0);
			EVE_CoCmd_number(s_pHalContext, 697, 82, 31, OPT_CENTERX | OPT_CENTERY, TimeSet);
		}
		else
			EVE_CoCmd_number(s_pHalContext, 685, 82, 31, OPT_CENTERX | OPT_CENTERY, TimeSet);
		EVE_CoCmd_text(s_pHalContext, 725, 88, 27, OPT_CENTERX | OPT_CENTERY, "Min");
#endif
#if defined(DISPLAY_RESOLUTION_HVGA_PORTRAIT)
		if (TimeSet < 10)
		{
			EVE_CoCmd_number(s_pHalContext, 50, 430, 31, OPT_CENTERX | OPT_CENTERY, 0);
			EVE_CoCmd_number(s_pHalContext, 75, 430, 31, OPT_CENTERX | OPT_CENTERY, TimeSet);
		}
		else
			EVE_CoCmd_number(s_pHalContext, 60, 430, 31, OPT_CENTERX | OPT_CENTERY, TimeSet);
		EVE_CoCmd_text(s_pHalContext, 100, 435, 21, OPT_CENTERX | OPT_CENTERY, "Min");
#endif
		EVE_Cmd_wr32(s_pHalContext, BEGIN(LINES));
		EVE_Cmd_wr32(s_pHalContext, LINE_WIDTH(1 * 16));

#if defined(DISPLAY_RESOLUTION_WQVGA)
		EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(92, 148, 212));
		//Blue line below butterfly
		EVE_Cmd_wr32(s_pHalContext, VERTEX2F(340 * 16, 90 * 16));
		EVE_Cmd_wr32(s_pHalContext, VERTEX2F(460 * 16, 90 * 16));

		//Orange line below butterfly
		EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 140, 0));
		EVE_Cmd_wr32(s_pHalContext, VERTEX2F(340 * 16, 90 * 16));
		EVE_Cmd_wr32(s_pHalContext, VERTEX2F(ButX * 16, 90 * 16));
#elif defined(DISPLAY_RESOLUTION_WVGA)
		EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(92, 148, 212));
		//Blue line below butterfly
		EVE_Cmd_wr32(s_pHalContext, VERTEX2F(592 * 16, 129 * 16));
		EVE_Cmd_wr32(s_pHalContext, VERTEX2F(760 * 16, 129 * 16));

		//Orange line below butterfly
		EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 140, 0));
		EVE_Cmd_wr32(s_pHalContext, VERTEX2F(592 * 16, 129 * 16));
		EVE_Cmd_wr32(s_pHalContext, VERTEX2F(ButX * 16, 129 * 16));
#endif
#if defined(DISPLAY_RESOLUTION_HVGA_PORTRAIT)
		EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(92, 148, 212));
		//Blue line below butterfly
		EVE_Cmd_wr32(s_pHalContext, VERTEX2F(0 * 16, 470 * 16));
		EVE_Cmd_wr32(s_pHalContext, VERTEX2F(320 * 16, 470 * 16));

		//Orange line below butterfly
		EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 140, 0));
		EVE_Cmd_wr32(s_pHalContext, VERTEX2F(0 * 16, 470 * 16));
		EVE_Cmd_wr32(s_pHalContext, VERTEX2F(ButX * 16, 470 * 16));
#endif
		//Butterfly
		EVE_Cmd_wr32(s_pHalContext, BEGIN(BITMAPS));
		EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 255, 255));
#if defined(DISPLAY_RESOLUTION_WQVGA)
		if (ButX > 460)ButX = 460;
		EVE_Cmd_wr32(s_pHalContext, VERTEX2II(ButX, 75, 3, 0));
#elif defined(DISPLAY_RESOLUTION_WVGA)
		if (ButX > 765)ButX = 765;
		EVE_Cmd_wr32(s_pHalContext, BITMAP_HANDLE(3));
		EVE_Cmd_wr32(s_pHalContext, VERTEX2F(ButX * 16, 105 * 16));
#endif
#if defined(DISPLAY_RESOLUTION_HVGA_PORTRAIT)
		if (ButX > 320)ButX = 320;
		EVE_Cmd_wr32(s_pHalContext, VERTEX2II(ButX, 450, 3, 0));
#endif

		EVE_Cmd_wr32(s_pHalContext, TAG_MASK(1));

		//Lock & Unlock Buton
		if (Read_Tag == 'U')LockFlag = 0;

		{
			EVE_Cmd_wr32(s_pHalContext, TAG('L'));
#ifdef DISPLAY_RESOLUTION_WQVGA
			EVE_Cmd_wr32(s_pHalContext, VERTEX2II(340, 105, 4, 0));
#elif DISPLAY_RESOLUTION_WVGA
			EVE_Cmd_wr32(s_pHalContext, BITMAP_HANDLE(4));
			EVE_Cmd_wr32(s_pHalContext, VERTEX2F(600 * 16, 163 * 16));
#endif
#ifdef DISPLAY_RESOLUTION_HVGA_PORTRAIT
			EVE_Cmd_wr32(s_pHalContext, VERTEX2II(160, 340, 4, 0));
#endif
		}

		if (Read_Tag == 'L')LockFlag = 1;

		// Settings Button
		EVE_Cmd_wr32(s_pHalContext, TAG('S'));
#ifdef DISPLAY_RESOLUTION_WQVGA
		EVE_Cmd_wr32(s_pHalContext, VERTEX2II(340, 145, 6, 0));
#elif DISPLAY_RESOLUTION_WVGA
		EVE_Cmd_wr32(s_pHalContext, BITMAP_HANDLE(6));
		EVE_Cmd_wr32(s_pHalContext, VERTEX2F(600 * 16, 249 * 16));
#endif
#ifdef DISPLAY_RESOLUTION_HVGA_PORTRAIT
		EVE_Cmd_wr32(s_pHalContext, VERTEX2II(160, 380, 6, 0));
#endif
		// Start Button
		EVE_Cmd_wr32(s_pHalContext, TAG('P'));
#ifdef DISPLAY_RESOLUTION_WQVGA
		EVE_Cmd_wr32(s_pHalContext, VERTEX2II(340, 185, 7, 0));
#elif DISPLAY_RESOLUTION_WVGA
		EVE_Cmd_wr32(s_pHalContext, BITMAP_HANDLE(7));
		EVE_Cmd_wr32(s_pHalContext, VERTEX2F(600 * 16, 333 * 16));
#endif
#ifdef DISPLAY_RESOLUTION_HVGA_PORTRAIT
		EVE_Cmd_wr32(s_pHalContext, VERTEX2II(160, 420, 7, 0));
#endif
		EVE_Cmd_wr32(s_pHalContext, TAG_MASK(0));

		if (Read_Tag == 'S')
			Settings();

		if (BrightNess == 0)BrightNess = 100;
		EVE_Hal_wr8(s_pHalContext, REG_PWM_DUTY, BrightNess);// start

		if (TimeSet == 0) ProcFinish = 2;
		if ((Read_Tag == 'P'))
		{
			PlayFlag = 1;
			Opt = 0;
			if (ProcFinish == 2)
			{
				TimeSet = 36;

#if defined(DISPLAY_RESOLUTION_WQVGA)
				Px = 44;
				ButX = 330;
#elif defined(DISPLAY_RESOLUTION_WVGA)
				Px = 76;
				ButX = 592;
#endif
#if defined(DISPLAY_RESOLUTION_HVGA_PORTRAIT)
				Px = 44;
				ButX = 280;
#endif
			}
		}
		if (PlayFlag == 1)
		{
			if (OptionsSet == 1)
			{
				Stflag = 1;
				OptionsSet = 2;
			}
			else if (OptionsSet != 1 && Opt != 1)
			{
				if ((IterCount % 100 > 0) && (IterCount % 100 < 100))
				{
#ifdef DISPLAY_RESOLUTION_WQVGA
					EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 0, 0));
					EVE_Cmd_wr32(s_pHalContext, BEGIN(RECTS));
					EVE_Cmd_wr32(s_pHalContext, VERTEX2F(110 * 16, 115 * 16));
					EVE_Cmd_wr32(s_pHalContext, VERTEX2F(375 * 16, 140 * 16));
					EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0, 0, 0));
					EVE_CoCmd_text(s_pHalContext, 120, 125, 27, OPT_CENTERY, "Please Select a Washing Mode");
#elif DISPLAY_RESOLUTION_WVGA
					EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 0, 0));
					EVE_Cmd_wr32(s_pHalContext, BEGIN(RECTS));
					EVE_Cmd_wr32(s_pHalContext, VERTEX2F(255 * 16, 255 * 16));
					EVE_Cmd_wr32(s_pHalContext, VERTEX2F(545 * 16, 300 * 16));
					EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0, 0, 0));
					EVE_CoCmd_text(s_pHalContext, 400, 278, 28, OPT_CENTERX | OPT_CENTERY, "Please Select a Washing Mode");
#endif
#ifdef DISPLAY_RESOLUTION_HVGA_PORTRAIT
					EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 0, 0));
					EVE_Cmd_wr32(s_pHalContext, BEGIN(RECTS));
					EVE_Cmd_wr32(s_pHalContext, VERTEX2F(30 * 16, 115 * 16));
					EVE_Cmd_wr32(s_pHalContext, VERTEX2F(280 * 16, 140 * 16));
					EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0, 0, 0));
					EVE_CoCmd_text(s_pHalContext, 40, 125, 27, OPT_CENTERY, "Please Select a Washing Mode");
#endif
					if (IterCount % 100 == 99)
					{
						Opt = 1;
						PlayFlag = 0;
					}
				}
			}
		}

		if (temp >= 50)temp = 0;
		EVE_Cmd_wr32(s_pHalContext, RESTORE_CONTEXT());
#ifdef DISPLAY_RESOLUTION_WQVGA
		By = 115;
		for (i = 0; i < 3; i++)
		{
			EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0, 200, 255));
			EVE_CoCmd_text(s_pHalContext, 380, By, 18, 0, Array_Menu_Options[i]);
			By += 40;
#elif DISPLAY_RESOLUTION_WVGA
		By = 182;
		for (i = 0; i < 3; i++)
		{
			EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0, 200, 255));
			EVE_CoCmd_text(s_pHalContext, 666, By, 28, 0, Array_Menu_Options[i]);
			By += 75;
#endif
#ifdef DISPLAY_RESOLUTION_HVGA_PORTRAIT
			By = 340;
			for (i = 0; i < 3; i++)
			{
				EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0, 200, 255));
				EVE_CoCmd_text(s_pHalContext, 210, By + 9, 18, 0, Array_Menu_Options[i]);
				By += 40;
#endif
		}
			if (LockFlag == 1)
			{
				EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(219, 180, 150));
				EVE_Cmd_wr32(s_pHalContext, COLOR_A(180));
				EVE_Cmd_wr32(s_pHalContext, BEGIN(EDGE_STRIP_A));
				EVE_Cmd_wr32(s_pHalContext, VERTEX2F(0, s_pHalContext->Height * 16));
				EVE_Cmd_wr32(s_pHalContext, VERTEX2F(s_pHalContext->Width * 16, s_pHalContext->Height * 16));
				EVE_Cmd_wr32(s_pHalContext, COLOR_A(255));
				EVE_Cmd_wr32(s_pHalContext, RESTORE_CONTEXT());

				EVE_Cmd_wr32(s_pHalContext, BEGIN(BITMAPS));
#if defined(SAMAPP_DISPLAY_WQVGA) || defined(SAMAPP_DISPLAY_QVGA)
				EVE_Cmd_wr32(s_pHalContext, BITMAP_TRANSFORM_A(128));
				EVE_Cmd_wr32(s_pHalContext, BITMAP_TRANSFORM_E(128));
#endif
				EVE_Cmd_wr32(s_pHalContext, TAG('U'));
				EVE_Cmd_wr32(s_pHalContext, BITMAP_HANDLE(4));
				EVE_Cmd_wr32(s_pHalContext, VERTEX2F((s_pHalContext->Width / 2) * 16, (s_pHalContext->Height / 2) * 16));

				EVE_Cmd_wr32(s_pHalContext, RESTORE_CONTEXT());
			}

			if (MainFlag == 0)
#if defined(DISPLAY_RESOLUTION_WQVGA)
				Px = 44;
#elif defined(DISPLAY_RESOLUTION_WVGA)
				Px = 76;
#endif
#if defined(DISPLAY_RESOLUTION_HVGA_PORTRAIT)
			Px = 44;
#endif

			EVE_Cmd_wr32(s_pHalContext, END());

			EVE_Cmd_wr32(s_pHalContext, DISPLAY());
			EVE_CoCmd_swap(s_pHalContext);
			EVE_Cmd_waitFlush(s_pHalContext);

			th += Addth;
			if (IterCount % 15 == 0 && Stflag == 1 && Pf == 0)
			{
				ButX += 1;
			}
			if (IterCount % 50 == 0 && Pf == 0 && Stflag == 1)
				TimeSet--;
			if (IterCount % 50 == 0 && Pf == 0)
#ifdef DISPLAY_RESOLUTION_WQVGA
				Px += 11;
#elif DISPLAY_RESOLUTION_WVGA
				Px += 19;
#endif
#ifdef DISPLAY_RESOLUTION_HVGA_PORTRAIT
			Px += 11;
#endif

			if (temp >= 1)temp++;

			IterCount += 1;
	} while (OptionsSet != 2);//(Read_Tag!= 'P');
	MainFlag++;
	return Read_Tag;
}

void DemoWashingMachine(EVE_HalContext* pHalContext) {
	s_pHalContext = pHalContext;
	Logo_Intial_setup(Main_Icons, 7);

	while (1)
	{
		uint8_t Val = MainWindow();
		if (Val == 'P')
		{
			Play_Sound(s_pHalContext, 0x56, SoundLev, 0);
			WashWindow();
		}
	}
}