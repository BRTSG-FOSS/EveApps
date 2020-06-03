
#ifndef FT_ESD_DL_H
#define FT_ESD_DL_H

#include <Ft_Platform.h>
#include <Ft_CoPro_Cmds.h>

#include "Ft_Esd.h"
#include "Ft_Gpu.h"
#include "Ft_Esd_BitmapInfo.h"
#include "Ft_Esd_Math.h"
#include "Ft_Esd_BitmapHandle.h"

ESD_CATEGORY(EsdUtilities, DisplayName = "ESD Utilities")
ESD_CATEGORY(EsdRenderable, DisplayName = "ESD Render Functions")
ESD_CATEGORY(EveRenderFunctions, DisplayName = "Display List", Category = EsdRenderable)

ESD_TYPE(EVE_HalContext *, Native = Pointer, Edit = Library)

// Keep cache of displaylist values that don't often change but are generally set by every widget to reduce display list size
#define ESD_DL_OPTIMIZE 1
#define ESD_DL_STATE_STACK_SIZE 5

// Whether the END command is sent
#define ESD_DL_END_PRIMITIVE 0

// Backport compatibility
#define Esd_Dl_Scissor_Get Ft_Esd_Dl_Scissor_Get
#define Esd_Dl_Scissor_Set Ft_Esd_Dl_Scissor_Set
#define Esd_Dl_Scissor_Adjust Ft_Esd_Dl_Scissor_Adjust
#define Esd_Dl_Scissor_Reset Ft_Esd_Dl_Scissor_Reset
#define Esd_Dl_TAG Ft_Esd_Dl_TAG
#define Esd_Dl_COLOR_RGB Ft_Esd_Dl_COLOR_RGB
#define Esd_Dl_COLOR_A Ft_Esd_Dl_COLOR_A
#define Esd_Dl_COLOR_ARGB Ft_Esd_Dl_COLOR_ARGB
#define Esd_Dl_BITMAP_HANDLE Ft_Esd_Dl_BITMAP_HANDLE
#define Esd_Dl_CELL Ft_Esd_Dl_CELL
#define Esd_Dl_ALPHA_FUNC Ft_Esd_Dl_Alpha_Func
#define Esd_Dl_SAVE_CONTEXT Ft_Esd_Dl_SAVE_CONTEXT
#define Esd_Dl_RESTORE_CONTEXT Ft_Esd_Dl_RESTORE_CONTEXT
#define Esd_Dl_VERTEX_FORMAT Ft_Esd_Dl_VERTEX_FORMAT
#define Esd_Dl_PALETTE_SOURCE Ft_Esd_Dl_PALETTE_SOURCE
#define Esd_Dl_LINE_WIDTH Ft_Esd_Dl_LINE_WIDTH
#define Esd_Dl_POINT_SIZE Ft_Esd_Dl_POINT_SIZE
#define FT_Esd_Dl_POINT_SIZE Ft_Esd_Dl_POINT_SIZE
#define Esd_Dl_BEGIN Ft_Esd_Dl_BEGIN
#define Esd_Dl_END Ft_Esd_Dl_END

//
// Structs
//
#if ESD_DL_OPTIMIZE
#define FT_ESD_STATE Ft_Esd_GpuState[Ft_Esd_GpuState_I]
typedef struct
{
	// Keep to a minimum
#if (EVE_SUPPORT_CHIPID >= EVE_FT810)
	ft_uint32_t PaletteSource;
#endif
	ft_int16_f4_t LineWidth;
	ft_int16_f4_t PointSize;
	ft_rgb32_t ColorRGB;
	ft_uint8_t ColorA;
	ft_uint8_t Handle; // Current handle
	ft_uint8_t Cell; // Current cell
#if (EVE_SUPPORT_CHIPID >= EVE_FT810)
	ft_uint8_t VertexFormat; // Current vertex format
#endif
	ft_bool_t BitmapTransform; // BitmapTransform other than default is set
} Ft_Esd_GpuState_T;
#endif

//
// Globals
//
extern ESD_EXTERN_LIBARY EVE_HalContext *Ft_Esd_Host;
#if ESD_DL_OPTIMIZE
extern ESD_EXTERN_LIBARY Ft_Esd_GpuState_T Ft_Esd_GpuState[ESD_DL_STATE_STACK_SIZE];
extern ESD_EXTERN_LIBARY ft_uint8_t Ft_Esd_GpuState_I;
extern ESD_EXTERN_LIBARY ft_uint8_t Ft_Esd_Primitive;
// extern ft_uint32_t Esd_CurrentContext->CoFgColor;
// extern ft_uint32_t Esd_CurrentContext->CoBgColor;
#endif
extern ESD_EXTERN_LIBARY Ft_Esd_Rect16 Ft_Esd_ScissorRect;

// Reset any cached state
void Esd_ResetGpuState();
void Esd_ResetCoState();

// Returns the current scissor area
ESD_FUNCTION(Ft_Esd_Dl_Scissor_Get, Type = Ft_Esd_Rect16, Category = EveRenderFunctions)
Ft_Esd_Rect16 Ft_Esd_Dl_Scissor_Get();

// Set scissor area. Cropped to the previous scissor area. Returns previous scissor area
ESD_FUNCTION(Ft_Esd_Dl_Scissor_Set, Type = Ft_Esd_Rect16, Category = EveRenderFunctions, Buffered)
ESD_PARAMETER(rect, Type = Ft_Esd_Rect16)
Ft_Esd_Rect16 Ft_Esd_Dl_Scissor_Set(Ft_Esd_Rect16 rect);

// Set scissor area. Cropped to the previous scissor area which must be passed as an argument
ESD_FUNCTION(Ft_Esd_Dl_Scissor_Adjust, Category = EveRenderFunctions)
ESD_PARAMETER(rect, Type = Ft_Esd_Rect16)
ESD_PARAMETER(state, Type = Ft_Esd_Rect16)
void Ft_Esd_Dl_Scissor_Adjust(Ft_Esd_Rect16 rect, Ft_Esd_Rect16 state);

// Reset scossor area to the previous state which must be passed as an argument
ESD_FUNCTION(Ft_Esd_Dl_Scissor_Reset, Category = EveRenderFunctions)
ESD_PARAMETER(state, Type = Ft_Esd_Rect16)
void Ft_Esd_Dl_Scissor_Reset(Ft_Esd_Rect16 state);

// Set current tag. Must be returned to 255 after usage, to ensure next widgets don't draw with invalid tag
ESD_FUNCTION(Ft_Esd_Dl_TAG, Type = ft_void_t, Category = EveRenderFunctions, Inline)
ESD_PARAMETER(s, Type = ft_uint8_t, DisplayName = "Tag", Default = 255, Min = 0, Max = 255)
inline static ft_void_t Ft_Esd_Dl_TAG(ft_uint8_t s)
{
	Eve_CoCmd_SendCmd(Ft_Esd_Host, TAG(s));
}

// Specify color RGB
ESD_FUNCTION(Ft_Esd_Dl_COLOR_RGB, Type = ft_void_t, Category = EveRenderFunctions, Inline)
ESD_PARAMETER(c, Type = ft_rgb32_t, DisplayName = "Color")
inline static ft_void_t Ft_Esd_Dl_COLOR_RGB(ft_rgb32_t c)
{
	ft_rgb32_t rgb = c & 0xFFFFFF;
#if ESD_DL_OPTIMIZE
	if (rgb != FT_ESD_STATE.ColorRGB)
	{
#endif
		Eve_CoCmd_SendCmd(Ft_Esd_Host, COLOR_RGB(0, 0, 0) | (rgb));
#if ESD_DL_OPTIMIZE
		FT_ESD_STATE.ColorRGB = rgb;
	}
#endif
}

// Specify alpha channel
ESD_FUNCTION(Ft_Esd_Dl_COLOR_A, Type = ft_void_t, Category = EveRenderFunctions, Inline)
ESD_PARAMETER(alpha, Type = ft_uint8_t, Default = 255, Min = 0, Max = 255)
inline static ft_void_t Ft_Esd_Dl_COLOR_A(ft_uint8_t alpha)
{
#if ESD_DL_OPTIMIZE
	if (alpha != FT_ESD_STATE.ColorA)
	{
#endif
		Eve_CoCmd_SendCmd(Ft_Esd_Host, COLOR_A(alpha));
#if ESD_DL_OPTIMIZE
		FT_ESD_STATE.ColorA = alpha;
	}
#endif
}

// Specify color: Alpha(31~24 bit) + RGB(23~0 bit)
ESD_FUNCTION(Ft_Esd_Dl_COLOR_ARGB, Type = ft_void_t, Category = EveRenderFunctions, Inline)
ESD_PARAMETER(c, Type = ft_argb32_t, DisplayName = "Color")
inline static ft_void_t Ft_Esd_Dl_COLOR_ARGB(ft_argb32_t c)
{
	Ft_Esd_Dl_COLOR_RGB(c);
	Ft_Esd_Dl_COLOR_A(c >> 24);
}

// Specify bitmap handle, see BITMAP_HANDLE
ESD_FUNCTION(Ft_Esd_Dl_BITMAP_HANDLE, Type = ft_void_t, Category = EveRenderFunctions, Inline)
ESD_PARAMETER(handle, Type = ft_uint8_t, Min = 0, Max = 31)
inline static ft_void_t Ft_Esd_Dl_BITMAP_HANDLE(ft_uint8_t handle)
{
#if ESD_DL_OPTIMIZE
	if (handle != FT_ESD_STATE.Handle)
	{
#endif
		Eve_CoCmd_SendCmd(Ft_Esd_Host, BITMAP_HANDLE(handle));
#if ESD_DL_OPTIMIZE
		FT_ESD_STATE.Handle = handle;
	}
#endif
}

// Specify cell number for bitmap, see CELL
ESD_FUNCTION(Ft_Esd_Dl_CELL, Type = ft_void_t, Category = EveRenderFunctions, Inline)
ESD_PARAMETER(cell, Type = ft_uint8_t, Min = 0, Max = 255)
inline static ft_void_t Ft_Esd_Dl_CELL(ft_uint8_t cell)
{
#if ESD_DL_OPTIMIZE
	if (cell != FT_ESD_STATE.Cell)
	{
#endif
		Eve_CoCmd_SendCmd(Ft_Esd_Host, CELL(cell));
#if ESD_DL_OPTIMIZE
		FT_ESD_STATE.Cell = cell;
	}
#endif
}

// Set Alpha_Func
ESD_FUNCTION(Ft_Esd_Dl_Alpha_Func, Type = ft_void_t, Category = EveRenderFunctions, Inline)
ESD_PARAMETER(func, Type = ft_uint8_t, Min = 0, Max = 7)
ESD_PARAMETER(ref, Type = ft_uint8_t, Min = 0, Max = 255)
inline static ft_void_t Ft_Esd_Dl_Alpha_Func(ft_uint8_t func, ft_uint8_t ref)
{
	Eve_CoCmd_SendCmd(Ft_Esd_Host, ALPHA_FUNC(func, ref));
}

// Save EVE context, see SAVE_CONTEXT
ESD_FUNCTION(Ft_Esd_Dl_SAVE_CONTEXT, Type = ft_void_t, Category = EveRenderFunctions, Inline)
inline static ft_void_t Ft_Esd_Dl_SAVE_CONTEXT()
{
#if ESD_DL_OPTIMIZE
	ft_uint8_t nextState;
#endif
	Eve_CoCmd_SendCmd(Ft_Esd_Host, SAVE_CONTEXT());
#if ESD_DL_OPTIMIZE
	nextState = Ft_Esd_GpuState_I + 1;
	if (nextState < ESD_DL_STATE_STACK_SIZE)
	{
		Ft_Esd_GpuState[nextState] = Ft_Esd_GpuState[Ft_Esd_GpuState_I];
		Ft_Esd_GpuState_I = nextState;
	}
#endif
}

// Restore EVE context, see RESTORE_CONTEXT
ESD_FUNCTION(Ft_Esd_Dl_RESTORE_CONTEXT, Type = ft_void_t, Category = EveRenderFunctions, Inline)
inline static ft_void_t Ft_Esd_Dl_RESTORE_CONTEXT()
{
	Eve_CoCmd_SendCmd(Ft_Esd_Host, RESTORE_CONTEXT());
#if ESD_DL_OPTIMIZE
	if (Ft_Esd_GpuState_I > 0)
		--Ft_Esd_GpuState_I;
#endif
}

#if (EVE_SUPPORT_CHIPID >= EVE_FT810)
// Specify vertex format , see VERTEX_FORMAT command
ESD_FUNCTION(Ft_Esd_Dl_VERTEX_FORMAT, Type = ft_void_t, Category = EveRenderFunctions, Inline)
ESD_PARAMETER(frac, Type = ft_uint8_t, Min = 0, Max = 4)
inline static ft_void_t Ft_Esd_Dl_VERTEX_FORMAT(ft_uint8_t frac)
{
#if ESD_DL_OPTIMIZE
	if (frac != FT_ESD_STATE.VertexFormat)
	{
#endif
		Eve_CoCmd_SendCmd(Ft_Esd_Host, VERTEX_FORMAT(frac));
#if ESD_DL_OPTIMIZE
		FT_ESD_STATE.VertexFormat = frac;
	}
#endif
}
#else
#define Ft_Esd_Dl_VERTEX_FORMAT(frac) eve_assert(false)
#endif

#if (EVE_SUPPORT_CHIPID >= EVE_FT810)
// Set palette source, see PALETTE_SOURCE command
ESD_FUNCTION(Ft_Esd_Dl_PALETTE_SOURCE, Type = ft_void_t, Category = EveRenderFunctions, Inline)
ESD_PARAMETER(addr, Type = ft_uint32_t, Min = 0)
inline static ft_void_t Ft_Esd_Dl_PALETTE_SOURCE(ft_uint32_t addr)
{
#if ESD_DL_OPTIMIZE
	if (addr != FT_ESD_STATE.PaletteSource)
	{
#endif
		Eve_CoCmd_SendCmd(Ft_Esd_Host, PALETTE_SOURCE(addr));
#if ESD_DL_OPTIMIZE
		FT_ESD_STATE.PaletteSource = addr;
	}
#endif
}
#else
#define Ft_Esd_Dl_PALETTE_SOURCE(addr) eve_assert(false)
#endif

ESD_FUNCTION(Ft_Esd_Dl_LINE_WIDTH, Type = ft_void_t, Category = EveRenderFunctions, Inline)
ESD_PARAMETER(width, Type = ft_int16_f4_t)
inline static ft_void_t Ft_Esd_Dl_LINE_WIDTH(ft_int16_f4_t width)
{
#if ESD_DL_OPTIMIZE
	if (width != FT_ESD_STATE.LineWidth)
	{
#endif
		Eve_CoCmd_SendCmd(Ft_Esd_Host, LINE_WIDTH(width));
#if ESD_DL_OPTIMIZE
		FT_ESD_STATE.LineWidth = width;
	}
#endif
}

ESD_FUNCTION(Ft_Esd_Dl_POINT_SIZE, Type = ft_void_t, Category = EveRenderFunctions, Inline)
ESD_PARAMETER(size, Type = ft_int16_f4_t)
inline static ft_void_t Ft_Esd_Dl_POINT_SIZE(ft_int16_f4_t size)
{
#if ESD_DL_OPTIMIZE
	if (size != FT_ESD_STATE.PointSize)
	{
#endif
		Eve_CoCmd_SendCmd(Ft_Esd_Host, POINT_SIZE(size));
#if ESD_DL_OPTIMIZE
		FT_ESD_STATE.PointSize = size;
	}
#endif
}

ESD_FUNCTION(Ft_Esd_Dl_BEGIN, Type = ft_void_t, Category = EveRenderFunctions, Inline)
ESD_PARAMETER(primitive, Type = ft_uint8_t)
inline static ft_void_t Ft_Esd_Dl_BEGIN(ft_uint8_t primitive)
{
#if ESD_DL_OPTIMIZE
	if (primitive != Ft_Esd_Primitive)
	{
#endif
		Eve_CoCmd_SendCmd(Ft_Esd_Host, BEGIN(primitive));
#if ESD_DL_OPTIMIZE
		Ft_Esd_Primitive = primitive;
	}
#endif
}

ESD_FUNCTION(Ft_Esd_Dl_END, Type = ft_void_t, Category = EveRenderFunctions, Inline)
inline static ft_void_t Ft_Esd_Dl_END()
{
#if ESD_DL_END_PRIMITIVE || !ESD_DL_OPTIMIZE
#if ESD_DL_OPTIMIZE
	if (Ft_Esd_Primitive != 0)
	{
#endif
		Eve_CoCmd_SendCmd(Ft_Esd_Host, END());
#if ESD_DL_OPTIMIZE
		Ft_Esd_Primitive = 0;
	}
#endif
#else
	// For continuous primitives, reset the active primitive.
	// This causes the BEGIN to be called for the next vertex series.
#if ESD_DL_OPTIMIZE
	switch (Ft_Esd_Primitive)
	{
	case LINE_STRIP:
	case EDGE_STRIP_R:
	case EDGE_STRIP_L:
	case EDGE_STRIP_A:
	case EDGE_STRIP_B:
		Ft_Esd_Primitive = 0;
	}
#endif
#endif
}

/* Display list calls without state caching */

// Fixed point vertex with subprecision depending on current vertex format
ESD_FUNCTION(Esd_Dl_VERTEX2F, Type = ft_void_t, Category = EveRenderFunctions, Inline)
ESD_PARAMETER(x, Type = ft_uint16_t)
ESD_PARAMETER(y, Type = ft_uint16_t)
inline static ft_void_t Esd_Dl_VERTEX2F(ft_uint16_t x, ft_uint16_t y)
{
	Eve_CoCmd_SendCmd(Ft_Esd_Host, VERTEX2F(x, y));
}

// Fixed point vertex using 4 bits subprecision
ESD_FUNCTION(Esd_Dl_VERTEX2F_4, Type = ft_void_t, Category = EveRenderFunctions, Inline)
ESD_PARAMETER(x, Type = ft_int16_f4_t)
ESD_PARAMETER(y, Type = ft_int16_f4_t)
inline static ft_void_t Esd_Dl_VERTEX2F_4(ft_int16_f4_t x, ft_int16_f4_t y)
{
	EVE_HalContext *phost = Ft_Esd_Host;
	(void)phost;
	if (EVE_CHIPID >= EVE_FT810)
		Esd_Dl_VERTEX_FORMAT(4);
	Eve_CoCmd_SendCmd(Ft_Esd_Host, VERTEX2F(x, y));
}

// Fixed point vertex using 2 bits subprecision
ESD_FUNCTION(Esd_Dl_VERTEX2F_2, Type = ft_void_t, Category = EveRenderFunctions, Inline)
ESD_PARAMETER(x, Type = ft_int16_f2_t)
ESD_PARAMETER(y, Type = ft_int16_f2_t)
inline static ft_void_t Esd_Dl_VERTEX2F_2(ft_int16_f2_t x, ft_int16_f2_t y)
{
	EVE_HalContext *phost = Ft_Esd_Host;
	(void)phost;
	if (EVE_CHIPID >= EVE_FT810)
	{
		Esd_Dl_VERTEX_FORMAT(2);
	}
	else
	{
		x <<= 2;
		y <<= 2;
	}
	Eve_CoCmd_SendCmd(Ft_Esd_Host, VERTEX2F(x, y));
}

// Fixed point vertex using 0 bits subprecision, or integer point vertex
ESD_FUNCTION(Esd_Dl_VERTEX2F_0, Type = ft_void_t, Category = EveRenderFunctions, Inline)
ESD_PARAMETER(x, Type = ft_uint16_t)
ESD_PARAMETER(y, Type = ft_uint16_t)
inline static ft_void_t Esd_Dl_VERTEX2F_0(ft_uint16_t x, ft_uint16_t y)
{
	EVE_HalContext *phost = Ft_Esd_Host;
	(void)phost;
	if (EVE_CHIPID >= EVE_FT810)
	{
		Esd_Dl_VERTEX_FORMAT(0);
		Eve_CoCmd_SendCmd(Ft_Esd_Host, VERTEX2F(x, y));
	}
	else
	{
		Eve_CoCmd_SendCmd(Ft_Esd_Host, VERTEX2II(x, y, 0, 0));
	}
}

// Display list calls without state caching
ESD_FUNCTION(Esd_Dl_VERTEX2II, Type = ft_void_t, Category = EveRenderFunctions, Inline)
ESD_PARAMETER(x, Type = ft_uint16_t)
ESD_PARAMETER(y, Type = ft_uint16_t)
ESD_PARAMETER(handle, Type = ft_uint8_t)
ESD_PARAMETER(cell, Type = ft_uint8_t)
inline static ft_void_t Esd_Dl_VERTEX2II(ft_uint16_t x, ft_uint16_t y, ft_uint8_t handle, ft_uint8_t cell)
{
	Eve_CoCmd_SendCmd(Ft_Esd_Host, VERTEX2II(x, y, handle, cell));
}

// Specify clear color RGB
ESD_FUNCTION(Esd_Dl_CLEAR_COLOR_RGB, Type = ft_void_t, Category = EveRenderFunctions, Inline)
ESD_PARAMETER(c, Type = ft_rgb32_t, DisplayName = "Color")
inline static ft_void_t Esd_Dl_CLEAR_COLOR_RGB(ft_rgb32_t c)
{
	ft_rgb32_t rgb = c & 0xFFFFFF;
	Eve_CoCmd_SendCmd(Ft_Esd_Host, CLEAR_COLOR_RGB(0, 0, 0) | (rgb));
}

// Specify clear color alpha channel
ESD_FUNCTION(Esd_Dl_CLEAR_COLOR_A, Type = ft_void_t, Category = EveRenderFunctions, Inline)
ESD_PARAMETER(alpha, Type = ft_uint8_t, Default = 255, Min = 0, Max = 255)
inline static ft_void_t Esd_Dl_CLEAR_COLOR_A(ft_uint8_t alpha)
{
	Eve_CoCmd_SendCmd(Ft_Esd_Host, CLEAR_COLOR_A(alpha));
}

// Specify clear color: Alpha(31~24 bit) + RGB(23~0 bit)
ESD_FUNCTION(Esd_Dl_CLEAR_COLOR_ARGB, Type = ft_void_t, Category = EveRenderFunctions, Inline)
ESD_PARAMETER(c, Type = ft_argb32_t, DisplayName = "Color")
inline static ft_void_t Esd_Dl_CLEAR_COLOR_ARGB(ft_argb32_t c)
{
	Esd_Dl_CLEAR_COLOR_RGB(c);
	Esd_Dl_CLEAR_COLOR_A(c >> 24);
}

/* end of supported functions */

// Get the height of builtin font
ESD_FUNCTION(Ft_Esd_GetFontHeight, Type = ft_uint16_t, DisplayName = "Get Font Height", Category = _GroupHidden, Macro)
ESD_PARAMETER(i, Type = int, DisplayName = "Bitmap Handle", Default = 0, Min = 0, Max = 31)
// inline static ft_uint16_t Ft_Esd_GetFontHeight(int i) { return Ft_Esd_FontHeight[i]; }
#define Ft_Esd_GetFontHeight(i) (Esd_GetFontHeight(Esd_GetRomFont(i)));

ESD_FUNCTION(Esd_Dl_CLEAR, Type = ft_void_t, Category = EveRenderFunctions, Inline)
ESD_PARAMETER(c, Type = ft_uint8_t, DisplayName = "Clear Color")
ESD_PARAMETER(s, Type = ft_uint8_t, DisplayName = "Clear Stencil")
ESD_PARAMETER(t, Type = ft_uint8_t, DisplayName = "Clear Tag")
inline static ft_void_t Esd_Dl_CLEAR(ft_uint8_t c, ft_uint8_t s, ft_uint8_t t)
{
	Eve_CoCmd_SendCmd(Ft_Esd_Host, CLEAR(c, s, t));
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

ft_void_t Esd_Dl_BITMAP_SOURCE(ft_uint32_t addr);

ESD_FUNCTION(FT_Esd_Dl_CLEAR_COLOR_RGB, Type = ft_void_t, Include = "Ft_Esd_Dl.h")
ESD_PARAMETER(c, Type = ft_uint32_t, EditType = Color)
ft_void_t FT_Esd_Dl_CLEAR_COLOR_RGB(ft_uint32_t c);

// ft_void_t Esd_Dl_BITMAP_LAYOUT(ft_uint16_t format, ft_uint16_t linestride, ft_uint16_t height);
// ft_void_t Esd_Dl_BITMAP_SIZE(ft_uint16_t filter, ft_uint16_t wrapx, ft_uint16_t wrapy, ft_uint16_t width, ft_uint16_t height);

ft_void_t Esd_Dl_ALPHA_FUNC(ft_uint16_t func, ft_uint16_t ref);
ft_void_t Esd_Dl_STENCIL_FUNC(ft_uint16_t func, ft_uint16_t ref, ft_uint16_t mask);
ft_void_t Esd_Dl_BLEND_FUNC(ft_uint16_t src, ft_uint16_t dst);
ft_void_t Esd_Dl_STENCIL_OP(ft_uint16_t sfail, ft_uint16_t spass);

ft_void_t Esd_Dl_CLEAR_STENCIL(ft_uint8_t s);
ft_void_t Esd_Dl_CLEAR_TAG(ft_uint8_t s);
ft_void_t Esd_Dl_STENCIL_MASK(ft_uint32_t mask);
ft_void_t Esd_Dl_TAG_MASK(ft_uint32_t mask);

ft_void_t Esd_Dl_BITMAP_TRANSFORM_A(ft_uint32_t a);
ft_void_t Esd_Dl_BITMAP_TRANSFORM_B(ft_uint32_t b);
ft_void_t Esd_Dl_BITMAP_TRANSFORM_C(ft_uint32_t c);
ft_void_t Esd_Dl_BITMAP_TRANSFORM_D(ft_uint32_t d);
ft_void_t Esd_Dl_BITMAP_TRANSFORM_E(ft_uint32_t e);
ft_void_t Esd_Dl_BITMAP_TRANSFORM_F(ft_uint32_t f);

// ft_void_t Esd_Dl_CALL(ft_uint16_t dest);
// ft_void_t Esd_Dl_JUMP(ft_uint16_t dest);
// ft_void_t Esd_Dl_RETURN();

// ft_void_t Esd_Dl_MACRO(ft_uint8_t m);

// ft_void_t Esd_Dl_DISPLAY();

ft_void_t Esd_Dl_COLOR_MASK(ft_uint8_t r, ft_uint8_t g, ft_uint8_t b, ft_uint8_t a);

*/

ESD_CATEGORY(EveRenderFunctionsProto, DisplayName = "EVE Render Functions (Prototype)", Category = _GroupHidden)
ESD_CATEGORY(EveRenderDisplayListProto, DisplayName = "Display List", Category = EveRenderFunctionsProto)
ESD_CATEGORY(EveRenderCoprocessorProto, DisplayName = "Coprocessor", Category = EveRenderFunctionsProto)

ESD_CATEGORY(EsdWidgetsProto, DisplayName = "ESD Widgets (Prototype)", Category = _GroupHidden)

ESD_FUNCTION(FT_Esd_Render_Rect_Grad, DisplayName = "Render GRAD RECT ", Include = "Ft_Esd_Dl.h", Category = EveRenderDisplayListProto)
ESD_PARAMETER(x, Type = ft_int16_t, Default = 0)
ESD_PARAMETER(y, Type = ft_int16_t, Default = 0)
ESD_PARAMETER(w, Type = ft_int16_t, Default = 60)
ESD_PARAMETER(h, Type = ft_int16_t, Default = 20)
ESD_PARAMETER(color1, Type = ft_uint32_t, Default = #FFFFFFFF)
ESD_PARAMETER(color2, Type = ft_uint32_t, Default = #FFFFFFFF)
ESD_PARAMETER(direction, Type = ft_int16_t, Default = 90)
void FT_Esd_Render_Rect_Grad(ft_int16_t x, ft_int16_t y, ft_int16_t w, ft_int16_t h, ft_uint32_t color1, ft_uint32_t color2, ft_int16_t direction);

ESD_FUNCTION(FT_Esd_Render_Rect, DisplayName = "Render RECT", Include = "Ft_Esd_Dl.h", Category = EveRenderDisplayListProto)
ESD_PARAMETER(x, Type = ft_int16_t, Default = 0)
ESD_PARAMETER(y, Type = ft_int16_t, Default = 0)
ESD_PARAMETER(w, Type = ft_int16_t, Default = 60)
ESD_PARAMETER(h, Type = ft_int16_t, Default = 20)
void FT_Esd_Render_Rect(ft_int16_t x, ft_int16_t y, ft_int16_t w, ft_int16_t h);

ESD_FUNCTION(Ft_Esd_Cmd_Button, DisplayName = "cmd_button", Include = "Ft_Esd_Dl.h", Category = EveRenderCoprocessorProto)
ESD_PARAMETER(x, Type = ft_int16_t, Default = 0)
ESD_PARAMETER(y, Type = ft_int16_t, Default = 0)
ESD_PARAMETER(w, Type = ft_int16_t, Default = 60)
ESD_PARAMETER(h, Type = ft_int16_t, Default = 20)
ESD_PARAMETER(font, Type = ft_int16_t, Default = 21)
ESD_PARAMETER(options, Type = Ft_CoPro_Opt)
ESD_PARAMETER(s, Type = const ft_char8_t *, Default = "Button")
ft_void_t Ft_Esd_Cmd_Button(ft_int16_t x, ft_int16_t y, ft_int16_t w, ft_int16_t h, ft_int16_t font, ft_uint16_t options, const ft_char8_t *s);

ESD_FUNCTION(Ft_Esd_Cmd_Number, DisplayName = "cmd_number", Include = "Ft_Esd_Dl.h", Category = EveRenderCoprocessorProto)
ESD_PARAMETER(x, Type = ft_int16_t, Default = 0)
ESD_PARAMETER(y, Type = ft_int16_t, Default = 0)
ESD_PARAMETER(font, Type = ft_int16_t, Default = 21)
ESD_PARAMETER(options, Type = Ft_CoPro_Opt)
ESD_PARAMETER(n, Type = ft_int32_t, Default = 0)
ft_void_t Ft_Esd_Cmd_Number(ft_int16_t x, ft_int16_t y, ft_int16_t font, ft_uint16_t options, ft_int32_t n);

ESD_FUNCTION(Ft_Gpu_Hal_Rd32, Type = ft_uint32_t, Buffered, Include = "Ft_Gpu_Hal.h", Category = _GroupHidden)
ESD_PARAMETER(phost, Type = EVE_HalContext *, Default = Ft_Esd_GetHost, Hidden, Internal, Static)
ESD_PARAMETER(addr, Type = ft_uint32_t, DisplayName = "Address")

ESD_FUNCTION(Eve_CoCmd_SendCmd, Include = "Ft_Gpu_Hal.h", Category = _GroupHidden)
ESD_PARAMETER(phost, Type = EVE_HalContext *, Default = Ft_Esd_GetHost, Hidden, Internal, Static)
ESD_PARAMETER(cmd, Type = ft_uint32_t)

#endif /* #ifndef FT_ESD_DL_H */

/* end of file */
