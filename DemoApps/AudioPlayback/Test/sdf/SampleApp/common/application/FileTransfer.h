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

#ifndef FILETRANSFER_H_
#define FILETRANSFER_H_
#include <stdint.h>

#define FTF_PROGESS_READ  1
#define FTF_PROGESS_WRITE 2

typedef struct Ftf_Progress {
#if defined(BT8XXEMU_PLATFORM) || defined(MSVC_PLATFORM)
	char file[200];
	char fileName[200];
	char message[200];
#else
	char file[300];
	char fileName[100];
	char message[200];
#endif
	uint32_t fileSize;
	uint32_t sent;
	uint32_t bytesPerPercent;
	uint32_t addr;
	uint8_t direction;
}Ftf_Progress_t;

#if defined(EVE_FLASH_AVAILABLE)
uint32_t Ftf_Flash_Erase(EVE_HalContext *phost);
uint32_t Ftf_Flash_ClearCache(EVE_HalContext* phost);
uint32_t Ftf_Write_BlobFile(EVE_HalContext *phost, char* blobfile);
uint32_t Ftf_Write_Blob_Default(EVE_HalContext* phost);

Ftf_Progress_t* Ftf_Progress_Close(EVE_HalContext *phost);
Ftf_Progress_t* Ftf_Progress_Init(EVE_HalContext *phost, char *filePath, char *fileName, uint32_t addr, uint8_t direction);
uint32_t Ftf_Progress_Write_Next(EVE_HalContext *phost, Ftf_Progress_t *progress);
uint32_t Ftf_Progress_Read_Next(EVE_HalContext *phost, Ftf_Progress_t *progress);
uint32_t Ftf_Progress_Ui(EVE_HalContext *phost, Ftf_Progress_t *progress);
uint32_t Ftf_Write_File_To_Flash_With_Progressbar(EVE_HalContext *phost, char *filePath, char *fileName, uint32_t address);
uint32_t Ftf_Read_File_From_Flash_With_Progressbar(EVE_HalContext *phost, uint8_t *filePath, char *fileName, uint32_t address, uint32_t size);

uint32_t Ftf_Write_File_To_Flash_By_Cmd_Fifo(EVE_HalContext* phost, char* fileName, uint32_t addr, int isErase);
uint32_t Ftf_Write_FileArr_To_Flash_By_Cmd_Fifo(EVE_HalContext* phost, const char* file[], uint32_t addr);

uint32_t Ftf_Write_File_To_Flash_By_RAM_G(EVE_HalContext *phost, char *fileName, uint32_t addr);
uint32_t Ftf_Write_FileArr_To_Flash_By_RAM_G(EVE_HalContext *phost, const char *file[], uint32_t addr);

uint32_t Ftf_Read_File_From_Flash(EVE_HalContext *phost, uint8_t *output, uint32_t address, uint32_t size);

uint32_t Ftf_Flash_Get_Size(EVE_HalContext *phost);
#endif

uint32_t Ftf_Write_File_nBytes_To_RAM_G(EVE_HalContext *phost, const char *file, uint32_t addr, int nbytes, int offset);
uint32_t Ftf_Write_File_To_RAM_G(EVE_HalContext *phost, const char *file, uint32_t addr);
uint32_t Ftf_Write_FileArr_To_RAM_G(EVE_HalContext *phost, const char *file[], uint32_t addr);
uint32_t Ftf_Read_File_From_RAM_G(EVE_HalContext *phost, uint8_t *output, uint32_t startAddress, uint32_t size);

#endif /* FILETRANSFER_H_ */

