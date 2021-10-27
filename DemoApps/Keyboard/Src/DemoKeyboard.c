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

#define ON          1
#define OFF         0						 
#define Font        27					// Font Size
#ifdef DISPLAY_RESOLUTION_WVGA
#if defined(MSVC_PLATFORM) || defined(BT8XXEMU_PLATFORM)
#define MAX_LINES  6					// Max Lines allows to Display
#else
#define MAX_LINES  5					// Max Lines allows to Display
#endif

#else
#define MAX_LINES   4
#endif
#define SPECIAL_FUN     251
#define BACK_SPACE	251				// Back space
#define CAPS_LOCK	252				// Caps Lock
#define NUMBER_LOCK	253				// Number Lock
#define BACK		254				// Exit 

#define LINE_STARTPOS	s_pHalContext->Width/50			// Start of Line
#define LINE_ENDPOS	s_pHalContext->Width			// max length of the line

#if EVE_CHIPID <= EVE_FT801
#define ROMFONT_TABLEADDRESS (0xFFFFC)
#else
#define ROMFONT_TABLEADDRESS 3145724UL // 2F FFFCh
#endif

struct {
	uint8_t Key_Detect : 1;
	uint8_t Caps : 1;
	uint8_t Numeric : 1;
	uint8_t Exit : 1;
} Flag;

struct Notepad_buffer {
	char8_t *temp;
	char8_t notepad[MAX_LINES + 1][800];
} Buffer;

static uchar8_t istouch() {
	return !(EVE_Hal_rd16(s_pHalContext, REG_TOUCH_RAW_XY) & 0x8000);
}

uint8_t Read_Keypad() {
	static uint8_t Read_tag = 0, temp_tag = 0, ret_tag = 0, touch_detect = 1;
	Read_tag = EVE_Hal_rd8(s_pHalContext, REG_TOUCH_TAG);
	ret_tag = NULL;
	if (istouch() == 0)
		touch_detect = 0;
	if (Read_tag != NULL)								// Allow if the Key is released
	{
		if (temp_tag != Read_tag && touch_detect == 0) {
			temp_tag = Read_tag;											// Load the Read tag to temp variable
			Play_Sound(s_pHalContext, 0x51, 100, 100);
			touch_detect = 1;
		}
	}
	else {
		if (temp_tag != 0) {
			Flag.Key_Detect = 1;
			Read_tag = temp_tag;
		}
		temp_tag = 0;
	}
	return Read_tag;
}

uint8_t Gpu_Rom_Font_WH(uint8_t Char, uint8_t font) {
	uint32_t ptr, Wptr;
	uint8_t Width = 0;

	ptr = EVE_Hal_rd32(s_pHalContext, ROMFONT_TABLEADDRESS);
	// read Width of the character
	Wptr = (ptr + (148 * (font - 16))) + Char;	// (table starts at font 16)
	Width = EVE_Hal_rd8(s_pHalContext, Wptr);
	return Width;
}

// notepad
void DemoKeyboard(EVE_HalContext* pHalContext) {
	s_pHalContext = pHalContext;
	/*local variables*/
	uint8_t Line = 0;
	uint16_t Disp_pos = 0, But_opt;
	uint16_t Read_sfk = 0, tval;
	uint16_t noofchars = 0, line2disp = 0, nextline = 0;
	uint8_t font = 27;

	// Clear Linebuffer
	for (tval = 0; tval < MAX_LINES; tval++)
		memset(&Buffer.notepad[tval], '\0', sizeof(Buffer.notepad[tval]));

	/*intial setup*/
	Line = 0;					// Starting line
	Disp_pos = LINE_STARTPOS;	                        // starting pos
	Flag.Numeric = OFF;                             // Disable the numbers and spcial charaters
	memset((Buffer.notepad[Line] + 0), '_', 1);	  	// For Cursor
	Disp_pos += Gpu_Rom_Font_WH(Buffer.notepad[Line][0], Font);	// Update the Disp_Pos
	noofchars += 1;                                                   // for cursor
																	  /*enter*/
	Flag.Exit = 0;
	do {
		Read_sfk = Read_Keypad();                // read the keys

		if (Flag.Key_Detect) {                    // check if key is pressed
			Flag.Key_Detect = 0;                     // clear it
			if (Read_sfk >= SPECIAL_FUN) {              // check any special function keys are pressed
				switch (Read_sfk) {
				case BACK_SPACE:
					if (noofchars > 1)  // check in the line there is any characters are present,cursor not include
					{
						noofchars -= 1;                      // clear the character inthe buffer
						Disp_pos -= Gpu_Rom_Font_WH(*(Buffer.notepad[Line] + noofchars - 1), Font); // Update the Disp_Pos
					}
					else {
						if (Line >= (MAX_LINES - 1))
							Line--;
						else
							Line = 0;              // check the lines
						noofchars = strlen(Buffer.notepad[Line]);		    // Read the len of the line
						for (tval = 0; tval < noofchars; tval++)			     // Compute the length of the Line
							Disp_pos += Gpu_Rom_Font_WH(Buffer.notepad[Line][tval], Font);  // Update the Disp_Pos
					}
					Buffer.temp = (Buffer.notepad[Line] + noofchars);     // load into temporary buffer
					Buffer.temp[-1] = '_';				  // update the string
					Buffer.temp[0] = '\0';
					break;

				case CAPS_LOCK:
					Flag.Caps ^= 1;        // toggle the caps lock on when the key detect
					break;

				case NUMBER_LOCK:
					Flag.Numeric ^= 1;    // toggle the number lock on when the key detect
					break;

				case BACK:
					for (tval = 0; tval < MAX_LINES; tval++)
						memset(&Buffer.notepad[tval], '\0', sizeof(Buffer.notepad[tval]));
					Line = 0;					// Starting line
					Disp_pos = LINE_STARTPOS;	                        // starting pos
					memset((Buffer.notepad[Line] + 0), '_', 1);	  	// For Cursor
					Disp_pos += Gpu_Rom_Font_WH(Buffer.notepad[Line][0], Font);	// Update the Disp_Pos
					noofchars += 1;
					break;
				}
			}
			else {
				Disp_pos += Gpu_Rom_Font_WH(Read_sfk, Font);              // update dispos
				Buffer.temp = Buffer.notepad[Line] + strlen(Buffer.notepad[Line]);  // load into temporary buffer
				Buffer.temp[-1] = Read_sfk;		 // update the string
				Buffer.temp[0] = '_';
				Buffer.temp[1] = '\0';
				noofchars = strlen(Buffer.notepad[Line]);    // get the string len
				if (Disp_pos > LINE_ENDPOS)             // check if there is place to put a character in a specific line
				{
					Buffer.temp = Buffer.notepad[Line] + (strlen(Buffer.notepad[Line]) - 1);
					Buffer.temp[0] = '\0';
					noofchars -= 1;
					Disp_pos = LINE_STARTPOS;
					Line++;
					if (Line >= MAX_LINES)
						Line = 0;
					memset((Buffer.notepad[Line]), '\0', sizeof(Buffer.notepad[Line]));	// Clear the line buffer
					for (; noofchars >= 1; noofchars--, tval++) {
						if (Buffer.notepad[Line - 1][noofchars] == ' ' || Buffer.notepad[Line - 1][noofchars] == '.')// In case of space(New String) or end of statement(.)
						{
							memset(Buffer.notepad[Line - 1] + noofchars, '\0', 1);
							noofchars += 1;							// Include the space
							memcpy(&Buffer.notepad[Line], (Buffer.notepad[Line - 1] + noofchars), tval);
							break;
						}
					}
					noofchars = strlen(Buffer.notepad[Line]);
					Buffer.temp = Buffer.notepad[Line] + noofchars;
					Buffer.temp[0] = '_';
					Buffer.temp[1] = '\0';
					for (tval = 0; tval < noofchars; tval++)
						Disp_pos += Gpu_Rom_Font_WH(Buffer.notepad[Line][tval], Font);	// Update the Disp_Pos
				}
			}
		}

		// Display List start
		EVE_CoCmd_dlStart(s_pHalContext);
		EVE_Cmd_wr32(s_pHalContext, CLEAR_COLOR_RGB(100, 100, 100));
		EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1));
		EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(255, 255, 255));
		EVE_Cmd_wr32(s_pHalContext, TAG_MASK(1));            // enable tagbuffer updation
		EVE_CoCmd_fgColor(s_pHalContext, 0x703800);
		EVE_CoCmd_bgColor(s_pHalContext, 0x703800);
		But_opt = (Read_sfk == BACK) ? OPT_FLAT : 0;          // button color change if the button during press
		EVE_Cmd_wr32(s_pHalContext, TAG(BACK));												// Back		 Return to Home
		EVE_CoCmd_button(s_pHalContext, (s_pHalContext->Width * 0.855), (s_pHalContext->Height * 0.83), (s_pHalContext->Width * 0.146), (s_pHalContext->Height * 0.112),
			font, But_opt, "Clear");
		But_opt = (Read_sfk == BACK_SPACE) ? OPT_FLAT : 0;
		EVE_Cmd_wr32(s_pHalContext, TAG(BACK_SPACE));													// BackSpace
		EVE_CoCmd_button(s_pHalContext, (s_pHalContext, s_pHalContext->Width * 0.875), (s_pHalContext->Height * 0.70), (s_pHalContext->Width * 0.125),
			(s_pHalContext->Height * 0.112), font, But_opt, "<-");
		But_opt = (Read_sfk == ' ') ? OPT_FLAT : 0;
		EVE_Cmd_wr32(s_pHalContext, TAG(' '));															// Space
		EVE_CoCmd_button(s_pHalContext, (s_pHalContext->Width * 0.115), (s_pHalContext->Height * 0.83), (s_pHalContext->Width * 0.73), (s_pHalContext->Height * 0.112),
			font, But_opt, "Space");

		if (Flag.Numeric == OFF) {
			EVE_CoCmd_keys(s_pHalContext, 0, (s_pHalContext->Height * 0.442), s_pHalContext->Width, (s_pHalContext->Height * 0.112), font, Read_sfk,
				(Flag.Caps == ON ? "QWERTYUIOP" : "qwertyuiop"));
			EVE_CoCmd_keys(s_pHalContext, (s_pHalContext->Width * 0.042), (s_pHalContext->Height * 0.57), (s_pHalContext->Width * 0.96), (s_pHalContext->Height * 0.112),
				font, Read_sfk, (Flag.Caps == ON ? "ASDFGHJKL" : "asdfghjkl"));
			EVE_CoCmd_keys(s_pHalContext, (s_pHalContext->Width * 0.125), (s_pHalContext->Height * 0.70), (s_pHalContext->Width * 0.73), (s_pHalContext->Height * 0.112),
				font, Read_sfk, (Flag.Caps == ON ? "ZXCVBNM" : "zxcvbnm"));

			But_opt = (Read_sfk == CAPS_LOCK) ? OPT_FLAT : 0;
			EVE_Cmd_wr32(s_pHalContext, TAG(CAPS_LOCK));														// Capslock
			EVE_CoCmd_button(s_pHalContext, 0, (s_pHalContext->Height * 0.70), (s_pHalContext->Width * 0.10), (s_pHalContext->Height * 0.112), font, But_opt,
				"a^");
			But_opt = (Read_sfk == NUMBER_LOCK) ? OPT_FLAT : 0;
			EVE_Cmd_wr32(s_pHalContext, TAG(NUMBER_LOCK));												// Numberlock
			EVE_CoCmd_button(s_pHalContext, 0, (s_pHalContext->Height * 0.83), (s_pHalContext->Width * 0.10), (s_pHalContext->Height * 0.112), font, But_opt,
				"12*");
		}
		if (Flag.Numeric == ON) {
			EVE_CoCmd_keys(s_pHalContext, (s_pHalContext->Width * 0), (s_pHalContext->Height * 0.442), s_pHalContext->Width, (s_pHalContext->Height * 0.112), font,
				Read_sfk, "1234567890");
			EVE_CoCmd_keys(s_pHalContext, (s_pHalContext->Width * 0.042), (s_pHalContext->Height * 0.57), (s_pHalContext->Width * 0.96), (s_pHalContext->Height * 0.112),
				font, Read_sfk, "-@#$%^&*(");
			EVE_CoCmd_keys(s_pHalContext, (s_pHalContext->Width * 0.125), (s_pHalContext->Height * 0.70), (s_pHalContext->Width * 0.73), (s_pHalContext->Height * 0.112),
				font, Read_sfk, ")_+[]{}");
			But_opt = (Read_sfk == NUMBER_LOCK) ? OPT_FLAT : 0;
			EVE_Cmd_wr32(s_pHalContext, TAG(253));													// Numberlock
			EVE_CoCmd_button(s_pHalContext, 0, (s_pHalContext->Height * 0.83), (s_pHalContext->Width * 0.10), (s_pHalContext->Height * 0.112), font, But_opt,
				"AB*");
		}
		EVE_Cmd_wr32(s_pHalContext, TAG_MASK(0));										// Disable the tag buffer updates
		EVE_Cmd_wr32(s_pHalContext, SCISSOR_XY(0, 0));
		EVE_Cmd_wr32(s_pHalContext, SCISSOR_SIZE(s_pHalContext->Width, (uint16_t)(s_pHalContext->Height * 0.405)));
		EVE_Cmd_wr32(s_pHalContext, CLEAR_COLOR_RGB(255, 255, 255));
		EVE_Cmd_wr32(s_pHalContext, CLEAR(1, 1, 1));
		EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0, 0, 0));												// Text Color
		line2disp = 0;
		while (line2disp <= Line) {
			nextline = 3 + (line2disp * (s_pHalContext->Height * .073));
			EVE_CoCmd_text(s_pHalContext, line2disp, nextline, font, 0, (const char*)&Buffer.notepad[line2disp]);
			line2disp++;
		}
		EVE_Cmd_wr32(s_pHalContext, DISPLAY());
		EVE_CoCmd_swap(s_pHalContext);
		EVE_Cmd_waitFlush(s_pHalContext);
	} while (1);
}