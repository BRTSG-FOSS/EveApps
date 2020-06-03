
#include "Ft_Platform.h"
#include "Ft_Esd_Dl.h"

#include "Ft_Esd_Core.h"

#include <stdio.h>
#define _USE_MATH_DEFINES 1
#include <math.h>

extern EVE_HalContext *Ft_Esd_Host;
/* extern Ft_Esd_GpuAlloc *Ft_Esd_GAlloc; */
extern int16_t ESD_DispWidth, ESD_DispHeight;

// GPU state for the current display list
#if ESD_DL_OPTIMIZE
Ft_Esd_GpuState_T Ft_Esd_GpuState[ESD_DL_STATE_STACK_SIZE];
ft_uint8_t Ft_Esd_GpuState_I;
ft_uint8_t Ft_Esd_Primitive;
// ft_uint32_t Esd_CurrentContext->CoFgColor;
// ft_uint32_t Esd_CurrentContext->CoBgColor;
#endif
Ft_Esd_Rect16 Ft_Esd_ScissorRect;

void Esd_ResetGpuState() // Begin of frame
{
#if ESD_DL_OPTIMIZE
	Ft_Esd_GpuState_I = 0;
	Ft_Esd_GpuState[0] = (Ft_Esd_GpuState_T)
	{
		.LineWidth = 16,
		.PointSize = 16,
		.ColorRGB = 0xFFFFFF,
		.ColorA = 0xFF,
		.Handle = FT_ESD_BITMAPHANDLE_INVALID,
#if (EVE_SUPPORT_CHIPID >= EVE_FT810)
		.VertexFormat = 4,
#endif
	};

	Ft_Esd_Primitive = 0;
#endif

	// Reset scissor state to display size
	Ft_Esd_ScissorRect.X = 0;
	Ft_Esd_ScissorRect.Y = 0;
	Ft_Esd_ScissorRect.Width = ESD_DispWidth;
	Ft_Esd_ScissorRect.Height = ESD_DispHeight;
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

Ft_Esd_Rect16 Ft_Esd_Dl_Scissor_Get()
{
	return Ft_Esd_ScissorRect;
}

Ft_Esd_Rect16 Ft_Esd_Dl_Scissor_Set(Ft_Esd_Rect16 rect)
{
	Ft_Esd_Rect16 state = Ft_Esd_ScissorRect;
	Ft_Esd_Dl_Scissor_Adjust(rect, state);
	return state;
}

void Ft_Esd_Dl_Scissor_Adjust(Ft_Esd_Rect16 rect, Ft_Esd_Rect16 state)
{
	EVE_HalContext *phost = Ft_Esd_Host;
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
	// Ft_Gpu_CoCmd_StartFunc(Ft_Esd_Host, FT_CMD_SIZE * 2);
	if (Ft_Esd_ScissorRect.X != rect.X || Ft_Esd_ScissorRect.Y != rect.Y)
		Ft_Gpu_CoCmd_SendCmd(Ft_Esd_Host, SCISSOR_XY(rect.X, rect.Y));
	if (Ft_Esd_ScissorRect.Width != rect.Width || Ft_Esd_ScissorRect.Height != rect.Height)
		Ft_Gpu_CoCmd_SendCmd(Ft_Esd_Host, SCISSOR_SIZE(rect.Width, rect.Height));
	// Ft_Gpu_CoCmd_EndFunc(Ft_Esd_Host);
	Ft_Esd_ScissorRect = rect;
}

void Ft_Esd_Dl_Scissor_Reset(Ft_Esd_Rect16 state)
{
	// Ft_Gpu_CoCmd_StartFunc(Ft_Esd_Host, FT_CMD_SIZE * 2);
	EVE_HalContext *phost = Ft_Esd_Host;
	(void)phost;
	if (Ft_Esd_ScissorRect.X != state.X || Ft_Esd_ScissorRect.Y != state.Y)
		Ft_Gpu_CoCmd_SendCmd(Ft_Esd_Host, SCISSOR_XY(state.X, state.Y));
	if (Ft_Esd_ScissorRect.Width != state.Width || Ft_Esd_ScissorRect.Height != state.Height)
		Ft_Gpu_CoCmd_SendCmd(Ft_Esd_Host, SCISSOR_SIZE(state.Width, state.Height));
	// Ft_Gpu_CoCmd_EndFunc(Ft_Esd_Host);
	Ft_Esd_ScissorRect = state;
}

/* end of supported functions */

// Deprecated
void FT_Esd_Render_Rect_Grad(ft_int16_t x, ft_int16_t y, ft_int16_t w, ft_int16_t h, ft_argb32_t color1, ft_argb32_t color2, ft_int16_t direction)
{
	EVE_HalContext *phost = Ft_Esd_Host;
	(void)phost;

	// FIXME: Use rect for parameters
	Ft_Esd_Rect16 rect = {
		.X = x,
		.Y = y,
		.Width = w,
		.Height = h,
	};

	if (color1 == color2)
	{
		// Not a gradient
		Ft_Esd_Rect16 scissor;
		// Ft_Gpu_CoCmd_StartFunc(Ft_Esd_Host, FT_CMD_SIZE * 4);
		Ft_Gpu_CoCmd_SendCmd(Ft_Esd_Host, SAVE_CONTEXT());
		scissor = Ft_Esd_Rect16_Crop(rect, Ft_Esd_Dl_Scissor_Get());
		Ft_Gpu_CoCmd_SendCmd(Ft_Esd_Host, SCISSOR_XY(scissor.X, scissor.Y));
		Ft_Gpu_CoCmd_SendCmd(Ft_Esd_Host, SCISSOR_SIZE(scissor.Width, scissor.Height));
		Ft_Esd_Dl_COLOR_ARGB(color1);
		FT_Esd_Render_Rect(x, y, w, h);
		Ft_Gpu_CoCmd_SendCmd(Ft_Esd_Host, RESTORE_CONTEXT());
		// Ft_Gpu_CoCmd_EndFunc(Ft_Esd_Host);
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

		Ft_Esd_Rect16 s = Ft_Esd_Dl_Scissor_Set(rect);
		Ft_Gpu_CoCmd_Gradient(Ft_Esd_Host, x0 - dx, y0 - dy, color1, x0 + dx, y0 + dy, color2);
		Ft_Esd_Dl_Scissor_Reset(s);
	}
}

// Deprecated
void FT_Esd_Render_Rect(ft_int16_t x, ft_int16_t y, ft_int16_t w, ft_int16_t h)
{
	Ft_Esd_Dl_BEGIN(RECTS);
	Ft_Gpu_CoCmd_SendCmd(Ft_Esd_Host, VERTEX2II(x, y, 0, 0));
	Ft_Gpu_CoCmd_SendCmd(Ft_Esd_Host, VERTEX2II(x + w, y + h, 0, 0));
	Ft_Esd_Dl_END();
}

// Deprecated
ft_void_t Ft_Esd_Cmd_Button(ft_int16_t x, ft_int16_t y, ft_int16_t w, ft_int16_t h, ft_int16_t font, ft_uint16_t options, const ft_char8_t *s)
{
	Ft_Gpu_CoCmd_Button(Ft_Esd_Host, x, y, w, h, font, options, s);
}

// Deprecated
ft_void_t Ft_Esd_Cmd_Number(ft_int16_t x, ft_int16_t y, ft_int16_t font, ft_uint16_t options, ft_int32_t n)
{
	Ft_Gpu_CoCmd_Number(Ft_Esd_Host, x, y, font, options, n);
}

/* end of file */
