
#include "Esd_Scissor.h"

#include "Esd_Context.h"

#include <stdio.h>
#define _USE_MATH_DEFINES 1
#include <math.h>

ESD_CORE_EXPORT Esd_Rect16 Esd_Scissor_Get()
{
	EVE_HalContext *phost = Esd_GetHost();

	return (Esd_Rect16) { { { EVE_DL_STATE.ScissorX,
		                      EVE_DL_STATE.ScissorY } },
		{ { EVE_DL_STATE.ScissorWidth,
		    EVE_DL_STATE.ScissorHeight } } };
}

ESD_CORE_EXPORT Esd_Rect16 Esd_Scissor_Set(Esd_Rect16 rect)
{
	Esd_Rect16 state = Esd_Scissor_Get();
	Esd_Scissor_Adjust(rect, state);
	return state;
}

ESD_CORE_EXPORT void Esd_Scissor_Adjust(Esd_Rect16 rect, Esd_Rect16 state)
{
	EVE_HalContext *phost = Esd_GetHost();
	int16_t x1diff;
	int16_t y1diff;
	int16_t x2diff;
	int16_t y2diff;

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
	if (rect.Width < 0)
		rect.Width = 0;
	if (rect.Height < 0)
		rect.Height = 0;

	EVE_CoDl_scissorXY(phost, rect.X, rect.Y);
	EVE_CoDl_scissorSize(phost, rect.Width, rect.Height);
}

ESD_CORE_EXPORT void Esd_Scissor_Reset(Esd_Rect16 state)
{
	EVE_HalContext *phost = Esd_GetHost();

	EVE_CoDl_scissorXY(phost, state.X, state.Y);
	EVE_CoDl_scissorSize(phost, state.Width, state.Height);
}

/* end of file */
