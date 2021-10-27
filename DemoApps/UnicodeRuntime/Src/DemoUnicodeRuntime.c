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

#include "Common.h"
#include "Mmngr.h"
#include "Filemngr.h"
#include "Xfont.h"
#include "Utf8.h"

  //APP CONFIG
#define CHARS_PER_FRAME (128 * 500)
#define STREAMSIZE      (CHARS_PER_FRAME * 4) // some utf8 characters can be stored by 4 bytes
#define IS_DUMP_VC      0
#define CHARS_PER_LINE  60

static void TakeVC1Dump(char *filename) {
#if IS_DUMP_VC
	FILE *vc1dump = fopen(filename, "wb");
	const uint32_t VERSION = 110;
	fwrite(&VERSION, 4, 1, vc1dump);
	const uint32_t WIDTH = s_pHalContext->Width, HEIGHT = s_pHalContext->Height;
	fwrite(&WIDTH, 4, 1, vc1dump);
	fwrite(&HEIGHT, 4, 1, vc1dump);
	const uint32_t macro0 = 0;//EVE_Hal_rd32(s_pHalContext, REG_MACRO_0);
	const uint32_t macro1 = 0;// EVE_Hal_rd32(s_pHalContext, REG_MACRO_1);
	fwrite(&macro0, 4, 1, vc1dump);
	fwrite(&macro1, 4, 1, vc1dump);
	const uint32_t CRC_IMAGE = 0;
	fwrite(&CRC_IMAGE, 4, 1, vc1dump);

	{
		for (int i = 0; i < 1024 * 1024; i += 4) {
			uint32_t ram_g_4bytes = EVE_Hal_rd32(s_pHalContext, RAM_G + i);
			fwrite(&ram_g_4bytes, 4, 1, vc1dump);
		}

		for (int i = 0; i < 8 * 1024; i += 4) {
			uint32_t ram_dl_4bytes = EVE_Hal_rd32(s_pHalContext, RAM_DL + i);
			fwrite(&ram_dl_4bytes, 4, 1, vc1dump);
		}
	}

	fclose(vc1dump);
#endif
}

static void dumpCP(CODEPOINT *cp, int numCP) {
	int i = 0;
	for (i = 0; i < numCP; i++) {
		APP_DBG("cp: %08lx, bytes: %u ", cp[i].codepoint, cp[i].bytes);
	}
	APP_DBG("total: %d cp", i);
}
static void dumpXFont(int* xf, int isWidthData) {
#if defined(MSVC_PLATFORM)
	APP_INF("signature               : %08x", XF_SIGNATURE(xf));
	APP_INF("size                    : %d", XF_SIZE(xf));
	APP_INF("format                  : %d", XF_FORMAT(xf));
	APP_INF("swizzle                 : %d", XF_SWIZZLE(xf));
	APP_INF("layout_width            : %d", XF_LAYOUT_WIDTH(xf));
	APP_INF("layout_height           : %d", XF_LAYOUT_HEIGHT(xf));
	APP_INF("pixel_width             : %d", XF_PIXEL_WIDTH(xf));
	APP_INF("pixel_height            : %d", XF_PIXEL_HEIGHT(xf));
	APP_INF("start_of_Graphic_data   : %d", XF_START_OF_GRAPHIC_DATA(xf));
	APP_INF("number_of_characters    : %d", XF_NUMBER_OF_CHARACTERS(xf));
	APP_INF("gPtrOffset              : %d", XF_GPTR(xf) - xf);
	APP_INF("gPtrSize                : %d", XF_WPTR(xf) - xf);
	APP_INF("widthdataOffset         : %d", XF_WIDTH(xf) - xf);
#endif
	if (isWidthData) {
		APP_DBG("gPTR: ");
		dumpMem((char*)XF_GPTR(xf), XF_NUMBER_OF_CHARACTERS(xf) / 128);

		APP_DBG("wPTR: ");
		dumpMem((char*)XF_WPTR(xf), XF_NUMBER_OF_CHARACTERS(xf) / 128);

		APP_DBG("width data: ");
		dumpMem((char*)XF_WIDTH(xf), XF_NUMBER_OF_CHARACTERS(xf));
	}
}

static int extractGlyph(unsigned long cp, char *xfontBuffer, char *xfontOut,
	char *glyphBuffer, char *glyphOut) {
	int* xf = (int*)xfontBuffer;
	int sizeCount = 0;
	int gly_offset = getGlyOffset(xf, cp);
	uint32_t bytes_per_glyph = XF_LAYOUT_WIDTH(xf) * XF_LAYOUT_HEIGHT(xf);

	buff2File(glyphOut, &glyphBuffer[gly_offset], bytes_per_glyph, fl_append);

	unsigned char c = getWidth(xf, cp);
	buff2File(xfontOut, (char*)&c, 1, fl_append);

	return APP_OK;
}
static int extractXfont(int numStreamChars, int numStreamCp, CODEPOINT *cp,
	char *xfontBuffer, char *xfontOut, char *glyphBuffer, char *glyphOut) {
	int *xf = (int*)xfontBuffer;
	int xf_new_number_of_characters = ALIGN(numStreamCp, 128);
	uint32_t bytes_per_glyph = XF_LAYOUT_WIDTH(xf) * XF_LAYOUT_HEIGHT(xf);

	const int xfontHeaderSize = 40;
	const int xfontWptrSize = 4 * (xf_new_number_of_characters / 128);
	const int xfontGptrSize = 4 * (xf_new_number_of_characters / 128);
	const int xfontWidthSize = numStreamCp;
	int xf_size = xfontHeaderSize + xfontWptrSize + xfontGptrSize + xfontWidthSize;
	int xf_start_of_Graphic_data = ALIGN(xf_size, 1024);

	// Empty newXfont
	buff2File(xfontOut, "", 0, fl_write);
	buff2File(glyphOut, "", 0, fl_write);

	APP_DBG("write newXfont static header");
	buff2File(xfontOut, xfontBuffer, 4, fl_append);
	buff2File(xfontOut, (char*)&xf_size, 4, fl_append);
	buff2File(xfontOut, &xfontBuffer[8], 4 * 6, fl_append);
	buff2File(xfontOut, (char*)&xf_start_of_Graphic_data, 4, fl_append);
	buff2File(xfontOut, (char*)&xf_new_number_of_characters, 4, fl_append);

	APP_DBG("write newXfont gptr");
	for (int i = 0; i < (xf_new_number_of_characters / 128); i++) {
		unsigned int offset = i * 128 * bytes_per_glyph;
		buff2File(xfontOut, (char*)&offset, 4, fl_append);
	}

	APP_DBG("write newXfont wptr");
	for (int i = 0; i < (xf_new_number_of_characters / 128); i++) {
		unsigned int offset = i * 128 + 40
			+ 4 * (xf_new_number_of_characters / 128)
			+ 4 * (xf_new_number_of_characters / 128);
		buff2File(xfontOut, (char*)&offset, 4, fl_append);
	}

	APP_DBG("Creating new glyph");
	for (int i = 0; i < numStreamCp; i++) {
		extractGlyph(cp[i].codepoint, xfontBuffer, xfontOut, glyphBuffer,
			glyphOut);
	}

	return APP_OK;
}
static int prepareXFont(unsigned char *strIn, char *strOut, char *xfontBuffIn,
	char *xfontOut, char *glyphBuffIn, char *glyphOut, int *numCharCount) {
	int numStreamChars = 0;
	int numStreamCps = 0; // number of code points
	int numBytes = 0;
	CODEPOINT* cp = (CODEPOINT*)malloc(sizeof(CODEPOINT) * (STREAMSIZE + 2));

	memset(cp, 0, sizeof(CODEPOINT) * (STREAMSIZE + 2));

	APP_DBG("Get code points");
	numBytes = getCodePointsOrdinal(strIn, strOut, STREAMSIZE, cp,
		&numStreamChars, &numStreamCps, CHARS_PER_FRAME);
	*numCharCount = numStreamChars;

	APP_DBG("numCp = %d, numStreamChars = %d", numStreamCps, numStreamChars);

#if APPDBG
	APP_DBG("DumpCP: ");
	dumpCP(cp, numStreamCps);

	APP_DBG("Dump Str out: ");
	dumpMem(strOut, STREAMSIZE);

	dumpXFont(xfontBuffIn, true);
#endif 

	APP_DBG("Extract XFONT");
	extractXfont(numStreamChars, numStreamCps, cp, xfontBuffIn, xfontOut,
		glyphBuffIn, glyphOut);

	free(cp);

	return numBytes;
}

static char ** streamToArray2D(char *stream, int streamSize, char *xfontIn, int lineWidth, int *lineNum) {
	static char *ret[100];//maximum 100 line
	char *line;
	char* xfont = file2NewBuff(xfontIn, 0, 0, 0);
	unsigned long c;

	int next = 0, last = 0;
	int linecount = 0;
	int start = 0;
	int byteLinecount = 0;
	int lineWidthCount = 0, wordStop = 0;

	memset(ret, 0, sizeof(ret));
#if APPDEBUG
	APP_DBG("dumpMem(stream, streamSize);");
	dumpMem(stream, streamSize);
#endif
	while (next < streamSize) {
		next = utf8_next(stream, streamSize, next, &c);
		byteLinecount += next - last;
		last = next;

		unsigned char charWidth = getWidth(xfont, c);
		if (c == CP_SPACE) {//space
			wordStop = lineWidthCount; // lineWidthCount is pointing to last char of a word
		}
		lineWidthCount += charWidth;

		if (lineWidthCount >= lineWidth || c == CP_NEWLINE) { // new line
			line = memget(byteLinecount + 5);
			memset(line, 0, byteLinecount + 5);
			memcpy(line, &stream[start], byteLinecount);
#if APPDEBUG
			APP_DBG("dumpMem(line, byteLinecount);");
			dumpMem(line, byteLinecount);
#endif

			ret[linecount] = line;
			linecount++;

			start += byteLinecount;
			byteLinecount = 0;
			lineWidthCount = 0;
		}
	}
	*lineNum = linecount;

	return ret;
}
static void displayUnicode_rundown(char *stream, int streamSize, int fontCn,
	uint32_t addrXfont, char *xfontIn) {
	const int x = 10;
	const int bottom = s_pHalContext->Height - 10;;
	const int top = bottom - 50;

	const int linePerFrame = 8;
	const int lineHeight = 50;
	int y = 30, yOffset = 5;
	int lineStart = 0;
	int lineNum = 0;

	char **out = streamToArray2D(stream, streamSize, xfontIn, s_pHalContext->Width - x - x, &lineNum);

	do {
		yOffset = top;
		while (yOffset < bottom) {
			y = 0;

			Display_Start(s_pHalContext);
			EVE_CoCmd_setFont2(s_pHalContext, fontCn, addrXfont, 0);
			EVE_CoCmd_fgColor(s_pHalContext, 0x101010);

			EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0, 0, 0));
			EVE_Cmd_wr32(s_pHalContext, SCISSOR_XY(0, 0));
			EVE_Cmd_wr32(s_pHalContext, SCISSOR_SIZE(s_pHalContext->Width - x - x, bottom));

			EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(10, 10, 10));
			for (int i = lineStart; i < (linePerFrame + lineStart); i++) {
				EVE_CoCmd_text(s_pHalContext, x, y + yOffset, fontCn, 0, out[i]);
				y -= lineHeight;
			}

			Display_End(s_pHalContext);
			EVE_sleep(2);
			yOffset++;
		}
		lineStart++;
#if IS_DUMP_VC
		if (lineStart > 3) {
			APP_INF("TakeVC1Dump Start");
			char filename[100];
			sprintf(filename, "sampleApp_%d.vc1dump", lineStart);
			TakeVC1Dump(filename);
			APP_INF("TakeVC1Dump Done");

			EVE_sleep(100000);
			exit(0);
		}
#endif
	} while (lineNum - lineStart >= linePerFrame);

	EVE_sleep(3000);
}
static void displayUnicode_runup(char *stream, int streamSize, int fontCn,
	uint32_t addrXfont, char *xfontIn) {
	const int x = 10;
	const int top = 1, bottom = 50 + top;
	const int linePerFrame = 8;
	const int lineHeight = 50;
	int y = 30, yOffset = 5;
	int lineStart = 0;
	int lineNum = 0;

	char **out = streamToArray2D(stream, streamSize, xfontIn, s_pHalContext->Width - x - x, &lineNum);

	do {
		yOffset = bottom;
		while (yOffset > top) {
			y = 0;

			Display_Start(s_pHalContext); APP_DBG("");
			EVE_CoCmd_setFont2(s_pHalContext, fontCn, addrXfont, 0);
			EVE_CoCmd_fgColor(s_pHalContext, 0x101010);
			EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0, 0, 0));
			EVE_Cmd_wr32(s_pHalContext, SCISSOR_XY(x, top));
			EVE_Cmd_wr32(s_pHalContext, SCISSOR_SIZE(s_pHalContext->Width - x - x, s_pHalContext->Height - top - top));

			for (int i = lineStart; i < (linePerFrame + lineStart); i++) {
				EVE_CoCmd_text(s_pHalContext, x, y + yOffset, fontCn, 0, out[i]);
				y += lineHeight;

				APP_DBG("i: %d, y: %d", i, y + yOffset);
			}

			Display_End(s_pHalContext); APP_DBG("Display_End");
			EVE_sleep(2);
			yOffset--;
		}
		lineStart++;
	} while (lineNum - lineStart >= linePerFrame);

	EVE_sleep(3000);
}
static void displayUnicode_simple(char *stream, int numChars, int fontCn,
	uint32_t addrXfont) {
	const int x = 10, lines = 6;
	const int lineHeight = 50;
#define CHARS_PER_LINE 40
	char displayBuffer[CHARS_PER_LINE + 1];

	int y = 10;

	Display_Start(s_pHalContext);

	EVE_CoCmd_setFont2(s_pHalContext, fontCn, addrXfont, 0);

	EVE_CoCmd_fgColor(s_pHalContext, 0x101010);
	EVE_Cmd_wr32(s_pHalContext, COLOR_RGB(0, 0, 0));

	for (int i = 0; i < lines; i++) {
		int offsetStr = i * CHARS_PER_LINE;
		int remainStr = numChars - offsetStr;
		int bytes = remainStr > CHARS_PER_LINE ? CHARS_PER_LINE : remainStr;

		if (offsetStr >= numChars) {
			break;
		}
		memset(displayBuffer, 0, sizeof(displayBuffer));
		memcpy(displayBuffer, &stream[offsetStr], bytes);

		EVE_CoCmd_text(s_pHalContext, x, y, fontCn, 0, displayBuffer);

		y += lineHeight;
	}

	Display_End(s_pHalContext);
	EVE_sleep(5000);

	//clear screen
	Display_Start(s_pHalContext);
	Display_End(s_pHalContext);
}
static void set_font(char *xfont, char* glyph, uint32_t addrXfont,
	uint32_t addrGlyph) {
	Gpu_Hal_LoadImageToMemory(s_pHalContext, xfont, addrXfont, LOAD);
	EVE_Cmd_waitFlush(s_pHalContext);
	Gpu_Hal_LoadImageToMemory(s_pHalContext, glyph, addrGlyph, LOAD);
	EVE_Cmd_waitFlush(s_pHalContext);
}
static void displayUnicode(char *stream, char *xfont, char* glyph,
	uint32_t addrXfont, uint32_t addrGlyph, int streamSize) {
	const uint32_t fontCn = 0;

	set_font(xfont, glyph, addrXfont, addrGlyph);
	//displayUnicode_simple(stream, streamSize, fontCn, addrXfont);
	//displayUnicode_runup(stream, streamSize, fontCn, addrXfont, xfont);
	displayUnicode_rundown(stream, streamSize, fontCn, addrXfont, xfont);
}

static void createSampleTXT(char *utfTxt) {
	char bufferTest[] = "một hai ba bốn năm sáu bảy tám chín mười";

	dumpMem(bufferTest, sizeof(bufferTest));

	APP_DBG("Writting %d bytes to %s...", sizeof(bufferTest), utfTxt);
	buff2File(utfTxt, bufferTest, sizeof(bufferTest), fl_write);
}

void DemoUnicodeRuntime(EVE_HalContext* pHalContext) {
#if EVE_CHIPID < EVE_BT815
#error Platform not supported
#endif

	s_pHalContext = pHalContext;
#define TESTFOLDER TEST_DIR "\\NotoSans\\"

	// congifuration
	int isSDCardFileList = 0;
	int isCreateNewUTF8TestFile = 0;

	// program input
	char* xfontIn = TESTFOLDER "NotoSansCJK-Regular_29.xfont";
	char* glyphIn = TESTFOLDER "NotoSansCJK-Regular_29.glyph";
	char* txtIn = TESTFOLDER   "in_utf8.txt";
	char* xfontOut = TESTFOLDER "out/" "ex_utf8.xfo";
	char* glyphOut = TESTFOLDER "out/" "ex_utf8.gly";

	// variables
	unsigned char* strIn;
	unsigned char* strOut;
	char *xfontBuffIn;
	char *glyphBuffIn;
	int byteCount = 0;
	int byteRead = 0;

	strIn = malloc(STREAMSIZE);
	strOut = malloc(STREAMSIZE + 2);

	if (isSDCardFileList) {
		fileList("/");
	}

	if (isCreateNewUTF8TestFile) {
		createSampleTXT(txtIn);
	}

	xfontBuffIn = file2NewBuff(xfontIn, 0, 0, 0);
	glyphBuffIn = file2NewBuff(glyphIn, 0, 0, 0);

	while (true) {
		memset(strIn, 0, STREAMSIZE);
		memset(strOut, 0, STREAMSIZE + 2);

		buff2File(xfontOut, "", 0, fl_write);
		buff2File(glyphOut, "", 0, fl_write);

		APP_INF("Reading %s from %d...\n", txtIn, byteRead);
		if (APP_ERROR
			== file2Buff(txtIn, (char*)strIn, byteRead, STREAMSIZE,
				&byteCount)) {
			APP_ERR("Read %s failed", txtIn);
			break;
		}

		if (byteCount == 0) {
			APP_INF("Done processing %s", txtIn);
			break;
		}

		int numChars = 0;
		int offset = prepareXFont(strIn, strOut, xfontBuffIn, xfontOut,
			glyphBuffIn, glyphOut, &numChars);

		if (!offset) {
			break;
		}

		byteRead += offset;

		APP_DBG("Unicode display:");
		int sog;
		file2Buff(xfontOut, &sog, 32, 4, NULL);
		displayUnicode(strOut, xfontOut, glyphOut, 0, sog, offset);

		APP_DBG("Done 1 more loop, offset = %d\n", offset);
	}

	memfree(xfontBuffIn, 0);
	memfree(glyphBuffIn, 0);
}
