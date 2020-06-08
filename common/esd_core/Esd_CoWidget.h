
#ifndef ESD_COWIDGET__H
#define ESD_COWIDGET__H

#include "ESD_Context.h"
#include "ESD_Math.h"

/*

The coprocessor provides several commands which use the same CMD_STOP
command, such as CMD_SPINNER and CMD_SKETCH. This source file provides
an interface to use these without causing conflicts on the CMD_STOP
command. Other long-running coprocessor commands are also handled here,
such as CMD_LOGO, and CMD_PLAYVIDEO.

*/

#pragma ESD_FUNCTION(ESD_CoWidget_Spinner, Type = void, Attributes = ESD_CORE_EXPORT, Category = _GroupHidden)
#pragma ESD_PARAMETER(owner, Type = void *, Default = 0, Hidden, Internal)
#pragma ESD_PARAMETER(x, Type = int16_t, Default = 0) // SCREEN_SIZE
#pragma ESD_PARAMETER(y, Type = int16_t, Default = 0) // SCREEN_SIZE
#pragma ESD_PARAMETER(style, Type = uint16_t, Default = 0) // TODO: Spinner style enum
#pragma ESD_PARAMETER(scale, Type = uint16_t, Default = 0)
ESD_CORE_EXPORT void ESD_CoWidget_Spinner(void *owner, int16_t x, int16_t y, uint16_t style, uint16_t scale);

#pragma ESD_FUNCTION(ESD_CoWidget_ScreenSaver, Type = void, Attributes = ESD_CORE_EXPORT, Category = _GroupHidden)
#pragma ESD_PARAMETER(owner, Type = void *, Default = 0, Hidden, Internal)
ESD_CORE_EXPORT void ESD_CoWidget_ScreenSaver(void *owner);

#pragma ESD_FUNCTION(ESD_CoWidget_Sketch, Type = void, Attributes = ESD_CORE_EXPORT, Category = _GroupHidden)
#pragma ESD_PARAMETER(owner, Type = void *, Default = 0, Hidden, Internal)
#pragma ESD_PARAMETER(globalRect, Type = Esd_Rect16, Default = 0) // SCREEN_SIZE
#pragma ESD_PARAMETER(addr, Type = uint32_t, Default = 0)
#pragma ESD_PARAMETER(format, Type = uint16_t, Default = L8)
ESD_CORE_EXPORT void ESD_CoWidget_Sketch(void *owner, ESD_Rect16 globalRect, uint32_t addr, uint16_t format);

#pragma ESD_FUNCTION(ESD_CoWidget_Stop, Type = void, Attributes = ESD_CORE_EXPORT, Category = _GroupHidden)
#pragma ESD_PARAMETER(owner, Type = void *, Default = 0, Hidden, Internal)
ESD_CORE_EXPORT void ESD_CoWidget_Stop(void *owner);

/* Pops up the default spinner on the next frame (the frame currently being built) until the subsequent frame is ready.
This is useful when a blocking function will keep the system busy on the next frame. */
#pragma ESD_FUNCTION(ESD_CoWidget_PopupSpinner, DisplayName = "Pop-up Spinner", Category = EsdUtilities, Inline)
static inline void ESD_CoWidget_PopupSpinner()
{
	ESD_CurrentContext->SpinnerPopup = true;
}

#endif /* #ifndef ESD_COWIDGET__H */

/* end of file */
