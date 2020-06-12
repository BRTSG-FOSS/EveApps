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

#ifndef ESD_COMPATIBILITY_CORE__H
#define ESD_COMPATIBILITY_CORE__H

#include "ESD_Core.h"

#include <FT_Platform.h>

#include "Ft_Esd.h"
#include "Ft_Esd_GpuAlloc.h"
#include "FT_Esd_Dl.h"
#include "FT_Esd_CoCmd.h"
#include "Ft_Esd_BitmapHandle.h"
#include "Ft_Esd_TouchTag.h"

#define Esd_Context ESD_Context
#define Esd_Parameters ESD_Parameters

#define Esd_CurrentContext ESD_CurrentContext
#define Ft_Esd_Host ESD_Host
#define Ft_Esd_GAlloc ESD_GAlloc

#define Esd_SetCurrent ESD_SetCurrent
#define Esd_Defaults ESD_Defaults
static inline void Esd_Initialize(Esd_Context *ec, Esd_Parameters *ep)
{
	ESD_Initialize();
	ESD_Open(ec, ep);
}
#define Esd_Release ESD_Close
#define Esd_Shutdown ESD_Release

#define Esd_Loop ESD_Loop

#define Esd_Start ESD_Start
#define Esd_Update ESD_Update
#define Esd_Render ESD_Render
#define Esd_WaitSwap ESD_WaitSwap
#define Esd_Stop ESD_Stop

#endif /* #ifndef ESD_CORE__H */

/* end of file */
