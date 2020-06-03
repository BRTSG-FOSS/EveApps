
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

// Keep cache of displaylist values that don't often change but are generally set by every widget to reduce display list size
#define ESD_DL_OPTIMIZE 1
#define ESD_DL_STATE_STACK_SIZE 5

// Whether the END command is sent
#define ESD_DL_END_PRIMITIVE 0

//
// Structs
//
#if ESD_DL_OPTIMIZE
#define ESD_STATE ESD_GpuState[ESD_GpuState_I]
typedef struct
{
	// Keep to a minimum
#if (EVE_SUPPORT_CHIPID >= EVE_FT810)
	uint32_t PaletteSource;
#endif
	int16_f4_t LineWidth;
	int16_f4_t PointSize;
	ft_rgb32_t ColorRGB;
	uint8_t ColorA;
	uint8_t Handle; // Current handle
	uint8_t Cell; // Current cell
#if (EVE_SUPPORT_CHIPID >= EVE_FT810)
	uint8_t VertexFormat; // Current vertex format
#endif
	bool BitmapTransform; // BitmapTransform other than default is set
} ESD_GpuState_T;
#endif

//
// Globals
//
#ifndef ESD_EXTERN_LIBARY
#define ESD_EXTERN_LIBARY
#endif
extern ESD_EXTERN_LIBARY EVE_HalContext *ESD_Host;
#if ESD_DL_OPTIMIZE
extern ESD_EXTERN_LIBARY ESD_GpuState_T ESD_GpuState[ESD_DL_STATE_STACK_SIZE];
extern ESD_EXTERN_LIBARY uint8_t ESD_GpuState_I;
extern ESD_EXTERN_LIBARY uint8_t ESD_Primitive;
// extern uint32_t Esd_CurrentContext->CoFgColor;
// extern uint32_t Esd_CurrentContext->CoBgColor;
#endif
extern ESD_EXTERN_LIBARY ESD_Rect16 ESD_ScissorRect;

// Reset any cached state
void Esd_ResetGpuState();
void Esd_ResetCoState();

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

// Set current tag. Must be returned to 255 after usage, to ensure next widgets don't draw with invalid tag
ESD_FUNCTION(ESD_Dl_TAG, Type = void, Category = EveRenderFunctions, Inline)
ESD_PARAMETER(s, Type = uint8_t, DisplayName = "Tag", Default = 255, Min = 0, Max = 255)
inline static void ESD_Dl_TAG(uint8_t s)
{
	EVE_CoCmd_dl(ESD_Host, TAG(s));
}

// Specify color RGB
ESD_FUNCTION(ESD_Dl_COLOR_RGB, Type = void, Category = EveRenderFunctions, Inline)
ESD_PARAMETER(c, Type = ft_rgb32_t, DisplayName = "Color")
inline static void ESD_Dl_COLOR_RGB(ft_rgb32_t c)
{
	ft_rgb32_t rgb = c & 0xFFFFFF;
#if ESD_DL_OPTIMIZE
	if (rgb != ESD_STATE.ColorRGB)
	{
#endif
		EVE_CoCmd_dl(ESD_Host, COLOR_RGB(0, 0, 0) | (rgb));
#if ESD_DL_OPTIMIZE
		ESD_STATE.ColorRGB = rgb;
	}
#endif
}

// Specify alpha channel
ESD_FUNCTION(ESD_Dl_COLOR_A, Type = void, Category = EveRenderFunctions, Inline)
ESD_PARAMETER(alpha, Type = uint8_t, Default = 255, Min = 0, Max = 255)
inline static void ESD_Dl_COLOR_A(uint8_t alpha)
{
#if ESD_DL_OPTIMIZE
	if (alpha != ESD_STATE.ColorA)
	{
#endif
		EVE_CoCmd_dl(ESD_Host, COLOR_A(alpha));
#if ESD_DL_OPTIMIZE
		ESD_STATE.ColorA = alpha;
	}
#endif
}

// Specify color: Alpha(31~24 bit) + RGB(23~0 bit)
ESD_FUNCTION(ESD_Dl_colorArgb, Type = void, Category = EveRenderFunctions, Inline)
ESD_PARAMETER(c, Type = ft_argb32_t, DisplayName = "Color")
inline static void ESD_Dl_colorArgb(ft_argb32_t c)
{
	ESD_Dl_COLOR_RGB(c);
	ESD_Dl_COLOR_A(c >> 24);
}

// Specify bitmap handle, see BITMAP_HANDLE
ESD_FUNCTION(ESD_Dl_BITMAP_HANDLE, Type = void, Category = EveRenderFunctions, Inline)
ESD_PARAMETER(handle, Type = uint8_t, Min = 0, Max = 31)
inline static void ESD_Dl_BITMAP_HANDLE(uint8_t handle)
{
#if ESD_DL_OPTIMIZE
	if (handle != ESD_STATE.Handle)
	{
#endif
		EVE_CoCmd_dl(ESD_Host, BITMAP_HANDLE(handle));
#if ESD_DL_OPTIMIZE
		ESD_STATE.Handle = handle;
	}
#endif
}

// Specify cell number for bitmap, see CELL
ESD_FUNCTION(ESD_Dl_CELL, Type = void, Category = EveRenderFunctions, Inline)
ESD_PARAMETER(cell, Type = uint8_t, Min = 0, Max = 255)
inline static void ESD_Dl_CELL(uint8_t cell)
{
#if ESD_DL_OPTIMIZE
	if (cell != ESD_STATE.Cell)
	{
#endif
		EVE_CoCmd_dl(ESD_Host, CELL(cell));
#if ESD_DL_OPTIMIZE
		ESD_STATE.Cell = cell;
	}
#endif
}

// Set Alpha_Func
ESD_FUNCTION(ESD_Dl_Alpha_Func, Type = void, Category = EveRenderFunctions, Inline)
ESD_PARAMETER(func, Type = uint8_t, Min = 0, Max = 7)
ESD_PARAMETER(ref, Type = uint8_t, Min = 0, Max = 255)
inline static void ESD_Dl_Alpha_Func(uint8_t func, uint8_t ref)
{
	EVE_CoCmd_dl(ESD_Host, ALPHA_FUNC(func, ref));
}

// Save EVE context, see SAVE_CONTEXT
ESD_FUNCTION(ESD_Dl_SAVE_CONTEXT, Type = void, Category = EveRenderFunctions, Inline)
inline static void ESD_Dl_SAVE_CONTEXT()
{
#if ESD_DL_OPTIMIZE
	uint8_t nextState;
#endif
	EVE_CoCmd_dl(ESD_Host, SAVE_CONTEXT());
#if ESD_DL_OPTIMIZE
	nextState = ESD_GpuState_I + 1;
	if (nextState < ESD_DL_STATE_STACK_SIZE)
	{
		ESD_GpuState[nextState] = ESD_GpuState[ESD_GpuState_I];
		ESD_GpuState_I = nextState;
	}
#endif
}

// Restore EVE context, see RESTORE_CONTEXT
ESD_FUNCTION(ESD_Dl_RESTORE_CONTEXT, Type = void, Category = EveRenderFunctions, Inline)
inline static void ESD_Dl_RESTORE_CONTEXT()
{
	EVE_CoCmd_dl(ESD_Host, RESTORE_CONTEXT());
#if ESD_DL_OPTIMIZE
	if (ESD_GpuState_I > 0)
		--ESD_GpuState_I;
#endif
}

#if (EVE_SUPPORT_CHIPID >= EVE_FT810)
// Specify vertex format , see VERTEX_FORMAT command
ESD_FUNCTION(ESD_Dl_VERTEX_FORMAT, Type = void, Category = EveRenderFunctions, Inline)
ESD_PARAMETER(frac, Type = uint8_t, Min = 0, Max = 4)
inline static void ESD_Dl_VERTEX_FORMAT(uint8_t frac)
{
#if ESD_DL_OPTIMIZE
	if (frac != ESD_STATE.VertexFormat)
	{
#endif
		EVE_CoCmd_dl(ESD_Host, VERTEX_FORMAT(frac));
#if ESD_DL_OPTIMIZE
		ESD_STATE.VertexFormat = frac;
	}
#endif
}
#else
#define ESD_Dl_VERTEX_FORMAT(frac) eve_assert(false)
#endif

#if (EVE_SUPPORT_CHIPID >= EVE_FT810)
// Set palette source, see PALETTE_SOURCE command
ESD_FUNCTION(ESD_Dl_PALETTE_SOURCE, Type = void, Category = EveRenderFunctions, Inline)
ESD_PARAMETER(addr, Type = uint32_t, Min = 0)
inline static void ESD_Dl_PALETTE_SOURCE(uint32_t addr)
{
#if ESD_DL_OPTIMIZE
	if (addr != ESD_STATE.PaletteSource)
	{
#endif
		EVE_CoCmd_dl(ESD_Host, PALETTE_SOURCE(addr));
#if ESD_DL_OPTIMIZE
		ESD_STATE.PaletteSource = addr;
	}
#endif
}
#else
#define ESD_Dl_PALETTE_SOURCE(addr) eve_assert(false)
#endif

ESD_FUNCTION(ESD_Dl_LINE_WIDTH, Type = void, Category = EveRenderFunctions, Inline)
ESD_PARAMETER(width, Type = int16_f4_t)
inline static void ESD_Dl_LINE_WIDTH(int16_f4_t width)
{
#if ESD_DL_OPTIMIZE
	if (width != ESD_STATE.LineWidth)
	{
#endif
		EVE_CoCmd_dl(ESD_Host, LINE_WIDTH(width));
#if ESD_DL_OPTIMIZE
		ESD_STATE.LineWidth = width;
	}
#endif
}

ESD_FUNCTION(ESD_Dl_POINT_SIZE, Type = void, Category = EveRenderFunctions, Inline)
ESD_PARAMETER(size, Type = int16_f4_t)
inline static void ESD_Dl_POINT_SIZE(int16_f4_t size)
{
#if ESD_DL_OPTIMIZE
	if (size != ESD_STATE.PointSize)
	{
#endif
		EVE_CoCmd_dl(ESD_Host, POINT_SIZE(size));
#if ESD_DL_OPTIMIZE
		ESD_STATE.PointSize = size;
	}
#endif
}

ESD_FUNCTION(ESD_Dl_BEGIN, Type = void, Category = EveRenderFunctions, Inline)
ESD_PARAMETER(primitive, Type = uint8_t)
inline static void ESD_Dl_BEGIN(uint8_t primitive)
{
#if ESD_DL_OPTIMIZE
	if (primitive != ESD_Primitive)
	{
#endif
		EVE_CoCmd_dl(ESD_Host, BEGIN(primitive));
#if ESD_DL_OPTIMIZE
		ESD_Primitive = primitive;
	}
#endif
}

ESD_FUNCTION(ESD_Dl_END, Type = void, Category = EveRenderFunctions, Inline)
inline static void ESD_Dl_END()
{
#if ESD_DL_END_PRIMITIVE || !ESD_DL_OPTIMIZE
#if ESD_DL_OPTIMIZE
	if (ESD_Primitive != 0)
	{
#endif
		EVE_CoCmd_dl(ESD_Host, END());
#if ESD_DL_OPTIMIZE
		ESD_Primitive = 0;
	}
#endif
#else
	// For continuous primitives, reset the active primitive.
	// This causes the BEGIN to be called for the next vertex series.
#if ESD_DL_OPTIMIZE
	switch (ESD_Primitive)
	{
	case LINE_STRIP:
	case EDGE_STRIP_R:
	case EDGE_STRIP_L:
	case EDGE_STRIP_A:
	case EDGE_STRIP_B:
		ESD_Primitive = 0;
	}
#endif
#endif
}

/* Display list calls without state caching */

// Fixed point vertex with subprecision depending on current vertex format
ESD_FUNCTION(ESD_Dl_VERTEX2F, Type = void, Category = EveRenderFunctions, Inline)
ESD_PARAMETER(x, Type = uint16_t)
ESD_PARAMETER(y, Type = uint16_t)
inline static void ESD_Dl_VERTEX2F(uint16_t x, uint16_t y)
{
	EVE_HalContext *phost = ESD_Host;
	EVE_CoCmd_dl(phost, VERTEX2F(x, y));
}

// Fixed point vertex using 4 bits subprecision
ESD_FUNCTION(ESD_Dl_VERTEX2F_4, Type = void, Category = EveRenderFunctions, Inline)
ESD_PARAMETER(x, Type = int16_f4_t)
ESD_PARAMETER(y, Type = int16_f4_t)
inline static void ESD_Dl_VERTEX2F_4(int16_f4_t x, int16_f4_t y)
{
	EVE_HalContext *phost = ESD_Host;
	if (EVE_CHIPID >= EVE_FT810)
		EVE_CoCmd_dl(phost, VERTEX_FORMAT(4));
	EVE_CoCmd_dl(phost, VERTEX2F(x, y));
}

// Fixed point vertex using 2 bits subprecision
ESD_FUNCTION(ESD_Dl_VERTEX2F_2, Type = void, Category = EveRenderFunctions, Inline)
ESD_PARAMETER(x, Type = int16_f2_t)
ESD_PARAMETER(y, Type = int16_f2_t)
inline static void ESD_Dl_VERTEX2F_2(int16_f2_t x, int16_f2_t y)
{
	EVE_HalContext *phost = ESD_Host;
	(void)phost;
	if (EVE_CHIPID >= EVE_FT810)
	{
		EVE_CoCmd_dl(phost, VERTEX_FORMAT(2));
	}
	else
	{
		x <<= 2;
		y <<= 2;
	}
	EVE_CoCmd_dl(ESD_Host, VERTEX2F(x, y));
}

// Fixed point vertex using 0 bits subprecision, or integer point vertex
ESD_FUNCTION(ESD_Dl_VERTEX2F_0, Type = void, Category = EveRenderFunctions, Inline)
ESD_PARAMETER(x, Type = uint16_t)
ESD_PARAMETER(y, Type = uint16_t)
inline static void ESD_Dl_VERTEX2F_0(uint16_t x, uint16_t y)
{
	EVE_HalContext *phost = ESD_Host;
	(void)phost;
	if (EVE_CHIPID >= EVE_FT810)
	{
		EVE_CoCmd_dl(phost, VERTEX_FORMAT(0));
		EVE_CoCmd_dl(ESD_Host, VERTEX2F(x, y));
	}
	else
	{
		x <<= 4;
		y <<= 4;
	}
	EVE_CoCmd_dl(ESD_Host, VERTEX2F(x, y));
}

// Display list calls without state caching
ESD_FUNCTION(ESD_Dl_VERTEX2II, Type = void, Category = EveRenderFunctions, Inline)
ESD_PARAMETER(x, Type = uint16_t)
ESD_PARAMETER(y, Type = uint16_t)
ESD_PARAMETER(handle, Type = uint8_t)
ESD_PARAMETER(cell, Type = uint8_t)
inline static void ESD_Dl_VERTEX2II(uint16_t x, uint16_t y, uint8_t handle, uint8_t cell)
{
	EVE_CoCmd_dl(ESD_Host, VERTEX2II(x, y, handle, cell));
}

// Specify clear color RGB
ESD_FUNCTION(ESD_Dl_CLEAR_COLOR_RGB, Type = void, Category = EveRenderFunctions, Inline)
ESD_PARAMETER(c, Type = ft_rgb32_t, DisplayName = "Color")
inline static void ESD_Dl_CLEAR_COLOR_RGB(ft_rgb32_t c)
{
	ft_rgb32_t rgb = c & 0xFFFFFF;
	EVE_CoCmd_dl(ESD_Host, CLEAR_COLOR_RGB(0, 0, 0) | (rgb));
}

// Specify clear color alpha channel
ESD_FUNCTION(ESD_Dl_CLEAR_COLOR_A, Type = void, Category = EveRenderFunctions, Inline)
ESD_PARAMETER(alpha, Type = uint8_t, Default = 255, Min = 0, Max = 255)
inline static void ESD_Dl_CLEAR_COLOR_A(uint8_t alpha)
{
	EVE_CoCmd_dl(ESD_Host, CLEAR_COLOR_A(alpha));
}

// Specify clear color: Alpha(31~24 bit) + RGB(23~0 bit)
ESD_FUNCTION(ESD_Dl_CLEAR_COLOR_ARGB, Type = void, Category = EveRenderFunctions, Inline)
ESD_PARAMETER(c, Type = ft_argb32_t, DisplayName = "Color")
inline static void ESD_Dl_CLEAR_COLOR_ARGB(ft_argb32_t c)
{
	ESD_Dl_CLEAR_COLOR_RGB(c);
	ESD_Dl_CLEAR_COLOR_A(c >> 24);
}

/* end of supported functions */

// Get the height of builtin font
ESD_FUNCTION(ESD_GetFontHeight, Type = uint16_t, DisplayName = "Get Font Height", Category = _GroupHidden, Macro)
ESD_PARAMETER(i, Type = int, DisplayName = "Bitmap Handle", Default = 0, Min = 0, Max = 31)
// inline static uint16_t ESD_GetFontHeight(int i) { return ESD_FontHeight[i]; }
#define ESD_GetFontHeight(i) (Esd_GetFontHeight(Esd_GetRomFont(i)));

ESD_FUNCTION(ESD_Dl_CLEAR, Type = void, Category = EveRenderFunctions, Inline)
ESD_PARAMETER(c, Type = uint8_t, DisplayName = "Clear Color")
ESD_PARAMETER(s, Type = uint8_t, DisplayName = "Clear Stencil")
ESD_PARAMETER(t, Type = uint8_t, DisplayName = "Clear Tag")
inline static void ESD_Dl_CLEAR(uint8_t c, uint8_t s, uint8_t t)
{
	EVE_CoCmd_dl(ESD_Host, CLEAR(c, s, t));
}

/*

#define STENCIL_FUNC(func,ref,mask) ((10UL<<24)|(((func)&7UL)<<16)|(((ref)&255UL)<<8)|(((mask)&255UL)<<0))
#define BLEND_FUNC(src,dst) ((11UL<<24)|(((src)&7UL)<<3)|(((dst)&7UL)<<0))
#define STENCIL_OP(sfail,spass) ((12UL<<24)|(((sfail)&7UL)<<3)|(((spass)&7UL)<<0))
#define CLEAR_STENCIL(s) ((17UL<<24)|(((s)&255UL)<<0))
#define CLEAR_TAG(s) ((18UL<<24)|(((s)&255UL)<<0))
#define STENCIL_MASK(mask) ((19UL<<24)|(((mask)&255UL)<<0))
#define TAG_MASK(mask) ((20UL<<24)|(((mask)&1UL)<<0))
#define BITMAP_TRANSFORM_A(a) ((21UL<<24)|(((a)&131071UL)<<0))
#define BITMAP_TRANSFORM_B(b) ((22UL<<24)|(((b)&131071UL)<<0))
#define BITMAP_TRANSFORM_C(c) ((23UL<<24)|(((c)&16777215UL)<<0))
#define BITMAP_TRANSFORM_D(d) ((24UL<<24)|(((d)&131071UL)<<0))
#define BITMAP_TRANSFORM_E(e) ((25UL<<24)|(((e)&131071UL)<<0))
#define BITMAP_TRANSFORM_F(f) ((26UL<<24)|(((f)&16777215UL)<<0))
#define COLOR_MASK(r,g,b,a) ((32UL<<24)|(((r)&1UL)<<3)|(((g)&1UL)<<2)|(((b)&1UL)<<1)|(((a)&1UL)<<0))
#define CLEAR(c,s,t) ((38UL<<24)|(((c)&1UL)<<2)|(((s)&1UL)<<1)|(((t)&1UL)<<0))
#define NOP() ((45UL<<24))
// #define CLEAR_COLOR_A(alpha) ((15UL<<24)|(((alpha)&255UL)<<0))
// #define CLEAR_COLOR_RGB(red, green, blue) ((2UL << 24) | (((red)&255UL) << 16) | (((green)&255UL) << 8) | (((blue)&255UL) << 0))
// #define VERTEX2F(x,y) ((1UL<<30)|(((x)&32767UL)<<15)|(((y)&32767UL)<<0))
// #define VERTEX2II(x,y,handle,cell) ((2UL<<30)|(((x)&511UL)<<21)|(((y)&511UL)<<12)|(((handle)&31UL)<<7)|(((cell)&127UL)<<0))
// #define ALPHA_FUNC(func,ref) ((9UL<<24)|(((func)&7UL)<<8)|(((ref)&255UL)<<0))
// #define POINT_SIZE(size) ((13UL<<24)|(((size)&8191UL)<<0))
// #define COLOR_A(alpha) ((16UL<<24)|(((alpha)&255UL)<<0))
// #define VERTEX_FORMAT(frac) ((39UL<<24)|(((frac)&7UL)<<0))
// #define PALETTE_SOURCE(addr) ((42UL<<24)|(((addr)&4194303UL)<<0))
// #define BEGIN(prim) ((31UL<<24)|(((prim)&15UL)<<0))
// #define END() ((33UL<<24))
// #define SAVE_CONTEXT() ((34UL<<24))
// #define RESTORE_CONTEXT() ((35UL<<24))
// Not exposed. #define SCISSOR_XY(x,y) ((27UL<<24)|(((x)&2047UL)<<11)|(((y)&2047UL)<<0))
// Not exposed. #define SCISSOR_SIZE(width,height) ((28UL<<24)|(((width)&4095UL)<<12)|(((height)&4095UL)<<0))
// Not exposed. #define CALL(dest) ((29UL<<24)|(((dest)&65535UL)<<0))
// Not exposed. #define JUMP(dest) ((30UL<<24)|(((dest)&65535UL)<<0))
// Not exposed. #define BITMAP_LAYOUT_H(linestride,height) ((40UL<<24)|(((linestride)&3UL)<<2)|(((height)&3UL)<<0))
// Not exposed. #define BITMAP_SIZE_H(width,height) ((41UL<<24)|(((width)&3UL)<<2)|(((height)&3UL)<<0))
// Not exposed. #define VERTEX_TRANSLATE_X(x) ((43UL<<24)|(((x)&131071UL)<<0))
// Not exposed. #define VERTEX_TRANSLATE_Y(y) ((44UL<<24)|(((y)&131071UL)<<0))
// Not exposed. #define RETURN() ((36UL<<24))
// Not exposed. #define MACRO(m) ((37UL<<24)|(((m)&1UL)<<0))
// Not exposed. #define DISPLAY() ((0UL<<24))

*/

/*

void ESD_Dl_BITMAP_SOURCE(uint32_t addr);

ESD_FUNCTION(ESD_Dl_CLEAR_COLOR_RGB, Type = void, Include = "ESD_Dl.h")
ESD_PARAMETER(c, Type = uint32_t, EditType = Color)
void ESD_Dl_CLEAR_COLOR_RGB(uint32_t c);

// void ESD_Dl_BITMAP_LAYOUT(uint16_t format, uint16_t linestride, uint16_t height);
// void ESD_Dl_BITMAP_SIZE(uint16_t filter, uint16_t wrapx, uint16_t wrapy, uint16_t width, uint16_t height);

void ESD_Dl_ALPHA_FUNC(uint16_t func, uint16_t ref);
void ESD_Dl_STENCIL_FUNC(uint16_t func, uint16_t ref, uint16_t mask);
void ESD_Dl_BLEND_FUNC(uint16_t src, uint16_t dst);
void ESD_Dl_STENCIL_OP(uint16_t sfail, uint16_t spass);

void ESD_Dl_CLEAR_STENCIL(uint8_t s);
void ESD_Dl_CLEAR_TAG(uint8_t s);
void ESD_Dl_STENCIL_MASK(uint32_t mask);
void ESD_Dl_TAG_MASK(uint32_t mask);

void ESD_Dl_BITMAP_TRANSFORM_A(uint32_t a);
void ESD_Dl_BITMAP_TRANSFORM_B(uint32_t b);
void ESD_Dl_BITMAP_TRANSFORM_C(uint32_t c);
void ESD_Dl_BITMAP_TRANSFORM_D(uint32_t d);
void ESD_Dl_BITMAP_TRANSFORM_E(uint32_t e);
void ESD_Dl_BITMAP_TRANSFORM_F(uint32_t f);

// void ESD_Dl_CALL(uint16_t dest);
// void ESD_Dl_JUMP(uint16_t dest);
// void ESD_Dl_RETURN();

// void ESD_Dl_MACRO(uint8_t m);

// void ESD_Dl_DISPLAY();

void ESD_Dl_COLOR_MASK(uint8_t r, uint8_t g, uint8_t b, uint8_t a);

*/

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

ESD_FUNCTION(ESD_Cmd_Button, DisplayName = "cmd_button", Include = "ESD_Dl.h", Category = EveRenderCoprocessorProto)
ESD_PARAMETER(x, Type = int16_t, Default = 0)
ESD_PARAMETER(y, Type = int16_t, Default = 0)
ESD_PARAMETER(w, Type = int16_t, Default = 60)
ESD_PARAMETER(h, Type = int16_t, Default = 20)
ESD_PARAMETER(font, Type = int16_t, Default = 21)
ESD_PARAMETER(options, Type = Ft_CoPro_Opt)
ESD_PARAMETER(s, Type = const char *, Default = "Button")
void ESD_Cmd_Button(int16_t x, int16_t y, int16_t w, int16_t h, int16_t font, uint16_t options, const char *s);

ESD_FUNCTION(ESD_Cmd_Number, DisplayName = "cmd_number", Include = "ESD_Dl.h", Category = EveRenderCoprocessorProto)
ESD_PARAMETER(x, Type = int16_t, Default = 0)
ESD_PARAMETER(y, Type = int16_t, Default = 0)
ESD_PARAMETER(font, Type = int16_t, Default = 21)
ESD_PARAMETER(options, Type = Ft_CoPro_Opt)
ESD_PARAMETER(n, Type = int32_t, Default = 0)
void ESD_Cmd_Number(int16_t x, int16_t y, int16_t font, uint16_t options, int32_t n);

ESD_FUNCTION(EVE_Hal_rd32, Type = uint32_t, Buffered, Include = "Ft_Gpu_Hal.h", Category = _GroupHidden)
ESD_PARAMETER(phost, Type = EVE_HalContext *, Default = ESD_GetHost, Hidden, Internal, Static)
ESD_PARAMETER(addr, Type = uint32_t, DisplayName = "Address")

ESD_FUNCTION(EVE_CoCmd_dl, Include = "Ft_Gpu_Hal.h", Category = _GroupHidden)
ESD_PARAMETER(phost, Type = EVE_HalContext *, Default = ESD_GetHost, Hidden, Internal, Static)
ESD_PARAMETER(cmd, Type = uint32_t)

#endif /* #ifndef ESD_DL_H */

/* end of file */
