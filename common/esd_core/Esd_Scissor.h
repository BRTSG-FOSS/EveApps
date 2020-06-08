
#ifndef ESD_SCISSOR__H
#define ESD_SCISSOR__H

#include "ESD_Base.h"
#include "ESD_Math.h"

//
// Globals
//
extern ESD_CORE_EXPORT ESD_Rect16 ESD_ScissorRect;

// Reset any cached state
void ESD_Scissor_dlStart();

// Returns the current scissor area
ESD_FUNCTION(ESD_Scissor_get, Type = ESD_Rect16, Attributes = ESD_CORE_EXPORT, Category = EveRenderFunctions)
ESD_CORE_EXPORT ESD_Rect16 ESD_Scissor_get();

// Set scissor area. Cropped to the previous scissor area. Returns previous scissor area
ESD_FUNCTION(ESD_Scissor_set, Type = ESD_Rect16, Attributes = ESD_CORE_EXPORT, Category = EveRenderFunctions, Buffered)
ESD_PARAMETER(rect, Type = ESD_Rect16)
ESD_CORE_EXPORT ESD_Rect16 ESD_Scissor_set(ESD_Rect16 rect);

// Set scissor area. Cropped to the previous scissor area which must be passed as an argument
ESD_FUNCTION(ESD_Scissor_adjust, Attributes = ESD_CORE_EXPORT, Category = EveRenderFunctions)
ESD_PARAMETER(rect, Type = ESD_Rect16)
ESD_PARAMETER(state, Type = ESD_Rect16)
ESD_CORE_EXPORT void ESD_Scissor_adjust(ESD_Rect16 rect, ESD_Rect16 state);

// Reset scossor area to the previous state which must be passed as an argument
ESD_FUNCTION(ESD_Scissor_reset, Attributes = ESD_CORE_EXPORT, Category = EveRenderFunctions)
ESD_PARAMETER(state, Type = ESD_Rect16)
ESD_CORE_EXPORT void ESD_Scissor_reset(ESD_Rect16 state);

#endif /* #ifndef ESD_SCISSOR__H */

/* end of file */
