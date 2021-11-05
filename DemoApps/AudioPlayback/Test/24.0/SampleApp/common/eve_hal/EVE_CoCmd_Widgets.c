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

#include <stdarg.h>

#if (EVE_SUPPORT_CHIPID >= EVE_BT815)
/* Count number of arguments in Cmd_Text for string format*/
static uint8_t countArgs(const char *str)
{
	uint8_t count = 0;
	const char *tmp = str;

	while ((tmp = strstr(tmp, "%")))
	{
		if (*(tmp + 1) == '%')
		{
			tmp += 2;
		}
		else
		{
			count++;
			tmp++;
		}
	}
	return count;
}
#endif

EVE_HAL_EXPORT void EVE_CoCmd_text(EVE_HalContext *phost, int16_t x, int16_t y, int16_t font, uint16_t options, const char *s, ...)
{
	va_list args;
#if (EVE_SUPPORT_CHIPID >= EVE_BT815)
	uint8_t num;
#endif

#if EVE_CMD_HOOKS
	if (phost->CoCmdHook && phost->CoCmdHook(phost, CMD_TEXT, 0))
		return;
#endif

	if (font >= 32)
	{
		if (font != 63)
			eve_printf_debug("Invalid font handle specified: %i\n", (int)font);
		return;
	}
	va_start(args, s);
#if (EVE_SUPPORT_CHIPID >= EVE_BT815) /* OPT_FORMAT not defined in FT8xx chip */
	num = (options & OPT_FORMAT) ? (countArgs(s)) : (0); /* Only check % characters if option OPT_FORMAT is set */
#endif

	EVE_Cmd_startFunc(phost);
	EVE_Cmd_wr32(phost, CMD_TEXT);
	EVE_Cmd_wr16(phost, x);
	EVE_Cmd_wr16(phost, y);
	EVE_Cmd_wr16(phost, font);
	EVE_Cmd_wr16(phost, options);
	EVE_Cmd_wrString(phost, s, EVE_CMD_STRING_MAX);
#if (EVE_SUPPORT_CHIPID >= EVE_BT815)
	if (EVE_CHIPID >= EVE_BT815)
	{
		for (uint8_t i = 0; i < num; i++)
			EVE_Cmd_wr32(phost, (uint32_t)va_arg(args, uint32_t));
	}
#endif
	EVE_Cmd_endFunc(phost);
	va_end(args);

#if (EVE_DL_OPTIMIZE)
	phost->DlPrimitive = 0;
#endif
}

EVE_HAL_EXPORT void EVE_CoCmd_text_s(EVE_HalContext *phost, int16_t x, int16_t y, int16_t font, uint16_t options, const char *s, uint32_t length)
{
#if EVE_CMD_HOOKS
	if (phost->CoCmdHook && phost->CoCmdHook(phost, CMD_TEXT, 0))
		return;
#endif

	if (font >= 32)
	{
		if (font != 63)
			eve_printf_debug("Invalid font handle specified: %i\n", (int)font);
		return;
	}

	EVE_Cmd_startFunc(phost);
	EVE_Cmd_wr32(phost, CMD_TEXT);
	EVE_Cmd_wr16(phost, x);
	EVE_Cmd_wr16(phost, y);
	EVE_Cmd_wr16(phost, font);
	EVE_Cmd_wr16(phost, options);
	EVE_Cmd_wrString(phost, s, length);
	EVE_Cmd_endFunc(phost);

#if (EVE_DL_OPTIMIZE)
	phost->DlPrimitive = 0;
#endif
}

EVE_HAL_EXPORT void EVE_CoCmd_text_ex(EVE_HalContext *phost, int16_t x, int16_t y, int16_t font, uint16_t options, bool bottom, int16_t baseLine, int16_t capsHeight, int16_t xOffset, const char *s)
{
	int16_t yOffset;
	if (options & OPT_CENTERY)
		yOffset = baseLine - (capsHeight >> 1);
	else if (bottom)
		yOffset = baseLine;
	else
		yOffset = baseLine - capsHeight;
	/*
	if (options & OPT_RIGHTX)
		xOffset = 0;
	else if (options & OPT_CENTERX)
		xOffset >>= 1;
	*/
	EVE_CoCmd_text(phost, x - xOffset, y - yOffset, font, options & ~OPT_CENTERY, s);
}

EVE_HAL_EXPORT void EVE_CoCmd_button(EVE_HalContext *phost, int16_t x, int16_t y, int16_t w, int16_t h, int16_t font, uint16_t options, const char *s, ...)
{
	va_list args;
#if (EVE_SUPPORT_CHIPID >= EVE_BT815)
	uint8_t num;
#endif

#if EVE_CMD_HOOKS
	if (phost->CoCmdHook && phost->CoCmdHook(phost, CMD_BUTTON, 0))
		return;
#endif

	if (font >= 32)
	{
		if (font != 63)
			eve_printf_debug("Invalid font handle specified: %i\n", (int)font);
		return;
	}

	va_start(args, s);
#if (EVE_SUPPORT_CHIPID >= EVE_BT815) /* OPT_FORMAT not defined in FT8xx chip */
	num = (options & OPT_FORMAT) ? (countArgs(s)) : (0); /* Only check % characters if option OPT_FORMAT is set */
#endif

	EVE_Cmd_startFunc(phost);
	EVE_Cmd_wr32(phost, CMD_BUTTON);
	EVE_Cmd_wr32(phost, (((uint32_t)y << 16) | (x & 0xffff)));
	EVE_Cmd_wr32(phost, (((uint32_t)h << 16) | (w & 0xffff)));
	EVE_Cmd_wr32(phost, (((uint32_t)options << 16) | (font & 0xffff)));
	EVE_Cmd_wrString(phost, s, EVE_CMD_STRING_MAX);
#if (EVE_SUPPORT_CHIPID >= EVE_BT815)
	if (EVE_CHIPID >= EVE_BT815)
	{
		for (uint8_t i = 0; i < num; i++)
			EVE_Cmd_wr32(phost, (uint32_t)va_arg(args, uint32_t));
	}
#endif
	EVE_Cmd_endFunc(phost);
	va_end(args);

#if (EVE_DL_OPTIMIZE)
	phost->DlPrimitive = 0;
#endif
}

EVE_HAL_EXPORT void EVE_CoCmd_keys(EVE_HalContext *phost, int16_t x, int16_t y, int16_t w, int16_t h, int16_t font, uint16_t options, const char *s)
{
#if EVE_CMD_HOOKS
	if (phost->CoCmdHook && phost->CoCmdHook(phost, CMD_KEYS, 0))
		return;
#endif

	if (font >= 32)
	{
		if (font != 63)
			eve_printf_debug("Invalid font handle specified: %i\n", (int)font);
		return;
	}

	EVE_Cmd_startFunc(phost);
	EVE_Cmd_wr32(phost, CMD_KEYS);
	EVE_Cmd_wr16(phost, x);
	EVE_Cmd_wr16(phost, y);
	EVE_Cmd_wr16(phost, w);
	EVE_Cmd_wr16(phost, h);
	EVE_Cmd_wr16(phost, font);
	EVE_Cmd_wr16(phost, options);
	EVE_Cmd_wrString(phost, s, EVE_CMD_STRING_MAX);
	EVE_Cmd_endFunc(phost);

#if (EVE_DL_OPTIMIZE)
	phost->DlPrimitive = 0;
#endif
}

EVE_HAL_EXPORT void EVE_CoCmd_toggle(EVE_HalContext *phost, int16_t x, int16_t y, int16_t w, int16_t font, uint16_t options, uint16_t state, const char *s, ...)
{
	va_list args;
#if (EVE_SUPPORT_CHIPID >= EVE_BT815)
	uint8_t num;
#endif

#if EVE_CMD_HOOKS
	if (phost->CoCmdHook && phost->CoCmdHook(phost, CMD_TOGGLE, 0))
		return;
#endif

	if (font >= 32)
	{
		if (font != 63)
			eve_printf_debug("Invalid font handle specified: %i\n", (int)font);
		return;
	}

	va_start(args, s);
#if (EVE_SUPPORT_CHIPID >= EVE_BT815) /* OPT_FORMAT not defined in FT8xx chip */
	num = (options & OPT_FORMAT) ? (countArgs(s)) : (0); //Only check % characters if option OPT_FORMAT is set
#endif

	EVE_Cmd_startFunc(phost);
	EVE_Cmd_wr32(phost, CMD_TOGGLE);
	EVE_Cmd_wr16(phost, x);
	EVE_Cmd_wr16(phost, y);
	EVE_Cmd_wr16(phost, w);
	EVE_Cmd_wr16(phost, font);
	EVE_Cmd_wr16(phost, options);
	EVE_Cmd_wr16(phost, state);
	EVE_Cmd_wrString(phost, s, EVE_CMD_STRING_MAX);
#if (EVE_SUPPORT_CHIPID >= EVE_BT815)
	if (EVE_CHIPID >= EVE_BT815)
	{
		for (uint8_t i = 0; i < num; i++)
			EVE_Cmd_wr32(phost, (uint32_t)va_arg(args, uint32_t));
	}
#endif
	EVE_Cmd_endFunc(phost);
	va_end(args);

#if (EVE_DL_OPTIMIZE)
	phost->DlPrimitive = 0;
#endif
}

EVE_HAL_EXPORT void EVE_CoCmd_number(EVE_HalContext *phost, int16_t x, int16_t y, int16_t font, uint16_t options, int32_t n)
{
#if EVE_CMD_HOOKS
	if (phost->CoCmdHook && phost->CoCmdHook(phost, CMD_NUMBER, 0))
		return;
#endif

	if (font >= 32)
	{
		if (font != 63)
			eve_printf_debug("Invalid font handle specified: %i\n", (int)font);
		return;
	}

	EVE_Cmd_startFunc(phost);
	EVE_Cmd_wr32(phost, CMD_NUMBER);
	EVE_Cmd_wr16(phost, x);
	EVE_Cmd_wr16(phost, y);
	EVE_Cmd_wr16(phost, font);
	EVE_Cmd_wr16(phost, options);
	EVE_Cmd_wr32(phost, n);
	EVE_Cmd_endFunc(phost);

#if (EVE_DL_OPTIMIZE)
	phost->DlPrimitive = 0;
#endif
}

/* end of file */
