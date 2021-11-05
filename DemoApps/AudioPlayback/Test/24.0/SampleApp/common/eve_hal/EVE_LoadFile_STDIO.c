/**
* This source code ("the Software") is provided by Bridgetek Pte Ltd
* ("Bridgetek") subject to the licence terms set out
*   http://brtchip.com/BRTSourceCodeLicenseAgreement/ ("the Licence Terms").
* You must read the Licence Terms before downloading or using the Software.
* By installing or using the Software you agree to the Licence Terms. If you
* do not agree to the Licence Terms then do not download or use the Software.
*
* Without prejudice to the Licence Terms, here is a summary of some of the key
* terms of the Licence Terms (and in the event of any conflict between this
* summary and the Licence Terms then the text of the Licence Terms will
* prevail).
*
* The Software is provided "as is".
* There are no warranties (or similar) in relation to the quality of the
* Software. You use it at your own risk.
* The Software should not be used in, or for, any medical device, system or
* appliance. There are exclusions of Bridgetek liability for certain types of loss
* such as: special loss or damage; incidental loss or damage; indirect or
* consequential loss or damage; loss of income; loss of business; loss of
* profits; loss of revenue; loss of contracts; business interruption; loss of
* the use of money or anticipated savings; loss of information; loss of
* opportunity; loss of goodwill or reputation; and/or loss of, damage to or
* corruption of data.
* There is a monetary cap on Bridgetek's liability.
* The Software may have subsequently been amended by another user and then
* distributed by that other user ("Adapted Software").  If so that user may
* have additional licence terms that apply to those amendments. However, Bridgetek
* has no liability in relation to those amendments.
*/

#include "EVE_LoadFile.h"
#include "EVE_Platform.h"
#if !defined(FT9XX_PLATFORM) && !defined(RP2040_PLATFORM)

#include <stdio.h>

/**
 * @brief Mount the SDcard
 * 
 * @param phost  Pointer to Hal context
 * @return true True if ok
 * @return false False if error
 */
EVE_HAL_EXPORT bool EVE_Util_loadSdCard(EVE_HalContext *phost)
{
	/* no-op */
	return true;
}

EVE_HAL_EXPORT bool EVE_Util_sdCardReady(EVE_HalContext *phost)
{
	/* no-op */
	return true;
}

/**
 * @brief Load a raw file into RAM_G
 * 
 * @param phost  Pointer to Hal context
 * @param address Address in RAM_G
 * @param filename File to load
 * @return true True if ok
 * @return false False if error
 */
#ifdef _WIN32
static bool loadRawFile(EVE_HalContext *phost, uint32_t address, const char *filename, const wchar_t *filenameW)
#else
EVE_HAL_EXPORT bool EVE_Util_loadRawFile(EVE_HalContext *phost, uint32_t address, const char *filename)
#endif
{
	FILE *afile;
	uint32_t ftsize = 0;
	uint8_t pbuff[8192];
	uint16_t blocklen;
	uint32_t addr = address;

#pragma warning(push)
#pragma warning(disable : 4996)
#ifdef _WIN32
	afile = filename ? fopen(filename, "rb") : _wfopen(filenameW, L"rb");
#else
	afile = fopen(filename, "rb"); // read Binary (rb)
#endif
#pragma warning(pop)
	if (afile == NULL)
	{
		eve_printf_debug("Unable to open: %s\n", filename);
		return false;
	}
	fseek(afile, 0, SEEK_END);
	ftsize = ftell(afile);
	fseek(afile, 0, SEEK_SET);
	while (ftsize > 0)
	{
		blocklen = ftsize > 8192 ? 8192 : (uint16_t)ftsize;
		blocklen = (uint16_t)fread(pbuff, 1, blocklen, afile);
		ftsize -= blocklen;
		EVE_Hal_wrMem(phost, addr, pbuff, blocklen);
		addr += blocklen;
	}
	fclose(afile);

	return true;
}

#ifdef _WIN32

EVE_HAL_EXPORT bool EVE_Util_loadRawFile(EVE_HalContext *phost, uint32_t address, const char *filename)
{
	return loadRawFile(phost, address, filename, NULL);
}

EVE_HAL_EXPORT bool EVE_Util_loadRawFileW(EVE_HalContext *phost, uint32_t address, const wchar_t *filename)
{
	return loadRawFile(phost, address, NULL, filename);
}

#endif

/**
 * @brief Load file into RAM_G by CMD_INFLATE
 * 
 * @param phost  Pointer to Hal context
 * @param address Address to write
 * @param filename File to load
 * @return true True if ok
 * @return false False if error
 */
#ifdef _WIN32
static bool loadInflateFile(EVE_HalContext *phost, uint32_t address, const char *filename, const wchar_t *filenameW)
#else
EVE_HAL_EXPORT bool EVE_Util_loadInflateFile(EVE_HalContext *phost, uint32_t address, const char *filename)
#endif
{
	FILE *afile;
	uint32_t ftsize = 0;
	uint8_t pbuff[8192];
	uint16_t blocklen;

	if (!EVE_Cmd_waitSpace(phost, 8))
		return false; // Space for CMD_INFLATE

#pragma warning(push)
#pragma warning(disable : 4996)
#ifdef _WIN32
	afile = filename ? fopen(filename, "rb") : _wfopen(filenameW, L"rb");
#else
	afile = fopen(filename, "rb"); // read Binary (rb)
#endif
#pragma warning(pop)
	if (afile == NULL)
	{
		eve_printf_debug("Unable to open: %s\n", filename);
		return false;
	}
	EVE_Cmd_wr32(phost, CMD_INFLATE);
	EVE_Cmd_wr32(phost, address);
	fseek(afile, 0, SEEK_END);
	ftsize = ftell(afile);
	fseek(afile, 0, SEEK_SET);
	while (ftsize > 0)
	{
		blocklen = ftsize > 8192 ? 8192 : (uint16_t)ftsize;
		blocklen = (uint16_t)fread(pbuff, 1, blocklen, afile); /* copy the data into pbuff and then transfter it to command buffer */
		ftsize -= blocklen;
		blocklen += 3;
		blocklen &= ~3U;

		if (!EVE_Cmd_wrMem(phost, (char *)pbuff, blocklen)) /* copy data continuously into command memory */
			break;
	}

	fclose(afile); /* close the opened compressed file */

	return EVE_Cmd_waitFlush(phost);
}

#ifdef _WIN32

EVE_HAL_EXPORT bool EVE_Util_loadInflateFile(EVE_HalContext *phost, uint32_t address, const char *filename)
{
	return loadInflateFile(phost, address, filename, NULL);
}

EVE_HAL_EXPORT bool EVE_Util_loadInflateFileW(EVE_HalContext *phost, uint32_t address, const wchar_t *filename)
{
	return loadInflateFile(phost, address, NULL, filename);
}

#endif

/**
 * @brief Load image into RAM_G
 * 
 * @param phost  Pointer to Hal context
 * @param address Address in RAM_G
 * @param filename File to load
 * @param format Target format of image
 * @return true True if ok
 * @return false False if error
 */
#ifdef _WIN32
static bool loadImageFile(EVE_HalContext *phost, uint32_t address, const char *filename, const wchar_t *filenameW, uint32_t *format)
#else
EVE_HAL_EXPORT bool EVE_Util_loadImageFile(EVE_HalContext *phost, uint32_t address, const char *filename, uint32_t *format)
#endif
{
	FILE *afile;
	uint32_t ftsize = 0;
	uint8_t pbuff[8192];
	uint16_t blocklen;

	if (phost->CmdFault)
		return false;

#pragma warning(push)
#pragma warning(disable : 4996)
#ifdef _WIN32
	afile = filename ? fopen(filename, "rb") : _wfopen(filenameW, L"rb");
#else
	afile = fopen(filename, "rb"); // read Binary (rb)
#endif
#pragma warning(pop)
	if (afile == NULL)
	{
		eve_printf_debug("Unable to open: %s\n", filename);
		return 0;
	}
	EVE_Cmd_wr32(phost, CMD_LOADIMAGE);
	EVE_Cmd_wr32(phost, address);
	EVE_Cmd_wr32(phost, OPT_NODL);
	// TODO: Let it write into the scratch display list handle,
	//  and read it out and write into the bitmapInfo the proper
	//  values to use. Replace compressed bool with uint8 enum to
	//  specify the loading mechanism
	fseek(afile, 0, SEEK_END);
	ftsize = ftell(afile);
	fseek(afile, 0, SEEK_SET);
	while (ftsize > 0)
	{
		blocklen = ftsize > 8192 ? 8192 : (uint16_t)ftsize;
		blocklen = (uint16_t)fread(pbuff, 1, blocklen, afile); /* copy the data into pbuff and then transfter it to command buffer */
		ftsize -= blocklen;
		blocklen += 3;
		blocklen &= ~3U;

		if (!EVE_Cmd_wrMem(phost, (char *)pbuff, blocklen))
			break;
	}

	fclose(afile); /* close the opened jpg file */

	if (!EVE_Cmd_waitFlush(phost))
		return false;

	if (format)
		*format = EVE_Hal_rd32(phost, 0x3097e8);

	return true;
}

#ifdef _WIN32

EVE_HAL_EXPORT bool EVE_Util_loadImageFile(EVE_HalContext *phost, uint32_t address, const char *filename, uint32_t *format)
{
	return loadImageFile(phost, address, filename, NULL, format);
}

EVE_HAL_EXPORT bool EVE_Util_loadImageFileW(EVE_HalContext *phost, uint32_t address, const wchar_t *filename, uint32_t *format)
{
	return loadImageFile(phost, address, NULL, filename, format);
}

#endif

#ifdef _WIN32
static bool loadCmdFile(EVE_HalContext *phost, const char *filename, const wchar_t *filenameW, uint32_t *transfered)
#else
EVE_HAL_EXPORT bool EVE_Util_loadCmdFile(EVE_HalContext *phost, const char *filename, uint32_t *transfered)
#endif
{
	FILE *afile;
	uint32_t ftsize = 0;
	uint8_t pbuff[8192];
	uint16_t blocklen;

#pragma warning(push)
#pragma warning(disable : 4996)
#ifdef _WIN32
	afile = filename ? fopen(filename, "rb") : _wfopen(filenameW, L"rb");
#else
	afile = fopen(filename, "rb"); // read Binary (rb)
#endif
#pragma warning(pop)
	if (afile == NULL)
	{
		eve_printf_debug("Unable to open: %s\n", filename);
		return false;
	}
	fseek(afile, 0, SEEK_END);
	ftsize = ftell(afile);
	fseek(afile, 0, SEEK_SET);
	while (ftsize > 0)
	{
		blocklen = ftsize > 8192 ? 8192 : (uint16_t)ftsize;
		blocklen = (uint16_t)fread(pbuff, 1, blocklen, afile); /* copy the data into pbuff and then transfter it to command buffer */
		ftsize -= blocklen;
		blocklen += 3;
		blocklen &= ~3U;
		if (!EVE_Cmd_wrMem(phost, (char *)pbuff, blocklen)) /* copy data continuously into command memory */
			break;
		if (transfered)
			*transfered += blocklen;
	}

	fclose(afile); /* close the opened file */

	return EVE_Cmd_waitFlush(phost);
}

#ifdef _WIN32

EVE_HAL_EXPORT bool EVE_Util_loadCmdFile(EVE_HalContext *phost, const char *filename, uint32_t *transfered)
{
	return loadCmdFile(phost, filename, NULL, transfered);
}

EVE_HAL_EXPORT bool EVE_Util_loadCmdFileW(EVE_HalContext *phost, const wchar_t *filename, uint32_t *transfered)
{
	return loadCmdFile(phost, NULL, filename, transfered);
}

#endif

#if (EVE_SUPPORT_CHIPID >= EVE_FT810)
#ifdef _WIN32
static bool loadMediaFile(EVE_HalContext *phost, const char *filename, const wchar_t *filenameW, uint32_t *transfered)
#else
EVE_HAL_EXPORT bool EVE_Util_loadMediaFile(EVE_HalContext *phost, const char *filename, uint32_t *transfered)
#endif
{
	FILE *afile;
	uint32_t remaining = 0;
	uint32_t blockSize = ((phost->MediaFifoSize >> 3) << 2) - 4;
#pragma warning(push)
#pragma warning(disable : 6255)
	uint8_t *pbuff = (uint8_t *)_alloca(blockSize);
#pragma warning(pop)
	uint16_t blocklen;
	if (!transfered)
		EVE_Util_closeFile(phost);
	if (phost->CmdFault)
		return false;
#pragma warning(push)
#pragma warning(disable : 4996)
#ifdef _WIN32
	if (phost->LoadFileHandle)
	{
		afile = phost->LoadFileHandle;
	}
	else if (filename)
	{
		afile = fopen(filename, "rb");
	}
	else
	{
		afile = _wfopen(filenameW, L"rb");
	}
#else
	afile = phost->LoadFileHandle ? phost->LoadFileHandle : fopen(filename, "rb"); // read Binary (rb)
#endif
#pragma warning(pop)
	if (afile == NULL)
	{
		eve_printf_debug("Unable to open: %s\n", filename);
		return false;
	}
	if (!phost->LoadFileHandle)
	{
		fseek(afile, 0, SEEK_END);
		remaining = ftell(afile);
		fseek(afile, transfered ? *transfered : 0, SEEK_SET);
		if (transfered)
			phost->LoadFileHandle = afile;
	}
	else
	{
		remaining = (uint32_t)phost->LoadFileRemaining;
	}
	while (remaining > 0)
	{
		blocklen = (uint16_t)min(blockSize, remaining);
		blocklen = (uint16_t)fread((void *)pbuff, 1, blocklen, afile); /* Copy the data into pbuff and then transfter it to command buffer */

		if (blocklen == 0)
		{
			eve_printf_debug("Read 0 bytes, unexpected end of file, %i bytes remaining", (int)remaining);
			break;
		}

		remaining -= blocklen;
		blocklen += 3;
		blocklen &= ~3U;

		if (transfered)
		{
			uint32_t transferedPart = 0;
			bool wrRes = EVE_MediaFifo_wrMem(phost, pbuff, blocklen, &transferedPart); /* Copy data continuously into media fifo memory */
			*transfered += transferedPart;
			if (transferedPart < blocklen)
			{
				long offset = (long)transferedPart - (long)blocklen; /* Negative */
				fseek(afile, offset, SEEK_CUR); /* Seek back */
				remaining -= offset; /* Increments remaining (double negative) */
				break; /* Early exit, processing done */
			}
			if (!wrRes)
				break;
		}
		else
		{
			if (!EVE_MediaFifo_wrMem(phost, pbuff, blocklen, NULL)) /* Copy data continuously into media fifo memory */
				break; /* Coprocessor fault */
		}
	}
	if (!transfered)
	{
		fclose(afile); /* Close the opened file */
	}
	else if (remaining)
	{
		phost->LoadFileRemaining = remaining; /* Save remaining */
	}
	else
	{
		EVE_Util_closeFile(phost);
	}
	return transfered ? EVE_Cmd_waitFlush(phost) : EVE_MediaFifo_waitFlush(phost, false);
}

void EVE_Util_closeFile(EVE_HalContext *phost)
{
	if (phost->LoadFileHandle)
	{
		fclose(phost->LoadFileHandle);
		phost->LoadFileHandle = NULL;
		phost->LoadFileRemaining = 0;
	}
}

#ifdef _WIN32

EVE_HAL_EXPORT bool EVE_Util_loadMediaFile(EVE_HalContext *phost, const char *filename, uint32_t *transfered)
{
	return loadMediaFile(phost, filename, NULL, transfered);
}

EVE_HAL_EXPORT bool EVE_Util_loadMediaFileW(EVE_HalContext *phost, const wchar_t *filename, uint32_t *transfered)
{
	return loadMediaFile(phost, NULL, filename, transfered);
}

#endif
#endif

#endif

/* end of file */
