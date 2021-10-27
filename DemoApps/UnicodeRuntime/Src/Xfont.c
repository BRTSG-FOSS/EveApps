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
#include "Xfont.h"
#include "Common.h"

int getGlyOffset(char* xf, unsigned long cp) {
	int noc = XF_NUMBER_OF_CHARACTERS(xf); // number_of_characters
	unsigned int* gPtr = XF_GPTR(xf);
	uint32_t bytes_per_glyph = XF_LAYOUT_WIDTH(xf) * XF_LAYOUT_HEIGHT(xf);
	int ret =  gPtr[cp / 128] + bytes_per_glyph * (cp % 128);

	return ret;
}
unsigned char getWidth(char *xf, unsigned long cp) {
	unsigned int* wPtr = XF_WPTR(xf);
	unsigned int offset = wPtr[cp / 128] + (cp % 128);
	char *widthdata = xf;

	return (unsigned char) widthdata[offset];
}

int parseXFONT(char *fileBuffer, XFONTEXTENDED *xf) {
	int *header = (int*)fileBuffer;
	int offset = 0;

	xf = (XFONTEXTENDED*)fileBuffer;

	xf->signature = header[offset++];
	xf->size = header[offset++];
	xf->format = header[offset++];
	xf->swizzle = header[offset++];
	xf->layout_width = header[offset++];
	xf->layout_height = header[offset++];
	xf->pixel_width = header[offset++];
	xf->pixel_height = header[offset++];
	xf->start_of_Graphic_data = header[offset++];
	xf->number_of_characters = header[offset++];

	xf->gPtrOffset = offset * 4;
	xf->gPtrSize = 4 * (xf->number_of_characters / 128);

	xf->wPtrOffset = xf->gPtrOffset + xf->gPtrSize;
	xf->wPtrSize = 4 * (xf->number_of_characters / 128);

	xf->widthdataOffset = xf->wPtrOffset + xf->wPtrSize;
	xf->widthdataSize = xf->number_of_characters;

	APP_DBG("Getting gptr data from offset %d, %d bytes", xf->gPtrOffset, xf->gPtrSize);
	xf->gptr = (int*)&fileBuffer[xf->gPtrOffset];

	APP_DBG("Getting wptr data from offset %d, %d bytes", xf->wPtrOffset, xf->wPtrSize);
	xf->wptr = (int*)&fileBuffer[xf->wPtrOffset];

	APP_DBG("widthdataOffset: %d", xf->widthdataOffset);
	xf->width = (int*)&fileBuffer[xf->widthdataOffset];

	return APP_OK;
}
