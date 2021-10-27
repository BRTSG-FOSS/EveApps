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

/**********************************************************************
***********************************************************************
**********************************************************************/

EVE_HAL_EXPORT void EVE_CoCmd_d(EVE_HalContext *phost, uint32_t cmd)
{
#if EVE_CMD_HOOKS
	if (phost->CoCmdHook && phost->CoCmdHook(phost, cmd, 0))
		return;
#endif

	EVE_Cmd_wr32(phost, cmd);
}

EVE_HAL_EXPORT void EVE_CoCmd_dd(EVE_HalContext *phost, uint32_t cmd, uint32_t d0)
{
#if EVE_CMD_HOOKS
	if (phost->CoCmdHook && phost->CoCmdHook(phost, cmd, d0))
		return;
#endif

	EVE_Cmd_startFunc(phost);
	EVE_Cmd_wr32(phost, cmd);
	EVE_Cmd_wr32(phost, d0);
	EVE_Cmd_endFunc(phost);
}

EVE_HAL_EXPORT void EVE_CoCmd_ddd(EVE_HalContext *phost, uint32_t cmd, uint32_t d0, uint32_t d1)
{
#if EVE_CMD_HOOKS
	if (phost->CoCmdHook && phost->CoCmdHook(phost, cmd, d0))
		return;
#endif

	EVE_Cmd_startFunc(phost);
	EVE_Cmd_wr32(phost, cmd);
	EVE_Cmd_wr32(phost, d0);
	EVE_Cmd_wr32(phost, d1);
	EVE_Cmd_endFunc(phost);
}

EVE_HAL_EXPORT void EVE_CoCmd_dddd(EVE_HalContext *phost, uint32_t cmd, uint32_t d0, uint32_t d1, uint32_t d2)
{
#if EVE_CMD_HOOKS
	if (phost->CoCmdHook && phost->CoCmdHook(phost, cmd, d0))
		return;
#endif

	EVE_Cmd_startFunc(phost);
	EVE_Cmd_wr32(phost, cmd);
	EVE_Cmd_wr32(phost, d0);
	EVE_Cmd_wr32(phost, d1);
	EVE_Cmd_wr32(phost, d2);
	EVE_Cmd_endFunc(phost);
}

EVE_HAL_EXPORT void EVE_CoCmd_ddddd(EVE_HalContext *phost, uint32_t cmd, uint32_t d0, uint32_t d1, uint32_t d2, uint32_t d3)
{
#if EVE_CMD_HOOKS
	if (phost->CoCmdHook && phost->CoCmdHook(phost, cmd, d0))
		return;
#endif

	EVE_Cmd_startFunc(phost);
	EVE_Cmd_wr32(phost, cmd);
	EVE_Cmd_wr32(phost, d0);
	EVE_Cmd_wr32(phost, d1);
	EVE_Cmd_wr32(phost, d2);
	EVE_Cmd_wr32(phost, d3);
	EVE_Cmd_endFunc(phost);
}

EVE_HAL_EXPORT void EVE_CoCmd_ddww(EVE_HalContext *phost, uint32_t cmd,
    uint32_t d0, uint16_t w1, uint16_t w2)
{
#if EVE_CMD_HOOKS
	if (phost->CoCmdHook && phost->CoCmdHook(phost, cmd, d0))
		return;
#endif

	EVE_Cmd_startFunc(phost);
	EVE_Cmd_wr32(phost, cmd);
	EVE_Cmd_wr32(phost, d0);
	EVE_Cmd_wr16(phost, w1);
	EVE_Cmd_wr16(phost, w2);
	EVE_Cmd_endFunc(phost);
}

EVE_HAL_EXPORT void EVE_CoCmd_ddwww(EVE_HalContext *phost, uint32_t cmd,
    uint32_t d0, uint16_t w1, uint16_t w2, uint16_t w3)
{
#if EVE_CMD_HOOKS
	if (phost->CoCmdHook && phost->CoCmdHook(phost, cmd, d0))
		return;
#endif

	EVE_Cmd_startFunc(phost);
	EVE_Cmd_wr32(phost, cmd);
	EVE_Cmd_wr32(phost, d0);
	EVE_Cmd_wr16(phost, w1);
	EVE_Cmd_wr16(phost, w2);
	EVE_Cmd_wr16(phost, w3);
	EVE_Cmd_wr16(phost, 0);
	EVE_Cmd_endFunc(phost);
}

EVE_HAL_EXPORT void EVE_CoCmd_dddwwww(EVE_HalContext *phost, uint32_t cmd,
    uint32_t d0, uint32_t d1,
    uint16_t w2, uint16_t w3, uint16_t w4, uint16_t w5)
{
#if EVE_CMD_HOOKS
	if (phost->CoCmdHook && phost->CoCmdHook(phost, cmd, d0))
		return;
#endif

	EVE_Cmd_startFunc(phost);
	EVE_Cmd_wr32(phost, cmd);
	EVE_Cmd_wr32(phost, d0);
	EVE_Cmd_wr32(phost, d1);
	EVE_Cmd_wr16(phost, w2);
	EVE_Cmd_wr16(phost, w3);
	EVE_Cmd_wr16(phost, w4);
	EVE_Cmd_wr16(phost, w5);
	EVE_Cmd_endFunc(phost);
}

EVE_HAL_EXPORT void EVE_CoCmd_dwwdd(EVE_HalContext *phost, uint32_t cmd,
    uint16_t w0, uint16_t w1, uint32_t d2, uint32_t d3)
{
#if EVE_CMD_HOOKS
	if (phost->CoCmdHook && phost->CoCmdHook(phost, cmd, 0))
		return;
#endif

	EVE_Cmd_startFunc(phost);
	EVE_Cmd_wr32(phost, cmd);
	EVE_Cmd_wr16(phost, w0);
	EVE_Cmd_wr16(phost, w1);
	EVE_Cmd_wr32(phost, d2);
	EVE_Cmd_wr32(phost, d3);
	EVE_Cmd_endFunc(phost);
}

EVE_HAL_EXPORT void EVE_CoCmd_dwwdwwd(EVE_HalContext *phost, uint32_t cmd,
    uint16_t w0, uint16_t w1, uint32_t d2,
    uint16_t w3, uint16_t w4, uint32_t d5)
{
#if EVE_CMD_HOOKS
	if (phost->CoCmdHook && phost->CoCmdHook(phost, cmd, 0))
		return;
#endif

	EVE_Cmd_startFunc(phost);
	EVE_Cmd_wr32(phost, cmd);
	EVE_Cmd_wr16(phost, w0);
	EVE_Cmd_wr16(phost, w1);
	EVE_Cmd_wr32(phost, d2);
	EVE_Cmd_wr16(phost, w3);
	EVE_Cmd_wr16(phost, w4);
	EVE_Cmd_wr32(phost, d5);
	EVE_Cmd_endFunc(phost);
}

EVE_HAL_EXPORT void EVE_CoCmd_dwwwwdw(EVE_HalContext *phost, uint32_t cmd,
    uint16_t w0, uint16_t w1, uint16_t w2, uint16_t w3,
    uint32_t d4, uint16_t w5)
{
#if EVE_CMD_HOOKS
	if (phost->CoCmdHook && phost->CoCmdHook(phost, cmd, 0))
		return;
#endif

	EVE_Cmd_startFunc(phost);
	EVE_Cmd_wr32(phost, cmd);
	EVE_Cmd_wr16(phost, w0);
	EVE_Cmd_wr16(phost, w1);
	EVE_Cmd_wr16(phost, w2);
	EVE_Cmd_wr16(phost, w3);
	EVE_Cmd_wr32(phost, d4);
	EVE_Cmd_wr16(phost, w5);
	EVE_Cmd_wr16(phost, 0);
	EVE_Cmd_endFunc(phost);
}

EVE_HAL_EXPORT void EVE_CoCmd_dwwwwdww(EVE_HalContext *phost, uint32_t cmd,
    uint16_t w0, uint16_t w1, uint16_t w2, uint16_t w3,
    uint32_t d4, uint16_t w5, uint16_t w6)
{
#if EVE_CMD_HOOKS
	if (phost->CoCmdHook && phost->CoCmdHook(phost, cmd, 0))
		return;
#endif

	EVE_Cmd_startFunc(phost);
	EVE_Cmd_wr32(phost, cmd);
	EVE_Cmd_wr16(phost, w0);
	EVE_Cmd_wr16(phost, w1);
	EVE_Cmd_wr16(phost, w2);
	EVE_Cmd_wr16(phost, w3);
	EVE_Cmd_wr32(phost, d4);
	EVE_Cmd_wr16(phost, w5);
	EVE_Cmd_wr16(phost, w6);
	EVE_Cmd_endFunc(phost);
}

EVE_HAL_EXPORT void EVE_CoCmd_dwwww(EVE_HalContext *phost, uint32_t cmd,
    uint16_t w0, uint16_t w1, uint16_t w2, uint16_t w3)
{
#if EVE_CMD_HOOKS
	if (phost->CoCmdHook && phost->CoCmdHook(phost, cmd, 0))
		return;
#endif

	EVE_Cmd_startFunc(phost);
	EVE_Cmd_wr32(phost, cmd);
	EVE_Cmd_wr16(phost, w0);
	EVE_Cmd_wr16(phost, w1);
	EVE_Cmd_wr16(phost, w2);
	EVE_Cmd_wr16(phost, w3);
	EVE_Cmd_endFunc(phost);
}

EVE_HAL_EXPORT void EVE_CoCmd_dwwwww(EVE_HalContext *phost, uint32_t cmd,
    uint16_t w0, uint16_t w1, uint16_t w2, uint16_t w3, uint16_t w4)
{
#if EVE_CMD_HOOKS
	if (phost->CoCmdHook && phost->CoCmdHook(phost, cmd, 0))
		return;
#endif

	EVE_Cmd_startFunc(phost);
	EVE_Cmd_wr32(phost, cmd);
	EVE_Cmd_wr16(phost, w0);
	EVE_Cmd_wr16(phost, w1);
	EVE_Cmd_wr16(phost, w2);
	EVE_Cmd_wr16(phost, w3);
	EVE_Cmd_wr16(phost, w4);
	EVE_Cmd_wr16(phost, 0);
	EVE_Cmd_endFunc(phost);
}

EVE_HAL_EXPORT void EVE_CoCmd_dwwwwd(EVE_HalContext *phost, uint32_t cmd,
    uint16_t w0, uint16_t w1, uint16_t w2, uint16_t w3, uint32_t d4)
{
#if EVE_CMD_HOOKS
	if (phost->CoCmdHook && phost->CoCmdHook(phost, cmd, 0))
		return;
#endif

	EVE_Cmd_startFunc(phost);
	EVE_Cmd_wr32(phost, cmd);
	EVE_Cmd_wr16(phost, w0);
	EVE_Cmd_wr16(phost, w1);
	EVE_Cmd_wr16(phost, w2);
	EVE_Cmd_wr16(phost, w3);
	EVE_Cmd_wr32(phost, d4);
	EVE_Cmd_endFunc(phost);
}

EVE_HAL_EXPORT void EVE_CoCmd_dwwwwwww(EVE_HalContext *phost, uint32_t cmd,
    uint16_t w0, uint16_t w1, uint16_t w2, uint16_t w3,
    uint16_t w4, uint16_t w5, uint16_t w6)
{
#if EVE_CMD_HOOKS
	if (phost->CoCmdHook && phost->CoCmdHook(phost, cmd, 0))
		return;
#endif

	EVE_Cmd_startFunc(phost);
	EVE_Cmd_wr32(phost, cmd);
	EVE_Cmd_wr16(phost, w0);
	EVE_Cmd_wr16(phost, w1);
	EVE_Cmd_wr16(phost, w2);
	EVE_Cmd_wr16(phost, w3);
	EVE_Cmd_wr16(phost, w4);
	EVE_Cmd_wr16(phost, w5);
	EVE_Cmd_wr16(phost, w6);
	EVE_Cmd_wr16(phost, 0);
	EVE_Cmd_endFunc(phost);
}

EVE_HAL_EXPORT void EVE_CoCmd_dwwwwwwww(EVE_HalContext *phost, uint32_t cmd,
    uint16_t w0, uint16_t w1, uint16_t w2, uint16_t w3,
    uint16_t w4, uint16_t w5, uint16_t w6, uint16_t w7)
{
#if EVE_CMD_HOOKS
	if (phost->CoCmdHook && phost->CoCmdHook(phost, cmd, 0))
		return;
#endif

	EVE_Cmd_startFunc(phost);
	EVE_Cmd_wr32(phost, cmd);
	EVE_Cmd_wr16(phost, w0);
	EVE_Cmd_wr16(phost, w1);
	EVE_Cmd_wr16(phost, w2);
	EVE_Cmd_wr16(phost, w3);
	EVE_Cmd_wr16(phost, w4);
	EVE_Cmd_wr16(phost, w5);
	EVE_Cmd_wr16(phost, w6);
	EVE_Cmd_wr16(phost, w7);
	EVE_Cmd_endFunc(phost);
}

EVE_HAL_EXPORT void EVE_CoCmd_dwwwwwwz(EVE_HalContext *phost, uint32_t cmd,
    uint16_t w0, uint16_t w1, uint16_t w2, uint16_t w3,
    uint16_t w4, uint16_t w5, const char *s)
{
#if EVE_CMD_HOOKS
	if (phost->CoCmdHook && phost->CoCmdHook(phost, cmd, 0))
		return;
#endif

	EVE_Cmd_startFunc(phost);
	EVE_Cmd_wr32(phost, cmd);
	EVE_Cmd_wr16(phost, w0);
	EVE_Cmd_wr16(phost, w1);
	EVE_Cmd_wr16(phost, w2);
	EVE_Cmd_wr16(phost, w3);
	EVE_Cmd_wr16(phost, w4);
	EVE_Cmd_wr16(phost, w5);
	EVE_Cmd_wrString(phost, s, EVE_CMD_STRING_MAX);
	EVE_Cmd_endFunc(phost);
}

EVE_HAL_EXPORT void EVE_CoCmd_dwwwwwwz_s(EVE_HalContext *phost, uint32_t cmd,
    uint16_t w0, uint16_t w1, uint16_t w2, uint16_t w3,
    uint16_t w4, uint16_t w5, const char *s, uint32_t len)
{
#if EVE_CMD_HOOKS
	if (phost->CoCmdHook && phost->CoCmdHook(phost, cmd, 0))
		return;
#endif

	EVE_Cmd_startFunc(phost);
	EVE_Cmd_wr32(phost, cmd);
	EVE_Cmd_wr16(phost, w0);
	EVE_Cmd_wr16(phost, w1);
	EVE_Cmd_wr16(phost, w2);
	EVE_Cmd_wr16(phost, w3);
	EVE_Cmd_wr16(phost, w4);
	EVE_Cmd_wr16(phost, w5);
	EVE_Cmd_wrString(phost, s, len);
	EVE_Cmd_endFunc(phost);
}

#if defined(_DEBUG) && defined(EVE_MULTI_GRAPHICS_TARGET)
EVE_HAL_EXPORT void EVE_CoCmd_debugUnsupported(const char *cmd, uint32_t chipId)
{
	eve_printf_debug("Coprocessor command %s is not supported on target platform %lx\n", cmd, chipId);
}
#endif

/**********************************************************************
***********************************************************************
**********************************************************************/

EVE_HAL_EXPORT bool EVE_CoCmd_getMatrix(EVE_HalContext *phost, int32_t *m)
{
	uint16_t resAddr;
	int i;

#if EVE_CMD_HOOKS
	if (phost->CoCmdHook && phost->CoCmdHook(phost, CMD_GETMATRIX, 0))
		return false;
#endif

	EVE_Cmd_startFunc(phost);
	EVE_Cmd_wr32(phost, CMD_GETMATRIX);
	resAddr = EVE_Cmd_moveWp(phost, 6 * 4);
	EVE_Cmd_endFunc(phost);

	/* Read result */
	if (m)
	{
		if (!EVE_Cmd_waitFlush(phost))
			return false;
		EVE_Hal_startTransfer(phost, EVE_TRANSFER_READ, RAM_CMD + resAddr);
		for (i = 0; i < 6; ++i)
			m[i] = EVE_Hal_transfer32(phost, 0);
		EVE_Hal_endTransfer(phost);
	}
	return true;
}

EVE_HAL_EXPORT void EVE_CoCmd_setRotate(EVE_HalContext *phost, uint32_t r)
{
	const bool swapXY = EVE_CHIPID >= EVE_FT810 ? r & 0x2 : false;

#if EVE_CMD_HOOKS
	/* Check hook */
	if (phost->CoCmdHook && phost->CoCmdHook(phost, CMD_SETROTATE, r))
		return;
#endif

#if (EVE_SUPPORT_CHIPID >= EVE_FT810)
	if (EVE_CHIPID >= EVE_FT810)
	{
		/* Update cached width and height based on rotation */
		if (swapXY)
		{
			phost->Width = EVE_Hal_rd16(phost, REG_VSIZE);
			phost->Height = EVE_Hal_rd16(phost, REG_HSIZE);
		}
		else
		{
			phost->Width = EVE_Hal_rd16(phost, REG_HSIZE);
			phost->Height = EVE_Hal_rd16(phost, REG_VSIZE);
		}

		/* Command */
		EVE_Cmd_startFunc(phost);
		EVE_Cmd_wr32(phost, CMD_SETROTATE);
		EVE_Cmd_wr32(phost, r);
		EVE_Cmd_endFunc(phost);
	}
	else
#endif
	{
		/* Software based fallback for FT80X */
		EVE_Hal_wr32(phost, REG_ROTATE, r);

		/* TODO: Adjust touch transform matrix */
	}

	/* Don't keep this in the write buffer */
	EVE_Hal_flush(phost);
}

#if (EVE_SUPPORT_CHIPID >= EVE_BT817)

/* Returns new frequency, or 0 in case of failure */
EVE_HAL_EXPORT uint32_t EVE_CoCmd_pclkFreq(EVE_HalContext *phost, uint32_t ftarget, int32_t rounding)
{
	uint16_t resAddr;

	EVE_MULTI_TARGET_CHECK_RETURN(CMD_PCLKFREQ, EVE_CHIPID >= EVE_BT817, 0);

#if EVE_CMD_HOOKS
	if (phost->CoCmdHook && phost->CoCmdHook(phost, CMD_PCLKFREQ, ftarget))
		return 0;
#endif

	EVE_Cmd_startFunc(phost);
	EVE_Cmd_wr32(phost, CMD_PCLKFREQ);
	EVE_Cmd_wr32(phost, ftarget);
	EVE_Cmd_wr32(phost, rounding);
	resAddr = EVE_Cmd_moveWp(phost, 4);
	EVE_Cmd_endFunc(phost);

	/* Wait for the result */
	if (!EVE_Cmd_waitFlush(phost))
		return 0;
	return EVE_Hal_rd32(phost, RAM_CMD + resAddr);
}

#endif

/**********************************************************************
***********************************************************************
**********************************************************************/

EVE_HAL_EXPORT uint32_t EVE_CoCmd_calibrate(EVE_HalContext *phost)
{
	uint16_t resAddr;

#if EVE_CMD_HOOKS
	if (phost->CoCmdHook && phost->CoCmdHook(phost, CMD_CALIBRATE, 0))
		return 0;
#endif

	EVE_Cmd_startFunc(phost);
	EVE_Cmd_wr32(phost, CMD_CALIBRATE);
	resAddr = EVE_Cmd_moveWp(phost, 4);
	EVE_Cmd_endFunc(phost);

#if (EVE_DL_OPTIMIZE)
	phost->DlPrimitive = 0;
#endif

	/* Wait for the result */
	if (!EVE_Cmd_waitFlush(phost))
		return 0;
	return EVE_Hal_rd32(phost, RAM_CMD + resAddr);
}

#if (EVE_SUPPORT_CHIPID >= EVE_BT815)

EVE_HAL_EXPORT bool EVE_CoCmd_animStart(EVE_HalContext *phost, int32_t ch, uint32_t aoptr, uint32_t loop)
{
	uint32_t flashStatus;

	EVE_MULTI_TARGET_CHECK_RETURN(CMD_ANIMSTART, EVE_CHIPID >= EVE_BT815, false);

#if EVE_CMD_HOOKS
	if (phost->CoCmdHook && phost->CoCmdHook(phost, CMD_ANIMSTART, ch))
		return false;
#endif

	if (!EVE_Cmd_waitFlush(phost))
		return false;
	flashStatus = EVE_Hal_rd32(phost, REG_FLASH_STATUS);
	if (flashStatus < FLASH_STATUS_FULL)
	{
		// Don't attempt to start animation without full flash speed, it hangs the coprocessor
		eve_printf_debug("Attempt to send CMD_ANIMSTART without FLASH_STATUS_FULL (REG_FLASH_STATUS: %i)\n", (int)flashStatus);
		return false;
	}

	EVE_Cmd_startFunc(phost);
	EVE_Cmd_wr32(phost, CMD_ANIMSTART);
	EVE_Cmd_wr32(phost, ch);
	EVE_Cmd_wr32(phost, aoptr);
	EVE_Cmd_wr32(phost, loop);
	EVE_Cmd_endFunc(phost);
	return EVE_Cmd_waitFlush(phost);
}

#endif

#if (EVE_SUPPORT_CHIPID >= EVE_BT817)

EVE_HAL_EXPORT uint32_t EVE_CoCmd_calibrateSub(EVE_HalContext *phost, uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
	uint16_t resAddr;

	EVE_MULTI_TARGET_CHECK_RETURN(CMD_CALIBRATESUB, EVE_CHIPID >= EVE_BT817, 0);

#if EVE_CMD_HOOKS
	if (phost->CoCmdHook && phost->CoCmdHook(phost, CMD_CALIBRATESUB, 0))
		return 0;
#endif

	EVE_Cmd_startFunc(phost);
	EVE_Cmd_wr32(phost, CMD_CALIBRATESUB);
	EVE_Cmd_wr16(phost, x);
	EVE_Cmd_wr16(phost, y);
	EVE_Cmd_wr16(phost, w);
	EVE_Cmd_wr16(phost, h);
	resAddr = EVE_Cmd_moveWp(phost, 4);
	EVE_Cmd_endFunc(phost);

#if (EVE_DL_OPTIMIZE)
	phost->DlPrimitive = 0;
#endif

	/* Wait for the result */
	if (!EVE_Cmd_waitFlush(phost))
		return 0;
	return EVE_Hal_rd32(phost, RAM_CMD + resAddr);
}

#endif

/**********************************************************************
***********************************************************************
**********************************************************************/

/* Nothing beyond this */
