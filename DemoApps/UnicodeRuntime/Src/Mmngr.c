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
#include "Mmngr.h"
#include "Common.h"

static unsigned int memUsed = 0;
char* memget(int size) {
	char *ptr = (char*)malloc(size);
	if (!ptr) {
		APP_ERR("Malloc failed for %d bytes, total used = %u ", size, memUsed);
		return 0;
	}

	memUsed += size;
	return ptr;
}
int memfree(char *ptr, int size) {
	free(ptr);
	memUsed -= size;
	return APP_OK;
}
int dumpMem(char *start, int size) {
#if APPDEBUG
	const int bytesPerLine = 64;
	const int group = 1;
	int t = 0;
	char *c = (char*)start;
	unsigned int sum = 0;

	for (int i = 0; i < size;) {
		t = 0;
		for (int j = 0; j < (bytesPerLine) && i < size; j++) {
			unsigned char x = c[i];
			sum += x;
			printf("%02x", x);
			i++;
			t++;
			if (t == group) {
				t = 0;
				printf(" ");
			}
		}
		printf("\n");
	}
#endif
	return APP_OK;
}
