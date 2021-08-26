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

#ifndef XFONT_H_
#define XFONT_H_

#define APP_ERROR 0
#define APP_OK    1
#define TRUE    1
#define false    0

// Xfont structure
//    integer values
#define XF_SIGNATURE(xf)                ( (unsigned int)((int*)xf)[0]  )
#define XF_SIZE(xf)                     ( (unsigned int)((int*)xf)[1]  )
#define XF_FORMAT(xf)                   ( (unsigned int)((int*)xf)[2]  )
#define XF_SWIZZLE(xf)                  ( (unsigned int)((int*)xf)[3]  )
#define XF_LAYOUT_WIDTH(xf)             ( (unsigned int)((int*)xf)[4]  )
#define XF_LAYOUT_HEIGHT(xf)            ( (unsigned int)((int*)xf)[5]  )
#define XF_PIXEL_WIDTH(xf)              ( (unsigned int)((int*)xf)[6]  )
#define XF_PIXEL_HEIGHT(xf)             ( (unsigned int)((int*)xf)[7]  )
#define XF_START_OF_GRAPHIC_DATA(xf)    ( (unsigned int)((int*)xf)[8]  )
#define XF_NUMBER_OF_CHARACTERS(xf)     ( (unsigned int)((int*)xf)[9]  )
//    position values
#define XF_GPTR(xf)                     ( (unsigned int*)&(((int*)xf)[10]) )
#define XF_WPTR(xf)                     ( (unsigned int*) &(((char*)xf)[40 + 4 * (XF_NUMBER_OF_CHARACTERS(xf) / 128)]))
#define XF_WIDTH(xf)                    ( (unsigned char*)&(((char*)xf)[40 + 8 * (XF_NUMBER_OF_CHARACTERS(xf) / 128)]))

// filesize of new xfont
#define XF_FILESIZE(xf,zzz)        (((int)(*XF_WIDTH(xf))) + zzz)

typedef struct XFONT_EXTENDED_ {
	int signature;
	int size;
	int format;
	int swizzle;
	int layout_width;
	int layout_height;
	int pixel_width;
	int pixel_height;
	int start_of_Graphic_data;
	int number_of_characters;

	int gPtrOffset;
	int gPtrSize;

	int wPtrOffset;
	int wPtrSize;

	int widthdataOffset;
	int widthdataSize;
	int widthdataSizeActualInXFONT;

	int *wptr;
	int *gptr;
	unsigned char *width;
} XFONTEXTENDED;

int getGlyOffset(char* xf, unsigned long cp);
unsigned char getWidth(char *xf, unsigned long cp);
int parseXFONT(char *file, XFONTEXTENDED *xf);

#endif /* XFONT_H_ */

