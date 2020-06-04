
#ifndef ESD_DL__H
#define ESD_DL__H

#include <EVE_Hal.h>

#include "ESD_TypeDefs.h"
#include "ESD_BitmapInfo.h"
#include "ESD_Math.h"
#include "ESD_BitmapHandle.h"

// TODO: Reduce this to only utility shortcuts

ESD_CATEGORY(EsdUtilities, DisplayName = "ESD Utilities")
ESD_CATEGORY(EsdRenderable, DisplayName = "ESD Render Functions")
ESD_CATEGORY(EveRenderFunctions, DisplayName = "Display List", Category = EsdRenderable)

ESD_TYPE(EVE_HalContext *, Native = Pointer, Edit = Library)

//
// Globals
//
#ifndef ESD_EXTERN_LIBARY
#define ESD_EXTERN_LIBARY
#endif
extern ESD_EXTERN_LIBARY EVE_HalContext *ESD_Host;
extern ESD_EXTERN_LIBARY ESD_Rect16 ESD_ScissorRect;

// Reset any cached state
void Esd_ResetGpuState();

// Returns the current scissor area
ESD_FUNCTION(ESD_Dl_Scissor_Get, Type = ESD_Rect16, Category = EveRenderFunctions)
ESD_Rect16 ESD_Dl_Scissor_Get();

// Set scissor area. Cropped to the previous scissor area. Returns previous scissor area
ESD_FUNCTION(ESD_Dl_Scissor_Set, Type = ESD_Rect16, Category = EveRenderFunctions, Buffered)
ESD_PARAMETER(rect, Type = ESD_Rect16)
ESD_Rect16 ESD_Dl_Scissor_Set(ESD_Rect16 rect);

// Set scissor area. Cropped to the previous scissor area which must be passed as an argument
ESD_FUNCTION(ESD_Dl_Scissor_Adjust, Category = EveRenderFunctions)
ESD_PARAMETER(rect, Type = ESD_Rect16)
ESD_PARAMETER(state, Type = ESD_Rect16)
void ESD_Dl_Scissor_Adjust(ESD_Rect16 rect, ESD_Rect16 state);

// Reset scossor area to the previous state which must be passed as an argument
ESD_FUNCTION(ESD_Dl_Scissor_Reset, Category = EveRenderFunctions)
ESD_PARAMETER(state, Type = ESD_Rect16)
void ESD_Dl_Scissor_Reset(ESD_Rect16 state);

// Get the height of builtin font
ESD_FUNCTION(ESD_GetFontHeight, Type = uint16_t, DisplayName = "Get Font Height", Category = _GroupHidden, Macro)
ESD_PARAMETER(i, Type = int, DisplayName = "Bitmap Handle", Default = 0, Min = 0, Max = 31)
// inline static uint16_t ESD_GetFontHeight(int i) { return ESD_FontHeight[i]; }
#define ESD_GetFontHeight(i) (ESD_getFontHeight(ESD_getRomFont(i)));

ESD_FUNCTION(ESD_Dl_CLEAR, Type = void, Category = EveRenderFunctions, Inline)
ESD_PARAMETER(c, Type = uint8_t, DisplayName = "Clear Color")
ESD_PARAMETER(s, Type = uint8_t, DisplayName = "Clear Stencil")
ESD_PARAMETER(t, Type = uint8_t, DisplayName = "Clear Tag")
inline static void ESD_Dl_CLEAR(uint8_t c, uint8_t s, uint8_t t)
{
	EVE_CoCmd_dl(ESD_Host, CLEAR(c, s, t));
}

ESD_CATEGORY(EveRenderFunctionsProto, DisplayName = "EVE Render Functions (Prototype)", Category = _GroupHidden)
ESD_CATEGORY(EveRenderDisplayListProto, DisplayName = "Display List", Category = EveRenderFunctionsProto)
ESD_CATEGORY(EveRenderCoprocessorProto, DisplayName = "Coprocessor", Category = EveRenderFunctionsProto)

ESD_CATEGORY(EsdWidgetsProto, DisplayName = "ESD Widgets (Prototype)", Category = _GroupHidden)

ESD_FUNCTION(ESD_Render_Rect_Grad, DisplayName = "Render GRAD RECT ", Include = "ESD_Dl.h", Category = EveRenderDisplayListProto)
ESD_PARAMETER(x, Type = int16_t, Default = 0)
ESD_PARAMETER(y, Type = int16_t, Default = 0)
ESD_PARAMETER(w, Type = int16_t, Default = 60)
ESD_PARAMETER(h, Type = int16_t, Default = 20)
ESD_PARAMETER(color1, Type = uint32_t, Default = #FFFFFFFF)
ESD_PARAMETER(color2, Type = uint32_t, Default = #FFFFFFFF)
ESD_PARAMETER(direction, Type = int16_t, Default = 90)
void ESD_Render_Rect_Grad(int16_t x, int16_t y, int16_t w, int16_t h, uint32_t color1, uint32_t color2, int16_t direction);

ESD_FUNCTION(ESD_Render_Rect, DisplayName = "Render RECT", Include = "ESD_Dl.h", Category = EveRenderDisplayListProto)
ESD_PARAMETER(x, Type = int16_t, Default = 0)
ESD_PARAMETER(y, Type = int16_t, Default = 0)
ESD_PARAMETER(w, Type = int16_t, Default = 60)
ESD_PARAMETER(h, Type = int16_t, Default = 20)
void ESD_Render_Rect(int16_t x, int16_t y, int16_t w, int16_t h);

#endif /* #ifndef ESD_DL__H */

/* end of file */
