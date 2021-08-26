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

#ifndef FILEIO_H_
#define FILEIO_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum _FILEIO_E_FOPEN {
	FILEIO_E_FOPEN_READ, FILEIO_E_FOPEN_WRITE, FILEIO_E_FOPEN_APPEND
};
enum _FILEIO_E_FRESULT{
	FILEIO_E_FRESULT_OK, FILEIO_E_FRESULT_FAIL, FILEIO_E_FRESULT_EOF
};
#define FileIO_Append(M, ...)                       \
{                                                   \
	int num = sprintf(msg, M, ##__VA_ARGS__);       \
	FileIO_File_Write(msg, num);                    \
}

int FileIO_File_Close();
int FileIO_File_Seek(unsigned long offset);
int FileIO_File_Open(const char *filePath, enum _FILEIO_E_FOPEN e);
int FileIO_File_Read(char* buffer, long bytes);
int FileIO_File_Write(const char* buffer, long buffersize);
int FileIO_File_List(char* path);

int FileIO_File_To_Buffer(const char *file, char *buff, long offset, int size,
		int *ByteCount);
void FileIO_Buffer_To_File(const char *txt, const char *buffer, unsigned long buffersize);
char *FileIO_Read_Line();

#endif /* FILEIO_H_ */

