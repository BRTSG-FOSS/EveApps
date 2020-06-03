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

/// A function to get milliseconds for current frame
ESD_FUNCTION(ESD_GetMillis, Type = ft_uint32_t, DisplayName = "Get Milliseconds", Category = EsdUtilities)
ft_uint32_t ESD_GetMillis() { return Esd_CurrentContext->Millis; }

/// A function to get the difference in milliseconds since last frame Update call
ESD_FUNCTION(ESD_GetDeltaMs, Type = ft_uint32_t, DisplayName = "Get Delta Ms", Category = EsdUtilities)
ft_uint32_t ESD_GetDeltaMs() { return Esd_CurrentContext->DeltaMs; }

/// A function to get the current HAL context data structure
ESD_FUNCTION(ESD_GetHost, Type = EVE_HalContext *, DisplayName = "Get EVE Host", Category = EsdUtilities)
EVE_HalContext *ESD_GetHost() { return ESD_Host; }

#ifdef ESD_SIMULATION

int ESD_DesignerMode = 0;

ft_uint32_t ESD_GAlloc_GetTotalUsed(ESD_GpuAlloc *ga)
{
	if (!ESD_GAlloc)
		return 0;
	return ESD_GpuAlloc_GetTotalUsed(ESD_GAlloc);
}

ft_uint32_t ESD_GAlloc_GetTotal(ESD_GpuAlloc *ga)
{
	if (!ESD_GAlloc)
		return 0;
	return ESD_GpuAlloc_GetTotal(ESD_GAlloc);
}

#endif

#if 0

// When not in the simulation, use the Ft_Main__Start etc symbols
// as exported by the single Application logic document included
#ifndef ESD_SIMULATION
#define Ft_Main__Start__ESD Ft_Main__Start
#define Ft_Main__Update__ESD Ft_Main__Update
#define Ft_Main__Render__ESD Ft_Main__Render
#define Ft_Main__Idle__ESD Ft_Main__Idle
#define Ft_Main__End__ESD Ft_Main__End
#endif

ft_void_t Ft_Main__Start__ESD();
ft_void_t Ft_Main__Update__ESD();
ft_void_t Ft_Main__Render__ESD();
ft_void_t Ft_Main__Idle__ESD();
ft_void_t Ft_Main__End__ESD();

static void Main_Start(void *context) { Ft_Main__Start__ESD(); }
static void Main_Update(void *context) { Ft_Main__Update__ESD(); }
static void Main_Render(void *context) { Ft_Main__Render__ESD(); }
static void Main_Idle(void *context) { Ft_Main__Idle__ESD(); }
static void Main_End(void *context) { Ft_Main__End__ESD(); }
/* Main entry point */
ft_int32_t main(ft_int32_t argc, ft_char8_t *argv[])
{
	Esd_Parameters ep;
	Esd_Defaults(&ep);
	ep.Start = Main_Start;
	ep.Update = Main_Update;
	ep.Render = Main_Render;
	ep.Idle = Main_Idle;
	ep.End = Main_End;
	Esd_Context ec;
	Esd_Initialize(&ec, &ep);
	Esd_Loop(&ec);
	Esd_Release(&ec);
	Esd_Shutdown();
	return EXIT_SUCCESS;
}

#endif

/* end of file */
