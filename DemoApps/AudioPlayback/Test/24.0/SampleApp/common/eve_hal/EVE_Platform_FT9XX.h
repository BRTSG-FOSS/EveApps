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

Author : BRIDGETEK

Revision History:
0.1 - date 2013.04.24 - Initial version
0.2 - date 2014.04.28 - Split in individual files according to platform
1.0 - date 2014.11.24 - Addition of FT81X
1.1 - date 2015.01.15 - Addition of module specific configurations switches. Especially for MM900 modules
1.2 - data 2017.03.24 - Addition of GPIOs pin definitions

*/

#ifndef EVE_PLATFORM_FT900__H
#define EVE_PLATFORM_FT900__H

#include "EVE_Config.h"
#if defined(FT9XX_PLATFORM)

#if defined(FT900_PLATFORM)

#define GPIO_UART0_TX (48)
#define GPIO_UART0_RX (49)

#define GPIO_SPIM_CLK (27)
#define GPIO_SPIM_SS0 (28)
#define GPIO_SPIM_MOSI (29)
#define GPIO_SPIM_MISO (30)

#define GPIO_SPIM_IO2 (31)
#define GPIO_SPIM_IO3 (32)

#define GPIO_SPIM_SS1 (33)
#define GPIO_SPIM_SS2 (34)

#define GPIO_ILI9488_CS1 (33)
#define GPIO_ILI9488_DCX (34)

#define GPIO_FT800_INT (42)
#define GPIO_FT800_PWD (43)

#define pad_ili9488_dcx (pad_func_0)
#define pad_ili9488_cs1 (pad_func_0)

#define GPIO_SD_CLK (19)
#define GPIO_SD_CMD (20)
#define GPIO_SD_DAT3 (21)
#define GPIO_SD_DAT2 (22)
#define GPIO_SD_DAT1 (23)
#define GPIO_SD_DAT0 (24)
#define GPIO_SD_CD (25)
#define GPIO_SD_WP (26)

#elif defined(FT93X_PLATFORM)

#define GPIO_UART0_TX (23)
#define GPIO_UART0_RX (22)

#define GPIO_SPIM_CLK (34)
#define GPIO30_SPIM_SS0 (30)
#define GPIO_SPIM_MOSI (36)
#define GPIO_SPIM_MISO (35)

#define GPIO_SPIM_IO2 (37)
#define GPIO_SPIM_IO3 (38)

#define GPIO_SPIM_SS1 (31)
#define GPIO_SPIM_SS2 (32)
#define GPIO_SPIM_SS3 (33)

#define GPIO_ILI9488_CS1 (31)
#define GPIO_ILI9488_DCX (8)

#define GPIO_FT800_INT (14)
#define GPIO_FT800_PWD (15)

#define pad_ili9488_dcx (pad_func_0)
#define pad_ili9488_cs1 (pad_func_0)

#define GPIO_SD_CLK (0)
#define GPIO_SD_CMD (1)
#define GPIO_SD_DAT3 (6)
#define GPIO_SD_DAT2 (5)
#define GPIO_SD_DAT1 (4)
#define GPIO_SD_DAT0 (3)
#define GPIO_SD_CD (2)
#define GPIO_SD_WP (7)

#endif

/* Timer 1 is been utilized in case of FT900 platform */
#define FT900_FT_MILLIS_TIMER (timer_select_b)
#define FT900_TIMER_MAX_VALUE (65536L)
#define FT900_TIMER_PRESCALE_VALUE (100)
#define FT900_TIMER_OVERFLOW_VALUE (1000)

/* Hardware or Module specific macros for gpio line numbers  */
#if defined(CLEO35)

#undef GPIO_FT800_PWD
#define GPIO_FT800_PWD (66) //for cleo35 module

#elif defined(CLEO50)

#undef GPIO_FT800_PWD
#define GPIO_FT800_PWD (0) //for cleo50 module

#elif defined(FT900DEMOBOARD)

#undef GPIO_FT800_PWD
#define GPIO_FT800_PWD (32)

#elif defined(PANL35)

#undef GPIO_FT800_INT
#undef GPIO_FT800_PWD
#define GPIO_FT800_INT (0)
#define GPIO_FT800_PWD (1)

#elif defined(PANL70)

#undef GPIO_FT800_INT
#undef GPIO_FT800_PWD
#define GPIO_FT800_INT (0)
#define GPIO_FT800_PWD (1)
#define GPIO_GOODIX (33)

#elif defined(PANL70PLUS)

#undef GPIO_FT800_INT
#undef GPIO_FT800_PWD
#define GPIO_FT800_INT (0)
#define GPIO_FT800_PWD (1)
#define GPIO_GOODIX (33)
#define pad_gpio_goodix (pad_func_0)

#endif

#ifndef RTC_PRESENT
#ifdef FT900_PLATFORM_RTC_I2C
#define RTC_PRESENT (1)
#endif
#endif /* RTC_PRESENT */

/* Standard C libraries */
#define ESD_USING_NEW_LIB // Using NewLib C Library for runtime

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>

/* FT900 libraries */
#include "ft900_uart_simple.h"
#include "ft900_sdhost.h"
#include "ft900_spi.h"
#include "ft900_gpio.h"
#include "ft900_rtc.h"
#include "ft900_interrupt.h"
#include "ft900_i2cm.h"
#include "ft900.h"
#include "ft900_delay.h"

#define eve_printf(fmt, ...) iprintf(fmt, ##__VA_ARGS__)
#define eve_sprintf(str, fmt, ...) siprintf(str, fmt, ##__VA_ARGS__)

#endif
#endif /* #ifndef EVE_PLATFORM_FT900__H */

/* end of file */
