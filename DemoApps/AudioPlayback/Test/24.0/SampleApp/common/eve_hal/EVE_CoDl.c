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

#include "EVE_CoDl.h"

#if defined(_MSC_VER) && (_MSC_VER < 1800)
#include <stdio.h> /* memset */
#endif

EVE_HAL_EXPORT void EVE_CoDlImpl_resetDlState(EVE_HalContext *phost)
{
#if (EVE_DL_OPTIMIZE) || (EVE_DL_CACHE_SCISSOR) || (EVE_SUPPORT_CHIPID < EVE_FT810) || defined(EVE_MULTI_GRAPHICS_TARGET)
	phost->DlStateIndex = 0;
#if defined(_MSC_VER) && (_MSC_VER < 1800)
	/* Designated initializers not supported in older Visual Studio versions */
	memset(&phost->DlState[0], 0, sizeof(EVE_HalDlState));
#if (EVE_DL_OPTIMIZE)
	phost->DlState[0].LineWidth = 16;
	phost->DlState[0].PointSize = 16;
	phost->DlState[0].ColorRGB = 0xFFFFFF;
	phost->DlState[0].ColorA = 0xFF;
	phost->DlState[0].Handle = 0x3F; /* Invalid value */
#endif
	phost->DlState[0].VertexFormat = 4;
#else
	phost->DlState[0] = (EVE_HalDlState)
	{
#if (EVE_DL_OPTIMIZE)
		.LineWidth = 16,
		.PointSize = 16,
		.ColorRGB = 0xFFFFFF,
#endif
#if (EVE_DL_CACHE_SCISSOR)
		.ScissorWidth = (uint16_t)phost->Width,
		.ScissorHeight = (uint16_t)phost->Height,
#endif
#if (EVE_DL_OPTIMIZE)
		.ColorA = 0xFF,
		.Handle = 0x3F, /* Invalid value */
#endif
#if (EVE_DL_OPTIMIZE) || (EVE_SUPPORT_CHIPID < EVE_FT810) || defined(EVE_MULTI_GRAPHICS_TARGET)
		    .VertexFormat = 4,
#elif (EVE_DL_CACHE_SCISSOR)
		0
#endif
	};
#endif
#endif

#if (EVE_DL_OPTIMIZE)
	phost->DlPrimitive = 0;
#endif
}

EVE_HAL_EXPORT void EVE_CoDlImpl_resetCoState(EVE_HalContext *phost)
{
#if (EVE_DL_OPTIMIZE)
	phost->CoFgColor = 0x003870;
	phost->CoBgColor = 0x002040;
	phost->CoBitmapTransform = false;
#endif

#if (EVE_SUPPORT_CHIPID >= EVE_FT810)
	if (EVE_CHIPID >= EVE_FT810)
	{
		phost->CoScratchHandle = 15;
	}
#endif
}

#if (EVE_SUPPORT_CHIPID < EVE_FT810) || defined(EVE_MULTI_GRAPHICS_TARGET)

EVE_HAL_EXPORT void EVE_CoDlImpl_vertex2ii_translate(EVE_HalContext *phost, uint16_t x, uint16_t y, uint8_t handle, uint8_t cell)
{
	int16_t xf = (x << 4) + EVE_DL_STATE.VertexTranslateX;
	int16_t yf = (y << 4) + EVE_DL_STATE.VertexTranslateY;
	if (xf < 0 || yf < 0 || xf & 0xF || yf & 0xF)
	{
#if (EVE_DL_OPTIMIZE) || (!EVE_DL_END_PRIMITIVE)
		if (phost->DlPrimitive == BITMAPS
#if (EVE_DL_OPTIMIZE)
		    && (EVE_DL_STATE.Handle != handle || EVE_DL_STATE.Cell != cell)
#endif
		)
#endif
		{
			EVE_CoCmd_dl(phost, SAVE_CONTEXT());
			EVE_CoCmd_dl(phost, BITMAP_HANDLE(handle));
			EVE_CoCmd_dl(phost, CELL(cell));
			EVE_CoCmd_dl(phost, VERTEX2F(xf, yf));
			EVE_CoCmd_dl(phost, RESTORE_CONTEXT());
		}
#if (EVE_DL_OPTIMIZE) || (!EVE_DL_END_PRIMITIVE)
		else
		{
			EVE_CoCmd_dl(phost, VERTEX2F(xf, yf));
		}
#endif
	}
	else
	{
		EVE_CoCmd_dl(phost, VERTEX2II(xf >> 4, yf >> 4, handle, cell));
	}
}

#endif

/* Nothing beyond this */
