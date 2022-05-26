
#ifndef FT_ESD_COMPATIBILITY_DL_H
#define FT_ESD_COMPATIBILITY_DL_H

#include "Esd_Core.h"

#include <Ft_Platform.h>
#include "Ft_CoPro_Cmds.h"

#include "Ft_Esd.h"
#include "FT_Gpu.h"
#include "Ft_Esd_BitmapInfo.h"
#include "Ft_Esd_Math.h"
#include "Ft_Esd_BitmapHandle.h"

#define ESD_DL_OPTIMIZE EVE_DL_OPTIMIZE
#define ESD_DL_STATE_STACK_SIZE EVE_DL_STATE_STACK_SIZE
#define ESD_DL_END_PRIMITIVE EVE_DL_END_PRIMITIVE

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

#if EVE_DL_OPTIMIZE
/* Ft_Esd_GpuState */
/* Ft_Esd_GpuState_I */
/* Ft_Esd_Primitive */
#endif
#define Ft_Esd_ScissorRect Esd_ScissorRect

#define Esd_ResetGpuState EVE_CoDlImpl_resetDlState
#define Esd_ResetCoState EVE_CoDlImpl_resetCoState

ESD_FUNCTION(Ft_Esd_Dl_Scissor_Get, Type = Ft_Esd_Rect16, Category = _GroupHidden, Macro)
#define Ft_Esd_Dl_Scissor_Get Esd_Scissor_Get

ESD_FUNCTION(Ft_Esd_Dl_Scissor_Set, Type = Ft_Esd_Rect16, Category = _GroupHidden, Macro)
ESD_PARAMETER(rect, Type = Ft_Esd_Rect16)
#define Ft_Esd_Dl_Scissor_Set Esd_Scissor_Set

ESD_FUNCTION(Ft_Esd_Dl_Scissor_Adjust, Category = _GroupHidden, Macro)
ESD_PARAMETER(rect, Type = Ft_Esd_Rect16)
ESD_PARAMETER(state, Type = Ft_Esd_Rect16)
#define Ft_Esd_Dl_Scissor_Adjust Esd_Scissor_Adjust

ESD_FUNCTION(Ft_Esd_Dl_Scissor_Reset, Category = _GroupHidden, Macro)
ESD_PARAMETER(state, Type = Ft_Esd_Rect16)
#define Ft_Esd_Dl_Scissor_Reset Esd_Scissor_Reset

ESD_FUNCTION(Ft_Esd_Dl_TAG, Type = ft_void_t, Category = _GroupHidden, Macro)
ESD_PARAMETER(s, Type = ft_uint8_t, DisplayName = "Tag", Default = 255, Min = 0, Max = 255)
#define Ft_Esd_Dl_TAG(s) EVE_CoDl_tag(Esd_GetHost(), s)

ESD_FUNCTION(Ft_Esd_Dl_COLOR_RGB, Type = ft_void_t, Category = _GroupHidden, Macro)
ESD_PARAMETER(c, Type = ft_rgb32_t, DisplayName = "Color")
#define Ft_Esd_Dl_COLOR_RGB(c) EVE_CoDl_colorRgb_ex(Esd_GetHost(), c)

ESD_FUNCTION(Ft_Esd_Dl_COLOR_A, Type = ft_void_t, Category = _GroupHidden, Macro)
ESD_PARAMETER(alpha, Type = ft_uint8_t, Default = 255, Min = 0, Max = 255)
#define Ft_Esd_Dl_COLOR_A(alpha) EVE_CoDl_colorA(Esd_GetHost(), alpha)

ESD_FUNCTION(Ft_Esd_Dl_COLOR_ARGB, Type = ft_void_t, Category = _GroupHidden, Macro)
ESD_PARAMETER(c, Type = ft_argb32_t, DisplayName = "Color")
#define Ft_Esd_Dl_COLOR_ARGB(c) EVE_CoDl_colorArgb_ex(Esd_GetHost(), c)

ESD_FUNCTION(Ft_Esd_Dl_BITMAP_HANDLE, Type = ft_void_t, Category = _GroupHidden, Macro)
ESD_PARAMETER(handle, Type = ft_uint8_t, Min = 0, Max = 31)
#define Ft_Esd_Dl_BITMAP_HANDLE(handle) EVE_CoDl_bitmapHandle(Esd_GetHost(), handle)

ESD_FUNCTION(Ft_Esd_Dl_CELL, Type = ft_void_t, Category = _GroupHidden, Macro)
ESD_PARAMETER(cell, Type = ft_uint8_t, Min = 0, Max = 255)
#define Ft_Esd_Dl_CELL EVE_CoDl_cell(Esd_GetHost(), cell)

ESD_FUNCTION(Ft_Esd_Dl_Alpha_Func, Type = ft_void_t, Category = _GroupHidden, Macro)
ESD_PARAMETER(func, Type = ft_uint8_t, Min = 0, Max = 7)
ESD_PARAMETER(ref, Type = ft_uint8_t, Min = 0, Max = 255)
#define Ft_Esd_Dl_Alpha_Func(func, ref) EVE_CoDl_alphaFunc(Esd_GetHost(), func, ref)

ESD_FUNCTION(Ft_Esd_Dl_SAVE_CONTEXT, Type = ft_void_t, Category = _GroupHidden, Macro)
#define Ft_Esd_Dl_SAVE_CONTEXT() EVE_CoDl_saveContext(Esd_GetHost())

ESD_FUNCTION(Ft_Esd_Dl_RESTORE_CONTEXT, Type = ft_void_t, Category = _GroupHidden, Macro)
#define Ft_Esd_Dl_RESTORE_CONTEXT() EVE_CoDl_restoreContext(Esd_GetHost())

ESD_FUNCTION(Ft_Esd_Dl_VERTEX_FORMAT, Type = ft_void_t, Category = _GroupHidden, Macro)
ESD_PARAMETER(frac, Type = ft_uint8_t, Min = 0, Max = 4)
#define Ft_Esd_Dl_VERTEX_FORMAT(frac) EVE_CoDl_vertexFormat(Esd_GetHost(), frac)

ESD_FUNCTION(Ft_Esd_Dl_PALETTE_SOURCE, Type = ft_void_t, Category = _GroupHidden, Macro)
ESD_PARAMETER(addr, Type = ft_uint32_t, Min = 0)
#define Ft_Esd_Dl_PALETTE_SOURCE(addr) EVE_CoDl_paletteSource(Esd_GetHost(), addr)

ESD_FUNCTION(Ft_Esd_Dl_LINE_WIDTH, Type = ft_void_t, Category = _GroupHidden, Macro)
ESD_PARAMETER(width, Type = ft_int16_f4_t)
#define Ft_Esd_Dl_LINE_WIDTH(width) EVE_CoDl_lineWidth(Esd_GetHost(), width)

ESD_FUNCTION(Ft_Esd_Dl_POINT_SIZE, Type = ft_void_t, Category = _GroupHidden, Macro)
ESD_PARAMETER(size, Type = ft_int16_f4_t)
#define Ft_Esd_Dl_POINT_SIZE(size) EVE_CoDl_pointSize(Esd_GetHost(), size)

ESD_FUNCTION(Ft_Esd_Dl_BEGIN, Type = ft_void_t, Category = _GroupHidden, Macro)
ESD_PARAMETER(primitive, Type = ft_uint8_t)
#define Ft_Esd_Dl_BEGIN(primitive) EVE_CoDl_begin(Esd_GetHost(), primitive)

ESD_FUNCTION(Ft_Esd_Dl_END, Type = ft_void_t, Category = _GroupHidden, Macro)
#define Ft_Esd_Dl_END() EVE_CoDl_end(Esd_GetHost())

ESD_FUNCTION(Esd_Dl_VERTEX2F, Type = ft_void_t, Category = _GroupHidden, Macro)
ESD_PARAMETER(x, Type = ft_uint16_t)
ESD_PARAMETER(y, Type = ft_uint16_t)
#define Esd_Dl_VERTEX2F(x, y) EVE_CoDl_vertex2f(Esd_GetHost(), x, y)

ESD_FUNCTION(Esd_Dl_VERTEX2F_4, Type = ft_void_t, Category = _GroupHidden, Macro)
ESD_PARAMETER(x, Type = ft_int16_f4_t)
ESD_PARAMETER(y, Type = ft_int16_f4_t)
#define Esd_Dl_VERTEX2F_4(x, y) EVE_CoDl_vertex2f_4(Esd_GetHost(), x, y)

ESD_FUNCTION(Esd_Dl_VERTEX2F_2, Type = ft_void_t, Category = _GroupHidden, Macro)
ESD_PARAMETER(x, Type = ft_int16_f2_t)
ESD_PARAMETER(y, Type = ft_int16_f2_t)
#define Esd_Dl_VERTEX2F_2(x, y) EVE_CoDl_vertex2f_2(Esd_GetHost(), x, y)

ESD_FUNCTION(Esd_Dl_VERTEX2F_0, Type = ft_void_t, Category = _GroupHidden, Macro)
ESD_PARAMETER(x, Type = ft_uint16_t)
ESD_PARAMETER(y, Type = ft_uint16_t)
#define Esd_Dl_VERTEX2F_0(x, y) EVE_CoDl_vertex2f_0(Esd_GetHost(), x, y)

ESD_FUNCTION(Esd_Dl_VERTEX2II, Type = ft_void_t, Category = _GroupHidden, Macro)
ESD_PARAMETER(x, Type = ft_uint16_t)
ESD_PARAMETER(y, Type = ft_uint16_t)
ESD_PARAMETER(handle, Type = ft_uint8_t)
ESD_PARAMETER(cell, Type = ft_uint8_t)
#define Esd_Dl_VERTEX2II(x, y, handle, cell) EVE_CoDl_vertex2ii(Esd_GetHost(), x, y, handle, cell)

ESD_FUNCTION(Esd_Dl_CLEAR_COLOR_RGB, Type = ft_void_t, Category = _GroupHidden, Macro)
ESD_PARAMETER(c, Type = ft_rgb32_t, DisplayName = "Color")
#define Esd_Dl_CLEAR_COLOR_RGB(c) EVE_CoDl_clearColorRgb_ex(Esd_GetHost(), c)

ESD_FUNCTION(Esd_Dl_CLEAR_COLOR_A, Type = ft_void_t, Category = _GroupHidden, Macro)
ESD_PARAMETER(alpha, Type = ft_uint8_t, Default = 255, Min = 0, Max = 255)
#define Esd_Dl_CLEAR_COLOR_A(alpha) EVE_CoDl_clearColorA(Esd_GetHost(), alpha)

ESD_FUNCTION(Esd_Dl_CLEAR_COLOR_ARGB, Type = ft_void_t, Category = EveRenderFunctions, _GroupHidden, Macro)
ESD_PARAMETER(c, Type = ft_argb32_t, DisplayName = "Color")
#define Esd_Dl_CLEAR_COLOR_ARGB(c) EVE_CoDl_clearColorArgb_ex(Esd_GetHost(), c)

ESD_FUNCTION(Ft_Esd_GetFontHeight, Type = ft_uint16_t, DisplayName = "Get Font Height", Category = _GroupHidden, Macro)
ESD_PARAMETER(i, Type = int, DisplayName = "Bitmap Handle", Default = 0, Min = 0, Max = 31)
#define Ft_Esd_GetFontHeight ESD_GetRomFontHeight

ESD_FUNCTION(Esd_Dl_CLEAR, Type = ft_void_t, Category = _GroupHidden, Macro)
ESD_PARAMETER(c, Type = ft_uint8_t, DisplayName = "Clear Color")
ESD_PARAMETER(s, Type = ft_uint8_t, DisplayName = "Clear Stencil")
ESD_PARAMETER(t, Type = ft_uint8_t, DisplayName = "Clear Tag")
#define Esd_Dl_CLEAR(c, s, t) EVE_CoDl_clear(Esd_GetHost(), c, s, t)

/*
ESD_FUNCTION(FT_Esd_Render_Rect_Grad, DisplayName = "Render GRAD RECT ", Include = "FT_Esd_Dl.h", Category = _GroupHidden, Macro)
ESD_PARAMETER(x, Type = ft_int16_t, Default = 0)
ESD_PARAMETER(y, Type = ft_int16_t, Default = 0)
ESD_PARAMETER(w, Type = ft_int16_t, Default = 60)
ESD_PARAMETER(h, Type = ft_int16_t, Default = 20)
ESD_PARAMETER(color1, Type = ft_uint32_t, Default = #FFFFFFFF)
ESD_PARAMETER(color2, Type = ft_uint32_t, Default = #FFFFFFFF)
ESD_PARAMETER(direction, Type = ft_int16_t, Default = 90)
#define FT_Esd_Render_Rect_Grad(x, y, w, h, color1, color2, direction)
*/

/*
ESD_FUNCTION(FT_Esd_Render_Rect, DisplayName = "Render RECT", Include = "FT_Esd_Dl.h", Category = _GroupHidden, Macro)
ESD_PARAMETER(x, Type = ft_int16_t, Default = 0)
ESD_PARAMETER(y, Type = ft_int16_t, Default = 0)
ESD_PARAMETER(w, Type = ft_int16_t, Default = 60)
ESD_PARAMETER(h, Type = ft_int16_t, Default = 20)
#define FT_Esd_Render_Rect(x, y, w, h)
*/

ESD_FUNCTION(Ft_Esd_Cmd_Button, DisplayName = "cmd_button", Include = "FT_Esd_Dl.h", Category = _GroupHidden, Macro)
ESD_PARAMETER(x, Type = ft_int16_t, Default = 0)
ESD_PARAMETER(y, Type = ft_int16_t, Default = 0)
ESD_PARAMETER(w, Type = ft_int16_t, Default = 60)
ESD_PARAMETER(h, Type = ft_int16_t, Default = 20)
ESD_PARAMETER(font, Type = ft_int16_t, Default = 21)
ESD_PARAMETER(options, Type = Ft_CoPro_Opt)
ESD_PARAMETER(s, Type = const ft_char8_t *, Default = "Button")
#define Ft_Esd_Cmd_Button(x, y, w, h, font, options, s) EVE_CoCmd_button(Esd_GetHost(), x, y, w, h, font, options, s)

ESD_FUNCTION(Ft_Esd_Cmd_Number, DisplayName = "cmd_number", Include = "FT_Esd_Dl.h", Category = _GroupHidden, Macro)
ESD_PARAMETER(x, Type = ft_int16_t, Default = 0)
ESD_PARAMETER(y, Type = ft_int16_t, Default = 0)
ESD_PARAMETER(font, Type = ft_int16_t, Default = 21)
ESD_PARAMETER(options, Type = Ft_CoPro_Opt)
ESD_PARAMETER(n, Type = ft_int32_t, Default = 0)
#define Ft_Esd_Cmd_Number(x, y, font, options, n) EVE_CoCmd_number(Esd_GetHost(), x, y, font, options, n)

ESD_FUNCTION(Ft_Gpu_Hal_Rd32, Type = ft_uint32_t, Buffered, Include = "FT_Gpu_Hal.h", Category = _GroupHidden, Macro)
ESD_PARAMETER(phost, Type = EVE_HalContext *, Default = Ft_Esd_GetHost, Hidden, Internal, Static)
ESD_PARAMETER(addr, Type = ft_uint32_t, DisplayName = "Address")

ESD_FUNCTION(Eve_CoCmd_SendCmd, Include = "FT_Gpu_Hal.h", Category = _GroupHidden, Macro)
ESD_PARAMETER(phost, Type = EVE_HalContext *, Default = Ft_Esd_GetHost, Hidden, Internal, Static)
ESD_PARAMETER(cmd, Type = ft_uint32_t)

#endif /* #ifndef FT_ESD_DL_H */

/* end of file */
