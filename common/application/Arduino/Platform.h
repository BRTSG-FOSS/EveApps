/*

Copyright (c) Bridgetek Pte Ltd

THIS SOFTWARE IS PROVIDED BY BRIDGETEK PTE LTD "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
BRIDGETEK PTE LTD BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
OF SUBSTITUTE GOODS OR SERVICES LOSS OF USE, DATA, OR PROFITS OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

BRIDGETEK DRIVERS MAY BE USED ONLY IN CONJUNCTION WITH PRODUCTS BASED ON BRIDGETEK PARTS.

BRIDGETEK DRIVERS MAY BE DISTRIBUTED IN ANY FORM AS LONG AS LICENSE INFORMATION IS NOT MODIFIED.

IF A CUSTOM VENDOR ID AND/OR PRODUCT ID OR DESCRIPTION STRING ARE USED, IT IS THE
RESPONSIBILITY OF THE PRODUCT MANUFACTURER TO MAINTAIN ANY CHANGES AND SUBSEQUENT WHQL
RE-CERTIFICATION AS A RESULT OF MAKING THESE CHANGES.

Abstract:

This file contains is functions for all UI fields.

Author : BRIDGETEK 

Revision History: 
0.1 - date 2013.04.24 - initial version
0.2 - date 2014.04.28 - Split in individual files according to platform
1.0 - date 2014.11.24 - Addition of FT81x
1.1 - date 2015.04.15 - Introduction of module specific configurations
*/

#ifndef _PLATFORM_H_
#define _PLATFORM_H_

#define ARDUINO_PLATFORM

/* Module specific configurations */
#define VM800P43_50
//#define VM800P35
//#define VM801P43_50
//#define VM800B43_50
//#define VM800B35
//#define VM801B43_50

#ifdef VM800P43_50

#define DISPLAY_RESOLUTION_WQVGA                (1)
#define FT800_ENABLE                            (1)
#define ENABLE_SPI_SINGLE                       (1)
#define ARDUINO_ATMEGA328P_I2C                  (1)
#define RTC_PRESENT                             (1)
#define FT800_CS                                (9)
#define SDCARD_CS                               (8)
#define FT800_INT                               (3)
#define FT800_PD_N                              (4)
#define ARDUINO_PRO_SPI_CS                      FT800_CS
#define ARDUINO_PLATFORM_SPI                    (1)
#define ARDUINO_PLATFORM_COCMD_BURST            (1)
#define RESISTANCE_THRESHOLD                    (1800)
#endif

#ifdef VM800P35

#define DISPLAY_RESOLUTION_QVGA                    (1)
#define FT800_ENABLE                               (1)
#define ENABLE_SPI_SINGLE                          (1)
#define ARDUINO_ATMEGA328P_I2C                     (1)
#define RTC_PRESENT                                (1)
#define FT800_CS                                   (9)
#define SDCARD_CS                                  (8)
#define FT800_INT                                  (3)
#define FT800_PD_N                                 (4)
#define ARDUINO_PRO_SPI_CS                         FT800_CS
#define ARDUINO_PLATFORM_SPI                       (1)
#define ARDUINO_PLATFORM_COCMD_BURST               (1)
#define RESISTANCE_THRESHOLD                       (1800)
#endif

#ifdef VM801P43_50

#define DISPLAY_RESOLUTION_WQVGA                (1)
#define FT801_ENABLE                            (1)
#define ENABLE_SPI_SINGLE                       (1)
#define ARDUINO_ATMEGA328P_I2C                  (1)
#define RTC_PRESENT                             (1)
#define FT800_CS                                (9)
#define SDCARD_CS                               (8)
#define FT800_INT                               (3)
#define FT800_PD_N                              (4)
#define ARDUINO_PRO_SPI_CS                      FT800_CS
#define ARDUINO_PLATFORM_SPI                    (1)
#define ARDUINO_PLATFORM_COCMD_BURST            (1)

#endif

#ifdef VM800B43_50

#define DISPLAY_RESOLUTION_WQVGA                (1)
#define FT800_ENABLE                            (1)
#define ENABLE_SPI_SINGLE                       (1)
#define FT800_INT                               (3)
#define FT800_PD_N                              (4)
#define SDCARD_CS                               (5)       
#define FT800_CS                                (10)
#define ARDUINO_PRO_SPI_CS                      FT800_CS
#define ARDUINO_PLATFORM_SPI                    (1)
#define ARDUINO_PLATFORM_COCMD_BURST            (1)
#define RESISTANCE_THRESHOLD                    (1800)
#endif

#ifdef VM800B35

#define DISPLAY_RESOLUTION_QVGA                   (1)
#define FT800_ENABLE                              (1)
#define ENABLE_SPI_SINGLE                         (1)
#define FT800_INT                                 (3)
#define FT800_PD_N                                (4)
#define SDCARD_CS                                 (5)       
#define FT800_CS                                  (10)
#define ARDUINO_PRO_SPI_CS                        FT800_CS
#define ARDUINO_PLATFORM_SPI                      (1)
#define ARDUINO_PLATFORM_COCMD_BURST              (1)
#define RESISTANCE_THRESHOLD                      (1800)
#endif

#ifdef VM801B43_50

#define DISPLAY_RESOLUTION_WQVGA                (1)
#define FT801_ENABLE                            (1)
#define ENABLE_SPI_SINGLE                       (1)
#define FT800_INT                               (3)
#define FT800_PD_N                              (4)
#define SDCARD_CS                               (5)
#define FT800_CS                                (10)
#define ARDUINO_PRO_SPI_CS                      FT800_CS
#define ARDUINO_PLATFORM_SPI                    (1)
#define ARDUINO_PLATFORM_COCMD_BURST            (1)

#endif

/* Custom configuration set by the user */
#if (!defined(VM800P43_50) && !defined(VM800P35) &&!defined(VM801P43_50) &&!defined(VM800B43_50) &&!defined(VM800B35) &&!defined(VM801B43_50))
#define DISPLAY_RESOLUTION_QVGA                     (1)
#define DISPLAY_RESOLUTION_WQVGA                    (1)
#define DISPLAY_RESOLUTION_WVGA                     (1)
#define DISPLAY_RESOLUTION_HVGA_PORTRAIT            (1)

/* Chip configuration specific macros */
#define FT800_ENABLE                            (1)
#define FT801_ENABLE                            (1)
#define FT810_ENABLE                            (1)
#define FT811_ENABLE                            (1)
#define FT812_ENABLE                            (1)
#define FT813_ENABLE                            (1)

/* SPI specific macros - compile time switches for SPI single, dial and quad use cases */
#define ENABLE_SPI_SINGLE                          (1)
#define ENABLE_SPI_DUAL                            (1)
#define ENABLE_SPI_QUAD                            (1)

#define FT800_PD_N                                 (4)
#define FT800_CS                                   (10)

/* Display driver configurations - mainly for ME900EV1 modules */
#define ENABLE_ILI9488_HVGA_PORTRAIT            (1)
#define ARDUINO_PLATFORM_SPI

/* Threshold for resistance */
#define RESISTANCE_THRESHOLD                    (1200)

#endif

#if (defined(FT800_ENABLE) || defined(FT801_ENABLE))
#define FT80X_ENABLE                                (1)
#endif

#if (defined(FT810_ENABLE) || defined(FT811_ENABLE) || defined(FT812_ENABLE) || defined(FT813_ENABLE))
#define FT81X_ENABLE                                (1)
#endif

/* Type of file to load from SDCard or Windows file system */
#define LOADIMAGE 1  //loadimage command takes destination address and options before the actual bitmap data
#define INFLATE 2    //inflate command takes destination address before the actual bitmap
#define LOAD 3       //load bitmaps directly

/* Standard C libraries */
#include <stdio.h>
/* Standard Arduino libraries */
#include <Arduino.h>
#include <EEPROM.h>
#include <SPI.h>
#include <avr/pgmspace.h>
#include <stdint.h>
typedef float float_t;

typedef boolean bool_t;
#define TRUE     (1)
#define FALSE    (0)

typedef char char8_t;
typedef unsigned char uchar8_t;
typedef signed char   schar8_t;

typedef PROGMEM const unsigned char  prog_uchar8_t;
typedef PROGMEM const char           prog_char8_t;
typedef PROGMEM const uint8_t        prog_uint8_t;
typedef PROGMEM const int8_t         prog_int8_t;
typedef PROGMEM const uint16_t       prog_uint16_t;
typedef PROGMEM const int16_t        prog_int16_t;
typedef PROGMEM const uint32_t       prog_uint32_t;
typedef PROGMEM const int32_t        prog_int32_t;
/* HAL inclusions */
#include "Gpu_Hal.h"
#include "Gpu.h"
#include "CoPro_Cmds.h"
#include "Hal_Utils.h"

#define FT800_SEL_PIN           FT800_CS

#endif /*_PLATFORM_H_*/
/* Nothing beyond this*/

