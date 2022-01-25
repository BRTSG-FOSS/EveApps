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

#define FONT_IN_SDCARD 0
#define PX(x) ((x) * 16)

#if FONT_IN_SDCARD

#if MSVC_PLATFORM
#define FONTPATH TEST_DIR "\\font\\"
#else
#define FONTPATH ""
#endif

const char* file_font_chinese_glyph = FONTPATH "01.gly";
const char* file_font_chinese_xfont = FONTPATH "01.xfo";
const char* file_font_english_glyph = FONTPATH "02.gly";
const char* file_font_english_xfont = FONTPATH "02.xfo";
const char* file_font_japanese_glyph = FONTPATH "03.gly";
const char* file_font_japanese_xfont = FONTPATH "03.xfo";
const char* file_font_japanese_glyph = FONTPATH "04.gly";
const char* file_font_japanese_xfont = FONTPATH "04.xfo";
#endif

const char8_t *txtStrCn[] = {
	u8"\x001b\x002e",
	u8"\x002d\x002c\x0039\x0032\x003b\x0035\x0037\x003c\x003d\x001c\x0029\x0036",
	u8"\x001a",
	u8"\x003f",
	u8"\x0009\x000b\x000c\x000d\x000e\x000f\x0010\x0011\x0012\x0013",
	u8"\x002b\x0038\x0033\x0020\x0026\x001f\x0025\x002a\x001d\x0028",
	u8"\x002f\x0024\x0031\x0034\x0021\x003a\x001e\x0022\x001b",
	u8"\x0041\x0040\x003e\x0023\x0030\x0027\x0019",
	u8"\x0001\x0018\x0007\x0005\x0015\x0017\x0016\x0014\x0004\x0003\x0006\x0008",

	NULL
};
const char8_t *txtStrEn[] = {
	u8"\x0015\x0029\x0022\x0027\x0024\x002e\x0023",
	u8"\x0017\x0023\x0020\x0002\x002c\x0030\x0024\x001e\x0026\x0002\x001d\x002d\x002a\x0032\x0029\x0002\x0021\x002a\x0033\x0002\x0025\x0030\x0028\x002b\x002e\x0002\x002a\x0031\x0020\x002d\x0002\x002f\x0023\x0020\x0002\x0027\x001c\x0035\x0034\x0002\x001f\x002a\x0022",
	u8"\x0016\x0029",
	u8"\x0016\x0021\x0021",
	u8"\x0009\x000b\x000c\x000d\x000e\x000f\x0010\x0011\x0012\x0013",
	u8"\x002c\x0032\x0020\x002d\x002f\x0034\x0030\x0024\x002a\x002b",
	u8"\x001c\x002e\x001f\x0021\x0022\x0023\x0025\x0026\x0027",
	u8"\x0035\x0033\x001e\x0031\x001d\x0029\x0028",
	u8"\x0001\x001b\x0007\x0005\x0018\x001a\x0019\x0014\x0004\x0003\x0006\x0008",

	NULL
};
const char8_t *txtStrJa[] = {
	u8"\x0045\x0046\x0042",
	u8"\x0039\x0035\x003a\x0039\x003e\x003f\x0036\x0040\x0038\x003b\x003c\x002b\x0043\x0044\x0028\x0047\x002a\x0041\x0033\x0048\x002d",
	u8"\x0036\x003f",
	u8"\x0036\x003c",
	u8"\x0009\x000b\x000c\x000d\x000e\x000f\x0010\x0011\x0012\x0013",
	u8"\x0023\x0026\x0019\x0020\x001a\x0034\x0028\x0029\x0031\x0021",
	u8"\x0024\x0027\x001f\x002b\x001b\x001c\x002e\x002a\x0032",
	u8"\x0025\x001e\x0022\x002c\x001d\x002f\x0030",
	u8"\x0001\x0018\x0007\x0005\x0015\x0017\x0016\x0014\x0004\x0003\x0006\x0008\x0002\x0002",
	NULL
};
const char8_t *txtStrHi[] = {
	u8"\x001a\x0019\x001f\x0038\x002a\x0035\x0021\x002f\x0032",
	u8"\x0023\x0038\x002c\x002a\x0031\x0023\x0002\x0028\x0034\x002a\x0035\x0002\x002b\x0037\x0029\x0022\x002f\x0032\x0002\x001b\x002b\x002d\x0032\x0002\x001e\x0033\x0023\x0038\x0023\x0035\x0002\x0026\x002a\x0002\x001e\x0034\x0024\x0023\x0032\x0002\x002e\x0036",
	u8"\x0026\x002a",
	u8"\x0027\x0019\x0024",
	u8"\x0009\x000b\x000c\x000d\x000e\x000f\x0010\x0011\x0012\x0013",
	u8"\x0033\x0034\x0029\x0023\x0021\x002b\x0025\x0026\x002c\x0020",
	u8"\x0019\x0035\x001e\x0031\x002e\x0032\x002a\x0030\x002d",
	u8"\x0038\x002a\x001f\x0027\x001a\x001c\x0024\x001d",
	u8"\x0001\x0018\x0007\x0005\x0015\x0017\x0016\x0014\x0004\x0003\x0006\x0008",
	NULL
};

typedef struct  _APPBUTTON {
	uint32_t x;
	uint32_t y;
	uint32_t w;
	uint32_t h;
	char8_t txt[200];
	uint32_t font;
	uint32_t opt;
	uint8_t tag;
	uint32_t isPressing;
}AppButton;

static int sizeofTextAlphabe = 0;
static AppButton buttonsEn;
static AppButton buttonsCn;
static AppButton buttonsJa;
static AppButton buttonsHi;

#if FONT_IN_SDCARD
static const uint32_t addrXfont_Cn = RAM_G + 1024 * 0; //
static const uint32_t addrGlyph_Cn = RAM_G + 1024 * 2; // 2048
static const uint32_t addrXfont_En = RAM_G + 1024 * 22; //
static const uint32_t addrGlyph_En = RAM_G + 1024 * 24; // 24576
static const uint32_t addrXfont_Ja = RAM_G + 1024 * 44; //
static const uint32_t addrGlyph_Ja = RAM_G + 1024 * 46; // 47104
#else
static const uint32_t addrGlyph_Cn = 4096;
static const uint32_t addrXfont_Cn = 20992;
static const uint32_t addrGlyph_En = 21120;
static const uint32_t addrXfont_En = 41856;
static const uint32_t addrGlyph_Ja = 41984;
static const uint32_t addrXfont_Ja = 74752;
static const uint32_t addrGlyph_Hi = 74944;
static const uint32_t addrXfont_Hi = 85888;
static const uint32_t size_last = 128;
#endif

static const uint32_t fontCn = 0;
static const uint32_t fontEn = 1;
static const uint32_t fontJa = 2;
static const uint32_t fontHi = 3;

static const int TAG_TOGGLE = 99;
static const int TAG_BTN_Cn = 98;
static const int TAG_BTN_En = 97;
static const int TAG_BTN_Ja = 96;
static const int TAG_BTN_Hi = 95;

static const uint8_t TAG_KEY_BACKSPACE = 94;
static const int TAG_KEY_TAB = 0;
static const int TAG_KEY_BACKTICK = 1;
static const int TAG_KEY_MINUS = 2;
static const int TAG_KEY_PLUS = 3;
static const int TAG_KEY_LBRACKET = 4;
static const int TAG_KEY_RBRACKET = 5;
static const int TAG_KEY_BACKSLASH = 6;
static const int TAG_KEY_SEMICOLON = 7;
static const int TAG_KEY_QOUTE = 8;
static const int TAG_KEY_DQOUTE = 9;
static const int TAG_KEY_COMMA = 10;
static const int TAG_KEY_DOT = 11;

static uint16_t toggleState = 0; // state of the toggle: 0 is off, 65535 is on.
static char *txtToggle;

const char **crrTxtStr = txtStrEn;
static uint32_t crrFont = 0;
char txtTyping[100] = { 0 };

static uint8_t lastChar = 0;

typedef enum _eTXTID {
	eTxtIdFIRST,
	eTxtIdLanguage = 0,
	eTxtIdtxt,
	eTxtIdOn,
	eTxtIdOff,
	eTxtIdNumber,
	eTxtIdAlphabeQ,
	eTxtIdAlphabeA,
	eTxtIdAlphabeZ,
	eTxtIdSpecial,
	eTxtIdLAST
}eTxtId;

int loadXfont() {
#if defined(EVE_FLASH_AVAILABLE)
	FlashHelper_SwitchFullMode(s_pHalContext);
#endif

#if FONT_IN_SDCARD
	Display_Start(s_pHalContext);

	Gpu_Hal_LoadImageToMemory(s_pHalContext, file_font_chinese_xfont, addrXfont_Cn, LOAD);
	EVE_Cmd_waitFlush(s_pHalContext);
	Gpu_Hal_LoadImageToMemory(s_pHalContext, file_font_chinese_glyph, addrGlyph_Cn, LOAD);
	EVE_Cmd_waitFlush(s_pHalContext);
	Gpu_Hal_LoadImageToMemory(s_pHalContext, file_font_english_xfont, addrXfont_En, LOAD);
	EVE_Cmd_waitFlush(s_pHalContext);
	Gpu_Hal_LoadImageToMemory(s_pHalContext, file_font_english_glyph, addrGlyph_En, LOAD);
	EVE_Cmd_waitFlush(s_pHalContext);
	Gpu_Hal_LoadImageToMemory(s_pHalContext, file_font_japanese_xfont, addrXfont_Ja, LOAD);
	EVE_Cmd_waitFlush(s_pHalContext);
	Gpu_Hal_LoadImageToMemory(s_pHalContext, file_font_japanese_glyph, addrGlyph_Ja, LOAD);
	EVE_Cmd_waitFlush(s_pHalContext);

	Display_End(s_pHalContext);
#else

	EVE_CoCmd_flashRead(s_pHalContext, 0, 0, (addrXfont_Hi + size_last));
#endif
	return 0;
}

static int processEvents() {
	int x = 600;
	int y = 160;
	int w = 40;
	int typingLength = strlen(txtTyping);
	static int counter = 0;
	const int timeval = 10;

	if (Gesture_Get()->tagReleased == buttonsCn.tag) {
		crrTxtStr = txtStrCn;
		crrFont = fontCn;
		memset(txtTyping, 0, sizeof(txtTyping));
	}
	else if (Gesture_Get()->tagReleased == buttonsEn.tag) {
		crrTxtStr = txtStrEn;
		crrFont = fontEn;
		memset(txtTyping, 0, sizeof(txtTyping));
	}
	else if (Gesture_Get()->tagReleased == buttonsJa.tag) {
		crrTxtStr = txtStrJa;
		crrFont = fontJa;
		memset(txtTyping, 0, sizeof(txtTyping));
	}
	else if (Gesture_Get()->tagReleased == buttonsHi.tag) {
		crrTxtStr = txtStrHi;
		crrFont = fontHi;
		memset(txtTyping, 0, sizeof(txtTyping));
	}
	else if (Gesture_Get()->tagReleased == TAG_TOGGLE) {
		if (toggleState) {
			toggleState = 0;
		}
		else {
			toggleState = 65535;
		}
	}
	else if (Gesture_Get()->tagReleased == TAG_KEY_BACKSPACE) {
		if (typingLength > 0) {
			txtTyping[typingLength - 1] = 0;
		}
	}
	else if (Gesture_Get()->tagReleased > 0) {
		if (typingLength > w) {
			memcpy(txtTyping, &txtTyping[1], typingLength);
			typingLength = w;
			txtTyping[typingLength] = 0;
		}

		txtTyping[typingLength] = Gesture_Get()->tagReleased;
	}
	else {
	}

	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(200, 200, 200));
	EVE_Cmd_wr32(s_pHalContext, LINE_WIDTH(PX(20)));
	EVE_Cmd_wr32(s_pHalContext, BEGIN(RECTS));
	EVE_Cmd_wr32(s_pHalContext, VERTEX2F(PX(70),     PX(y - 10)));
	EVE_Cmd_wr32(s_pHalContext, VERTEX2F(PX(10 + x), PX(y + 50)));
	EVE_Cmd_wr32(s_pHalContext, END());

	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0, 0, 0));
	
	if (txtTyping[0] != 0) {
		EVE_CoCmd_text(s_pHalContext, x, y, crrFont, OPT_RIGHTX, txtTyping);
	}

	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 255, 255));
	EVE_Cmd_wr32(s_pHalContext, LINE_WIDTH(PX(20)));
	EVE_Cmd_wr32(s_pHalContext, BEGIN(RECTS));
	EVE_Cmd_wr32(s_pHalContext, VERTEX2F(PX(0),   PX(y - 10)));
	EVE_Cmd_wr32(s_pHalContext, VERTEX2F(PX(70 - 20*2),  PX(y + 50)));
	EVE_Cmd_wr32(s_pHalContext, END());

	if (counter > timeval) {
		EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0, 0, 255));
		EVE_CoCmd_text(s_pHalContext, x, y, 29, 0, "|");
	}

	counter++;
	counter = counter % (timeval * 2);

	txtToggle = crrTxtStr[eTxtIdOff];
	if (toggleState) {
		txtToggle = crrTxtStr[eTxtIdOn];
	}
	return 0;
}

static void drawOneButton(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t font, uint32_t opt, uint8_t tag, char8_t *txt) {
	EVE_CoCmd_track(s_pHalContext, x, y, w, h, tag);
	EVE_Cmd_wr32(s_pHalContext, TAG(tag));
	EVE_CoCmd_button(s_pHalContext, x, y, w, h, font, opt, txt);
	EVE_Cmd_wr32(s_pHalContext, TAG(0));
}

static void drawNextKey(int8_t keyType, int16_t *x, int16_t y, int16_t w, int16_t h, int16_t font, uint16_t options, const char8_t* s, uint32_t tag) {
	uint8_t str[2];
	uint8_t *tobeDraw = s;

	str[0] = s[0];
	str[1] = 0;
	if (keyType == 1) {
		tobeDraw = str;
	}

	if (keyType != 2) {// this is a key
		EVE_CoCmd_keys(s_pHalContext, *x, y, w, h, font, lastChar, tobeDraw);
	}
	else {// this is button
		EVE_Cmd_wr32(s_pHalContext, TAG(tag));
		EVE_CoCmd_button(s_pHalContext, *x, y, w, h, font, options, tobeDraw);
		EVE_Cmd_wr32(s_pHalContext, TAG(0));
	}
	*x += (w + 3); // 3 is the gap
}

static void drawKeyBoard() {
	const int16_t xKey = 60;
	const int16_t yKey = 240;
	const int16_t wKey = 45;
	const int16_t hKey = 45;
	const int16_t yGap = 50;
	const int16_t xGap = 20;
	int x = xKey, y = yKey;

	const int KEY_MULTIPLE = 0;
	const int KEY_SINGLE = 1;
	const int BUTTON = 2;

	// first row
	drawNextKey(KEY_SINGLE, &x, y, wKey, hKey, crrFont, 0, &crrTxtStr[eTxtIdSpecial][TAG_KEY_BACKTICK], 0);
	drawNextKey(KEY_MULTIPLE, &x, y, wKey * 10, hKey, crrFont, 0, crrTxtStr[eTxtIdNumber], 0);
	drawNextKey(KEY_SINGLE, &x, y, wKey, hKey, crrFont, 0, &crrTxtStr[eTxtIdSpecial][TAG_KEY_MINUS], 0);
	drawNextKey(KEY_SINGLE, &x, y, wKey, hKey, crrFont, 0, &crrTxtStr[eTxtIdSpecial][TAG_KEY_PLUS], 0);
	drawNextKey(BUTTON, &x, y, wKey * 2, hKey, 26, 0, "Backspace", TAG_KEY_BACKSPACE);

	// second row
	x = xKey; y = yKey + yGap;
	drawNextKey(BUTTON, &x, y, wKey * 3 / 2, hKey, 26, 0, "Tab", 0);
	drawNextKey(KEY_MULTIPLE, &x, y, wKey * 10, hKey, crrFont, 0, crrTxtStr[eTxtIdAlphabeQ], 0);
	drawNextKey(KEY_SINGLE, &x, y, wKey, hKey, crrFont, 0, &crrTxtStr[eTxtIdSpecial][TAG_KEY_LBRACKET], 0);
	drawNextKey(KEY_SINGLE, &x, y, wKey, hKey, crrFont, 0, &crrTxtStr[eTxtIdSpecial][TAG_KEY_RBRACKET], 0);
	drawNextKey(KEY_SINGLE, &x, y, wKey * 3 / 2, hKey, crrFont, 0, &crrTxtStr[eTxtIdSpecial][TAG_KEY_BACKSLASH], 0);

	// third row
	x = xKey; y = yKey + 2 * yGap;
	drawNextKey(BUTTON, &x, y, wKey * 2, hKey, 26, 0, "Caps Lock", 0);
	drawNextKey(KEY_MULTIPLE, &x, y, wKey * 9, hKey, crrFont, 0, crrTxtStr[eTxtIdAlphabeA], 0);
	drawNextKey(KEY_SINGLE, &x, y, wKey, hKey, crrFont, 0, &crrTxtStr[eTxtIdSpecial][TAG_KEY_SEMICOLON], 0);
	drawNextKey(KEY_SINGLE, &x, y, wKey, hKey, crrFont, 0, &crrTxtStr[eTxtIdSpecial][TAG_KEY_DQOUTE], 0);
	drawNextKey(BUTTON, &x, y, wKey * 2, hKey, 26, 0, "Enter", 0);

	// fourth row
	x = xKey; y = yKey + 3 * yGap;
	drawNextKey(BUTTON, &x, y, wKey * 3, hKey, 26, 0, "Shift", 0);
	drawNextKey(KEY_MULTIPLE, &x, y, wKey * 7, hKey, crrFont, 0, crrTxtStr[eTxtIdAlphabeZ], 0);
	drawNextKey(KEY_SINGLE, &x, y, wKey, hKey, crrFont, 0, &crrTxtStr[eTxtIdSpecial][TAG_KEY_COMMA], 0);
	drawNextKey(KEY_SINGLE, &x, y, wKey, hKey, crrFont, 0, &crrTxtStr[eTxtIdSpecial][TAG_KEY_DOT], 0);
	drawNextKey(BUTTON, &x, y, wKey * 3, hKey, 26, 0, "Shift", 0);

	// fifth row - display list may overflow
	/*x = xKey; y = yKey + 4 * yGap;
	drawNextKey(BTN, &x, y, wKey, hKey, 26, 0, "Ctrl");
	drawNextKey(BTN, &x, y, wKey, hKey, 26, 0, " ");
	drawNextKey(BTN, &x, y, wKey, hKey, 26, 0, "Alt");
	drawNextKey(BTN, &x, y, wKey, hKey, 26, 0, "Space");
	drawNextKey(BTN, &x, y, wKey, hKey, 26, 0, "Alt");
	drawNextKey(BTN, &x, y, wKey, hKey, 26, 0, "Fn");
	drawNextKey(BTN, &x, y, wKey, hKey, 26, 0, " ");
	drawNextKey(BTN, &x, y, wKey, hKey, 26, 0, "Ctrl");*/
}

static void drawLanguages() {
	AppButton *b;

	b = &buttonsEn;
	drawOneButton(b->x, b->y, b->w, b->h, b->font, b->opt, b->tag, b->txt);
	b = &buttonsCn;
	drawOneButton(b->x, b->y, b->w, b->h, b->font, b->opt, b->tag, b->txt);
	b = &buttonsJa;
	drawOneButton(b->x, b->y, b->w, b->h, b->font, b->opt, b->tag, b->txt);
	b = &buttonsHi;
	drawOneButton(b->x, b->y, b->w, b->h, b->font, b->opt, b->tag, b->txt);
}

void initButtons() {
	const int wLanguage = 150;
	const int hLanguage = 40;
	const int xLanguage = s_pHalContext->Width - wLanguage - 10;

	memcpy(buttonsCn.txt, txtStrCn[eTxtIdLanguage], strlen(txtStrCn[eTxtIdLanguage]));
	memcpy(buttonsEn.txt, txtStrEn[eTxtIdLanguage], strlen(txtStrEn[eTxtIdLanguage]));
	memcpy(buttonsJa.txt, txtStrJa[eTxtIdLanguage], strlen(txtStrJa[eTxtIdLanguage]));
	memcpy(buttonsHi.txt, txtStrHi[eTxtIdLanguage], strlen(txtStrHi[eTxtIdLanguage]));

	buttonsCn.w = buttonsEn.w = buttonsJa.w = buttonsHi.w = wLanguage;
	buttonsCn.h = buttonsEn.h = buttonsJa.h = buttonsHi.h = hLanguage;
	buttonsCn.x = buttonsEn.x = buttonsJa.x = buttonsHi.x = xLanguage;

	buttonsCn.font = fontCn;
	buttonsEn.font = fontEn;
	buttonsJa.font = fontJa;
	buttonsHi.font = fontHi;

	buttonsCn.y = 10 + (hLanguage + 10) * 0;
	buttonsEn.y = 10 + (hLanguage + 10) * 1;
	buttonsJa.y = 10 + (hLanguage + 10) * 2;
	buttonsHi.y = 10 + (hLanguage + 10) * 3;

	buttonsCn.tag = TAG_BTN_Cn;
	buttonsEn.tag = TAG_BTN_En;
	buttonsJa.tag = TAG_BTN_Ja;
	buttonsHi.tag = TAG_BTN_Hi;
}

void DemoUnicode(EVE_HalContext* pHalContext) {
#if EVE_CHIPID < EVE_BT815
#error Platform not supported
#endif

	s_pHalContext = pHalContext;

	Display_Start(s_pHalContext);

	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0x80, 0x80, 0x00));

	EVE_CoCmd_text(s_pHalContext, (s_pHalContext->Width / 2), 100, 31, OPT_CENTERX, "Demonstrate the Unicode support for:");
	EVE_CoCmd_text(s_pHalContext, (s_pHalContext->Width / 2), 150, 30, OPT_CENTERX, "English");
	EVE_CoCmd_text(s_pHalContext, (s_pHalContext->Width / 2), 200, 30, OPT_CENTERX, "Chinese");
	EVE_CoCmd_text(s_pHalContext, (s_pHalContext->Width / 2), 250, 30, OPT_CENTERX, "Japanese");
	EVE_CoCmd_text(s_pHalContext, (s_pHalContext->Width / 2), 300, 30, OPT_CENTERX, "Hindi");

	Display_End(s_pHalContext);

	EVE_sleep(2000);

	loadXfont();

	txtToggle = crrTxtStr[eTxtIdOff];
	crrFont = fontEn;
	crrTxtStr = txtStrEn;

	initButtons();
	while (1) {
		Display_Start(s_pHalContext);

		EVE_CoCmd_setFont2(s_pHalContext, fontCn, addrXfont_Cn, 0);
		EVE_CoCmd_setFont2(s_pHalContext, fontEn, addrXfont_En, 0);
		EVE_CoCmd_setFont2(s_pHalContext, fontJa, addrXfont_Ja, 0);
		EVE_CoCmd_setFont2(s_pHalContext, fontHi, addrXfont_Hi, 0);

		EVE_CoCmd_fgColor(s_pHalContext, 0x101010);
		if (toggleState) {
			EVE_CoCmd_bgColor(s_pHalContext, 255);
			EVE_CoCmd_gradColor(s_pHalContext, 0x00ff00);
		}
		else {
			EVE_CoCmd_bgColor(s_pHalContext, 0x402000);
			EVE_CoCmd_gradColor(s_pHalContext, 0x0000ff);
		}
		drawLanguages();
		drawKeyBoard();

		EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0, 0, 0));
		EVE_CoCmd_text(s_pHalContext, 40, 90, crrFont, 0, crrTxtStr[eTxtIdtxt]);

		EVE_Cmd_wr32(s_pHalContext, TAG(TAG_TOGGLE));
		EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 255, 255));
		EVE_CoCmd_toggle(s_pHalContext, 60, 20, 100, crrFont, 0, toggleState, txtToggle);
		EVE_Cmd_wr32(s_pHalContext, TAG(0));

		Gesture_Renew(s_pHalContext);
		processEvents();
		Display_End(s_pHalContext);

		EVE_sleep(10);
	}// loop forever
}
