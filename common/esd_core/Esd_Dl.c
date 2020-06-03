
#include "Ft_Platform.h"
#include "ESD_Dl.h"

#include "ESD_Core.h"

#include <stdio.h>
#define _USE_MATH_DEFINES 1
#include <math.h>

extern EVE_HalContext *ESD_Host;
/* extern ESD_GpuAlloc *ESD_GAlloc; */
extern int16_t ESD_DispWidth, ESD_DispHeight;

// GPU state for the current display list
#if ESD_DL_OPTIMIZE
ESD_GpuState_T ESD_GpuState[ESD_DL_STATE_STACK_SIZE];
ft_uint8_t ESD_GpuState_I;
ft_uint8_t ESD_Primitive;
// ft_uint32_t Esd_CurrentContext->CoFgColor;
// ft_uint32_t Esd_CurrentContext->CoBgColor;
#endif
ESD_Rect16 ESD_ScissorRect;

void Esd_ResetGpuState() // Begin of frame
{
#if ESD_DL_OPTIMIZE
	ESD_GpuState_I = 0;
	ESD_GpuState[0] = (ESD_GpuState_T)
	{
		.LineWidth = 16,
		.PointSize = 16,
		.ColorRGB = 0xFFFFFF,
		.ColorA = 0xFF,
		.Handle = ESD_BITMAPHANDLE_INVALID,
#if (EVE_SUPPORT_CHIPID >= EVE_FT810)
		.VertexFormat = 4,
#endif
	};

	ESD_Primitive = 0;
#endif

	// Reset scissor state to display size
	ESD_ScissorRect.X = 0;
	ESD_ScissorRect.Y = 0;
	ESD_ScissorRect.Width = ESD_DispWidth;
	ESD_ScissorRect.Height = ESD_DispHeight;
}

void Esd_ResetCoState()
{
#if ESD_DL_OPTIMIZE
	Esd_CurrentContext->CoFgColor = 0x003870;
	Esd_CurrentContext->CoBgColor = 0x002040;
#endif

#if (EVE_SUPPORT_CHIPID >= EVE_FT810)
	Esd_CurrentContext->CoScratchHandle = 15;
#endif
}

ESD_Rect16 ESD_Dl_Scissor_Get()
{
	return ESD_ScissorRect;
}

ESD_Rect16 ESD_Dl_Scissor_Set(ESD_Rect16 rect)
{
	ESD_Rect16 state = ESD_ScissorRect;
	ESD_Dl_Scissor_Adjust(rect, state);
	return state;
}

void ESD_Dl_Scissor_Adjust(ESD_Rect16 rect, ESD_Rect16 state)
{
	EVE_HalContext *phost = ESD_Host;
	(void)phost;

	ft_int16_t x1diff;
	ft_int16_t y1diff;
	ft_int16_t x2diff;
	ft_int16_t y2diff;

	x1diff = (state.X - rect.X); // old x1 - new x1
	if (x1diff > 0) // old x1 > new x1
	{
		rect.Width -= x1diff;
		rect.X = state.X;
	}
	y1diff = (state.Y - rect.Y); // old y1 - new y1
	if (y1diff > 0) // old y1 > new y1
	{
		rect.Height -= y1diff;
		rect.Y = state.Y;
	}
	x2diff = (state.X + state.Width) - (rect.X + rect.Width); // old x2 - new x2
	if (x2diff < 0) // old x2 < new x2
	{
		rect.Width += x2diff;
	}
	y2diff = (state.Y + state.Height) - (rect.Y + rect.Height); // old y2 - new y2
	if (y2diff < 0) // old y2 < new y2
	{
		rect.Height += y2diff;
	}
	// Ft_Gpu_CoCmd_StartFunc(ESD_Host, FT_CMD_SIZE * 2);
	if (ESD_ScissorRect.X != rect.X || ESD_ScissorRect.Y != rect.Y)
		Ft_Gpu_CoCmd_SendCmd(ESD_Host, SCISSOR_XY(rect.X, rect.Y));
	if (ESD_ScissorRect.Width != rect.Width || ESD_ScissorRect.Height != rect.Height)
		Ft_Gpu_CoCmd_SendCmd(ESD_Host, SCISSOR_SIZE(rect.Width, rect.Height));
	// Ft_Gpu_CoCmd_EndFunc(ESD_Host);
	ESD_ScissorRect = rect;
}

void ESD_Dl_Scissor_Reset(ESD_Rect16 state)
{
	// Ft_Gpu_CoCmd_StartFunc(ESD_Host, FT_CMD_SIZE * 2);
	EVE_HalContext *phost = ESD_Host;
	(void)phost;
	if (ESD_ScissorRect.X != state.X || ESD_ScissorRect.Y != state.Y)
		Ft_Gpu_CoCmd_SendCmd(ESD_Host, SCISSOR_XY(state.X, state.Y));
	if (ESD_ScissorRect.Width != state.Width || ESD_ScissorRect.Height != state.Height)
		Ft_Gpu_CoCmd_SendCmd(ESD_Host, SCISSOR_SIZE(state.Width, state.Height));
	// Ft_Gpu_CoCmd_EndFunc(ESD_Host);
	ESD_ScissorRect = state;
}

/* end of supported functions */

// Deprecated
void FT_Esd_Render_Rect_Grad(ft_int16_t x, ft_int16_t y, ft_int16_t w, ft_int16_t h, ft_argb32_t color1, ft_argb32_t color2, ft_int16_t direction)
{
	EVE_HalContext *phost = ESD_Host;
	(void)phost;

	// FIXME: Use rect for parameters
	ESD_Rect16 rect = {
		.X = x,
		.Y = y,
		.Width = w,
		.Height = h,
	};

	if (color1 == color2)
	{
		// Not a gradient
		ESD_Rect16 scissor;
		// Ft_Gpu_CoCmd_StartFunc(ESD_Host, FT_CMD_SIZE * 4);
		Ft_Gpu_CoCmd_SendCmd(ESD_Host, SAVE_CONTEXT());
		scissor = ESD_Rect16_Crop(rect, ESD_Dl_Scissor_Get());
		Ft_Gpu_CoCmd_SendCmd(ESD_Host, SCISSOR_XY(scissor.X, scissor.Y));
		Ft_Gpu_CoCmd_SendCmd(ESD_Host, SCISSOR_SIZE(scissor.Width, scissor.Height));
		ESD_Dl_COLOR_ARGB(color1);
		FT_Esd_Render_Rect(x, y, w, h);
		Ft_Gpu_CoCmd_SendCmd(ESD_Host, RESTORE_CONTEXT());
		// Ft_Gpu_CoCmd_EndFunc(ESD_Host);
		return;
	}

	scope
	{
		double radius = direction * M_PI / 180.0f;
		double sine = sin(radius), cosine = cos(radius);
		ft_int16_t x0 = x + (w >> 1);
		ft_int16_t y0 = y + (h >> 1);
		ft_int16_t l = (ft_int16_t)(sqrt(w * w + h * h) * 0.8); // use 80% to apply gradient effect
		ft_int16_t half = l >> 1;
		ft_int16_t dy = (ft_int16_t)(half * sine);
		ft_int16_t dx = (ft_int16_t)(half * cosine);

		ESD_Rect16 s = ESD_Dl_Scissor_Set(rect);
		Ft_Gpu_CoCmd_Gradient(ESD_Host, x0 - dx, y0 - dy, color1, x0 + dx, y0 + dy, color2);
		ESD_Dl_Scissor_Reset(s);
	}
}

// Deprecated
void FT_Esd_Render_Rect(ft_int16_t x, ft_int16_t y, ft_int16_t w, ft_int16_t h)
{
	ESD_Dl_BEGIN(RECTS);
	Ft_Gpu_CoCmd_SendCmd(ESD_Host, VERTEX2II(x, y, 0, 0));
	Ft_Gpu_CoCmd_SendCmd(ESD_Host, VERTEX2II(x + w, y + h, 0, 0));
	ESD_Dl_END();
}

// Deprecated
ft_void_t ESD_Cmd_Button(ft_int16_t x, ft_int16_t y, ft_int16_t w, ft_int16_t h, ft_int16_t font, ft_uint16_t options, const ft_char8_t *s)
{
	Ft_Gpu_CoCmd_Button(ESD_Host, x, y, w, h, font, options, s);
}

// Deprecated
ft_void_t ESD_Cmd_Number(ft_int16_t x, ft_int16_t y, ft_int16_t font, ft_uint16_t options, ft_int32_t n)
{
	Ft_Gpu_CoCmd_Number(ESD_Host, x, y, font, options, n);
}

/* end of file */
