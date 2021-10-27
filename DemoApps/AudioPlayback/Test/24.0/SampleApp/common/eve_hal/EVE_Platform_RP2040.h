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

#ifndef EVE_PLATFORM_RP2040__H
#define EVE_PLATFORM_RP2040__H

#include "EVE_Config.h"
#if defined(RP2040_PLATFORM)

/* RP2040 platform libraries */
#include "pico/stdlib.h"
#include "hardware/spi.h"
/* #include "hardware/dma.h" */
/* #include "hardware/pio.h" */

/* Standard C libraries */
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>

/* RP2040 default platform definitions. Configuration can be changed programatically in initialization parameters */
/* Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments */
#define EVE_DEFAULT_SPI0_MISO 4 /* GP0, GP4, GP16 */
#define EVE_DEFAULT_SPI0_CS   5 /* GP1, GP5, GP17 */
#define EVE_DEFAULT_SPI0_SCK  2 /* GP2, GP6, GP18 */
#define EVE_DEFAULT_SPI0_MOSI 3 /* GP3, GP7, GP19 */
#define EVE_DEFAULT_SPI0_INT  6
#define EVE_DEFAULT_SPI0_PWD  7
#define EVE_DEFAULT_SPI0_IO2  14
#define EVE_DEFAULT_SPI0_IO3  15

#define EVE_DEFAULT_SPI1_MISO 12 /* GP8, GP12 */
#define EVE_DEFAULT_SPI1_CS1  13 /* GP9, GP13 */
#define EVE_DEFAULT_SPI1_CS2  22
#define EVE_DEFAULT_SPI1_SCK  10 /* GP10, GP14 */
#define EVE_DEFAULT_SPI1_MOSI 11 /* GP11, GP15 */
#define EVE_DEFAULT_SPI1_INT  16
#define EVE_DEFAULT_SPI1_PWD  17

/* Only use SPI0 by default, otherwise EVE_Hal_open will alternate between SPI0 and SPI1 for multiple devices */
#define EVE_DEFAULT_SPI0_ONLY 1

/*
By default, `stdin` and `stdout` go through UART0, using GP0 and GP1.
Using USB CDC (USB serial) is possible too, specified in the CMake options, see Getting Started guide.
*/

#ifdef CFG_TUSB_DEBUG_PRINTF
#define eve_printf CFG_TUSB_DEBUG_PRINTF
#endif

#endif
#endif /* #ifndef EVE_PLATFORM_RP2040__H */

/* end of file */
