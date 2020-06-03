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

#include "ESD_Core.h"
#include "ESD_Utility.h"

#include "ESD_GpuAlloc.h"
#include "ESD_Dl.h"
#include "ESD_CoCmd.h"
#include "ESD_BitmapHandle.h"
#include "ESD_TouchTag.h"

//
// Globals
//
Esd_Context *Esd_CurrentContext = NULL;
EVE_HalContext *ESD_Host = NULL; // Pointer to current s_Host
ESD_GpuAlloc *ESD_GAlloc = NULL; // Pointer to current s_GAlloc
int16_t ESD_DispWidth, ESD_DispHeight;

//
// External definitions
//
// TODO: Decouple global state
#if _DEBUG
void Esd_CheckTypeSizes();
#endif
// FT_Mcu_Hal.c
// void Eve_BootupConfig(EVE_HalContext *s_Host);
#define Eve_BootupConfig EVE_Util_bootupConfig

// When not in the simulation, use the Ft_Main__Start etc symbols
// as exported by the single Application logic document included
#ifndef ESD_SIMULATION
#ifdef BT8XXEMU_PLATFORM
#define Ft_Main__Running__ESD() BT8XXEMU_isRunning(ESD_Host->Emulator)
#else
#define Ft_Main__Running__ESD() (1)
#endif
#else
int Ft_Main__Running__ESD();
#endif

void Esd_ResetGpuState();
void Esd_ResetCoState(); // TODO: Call after coprocessor reset procedure

// extern void ESD_Widget_ProcessFree(); // TODO: Bind from widgets

// extern void ESD_Timer_CancelGlobal(); // TODO: Bind from widgets
// extern void ESD_Timer_UpdateGlobal(); // TODO: Bind from widgets

//
// Constants
//
#define FT_WELCOME_MESSAGE "Copyright (C) Bridgetek Pte Ltd\n"

void Esd_SetCurrent(Esd_Context *ec)
{
	Esd_CurrentContext = ec;
	ESD_Host = &ec->HalContext;
	ESD_GAlloc = &ec->GpuAlloc;
}

void Esd_Defaults(Esd_Parameters *ep)
{
	memset(ep, 0, sizeof(Esd_Parameters));
}

bool cbCmdWait(struct EVE_HalContext *phost)
{
	Esd_Context *ec = (Esd_Context *)phost->UserContext;

	/* Loop an idle task instead of doing nothing */
	if (ec->Idle)
		ec->Idle(ec->UserContext);
	/* EVE_Hal_idle(&ec->HalContext); */ /* Already called by EVE HAL */
	ec->SwapIdled = true;

#if defined(EVE_MULTI_TARGET)
	return (phost->Host == EVE_HOST_BT8XXEMU) ? BT8XXEMU_isRunning(phost->Emulator) : true;
#elif defined(BT8XXEMU_PLATFORM)
	/* TODO: This may be handled by HAL idle function instead */
	return BT8XXEMU_isRunning(phost->Emulator);
#else
	return true;
#endif
}

void Esd_Initialize(Esd_Context *ec, Esd_Parameters *ep)
{
	memset(ec, 0, sizeof(Esd_Context));
	ec->ClearColor = 0x212121;
	ec->Start = (void (*)(void *))ep->Start;
	ec->Update = (void (*)(void *))ep->Update;
	ec->Render = (void (*)(void *))ep->Render;
	ec->Idle = (void (*)(void *))ep->Idle;
	ec->End = (void (*)(void *))ep->End;
	ec->UserContext = ep->UserContext;
	Esd_SetCurrent(ec);

	Ft_Gpu_HalInit_t halInit;
	Ft_Gpu_Hal_Init(&halInit);

#ifdef ESD_SIMULATION
	eve_printf_debug("\f"); // Shows horizontal line in ESD output window
#endif
	eve_printf_debug(FT_WELCOME_MESSAGE);

#if _DEBUG
	Esd_CheckTypeSizes();
#endif

	EVE_HalParameters parameters;
	EVE_Hal_defaults(&parameters);
	parameters.UserContext = ec;
	parameters.CbCmdWait = cbCmdWait;
	eve_assert_do(EVE_Hal_open(&ec->HalContext, &parameters));
	eve_assert(ec->HalContext.UserContext == ec);
	eve_assert(ec->HalContext.Parameters.UserContext == ec);

	EVE_HalContext *phost = &ec->HalContext;
	EVE_Util_bootupConfig(phost);

	ESD_DispWidth = EVE_Hal_rd16(phost, REG_HSIZE);
	ESD_DispHeight = EVE_Hal_rd16(phost, REG_VSIZE);

#ifndef ESD_SIMULATION
	// TODO: Store calibration somewhere!
	if (!Esd_Calibrate())
	{
		eve_printf_debug("Calibrate failed\n");
	}
#endif

	ec->GpuAlloc.RamGSize = RAM_G_SIZE;
	ESD_GpuAlloc_Reset(&ec->GpuAlloc);

	Esd_BitmapHandle_Initialize();
	Esd_ResetCoState();
}

void Esd_Release(Esd_Context *ec)
{
	// ESD_Widget_ProcessFree(); // TODO: Link this back up!!!
	Ft_Gpu_Hal_Close(&ec->HalContext);
	memset(ec, 0, sizeof(Esd_Context));

	Esd_CurrentContext = NULL;
	ESD_Host = NULL;
	ESD_GAlloc = NULL;
}

void Esd_Shutdown()
{
	Ft_Gpu_Hal_DeInit();
}

void Esd_Loop(Esd_Context *ec)
{
	Esd_SetCurrent(ec);
	EVE_HalContext *phost = &ec->HalContext;
	(void)phost;

	if (!Ft_Main__Running__ESD() || ec->RequestStop)
		return;

	Esd_Start(ec);

	while (Ft_Main__Running__ESD() && !ec->RequestStop)
	{
		Esd_Update(ec);
		Esd_Render(ec);
		Esd_WaitSwap(ec);
	}

	Esd_Stop(ec);
}

void Esd_Start(Esd_Context *ec)
{
	Esd_SetCurrent(ec);

	// Initialize framework
	Esd_ResetGpuState();
	ec->Frame = 0;
	ec->Millis = EVE_millis();
	// ESD_Timer_CancelGlobal(); // TODO

	// Initialize storage
	EVE_Util_loadSdCard(&ec->HalContext);
#if defined(EVE_FLASH_AVAILABLE)
	Esd_AttachFlashFast();
#endif

	// Initialize application
	if (ec->Start)
		ec->Start(ec->UserContext);
}

void Esd_Update(Esd_Context *ec)
{
	Esd_SetCurrent(ec);
	EVE_HalContext *phost = &ec->HalContext;

	// Restore initial frame values
	// Ft_Gpu_CoCmd_LoadIdentity(phost); // ?
	Esd_ResetGpuState();
	// ESD_Widget_ProcessFree(); // TODO: Link this back up!!!
	Esd_BitmapHandle_FrameStart(&ec->HandleState);

	if (ec->ShowLogo)
		return;
	if (ec->ShowingLogo)
		ec->Millis = ft_millis(); // Reset time

	// Verify initialization
	Ft_Hal_LoadSDCard();
#if defined(EVE_FLASH_AVAILABLE)
	Esd_AttachFlashFast();
#endif

	// Idle at least once every frame
	ec->LoopState = ESD_LOOPSTATE_IDLE;
	if (!ec->SwapIdled)
	{
		if (ec->Idle)
			ec->Idle(ec->UserContext);
		EVE_Hal_idle(phost);
	}

	// Update GUI state before render
	ec->LoopState = ESD_LOOPSTATE_UPDATE;
	uint32_t ms = ft_millis(); // Calculate frame time delta
	ec->DeltaMs = ms - ec->Millis;
	ec->Millis = ms;
	ESD_GpuAlloc_Update(ESD_GAlloc); // Run GC
	ESD_TouchTag_Update(NULL); // Update touch
	if (ec->Update)
		ec->Update(ec->UserContext);
	// ESD_Timer_UpdateGlobal(); // TODO

	// Return to idle state inbetween
	ec->LoopState = ESD_LOOPSTATE_IDLE;
}

void Esd_Render(Esd_Context *ec)
{
	Esd_SetCurrent(ec);
	EVE_HalContext *phost = &ec->HalContext;

	if (ec->ShowLogo)
	{
		ec->ShowLogo = false;
		ec->ShowingLogo = true;
		Esd_BeginLogo();
		return;
	}

	if (ec->ShowingLogo)
	{
		ec->ShowingLogo = false;
		Esd_EndLogo();
	}

	// Process all coprocessor commands
	ec->LoopState = ESD_LOOPSTATE_RENDER;

	Ft_Gpu_CoCmd_StartFrame(phost);

	Ft_Gpu_CoCmd_SendCmd(phost, CMD_DLSTART);
	Ft_Gpu_CoCmd_SendCmd(phost, (2UL << 24) | ec->ClearColor); // Set CLEAR_COLOR_RGB from user var
	Ft_Gpu_CoCmd_SendCmd(phost, CLEAR_TAG(255)); // Always default to 255, so no touch = 0, touch non-tag = 255
	Ft_Gpu_CoCmd_SendCmd(phost, CLEAR(1, 1, 1));
	if (ec->Render)
		ec->Render(ec->UserContext);

	if (ec->SpinnerPopup)
	{
		// Spinner used for switching longer loading pages with bitmaps etc
		ESD_Dl_COLOR_RGB(~(ec->ClearColor));
		ESD_CoCmd_Spinner(Esd_Update, ESD_DispWidth >> 1, ESD_DispHeight >> 1, 0, 0);
		ec->SpinnerPopup = false;
		ec->SpinnerPopped = true;
	}
	else if (ec->SpinnerPopped)
	{
		ESD_CoCmd_Stop(Esd_Update);
		ec->SpinnerPopped = false;
	}

	Ft_Gpu_CoCmd_SendCmd(phost, DISPLAY());
	Ft_Gpu_CoCmd_Swap(phost);

	Ft_Gpu_CoCmd_EndFrame(phost);

	// Replacement for Ft_Gpu_Hal_WaitCmdfifo_empty(phost); with idle function
	ec->LoopState = ESD_LOOPSTATE_IDLE;

	// Advance frame count
	++ec->Frame;
}

bool Esd_WaitSwap(Esd_Context *ec)
{
	Esd_SetCurrent(ec);
	EVE_HalContext *phost = &ec->HalContext;
	(void)phost;

	ec->SwapIdled = false;
	EVE_Cmd_waitFlush(&ec->HalContext);

	/* Reset the coprocessor in case of fault */
	if (ec->HalContext.CmdFault)
	{
		/* TODO: Create a utility function that resets the coprocessor and all cached state */
		EVE_Util_resetCoprocessor(&ec->HalContext);
		Esd_ResetCoState();
		Esd_BitmapHandle_Reset(&ec->HandleState);

#if _DEBUG
		/* Show error for a while */
		EVE_sleep(1000);
#endif

		return false;
	}

	return true;
}

void Esd_Stop(Esd_Context *ec)
{
	Esd_SetCurrent(ec);

	// Cleanup application (generally unreachable)
	ec->LoopState = ESD_LOOPSTATE_NONE;
	// ESD_Timer_CancelGlobal(); // TODO
	if (ec->End)
		ec->End(ec->UserContext);
}

/* end of file */
