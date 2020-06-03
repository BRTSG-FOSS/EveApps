
#include <Ft_Platform.h>
#include "ESD_Utility.h"

#include "Ft_Gpu_Hal.h"
#include "Ft_Gpu.h"
#include "Ft_Esd.h"
#include "ESD_Dl.h"
#include "ESD_GpuAlloc.h"
#include "ESD_TouchTag.h"
#include "ESD_CoCmd.h"
#include "ESD_Core.h"

extern EVE_HalContext *ESD_Host;
extern ESD_GpuAlloc *ESD_GAlloc;

#if defined(EVE_FLASH_AVAILABLE)
#ifndef NDEBUG
static uint32_t s_FlashErrorLast = ~0;
#endif
#endif

//A function to enable spinner when frame is rendered.
ESD_FUNCTION(ESD_Spinner_Popup, DisplayName = "Pop-up Spinner", Category = EsdUtilities)
void ESD_Spinner_Popup()
{
	Esd_CurrentContext->SpinnerPopup = true;
}

#if defined(EVE_FLASH_AVAILABLE)
#ifdef ESD_SIMULATION
extern void Esd_SetFlashStatus__ESD(int status);
extern void Esd_SetFlashSize__ESD(int size);
#else
#define Esd_SetFlashStatus__ESD(status) \
	do                                  \
	{                                   \
	} while (false)
#define Esd_SetFlashSize__ESD(status) \
	do                                \
	{                                 \
	} while (false)
#endif

void Esd_AttachFlashFast()
{
	// Wait for flash status to move on from FLASH_STATUS_INIT
	EVE_HalContext *phost = ESD_Host;
	uint32_t flashStatus;
	while (!(flashStatus = Ft_Gpu_Hal_Rd32(phost, REG_FLASH_STATUS)))
	{
#ifndef NDEBUG
		eve_printf_debug("Waiting for REG_FLASH_STATUS (%u)\n", flashStatus);
#endif
	}

	// No need to continue if flash is okay
	if (flashStatus < FLASH_STATUS_FULL)
	{
		uint32_t error;

		Esd_SetFlashStatus__ESD(flashStatus);

		flashStatus = Ft_Gpu_CoCmd_FlashAttach(phost);
		Esd_SetFlashStatus__ESD(flashStatus);
		Esd_SetFlashSize__ESD(Ft_Gpu_Hal_Rd32(phost, REG_FLASH_SIZE));

		flashStatus = Ft_Gpu_CoCmd_FlashFast(phost, &error);
		Esd_SetFlashStatus__ESD(flashStatus);

#ifndef NDEBUG
		if (error != s_FlashErrorLast)
		{
			s_FlashErrorLast = error;
			switch (error)
			{
			case 0:
				eve_printf_debug("Flash OK\n");
				break;
			case FLASH_ERROR_NOTATTACHED: // 0xE001 :
				eve_printf_debug("Flash is not attached\n");
				break;
			case FLASH_ERROR_HEADERMISSING: // 0xE002:
				eve_printf_debug("No header detected in sector 0. Is flash blank?\n");
				break;
			case FLASH_ERROR_HEADERCRCFAILED: // 0xE003:
				eve_printf_debug("Sector 0 flash data failed integrity check\n");
				break;
			case FLASH_ERROR_FIRMWAREMISMATCH: // 0xE004:
				eve_printf_debug("Flash device/blob mismatch. Was correct blob loaded?\n");
				break;
			case FLASH_ERROR_FULLSPEEDFAILED: // 0xE005:
				eve_printf_debug("Failed full-speed flash test. Check board wiring\n");
				break;
			default:
				eve_printf_debug("Unknown flash error (%u)\n", error);
			}
		}
#endif
	}
}
#else
#define Esd_AttachFlashFast() eve_noop()
#endif

void Esd_BeginLogo()
{
	EVE_HalContext *phost = ESD_Host;
	ESD_GpuAlloc_Reset(ESD_GAlloc);
	ESD_GpuAlloc_Alloc(ESD_GAlloc, RAM_G_SIZE, 0); // Block allocation
	Ft_Gpu_CoCmd_StartFrame(phost);
	Ft_Gpu_CoCmd_DlStart(phost);
	Ft_Gpu_CoCmd_SendCmd(phost, CLEAR_COLOR_RGB(255, 255, 255));
	Ft_Gpu_CoCmd_SendCmd(phost, CLEAR(1, 0, 0));
	Ft_Gpu_CoCmd_SendCmd(phost, DISPLAY());
	Ft_Gpu_CoCmd_Swap(phost);
	Ft_Gpu_CoCmd_DlStart(phost);
	Ft_Gpu_CoCmd_SendCmd(phost, CLEAR_COLOR_RGB(255, 255, 255));
	Ft_Gpu_CoCmd_SendCmd(phost, CLEAR(1, 0, 0));
	Ft_Gpu_CoCmd_SendCmd(phost, DISPLAY());
	Ft_Gpu_CoCmd_Swap(phost);
	Ft_Gpu_CoCmd_DlStart(phost);
	Ft_Gpu_CoCmd_SendCmd(phost, CLEAR_COLOR_RGB(255, 255, 255));
	Ft_Gpu_CoCmd_SendCmd(phost, CLEAR(1, 0, 0));
	Ft_Gpu_CoCmd_SendCmd(phost, DISPLAY());
	// Ft_Gpu_CoCmd_MemSet(ESD_Host, 0, 0xFF, RAM_G_SIZE);
	Ft_Gpu_CoCmd_EndFrame(phost);
	Ft_Gpu_Hal_WaitCmdFifoEmpty(phost);
	Ft_Gpu_CoCmd_StartFrame(phost);
	Ft_Gpu_CoCmd_Logo(phost);
	Ft_Gpu_CoCmd_EndFrame(phost);
	Ft_Gpu_Hal_WaitLogo_Finish(phost);
	EVE_sleep(3000);
}

void Esd_EndLogo()
{
	Ft_Gpu_CoCmd_StartFrame(ESD_Host);
	Ft_Gpu_CoCmd_DlStart(ESD_Host);
	Ft_Gpu_CoCmd_SendCmd(ESD_Host, CLEAR_COLOR_RGB(255, 255, 255));
	Ft_Gpu_CoCmd_SendCmd(ESD_Host, CLEAR(1, 0, 0));
	Ft_Gpu_CoCmd_SendCmd(ESD_Host, DISPLAY());
	Ft_Gpu_CoCmd_Swap(ESD_Host);
	Ft_Gpu_CoCmd_EndFrame(ESD_Host);
	Ft_Gpu_Hal_WaitCmdFifoEmpty(ESD_Host);
	ESD_GpuAlloc_Reset(ESD_GAlloc);
}

void Esd_ShowLogo()
{
	Esd_CurrentContext->ShowLogo = true;
}

/// Run calibrate procedure
bool Esd_Calibrate()
{
	EVE_HalContext *phost = ESD_Host;
	uint32_t result;
	uint32_t transMatrix[6];

#if defined(EVE_SCREEN_CAPACITIVE)
	Ft_Gpu_Hal_Wr8(phost, REG_CTOUCH_EXTENDED, CTOUCH_MODE_COMPATIBILITY);
#endif

	eve_printf_debug("App_CoPro_Widget_Calibrate: Start Frame\n");
	Ft_Gpu_CoCmd_StartFrame(phost);

	Ft_Gpu_CoCmd_DlStart(phost);
	Ft_Gpu_CoCmd_SendCmd(phost, CLEAR_COLOR_RGB(64, 64, 64));
	Ft_Gpu_CoCmd_SendCmd(phost, CLEAR(1, 1, 1));
	Ft_Gpu_CoCmd_SendCmd(phost, COLOR_RGB(0xff, 0xff, 0xff));

	// Ft_Gpu_CoCmd_Text(phost, (ESD_Host->Parameters.Display.Width / 2), (ESD_Host->Parameters.Display.Height / 2), 27, OPT_CENTER, "Please Tap on the dot");

	result = Ft_Gpu_CoCmd_Calibrate(phost);

	eve_printf_debug("App_CoPro_Widget_Calibrate: End Frame\n");
	Ft_Gpu_CoCmd_EndFrame(phost);

	// Print the configured values
	Ft_Gpu_Hal_RdMem(phost, REG_TOUCH_TRANSFORM_A, (uint8_t *)transMatrix, 4 * 6); //read all the 6 coefficients
	eve_printf_debug("Touch screen transform values are A 0x%lx,B 0x%lx,C 0x%lx,D 0x%lx,E 0x%lx, F 0x%lx\n",
	    transMatrix[0], transMatrix[1], transMatrix[2], transMatrix[3], transMatrix[4], transMatrix[5]);

	return result != 0;
}

/* end of file */
