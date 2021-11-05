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

#include "EVE_HalImpl.h"
#include "EVE_Platform.h"
#if defined(EVE_MULTI_PLATFORM_TARGET)

static size_t s_TotalDeviceCount = 0;
static size_t s_DeviceCountBT8XXEMU = 0;
static size_t s_DeviceCountMPSSE = 0;
static size_t s_DeviceCountFT4222 = 0;

DWORD g_NumDevsD2XX = 0;

/*********
** INIT **
*********/

/* Initialize HAL platform */
void EVE_HalImpl_BT8XXEMU_initialize();
void EVE_HalImpl_FT4222_initialize();
void EVE_HalImpl_MPSSE_initialize();
void EVE_HalImpl_initialize()
{
	EVE_HalImpl_BT8XXEMU_initialize();
	EVE_HalImpl_MPSSE_initialize();
	EVE_HalImpl_FT4222_initialize();
}

/* Release HAL platform */
void EVE_HalImpl_BT8XXEMU_release();
void EVE_HalImpl_FT4222_release();
void EVE_HalImpl_MPSSE_release();
void EVE_HalImpl_release()
{
	EVE_HalImpl_FT4222_release();
	EVE_HalImpl_MPSSE_release();
	EVE_HalImpl_BT8XXEMU_release();
}

size_t EVE_Hal_BT8XXEMU_list();
size_t EVE_Hal_FT4222_list();
size_t EVE_Hal_MPSSE_list();
EVE_HAL_EXPORT size_t EVE_Hal_list()
{
	s_DeviceCountBT8XXEMU = EVE_Hal_BT8XXEMU_list();
	s_DeviceCountMPSSE = EVE_Hal_MPSSE_list();
	s_DeviceCountFT4222 = EVE_Hal_FT4222_list();
	s_TotalDeviceCount = s_DeviceCountBT8XXEMU + s_DeviceCountMPSSE + s_DeviceCountFT4222;
	return s_TotalDeviceCount;
}

void EVE_Hal_BT8XXEMU_info(EVE_DeviceInfo *deviceInfo, size_t deviceIdx);
void EVE_Hal_FT4222_info(EVE_DeviceInfo *deviceInfo, size_t deviceIdx);
void EVE_Hal_MPSSE_info(EVE_DeviceInfo *deviceInfo, size_t deviceIdx);
EVE_HAL_EXPORT void EVE_Hal_info(EVE_DeviceInfo *deviceInfo, size_t deviceIdx)
{
	if (deviceIdx < s_DeviceCountBT8XXEMU)
	{
		EVE_Hal_BT8XXEMU_info(deviceInfo, deviceIdx);
	}
	else if (deviceIdx < s_DeviceCountBT8XXEMU + s_DeviceCountMPSSE)
	{
		EVE_Hal_MPSSE_info(deviceInfo, deviceIdx - s_DeviceCountBT8XXEMU);
	}
	else if (deviceIdx < s_DeviceCountBT8XXEMU + s_DeviceCountMPSSE + s_DeviceCountFT4222)
	{
		EVE_Hal_FT4222_info(deviceInfo, deviceIdx - s_DeviceCountBT8XXEMU - s_DeviceCountMPSSE);
	}
	else
	{
		memset(deviceInfo, 0, sizeof(EVE_DeviceInfo));
	}
}

/* Check whether the context is the specified device */
bool EVE_Hal_BT8XXEMU_isDevice(EVE_HalContext *phost, size_t deviceIdx);
bool EVE_Hal_FT4222_isDevice(EVE_HalContext *phost, size_t deviceIdx);
bool EVE_Hal_MPSSE_isDevice(EVE_HalContext *phost, size_t deviceIdx);
EVE_HAL_EXPORT bool EVE_Hal_isDevice(EVE_HalContext *phost, size_t deviceIdx)
{
	if (deviceIdx < s_DeviceCountBT8XXEMU)
	{
		return EVE_Hal_BT8XXEMU_isDevice(phost, deviceIdx);
	}
	else if (deviceIdx < s_DeviceCountBT8XXEMU + s_DeviceCountMPSSE)
	{
		return EVE_Hal_MPSSE_isDevice(phost, deviceIdx - s_DeviceCountBT8XXEMU);
	}
	else
	{
		return EVE_Hal_FT4222_isDevice(phost, deviceIdx - s_DeviceCountBT8XXEMU - s_DeviceCountMPSSE);
	}
}

/* Get the default configuration parameters */
bool EVE_HalImpl_BT8XXEMU_defaults(EVE_HalParameters *parameters, size_t deviceIdx);
bool EVE_HalImpl_FT4222_defaults(EVE_HalParameters *parameters, size_t deviceIdx);
bool EVE_HalImpl_MPSSE_defaults(EVE_HalParameters *parameters, size_t deviceIdx);
bool EVE_HalImpl_defaults(EVE_HalParameters *parameters, size_t deviceIdx)
{
	bool res;
	if (deviceIdx < s_DeviceCountBT8XXEMU)
	{
		res = EVE_HalImpl_BT8XXEMU_defaults(parameters, deviceIdx);
		parameters->Host = EVE_HOST_BT8XXEMU;
	}
	else if (deviceIdx < s_DeviceCountBT8XXEMU + s_DeviceCountMPSSE)
	{
		res = EVE_HalImpl_MPSSE_defaults(parameters, deviceIdx - s_DeviceCountBT8XXEMU);
		parameters->Host = EVE_HOST_MPSSE;
	}
	else if (deviceIdx < s_DeviceCountBT8XXEMU + s_DeviceCountMPSSE + s_DeviceCountFT4222)
	{
		res = EVE_HalImpl_FT4222_defaults(parameters, deviceIdx - s_DeviceCountBT8XXEMU - s_DeviceCountMPSSE);
		parameters->Host = EVE_HOST_FT4222;
	}
	else if (res = EVE_HalImpl_FT4222_defaults(parameters, deviceIdx - s_DeviceCountBT8XXEMU - s_DeviceCountMPSSE))
	{
		parameters->Host = EVE_HOST_FT4222;
	}
	else if (res = EVE_HalImpl_MPSSE_defaults(parameters, deviceIdx - s_DeviceCountBT8XXEMU))
	{
		parameters->Host = EVE_HOST_MPSSE;
	}
	else if (res = EVE_HalImpl_BT8XXEMU_defaults(parameters, deviceIdx))
	{
		parameters->Host = EVE_HOST_BT8XXEMU;
	}
	return res;
}

/* Opens a new HAL context using the specified parameters */
bool EVE_HalImpl_BT8XXEMU_open(EVE_HalContext *phost, const EVE_HalParameters *parameters);
bool EVE_HalImpl_FT4222_open(EVE_HalContext *phost, const EVE_HalParameters *parameters);
bool EVE_HalImpl_MPSSE_open(EVE_HalContext *phost, const EVE_HalParameters *parameters);
bool EVE_HalImpl_open(EVE_HalContext *phost, const EVE_HalParameters *parameters)
{
	bool res;
	switch (parameters->Host)
	{
	case EVE_HOST_BT8XXEMU:
		res = EVE_HalImpl_BT8XXEMU_open(phost, parameters);
		break;
	case EVE_HOST_FT4222:
		res = EVE_HalImpl_FT4222_open(phost, parameters);
		break;
	case EVE_HOST_MPSSE:
		res = EVE_HalImpl_MPSSE_open(phost, parameters);
		break;
	default:
		res = false;
		break;
	}
	phost->Host = parameters->Host;
	return res;
}

/* Close a HAL context */
void EVE_HalImpl_BT8XXEMU_close(EVE_HalContext *phost);
void EVE_HalImpl_FT4222_close(EVE_HalContext *phost);
void EVE_HalImpl_MPSSE_close(EVE_HalContext *phost);
void EVE_HalImpl_close(EVE_HalContext *phost)
{
	switch (phost->Host)
	{
	case EVE_HOST_BT8XXEMU:
		EVE_HalImpl_BT8XXEMU_close(phost);
		break;
	case EVE_HOST_FT4222:
		EVE_HalImpl_FT4222_close(phost);
		break;
	case EVE_HOST_MPSSE:
		EVE_HalImpl_MPSSE_close(phost);
		break;
	}
}

/* Idle. Call regularly to update frequently changing internal state */
void EVE_HalImpl_BT8XXEMU_idle(EVE_HalContext *phost);
void EVE_HalImpl_FT4222_idle(EVE_HalContext *phost);
void EVE_HalImpl_MPSSE_idle(EVE_HalContext *phost);
void EVE_HalImpl_idle(EVE_HalContext *phost)
{
	switch (phost->Host)
	{
	case EVE_HOST_BT8XXEMU:
		EVE_HalImpl_BT8XXEMU_idle(phost);
		break;
	case EVE_HOST_FT4222:
		EVE_HalImpl_FT4222_idle(phost);
		break;
	case EVE_HOST_MPSSE:
		EVE_HalImpl_MPSSE_idle(phost);
		break;
	}
}

/*************
** TRANSFER **
*************/

void EVE_Hal_BT8XXEMU_startTransfer(EVE_HalContext *phost, EVE_TRANSFER_T rw, uint32_t addr);
void EVE_Hal_FT4222_startTransfer(EVE_HalContext *phost, EVE_TRANSFER_T rw, uint32_t addr);
void EVE_Hal_MPSSE_startTransfer(EVE_HalContext *phost, EVE_TRANSFER_T rw, uint32_t addr);
EVE_HAL_EXPORT void EVE_Hal_startTransfer(EVE_HalContext *phost, EVE_TRANSFER_T rw, uint32_t addr)
{
	switch (phost->Host)
	{
	case EVE_HOST_BT8XXEMU:
		EVE_Hal_BT8XXEMU_startTransfer(phost, rw, addr);
		break;
	case EVE_HOST_FT4222:
		EVE_Hal_FT4222_startTransfer(phost, rw, addr);
		break;
	case EVE_HOST_MPSSE:
		EVE_Hal_MPSSE_startTransfer(phost, rw, addr);
		break;
	}
}

void EVE_Hal_BT8XXEMU_endTransfer(EVE_HalContext *phost);
void EVE_Hal_FT4222_endTransfer(EVE_HalContext *phost);
void EVE_Hal_MPSSE_endTransfer(EVE_HalContext *phost);
EVE_HAL_EXPORT void EVE_Hal_endTransfer(EVE_HalContext *phost)
{
	switch (phost->Host)
	{
	case EVE_HOST_BT8XXEMU:
		EVE_Hal_BT8XXEMU_endTransfer(phost);
		break;
	case EVE_HOST_FT4222:
		EVE_Hal_FT4222_endTransfer(phost);
		break;
	case EVE_HOST_MPSSE:
		EVE_Hal_MPSSE_endTransfer(phost);
		break;
	}
}

void EVE_Hal_BT8XXEMU_flush(EVE_HalContext *phost);
void EVE_Hal_FT4222_flush(EVE_HalContext *phost);
void EVE_Hal_MPSSE_flush(EVE_HalContext *phost);
EVE_HAL_EXPORT void EVE_Hal_flush(EVE_HalContext *phost)
{
	switch (phost->Host)
	{
	case EVE_HOST_BT8XXEMU:
		EVE_Hal_BT8XXEMU_flush(phost);
		break;
	case EVE_HOST_FT4222:
		EVE_Hal_FT4222_flush(phost);
		break;
	case EVE_HOST_MPSSE:
		EVE_Hal_MPSSE_flush(phost);
		break;
	}
}

uint8_t EVE_Hal_BT8XXEMU_transfer8(EVE_HalContext *phost, uint8_t value);
uint8_t EVE_Hal_FT4222_transfer8(EVE_HalContext *phost, uint8_t value);
uint8_t EVE_Hal_MPSSE_transfer8(EVE_HalContext *phost, uint8_t value);
EVE_HAL_EXPORT uint8_t EVE_Hal_transfer8(EVE_HalContext *phost, uint8_t value)
{
	switch (phost->Host)
	{
	case EVE_HOST_BT8XXEMU:
		return EVE_Hal_BT8XXEMU_transfer8(phost, value);
	case EVE_HOST_FT4222:
		return EVE_Hal_FT4222_transfer8(phost, value);
	case EVE_HOST_MPSSE:
		return EVE_Hal_MPSSE_transfer8(phost, value);
	}
	return 0;
}

uint16_t EVE_Hal_BT8XXEMU_transfer16(EVE_HalContext *phost, uint16_t value);
uint16_t EVE_Hal_FT4222_transfer16(EVE_HalContext *phost, uint16_t value);
uint16_t EVE_Hal_MPSSE_transfer16(EVE_HalContext *phost, uint16_t value);
EVE_HAL_EXPORT uint16_t EVE_Hal_transfer16(EVE_HalContext *phost, uint16_t value)
{
	switch (phost->Host)
	{
	case EVE_HOST_BT8XXEMU:
		return EVE_Hal_BT8XXEMU_transfer16(phost, value);
	case EVE_HOST_FT4222:
		return EVE_Hal_FT4222_transfer16(phost, value);
	case EVE_HOST_MPSSE:
		return EVE_Hal_MPSSE_transfer16(phost, value);
	}
	return 0;
}

uint32_t EVE_Hal_BT8XXEMU_transfer32(EVE_HalContext *phost, uint32_t value);
uint32_t EVE_Hal_FT4222_transfer32(EVE_HalContext *phost, uint32_t value);
uint32_t EVE_Hal_MPSSE_transfer32(EVE_HalContext *phost, uint32_t value);
EVE_HAL_EXPORT uint32_t EVE_Hal_transfer32(EVE_HalContext *phost, uint32_t value)
{
	switch (phost->Host)
	{
	case EVE_HOST_BT8XXEMU:
		return EVE_Hal_BT8XXEMU_transfer32(phost, value);
	case EVE_HOST_FT4222:
		return EVE_Hal_FT4222_transfer32(phost, value);
	case EVE_HOST_MPSSE:
		return EVE_Hal_MPSSE_transfer32(phost, value);
	}
	return 0;
}

void EVE_Hal_BT8XXEMU_transferMem(EVE_HalContext *phost, uint8_t *result, const uint8_t *buffer, uint32_t size);
void EVE_Hal_FT4222_transferMem(EVE_HalContext *phost, uint8_t *result, const uint8_t *buffer, uint32_t size);
void EVE_Hal_MPSSE_transferMem(EVE_HalContext *phost, uint8_t *result, const uint8_t *buffer, uint32_t size);
EVE_HAL_EXPORT void EVE_Hal_transferMem(EVE_HalContext *phost, uint8_t *result, const uint8_t *buffer, uint32_t size)
{
	switch (phost->Host)
	{
	case EVE_HOST_BT8XXEMU:
		EVE_Hal_BT8XXEMU_transferMem(phost, result, buffer, size);
		break;
	case EVE_HOST_FT4222:
		EVE_Hal_FT4222_transferMem(phost, result, buffer, size);
		break;
	case EVE_HOST_MPSSE:
		EVE_Hal_MPSSE_transferMem(phost, result, buffer, size);
		break;
	}
}

void EVE_Hal_BT8XXEMU_transferProgMem(EVE_HalContext *phost, uint8_t *result, eve_progmem_const uint8_t *buffer, uint32_t size);
void EVE_Hal_FT4222_transferProgMem(EVE_HalContext *phost, uint8_t *result, eve_progmem_const uint8_t *buffer, uint32_t size);
void EVE_Hal_MPSSE_transferProgMem(EVE_HalContext *phost, uint8_t *result, eve_progmem_const uint8_t *buffer, uint32_t size);
EVE_HAL_EXPORT void EVE_Hal_transferProgMem(EVE_HalContext *phost, uint8_t *result, eve_progmem_const uint8_t *buffer, uint32_t size)
{
	switch (phost->Host)
	{
	case EVE_HOST_BT8XXEMU:
		EVE_Hal_BT8XXEMU_transferProgMem(phost, result, buffer, size);
		break;
	case EVE_HOST_FT4222:
		EVE_Hal_FT4222_transferProgMem(phost, result, buffer, size);
		break;
	case EVE_HOST_MPSSE:
		EVE_Hal_MPSSE_transferProgMem(phost, result, buffer, size);
		break;
	}
}

uint32_t EVE_Hal_BT8XXEMU_transferString(EVE_HalContext *phost, const char *str, uint32_t index, uint32_t size, uint32_t padMask);
uint32_t EVE_Hal_FT4222_transferString(EVE_HalContext *phost, const char *str, uint32_t index, uint32_t size, uint32_t padMask);
uint32_t EVE_Hal_MPSSE_transferString(EVE_HalContext *phost, const char *str, uint32_t index, uint32_t size, uint32_t padMask);
EVE_HAL_EXPORT uint32_t EVE_Hal_transferString(EVE_HalContext *phost, const char *str, uint32_t index, uint32_t size, uint32_t padMask)
{
	switch (phost->Host)
	{
	case EVE_HOST_BT8XXEMU:
		return EVE_Hal_BT8XXEMU_transferString(phost, str, index, size, padMask);
	case EVE_HOST_FT4222:
		return EVE_Hal_FT4222_transferString(phost, str, index, size, padMask);
	case EVE_HOST_MPSSE:
		return EVE_Hal_MPSSE_transferString(phost, str, index, size, padMask);
	}
	return 0;
}

/************
** UTILITY **
************/

void EVE_Hal_BT8XXEMU_hostCommand(EVE_HalContext *phost, uint8_t cmd);
void EVE_Hal_FT4222_hostCommand(EVE_HalContext *phost, uint8_t cmd);
void EVE_Hal_MPSSE_hostCommand(EVE_HalContext *phost, uint8_t cmd);
EVE_HAL_EXPORT void EVE_Hal_hostCommand(EVE_HalContext *phost, uint8_t cmd)
{
	switch (phost->Host)
	{
	case EVE_HOST_BT8XXEMU:
		EVE_Hal_BT8XXEMU_hostCommand(phost, cmd);
		break;
	case EVE_HOST_FT4222:
		EVE_Hal_FT4222_hostCommand(phost, cmd);
		break;
	case EVE_HOST_MPSSE:
		EVE_Hal_MPSSE_hostCommand(phost, cmd);
		break;
	}
}

/* This API sends a 3byte command to the phost */
void EVE_Hal_BT8XXEMU_hostCommandExt3(EVE_HalContext *phost, uint8_t cmd);
void EVE_Hal_FT4222_hostCommandExt3(EVE_HalContext *phost, uint8_t cmd);
void EVE_Hal_MPSSE_hostCommandExt3(EVE_HalContext *phost, uint8_t cmd);
EVE_HAL_EXPORT void EVE_Hal_hostCommandExt3(EVE_HalContext *phost, uint32_t cmd)
{
	switch (phost->Host)
	{
	case EVE_HOST_BT8XXEMU:
		EVE_Hal_BT8XXEMU_hostCommandExt3(phost, cmd);
		break;
	case EVE_HOST_FT4222:
		EVE_Hal_FT4222_hostCommandExt3(phost, cmd);
		break;
	case EVE_HOST_MPSSE:
		EVE_Hal_MPSSE_hostCommandExt3(phost, cmd);
		break;
	}
}

/* Toggle PD_N pin of FT800 board for a power cycle */
bool EVE_Hal_BT8XXEMU_powerCycle(EVE_HalContext *phost, bool up);
bool EVE_Hal_FT4222_powerCycle(EVE_HalContext *phost, bool up);
bool EVE_Hal_MPSSE_powerCycle(EVE_HalContext *phost, bool up);
EVE_HAL_EXPORT bool EVE_Hal_powerCycle(EVE_HalContext *phost, bool up)
{
	switch (phost->Host)
	{
	case EVE_HOST_BT8XXEMU:
		return EVE_Hal_BT8XXEMU_powerCycle(phost, up);
		break;
	case EVE_HOST_FT4222:
		return EVE_Hal_FT4222_powerCycle(phost, up);
		break;
	case EVE_HOST_MPSSE:
		return EVE_Hal_MPSSE_powerCycle(phost, up);
		break;
	}
	return false;
}

void EVE_Hal_BT8XXEMU_setSPI(EVE_HalContext *phost, EVE_SPI_CHANNELS_T numchnls, uint8_t numdummy);
void EVE_Hal_FT4222_setSPI(EVE_HalContext *phost, EVE_SPI_CHANNELS_T numchnls, uint8_t numdummy);
void EVE_Hal_MPSSE_setSPI(EVE_HalContext *phost, EVE_SPI_CHANNELS_T numchnls, uint8_t numdummy);
EVE_HAL_EXPORT void EVE_Hal_setSPI(EVE_HalContext *phost, EVE_SPI_CHANNELS_T numchnls, uint8_t numdummy)
{
	switch (phost->Host)
	{
	case EVE_HOST_BT8XXEMU:
		EVE_Hal_BT8XXEMU_setSPI(phost, numchnls, numdummy);
		break;
	case EVE_HOST_FT4222:
		EVE_Hal_FT4222_setSPI(phost, numchnls, numdummy);
		break;
	case EVE_HOST_MPSSE:
		EVE_Hal_MPSSE_setSPI(phost, numchnls, numdummy);
		break;
	}
}

void EVE_Hal_BT8XXEMU_restoreSPI(EVE_HalContext *phost);
void EVE_Hal_FT4222_restoreSPI(EVE_HalContext *phost);
void EVE_Hal_MPSSE_restoreSPI(EVE_HalContext *phost);
EVE_HAL_EXPORT void EVE_Hal_restoreSPI(EVE_HalContext *phost)
{
	switch (phost->Host)
	{
	case EVE_HOST_BT8XXEMU:
		EVE_Hal_BT8XXEMU_restoreSPI(phost);
		break;
	case EVE_HOST_FT4222:
		EVE_Hal_FT4222_restoreSPI(phost);
		break;
	case EVE_HOST_MPSSE:
		EVE_Hal_MPSSE_restoreSPI(phost);
		break;
	}
}

/*********
** MISC **
*********/

bool EVE_UtilImpl_BT8XXEMU_bootupDisplayGpio(EVE_HalContext *phost);
bool EVE_UtilImpl_FT4222_bootupDisplayGpio(EVE_HalContext *phost);
bool EVE_UtilImpl_MPSSE_bootupDisplayGpio(EVE_HalContext *phost);
bool EVE_UtilImpl_bootupDisplayGpio(EVE_HalContext *phost)
{
	switch (phost->Host)
	{
	case EVE_HOST_BT8XXEMU:
		return EVE_UtilImpl_BT8XXEMU_bootupDisplayGpio(phost);
	case EVE_HOST_FT4222:
		return EVE_UtilImpl_FT4222_bootupDisplayGpio(phost);
	case EVE_HOST_MPSSE:
		return EVE_UtilImpl_MPSSE_bootupDisplayGpio(phost);
	}
	return false;
}

#endif /* #if defined(BT8XXEMU_PLATFORM) */

/* end of file */
