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

#include "Common.h"
#include "App.h"

static EVE_HalContext *s_pHalContext;

typedef enum _KEYBOARD_TYPE_E {
	KEYBOARD_0_9, KEYBOARD_HEXA, KEYBOARD_09_AZ, KEYBOARD_09_az_AZ
}KEYBOARD_TYPE;

uint8_t* keyChars[] = {
	/*KEYBOARD_0_9*/ "0123456789 ",
	/*KEYBOARD_HEXA*/ "0123456789ABCDEF " "\xff\xfe",
	/*KEYBOARD_09_AZ*/ "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ ",
	/*KEYBOARD_09_az_AZ*/ "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ ",
};

#define KEYTAG_MIN    1
#define KEYTAG_MAX    253
#define KEYCODE_SPACE ' '
#define KEYCODE_CLOSE 254
#define KEYCODE_BACK  255
typedef struct Keyboard_ {
	KEYBOARD_TYPE type;
	uint16_t x, y, w, h, font, opt;
}Keyboard;

#define INPUT_MAXLEN 100
#define INPUT_PREFIX  50
typedef struct Input_ {
	uint32_t tag;
	KEYBOARD_TYPE keyboard_type;
	uint8_t isKeyboardActive;
	uint32_t crrLen;

	uint8_t currText[INPUT_MAXLEN];// text string
	uint8_t prefix[INPUT_PREFIX];// text string
	uint16_t x, y, w, h, font, opt;
}Input;

#define KEY_W (s_pHalContext->Width < 800?30:50)
#define INPUT_W (s_pHalContext->Width < 800?250:500)
#define TEXTARE_W (INPUT_W + 10 + SEND_BTN_W)
#define TEXTARE_MAX_S (s_pHalContext->Width < 800?200:500)
#define SEND_BTN_W (s_pHalContext->Width < 800 ? 60 : 100)

typedef struct params_ {
	int len8bit;
	int len32bit;
	union {
		uint8_t  arr8bit[128 * 4]; // maximum 128 param
		uint32_t arr32bit[128];
	};
}PARAM;

typedef struct SCENARIO_ {
	uint8_t title[100];
	uint8_t w[50];
	uint8_t r[50];
	uint32_t duration;
}SCENARIO;

Input ip1;
Input ip2;

#define RAM_CMD_SIZE (4 * 1024)
#define BUFFSIZE     (RAM_CMD_SIZE / 5)
#define BUFFDISPSIZE (RAM_CMD_SIZE)

uint8_t buff[BUFFSIZE];
uint8_t buff2disp[RAM_CMD_SIZE];
static uint8_t isPreviousError = 0;
static uint8_t isKbActive = 0;

typedef enum _TAG_E {
	TAG_IP1 = 1,
	TAG_IP2,
	TAG_BTN1,
	TAG_BTN2,
	TAG_BTN_GET_JEDEC,
	TAG_BTN_GET_SFDP,
	TAG_BTN_GET_SFDP_RAW,
	TAG_BTN_GET_DEVICEID,
	TAG_BTN_GET_UNIQID,
}_TAG;

SCENARIO sc[] = {
	{"Read JEDEC ID" , "1 9F","0 3", 2000},
	{"Read SFDP"     , "4 5A000000","0 100", 2000},
	{"Read Device ID", "1 90","0 5", 2000},
	{"Read Unique ID", "1 4B","0 0C", 2000},
};

/// Keyboard
uint8_t keyboard_event(Keyboard* kb) {
	uint8_t key = Gesture_Get()->tagReleased;
	if (key == 0) {
		return key;
	}
	for (int i = 0; i < strlen(keyChars[kb->type]); i++) {
		if (key == keyChars[kb->type][i]) {
			return key;
		}
	}

	/// Key is invalid
	return 0;
}

void keyboard_draw_type_09(Keyboard kb) {
	EVE_CoCmd_fgColor(s_pHalContext, 0x404080);
	EVE_CoCmd_gradColor(s_pHalContext, 0x00ff00);

	const int num_col = 3;

	uint32_t x = kb.x;
	uint32_t y = kb.y;
	EVE_CoCmd_keys(s_pHalContext, x, y, kb.w * num_col, kb.h, kb.font, kb.opt, "789");

	y += kb.h;
	EVE_CoCmd_keys(s_pHalContext, x, y, kb.w * num_col, kb.h, kb.font, kb.opt, "456");

	y += kb.h;
	EVE_CoCmd_keys(s_pHalContext, x, y, kb.w * num_col, kb.h, kb.font, kb.opt, "123");

	y += kb.h;
	EVE_CoCmd_keys(s_pHalContext, x, y, kb.w * num_col, kb.h, kb.font, kb.opt, "0");
	EVE_CoCmd_gradColor(s_pHalContext, 0x00ffff);

	x = kb.x + 10 + kb.w * num_col;
	y = kb.y;
	EVE_Cmd_wr32(s_pHalContext, TAG(KEYCODE_BACK));
	EVE_CoCmd_button(s_pHalContext, x, y, kb.w, kb.h, kb.font, 0, "  <- ");

	y += kb.h;
	EVE_Cmd_wr32(s_pHalContext, TAG(KEYCODE_SPACE));
	EVE_CoCmd_button(s_pHalContext, x, y, kb.w, kb.h, kb.font, 0, "Space");

	y += kb.h;
	EVE_Cmd_wr32(s_pHalContext, TAG(KEYCODE_CLOSE));
	EVE_CoCmd_button(s_pHalContext, x, y, kb.w, kb.h, kb.font, 0, "Close");
}

void keyboard_draw_hexa(Keyboard kb) {
	/* Construct a simple keyboard - note that the tags associated with the keys are the character values given in the arguments */
	EVE_CoCmd_fgColor(s_pHalContext, 0x404080);
	EVE_CoCmd_gradColor(s_pHalContext, 0x00ff00);

	const int num_col = 5;
	const int margin_top = 2;
	const int margin_left = 2;

	uint32_t x = kb.x;
	uint32_t y = kb.y;
	EVE_CoCmd_keys(s_pHalContext, x, y, kb.w * num_col, kb.h, kb.font, kb.opt, "AB789");

	y += kb.h + margin_top;
	EVE_CoCmd_keys(s_pHalContext, x, y, kb.w * num_col, kb.h, kb.font, kb.opt, "CD456");

	y += kb.h + margin_top;
	EVE_CoCmd_keys(s_pHalContext, x, y, kb.w * num_col, kb.h, kb.font, kb.opt, "EF123");

	y += kb.h + margin_top;
	EVE_CoCmd_keys(s_pHalContext, x + kb.w * 2, y, kb.w - margin_left, kb.h - margin_left, kb.font, kb.opt, "0");
	EVE_CoCmd_gradColor(s_pHalContext, 0x00ffff);

	x = kb.x + margin_left + kb.w * 3;
	EVE_Cmd_wr32(s_pHalContext, TAG(KEYCODE_SPACE));
	EVE_CoCmd_button(s_pHalContext, x, y, kb.w * 2 - margin_left * 2, kb.h - margin_left, kb.font, 0, "Space");

	x = kb.x + 10 + kb.w * num_col;
	y = kb.y;
	EVE_Cmd_wr32(s_pHalContext, TAG(KEYCODE_BACK));
	EVE_CoCmd_button(s_pHalContext, x, y, kb.w, kb.h, kb.font, 0, "  <- ");

	y += kb.h + margin_top;
	EVE_Cmd_wr32(s_pHalContext, TAG(KEYCODE_CLOSE));
	EVE_CoCmd_button(s_pHalContext, x, y, kb.w, kb.h, kb.font, 0, "Close");
}

void keyboard_draw(Keyboard kb) {
	isKbActive = 1;
	switch (kb.type) {
	case KEYBOARD_0_9:
		keyboard_draw_type_09(kb);
		break;
	case KEYBOARD_HEXA:
		keyboard_draw_hexa(kb);
		break;
	default:
		APP_DBG("Keyboard is not implemented");
		break;
	}
}

/// Input
void input_event(Input* ip) {
	Keyboard kb;
	kb.type = ip->keyboard_type;
	kb.w = KEY_W;
	kb.h = KEY_W;
	kb.font = 21;
	kb.opt = 0;
	kb.x = ip->x;
	kb.y = ip->y + ip->h + 10;

	if (ip->isKeyboardActive == 0 &&
		Gesture_Get()->tagReleased == ip->tag &&
		isKbActive == 0) {
		ip->isKeyboardActive = 1;
		return;
	}

	if (ip->isKeyboardActive == 1) {
		uint8_t key = keyboard_event(&kb);
		if (key == KEYCODE_CLOSE) {
			ip->isKeyboardActive = 0;
			isKbActive = 0;
			return;
		}
		else if (key == KEYCODE_BACK) {
			if (ip->crrLen > 0) {
				ip->currText[ip->crrLen - 1] = 0;
				ip->crrLen--;
			}
		}
		else if (key >= KEYTAG_MIN && key <= KEYTAG_MAX) {
			if (INPUT_MAXLEN > ip->crrLen) {
				ip->currText[ip->crrLen] = key;
				ip->crrLen++;
			}
		}

		keyboard_draw(kb);
	}
}

void input_draw(Input* ip) {
	uint32_t text_x = ip->x;
	uint32_t text_y = ip->y + (ip->h / 2) - (30 - ip->font);

	// Border black 1 pixel
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0, 0, 0));
	EVE_Cmd_wr32(s_pHalContext, TAG(ip->tag));
	EVE_Cmd_wr32(s_pHalContext, BEGIN(RECTS));
	// EVE_Cmd_wr32(s_pHalContext, LINE_WIDTH(VP(ip->h)));
	EVE_Cmd_wr32(s_pHalContext, VERTEX2F(VP(ip->x), VP(ip->y)));
	EVE_Cmd_wr32(s_pHalContext, VERTEX2F(VP(ip->x + ip->w), VP(ip->y + ip->h)));

	// Text frame white
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 255, 255));
	EVE_Cmd_wr32(s_pHalContext, BEGIN(RECTS));
	// EVE_Cmd_wr32(s_pHalContext, LINE_WIDTH(VP(ip->h)));
	EVE_Cmd_wr32(s_pHalContext, VERTEX2F(VP(ip->x), VP(ip->y)));
	EVE_Cmd_wr32(s_pHalContext, VERTEX2F(VP(ip->x + ip->w), VP(ip->y + ip->h)));

	// text black
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0, 0, 0));
	uint8_t str[INPUT_MAXLEN + INPUT_PREFIX];
	snprintf(str, sizeof(str), "%s%s", ip->prefix, ip->currText);
	EVE_CoCmd_text(s_pHalContext, text_x, text_y, ip->font, ip->opt, str);
}

void input_init(Input* ip, uint32_t tag, KEYBOARD_TYPE keyboard_type,
	uint8_t* currText, uint8_t* prefix,
	uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t font, uint16_t opt) {

	memset(ip->currText, 0, INPUT_MAXLEN);
	memset(ip->prefix, 0, INPUT_PREFIX);

	ip->crrLen = strlen(currText);
	memcpy(ip->currText, currText, ip->crrLen);

	int t = strlen(prefix);
	memcpy(ip->prefix, prefix, strlen(prefix));

	ip->tag = tag;
	ip->x = x;
	ip->y = y;
	ip->w = w;
	ip->h = h;
	ip->opt = opt;
	ip->font = font;
	ip->keyboard_type = keyboard_type;

	ip->isKeyboardActive = 0;
	ip->crrLen = 0;
}

///Text area
void textArea(uint8_t isWrap, uint8_t* title, uint8_t* txt, uint32_t txt_size,
	uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t font, uint16_t opt) {

	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0, 0, 0));
	EVE_CoCmd_text(s_pHalContext, x, y, font, opt | OPT_FILL, title);

	y += 21;
	h -= 21;
	// Border black 1 pixel
	EVE_Cmd_wr32(s_pHalContext, TAG(0));
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0, 0, 0));
	EVE_Cmd_wr32(s_pHalContext, BEGIN(RECTS));
	//EVE_Cmd_wr32(s_pHalContext, LINE_WIDTH(VP(w)));
	EVE_Cmd_wr32(s_pHalContext, VERTEX2F(VP(x), VP(y)));
	EVE_Cmd_wr32(s_pHalContext, VERTEX2F(VP(x + w), VP(y + h)));

	// Text frame white
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 255, 255));
	EVE_Cmd_wr32(s_pHalContext, BEGIN(RECTS));
	//EVE_Cmd_wr32(s_pHalContext, LINE_WIDTH(VP(w)));
	EVE_Cmd_wr32(s_pHalContext, VERTEX2F(VP(x), VP(y)));
	EVE_Cmd_wr32(s_pHalContext, VERTEX2F(VP(x + w), VP(y + h)));

	// text black
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0, 0, 0));
	txt_size = txt_size > TEXTARE_MAX_S ? TEXTARE_MAX_S : txt_size;
	txt[txt_size - 1] = 0; /// Ensure end of line, avoid hanging up
	EVE_CoCmd_text(s_pHalContext, x, y, font, opt, txt);
}

/// Program
uint32_t sendtx(uint32_t num, uint8_t* data) {
	const allign = 4;
	int numW = ALIGN(num, allign);
	int padding = numW - num;
	isPreviousError = 0;

	/// Deselect flash
	EVE_Cmd_wr32(s_pHalContext, CMD_FLASHSPIDESEL);
	EVE_Cmd_waitFlush(s_pHalContext);

	EVE_Cmd_wr32(s_pHalContext, CMD_FLASHSPITX);
	EVE_Cmd_wr32(s_pHalContext, num);
	for (int i = 0; i < num; i += allign) {
		uint32_t d32 = 0;
		uint8_t d8[4];
		int k = 0;
		for (int j = (allign - 1); j >= 0; j--) {
			if ((i + j) < num) {
				d8[k++] = data[i + j];
			}
			else {
				d8[k++] = 0;
			}
		}
		d32 = d8[0];
		d32 = (d32 << 8) | d8[1];
		d32 = (d32 << 8) | d8[2];
		d32 = (d32 << 8) | d8[3];

		EVE_Cmd_wr32(s_pHalContext, d32);
		d32 = 0;
	}
	EVE_CoCmd_nop(s_pHalContext);
	EVE_Cmd_waitFlush(s_pHalContext);
	return;
}

uint32_t sendrx(uint32_t dest, uint32_t num, uint8_t* buffer) {
	isPreviousError = 0;
	
	memset(buffer, 0, num);
	EVE_CoCmd_memZero(s_pHalContext, dest, num);
	EVE_Cmd_wr32(s_pHalContext, CMD_FLASHSPIRX);
	EVE_Cmd_wr32(s_pHalContext, dest);
	EVE_Cmd_wr32(s_pHalContext, num);
	EVE_Cmd_waitFlush(s_pHalContext);

	EVE_Hal_rdMem(s_pHalContext, buffer, dest, num);
	EVE_Cmd_waitFlush(s_pHalContext);
}

uint32_t str2dec_32bit(uint8_t* str) {
	return strtol(str, NULL, 16);
}

uint8_t getParamtx(uint8_t* string, PARAM* pm) {
	// command "89ABCDEF 89ABCDEF" = "89ABCDEF 89 AB CD EF"
	// command "       F 89ABCDEF" = "       F 89 AB CD EF"
	// command "       F       EF" = "       F EF 00 00 00" 
	// command "       F      EF0" = "       F EF 00 00 00" 

	uint8_t isParsingOk = 1;

	pm->len32bit = 0;

#define CHAR_PER_INT32  8 // 1 number 32 bit has 8 character 0->F
	uint8_t param[CHAR_PER_INT32];
	uint8_t index = 0;

	for (int i = 0; string[i] != 0; i++)
	{
		int len = 0;
		if (string[i] == ' ' || string[i + 1] == 0) {
			if (string[i + 1] == 0) { /// Last character
				if (index >= CHAR_PER_INT32) {
					isParsingOk = 0;
					return isParsingOk;
				}
				param[index] = string[i];
				index++;
			}
			if (pm->len32bit > 0) {
				/// Parse 8bit string from 2nd param
				pm->len8bit = pm->len32bit * 4;
				pm->arr32bit[pm->len32bit] = 0;

				for (int j = 0; j < index; j += 2) {
					uint8_t c[3];
					c[0] = param[j + 0];
					c[1] = param[j + 1];
					c[2] = 0; // end of string
					uint32_t num8bit = str2dec_32bit(c);

					pm->arr8bit[pm->len8bit++] = num8bit;
				}
				pm->len32bit++;
			}
			else {
				/// Parse 32bit number 1st param
				pm->arr32bit[pm->len32bit] = str2dec_32bit(param);
				pm->len32bit++;
			}

			/// Reset index
			index = 0;
			memset(param, 0, sizeof(param));
		}
		else {
			if (index >= CHAR_PER_INT32) {
				isParsingOk = 0;
				return isParsingOk;
			}

			/// character to 8bit array
			param[index] = string[i];
			index++;
		}
	}

	return isParsingOk;
}

uint8_t getParamrx(uint8_t* string, PARAM* pm) {
	// command "89ABCDEF 89ABCDEF" = "89ABCDEF 89ABCDEF"
	// command "       F 89ABCDEF" = "       F 89ABCDEF"
	// command "       F       EF" = "       F 000000EF" 
	// command "       F      EF0" = "       F 00000EF0"

	uint8_t isParsingOk = 1;

	pm->len32bit = 0;

#define CHAR_PER_INT32  8 // 1 number 32 bit has 8 character 0->F
	uint8_t param[CHAR_PER_INT32];
	uint8_t index = 0;

	for (int i = 0; string[i] != 0; i++)
	{
		int len = 0;
		if (string[i] == ' ' || string[i + 1] == 0) {
			if (string[i + 1] == 0) { /// Last character
				if (index >= CHAR_PER_INT32) {
					isParsingOk = 0;
					return isParsingOk;
				}
				param[index] = string[i];
				index++;
			}
			// String to int32 bit
			pm->arr32bit[pm->len32bit] = str2dec_32bit(param);
			pm->len32bit++;

			/// Reset index
			index = 0;
			memset(param, 0, sizeof(param));
		}
		else {

			if (index >= CHAR_PER_INT32) {
				isParsingOk = 0;
				return isParsingOk;
			}
			param[index] = string[i];
			index++;
		}
	}

	return isParsingOk;
}

uint8_t* int8_2_char_0xXX(uint8_t v) {
	static char s[5];
	snprintf(s, 5, "0x%02X", v);
	return s;
}

uint8_t* int8_2_char_XX(uint8_t v) {
	static char s[3];
	snprintf(s, 3, "%02X", v);
	return s;
}

void char_to_hexaString_8x(uint8_t* in, uint8_t* out, int numGet, int int32_per_line) {
	int i = 0, j = 0, start4bytes = 0, newline = 0;

	for (; i < numGet; i++) {
		uint8_t* s = int8_2_char_XX(in[i]);
		out[j++] = s[0];
		out[j++] = s[1];
		out[j + 1] = 0;

		start4bytes++;
		if (start4bytes == 4) {
			out[j++] = ' ';
			start4bytes = 0;
			newline++;
			if (newline == int32_per_line) {
				out[j - 1] = '\n';
				newline = 0;
			}
		}
	}
}

void char_to_hexaString_2x(uint8_t* in, uint8_t* out, int numGet, int int32_per_line) {
	memset(out, 0, numGet * 4 + numGet / 4 + (numGet / int32_per_line));
	int i = 0, j = 0, start4bytes = 0, newline = 0;

	for (; i < numGet; i++) {
		uint8_t* s = int8_2_char_0xXX(in[i]);
		// out[j++] = s[0]; // 0
		// out[j++] = s[1]; // x
		out[j++] = s[2];
		out[j++] = s[3];
		out[j + 1] = 0;// EOL
		start4bytes++;

		if (start4bytes == 4) {
			out[j++] = ' ';
			start4bytes = 0;
			newline++;
			if (newline == int32_per_line) {
				out[j - 1] = '\n';
				newline = 0;
			}
			else {
				//out[j++] = ' '; // space every 4 bytes
			}
		}
		else {
			//out[j++] = ' '; // space every 4 bytes
		}
	}
}

void ReadSFDP_raw() {
	sendtx(4, "\x5A\x00\x00\x00");
	sendrx(0, 16, buff);

	const int offset_raw = 0x0C + 1; /// 1 = dummy byte

	uint8_t command[4];
	uint8_t commandDisp[11];
	command[0] = buff[offset_raw];
	command[1] = buff[offset_raw + 2];
	command[2] = buff[offset_raw + 4];
	command[3] = 0;

	memset(commandDisp, 0, sizeof(commandDisp));
	commandDisp[0] = '4';
	commandDisp[1] = ' ';
	commandDisp[2] = '5';
	commandDisp[3] = 'A';
	sprintf(&commandDisp[4], "%02X", command[0]);
	sprintf(&commandDisp[6], "%02X", command[1]);
	sprintf(&commandDisp[8], "%02X", command[2]);

	uint8_t rx_str[] = "0 41"; // 1+16*4 = 65 = 41h
	memcpy(ip1.currText, commandDisp, sizeof(commandDisp));
	memcpy(ip2.currText, rx_str, sizeof(rx_str));

	ip1.crrLen = strlen(ip1.currText);
	ip2.crrLen = strlen(ip2.currText);

}

/// return 0 = error
int event(int event_tag) {
	PARAM pm;
	uint32_t numGet = 0;

	memset(pm.arr32bit, 0, sizeof(pm.arr32bit));

	int event_num = Gesture_Get()->tagReleased;
	int numDispPerLine = s_pHalContext->Width < 800 ? 5 : 8;
	if (event_tag > 0) {
		event_num = event_tag;
	}

	switch (event_num) {
	case TAG_BTN1:
		/// send tx
		if (!getParamtx(ip1.currText, &pm)) {
			return 0;
		}

		sendtx(pm.arr32bit[0], &pm.arr32bit[1]);

		break;
	case TAG_BTN2:
		memset(buff, 0, sizeof(buff));

		/// send rx and read ramg
		if (!getParamrx(ip2.currText, &pm)) {
			return 0;
		}
		numGet = pm.arr32bit[1];
		if (numGet > sizeof(buff)) {
			// Memory overflow
			return 0;
		}
		sendrx(pm.arr32bit[0], numGet, buff);
		char_to_hexaString_2x(buff, buff2disp, numGet, numDispPerLine);
		break;
	case TAG_BTN_GET_DEVICEID:
		memcpy(ip1.currText, sc[0].w, sizeof(sc[0].w));
		memcpy(ip2.currText, sc[0].r, sizeof(sc[0].r));
		ip1.crrLen = strlen(ip1.currText);
		ip2.crrLen = strlen(ip2.currText);
		return event(TAG_BTN1) | event(TAG_BTN2);
		break;
	case TAG_BTN_GET_UNIQID:
		memcpy(ip1.currText, sc[1].w, sizeof(sc[1].w));
		memcpy(ip2.currText, sc[1].r, sizeof(sc[1].r));
		ip1.crrLen = strlen(ip1.currText);
		ip2.crrLen = strlen(ip2.currText);
		return event(TAG_BTN1) | event(TAG_BTN2);
		break;
	case TAG_BTN_GET_JEDEC:
		memcpy(ip1.currText, sc[2].w, sizeof(sc[2].w));
		memcpy(ip2.currText, sc[2].r, sizeof(sc[2].r));
		ip1.crrLen = strlen(ip1.currText);
		ip2.crrLen = strlen(ip2.currText);
		return event(TAG_BTN1) | event(TAG_BTN2);
		break;
	case TAG_BTN_GET_SFDP:
		memcpy(ip1.currText, sc[3].w, sizeof(sc[3].w));
		memcpy(ip2.currText, sc[3].r, sizeof(sc[3].r));
		ip1.crrLen = strlen(ip1.currText);
		ip2.crrLen = strlen(ip2.currText);
		return event(TAG_BTN1) | event(TAG_BTN2);
		break;
	case TAG_BTN_GET_SFDP_RAW:
		ReadSFDP_raw();
		return event(TAG_BTN1) | event(TAG_BTN2);
		break;
	default:
		break;
	}

	int x = 15;
	int y = 50 + 100;
	int w = TEXTARE_W;
	int h = s_pHalContext->Height - y - 10;
	int font = s_pHalContext->Width<800?20:21;
	int opt = 0;
	textArea(0, "Ram_G: ", buff2disp, BUFFDISPSIZE, x, y, w, h, font, opt);

	//y += h + 15;
	//textArea(1, "Ram_G in text:", buff, BUFFSIZE, x, y, w, h, font, opt);

	return 1;///no error
}

void ui() {
	/// Init 2 input newly
	uint32_t font_btn = s_pHalContext->Width < 800 ? 16 : 18;
	input_init(&ip1, TAG_IP1, KEYBOARD_HEXA, "", "CMD_FLASHSPITX ", 15, 50 + 10, INPUT_W, 30, font_btn, 0);
	input_init(&ip2, TAG_IP2, KEYBOARD_HEXA, "", "CMD_FLASHSPIRX ", 15, 50 + 50, INPUT_W, 30, font_btn, 0);

	int sc_id = 0;
	int sc_num = 4; // the number of sc array
	uint32_t font = s_pHalContext->Width < 800 ? 22 : 30;

	while (1) {
		Display_Start(s_pHalContext);
		Gesture_Renew(s_pHalContext);

		/// Draw 2 text input
		input_draw(&ip1);
		input_draw(&ip2);

		// button text
		EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0xFF, 0xFF, 0xFF));

		/// Send button
		EVE_Cmd_wr32(s_pHalContext, TAG(TAG_BTN1));
		EVE_CoCmd_button(s_pHalContext, ip1.x + ip1.w + 10, ip1.y, SEND_BTN_W, ip1.h, ip1.font, 0, "Send Tx");

		/// Send button
		EVE_Cmd_wr32(s_pHalContext, TAG(TAG_BTN2));
		EVE_CoCmd_button(s_pHalContext, ip2.x + ip2.w + 10, ip2.y, SEND_BTN_W, ip2.h, ip2.font, 0, "Send Rx");

		/// Run automation scenarios
		if (sc_id < sc_num) {
			char disp[300];
			sprintf(disp, "Running the scenario %d%s : %s", sc_id + 1,
				sc_id == 0 ? "st" : sc_id == 1 ? "nd" : sc_id == 2 ? "rd" : "th",
				sc[sc_id].title);
			// text text
			EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0, 0, 0));
			EVE_CoCmd_text(s_pHalContext, 10, 10, font, 0, disp);

			memset(ip1.currText, 0, INPUT_MAXLEN);
			memset(ip2.currText, 0, INPUT_MAXLEN);

			ip1.crrLen = strlen(sc[sc_id].w);
			ip2.crrLen = strlen(sc[sc_id].r);

			memcpy(ip1.currText, sc[sc_id].w, ip1.crrLen);
			memcpy(ip2.currText, sc[sc_id].r, ip2.crrLen);

			int ret = event(TAG_BTN1);
			EVE_sleep(1000);
			ret |= event(TAG_BTN2);
			if (!ret) {
				EVE_CoCmd_text(s_pHalContext, 5, 1, 25, 0, "Parsing error");
				APP_DBG("Event error");
			}

			input_draw(&ip1);
			input_draw(&ip2);
		}
		else {
			/// User input directly to send command
			// Top Header
			EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0, 0, 0));
			if (isPreviousError) {
				EVE_CoCmd_text(s_pHalContext, 5, 1, 25, 0, "Parsing error");
				APP_DBG("Event error");
			}
			else {
				EVE_CoCmd_text(s_pHalContext, 10, 10, font, 0, "Send Tx then Rx to get flash content in low-level mode");
			}

			/// Control buttons
			int x = ip1.x + ip1.w + SEND_BTN_W + 20;
			int y = ip1.y;
			int w = 120;
			int h = 30;
			int font = ip1.font;
			int opt = 0;
			const int margin_top = 5;

			// button text
			EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0xFF, 0xFF, 0xFF));

			/// JEDEC ID
			EVE_Cmd_wr32(s_pHalContext, TAG(TAG_BTN_GET_DEVICEID));
			EVE_CoCmd_button(s_pHalContext, x, y, w, h, font, opt, "Get Device ID");

			y += h + margin_top;
			EVE_Cmd_wr32(s_pHalContext, TAG(TAG_BTN_GET_UNIQID));
			EVE_CoCmd_button(s_pHalContext, x, y, w, h, font, opt, "Get Unique ID");
			y += h + margin_top;
			EVE_Cmd_wr32(s_pHalContext, TAG(TAG_BTN_GET_JEDEC));
			EVE_CoCmd_button(s_pHalContext, x, y, w, h, font, opt, "Get JEDEC ID");
			y += h + margin_top;
			EVE_Cmd_wr32(s_pHalContext, TAG(TAG_BTN_GET_SFDP));
			EVE_CoCmd_button(s_pHalContext, x, y, w, h, font, opt, "Get SFDP header");
			y += h + margin_top;
			EVE_Cmd_wr32(s_pHalContext, TAG(TAG_BTN_GET_SFDP_RAW));
			EVE_CoCmd_button(s_pHalContext, x, y, w, h, font, opt, "Get SFDP raw");
			y += h + margin_top;
			
			int ret = event(0);
			if (!ret) {
				isPreviousError = 1;
			}

			input_event(&ip1);
			input_event(&ip2);
		}
		Display_End(s_pHalContext);

		if (sc_id < sc_num) {
			EVE_sleep(sc[sc_id].duration);
			sc_id++;
		}
	}
}

void TestCmd() {
	int byte4 = 4;
#define CENTER_X ((s_pHalContext->Width - w) / 2)
#define CENTER_Y ((s_pHalContext->Height -h) / 2)

	uint32_t w = s_pHalContext->Width - 200;
	uint32_t h = 100;
	uint32_t x = (s_pHalContext->Width - w) / 2;
	uint32_t y = (s_pHalContext->Height - h) / 2;
	uint32_t font = 21;
	uint32_t opt = 0;
	uint32_t num_per_line = w / font / 4;

	//sendtx(1, "\x9F");
	//sendrx(0, 3, buff);
	//APP_DBG("%d-%d-%d", buff[0], buff[1], buff[2]);

	sendtx(4, "\x5A");
	sendrx(0, 1, buff);
	sendrx(0, 256, buff);
	FileIO_Buffer_To_File("sfdp.bin", buff, 256);

	char_to_hexaString_8x(buff, buff2disp, 256, num_per_line);
	Display_Start(s_pHalContext);

	h = 400;
	textArea(0, "SFDP in hex: ", buff2disp, BUFFDISPSIZE, CENTER_X, CENTER_Y, w, h, font, opt);
	//textArea(1, "SFDP in text:", buff, BUFFSIZE, 15, 15 + 100 + 15, s_pHalContext->Width - 200, 100, 21, 0);
	Display_End(s_pHalContext);
	EVE_sleep(2000);
}

void DemoFlashBitbang(EVE_HalContext* pHalContext) {
#if !defined(EVE_FLASH_AVAILABLE)
#error Platform not supported
#endif
	s_pHalContext = pHalContext;

	/// Detach flash
	FlashHelper_SwitchState(s_pHalContext, FLASH_STATUS_DETACHED);
	EVE_Cmd_waitFlush(s_pHalContext);
	uint8_t curr_flash_state = EVE_Hal_rd8(s_pHalContext, REG_FLASH_STATUS);
	if (FLASH_STATUS_DETACHED != curr_flash_state) {
		APP_ERR("Flash is not able to detach. Exitting...");
		exit(0);
	}

	/// Deselect flash
	EVE_Cmd_wr32(s_pHalContext, CMD_FLASHSPIDESEL);
	EVE_Cmd_waitFlush(s_pHalContext);

	//TestCmd();

	/// Start demo
	ui();
}

