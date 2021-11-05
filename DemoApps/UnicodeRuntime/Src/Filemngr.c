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

#include <errno.h>

#include "Platform.h"
#include "Filemngr.h"
#include "Common.h"

// POSIX	FatFs                                           | App
// "r"	    FA_READ                                         | fl_read
// "r+"	    FA_READ          | FA_WRITE                     |
// "w"	    FA_CREATE_ALWAYS | FA_WRITE                     | fl_write
// "w+"	    FA_CREATE_ALWAYS | FA_WRITE | FA_READ           |
// "a"	    FA_OPEN_APPEND   | FA_WRITE                     | fl_append
// "a+"	    FA_OPEN_APPEND   | FA_WRITE | FA_READ           |
// "wx"	    FA_CREATE_NEW    | FA_WRITE                     |
// "w+x"	FA_CREATE_NEW    | FA_WRITE | FA_READ           |

static long int curPos = 0;
static long int filesz = 0;
static int isFOpenning = 0;
#if defined (ARDUINO_PLATFORM) || defined (FT900_PLATFORM) || defined (FT93X_PLATFORM)
FIL fp;
#else
static FILE *fp;
#endif

#if defined (ARDUINO_PLATFORM) || defined (FT900_PLATFORM) || defined (FT93X_PLATFORM)
/* input: path: Start node to be scanned (***also used as work area***) */
static FRESULT scan_files(char* path ) {
	FRESULT res;
	DIR dir;
	UINT i;
	static FILINFO fno;

	res = f_opendir(&dir, path); /* Open the directory */
	if (res == FR_OK) {
		for (;;) {
			res = f_readdir(&dir, &fno); /* Read a directory item */
			if (res != FR_OK || fno.fname[0] == 0)
				break; /* Break on error or end of dir */
			if (fno.fattrib & AM_DIR) { /* It is a directory */
				i = strlen(path);
				sprintf(&path[i], "/%s", fno.fname);
				res = scan_files(path); /* Enter the directory */
				if (res != FR_OK)
					break;
				path[i] = 0;
			} else { /* It is a file. */
				printf("%s/%s\n", path, fno.fname);
			}
		}
		f_closedir(&dir);
	}

	return res;
}
int fileClose() {
	if (isFOpenning) {
		f_close(&fp);
		memset(&fp, 0, sizeof(FIL));
		curPos = 0;
		filesz = 0;
		isFOpenning = 0;
	}
	return APP_OK;
}
int fileSeek(unsigned long offset) {
	FRESULT fResult;
	if (!isFOpenning) {
		APP_ERR("File haven't openned");
		return APP_ERROR;
	}
	fResult = f_lseek(&fp, offset);

	if (fResult != FR_OK) {
		APP_ERR("Seek error, error: %d", fResult);
		return APP_ERROR;
	}
	return APP_OK;
}
int fileOpen(const char *filePath, enum eFileMode e, int *size) {
	FRESULT fResult;
	BYTE mode;

	switch (e) {
	case fl_read:
		mode = FA_READ | FA_OPEN_EXISTING;
		break;
	case fl_write:
		mode = FA_CREATE_ALWAYS | FA_WRITE;
		break;
	case fl_append:
		mode = FA_WRITE;
		break;
	default:
		APP_ERR("File open mode is not recognized");
		return APP_ERROR;
	}
	fResult = f_open(&fp, filePath, mode);
	if (fResult != FR_OK) {
		APP_ERR("Cannot open %s, please check SD card, error: %d", filePath,
				fResult);
		return APP_ERROR;
	}

	filesz = f_size(&fp);

	isFOpenning = 1;

	if (mode == fl_append) {
		fileSeek(filesz);
	}

	if (size) {
		*size = (int) filesz;
	}
	APP_DBG("File size: %d bytes", *size);

	return APP_OK;
}
int fileRead(char* buffer, long bytes) {
	UINT bytesread = 0;
	UINT bytescount = 0;
	UINT bytescounts = 0;
	int chunk_size = 1024;
	FRESULT fResult;

	if (!isFOpenning) {
		APP_ERR("File haven't opened");
		return APP_ERROR;
	}
	if (curPos >= filesz) { // reached EOF, close the file
		APP_DBG("Reached EOF");
		return 0;
	}

	while (bytescounts < bytes) {
		chunk_size = bytes > chunk_size ? chunk_size : bytes;
		bytesread =
				(filesz - curPos) > chunk_size ? chunk_size : (filesz - curPos);

		fResult = f_read(&fp, &buffer[bytescounts], bytesread, &bytescount);
		if (fResult != FR_OK) {
			APP_ERR("Error on f_read\n");
			return APP_ERROR;
		}
		if (bytesread != bytescount) {
			APP_INF("Reached EOF, stop");
			bytescounts += bytescount;
			break;
		}
		curPos += bytescount;
		if (curPos >= filesz) { // reached EOF, break and return bytescounts
			APP_INF("Closed file");
			return bytescounts;
		}
		bytescounts += bytescount;
	}

	return bytescounts;
}
int fileWrite(char* buffer, long buffersize) {
	FRESULT fResult;

	UINT written = 0;

	if (!isFOpenning) {
		APP_ERR("File haven't openned");
		return APP_ERROR;
	}

	fResult = f_write(&fp, buffer, buffersize, &written);

	if (fResult != FR_OK) {
		APP_ERR("Error on f_write\n");
		return APP_ERROR;
	}
	if (buffersize != written) {
		APP_ERR("Writting error");
		fileClose();
		return APP_ERROR;
	}

	return APP_OK;
}
int fileList(char* path) {
	scan_files(path);
	return APP_OK;
}
#else
int fileClose() {
	if (isFOpenning) {
		fclose(fp);
		fp = NULL;
		curPos = 0;
		filesz = 0;
		isFOpenning = 0;
	}
	return APP_OK;
}
int fileSeek(unsigned long offset) {
	int ret = 0;

	APP_DBG("Seeking to %ld", offset);

	if (!isFOpenning) {
		APP_ERR("File haven't openned");
		return APP_ERROR;
	}

	ret = fseek(fp, offset, SEEK_SET);

	if (ret) {
		APP_ERR("Seek to %lu error, error: %d", offset, ret);
		return APP_ERROR;
	}
	return APP_OK;
}

int fileOpen(const char *filePath, enum eFileMode e, int *size){
	char mode[3];
	mode[1] = 'b';
	mode[2] = 0;

	switch (e) {
	case fl_read:
		mode[0] = 'r';
		break;
	case fl_write:
		mode[0] = 'w';
		break;
	case fl_append:
		mode[0] = 'a';
		break;
	default:
		APP_ERR("File open mode is not recognized");
		return APP_ERROR;
	}

	fp = fopen(filePath, mode);
	if (!fp) {
		APP_ERR("Cannot open %s, please check SD card, error: %d", filePath, errno);
		return APP_ERROR;
	}

	fseek(fp, 0, SEEK_END);
	filesz = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	isFOpenning = 1;

	if (size) {
		*size = (int)filesz;
	}

	return APP_OK;
}
// read number of bytes into buffer
int fileRead(char* buffer, long bytes) {
	UINT bytesread = 0;
	UINT bytescount = 0;
	UINT bytescounts = 0;
	int chunk_size = 1024;

	if (!fp || !isFOpenning) {
		APP_ERR("File is not opening");
		return APP_ERROR;
	}

	if (curPos >= filesz) { // reached EOF, close the file
		APP_DBG("Reached EOF");
		return 0;
	}

	if (chunk_size > bytes) {
		chunk_size = bytes;
	}
	
	while (bytescounts < bytes) {
		int remainBytes = bytes - bytescounts;

		if (remainBytes > chunk_size) {
			bytesread = chunk_size;
		} else {
			bytesread = remainBytes;
		}

		bytescount = fread(&buffer[bytescounts], 1, bytesread, fp);

		if (!feof(fp) && bytesread != bytescount) {
			APP_ERR("Error on f_read\n");
			return APP_ERROR;
		}

		if (bytesread != bytescount) {
			APP_INF("Reached EOF, stop");
			bytescounts += bytescount;
			break;
		}

		curPos += bytescount;

		if (curPos >= filesz) { // reached EOF, break and return bytescounts
			APP_INF("Closed file");
			return bytescounts;
		}
		bytescounts += bytescount;
	}
	return bytescounts;
}
int fileWrite(char* buffer, long buffersize) {
	UINT written = 0;

	if (!isFOpenning) {
		APP_ERR("File haven't openned");
		return APP_ERROR;
	}

	written = fwrite(buffer, 1, buffersize, fp);

	if (written != buffersize) {
		APP_ERR("Writting error: written=%u, Expected=%ld, errno=%d", written, buffersize, errno); 
		fileClose();
		return APP_ERROR;
	}

	return APP_OK;
}
int fileList(char* path) {
}
#endif

int file2Buff(char *txt, char *buff, long offset, int size,
		int *byteCount) {
	int fileSize = 0;
	int numRead = 0;

	fileOpen(txt, fl_read, &fileSize);
	numRead = fileSize - offset;

	if (offset >= fileSize) {
		return APP_ERROR;
	}

	if (APP_ERROR == fileSize) {
		return APP_ERROR;
	}

	if (size > 0 && size < numRead) {
		numRead = size;
	}

	// seek file pointer to offset
	fileSeek(offset);

	fileRead(buff, numRead);

	fileClose();

	if (byteCount) {
		*byteCount = numRead;
	}

	return APP_OK;
}
char* file2NewBuff(char *txt, unsigned long offset, int size,
		int *bufferSize) {
	int fileSize = 0;
	char* ret = 0;
	int numRead = 0;

	APP_INF("Reading file %s to buffer", txt);

	fileOpen(txt, fl_read, &fileSize);
	numRead = fileSize - offset;

	if (APP_ERROR == fileSize) {
		return NULL;
	}

	if (size > 0 && size < numRead) {
		numRead = size;
	}

	ret = (char*) memget(numRead);
	if (!ret) {
		APP_ERR("mm failed at %d", __LINE__);
		return APP_ERROR;
	}

	// seek file pointer to offset
	fileSeek(offset);

	fileRead(ret, numRead);

	fileClose();

	if (bufferSize) {
		*bufferSize = numRead;
	}

	return ret;
}
void buff2File(char *txt, char *buffer, unsigned long buffersize,
		enum eFileMode mode) {
	if (APP_ERROR == fileOpen(txt, mode, NULL)) {
		APP_ERR("Error open file");
		return;
	}
	fileWrite(buffer, buffersize);
	fileClose();
}
void fileDump(char *file) {
	char buff[1024];
	int bytecount;
	file2Buff(file, buff, 0, 1024, &bytecount);

	dumpMem(buff, bytecount);
}
