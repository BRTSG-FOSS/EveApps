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

#if defined(FT900_PLATFORM) || defined(FT93X_PLATFORM)
#include "ff.h"
#endif

#define NORMAL_PRESSURE 1200
#define MAX_ADDITIONAL_VEL 64
#define BASE_VELOCITY 32
#define COLUMN_GAP 8
#define POINTS_PER_COLUMN 5
#define BUTTON_GAP 5
#define BUTTON_CURVATURE 5
#define NUM_OF_ICONS 14
#define ICON_HEIGHT bitmapInfo[0].Height
#define ICON_WIDTH bitmapInfo[0].Width
#define ICON_STRIDE bitmapInfo[0].Stride
#define BOUNCE_AMOUNT 12

#define VISIBLE_ICONS 3
#define COMBINATION_AMOUNT 5
#define PAYOUT_TABLE_SIZE 15
#define TOTAL_BET_LINES 12
#define BET_TEXT "Bet:"
#define COLUMN_TEXT "Column: "
#define LINE_TEXT "Line: "
#define BALANCE_TEXT "Balance: "
#define RESET_TEXT "reset"
#define PAYOUT_TEXT "Payout"
#define INITIAL_BALANCE 1000
#define ANY_FRUIT_TEXT "Fruits"

#ifdef TEST
#define SPINNING_SOUND 0x00        
#define COIN_COLLISION_SOUND 0x00 
#define BUTTON_PRESS_SOUND 0x00  
#define COIN_REWARD_SOUND 0x00  
#else
#define SPINNING_SOUND 0x50        //click sound effect
#define COIN_COLLISION_SOUND 0x43 //glockenspiel sound effect
#define BUTTON_PRESS_SOUND 0x51  //switch sound effect
#define COIN_REWARD_SOUND 0x52  //cowbell sound effect
#endif 

#if defined(DISPLAY_RESOLUTION_WVGA)
#define SPIN_COLUMNS 5
#define STATUS_BAR_HEIGHT 95
#elif defined(DISPLAY_RESOLUTION_WQVGA)
#define SPIN_COLUMNS 5
#define STATUS_BAR_HEIGHT 50 
#elif defined(DISPLAY_RESOLUTION_QVGA)
#define SPIN_COLUMNS 3
#define STATUS_BAR_HEIGHT 50 
#endif

#if EVE_CHIPID <= EVE_FT801
#define ROMFONT_TABLEADDRESS (0xFFFFC)
#else
#define ROMFONT_TABLEADDRESS 3145724UL // 2F FFFCh
#endif

typedef signed char        schar8_t;

uint8_t temp_tag;

typedef struct bitmap_header_t
{
	uint16_t Width;
	uint16_t Height;
	uint16_t Stride;
	int32_t Offset;
}bitmap_header;

typedef struct spinning_column_t
{
	uint8_t curIndex;
	uint8_t iconArray[20];
	schar8_t velocity;
	uint8_t drawnIndex;
	schar8_t bounceOffset;
	uint8_t bounceAmount;
}spinning_column;

typedef struct coins_t
{
	int32_t xPos;
	int32_t yPos;
	schar8_t xVel;
	schar8_t yVel;
	uint8_t index;
	bool_t fall;
}spin_coins;

typedef struct bet_lines_t
{
	int16_t x0;
	int16_t y0;
	int16_t x1;
	int16_t y1;
	int16_t x2;
	int16_t y2;
	int16_t x3;
	int16_t y3;
	uint8_t r;
	uint8_t g;
	uint8_t b;
}bet_lines;

typedef struct bet_lines_index_t
{
	char8_t line[5];
}bet_lines_index;

spin_coins spinCoins[8];

typedef struct ui_attributes_t
{
	uint8_t spinButtonWidth;
	uint8_t spinColumns;
	uint16_t spinColumnXOffset;
	uint16_t spinColumnYOffset;
	uint16_t visibleIconsHeight;
	uint16_t columnPixelHeight;
}ui_attributes;

typedef struct jackpot_attributes_t
{
	uint8_t spinColumns;
	bool_t spinning;
	bool_t released;
	bool_t rewardedPoints;
	bool_t showPayoutTable;
	bool_t reset;
	bool_t betChanged;
	bool_t displayRewardLine;
	int16_t balance;
	uint8_t totalBetAmount;
	uint8_t stoppedColumns;
	uint8_t spinColumnSelected;
	uint8_t payoutTableSize;
	uint8_t winningIndex[SPIN_COLUMNS];
	uint8_t touchTag;
	uint8_t pendown;
	int16_t payoutTableShift;
	uint8_t lineBet[12][5];
	uint8_t selectedBetLine;
	uint8_t selectedBetMultiplier;
	uint8_t winningLineIcons;
}jackpot_attributes;

uint8_t Gpu_Rom_Font_WH(uint8_t Char, uint8_t font);
uint16_t stringPixelWidth(const char8_t* text, uint8_t font);
uint16_t unsignedNumberPixelWidth(int16_t digits, uint8_t font);
void drawPayoutTable();
void drawSpinColumns();
void scroll();
void updateIndex();
void touched(uint8_t touchVal);
uint8_t nextRandomInt8(uint8_t seed);
uint16_t getPoints();
void jackpot();
void startingAnimation();

uint8_t fontPixelHeight(uint8_t font);
void clearArray(uint8_t* index, uint16_t size, uint8_t defaultValue);
bool_t ableToSpin();
void displayRewardText(uint16_t points);
//void displaySelectedIcon(uint8_t index);
bool_t ableToContinue();
void coinsAnimation(uint8_t);
int32_t get_free_memory();
void lineBetButtons();
uint16_t getlineBetPoints();

uint8_t payoutTable[] = { 8, 	7, 	7, 	6, 	6, 	5, 	5, 	4, 	4, 	3, 	3, 	2, 	2, 	1, 	1 };

bet_lines_index linePosition[] = {
	{ 't','t','t','t','t' },  //0
	{ 't','t','t','m','b' },//1
	{ 'm','m','m','m','m' },//2
	{ 'm','m','m','m','t' },//3
	{ 'b','b','b','b','b' },//4
	{ 'b','b','b','m','t' },//5
	{ 'm','t','t','t','t' },//6
	{ 't','m','b','m','t' },//7
	{ 'b','m','m','m','m' },//8
	{ 't','m','m','m','m' },//9
	{ 'm','m','m','m','b' },//10
	{ 'b','m','t','m','b' }//11
};

#if defined(DISPLAY_RESOLUTION_WQVGA) | defined(DISPLAY_RESOLUTION_QVGA)
bet_lines betLines[] = {
	{ 44,38,63,50,235,50,404,50,0x7e,0x1e,0x9c },     //1
	{ 46,67,63,55,235,55,404,170,0x15,0xb0,0x1a },    //2
	{ 47,100,63,102,235,102,404,102,0x03,0x46,0xDf }, //3
	{ 46,129,63,107,235,107,404,60,0xff,0x81,0xc0 },  //4
	{ 48,160,63,170,235,170,404,170,0x65,0x37,0x00 }, //5
	{ 49,189,63,165,235,165,404,60,0xe5,0x00,0x00 },  //6
	{ 423,36,404,60,235,60,63,112,0x95,0xd0,0xfc },   //7
	{ 424,67,398,60,235,160,63,60,0x02,0x93,0x86 },   //8
	{ 425,95,404,112,164,112,63,170,0xf9,0x73,0x06 }, //9
	{ 424,126,404,117,159,117,59,58,0x96,0xf9,0x7b }, //10
	{ 424,157,404,170,299,122,63,122,0xc2,0x00,0x78 },//11
	{ 424,186,400,180,235,75,83,170,0xff,0xff,0x14 }, //12
};
#elif defined(DISPLAY_RESOLUTION_WVGA)
bet_lines betLines[] = {
	{ 100,75,125,100,400,100,650,100,0x7e,0x1e,0x9c },     //1
	{ 100,125,125,108,400,108,650,300,0x15,0xb0,0x1a },    //2
	{ 100,190,200,190,400,190,650,190,0x03,0x46,0xDf }, //3
	{ 100,225,200,195,400,195,650,105,0xff,0x81,0xc0 },  //4
	{ 100,275,125,300,400,300,650,300,0x65,0x37,0x00 }, //5
	{ 100,320,125,308,400,308,650,108,0xe5,0x00,0x00 },  //6
	{ 680,75,650,116,400,116,130,181,0x95,0xd0,0xfc },   //7
	{ 680,125,650,109,400,300,125,115,0x02,0x93,0x86 },   //8
	{ 680,175,650,208,300,208,125,316,0xf9,0x73,0x06 }, //9
	{ 680,220,650,216,300,216,125,116,0x96,0xf9,0x7b }, //10
	{ 680,270,650,275,500,222,125,222,0xc2,0x00,0x78 },//11
	{ 680,320,625,300,400,125,175,300,0xff,0xff,0x14 }, //12
};
#endif

#if defined(DISPLAY_RESOLUTION_WQVGA) | defined(DISPLAY_RESOLUTION_WVGA)
spinning_column columns[] = {
	{ 6,{ 2,6,1,5,11,7,10,0,4,8,12,3,9,13 },70,6,30,30 },
	{ 5,{ 11,8,12,1,7,4,0,2,5,3,13,6,10,9 },42,8,30,30 },
	{ 9,{ 9,1,2,4,11,10,13,6,5,7,0,3,12,8 },56,1,30,30 },
	{ 11,{ 10,5,1,11,2,3,7,4,8,12,9,6,0,13 },42,5,30,30 },
	{ 10,{ 5,2,11,4,8,9,7,12,10,13,1,0,3,6 },84,1,30,30 },
};

#endif

#ifdef DISPLAY_RESOLUTION_QVGA
spinning_column columns[] = {
	{ 6,{ 2,6,1,5,11,7,10,0,4,8,12,3,9,13 },70,6,30,30 },
	{ 5,{ 11,8,12,1,7,4,0,2,5,3,13,6,10,9 },42,8,30,30 },
	{ 9,{ 9,1,2,4,11,10,13,6,5,7,0,3,12,8 },56,1,30,30 },
};
#endif

#if defined(DISPLAY_RESOLUTION_WVGA)
bitmap_header bitmapInfo[] =
{
	/*width,height,stride,memory offset */
	{ 105, 90,105 * 2,0 }, // this one represents all other spinning icons
	{ 16, 16, 16 * 2,0 },  //background icon
	{ 48, 51, 48 * 2,0 },  //coin .bin file
	{ 48, 51, 48 * 2,0 },
	{ 48, 51, 48 * 2,0 },
	{ 48, 51, 48 * 2,0 },
	{ 48, 51, 48 * 2,0 },
	{ 48, 51, 48 * 2,0 },
	{ 48, 51, 48 * 2,0 },
	{ 105,270,  105,0 },  //overlay
	{ 119,310,119 * 2,0 } //outer overlay
};

#elif defined(DISPLAY_RESOLUTION_QVGA) | defined(DISPLAY_RESOLUTION_WQVGA)
bitmap_header bitmapInfo[] =
{
	/*width,height,stride,memory offset */
	{ 64,55,	64 * 2,	0 }, // this one represents all other spinning icons
	{ 16,16, 16 * 2,	0 },  //background icon
	{ 30,32,	30 * 2,	0 },  //coin .bin file
	{ 30,32,	30 * 2,	0 },
	{ 30,32,	30 * 2,	0 },
	{ 30,32,	30 * 2,	0 },
	{ 30,32,	30 * 2,	0 },
	{ 30,32,	30 * 2,	0 },
	{ 30,32,	30 * 2,	0 },
	{ 64,165,64,  0 },  //overlay
	{ 72,190,72 * 2,0 } //outer overlay
};
#endif

ui_attributes UI;
jackpot_attributes JPot;

/* Global used for Hal Context */

/*
//this function retrieves the current ram usage for the Arduino platform.  Negative number means stack overflow.
int32_t get_free_memory()
{
#ifdef ARDUINO_PLATFORM
extern int32_t __heap_start, *__brkval;
int32_t v;
return (int32_t) &v - (__brkval == 0 ? (int32_t) &__heap_start : (int32_t) __brkval);
#endif
return 0;
}
*/

static uint8_t currentLine = 0;
#if defined(MSVC_PLATFORM) || defined(BT8XXEMU_PLATFORM)
static time_t lastMilli;
#endif
#if defined(FT900_PLATFORM) || defined(FT93X_PLATFORM)
uint32_t curMilli = 0;
uint32_t getCurTime = 0;
#endif

bool_t finished = FALSE;

bool_t displayTheLine(uint8_t line) {
#ifdef ARDUINO_PLATFORM
	static uint32_t lastMilli = millis();
	uint32_t curMilli;
	bool_t nextLine = FALSE;
	uint8_t i, j;
	for (i = 0; i<SPIN_COLUMNS; i++) {
		if (JPot.lineBet[currentLine][i] < 254) {
			nextLine = TRUE;
			break;
		}
	}

	if (nextLine) {
		curMilli = millis();
		if ((curMilli - lastMilli)>1000) {
			currentLine = (currentLine + 1) >= TOTAL_BET_LINES ? 0 : (currentLine + 1);
			lastMilli = curMilli;
			return FALSE;
		}
		else {
			return TRUE;
		}
	}
	else {
		currentLine = (currentLine + 1) >= TOTAL_BET_LINES ? 0 : (currentLine + 1);
		return FALSE;
	}
#endif

#if defined(FT900_PLATFORM) || defined(FT93X_PLATFORM)
	bool_t nextLine = FALSE;
	uint8_t i;
	static uint32_t lastMilli;
	lastMilli = EVE_millis();
	for (i = 0; i<SPIN_COLUMNS; i++) {
		if (JPot.lineBet[currentLine][i] < 254) {
			nextLine = TRUE;
			break;
		}
	}

	if (nextLine) {
		curMilli = EVE_millis();
		if ((curMilli - lastMilli)>1000) {
			currentLine = (currentLine + 1) >= TOTAL_BET_LINES ? 0 : (currentLine + 1);
			lastMilli = curMilli;
			return FALSE;
		}
		else {
			return TRUE;
		}
	}
	else {
		currentLine = (currentLine + 1) >= TOTAL_BET_LINES ? 0 : (currentLine + 1);
		return FALSE;
	}
#endif

#if defined(MSVC_PLATFORM) || defined(BT8XXEMU_PLATFORM)
	time_t curMilli;
	bool_t nextLine = FALSE;
	uint8_t i, j;
	for (i = 0; i<SPIN_COLUMNS; i++) {
		if (JPot.lineBet[currentLine][i] < 254) {
			nextLine = TRUE;
			break;
		}
	}

	if (nextLine) {
		curMilli = time(NULL);
		if ((curMilli - lastMilli)>1) {
			currentLine = (currentLine + 1) >= TOTAL_BET_LINES ? 0 : (currentLine + 1);
			lastMilli = curMilli;
			return FALSE;
		}
		else {
			return TRUE;
		}
	}
	else {
		currentLine = (currentLine + 1) >= TOTAL_BET_LINES ? 0 : (currentLine + 1);
		return FALSE;
	}

#endif
}

void drawbetlines() {
	uint8_t i;
	EVE_Cmd_wr32(s_pHalContext, LINE_WIDTH(3 * 16));
	EVE_Cmd_wr32(s_pHalContext, COLOR_A(255));
	if (JPot.displayRewardLine) {
		if (displayTheLine(currentLine)) {
			EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(betLines[currentLine].r, betLines[currentLine].g, betLines[currentLine].b));
			EVE_Cmd_wr32(s_pHalContext, BEGIN(LINE_STRIP));
#if defined(DISPLAY_RESOLUTION_QVGA) | defined(DISPLAY_RESOLUTION_WQVGA)
			EVE_Cmd_wr32(s_pHalContext, VERTEX2II(betLines[currentLine].x0, betLines[currentLine].y0, 0, 0));
			EVE_Cmd_wr32(s_pHalContext, VERTEX2II(betLines[currentLine].x1, betLines[currentLine].y1, 0, 0));
			EVE_Cmd_wr32(s_pHalContext, VERTEX2II(betLines[currentLine].x2, betLines[currentLine].y2, 0, 0));
			EVE_Cmd_wr32(s_pHalContext, VERTEX2II(betLines[currentLine].x3, betLines[currentLine].y3, 0, 0));
#elif defined(DISPLAY_RESOLUTION_WVGA)
			EVE_Cmd_wr32(s_pHalContext, VERTEX2F(betLines[currentLine].x0 * 16, betLines[currentLine].y0 * 16));
			EVE_Cmd_wr32(s_pHalContext, VERTEX2F(betLines[currentLine].x1 * 16, betLines[currentLine].y1 * 16));
			EVE_Cmd_wr32(s_pHalContext, VERTEX2F(betLines[currentLine].x2 * 16, betLines[currentLine].y2 * 16));
			EVE_Cmd_wr32(s_pHalContext, VERTEX2F(betLines[currentLine].x3 * 16, betLines[currentLine].y3 * 16));
#endif
			EVE_Cmd_wr32(s_pHalContext, END());
		}
	}
	else {
		for (i = 0; i<JPot.selectedBetLine; i++) {
			EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(betLines[i].r, betLines[i].g, betLines[i].b));
			EVE_Cmd_wr32(s_pHalContext, BEGIN(LINE_STRIP));
#if defined(DISPLAY_RESOLUTION_QVGA) | defined(DISPLAY_RESOLUTION_WQVGA)
			EVE_Cmd_wr32(s_pHalContext, VERTEX2II(betLines[i].x0, betLines[i].y0, 0, 0));
			EVE_Cmd_wr32(s_pHalContext, VERTEX2II(betLines[i].x1, betLines[i].y1, 0, 0));
			EVE_Cmd_wr32(s_pHalContext, VERTEX2II(betLines[i].x2, betLines[i].y2, 0, 0));
			EVE_Cmd_wr32(s_pHalContext, VERTEX2II(betLines[i].x3, betLines[i].y3, 0, 0));
#elif defined(DISPLAY_RESOLUTION_WVGA)
			EVE_Cmd_wr32(s_pHalContext, VERTEX2F(betLines[i].x0 * 16, betLines[i].y0 * 16));
			EVE_Cmd_wr32(s_pHalContext, VERTEX2F(betLines[i].x1 * 16, betLines[i].y1 * 16));
			EVE_Cmd_wr32(s_pHalContext, VERTEX2F(betLines[i].x2 * 16, betLines[i].y2 * 16));
			EVE_Cmd_wr32(s_pHalContext, VERTEX2F(betLines[i].x3 * 16, betLines[i].y3 * 16));
#endif
			EVE_Cmd_wr32(s_pHalContext, END());
		}
	}
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 255, 255));
}

//static uint32_t ramOffset=0;

void jackpotSetup() {
	char8_t path[64];
	char8_t homePath[] = TEST_DIR "\\";
	uint32_t ramOffset = 0;
	char8_t fileName[3];
	uint8_t i;
#if defined(DISPLAY_RESOLUTION_WQVGA)
	char8_t bitmapExtention[] = "J.jpg";
	UI.spinButtonWidth = 80;
#elif defined(DISPLAY_RESOLUTION_QVGA)
	char8_t bitmapExtention[] = "J.jpg";
	UI.spinButtonWidth = 50;
#elif defined(DISPLAY_RESOLUTION_WVGA)
	char8_t bitmapExtention[] = "JH.jpg";
	UI.spinButtonWidth = 100;

#endif

	UI.columnPixelHeight = NUM_OF_ICONS*ICON_HEIGHT;
	JPot.spinning = FALSE;
	JPot.released = FALSE;
	JPot.rewardedPoints = TRUE;
	JPot.showPayoutTable = FALSE;
	JPot.reset = FALSE;
	JPot.balance = INITIAL_BALANCE;
	JPot.spinColumnSelected = 0;
	JPot.pendown = 0;
	JPot.payoutTableShift = 0;
	JPot.selectedBetMultiplier = 0;
	JPot.selectedBetLine = 12;
	UI.visibleIconsHeight = VISIBLE_ICONS * ICON_HEIGHT;
	UI.spinColumnXOffset = (s_pHalContext->Width >> 1) - ((SPIN_COLUMNS*ICON_WIDTH + SPIN_COLUMNS*COLUMN_GAP) >> 1);  //center the spinning columns at the middle of the screen
	UI.spinColumnYOffset = (((s_pHalContext->Height - STATUS_BAR_HEIGHT) >> 1) - ((UI.visibleIconsHeight) >> 1));   //center the spinning columns at the middle of the screen

	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0, 0, 0));
	EVE_CoCmd_dlStart(s_pHalContext);        // start
	EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1));
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 255, 255));
#if defined(DISPLAY_RESOLUTION_WVGA)
	EVE_CoCmd_text(s_pHalContext, s_pHalContext->Width >> 1, (s_pHalContext->Height >> 1) - 50, 29, OPT_CENTER, "Loading bitmap...");
#else
	EVE_CoCmd_text(s_pHalContext, s_pHalContext->Width >> 1, s_pHalContext->Height >> 1, 29, OPT_CENTER, "Loading bitmap...");
#endif
	EVE_CoCmd_spinner(s_pHalContext, (s_pHalContext->Width >> 1), (s_pHalContext->Height >> 1) + 50, 0, 0);
	EVE_Cmd_waitFlush(s_pHalContext);
	delay(200);

	EVE_CoCmd_dlStart(s_pHalContext);        // start
	EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1));
	printf("Loading regular bitmaps\n");
	//load the bitmaps into memory, all spinning icons are assumed to be the same in all dimensions.
	for (i = 0; i<NUM_OF_ICONS; i++) {
		path[0] = fileName[0] = '\0';
		Gpu_Hal_Dec2Ascii(fileName, i);
		strcat(path, homePath);
		strcat(path, fileName);
		strcat(path, bitmapExtention);
		EVE_Cmd_wr32(s_pHalContext, BITMAP_HANDLE((i)));
		EVE_Cmd_wr32(s_pHalContext, BITMAP_SOURCE(ramOffset));
		EVE_Cmd_wr32(s_pHalContext, BITMAP_LAYOUT(RGB565, ICON_STRIDE, ICON_HEIGHT));
		EVE_Cmd_wr32(s_pHalContext, BITMAP_SIZE(NEAREST, BORDER, BORDER, ICON_WIDTH, ICON_HEIGHT));
		Gpu_Hal_LoadImageToMemory(s_pHalContext, path, ramOffset, LOADIMAGE);
		ramOffset += (ICON_STRIDE*ICON_HEIGHT);
	}

	printf("Loading status bar\n");
	//load status bar bitmap by itself because the width is being repeated 
	path[0] = fileName[0] = '\0';
	Gpu_Hal_Dec2Ascii(fileName, 14);
	strcat(path, homePath);
	strcat(path, fileName);
	strcat(path, bitmapExtention);
	EVE_Cmd_wr32(s_pHalContext, BITMAP_HANDLE((14)));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_SOURCE(ramOffset));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_LAYOUT(RGB565, 2, STATUS_BAR_HEIGHT));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_SIZE(NEAREST, REPEAT, BORDER, s_pHalContext->Width, STATUS_BAR_HEIGHT));
#if defined(DISPLAY_RESOLUTION_WVGA)
	EVE_Cmd_wr32(s_pHalContext, BITMAP_SIZE_H(s_pHalContext->Width >> 9, STATUS_BAR_HEIGHT >> 9));
#endif
	Gpu_Hal_LoadImageToMemory(s_pHalContext, path, ramOffset, LOADIMAGE);
	ramOffset += (2 * STATUS_BAR_HEIGHT);

	printf("Loading background bitmap\n");
	//load background bitmap into memory because the width and height are repeated. All bitmap handles have been exhausted, this icon will be drawn by specifying its souce, layout, and size. 
	path[0] = fileName[0] = '\0';
	Gpu_Hal_Dec2Ascii(fileName, 15);
	strcat(path, homePath);
	strcat(path, fileName);
	strcat(path, bitmapExtention);
	bitmapInfo[1].Offset = ramOffset;
	Gpu_Hal_LoadImageToMemory(s_pHalContext, path, ramOffset, LOADIMAGE);
	ramOffset += (bitmapInfo[1].Stride*bitmapInfo[1].Height);

	printf("loading coins\n");
	//load .bin coin bitmap files. 
	for (i = 16; i<24; i++) {
		path[0] = fileName[0] = '\0';
		strcat(path, homePath);
		Gpu_Hal_Dec2Ascii(fileName, i);
		strcat(path, fileName);
#if defined(DISPLAY_RESOLUTION_WQVGA) || defined(DISPLAY_RESOLUTION_QVGA)
		strcat(path, "J.bin");
#elif defined(DISPLAY_RESOLUTION_WVGA)
		strcat(path, "JH.bin");
#endif
		bitmapInfo[i - 14].Offset = ramOffset;
		Gpu_Hal_LoadImageToMemory(s_pHalContext, path, ramOffset, INFLATE);
		ramOffset += (bitmapInfo[i - 14].Stride*bitmapInfo[i - 14].Height);
	}

	printf("loading overlay\n");
	path[0] = '\0';
	strcat(path, homePath);

#if defined(DISPLAY_RESOLUTION_WQVGA) || defined(DISPLAY_RESOLUTION_QVGA)
	strcat(path, "overlay.raw");
#elif defined(DISPLAY_RESOLUTION_WVGA)
	strcat(path, "overlayH.raw");
#endif
	bitmapInfo[9].Offset = ramOffset;
	Gpu_Hal_LoadImageToMemory(s_pHalContext, path, ramOffset, LOAD);
	ramOffset += (bitmapInfo[9].Stride*bitmapInfo[9].Height * 2);

	printf("loading outer overlay\n");
	path[0] = '\0';
	strcat(path, homePath);

#if defined(DISPLAY_RESOLUTION_WQVGA) || defined(DISPLAY_RESOLUTION_QVGA)
	strcat(path, "outer.raw");
#elif defined(DISPLAY_RESOLUTION_WVGA)
	strcat(path, "outerH.raw");
#endif
	bitmapInfo[10].Offset = ramOffset;
	Gpu_Hal_LoadImageToMemory(s_pHalContext, path, ramOffset, LOAD);
	ramOffset += (bitmapInfo[10].Stride*bitmapInfo[10].Height);

	EVE_Cmd_wr32(s_pHalContext, DISPLAY());
	EVE_CoCmd_swap(s_pHalContext);
	EVE_Cmd_waitFlush(s_pHalContext);
}

void coinsAnimation(uint8_t coinAmount) {
	schar8_t gravity = 2;
	uint8_t i;

	EVE_Cmd_wr32(s_pHalContext, BLEND_FUNC(SRC_ALPHA, ONE_MINUS_SRC_ALPHA));
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 255, 255));
	EVE_Cmd_wr32(s_pHalContext, BEGIN(BITMAPS));

#if 0
	for (i = 0; i<coinAmount; i++) {
		if (spinCoins[i].index >= 7) {
			spinCoins[i].index = 0;
		}
		if (((spinCoins[i].xPos + bitmapInfo[spinCoins[i].index + 2].Width)<0) || (spinCoins[i].xPos>s_pHalContext->Width) || (spinCoins[i].yPos>s_pHalContext->Height)) {
			continue;
		}
		EVE_Cmd_wr32(s_pHalContext, BITMAP_SOURCE(bitmapInfo[spinCoins[i].index + 2].Offset));
		EVE_Cmd_wr32(s_pHalContext, BITMAP_LAYOUT(ARGB4, bitmapInfo[spinCoins[i].index + 2].Stride, bitmapInfo[spinCoins[i].index + 2].Height));
		EVE_Cmd_wr32(s_pHalContext, BITMAP_SIZE(NEAREST, BORDER, BORDER, bitmapInfo[spinCoins[i].index + 2].Width, bitmapInfo[spinCoins[i].index + 2].Height));
		EVE_Cmd_wr32(s_pHalContext, VERTEX2F(spinCoins[i].xPos * 16, spinCoins[i].yPos * 16));
		spinCoins[i].index += 1;
		if (spinCoins[i].index >= 7) {
			spinCoins[i].index = 0;
		}

		spinCoins[i].xPos += spinCoins[i].xVel;
		spinCoins[i].yPos += spinCoins[i].yVel;
		spinCoins[i].yVel += gravity;

		if ((spinCoins[i].yPos + bitmapInfo[spinCoins[i].index + 2].Height)> (s_pHalContext->Height - STATUS_BAR_HEIGHT) && !spinCoins[i].fall) {
			spinCoins[i].yPos = s_pHalContext->Height - STATUS_BAR_HEIGHT - bitmapInfo[spinCoins[i].index + 2].Height;
			spinCoins[i].yVel = -1 * ((spinCoins[i].yVel * 3) / 4);
			Play_Sound(s_pHalContext, COIN_COLLISION_SOUND, 255, 0x00); //play sound when the coin hits the status bar
			if (spinCoins[i].yVel == 0) {
				spinCoins[i].fall = TRUE;
			}
		}
	}
#else
	for (i = 0; i<coinAmount; i++) {
		if (spinCoins[i].index >= 7) {
			spinCoins[i].index = 0;
		}
		if (((spinCoins[i].xPos + bitmapInfo[spinCoins[i].index + 2].Width)<0) || (spinCoins[i].xPos>s_pHalContext->Width) || (spinCoins[i].yPos>s_pHalContext->Height)) {
			continue;
		}

		if ((spinCoins[i].yPos + bitmapInfo[spinCoins[i].index + 2].Height)> (s_pHalContext->Height - STATUS_BAR_HEIGHT) && !spinCoins[i].fall) {
			spinCoins[i].yPos = s_pHalContext->Height - STATUS_BAR_HEIGHT - bitmapInfo[spinCoins[i].index + 2].Height;
			spinCoins[i].yVel = -1 * ((spinCoins[i].yVel * 3) / 4);
			Play_Sound(s_pHalContext, COIN_COLLISION_SOUND, 255, 0x00); //play sound when the coin hits the status bar
			if (spinCoins[i].yVel == 0) {
				spinCoins[i].fall = TRUE;
			}
		}

		EVE_Cmd_wr32(s_pHalContext, BITMAP_SOURCE(bitmapInfo[spinCoins[i].index + 2].Offset));
		EVE_Cmd_wr32(s_pHalContext, BITMAP_LAYOUT(ARGB4, bitmapInfo[spinCoins[i].index + 2].Stride, bitmapInfo[spinCoins[i].index + 2].Height));
		EVE_Cmd_wr32(s_pHalContext, BITMAP_SIZE(NEAREST, BORDER, BORDER, bitmapInfo[spinCoins[i].index + 2].Width, bitmapInfo[spinCoins[i].index + 2].Height));
		EVE_Cmd_wr32(s_pHalContext, VERTEX2F(spinCoins[i].xPos * 16, spinCoins[i].yPos * 16));
		spinCoins[i].index += 1;
		if (spinCoins[i].index >= 7) {
			spinCoins[i].index = 0;
		}

		spinCoins[i].xPos += spinCoins[i].xVel;
		spinCoins[i].yPos += spinCoins[i].yVel;
		spinCoins[i].yVel += gravity;

	}

#endif
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

uint16_t stringPixelWidth(const char8_t* text, uint8_t font) {
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

//Numbers of the same font have the same width
uint16_t unsignedNumberPixelWidth(int16_t digits, uint8_t font) {
	return Gpu_Rom_Font_WH('1', font)*digits;
}

void drawPayoutTable() {
#if defined(DISPLAY_RESOLUTION_WQVGA) || defined(DISPLAY_RESOLUTION_QVGA)
	uint8_t  numFont = 23, rateOfChange = 2, payoutNumOffset = fontPixelHeight(numFont) >> 1, anyFruitTextFont = 25, topIndex, i, currentMultiplier;
#elif defined(DISPLAY_RESOLUTION_WVGA)
	uint8_t  numFont = 30, rateOfChange = 3, /*payoutNumOffset=fontPixelHeight(numFont)>>1*/payoutNumOffset = ICON_HEIGHT / 2, anyFruitTextFont = 25, topIndex, i, currentMultiplier;
#endif
	static uint8_t anyFruitTextLength, payoutNumWidth, anyFruitTextHeight, firstTime = 1, multiplier = 5;
	static int16_t topPoint = 0, tableSize;
	int16_t startingX, startingY = 0, payoutSetHeight = PAYOUT_TABLE_SIZE * ICON_HEIGHT;

	scroll();  //updates payout table offset

	if (firstTime) {
		anyFruitTextLength = (uint8_t)stringPixelWidth(ANY_FRUIT_TEXT, anyFruitTextFont);
		payoutNumWidth = (uint8_t)unsignedNumberPixelWidth(3, numFont);
		anyFruitTextHeight = fontPixelHeight(anyFruitTextFont);
		tableSize = s_pHalContext->Height - STATUS_BAR_HEIGHT;

		firstTime = 0;
	}

	topPoint += JPot.payoutTableShift;
	rateOfChange = MAX(1, abs(JPot.payoutTableShift) >>2);
	if (JPot.payoutTableShift>0) {
		JPot.payoutTableShift -= rateOfChange;
		JPot.payoutTableShift = JPot.payoutTableShift < 0 ? 0 : JPot.payoutTableShift;
	}
	else if (JPot.payoutTableShift<0) {
		JPot.payoutTableShift += rateOfChange;
		JPot.payoutTableShift = JPot.payoutTableShift > 0 ? 0 : JPot.payoutTableShift;
	}

	if (topPoint<0) {
		topPoint = (payoutSetHeight + topPoint);
		multiplier++;
		if (multiplier>5) {
			multiplier = 1;
		}
	}
	else if (topPoint>payoutSetHeight) {
		topPoint = topPoint%payoutSetHeight;
		multiplier--;
		if (multiplier<1) {
			multiplier = 5;
		}

	}
	startingX = UI.spinColumnXOffset;
	topIndex = topPoint / ICON_HEIGHT;
	startingY = (topPoint%ICON_HEIGHT)*(-1);
	currentMultiplier = multiplier;

	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 255, 255));
	EVE_Cmd_wr32(s_pHalContext, BEGIN(RECTS));
	EVE_Cmd_wr32(s_pHalContext, LINE_WIDTH(16));
	EVE_Cmd_wr32(s_pHalContext, COLOR_A(0xff));
	EVE_Cmd_wr32(s_pHalContext, COLOR_MASK(1, 1, 1, 1));
	EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1));
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0, 0, 0));
#if defined(DISPLAY_RESOLUTION_WQVGA) || defined(DISPLAY_RESOLUTION_QVGA)
	EVE_Cmd_wr32(s_pHalContext, VERTEX2II(UI.spinColumnXOffset, 0, 0, 0));
	EVE_Cmd_wr32(s_pHalContext, VERTEX2II(UI.spinColumnXOffset + ICON_WIDTH * 3 + payoutNumWidth * 2, tableSize, 0, 0));
#elif defined(DISPLAY_RESOLUTION_WVGA)
	EVE_Cmd_wr32(s_pHalContext, VERTEX2F(UI.spinColumnXOffset * 16, 0));
	EVE_Cmd_wr32(s_pHalContext, VERTEX2F((UI.spinColumnXOffset + ICON_WIDTH * 3 + payoutNumWidth * 2) * 16, tableSize * 16));
#endif

	EVE_Cmd_wr32(s_pHalContext, COLOR_MASK(1, 1, 1, 1));
	EVE_Cmd_wr32(s_pHalContext, BLEND_FUNC(DST_ALPHA, ONE_MINUS_DST_ALPHA));
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 255, 255));
	EVE_Cmd_wr32(s_pHalContext, BEGIN(BITMAPS));

	//a maximum of 5 icons can be drawn with a drawing table of 220 pixels in height.
	for (i = 0; i<6; i++) {
		if (topIndex>14) {
			topIndex = 0;
			currentMultiplier--;
			if (currentMultiplier<1) {
				currentMultiplier = 5;
			}
		}

		if (topIndex != 14) {
			EVE_Cmd_wr32(s_pHalContext, BITMAP_HANDLE(topIndex));
			EVE_Cmd_wr32(s_pHalContext, VERTEX2F(startingX * 16, startingY * 16));
			startingX += (ICON_WIDTH + BUTTON_GAP);
		}
		else { //fruit combination is at index 14
			EVE_Cmd_wr32(s_pHalContext, BLEND_FUNC(SRC_ALPHA, ZERO));
			EVE_CoCmd_text(s_pHalContext, startingX, startingY + ((ICON_HEIGHT >> 1) - (anyFruitTextHeight >> 1)), anyFruitTextFont/*font*/, 0, ANY_FRUIT_TEXT);
			startingX += anyFruitTextLength + BUTTON_GAP;
			EVE_Cmd_wr32(s_pHalContext, BLEND_FUNC(DST_ALPHA, ONE_MINUS_DST_ALPHA));
		}

		//draw payout amount with the correct multipler
		EVE_Cmd_wr32(s_pHalContext, BLEND_FUNC(SRC_ALPHA, ZERO));
		startingX += BUTTON_GAP;
		EVE_CoCmd_text(s_pHalContext, startingX, startingY + payoutNumOffset, numFont, 0, "X");
		startingX += 20;
		EVE_CoCmd_number(s_pHalContext, startingX, startingY + payoutNumOffset, numFont, 0, currentMultiplier);
		startingX += ICON_WIDTH;
		if (currentMultiplier != 1 || topIndex == 14) {
			EVE_CoCmd_number(s_pHalContext, startingX, startingY + payoutNumOffset, numFont, 0, payoutTable[topIndex] * currentMultiplier);
		}
		else {
			EVE_CoCmd_number(s_pHalContext, startingX, startingY + payoutNumOffset, numFont, 0, 0);
		}
		EVE_Cmd_wr32(s_pHalContext, BLEND_FUNC(DST_ALPHA, ONE_MINUS_DST_ALPHA));

		startingY += ICON_HEIGHT;
		startingX = UI.spinColumnXOffset;
		topIndex++;
	}

	EVE_CoCmd_loadIdentity(s_pHalContext);
	EVE_CoCmd_setMatrix(s_pHalContext);
	EVE_Cmd_wr32(s_pHalContext, BLEND_FUNC(SRC_ALPHA, ONE_MINUS_SRC_ALPHA));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_HANDLE((15)));
}

void redrawColumnIcons() {
	int16_t topIndex, bottomIndex, i, startingX = UI.spinColumnXOffset, startingY = UI.spinColumnYOffset;

	EVE_Cmd_wr32(s_pHalContext, BEGIN(BITMAPS));

	for (i = 0; i<SPIN_COLUMNS; i++) {
		EVE_Cmd_wr32(s_pHalContext, COLOR_A(255));
		EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 255, 255));
		topIndex = columns[i].curIndex;
		bottomIndex = (topIndex + 2) % NUM_OF_ICONS;

		//draw icons from top to the bottom index
		while (1) {
			//the drawn icon in each column has a brighter color
			if ((columns[i].iconArray[topIndex] == JPot.lineBet[currentLine][i]) && JPot.displayRewardLine) {
				EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 255, 255));
#if defined(DISPLAY_RESOLUTION_WQVGA) | defined(DISPLAY_RESOLUTION_QVGA)
				EVE_Cmd_wr32(s_pHalContext, VERTEX2II(startingX, (startingY), columns[i].iconArray[topIndex], 0));
#elif defined(DISPLAY_RESOLUTION_WVGA)
				EVE_Cmd_wr32(s_pHalContext, BITMAP_HANDLE(columns[i].iconArray[topIndex]));
				EVE_Cmd_wr32(s_pHalContext, VERTEX2F(startingX * 16, startingY * 16));
#endif
				//icons in the winning line will have corresponding color box on top of it
				EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(betLines[currentLine].r, betLines[currentLine].g, betLines[currentLine].b));
				EVE_Cmd_wr32(s_pHalContext, LINE_WIDTH(16 * 2));
				EVE_Cmd_wr32(s_pHalContext, BEGIN(LINE_STRIP));
#if defined(DISPLAY_RESOLUTION_WQVGA) | defined(DISPLAY_RESOLUTION_QVGA)
				EVE_Cmd_wr32(s_pHalContext, VERTEX2II(startingX - 1, startingY - 1, 0, 0));
				EVE_Cmd_wr32(s_pHalContext, VERTEX2II(startingX + ICON_WIDTH + 1, startingY - 1, 0, 0));
				EVE_Cmd_wr32(s_pHalContext, VERTEX2II(startingX + ICON_WIDTH + 1, startingY + ICON_HEIGHT + 1, 0, 0));
				EVE_Cmd_wr32(s_pHalContext, VERTEX2II(startingX - 1, startingY + ICON_HEIGHT + 1, 0, 0));
				EVE_Cmd_wr32(s_pHalContext, VERTEX2II(startingX - 1, startingY - 1, 0, 0));
#elif defined(DISPLAY_RESOLUTION_WVGA)
				EVE_Cmd_wr32(s_pHalContext, VERTEX2F((startingX - 1) * 16, (startingY - 1) * 16));
				EVE_Cmd_wr32(s_pHalContext, VERTEX2F((startingX + ICON_WIDTH + 1) * 16, (startingY - 1) * 16));
				EVE_Cmd_wr32(s_pHalContext, VERTEX2F((startingX + ICON_WIDTH + 1) * 16, (startingY + ICON_HEIGHT + 1) * 16));
				EVE_Cmd_wr32(s_pHalContext, VERTEX2F((startingX - 1) * 16, (startingY + ICON_HEIGHT + 1) * 16));
				EVE_Cmd_wr32(s_pHalContext, VERTEX2F((startingX - 1) * 16, (startingY - 1) * 16));
#endif

				EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 255, 255));
				EVE_Cmd_wr32(s_pHalContext, BEGIN(BITMAPS));
			}

			if (topIndex == bottomIndex) {
				break;
			}
			startingY += ICON_HEIGHT;
			topIndex = (topIndex + 1) % NUM_OF_ICONS;
		}

		startingX += (ICON_WIDTH + COLUMN_GAP);
		startingY = UI.spinColumnYOffset;
	}
	EVE_Cmd_wr32(s_pHalContext, BLEND_FUNC(SRC_ALPHA, ONE_MINUS_SRC_ALPHA));
	EVE_Cmd_wr32(s_pHalContext, BITMAP_HANDLE(15));
}

void drawSpinColumns() {
	int16_t topIndex, bottomIndex, i, j, startingX = UI.spinColumnXOffset, startingY = UI.spinColumnYOffset, highLightingIndex;
	static schar8_t bounceOffset;

	updateIndex();
	EVE_Cmd_wr32(s_pHalContext, BEGIN(BITMAPS));
	bounceOffset = 0;
	JPot.stoppedColumns = 0;
	for (i = 0; i<SPIN_COLUMNS; i++) {
		EVE_Cmd_wr32(s_pHalContext, COLOR_A(255));
		EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 255, 255));
		topIndex = columns[i].curIndex;
		bottomIndex = (topIndex + 2) % NUM_OF_ICONS;
		highLightingIndex = (columns[i].curIndex + 1) % NUM_OF_ICONS;

		//bouncing effect happens after the column was stopped
		if (columns[i].velocity <= 0) {
			if (columns[i].bounceAmount>0) {
				bounceOffset = columns[i].bounceOffset = columns[i].bounceOffset - 5;
				if (bounceOffset<0) {
					bounceOffset = columns[i].bounceOffset = columns[i].bounceAmount = columns[i].bounceAmount >> 1;
				}
			}
			else {
				JPot.stoppedColumns++;
			}
		}
		else {
			Play_Sound(s_pHalContext, SPINNING_SOUND, 255, 0x00);  //spinning sound for each column
		}

		//draw icons from top to the bottom index
		while (topIndex != bottomIndex) {
			//the drawn icon in each column has a brighter color
			if ((topIndex == highLightingIndex) && (columns[i].velocity <= 0)) {
				//columns[i].drawnIndex=columns[i].iconArray[highLightingIndex];
				columns[i].drawnIndex = highLightingIndex;
				EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 255, 255));
#if defined(DISPLAY_RESOLUTION_WQVGA) | defined(DISPLAY_RESOLUTION_QVGA)
				EVE_Cmd_wr32(s_pHalContext, VERTEX2II(startingX, (startingY + bounceOffset), columns[i].iconArray[topIndex], 0));
#elif defined(DISPLAY_RESOLUTION_WVGA)
				EVE_Cmd_wr32(s_pHalContext, BITMAP_HANDLE(columns[i].iconArray[topIndex]));
				EVE_Cmd_wr32(s_pHalContext, VERTEX2F(startingX * 16, (startingY + bounceOffset) * 16));
#endif
				//icons in the winning combination will have a red box on top of it.
				if (JPot.winningIndex[i] != 255 && JPot.rewardedPoints) {
					//highlight box
					EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 0, 0));
					EVE_Cmd_wr32(s_pHalContext, LINE_WIDTH(16 * 2));
					EVE_Cmd_wr32(s_pHalContext, BEGIN(LINES));
#if defined(DISPLAY_RESOLUTION_WQVGA) | defined(DISPLAY_RESOLUTION_QVGA)
					//top line
					EVE_Cmd_wr32(s_pHalContext, VERTEX2II(startingX + 2, startingY + bounceOffset, 0, 0));
					EVE_Cmd_wr32(s_pHalContext, VERTEX2II(startingX + ICON_WIDTH, startingY + bounceOffset, 0, 0));
					//left line
					EVE_Cmd_wr32(s_pHalContext, VERTEX2II(startingX + 3, startingY + bounceOffset, 0, 0));
					EVE_Cmd_wr32(s_pHalContext, VERTEX2II(startingX + 3, startingY + ICON_HEIGHT + bounceOffset, 0, 0));
					//right line
					EVE_Cmd_wr32(s_pHalContext, VERTEX2II(startingX + ICON_WIDTH - 1, startingY + bounceOffset, 0, 0));
					EVE_Cmd_wr32(s_pHalContext, VERTEX2II(startingX + ICON_WIDTH - 1, startingY + ICON_HEIGHT + bounceOffset, 0, 0));
					//bottom line
					EVE_Cmd_wr32(s_pHalContext, VERTEX2II(startingX - 1, startingY + ICON_HEIGHT + bounceOffset - 1, 0, 0));
					EVE_Cmd_wr32(s_pHalContext, VERTEX2II(startingX + ICON_WIDTH - 1, startingY + ICON_HEIGHT + bounceOffset - 1, 0, 0));
#elif defined(DISPLAY_RESOLUTION_WVGA)
					//top line
					EVE_Cmd_wr32(s_pHalContext, VERTEX2F((startingX + 2) * 16, (startingY + bounceOffset) * 16));
					EVE_Cmd_wr32(s_pHalContext, VERTEX2F((startingX + ICON_WIDTH) * 16, (startingY + bounceOffset) * 16));
					//left line							
					EVE_Cmd_wr32(s_pHalContext, VERTEX2F((startingX + 3) * 16, (startingY + bounceOffset) * 16));
					EVE_Cmd_wr32(s_pHalContext, VERTEX2F((startingX + 3) * 16, (startingY + ICON_HEIGHT + bounceOffset) * 16));
					//right line						
					EVE_Cmd_wr32(s_pHalContext, VERTEX2F((startingX + ICON_WIDTH - 1) * 16, (startingY + bounceOffset) * 16));
					EVE_Cmd_wr32(s_pHalContext, VERTEX2F((startingX + ICON_WIDTH - 1) * 16, (startingY + ICON_HEIGHT + bounceOffset) * 16));
					//bottom line						
					EVE_Cmd_wr32(s_pHalContext, VERTEX2F((startingX - 1) * 16, (startingY + ICON_HEIGHT + bounceOffset - 1) * 16));
					EVE_Cmd_wr32(s_pHalContext, VERTEX2F((startingX + ICON_WIDTH - 1) * 16, (startingY + ICON_HEIGHT + bounceOffset - 1) * 16));
#endif
					EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 255, 255));
					EVE_Cmd_wr32(s_pHalContext, BEGIN(BITMAPS));
				}
			}
			else {
#if defined(DISPLAY_RESOLUTION_WQVGA) | defined(DISPLAY_RESOLUTION_QVGA)
				EVE_Cmd_wr32(s_pHalContext, VERTEX2II(startingX, (startingY + bounceOffset), columns[i].iconArray[topIndex], 0));
#elif defined(DISPLAY_RESOLUTION_WVGA)
				EVE_Cmd_wr32(s_pHalContext, BITMAP_HANDLE(columns[i].iconArray[topIndex]));
				EVE_Cmd_wr32(s_pHalContext, VERTEX2F(startingX * 16, (startingY + bounceOffset) * 16));
#endif
			}
			startingY += ICON_HEIGHT;
			topIndex = (topIndex + 1) % NUM_OF_ICONS;
		}

#if defined(DISPLAY_RESOLUTION_WQVGA) | defined(DISPLAY_RESOLUTION_QVGA)
		EVE_Cmd_wr32(s_pHalContext, VERTEX2II(startingX, (startingY + bounceOffset), columns[i].iconArray[topIndex], 0));
#elif defined(DISPLAY_RESOLUTION_WVGA)
		EVE_Cmd_wr32(s_pHalContext, BITMAP_HANDLE(columns[i].iconArray[topIndex]));
		EVE_Cmd_wr32(s_pHalContext, VERTEX2F(startingX * 16, (startingY + bounceOffset) * 16));
#endif

		EVE_Cmd_wr32(s_pHalContext, BITMAP_HANDLE(15));
		EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0, 0, 0));
		EVE_Cmd_wr32(s_pHalContext, COLOR_A(255));

		//draw the overlay, which is exactly on top of the bitmaps
		EVE_Cmd_wr32(s_pHalContext, BITMAP_SOURCE(bitmapInfo[9].Offset));
		EVE_Cmd_wr32(s_pHalContext, BITMAP_LAYOUT(L8, bitmapInfo[9].Stride, bitmapInfo[9].Height));
		EVE_Cmd_wr32(s_pHalContext, BITMAP_SIZE(NEAREST, BORDER, BORDER, bitmapInfo[9].Width, bitmapInfo[9].Height));
		EVE_Cmd_wr32(s_pHalContext, VERTEX2F((startingX) * 16, (UI.spinColumnYOffset) * 16));

		//draw the outer overlay, some offsets
		EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 255, 255));
		EVE_Cmd_wr32(s_pHalContext, BITMAP_SOURCE(bitmapInfo[10].Offset));
		EVE_Cmd_wr32(s_pHalContext, BITMAP_LAYOUT(ARGB4, bitmapInfo[10].Stride, bitmapInfo[10].Height));
		EVE_Cmd_wr32(s_pHalContext, BITMAP_SIZE(NEAREST, BORDER, BORDER, bitmapInfo[10].Width, bitmapInfo[10].Height));
#if defined(DISPLAY_RESOLUTION_WQVGA) | defined(DISPLAY_RESOLUTION_QVGA)
		EVE_Cmd_wr32(s_pHalContext, VERTEX2F((startingX - 3) * 16, (UI.spinColumnYOffset - 12) * 16));
#elif defined(DISPLAY_RESOLUTION_WVGA)
		EVE_Cmd_wr32(s_pHalContext, VERTEX2F((startingX - 6) * 16, (UI.spinColumnYOffset - 20) * 16));
#endif

		//selected columns indication: dots
		if ((i + 1) <= JPot.spinColumnSelected) {
			EVE_Cmd_wr32(s_pHalContext, POINT_SIZE(6 * 16));
			EVE_Cmd_wr32(s_pHalContext, BEGIN(FTPOINTS));
#if defined(DISPLAY_RESOLUTION_WQVGA) | defined(DISPLAY_RESOLUTION_QVGA)
			EVE_Cmd_wr32(s_pHalContext, VERTEX2II(startingX + (ICON_WIDTH >> 1), UI.spinColumnYOffset - 6, 0, 0));
			EVE_Cmd_wr32(s_pHalContext, VERTEX2II(startingX + (ICON_WIDTH >> 1), startingY + ICON_HEIGHT + 6, 0, 0));
#elif defined(DISPLAY_RESOLUTION_WVGA)
			EVE_Cmd_wr32(s_pHalContext, VERTEX2F((startingX + (ICON_WIDTH >> 1)) * 16, (UI.spinColumnYOffset - 6) * 16));
			EVE_Cmd_wr32(s_pHalContext, VERTEX2F((startingX + (ICON_WIDTH >> 1)) * 16, (startingY + ICON_HEIGHT + 6) * 16));
#endif
			EVE_Cmd_wr32(s_pHalContext, BEGIN(BITMAPS));
		}

		startingX += (ICON_WIDTH + COLUMN_GAP);
		startingY = UI.spinColumnYOffset;
		bounceOffset = 0;
	}
	EVE_Cmd_wr32(s_pHalContext, BLEND_FUNC(SRC_ALPHA, ONE_MINUS_SRC_ALPHA));
}

//payout table scrolling
void scroll() {
	static uint16_t curScreenY, preScreenY1, preScreenY2;
	static bool_t continueTouch = FALSE;
	if (!continueTouch && EVE_Hal_rd8(s_pHalContext, REG_TOUCH_TAG) == 200) {
		continueTouch = TRUE;
		curScreenY = EVE_Hal_rd16(s_pHalContext, REG_TOUCH_SCREEN_XY);
		preScreenY2 = preScreenY1 = curScreenY;
	}
	//scroll while pendown
	if (continueTouch && !JPot.pendown) {
		preScreenY2 = preScreenY1;
		curScreenY = EVE_Hal_rd16(s_pHalContext, REG_TOUCH_SCREEN_XY);
		if ((curScreenY != 32768) && (preScreenY1 != 32768)) {
			if (curScreenY != preScreenY1) {
				JPot.payoutTableShift = (preScreenY1 - curScreenY);
			}
		}
	}

	//scroll on release
	if (continueTouch && JPot.pendown) {
		if (preScreenY2 != 32768 && curScreenY != 32768) {
			JPot.payoutTableShift = (preScreenY2 - curScreenY) << 1;
			if (JPot.payoutTableShift > 128)
				JPot.payoutTableShift = 128;
			else if (JPot.payoutTableShift < -128)
				JPot.payoutTableShift = -128;
		}
		continueTouch = FALSE;
	}
	preScreenY1 = curScreenY;
}

//decrease the spinning column velocity afer the spin button has been released
void updateIndex() {
	static uint8_t i;
	uint16_t pressure;

	if (JPot.spinning) {
		//button has been released
		if (JPot.released) {
			for (i = 0; i<SPIN_COLUMNS; i++) {
				if (columns[i].velocity>0) {
					columns[i].velocity = (columns[i].velocity - 1) <= 0 ? 0 : (columns[i].velocity - 1);
					columns[i].curIndex = (columns[i].curIndex + i + 1) % NUM_OF_ICONS;
				}
			}
		}
		//button is being pressed
		else if (JPot.touchTag == 201) {
			pressure = EVE_Hal_rd16(s_pHalContext, REG_TOUCH_RZ);
			for (i = 0; i<SPIN_COLUMNS; i++) {
				columns[i].velocity = ((NORMAL_PRESSURE - pressure)*MAX_ADDITIONAL_VEL) / NORMAL_PRESSURE;
				//if the the touch was barely sensed
				if (columns[i].velocity<10) {
					columns[i].velocity = (BASE_VELOCITY + (nextRandomInt8(0) & 31));
				}
				columns[i].curIndex = (columns[i].curIndex + 1 + (nextRandomInt8(0) & 3)) % NUM_OF_ICONS;
			}
		}
		else {
			pressure = EVE_Hal_rd16(s_pHalContext, REG_TOUCH_RZ);
			for (i = 0; i<SPIN_COLUMNS; i++) {
				columns[i].velocity = ((NORMAL_PRESSURE - pressure)*MAX_ADDITIONAL_VEL) / NORMAL_PRESSURE;
				//if the the touch was barely sensed
				if (columns[i].velocity<10) {
					columns[i].velocity = (BASE_VELOCITY + (nextRandomInt8(0) & 31));
				}
				columns[i].curIndex = (columns[i].curIndex + 1 + (nextRandomInt8(0) & 3)) % NUM_OF_ICONS;
			}
			JPot.released = TRUE;
		}
	}
}

bool_t ableToSpin() {
	if (JPot.balance >= JPot.totalBetAmount && !JPot.spinning) {
		return TRUE;
	}
	else {
		return FALSE;
	}
}

bool_t ableToContinue() {
	if (JPot.balance >= POINTS_PER_COLUMN) {
		return TRUE;
	}
	else {
		return FALSE;
	}
}

void unsignedNumToAsciiArray(char8_t *buf, uint16_t num) {
	uint8_t temp, i, index = 0, last;
	do {
		buf[index] = (num % 10) + 48;
		index++;
		num /= 10;
	} while (num != 0);
	buf[index] = '\0';

	last = index - 1;
	for (i = 0; i < index / 2; i++) {
		temp = buf[i];
		buf[i] = buf[last];
		buf[last] = temp;
		last--;
	}
}

void touched(uint8_t touchVal) {
	static uint8_t i, j;
	uint8_t tempVal = 0, betMax = 10, lineBetMax = 10;
	JPot.touchTag = touchVal;

	/* This get the pendown state by sensing ADC sensor. Not support in BT8XXEMU_PLATFORM (default=0) */
	/* REG_TOUCH_DIRECT_XY bit 31: 0 = touched, 1 = non-touched */
	JPot.pendown = (uint8_t)((EVE_Hal_rd32(s_pHalContext, REG_TOUCH_DIRECT_XY) >> 31) & 0x1);
	if (JPot.spinning && !JPot.released && JPot.pendown) {
		JPot.released = TRUE;
		for (i = 0; i<SPIN_COLUMNS; i++) {
			nextRandomInt8((GET_CURR_MILLIS()) & 0xFF);
			columns[i].velocity = (BASE_VELOCITY + ((nextRandomInt8(0) >> 4) & 31L));
		}
	}
	if (touchVal != 0 && touchVal != 200 && touchVal != 201) {
		Play_Sound(s_pHalContext, BUTTON_PRESS_SOUND, 255, 0x00);
	}

	//bet line selection
	if (touchVal>0 && touchVal <= 12) {
		if (!JPot.spinning) {
			JPot.selectedBetLine = touchVal;
			JPot.totalBetAmount = POINTS_PER_COLUMN * JPot.spinColumnSelected + JPot.selectedBetLine * POINTS_PER_COLUMN;
			JPot.displayRewardLine = FALSE;
		}
		return;
	}

	//multiplier selection
	if (touchVal >= 100 && touchVal<100 + SPIN_COLUMNS) {
		JPot.selectedBetMultiplier = touchVal - 100;
		return;
	}

	//status bar buttons
	switch (touchVal) {
	case 201: //spin button
		if (ableToSpin() && !JPot.showPayoutTable) {
			JPot.spinning = TRUE;
			JPot.released = FALSE;
			JPot.rewardedPoints = FALSE;
			JPot.balance -= JPot.totalBetAmount;
			if (ableToContinue()) {
				JPot.reset = FALSE;
			}
			for (i = 0; i<SPIN_COLUMNS; i++) {
				columns[i].velocity = BASE_VELOCITY;
				columns[i].bounceAmount = columns[i].bounceOffset = BOUNCE_AMOUNT;
			}
		}
		break;
	case 202:  //column betting increase button
		if (!JPot.spinning && !JPot.showPayoutTable) {
			if (JPot.spinColumnSelected + 1 <= SPIN_COLUMNS && JPot.balance >= POINTS_PER_COLUMN) {
				JPot.spinColumnSelected++;
				JPot.totalBetAmount = POINTS_PER_COLUMN * JPot.spinColumnSelected + JPot.selectedBetLine * POINTS_PER_COLUMN;
				if (ableToContinue()) {
					JPot.reset = FALSE;
				}
			}
		}
		break;
	case 203:  //column betting decrease button
		if (!JPot.spinning && !JPot.showPayoutTable) {
			if (JPot.spinColumnSelected - 1 >= 1) {
				JPot.spinColumnSelected--;
				JPot.totalBetAmount = POINTS_PER_COLUMN * JPot.spinColumnSelected + JPot.selectedBetLine * POINTS_PER_COLUMN;
				if (ableToContinue()) {
					JPot.reset = FALSE;
				}
			}

		}
		break;
	case 204:  //line betting increase button
		if (!JPot.spinning && !JPot.showPayoutTable) {
			JPot.selectedBetLine = (JPot.selectedBetLine + 1) > TOTAL_BET_LINES ? TOTAL_BET_LINES : (JPot.selectedBetLine + 1);
			JPot.totalBetAmount = POINTS_PER_COLUMN * JPot.spinColumnSelected + JPot.selectedBetLine * POINTS_PER_COLUMN;
			if (ableToContinue()) {
				JPot.reset = FALSE;
			}
			JPot.betChanged = TRUE;
		}

		break;
	case 205:  //line betting decrease
		if (!JPot.spinning && !JPot.showPayoutTable) {
			JPot.selectedBetLine = (JPot.selectedBetLine - 1) < 0 ? 0 : (JPot.selectedBetLine - 1);
			JPot.totalBetAmount = POINTS_PER_COLUMN * JPot.spinColumnSelected + JPot.selectedBetLine * POINTS_PER_COLUMN;
			if (ableToContinue()) {
				JPot.reset = FALSE;
			}
			JPot.betChanged = TRUE;
		}
		break;
	case 206:  //reset game button
		if (!JPot.spinning && !JPot.showPayoutTable) {
			if (JPot.reset) {
				JPot.balance = INITIAL_BALANCE;
				JPot.selectedBetLine = 1;
				if (ableToContinue()) {
					JPot.reset = FALSE;
				}
			}
			else {
				JPot.selectedBetLine = 1;
				JPot.spinColumnSelected = 1;
				JPot.totalBetAmount = POINTS_PER_COLUMN;
				JPot.reset = TRUE;
			}
		}
		break;
	case 207: //payout table button
		if (ableToContinue()) {
			JPot.reset = FALSE;
		}
		if (JPot.showPayoutTable) {
			JPot.showPayoutTable = FALSE;
		}
		else {
			JPot.showPayoutTable = TRUE;
		}
		JPot.betChanged = TRUE;
		break;

	}
}

//xor shift random number generation
uint8_t nextRandomInt8(uint8_t seed) {
	static uint8_t x = 111;
	if (seed != 0) x = seed;
	x ^= (x << 7);
	x ^= (x >> 5);
	return x ^= (x << 3);
}

void clearArray(uint8_t* index, uint16_t size, uint8_t defaultValue) {
	uint16_t i;
	for (i = 0; i<size; i++) {
		index[i] = defaultValue;
	}
}

uint16_t getPoints() {
	uint8_t i, j, lookingFor = 5, fruitCount = 0, fruitTableSize = 7, tempMin, tempVal = 0;
	uint8_t fruitTable[] = { 3,5,6,9,10,11,12 };
	uint8_t tempIndex[NUM_OF_ICONS];
	uint16_t points = 0;
	bool_t finished = FALSE;

	JPot.rewardedPoints = TRUE;
	clearArray(JPot.winningIndex, SPIN_COLUMNS, 255);
	clearArray(tempIndex, NUM_OF_ICONS, 0);
	//count the winning indices
	for (i = 0; i<JPot.spinColumnSelected; i++) {
		tempIndex[columns[i].iconArray[columns[i].drawnIndex]] += 1;
	}

	for (i = 0; i<NUM_OF_ICONS;) {
		//check for combination with the highest payout value first.
		if (tempIndex[i] >= lookingFor) {
			for (j = 0; j<JPot.spinColumnSelected; j++) {
				if (columns[j].iconArray[columns[j].drawnIndex] == i) {
					JPot.winningIndex[j] = tempIndex[i];
				}
			}
			if (lookingFor>2) {
				return payoutTable[i] * lookingFor;
			}
			else if (lookingFor == 2) {
				points += payoutTable[i] * 2;
			}
		}
		i++;
		if (i == NUM_OF_ICONS) {
			lookingFor--;
			i = 0;
			if (lookingFor<2) {
				if (points>0) {
					return points;
				}
				break;
			}
		}
	}

	//check for any fruits
	for (i = 0; i<fruitTableSize; i++) {
		for (j = 0; j<JPot.spinColumnSelected; j++) {
			if (fruitTable[i] == columns[j].iconArray[columns[j].drawnIndex]) {
				JPot.winningIndex[j] = columns[j].iconArray[columns[j].drawnIndex];
				fruitCount++;
				tempVal++;
			}
		}
	}

	return fruitCount*payoutTable[14];
}

void startingAnimation() {
	uint8_t i;
	JPot.spinning = TRUE;
	JPot.released = TRUE;
	EVE_sleep(1000);

	for (i = 0; i<SPIN_COLUMNS; i++) {
		columns[i].bounceAmount = columns[i].bounceOffset = BOUNCE_AMOUNT;
	}

	while (1) {
		EVE_CoCmd_dlStart(s_pHalContext);
		EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1));
		EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 255, 255));
		updateIndex();
		drawSpinColumns();
		EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 0, 0));

#if defined(DISPLAY_RESOLUTION_WQVGA)
		EVE_CoCmd_text(s_pHalContext, s_pHalContext->Width >> 1, UI.spinColumnYOffset + UI.visibleIconsHeight + 30, 31, OPT_CENTER, "JACKPOT by Bridgetek");
#elif defined(DISPLAY_RESOLUTION_QVGA)
		EVE_CoCmd_text(s_pHalContext, s_pHalContext->Width >> 1, UI.spinColumnYOffset + UI.visibleIconsHeight + 30, 30, OPT_CENTER, "JACKPOT by Bridgetek");
#elif defined(DISPLAY_RESOLUTION_WVGA)
		EVE_CoCmd_text(s_pHalContext, s_pHalContext->Width >> 1, UI.spinColumnYOffset + UI.visibleIconsHeight + 60, 31, OPT_CENTER, "JACKPOT by Bridgetek");
#endif

		EVE_Cmd_wr32(s_pHalContext, DISPLAY());
		EVE_CoCmd_swap(s_pHalContext);
		EVE_Cmd_waitFlush(s_pHalContext);
		if (JPot.stoppedColumns == SPIN_COLUMNS) {
			break;
		}
	}

	JPot.spinning = FALSE;
	JPot.released = FALSE;
	JPot.spinColumnSelected = SPIN_COLUMNS;

	for (i = 0; i<8; i++) {
		spinCoins[i].fall = FALSE;
		spinCoins[i].xPos = s_pHalContext->Width >> 1;
		spinCoins[i].yPos = s_pHalContext->Height / 3;
		spinCoins[i].yVel = -2 + (nextRandomInt8(0) & 3)*-1;
		nextRandomInt8(0);
		if ((nextRandomInt8(0) >> 7) & 1) {
			spinCoins[i].xVel = -4 + (nextRandomInt8(0) & 3)*-1;
		}
		else {
			spinCoins[i].xVel = 4 + (nextRandomInt8(0) & 3);
		}
		spinCoins[i].index = (nextRandomInt8(0) & 7);
	}

	for (i = 0; i<8; i++) {
		Play_Sound(s_pHalContext, COIN_REWARD_SOUND, 255, 0x00);
	}
}

//display fading out reward amount
void displayRewardText(uint16_t points) {
	uint8_t limit = 10;
	static uint8_t counter = 0;
	static uint16_t lastPoints = 0;
	static bool_t pointSet = FALSE;

	if (points != lastPoints && !pointSet) {
		counter = 255;
		lastPoints = points;
		pointSet = TRUE;
	}
	else if (points != lastPoints && pointSet) {
		counter = 255;
		lastPoints = points;
		pointSet = TRUE;
	}

	if (counter>limit) {
		if (!JPot.spinning && points>0) {
			EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 0, 0));
			EVE_Cmd_wr32(s_pHalContext, COLOR_A(counter));
			EVE_CoCmd_text(s_pHalContext, s_pHalContext->Width >> 1, ICON_HEIGHT * 2 + (ICON_HEIGHT >> 1), 31, OPT_RIGHTX, "+ ");
			EVE_CoCmd_number(s_pHalContext, s_pHalContext->Width >> 1, ICON_HEIGHT * 2 + (ICON_HEIGHT >> 1), 31, 0, points);
			EVE_Cmd_wr32(s_pHalContext, COLOR_A(255));
		}
		else {
			lastPoints = 0;
		}
		counter -= 5;
		if (counter <= limit) {
			lastPoints = points;
			pointSet = FALSE;
		}
	}
}

//selected a different bet line button will display the corresponding indexed bitmap/text for a brief moment
void displaySelectedIcon(uint8_t index) {
	uint8_t limit = 50;
	static uint8_t counter = 0, lastIndex = 0;
	static bool_t counterSet = FALSE;

	if (index != lastIndex && !counterSet) {
		counter = 255;
		lastIndex = index;
		counterSet = TRUE;
	}
	else if (index != lastIndex && counterSet) {
		counter = 255;
		lastIndex = index;
		counterSet = TRUE;
	}

	if (counter>limit) {
		if (!JPot.spinning) {
			EVE_Cmd_wr32(s_pHalContext, BEGIN(BITMAPS));
			EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 255, 255));
			EVE_Cmd_wr32(s_pHalContext, COLOR_A(counter));
			if (index != 14) {
				EVE_Cmd_wr32(s_pHalContext, VERTEX2II(s_pHalContext->Width >> 1, s_pHalContext->Height >> 1, index, 0));
			}
			else {
				EVE_CoCmd_text(s_pHalContext, s_pHalContext->Width >> 1, s_pHalContext->Height >> 1, 25, 0, ANY_FRUIT_TEXT);
			}
			EVE_Cmd_wr32(s_pHalContext, COLOR_A(255));
		}
		counter -= 10;
		if (counter <= limit) {
			lastIndex = index;
			counterSet = FALSE;
		}
	}
}

void lineBetButtons() {
#if defined(DISPLAY_RESOLUTION_WQVGA)
	uint8_t i, buttonWidth = 40, buttonHeight = 18, font = 22, tempIndex = 0;
	int16_t lineButtonStartingX = UI.spinColumnXOffset - 50;
	int16_t lineButtonStartingY = UI.spinColumnYOffset;
#elif defined(DISPLAY_RESOLUTION_QVGA)
	uint8_t i, buttonWidth = 35, buttonHeight = 15, font = 22, tempIndex = 0;
	int16_t lineButtonStartingX = 0;
	int16_t lineButtonStartingY = UI.spinColumnYOffset + 7;
#elif defined(DISPLAY_RESOLUTION_WVGA)  //needs to change for the larger screen size
	uint8_t i, buttonWidth = 65, buttonHeight = 36, font = 28, tempIndex = 0;
	int16_t lineButtonStartingX = UI.spinColumnXOffset - 80;
	int16_t lineButtonStartingY = UI.spinColumnYOffset;
#endif

	char8_t name[4];
	char8_t points[5];
	char8_t value[10];

	uint32_t normalButtonColor = 0x8B8682;
	EVE_Cmd_wr32(s_pHalContext, TAG_MASK(1));
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 255, 255));
	EVE_CoCmd_fgColor(s_pHalContext, 0xB8860B);
	//line button
	for (i = 0; i<TOTAL_BET_LINES; i++) {
		name[0] = '\0';
		Gpu_Hal_Dec2Ascii(name, i + 1);
		EVE_Cmd_wr32(s_pHalContext, TAG(i + 1));
		if (temp_tag == (i + 1)) {
			EVE_CoCmd_button(s_pHalContext, lineButtonStartingX, lineButtonStartingY, buttonWidth, buttonHeight, font, OPT_FLAT, name);
		}
		else if (JPot.selectedBetLine == (i + 1)) {  //current line is selected
			EVE_CoCmd_fgColor(s_pHalContext, 0x0000CD);
			EVE_CoCmd_button(s_pHalContext, lineButtonStartingX, lineButtonStartingY, buttonWidth, buttonHeight, font, OPT_FLAT, name);
			EVE_CoCmd_fgColor(s_pHalContext, 0xB8860B);
		}
		else {
			EVE_CoCmd_button(s_pHalContext, lineButtonStartingX, lineButtonStartingY, buttonWidth, buttonHeight, font, 0, name);
		}

		lineButtonStartingY += (buttonHeight + 12);
		if (i == 5) {
#if defined(DISPLAY_RESOLUTION_WVGA)
			lineButtonStartingY = UI.spinColumnYOffset;
			lineButtonStartingX = UI.spinColumnXOffset + (SPIN_COLUMNS*ICON_WIDTH + SPIN_COLUMNS * COLUMN_GAP + 5);
#elif defined(DISPLAY_RESOLUTION_WQVGA)
			lineButtonStartingY = UI.spinColumnYOffset;
			lineButtonStartingX = UI.spinColumnXOffset + (SPIN_COLUMNS*ICON_WIDTH + SPIN_COLUMNS * COLUMN_GAP);
#elif defined(DISPLAY_RESOLUTION_QVGA)
			lineButtonStartingY = UI.spinColumnYOffset;
			lineButtonStartingX = UI.spinColumnXOffset + (SPIN_COLUMNS*ICON_WIDTH + SPIN_COLUMNS * COLUMN_GAP);
#endif 
		}
	}
}

void statusBar() {
	static bool_t firstTime = 1;
	static uint8_t betTextPixelLength, columnTextPixelLength, lineTextPixelLength, balanceTextPixelLength, resetTextPixelLength, payoutTextPixelLength;
	static uint8_t count = 0, payoutTableButtonHeight;
	static uint8_t font = 18, betLineFont = 27;

	if (firstTime) {
		balanceTextPixelLength = (uint8_t)stringPixelWidth(BALANCE_TEXT, betLineFont);
		resetTextPixelLength = (uint8_t)stringPixelWidth(RESET_TEXT, 22);
		payoutTextPixelLength = (uint8_t)stringPixelWidth(PAYOUT_TEXT, 22);
#if defined(DISPLAY_RESOLUTION_QVGA)
		betLineFont = 22;
		payoutTableButtonHeight = 17;
#elif defined(DISPLAY_RESOLUTION_WQVGA)
		betLineFont = 27;
		payoutTableButtonHeight = 17;
#elif defined(DISPLAY_RESOLUTION_WVGA)
		betLineFont = 30;
		payoutTableButtonHeight = 38;
#endif
		betTextPixelLength = (uint8_t)stringPixelWidth(BET_TEXT, betLineFont);
		lineTextPixelLength = (uint8_t)stringPixelWidth(LINE_TEXT, betLineFont);
		columnTextPixelLength = (uint8_t)stringPixelWidth(COLUMN_TEXT, betLineFont);
		firstTime = 0;
	}

	EVE_Cmd_wr32(s_pHalContext, TAG(0));

	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 255, 255));
	EVE_Cmd_wr32(s_pHalContext, BEGIN(BITMAPS));

	EVE_Cmd_wr32(s_pHalContext, BLEND_FUNC(SRC_ALPHA, ZERO));
	EVE_Cmd_wr32(s_pHalContext, VERTEX2II(0, s_pHalContext->Height - STATUS_BAR_HEIGHT, 14, 0));
	EVE_Cmd_wr32(s_pHalContext, BLEND_FUNC(SRC_ALPHA, ONE_MINUS_SRC_ALPHA));

	//betting column amount
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0, 0, 0));
	EVE_CoCmd_text(s_pHalContext, (s_pHalContext->Width >> 1) + BUTTON_GAP, s_pHalContext->Height - ((STATUS_BAR_HEIGHT * 3) >> 2) - BUTTON_GAP * 2, betLineFont, 0, COLUMN_TEXT);
	EVE_CoCmd_number(s_pHalContext, (s_pHalContext->Width >> 1) + BUTTON_GAP + columnTextPixelLength, s_pHalContext->Height - ((STATUS_BAR_HEIGHT * 3) >> 2) - BUTTON_GAP * 2, betLineFont, 0, JPot.spinColumnSelected);

	//total bet amount
#if defined(DISPLAY_RESOLUTION_QVGA)
	EVE_CoCmd_text(s_pHalContext, resetTextPixelLength + BUTTON_GAP, s_pHalContext->Height - ((STATUS_BAR_HEIGHT * 3) >> 2) - BUTTON_GAP * 2, betLineFont, 0, BET_TEXT);
	EVE_CoCmd_number(s_pHalContext, resetTextPixelLength + BUTTON_GAP * 2 + betTextPixelLength, s_pHalContext->Height - ((STATUS_BAR_HEIGHT * 3) >> 2) - BUTTON_GAP * 2, betLineFont, 0, JPot.totalBetAmount);
#elif defined(DISPLAY_RESOLUTION_WQVGA) || defined(DISPLAY_RESOLUTION_WVGA)
	EVE_CoCmd_text(s_pHalContext, resetTextPixelLength + BUTTON_GAP + 10, s_pHalContext->Height - ((STATUS_BAR_HEIGHT * 3) >> 2) - BUTTON_GAP * 2, betLineFont, 0, BET_TEXT);
	EVE_CoCmd_number(s_pHalContext, resetTextPixelLength + BUTTON_GAP * 2 + betTextPixelLength + 8, s_pHalContext->Height - ((STATUS_BAR_HEIGHT * 3) >> 2) - BUTTON_GAP * 2, betLineFont, 0, JPot.totalBetAmount);
#endif

	//selected bet line and its bet amount
	EVE_CoCmd_text(s_pHalContext, (s_pHalContext->Width >> 1) - BUTTON_GAP - (UI.spinButtonWidth >> 1) * 2 - 2, s_pHalContext->Height - ((STATUS_BAR_HEIGHT * 3) >> 2) - BUTTON_GAP * 2, betLineFont, 0, LINE_TEXT);
	EVE_CoCmd_number(s_pHalContext, (s_pHalContext->Width >> 1) - BUTTON_GAP - (UI.spinButtonWidth >> 1) * 2 - 2 + lineTextPixelLength, s_pHalContext->Height - ((STATUS_BAR_HEIGHT * 3) >> 2) - BUTTON_GAP * 2, betLineFont, 0, JPot.selectedBetLine);

	//balance text and value
	EVE_Cmd_wr32(s_pHalContext, LINE_WIDTH(BUTTON_CURVATURE * 16));

#if defined(DISPLAY_RESOLUTION_WVGA)
	EVE_CoCmd_text(s_pHalContext, BUTTON_CURVATURE * 2, s_pHalContext->Height - (STATUS_BAR_HEIGHT >> 1), betLineFont, 0, BALANCE_TEXT);
	EVE_CoCmd_number(s_pHalContext, BUTTON_CURVATURE * 10 + betTextPixelLength + 35, s_pHalContext->Height - (STATUS_BAR_HEIGHT >> 1), betLineFont, 0, JPot.balance);
#else
	EVE_CoCmd_text(s_pHalContext, BUTTON_CURVATURE * 2, s_pHalContext->Height - (STATUS_BAR_HEIGHT >> 1), betLineFont, 0, BALANCE_TEXT);
	EVE_CoCmd_number(s_pHalContext, BUTTON_CURVATURE * 10 + betTextPixelLength + 35, s_pHalContext->Height - (STATUS_BAR_HEIGHT >> 1), betLineFont, OPT_RIGHTX, JPot.balance);
#endif

	EVE_Cmd_wr32(s_pHalContext, TAG_MASK(1));

	//payout table tracking box
	EVE_Cmd_wr32(s_pHalContext, COLOR_MASK(0, 0, 0, 0));
	EVE_Cmd_wr32(s_pHalContext, LINE_WIDTH(16));
	EVE_Cmd_wr32(s_pHalContext, BEGIN(RECTS));
	EVE_Cmd_wr32(s_pHalContext, TAG(200));
	EVE_Cmd_wr32(s_pHalContext, VERTEX2F((UI.spinColumnXOffset) * 16, 0));
	EVE_Cmd_wr32(s_pHalContext, VERTEX2F(s_pHalContext->Width * 16, (s_pHalContext->Height - STATUS_BAR_HEIGHT - BUTTON_GAP * 2) * 16));
	EVE_Cmd_wr32(s_pHalContext, COLOR_MASK(1, 1, 1, 1));

	//spin button
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 0, 0));
	EVE_CoCmd_fgColor(s_pHalContext, 0xB8860B);
	EVE_Cmd_wr32(s_pHalContext, TAG(201));
#if defined(DISPLAY_RESOLUTION_WQVGA)
	if (temp_tag == 201 && !JPot.pendown)
		EVE_CoCmd_button(s_pHalContext, s_pHalContext->Width - UI.spinButtonWidth - BUTTON_GAP, s_pHalContext->Height - STATUS_BAR_HEIGHT + BUTTON_GAP, UI.spinButtonWidth, STATUS_BAR_HEIGHT - BUTTON_GAP * 2, 30, OPT_FLAT, "Spin");
	else
		EVE_CoCmd_button(s_pHalContext, s_pHalContext->Width - UI.spinButtonWidth - BUTTON_GAP, s_pHalContext->Height - STATUS_BAR_HEIGHT + BUTTON_GAP, UI.spinButtonWidth, STATUS_BAR_HEIGHT - BUTTON_GAP * 2, 30, 0, "Spin");
#elif defined(DISPLAY_RESOLUTION_QVGA)
	if (temp_tag == 201 && !JPot.pendown)
		EVE_CoCmd_button(s_pHalContext, s_pHalContext->Width - UI.spinButtonWidth - BUTTON_GAP, s_pHalContext->Height - STATUS_BAR_HEIGHT + BUTTON_GAP, UI.spinButtonWidth, STATUS_BAR_HEIGHT - BUTTON_GAP * 2, 29, OPT_FLAT, "Spin");
	else
		EVE_CoCmd_button(s_pHalContext, s_pHalContext->Width - UI.spinButtonWidth - BUTTON_GAP, s_pHalContext->Height - STATUS_BAR_HEIGHT + BUTTON_GAP, UI.spinButtonWidth, STATUS_BAR_HEIGHT - BUTTON_GAP * 2, 29, 0, "Spin");
#elif defined(DISPLAY_RESOLUTION_WVGA)
	if (temp_tag == 201 && !JPot.pendown) {
		 EVE_CoCmd_button(s_pHalContext, s_pHalContext->Width - UI.spinButtonWidth - BUTTON_GAP, s_pHalContext->Height - STATUS_BAR_HEIGHT + BUTTON_GAP, UI.spinButtonWidth, STATUS_BAR_HEIGHT - BUTTON_GAP * 2, 30, OPT_FLAT, "Spin");
	}
	else {
		 EVE_CoCmd_button(s_pHalContext, s_pHalContext->Width - UI.spinButtonWidth - BUTTON_GAP, s_pHalContext->Height - STATUS_BAR_HEIGHT + BUTTON_GAP, UI.spinButtonWidth, STATUS_BAR_HEIGHT - BUTTON_GAP * 2, 30, 0, "Spin");
	}
#endif

	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 0, 0));

	//column selection button increase
	EVE_Cmd_wr32(s_pHalContext, TAG(202));
	if (temp_tag == 202) {
		 EVE_CoCmd_button(s_pHalContext, (s_pHalContext->Width >> 1) + BUTTON_GAP, s_pHalContext->Height - (STATUS_BAR_HEIGHT >> 1), UI.spinButtonWidth >> 1, STATUS_BAR_HEIGHT >> 1, 30, OPT_FLAT, "+");
	}
	else {
		 EVE_CoCmd_button(s_pHalContext, (s_pHalContext->Width >> 1) + BUTTON_GAP, s_pHalContext->Height - (STATUS_BAR_HEIGHT >> 1), UI.spinButtonWidth >> 1, STATUS_BAR_HEIGHT >> 1, 30, 0, "+");
	}

	//column selection button decrease
	EVE_Cmd_wr32(s_pHalContext, TAG(203));
	if (temp_tag == 203) {
		 EVE_CoCmd_button(s_pHalContext, (s_pHalContext->Width >> 1) + BUTTON_GAP + 2 + (UI.spinButtonWidth >> 1), s_pHalContext->Height - (STATUS_BAR_HEIGHT >> 1), UI.spinButtonWidth >> 1, STATUS_BAR_HEIGHT >> 1, 30, OPT_FLAT, "-");
	}
	else {
		 EVE_CoCmd_button(s_pHalContext, (s_pHalContext->Width >> 1) + BUTTON_GAP + 2 + (UI.spinButtonWidth >> 1), s_pHalContext->Height - (STATUS_BAR_HEIGHT >> 1), UI.spinButtonWidth >> 1, STATUS_BAR_HEIGHT >> 1, 30, 0, "-");
	}
	//coin selection button decrease
	EVE_Cmd_wr32(s_pHalContext, TAG(205));
	if (temp_tag == 205) {
		 EVE_CoCmd_button(s_pHalContext, (s_pHalContext->Width >> 1) - BUTTON_GAP - (UI.spinButtonWidth >> 1), s_pHalContext->Height - (STATUS_BAR_HEIGHT >> 1), UI.spinButtonWidth >> 1, STATUS_BAR_HEIGHT >> 1, 30, OPT_FLAT, "-");
	}
	else {
		 EVE_CoCmd_button(s_pHalContext, (s_pHalContext->Width >> 1) - BUTTON_GAP - (UI.spinButtonWidth >> 1), s_pHalContext->Height - (STATUS_BAR_HEIGHT >> 1), UI.spinButtonWidth >> 1, STATUS_BAR_HEIGHT >> 1, 30, 0, "-");
	}
	//coin selection button increase
	EVE_Cmd_wr32(s_pHalContext, TAG(204));
	if (temp_tag == 204) {
		 EVE_CoCmd_button(s_pHalContext, (s_pHalContext->Width >> 1) - BUTTON_GAP - (UI.spinButtonWidth >> 1) * 2 - 2, s_pHalContext->Height - (STATUS_BAR_HEIGHT >> 1), UI.spinButtonWidth >> 1, STATUS_BAR_HEIGHT >> 1, 30, OPT_FLAT, "+");
	}
	else {
		 EVE_CoCmd_button(s_pHalContext, (s_pHalContext->Width >> 1) - BUTTON_GAP - (UI.spinButtonWidth >> 1) * 2 - 2, s_pHalContext->Height - (STATUS_BAR_HEIGHT >> 1), UI.spinButtonWidth >> 1, STATUS_BAR_HEIGHT >> 1, 30, 0, "+");
	}
	//reset or new game button
	EVE_Cmd_wr32(s_pHalContext, TAG(206));
	if (JPot.reset && !JPot.spinning) {
		if (temp_tag == 206) {
			EVE_CoCmd_button(s_pHalContext, count, s_pHalContext->Height - STATUS_BAR_HEIGHT + BUTTON_GAP, resetTextPixelLength + 5, (STATUS_BAR_HEIGHT >> 1) - BUTTON_GAP * 2, 21, OPT_FLAT, "N.G.");
		}
		else {
			EVE_CoCmd_button(s_pHalContext, count, s_pHalContext->Height - STATUS_BAR_HEIGHT + BUTTON_GAP, resetTextPixelLength + 5, (STATUS_BAR_HEIGHT >> 1) - BUTTON_GAP * 2, 21, 0, "N.G.");
		}
	}
	else {
		if (temp_tag == 206) {
			EVE_CoCmd_button(s_pHalContext, count, s_pHalContext->Height - STATUS_BAR_HEIGHT + BUTTON_GAP, resetTextPixelLength + 5, (STATUS_BAR_HEIGHT >> 1) - BUTTON_GAP * 2, 22, OPT_FLAT, RESET_TEXT);
		}
		else {
			EVE_CoCmd_button(s_pHalContext, count, s_pHalContext->Height - STATUS_BAR_HEIGHT + BUTTON_GAP, resetTextPixelLength + 5, (STATUS_BAR_HEIGHT >> 1) - BUTTON_GAP * 2, 22, 0, RESET_TEXT);
		}
	}

	//payout table button
	EVE_Cmd_wr32(s_pHalContext, TAG(207));
	if (JPot.showPayoutTable) {
		if (temp_tag == 207) {
			 EVE_CoCmd_button(s_pHalContext, 0, s_pHalContext->Height - STATUS_BAR_HEIGHT - payoutTableButtonHeight, payoutTextPixelLength + 5, payoutTableButtonHeight - BUTTON_CURVATURE, 22, OPT_FLAT, "exit");
		}
		else {
			 EVE_CoCmd_button(s_pHalContext, 0, s_pHalContext->Height - STATUS_BAR_HEIGHT - payoutTableButtonHeight, payoutTextPixelLength + 5, payoutTableButtonHeight - BUTTON_CURVATURE, 22, 0, "exit");
		}
	}
	else {
		if (temp_tag == 207) {
			 EVE_CoCmd_button(s_pHalContext, 0, s_pHalContext->Height - STATUS_BAR_HEIGHT - payoutTableButtonHeight, payoutTextPixelLength + 5,/*(STATUS_BAR_HEIGHT>>1)-BUTTON_GAP*2*/payoutTableButtonHeight - BUTTON_CURVATURE, 22, OPT_FLAT, PAYOUT_TEXT);
		}
		else {
			 EVE_CoCmd_button(s_pHalContext, 0, s_pHalContext->Height - STATUS_BAR_HEIGHT - payoutTableButtonHeight, payoutTextPixelLength + 5, payoutTableButtonHeight - BUTTON_CURVATURE, 22, 0, PAYOUT_TEXT);
		}
	}

	EVE_Cmd_wr32(s_pHalContext, TAG_MASK(0));
}

uint16_t fillLineIndexandPoints() {
	uint8_t i, j, k, l, m, n, o, fruitCount = 0, fruitTableSize = 7, lookingFor = SPIN_COLUMNS, tempPoints, lastLookingFor;
	uint8_t fruitTable[] = { 3,5,6,9,10,11,12 }, wonIndex[2];
	bool_t finished = FALSE, fruitFound = FALSE, setVal;
	uint8_t tempIndex[NUM_OF_ICONS];
	uint16_t totalPoints = 0;

	for (i = 0; i<TOTAL_BET_LINES; i++) {
		if (i >= JPot.selectedBetLine) {
			for (n = 0; n<SPIN_COLUMNS; n++) {
				JPot.lineBet[i][n] = 254;
			}
			continue;
		}

		finished = FALSE;
		clearArray(tempIndex, NUM_OF_ICONS, 0);
		lookingFor = SPIN_COLUMNS;
		fruitCount = 0;
		tempPoints = 0;
		lastLookingFor = 254;
		wonIndex[0] = 254;
		wonIndex[1] = 254;
		setVal = FALSE;
		for (j = 0; j<SPIN_COLUMNS; j++) {
			if (linePosition[i].line[j] == 't') {
				JPot.lineBet[i][j] = (columns[j].drawnIndex - 1) < 0 ? columns[j].iconArray[NUM_OF_ICONS - 1] : columns[j].iconArray[columns[j].drawnIndex - 1];
			}
			else if (linePosition[i].line[j] == 'm') {
				JPot.lineBet[i][j] = columns[j].iconArray[columns[j].drawnIndex];
			}
			else {
				JPot.lineBet[i][j] = (columns[j].drawnIndex + 1) >= NUM_OF_ICONS ? columns[j].iconArray[0] : columns[j].iconArray[columns[j].drawnIndex + 1];
			}
			tempIndex[JPot.lineBet[i][j]]++;
		}

		for (k = 0; k<NUM_OF_ICONS;) {
			//check for combination with the highest payout value first.
			if (tempIndex[k] >= lookingFor) {
				if (setVal == FALSE) {
					for (o = 0; o<NUM_OF_ICONS; o++) {
						if (tempIndex[o] >= lookingFor) {
							if (wonIndex[0] == 254) {  //for 5 columns
								wonIndex[0] = o;
							}
							else {
								wonIndex[1] = o;
							}
						}
					}
					setVal = TRUE;
				}
				for (l = 0; l<SPIN_COLUMNS; l++) {
					if (JPot.lineBet[i][l] != wonIndex[0] && JPot.lineBet[i][l] != wonIndex[1]) {
						JPot.lineBet[i][l] = 254;
					}
				}
				if (lookingFor>2) {
					totalPoints = payoutTable[k] * lookingFor;
					finished = TRUE;
				}
				else if (lookingFor == 2) {
					tempPoints += payoutTable[k] * 2;
				}
			}
			if (finished) {
				break;
			}
			k++;
			if (k == NUM_OF_ICONS) {
				lookingFor--;
				k = 0;
				if (lookingFor<2) {
					break;
				}
			}
		}

		if (!finished && tempPoints>0) {
			totalPoints += tempPoints;
			finished = TRUE;
		}

		//check for any fruits
		if (!finished) {
			for (m = 0; m<SPIN_COLUMNS; m++) {
				fruitFound = FALSE;
				for (n = 0; n<fruitTableSize; n++) {
					if (JPot.lineBet[i][m] == fruitTable[n]) {
						fruitFound = TRUE;
						fruitCount++;
					}
				}
				if (!fruitFound) {
					JPot.lineBet[i][m] = 254;
				}
			}
			totalPoints += fruitCount*payoutTable[14];
		}
	}
	return totalPoints;
}

void DemoJackpot(EVE_HalContext* pHalContext) {
	s_pHalContext = pHalContext;
	jackpotSetup();
	startingAnimation();

	uint16_t earnedPoints = 0;
	uint8_t i, spinCoinAmount = 8;
#if defined(MSVC_PLATFORM) || defined(BT8XXEMU_PLATFORM)
	lastMilli = time(NULL);
#endif
	JPot.totalBetAmount = POINTS_PER_COLUMN * JPot.spinColumnSelected + JPot.selectedBetLine * POINTS_PER_COLUMN;
	while (1) {
		EVE_Cmd_wr32(s_pHalContext, CMD_DLSTART);
		EVE_Cmd_wr32(s_pHalContext, CLEAR_COLOR_RGB(0, 0, 0));
		EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1));
		EVE_Hal_wr8(s_pHalContext, REG_VOL_SOUND, 50);
		temp_tag = Gesture_GetTag(s_pHalContext);
		touched(temp_tag);

		EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 255, 255));
		EVE_Cmd_wr32(s_pHalContext, BEGIN(BITMAPS));

		//coin repeat background

		EVE_Cmd_wr32(s_pHalContext, BITMAP_HANDLE((15)));
		EVE_Cmd_wr32(s_pHalContext, BITMAP_SOURCE(bitmapInfo[1].Offset));
		EVE_Cmd_wr32(s_pHalContext, BITMAP_LAYOUT(RGB565, bitmapInfo[1].Width, bitmapInfo[1].Height));
		EVE_Cmd_wr32(s_pHalContext, BITMAP_SIZE(NEAREST, REPEAT, REPEAT, s_pHalContext->Width, s_pHalContext->Height - STATUS_BAR_HEIGHT));
#if defined(DISPLAY_RESOLUTION_WVGA)
		EVE_Cmd_wr32(s_pHalContext, BITMAP_SIZE_H(s_pHalContext->Width >> 9, (s_pHalContext->Height - STATUS_BAR_HEIGHT) >> 9));
#endif
		EVE_Cmd_wr32(s_pHalContext, VERTEX2F(0, 0));

		if (JPot.showPayoutTable) {
			drawPayoutTable();
		}
		else {
			drawSpinColumns();
			if (!JPot.spinning)
				drawbetlines();

			if (JPot.rewardedPoints) {
				redrawColumnIcons();
			}
		}

		//reward points when all columns have stopped
		if (JPot.stoppedColumns == SPIN_COLUMNS) {
			JPot.spinning = FALSE;
			if (!JPot.rewardedPoints) {
				earnedPoints = fillLineIndexandPoints();
				JPot.displayRewardLine = TRUE;
#if defined(FT900_PLATFORM) || defined(FT93X_PLATFORM)
				curMilli = EVE_millis();
#endif
				currentLine = 0;
				earnedPoints += getPoints();
				JPot.betChanged = FALSE;
				if (earnedPoints>0) {
					Play_Sound(s_pHalContext, SPINNING_SOUND, 255, 0x00);  //winning sound
					JPot.balance += earnedPoints;
					spinCoinAmount = (earnedPoints >> 4);
					if (spinCoinAmount>8) {
						spinCoinAmount = 8;
					}
					for (i = 0; i<spinCoinAmount; i++) {
						spinCoins[i].fall = FALSE;
						spinCoins[i].xPos = s_pHalContext->Width >> 1;
						spinCoins[i].yPos = s_pHalContext->Height / 3;
						spinCoins[i].yVel = -2 + (nextRandomInt8(0) & 3)*-1;
						nextRandomInt8(0);
						if (nextRandomInt8(0) & 1) {
							spinCoins[i].xVel = -4 + (nextRandomInt8(0) & 3)*-1;
						}
						else {
							spinCoins[i].xVel = 4 + (nextRandomInt8(0) & 3);
						}
						spinCoins[i].index = (nextRandomInt8(0) & 7);
					}

					for (i = 0; i<spinCoinAmount; i++) {
						Play_Sound(s_pHalContext, COIN_REWARD_SOUND, 255, 0x00);
					}

				}

				//check the balance to see if the game can be continued. 
				if (!ableToContinue() && JPot.rewardedPoints) {
					JPot.reset = TRUE;
				}
				else {
					JPot.reset = FALSE;
				}
			}
		}

		statusBar();
		if (!JPot.showPayoutTable) {
			lineBetButtons();
		}

		coinsAnimation(spinCoinAmount);

		displayRewardText(earnedPoints);

		//displaySelectedIcon(JPot.selectedBetLine);
		EVE_Cmd_wr32(s_pHalContext, DISPLAY());
		EVE_CoCmd_swap(s_pHalContext);
		EVE_Cmd_waitFlush(s_pHalContext);

		EVE_sleep(50);

	}
}
