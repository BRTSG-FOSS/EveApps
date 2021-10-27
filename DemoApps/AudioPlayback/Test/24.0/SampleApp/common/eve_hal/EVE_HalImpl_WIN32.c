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

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 28159) // Use GetTickCount64 instead of GetTickCount
#endif

#include "EVE_HalImpl.h"
#include "EVE_Platform.h"
#if defined(_WIN32)

/************
** UTILITY **
************/

/**
 * @brief Get current system clock of Coprocessor
 * 
 * @param phost Pointer to Hal context
 * @return uint32_t Frequency of Coprocessor
 */
uint32_t EVE_Hal_currentFrequency(EVE_HalContext *phost)
{
	uint32_t t0;
	uint32_t t1;

	t0 = EVE_Hal_rd32(phost, REG_CLOCK); /* t0 read */
	/* may not be precise */
	EVE_sleep(15625 / 1000);

	t1 = EVE_Hal_rd32(phost, REG_CLOCK); /* t1 read */
	return ((t1 - t0) * 64); /* bitshift 6 places is the same as multiplying 64 */
}

/*********
** MISC **
*********/

/**
 * @brief Init host MCU
 * 
 */
void EVE_Mcu_initialize()
{
	/* no-op */
}

/**
 * @brief Release host MCU
 * 
 */
void EVE_Mcu_release()
{
	/* no-op */
}

/*********
** MISC **
*********/

static DWORD s_Millis_Start;
static ULONGLONG s_Millis64_Start;

/**
 * @brief Init timer
 * 
 */
void EVE_Millis_initialize()
{
	s_Millis_Start = GetTickCount();
	s_Millis64_Start = GetTickCount64();
}

/**
 * @brief Release timer
 * 
 */
void EVE_Millis_release()
{
	/* no-op */
}

/**
 * @brief Get clock in miliseond
 * 
 * global counter to loopback after ~49.71 days
 * 
 * @return uint32_t Clock number
 */
uint32_t EVE_millis()
{
	return GetTickCount() - s_Millis_Start;
}

/**
* @brief Get clock in miliseond
* 
* @return uint64_t Clock number
*/
uint64_t EVE_millis64()
{
	return GetTickCount64() - s_Millis64_Start;
}

#if defined(ESD_SIMULATION)
int Ft_Sleep__ESD(int ms);
#endif

/**
 * @brief Sleep in milisecond
 * 
 * @param ms Milisecond
 */
void EVE_sleep(uint32_t ms)
{
#if defined(ESD_SIMULATION)
	Ft_Sleep__ESD(ms);
#else
	Sleep(ms);
#endif
}

#endif

#ifdef _MSC_VER
#pragma warning(pop)
#endif

/* end of file */
