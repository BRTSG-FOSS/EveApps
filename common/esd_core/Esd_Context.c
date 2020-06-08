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
#if defined(EVE_MULTI_TARGET)
#define ESD_IsRunning__ESD() (phost->Host == EVE_HOST_BT8XXEMU ? BT8XXEMU_isRunning(phost->Emulator) : true)
#elif defined(BT8XXEMU_PLATFORM)
#define ESD_IsRunning__ESD() BT8XXEMU_isRunning(ESD_Host->Emulator)
#else
#define ESD_IsRunning__ESD() true
#endif
#else
int ESD_IsRunning__ESD();
#endif

void ESD_Scissor_DlStart();

// extern void ESD_Widget_ProcessFree(); // TODO: Bind from widgets

// extern void ESD_Timer_CancelGlobal(); // TODO: Bind from widgets
// extern void ESD_Timer_UpdateGlobal(); // TODO: Bind from widgets

//
// Constants
//
#define FT_WELCOME_MESSAGE "Copyright (C) Bridgetek Pte Ltd\n"

ESD_CORE_EXPORT void ESD_SetCurrent(ESD_Context *ec)
{
	ESD_CurrentContext = ec;
	ESD_Host = &ec->HalContext;
	ESD_GAlloc = &ec->GpuAlloc;
}

ESD_CORE_EXPORT void ESD_Defaults(ESD_Parameters *ep)
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

	return ESD_IsRunning__ESD() && !ec->RequestStop;
}

ESD_CORE_EXPORT void ESD_Initialize()
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

#ifdef ESD_SIMULATION

void ESD_HookOpen__ESD(ESD_Context *ec);
void ESD_HookClose__ESD(ESD_Context *ec);

ESD_Callback ESD_HookStart__ESD(ESD_Context *ec, ESD_Callback start);
ESD_Callback ESD_HookUpdate__ESD(ESD_Context *ec, ESD_Callback update);
ESD_Callback ESD_HookRender__ESD(ESD_Context *ec, ESD_Callback render);
ESD_Callback ESD_HookIdle__ESD(ESD_Context *ec, ESD_Callback idle);
ESD_Callback ESD_HookEnd__ESD(ESD_Context *ec, ESD_Callback end);

#endif

ESD_CORE_EXPORT void ESD_Open(ESD_Context *ec, ESD_Parameters *ep)
{
	EVE_HalContext *phost;
	EVE_CHIPID_T chipId;
	size_t deviceIdx;

	memset(ec, 0, sizeof(ESD_Context));
	ec->ClearColor = 0x212121;
	ec->Start = ep->Start;
	ec->Update = ep->Update;
	ec->Render = ep->Render;
	ec->Idle = ep->Idle;
	ec->End = ep->End;
	ec->UserContext = ep->UserContext;
	ESD_SetCurrent(ec);

#ifdef ESD_SIMULATION
	ESD_HookOpen__ESD(ec);
	ec->Start = ESD_HookStart__ESD(ec, ec->Start);
	ec->Update = ESD_HookStart__ESD(ec, ec->Update);
	ec->Render = ESD_HookStart__ESD(ec, ec->Render);
	ec->Idle = ESD_HookStart__ESD(ec, ec->Idle);
	ec->End = ESD_HookStart__ESD(ec, ec->End);
#endif

#ifdef ESD_SIMULATION
	chipId = EVE_SUPPORT_CHIPID;
	deviceIdx = -1;
#else
	/* Interactive device selection */
	EVE_Util_selectDeviceInteractive(&chipId, &deviceIdx);
#endif

	{
		EVE_HalParameters parameters;
		EVE_Hal_defaultsEx(&parameters, deviceIdx);
		parameters.UserContext = ec;
		parameters.CbCmdWait = cbCmdWait;
		eve_assert_do(EVE_Hal_open(&ec->HalContext, &parameters));
	}

	phost = &ec->HalContext;
#if ESD_SIMULATION
	eve_assert_do(EVE_Util_bootupConfig(phost));
#else
	eve_assert_do(EVE_Util_bootupConfigInteractive(phost, EVE_DISPLAY_DEFAULT));
#endif

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

ESD_CORE_EXPORT void ESD_Close(ESD_Context *ec)
{
	// ESD_Widget_ProcessFree(); // TODO: Link this back up (add ESD_Framework_Initialize and ESD_Framework_Release)
	EVE_Hal_close(&ec->HalContext);
	memset(ec, 0, sizeof(ESD_Context));

#ifdef ESD_SIMULATION
	ESD_HookClose__ESD(ec);
#endif

	ESD_CurrentContext = NULL;
	ESD_Host = NULL;
	ESD_GAlloc = NULL;
}

ESD_CORE_EXPORT void ESD_Release()
{
	EVE_Hal_release();
}

ESD_CORE_EXPORT void ESD_Loop(ESD_Context *ec)
{
	EVE_HalContext *phost = &ec->HalContext;
	ESD_SetCurrent(ec);
	(void)phost;

	if (!ESD_IsRunning__ESD() || ec->RequestStop)
		return;

	ESD_Start(ec);

	while (ESD_IsRunning__ESD() && !ec->RequestStop)
	{
		ESD_Update(ec);
		ESD_Render(ec);
		ESD_WaitSwap(ec);
	}

	ESD_Stop(ec);
}

ESD_CORE_EXPORT void ESD_Start(ESD_Context *ec)
{
	ESD_SetCurrent(ec);

	// Initialize framework
	ESD_Scissor_DlStart();
	ec->Frame = 0;
	ec->Millis = EVE_millis();
	// ESD_Timer_CancelGlobal(); // TODO (in ESD_Framework_Initialize)

	// Initialize storage
	EVE_Util_loadSdCard(&ec->HalContext);
#if defined(EVE_FLASH_AVAILABLE)
	ESD_AttachFlashFast();
#endif

	// Initialize application
	if (ec->Start)
		ec->Start(ec->UserContext);
}

ESD_CORE_EXPORT void ESD_Update(ESD_Context *ec)
{
	uint32_t ms;

	EVE_HalContext *phost = &ec->HalContext;
	ESD_SetCurrent(ec);

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
	ms = EVE_millis(); // Calculate frame time delta
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

ESD_CORE_EXPORT void ESD_Render(ESD_Context *ec)
{
	EVE_HalContext *phost = &ec->HalContext;
	ESD_SetCurrent(ec);

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

	EVE_CoCmd_dlStart(phost);
	EVE_CoDl_clearColorRgb_ex(phost, ec->ClearColor); // Set CLEAR_COLOR_RGB from user var
	EVE_CoDl_clearTag(phost, 255); // Always default to 255, so no touch = 0, touch non-tag = 255
	EVE_CoDl_clear(phost, true, true, true);
	if (ec->Render)
		ec->Render(ec->UserContext);

	if (ec->SpinnerPopup)
	{
		// Spinner used for switching longer loading pages with bitmaps etc
		EVE_CoDl_colorRgb_ex(phost, ~(ec->ClearColor));
		ESD_CoWidget_Spinner(ESD_Update, phost->Width >> 1, phost->Height >> 1, 0, 0);
		ec->SpinnerPopup = false;
		ec->SpinnerPopped = true;
	}
	else if (ec->SpinnerPopped)
	{
		ESD_CoWidget_Stop(ESD_Update);
		ec->SpinnerPopped = false;
	}

	EVE_CoDl_display(phost);
	EVE_CoCmd_swap(phost);

	// Replacement for Ft_Gpu_Hal_WaitCmdfifo_empty(phost); with idle function
	ec->LoopState = ESD_LOOPSTATE_IDLE;

	// Advance frame count
	++ec->Frame;
}

ESD_CORE_EXPORT bool ESD_WaitSwap(ESD_Context *ec)
{
	EVE_HalContext *phost = &ec->HalContext;
	ESD_SetCurrent(ec);
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

ESD_CORE_EXPORT void ESD_Stop(ESD_Context *ec)
{
	ESD_SetCurrent(ec);

	// Cleanup application (generally unreachable)
	ec->LoopState = ESD_LOOPSTATE_NONE;
	// ESD_Timer_CancelGlobal(); // TODO
	if (ec->End)
		ec->End(ec->UserContext);
}

/* Implements a generic main function, for reference only */
/*
int ESD_Main(int argc, char *argv[])
{
	ESD_Initialize();

	ESD_Parameters ep;
	ESD_Defaults(&ep);
	ep.Start = Main_Start;
	ep.Update = Main_Update;
	ep.Render = Main_Render;
	ep.Idle = Main_Idle;
	ep.End = Main_End;
	ESD_Context ec;

	ESD_Open(&ec, &ep);
	ESD_Loop(&ec);
	ESD_Close(&ec);

	ESD_Release();
	return EXIT_SUCCESS;
}
*/

/* end of file */
