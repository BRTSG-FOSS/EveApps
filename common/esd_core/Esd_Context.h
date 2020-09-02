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

#ifndef ESD_CONTEXT___H
#define ESD_CONTEXT___H

#include "Esd_Base.h"
#include "Esd_GpuAlloc.h"
#include "Esd_Scissor.h"
#include "Esd_BitmapHandle.h"
#include "Esd_TouchTag.h"

/* Runtime context of ESD */
typedef struct
{
	EVE_HalContext HalContext; //< Pointer to current s_Host
	Esd_GpuAlloc GpuAlloc; //< Pointer to current s_GAlloc
	uint32_t Millis; //< Time in milliseconds for current frame
	uint32_t DeltaMs; //< Delta time in milliseconds between frames
	uint32_t Frame; //< Number of times Render has been called
	esd_rgb32_t ClearColor; //< Screen clear color (default is 0x212121)
	uint8_t LoopState; //< Current state of loop

	bool RequestStop; //< Flag to stop the loop

	bool ShowLogo; //< True to pop up logo during next render
	bool SpinnerPopup;

	bool SwapIdled; //< True if idled during swap
	bool SpinnerPopped; //< Spinner is currently visible
	bool ShowingLogo; //< Logo is currently showing (animation already finished)
	void *CmdOwner; //< Owner of currently long-running coprocessor function (sketch, spinner, etc.)

	void *DeferredFree;

	Esd_HandleState HandleState;

	/* Callbacks called by Esd_Loop */
	Esd_Callback Start;
	Esd_Callback Update;
	Esd_Callback Render;
	Esd_Callback Idle;
	Esd_Callback End;
	void *UserContext;

} Esd_Context;

/// Parameters for initializing an ESD context
typedef struct
{
	/* Callbacks called by Esd_Loop */
	Esd_Callback Start;
	Esd_Callback Update;
	Esd_Callback Render;
	Esd_Callback Idle;
	Esd_Callback End;
	void *UserContext;

	/* Flash file path */
	eve_tchar_t FlashFilePath[260];

} Esd_Parameters;

extern ESD_CORE_EXPORT Esd_Context *Esd_CurrentContext; //< Pointer to current ESD context
extern ESD_CORE_EXPORT EVE_HalContext *Esd_Host; //< Pointer to current EVE hal context
extern ESD_CORE_EXPORT Esd_GpuAlloc *Esd_GAlloc; //< Pointer to current allocator

#if (EVE_SUPPORT_CHIPID >= EVE_FT810)
#define ESD_CO_SCRATCH_HANDLE (EVE_CHIPID >= EVE_FT810 ? Esd_Host->CoScratchHandle : 15)
#else
#define ESD_CO_SCRATCH_HANDLE (15)
#endif

ESD_CORE_EXPORT void Esd_SetCurrent(Esd_Context *ec);

ESD_CORE_EXPORT void Esd_Initialize();
ESD_CORE_EXPORT void Esd_Release();

ESD_CORE_EXPORT void Esd_Defaults(Esd_Parameters *ep);
ESD_CORE_EXPORT bool Esd_Open(Esd_Context *ec, Esd_Parameters *ep);
ESD_CORE_EXPORT void Esd_Close(Esd_Context *ec);

/* Main loop, calls Esd_Start, Esd_Update, Esd_WaitSwap, and Esd_Stop */
ESD_CORE_EXPORT void Esd_Loop(Esd_Context *ec);

ESD_CORE_EXPORT void Esd_Start(Esd_Context *ec);
ESD_CORE_EXPORT void Esd_Update(Esd_Context *ec);
ESD_CORE_EXPORT void Esd_Render(Esd_Context *ec);
ESD_CORE_EXPORT bool Esd_WaitSwap(Esd_Context *ec);
ESD_CORE_EXPORT void Esd_Stop(Esd_Context *ec);

/// A function to get milliseconds for current frame
ESD_FUNCTION(Esd_GetMillis, Type = uint32_t, DisplayName = "Get Milliseconds", Category = EsdUtilities, Inline, Include = "Esd_Core.h")
static inline uint32_t Esd_GetMillis() { return Esd_CurrentContext->Millis; }

/// A function to get the difference in milliseconds since last frame Update call
ESD_FUNCTION(Esd_GetDeltaMs, Type = uint32_t, DisplayName = "Get Delta Ms", Category = EsdUtilities, Inline, Include = "Esd_Core.h")
static inline uint32_t Esd_GetDeltaMs() { return Esd_CurrentContext->DeltaMs; }

/// A function to get the current HAL context data structure
ESD_FUNCTION(Esd_GetHost, Type = EVE_HalContext *, DisplayName = "Get EVE Host", Category = EsdUtilities, Inline, Include = "Esd_Core.h")
static inline EVE_HalContext *Esd_GetHost() { return Esd_Host; }

#endif /* #ifndef ESD_CONTEXT__H */

/* end of file */
