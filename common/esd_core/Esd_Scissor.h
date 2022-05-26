
#ifndef ESD_SCISSOR__H
#define ESD_SCISSOR__H

#include "Esd_Base.h"
#include "Esd_Math.h"

#ifdef __cplusplus
extern "C" {
#endif

// Returns the current scissor area
ESD_FUNCTION(Esd_Scissor_Get, Type = Esd_Rect16, Attributes = ESD_CORE_EXPORT, Category = EveRenderFunctions, Include = "Esd_Core.h")
ESD_CORE_EXPORT Esd_Rect16 Esd_Scissor_Get();

// Set scissor area. Cropped to the previous scissor area. Returns previous scissor area
ESD_FUNCTION(Esd_Scissor_Set, Type = Esd_Rect16, Attributes = ESD_CORE_EXPORT, Category = EveRenderFunctions, Buffered, Include = "Esd_Core.h")
ESD_PARAMETER(rect, Type = Esd_Rect16)
ESD_CORE_EXPORT Esd_Rect16 Esd_Scissor_Set(Esd_Rect16 rect);

// Set scissor area. Cropped to the previous scissor area which must be passed as an argument
ESD_FUNCTION(Esd_Scissor_Adjust, Attributes = ESD_CORE_EXPORT, Category = EveRenderFunctions, Include = "Esd_Core.h")
ESD_PARAMETER(rect, Type = Esd_Rect16)
ESD_PARAMETER(state, Type = Esd_Rect16)
ESD_CORE_EXPORT void Esd_Scissor_Adjust(Esd_Rect16 rect, Esd_Rect16 state);

// Reset scossor area to the previous state which must be passed as an argument
ESD_FUNCTION(Esd_Scissor_Reset, Attributes = ESD_CORE_EXPORT, Category = EveRenderFunctions, Include = "Esd_Core.h")
ESD_PARAMETER(state, Type = Esd_Rect16)
ESD_CORE_EXPORT void Esd_Scissor_Reset(Esd_Rect16 state);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef ESD_SCISSOR__H */

/* end of file */
