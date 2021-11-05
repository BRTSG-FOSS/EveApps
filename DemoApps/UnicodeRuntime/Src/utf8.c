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

#include "Utf8.h"
#include "Common.h"

// len is number of bytes of str
unsigned int utf8_next(unsigned char *str, int len, int offset, unsigned long *codePoint) {
	unsigned char *s = &str[offset];
	unsigned char *next = s;

	int remainBytes = len - offset;

	while (next == s && offset < len) {
		s = &str[offset];
		next = s;
		remainBytes = len - offset;

		if (s[0] < 0x80) { // 1 byte
			*codePoint = s[0];
			next = s + 1;
		} else if ((s[0] & 0xe0) == 0xc0) { // 2 bytes
			if (remainBytes < 2) { // the word is truncated
				return -1;
			}
			*codePoint = ((long)(s[0] & 0x1f) << 6) | ((long)(s[1] & 0x3f) << 0);
			next = s + 2;
		} else if ((s[0] & 0xf0) == 0xe0) { // 3 bytes
			if (remainBytes < 3) { // the word is truncated
				return -1;
			}
			*codePoint = ((long)(s[0] & 0x0f) << 12) | ((long)(s[1] & 0x3f) << 6)
				| ((long)(s[2] & 0x3f) << 0);
			next = s + 3;
		} else if ((s[0] & 0xf8) == 0xf0 && (s[0] <= 0xf4)) { // 4 bytes
			if (remainBytes < 4) { // the word is truncated
				return -1;
			}
			*codePoint = ((long)(s[0] & 0x07) << 18) | ((long)(s[1] & 0x3f) << 12)
				| ((long)(s[2] & 0x3f) << 6) | ((long)(s[3] & 0x3f) << 0);
			next = s + 4;
		} else { // invalid, skip this byte
			APP_DBG("Invalid byte, offset=%d, (int) (next - str): %d, s[0]: %02x", offset, (int)(next - str), s[0]);
			offset++;
		}
	}
	// Codepoints U+D800 - U+DFFF are reserved exclusively for use with UTF-16
	// https://stackoverflow.com/questions/40184882/why-were-the-code-points-in-the-range-of-ud800-to-udfff-removed-from-the-unico
	if (*codePoint >= 0xd800 && *codePoint <= 0xdfff) {
		*codePoint = -1; // surrogate half
	}

	if (offset == len) {
		*codePoint = -1;
		return -1; // invalid stream
	}
	return (int) (next - str);
}
static int insertCp(CODEPOINT *cp, int* numCp, unsigned long value, int bytes,
		int* index) {
	int i = 0;

	// check if cp already has
	for (i = 0; i < *numCp; i++) {
		if (cp[i].codepoint == value) {
			*index = i; // set index of present cp
			return APP_ERROR; // return error code
		}
	}

	// insert newly cp
	cp[*numCp].codepoint = value;
	cp[*numCp].bytes = bytes;

	*numCp = *numCp + 1;

	return APP_OK; // insert successfully
}
static char* int_2_UTF8(unsigned int value, int *bytes) {
	static char ret[4];

	memset(ret, 0, sizeof(ret));

	if (value <= 0x7f) {
		*bytes = 1;
		ret[0] = value;
	} else if (value <= 0x7ff) {
		*bytes = 2;
		ret[0] = (char)(0xc0 | ((value >> 6) & 0x1f));
		ret[1] = (char)(0x80 | (value & 0x3f));
	} else if (value <= 0xffff) {
		*bytes = 3;
		ret[0] = (char)(0xe0 | ((value >> 12) & 0x0f));
		ret[1] = (char)(0x80 | ((value >> 6) & 0x3f));
		ret[2] = (char)(0x80 | (value & 0x3f));
	} else {
		*bytes = 4;
		ret[0] = (char)(0xf0 | ((value >> 18) & 0x07));
		ret[1] = (char)(0x80 | ((value >> 12) & 0x3f));
		ret[2] = (char)(0x80 | ((value >> 6) & 0x3f));
		ret[3] = (char)(0x80 | (value & 0x3f));
	}
	return ret;
}
static int insertStrOut(unsigned char *str, int* index, unsigned int value) {
	int bytes = 0;
	unsigned char* code = int_2_UTF8(value, &bytes);

#if APPDEBUG
		printf("int: %d, code: ", value);
		for (int i = 0; i < bytes; i++) {
			printf("%02x ", code[i]);
		}
		printf("\n");
#endif

	memcpy(&str[*index], code, bytes);
	*index = *index + bytes;
}
// return number of bytes parsed
int getCodePointsOrdinal(unsigned char* strIn, char *strOut, int size, CODEPOINT* cp,
		int *numChars, int *numCp, int max_stream_word) {
/* stream:      A F B D A B E 1 E 2 1 ==> cp: 100 200 A00 B00 D00 E00 F00 => glyph address offset: z t c g m p r
 * new glyph cp:                                                                                   1 2 3 4 5 6 7
 * new stream:  3 7 4 5 3 4 6 1 6 2 1
 * convert map:
 * 	(nul)		\x0000
 *  space      	\x0001
 *  newline     \x000A
 *  others      others
 *  */
	int next = 0, last = 0, offset = 0;;
	int newCP = 1;
	CODEPOINT tcp; // temporary cp
	int index;
	int isMeetNullChar = 0;
	int strOutIndex = 0;

	*numCp = 0;
	*numChars = 0;
	// null
	insertCp(cp, numCp, UTF8_NULL, 1, &index);
	//space
	insertCp(cp, numCp, UTF8_SPACE, 1, &index);

	int fff = 0;
	while (!isMeetNullChar && next < size && next >= 0 && *numChars <= max_stream_word) {
		last = next;
		next = utf8_next(strIn, size, offset, &tcp.codepoint);
		
		if(-1 == tcp.codepoint){
			APP_INF("Got invalid codepoint, ignored");
			break;
		}
		if(-1 == next){
			APP_INF("Reach end of stream, offset = %d", offset);
			// return number of bytes parsed
			break;
		}
		// set next character position to be parsed
		offset = next;

		tcp.bytes = next - last;

		// ignore specials cp
		switch (tcp.codepoint) {
		case UTF8_NULL: // null
			isMeetNullChar = 1;
			break;
		case UTF8_SPACE: // space
			insertStrOut(strOut, &strOutIndex, CP_SPACE);
			*numChars = *numChars + 1;
			continue;
			break;
		case UTF8_NEWLINE: //newline
			insertStrOut(strOut, &strOutIndex, CP_NEWLINE);
			*numChars = *numChars + 1;
			continue;
			break;
		default:
			break;
		}

		if (*numCp == CP_NEWLINE) {
			insertCp(cp, numCp, CP_NEWLINE, 1, &index);
		}

		if (isMeetNullChar) {
			break;
		}

		if (APP_OK == insertCp(cp, numCp, tcp.codepoint, tcp.bytes, &index)) {
			newCP++;
			if (newCP == 0x0a) {
				newCP = 0x0b;
			}
			insertStrOut(strOut, &strOutIndex, newCP);
			*numChars = *numChars + 1;
		} else {
			insertStrOut(strOut, &strOutIndex, index);
			*numChars = *numChars + 1;
		}
	}

	insertStrOut(strOut, &strOutIndex, CP_NULL);
	*numChars = *numChars + 1;

	return offset;// return number of bytes parsed
}

int isEndByUtf8(unsigned char x) {
	if ((x >> 7) == 0 || (x >> 6) == 2) {
		return TRUE;
	}
	return FALSE;
}

