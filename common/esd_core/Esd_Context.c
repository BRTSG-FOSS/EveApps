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

#include "ESD_Context.h"
#include "ESD_Utility.h"

#include "ESD_GpuAlloc.h"
#include "ESD_Scissor.h"
#include "ESD_BitmapHandle.h"
#include "ESD_TouchTag.h"

//
// Globals
//
ESD_CORE_EXPORT ESD_Context *ESD_CurrentContext = NULL;
ESD_CORE_EXPORT EVE_HalContext *ESD_Host = NULL; // Pointer to current s_Host
ESD_CORE_EXPORT ESD_GpuAlloc *ESD_GAlloc = NULL; // Pointer to current s_GAlloc

//
// External definitions
//
#if _DEBUG
void ESD_CheckTypeSizes();
#endif

// When not in the simulation, use the ESD_Main__Start etc symbols
// as exported by the single Application logic document included
#ifndef ESD_SIMULATION
#ifdef BT8XXEMU_PLATFORM
#define ESD_Main__Running__ESD() BT8XXEMU_isRunning(ESD_Host->Emulator)
#else
#define ESD_Main__Running__ESD() (1)
#endif
#else
int ESD_Main__Running__ESD();
#endif

void ESD_Scissor_DlStart();

// extern void ESD_Widget_ProcessFree(); // TODO: Bind from widgets

// extern void ESD_Timer_CancelGlobal(); // TODO: Bind from widgets
// extern void ESD_Timer_UpdateGlobal(); // TODO: Bind from widgets

//
// Constants
//
#define FT_WELCOME_MESSAGE "Copyright (C) Bridgetek Pte Ltd\n"

void ESD_SetCurrent(ESD_Context *ec)
{
	ESD_CurrentContext = ec;
	ESD_Host = &ec->HalContext;
	ESD_GAlloc = &ec->GpuAlloc;
}

void ESD_Defaults(ESD_Parameters *ep)
{
	memset(ep, 0, sizeof(ESD_Parameters));
}

bool cbCmdWait(struct EVE_HalContext *phost)
{
	ESD_Context *ec = (ESD_Context *)phost->UserContext;

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

void ESD_Initialize()
{
	EVE_Hal_initialize();

#ifdef ESD_SIMULATION
	eve_printf_debug("\f"); // Shows horizontal line in ESD output window
#endif
	eve_printf_debug(FT_WELCOME_MESSAGE);

#if _DEBUG
	ESD_CheckTypeSizes();
#endif
}

void ESD_Open(ESD_Context *ec, ESD_Parameters *ep)
{
	memset(ec, 0, sizeof(ESD_Context));
	ec->ClearColor = 0x212121;
	ec->Start = (void (*)(void *))ep->Start;
	ec->Update = (void (*)(void *))ep->Update;
	ec->Render = (void (*)(void *))ep->Render;
	ec->Idle = (void (*)(void *))ep->Idle;
	ec->End = (void (*)(void *))ep->End;
	ec->UserContext = ep->UserContext;
	ESD_SetCurrent(ec);

	// TODO: Use interactive launch and adjust launch for emulator flash

	EVE_HalParameters parameters;
	EVE_Hal_defaults(&parameters);
	parameters.UserContext = ec;
	parameters.CbCmdWait = cbCmdWait;
	eve_assert_do(EVE_Hal_open(&ec->HalContext, &parameters));
	eve_assert(ec->HalContext.UserContext == ec);

	EVE_HalContext *phost = &ec->HalContext;
	EVE_Util_bootupConfig(phost);

#ifndef ESD_SIMULATION
	// TODO: Store calibration somewhere
	if (!ESD_Calibrate())
	{
		eve_printf_debug("Calibrate failed\n");
	}
#endif

	ec->GpuAlloc.RamGSize = RAM_G_SIZE;
	ESD_GpuAlloc_Reset(&ec->GpuAlloc);

	ESD_BitmapHandle_Initialize();
}

// !!! NOTE:
#define ESD_Release ESD_Close
#define ESD_Shutdown ESD_Release

void ESD_Close(ESD_Context *ec)
{
	// ESD_Widget_ProcessFree(); // TODO: Link this back up!!!
	EVE_Hal_close(&ec->HalContext);
	memset(ec, 0, sizeof(ESD_Context));

	ESD_CurrentContext = NULL;
	ESD_Host = NULL;
	ESD_GAlloc = NULL;
}

void ESD_Release()
{
	EVE_Hal_release();
}

void ESD_Loop(ESD_Context *ec)
{
	ESD_SetCurrent(ec);
	EVE_HalContext *phost = &ec->HalContext;
	(void)phost;

	if (!ESD_Main__Running__ESD() || ec->RequestStop)
		return;

	ESD_Start(ec);

	while (ESD_Main__Running__ESD() && !ec->RequestStop)
	{
		ESD_Update(ec);
		ESD_Render(ec);
		ESD_WaitSwap(ec);
	}

	ESD_Stop(ec);
}

void ESD_Start(ESD_Context *ec)
{
	ESD_SetCurrent(ec);

	// Initialize framework
	ESD_Scissor_DlStart();
	ec->Frame = 0;
	ec->Millis = EVE_millis();
	// ESD_Timer_CancelGlobal(); // TODO

	// Initialize storage
	EVE_Util_loadSdCard(&ec->HalContext);
#if defined(EVE_FLASH_AVAILABLE)
	ESD_AttachFlashFast();
#endif

	// Initialize application
	if (ec->Start)
		ec->Start(ec->UserContext);
}

void ESD_Update(ESD_Context *ec)
{
	ESD_SetCurrent(ec);
	EVE_HalContext *phost = &ec->HalContext;

	// Restore initial frame values
	// EVE_CoCmd_loadIdentity(phost); // ?
	ESD_Scissor_DlStart();
	// ESD_Widget_ProcessFree(); // TODO: Link this back up!!!
	ESD_BitmapHandle_FrameStart(&ec->HandleState);

	if (ec->ShowLogo)
		return;
	if (ec->ShowingLogo)
		ec->Millis = EVE_millis(); // Reset time

	// Verify initialization
	EVE_Util_loadSdCard(phost); // Does this really need phost???
#if defined(EVE_FLASH_AVAILABLE)
	ESD_AttachFlashFast();
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
	uint32_t ms = EVE_millis(); // Calculate frame time delta
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

void ESD_Render(ESD_Context *ec)
{
	ESD_SetCurrent(ec);
	EVE_HalContext *phost = &ec->HalContext;

	if (ec->ShowLogo)
	{
		ec->ShowLogo = false;
		ec->ShowingLogo = true;
		ESD_BeginLogo();
		return;
	}

	if (ec->ShowingLogo)
	{
		ec->ShowingLogo = false;
		ESD_EndLogo();
	}

	// Process all coprocessor commands
	ec->LoopState = ESD_LOOPSTATE_RENDER;

	EVE_CoCmd_dl(phost, CMD_DLSTART);
	EVE_CoCmd_dl(phost, (2UL << 24) | ec->ClearColor); // Set CLEAR_COLOR_RGB from user var
	EVE_CoCmd_dl(phost, CLEAR_TAG(255)); // Always default to 255, so no touch = 0, touch non-tag = 255
	EVE_CoCmd_dl(phost, CLEAR(1, 1, 1));
	if (ec->Render)
		ec->Render(ec->UserContext);

	if (ec->SpinnerPopup)
	{
		// Spinner used for switching longer loading pages with bitmaps etc
		EVE_CoDl_colorRgb_ex(phost, ~(ec->ClearColor));
		// FIXME 2020 JUN 04: Spinner with ownership -- ESD_CoCmd_spinner(ESD_Update, phost->Width >> 1, phost->Height >> 1, 0, 0);
		ec->SpinnerPopup = false;
		ec->SpinnerPopped = true;
	}
	else if (ec->SpinnerPopped)
	{
		// FIXME 2020 JUN 04: Spinner with ownership -- ESD_CoCmd_stop(ESD_Update);
		ec->SpinnerPopped = false;
	}

	EVE_CoCmd_dl(phost, DISPLAY());
	EVE_CoCmd_swap(phost);

	// Replacement for Ft_Gpu_Hal_WaitCmdfifo_empty(phost); with idle function
	ec->LoopState = ESD_LOOPSTATE_IDLE;

	// Advance frame count
	++ec->Frame;
}

bool ESD_WaitSwap(ESD_Context *ec)
{
	ESD_SetCurrent(ec);
	EVE_HalContext *phost = &ec->HalContext;
	(void)phost;

	ec->SwapIdled = false;
	EVE_Cmd_waitFlush(&ec->HalContext);

	/* Reset the coprocessor in case of fault */
	if (ec->HalContext.CmdFault)
	{
		/* TODO: Create a utility function that resets the coprocessor and all cached state */
		EVE_Util_resetCoprocessor(&ec->HalContext);
		ESD_BitmapHandle_Reset(&ec->HandleState);

#if _DEBUG
		/* Show error for a while */
		EVE_sleep(1000);
#endif

		return false;
	}

	return true;
}

void ESD_Stop(ESD_Context *ec)
{
	ESD_SetCurrent(ec);

	// Cleanup application (generally unreachable)
	ec->LoopState = ESD_LOOPSTATE_NONE;
	// ESD_Timer_CancelGlobal(); // TODO
	if (ec->End)
		ec->End(ec->UserContext);
}

/* end of file */
