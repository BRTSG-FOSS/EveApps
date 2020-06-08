
#ifndef ESD_SCISSOR__H
#define ESD_SCISSOR__H

#include "ESD_Base.h"
#include "ESD_Math.h"

//
// Globals
//
extern ESD_CORE_EXPORT ESD_Rect16 ESD_ScissorRect;

// Reset any cached state
void ESD_Scissor_DlStart();

// Returns the current scissor area
#pragma ESD_FUNCTION(ESD_Scissor_Get, Type = ESD_Rect16, Attributes = ESD_CORE_EXPORT, Category = EveRenderFunctions)
ESD_CORE_EXPORT ESD_Rect16 ESD_Scissor_Get();

// Set scissor area. Cropped to the previous scissor area. Returns previous scissor area
#pragma ESD_FUNCTION(ESD_Scissor_Set, Type = ESD_Rect16, Attributes = ESD_CORE_EXPORT, Category = EveRenderFunctions, Buffered)
#pragma ESD_PARAMETER(rect, Type = ESD_Rect16)
ESD_CORE_EXPORT ESD_Rect16 ESD_Scissor_Set(ESD_Rect16 rect);

// Set scissor area. Cropped to the previous scissor area which must be passed as an argument
#pragma ESD_FUNCTION(ESD_Scissor_Adjust, Attributes = ESD_CORE_EXPORT, Category = EveRenderFunctions)
#pragma ESD_PARAMETER(rect, Type = ESD_Rect16)
#pragma ESD_PARAMETER(state, Type = ESD_Rect16)
ESD_CORE_EXPORT void ESD_Scissor_Adjust(ESD_Rect16 rect, ESD_Rect16 state);

// Reset scossor area to the previous state which must be passed as an argument
#pragma ESD_FUNCTION(ESD_Scissor_Reset, Attributes = ESD_CORE_EXPORT, Category = EveRenderFunctions)
#pragma ESD_PARAMETER(state, Type = ESD_Rect16)
ESD_CORE_EXPORT void ESD_Scissor_Reset(ESD_Rect16 state);

#endif /* #ifndef ESD_SCISSOR__H */

/* end of file */
