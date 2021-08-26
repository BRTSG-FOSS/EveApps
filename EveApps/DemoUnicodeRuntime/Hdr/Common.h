#ifndef COMMON_H_
#define COMMON_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Platform.h"

#define APP_ERROR 0
#define APP_OK    1
#define TRUE    1
#define false    0

// Fat32 - 3rd party library to read SDcard from FT9X platform
#if defined(FT900_PLATFORM) || defined(FT93X_PLATFORM)
#include "ff.h"
#endif

/* macro: PRINTF_LEVEL
 *    range: 0-3
 *    1 for release mode: Only APP_INF messages and printf function calls
 *    2 and 3 for debug mode
 *       - 2: Only APP_ERR messages
 *       - 3: APP_ERR messages and APP_DBG messages
 *    0 to print nothing
 */
#define PRINTF_LEVEL                        3

#define APP_PRINTF_INFO(M, ...)             printf("[INF] " M "\n", ##__VA_ARGS__)
#define APP_PRINTF_ERROR(M, ...)            printf("[ERROR] %s:%d: " M "\n", __func__, __LINE__, ##__VA_ARGS__)
#define APP_PRINTF_DEBUG(M, ...)            printf("[DBG] %s:%d: " M "\n", __func__, __LINE__, ##__VA_ARGS__)
#if   PRINTF_LEVEL == 1                     
#define APP_INF                             APP_PRINTF_INFO
#define APP_ERR(M,...)                      /* nothing */
#define APP_DBG(M,...)                      /* nothing */
#elif PRINTF_LEVEL == 2                     
#define APP_INF                             APP_PRINTF_INFO
#define APP_ERR                             APP_PRINTF_ERROR
#define APP_DBG(M,...)                      /* nothing */
#elif PRINTF_LEVEL == 3                     
#define APP_INF                             APP_PRINTF_INFO
#define APP_ERR                             APP_PRINTF_ERROR
#define APP_DBG                             APP_PRINTF_DEBUG
#else                                       
#define APP_INF(M,...)                      /* nothing */
#define APP_ERR(M,...)                      /* nothing */
#define APP_DBG(M,...)                      /* nothing */
#define printf(M,...)                       /* nothing */
#endif                                      

#endif /* COMMON_H_ */
