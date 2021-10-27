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

#ifndef FILEMNGR_H_
#define FILEMNGR_H_

#define APP_ERROR 0
#define APP_OK    1
#define TRUE    1
#define false    0

enum eFileMode {
	fl_read, fl_write, fl_append
};
enum eresult {
	ok, fail, end_of_file
};
typedef struct FILE_ {
	enum eresult result;
	char *buffer;
	int size;
} APPFILE;

int fileClose();
int fileSeek(unsigned long offset);
int fileOpen(const char *filePath, enum eFileMode e, int *size);
int fileRead(char* buffer, long bytes);
int fileWrite(char* buffer, long buffersize);
int fileList(char* path);

int file2Buff(char *txt, char *buff, long offset, int size,
		int *ByteCount);

char* file2NewBuff(char *txt, unsigned long offset, int size,
		int *bufferSize);
void buff2File(char *txt, char *buffer, unsigned long buffersize,
		enum eFileMode mode);
void fileDump(char *file);

#endif /* FILEMNGR_H_ */

