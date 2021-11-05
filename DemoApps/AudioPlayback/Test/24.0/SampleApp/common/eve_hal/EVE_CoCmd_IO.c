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

#include "EVE_Platform.h"

EVE_HAL_EXPORT bool EVE_CoCmd_memCrc(EVE_HalContext *phost, uint32_t ptr, uint32_t num, uint32_t *result)
{
	uint16_t resAddr;

#if EVE_CMD_HOOKS
	if (phost->CoCmdHook && phost->CoCmdHook(phost, CMD_MEMCRC, 0))
		return false;
#endif

	EVE_Cmd_startFunc(phost);
	EVE_Cmd_wr32(phost, CMD_MEMCRC);
	EVE_Cmd_wr32(phost, ptr);
	EVE_Cmd_wr32(phost, num);
	resAddr = EVE_Cmd_moveWp(phost, 4);
	EVE_Cmd_endFunc(phost);

	/* Read result */
	if (result)
	{
		if (!EVE_Cmd_waitFlush(phost))
			return false;
		*result = EVE_Hal_rd32(phost, RAM_CMD + resAddr);
	}
	return true;
}

EVE_HAL_EXPORT bool EVE_CoCmd_regRead(EVE_HalContext *phost, uint32_t ptr, uint32_t *result)
{
	uint16_t resAddr;

#if EVE_CMD_HOOKS
	if (phost->CoCmdHook && phost->CoCmdHook(phost, CMD_REGREAD, 0))
		return false;
#endif

	EVE_Cmd_startFunc(phost);
	EVE_Cmd_wr32(phost, CMD_REGREAD);
	EVE_Cmd_wr32(phost, ptr);
	resAddr = EVE_Cmd_moveWp(phost, 4);
	EVE_Cmd_endFunc(phost);

	/* Read result */
	if (result)
	{
		if (!EVE_Cmd_waitFlush(phost))
			return false;
		*result = EVE_Hal_rd32(phost, RAM_CMD + resAddr);
	}
	return true;
}

EVE_HAL_EXPORT bool EVE_CoCmd_inflate_progMem(EVE_HalContext *phost, uint32_t dst, eve_progmem_const uint8_t *src, uint32_t size)
{
	if (!EVE_Cmd_waitFlush(phost))
		return false; // Coprocessor must be ready
	EVE_Cmd_startFunc(phost);
	EVE_Cmd_wr32(phost, CMD_INFLATE);
	EVE_Cmd_wr32(phost, dst);
	EVE_Cmd_wrProgMem(phost, src, (size + 3) & ~0x3UL);
	EVE_Cmd_endFunc(phost);
	return EVE_Cmd_waitFlush(phost); // Resource failed to load
}

/* Get the end memory address of data inflated by CMD_INFLATE */
EVE_HAL_EXPORT bool EVE_CoCmd_getPtr(EVE_HalContext *phost, uint32_t *result)
{
	uint16_t resAddr;

#if EVE_CMD_HOOKS
	if (phost->CoCmdHook && phost->CoCmdHook(phost, CMD_GETPTR, 0))
		return false;
#endif

	EVE_Cmd_startFunc(phost);
	EVE_Cmd_wr32(phost, CMD_GETPTR);
	resAddr = EVE_Cmd_moveWp(phost, 12);
	EVE_Cmd_endFunc(phost);

	/* Read result */
	if (result)
	{
		if (!EVE_Cmd_waitFlush(phost))
			return false;
		*result = EVE_Hal_rd32(phost, RAM_CMD + resAddr);
	}
	return true;
}

EVE_HAL_EXPORT bool EVE_CoCmd_loadImage_progMem(EVE_HalContext *phost, uint32_t dst, eve_progmem_const uint8_t *src, uint32_t size, uint32_t *format)
{
#if EVE_CMD_HOOKS
	if (phost->CoCmdHook && phost->CoCmdHook(phost, CMD_LOADIMAGE, dst))
		return false;
#endif

	if (!EVE_Cmd_waitFlush(phost))
		return false; // Coprocessor must be ready
	EVE_Cmd_startFunc(phost);
	EVE_Cmd_wr32(phost, CMD_LOADIMAGE);
	EVE_Cmd_wr32(phost, dst);
	EVE_Cmd_wr32(phost, OPT_NODL);
	EVE_Cmd_wrProgMem(phost, src, (size + 3) & ~0x3UL);
	EVE_Cmd_endFunc(phost);
	if (!EVE_Cmd_waitFlush(phost))
		return false; // Image failed to load
	if (format)
		*format = EVE_Hal_rd32(phost, 0x3097e8);
	return true;
}

EVE_HAL_EXPORT bool EVE_CoCmd_getProps(EVE_HalContext *phost, uint32_t *ptr, uint32_t *w, uint32_t *h)
{
	uint16_t resAddr;

#if EVE_CMD_HOOKS
	if (phost->CoCmdHook && phost->CoCmdHook(phost, CMD_GETPROPS, 0))
		return false;
#endif

	EVE_Cmd_startFunc(phost);
	EVE_Cmd_wr32(phost, CMD_GETPROPS);
	resAddr = EVE_Cmd_moveWp(phost, 12);
	EVE_Cmd_endFunc(phost);

	/* Read result */
	if (ptr || w || h)
	{
		if (!EVE_Cmd_waitFlush(phost))
			return false;
		if (ptr)
			*ptr = EVE_Hal_rd32(phost, RAM_CMD + resAddr);
		if (w)
			*w = EVE_Hal_rd32(phost, RAM_CMD + resAddr + 4);
		if (h)
			*h = EVE_Hal_rd32(phost, RAM_CMD + resAddr + 8);
	}
	return true;
}

#if (EVE_SUPPORT_CHIPID >= EVE_BT815) && defined(EVE_FLASH_AVAILABLE)

EVE_HAL_EXPORT bool EVE_CoCmd_flashErase_flush(EVE_HalContext *phost)
{
	EVE_MULTI_TARGET_CHECK_RETURN(CMD_FLASHERASE, EVE_CHIPID >= EVE_BT815, false);

	if (!EVE_Cmd_waitFlush(phost))
		return false; // Coprocessor must be ready

	EVE_CoCmd_d(phost, CMD_FLASHERASE);

	return EVE_Cmd_waitFlush(phost);
}

EVE_HAL_EXPORT bool EVE_CoCmd_flashRead_flush(EVE_HalContext *phost, uint32_t dest, uint32_t src, uint32_t num)
{
	EVE_MULTI_TARGET_CHECK_RETURN(CMD_FLASHREAD, EVE_CHIPID >= EVE_BT815, false);

	if (!EVE_Cmd_waitFlush(phost))
		return false; // Coprocessor must be ready

	EVE_CoCmd_dddd(phost, CMD_FLASHREAD, dest, src, num);

	return EVE_Cmd_waitFlush(phost);
}

/// Attach flash. Return new FLASH_STATUS
EVE_HAL_EXPORT uint32_t EVE_CoCmd_flashAttach(EVE_HalContext *phost)
{
	uint32_t flashStatus;
	EVE_MULTI_TARGET_CHECK_RETURN(CMD_FLASHATTACH, EVE_CHIPID >= EVE_BT815, 0);
	if (!EVE_Cmd_waitFlush(phost))
		return EVE_Hal_rd32(phost, REG_FLASH_STATUS); // Coprocessor must be ready
	flashStatus = EVE_Hal_rd32(phost, REG_FLASH_STATUS);
	if (flashStatus != FLASH_STATUS_DETACHED)
		return flashStatus; // Only attach when detached
	EVE_CoCmd_d(phost, CMD_FLASHATTACH);
	EVE_Cmd_waitFlush(phost); // Wait for command completion
	return EVE_Hal_rd32(phost, REG_FLASH_STATUS); // Return current status
}

/// Enter fast flash state. Return new FLASH_STATUS
EVE_HAL_EXPORT uint32_t EVE_CoCmd_flashFast(EVE_HalContext *phost, uint32_t *result)
{
	uint16_t resAddr;
	uint32_t flashStatus;

	EVE_MULTI_TARGET_CHECK_RETURN(CMD_FLASHFAST, EVE_CHIPID >= EVE_BT815, 0);

	if (!EVE_Cmd_waitFlush(phost))
	{
		if (result)
			*result = 0xE000;
		return EVE_Hal_rd32(phost, REG_FLASH_STATUS);
	} // Coprocessor must be ready
	flashStatus = EVE_Hal_rd32(phost, REG_FLASH_STATUS);
	if (flashStatus < FLASH_STATUS_BASIC)
	{
		if (result)
			*result = 0xE001;
		return flashStatus;
	}
	if (flashStatus > FLASH_STATUS_BASIC)
	{
		if (result)
			*result = 0;
		return flashStatus;
	} // Only enter fast mode when attached

#if EVE_CMD_HOOKS
	if (phost->CoCmdHook && phost->CoCmdHook(phost, CMD_FLASHFAST, 0))
		return flashStatus;
#endif

	EVE_Cmd_startFunc(phost);
	EVE_Cmd_wr32(phost, CMD_FLASHFAST);
	resAddr = EVE_Cmd_moveWp(phost, 4); // Get the address where the coprocessor will write the result
	EVE_Cmd_endFunc(phost);
	EVE_Cmd_waitFlush(phost); // Wait for command completion
	if (result)
		*result = EVE_Hal_rd32(phost, RAM_CMD + resAddr); // Fetch result
	return EVE_Hal_rd32(phost, REG_FLASH_STATUS); // Return current status
}

EVE_HAL_EXPORT bool EVE_CoCmd_loadImage_flash(EVE_HalContext *phost, uint32_t dst, uint32_t src, uint32_t *format)
{
#if EVE_CMD_HOOKS
	if (phost->CoCmdHook && phost->CoCmdHook(phost, CMD_FLASHSOURCE, src))
		return false;
	if (phost->CoCmdHook && phost->CoCmdHook(phost, CMD_LOADIMAGE, dst))
		return false;
#endif

	if (!EVE_Cmd_waitFlush(phost))
		return false; // Coprocessor must be ready
	EVE_Cmd_startFunc(phost);
	EVE_Cmd_wr32(phost, CMD_FLASHSOURCE);
	EVE_Cmd_wr32(phost, src);
	EVE_Cmd_wr32(phost, CMD_LOADIMAGE);
	EVE_Cmd_wr32(phost, dst);
	EVE_Cmd_wr32(phost, OPT_FLASH | OPT_NODL);
	EVE_Cmd_endFunc(phost);
	if (!EVE_Cmd_waitFlush(phost))
		return false; // Image failed to load
	if (format)
		*format = EVE_Hal_rd32(phost, 0x3097e8);
	return true;
}

EVE_HAL_EXPORT bool EVE_CoCmd_inflate_flash(EVE_HalContext *phost, uint32_t dst, uint32_t src)
{
#if EVE_CMD_HOOKS
	if (phost->CoCmdHook && phost->CoCmdHook(phost, CMD_FLASHSOURCE, src))
		return false;
	if (phost->CoCmdHook && phost->CoCmdHook(phost, CMD_INFLATE2, dst))
		return false;
#endif

	if (!EVE_Cmd_waitFlush(phost))
		return false; // Coprocessor must be ready
	EVE_Cmd_startFunc(phost);
	EVE_Cmd_wr32(phost, CMD_FLASHSOURCE);
	EVE_Cmd_wr32(phost, src);
	EVE_Cmd_wr32(phost, CMD_INFLATE2);
	EVE_Cmd_wr32(phost, dst);
	EVE_Cmd_wr32(phost, OPT_FLASH);
	EVE_Cmd_endFunc(phost);
	return EVE_Cmd_waitFlush(phost); // Image failed to load
}

#endif

#if (EVE_SUPPORT_CHIPID >= EVE_BT817)

EVE_HAL_EXPORT bool EVE_CoCmd_getImage(EVE_HalContext *phost, uint32_t *source, uint32_t *fmt, uint32_t *w, uint32_t *h, uint32_t *palette)
{
	uint16_t resAddr;

	EVE_MULTI_TARGET_CHECK_RETURN(CMD_GETIMAGE, EVE_CHIPID >= EVE_BT817, false);

#if EVE_CMD_HOOKS
	if (phost->CoCmdHook && phost->CoCmdHook(phost, CMD_GETIMAGE, 0))
		return false;
#endif

	EVE_Cmd_startFunc(phost);
	EVE_Cmd_wr32(phost, CMD_GETIMAGE);
	resAddr = EVE_Cmd_moveWp(phost, 4 * 5);
	EVE_Cmd_endFunc(phost);

	/* Read result */
	if (source || fmt || w || h || palette)
	{
		if (!EVE_Cmd_waitFlush(phost))
			return false;
		if (source)
			*source = EVE_Hal_rd32(phost, RAM_CMD + resAddr);
		if (fmt)
			*fmt = EVE_Hal_rd32(phost, RAM_CMD + resAddr + 4);
		if (w)
			*w = EVE_Hal_rd32(phost, RAM_CMD + resAddr + 8);
		if (h)
			*h = EVE_Hal_rd32(phost, RAM_CMD + resAddr + 12);
		if (palette)
			*palette = EVE_Hal_rd32(phost, RAM_CMD + resAddr + 16);
	}
	return true;
}

EVE_HAL_EXPORT bool EVE_CoCmd_fontCacheQuery(EVE_HalContext *phost, uint32_t *total, int32_t *used)
{
	uint16_t resAddr;

	EVE_MULTI_TARGET_CHECK_RETURN(CMD_FONTCACHEQUERY, EVE_CHIPID >= EVE_BT817, false);

#if EVE_CMD_HOOKS
	if (phost->CoCmdHook && phost->CoCmdHook(phost, CMD_FONTCACHEQUERY, 0))
		return false;
#endif

	EVE_Cmd_startFunc(phost);
	EVE_Cmd_wr32(phost, CMD_FONTCACHEQUERY);
	resAddr = EVE_Cmd_moveWp(phost, 8);
	EVE_Cmd_endFunc(phost);

	/* Read result */
	if (total || used)
	{
		if (!EVE_Cmd_waitFlush(phost))
			return false;
		if (total)
			*total = EVE_Hal_rd32(phost, RAM_CMD + resAddr);
		if (used)
			*used = EVE_Hal_rd32(phost, RAM_CMD + resAddr + 4);
	}
	return true;
}

#endif

/* end of file */
