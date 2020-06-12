
#include "ESD_Scissor.h"

#include "ESD_Context.h"

#include <stdio.h>
#define _USE_MATH_DEFINES 1
#include <math.h>

// GPU state for the current display list
ESD_CORE_EXPORT ESD_Rect16 ESD_ScissorRect;

void ESD_Scissor_DlStart() // Begin of frame
{
	EVE_HalContext *phost = ESD_GetHost();

	// Reset scissor state to display size
	ESD_ScissorRect.X = 0;
	ESD_ScissorRect.Y = 0;
	ESD_ScissorRect.Width = phost->Width;
	ESD_ScissorRect.Height = phost->Height;
}

ESD_CORE_EXPORT ESD_Rect16 ESD_Scissor_Get()
{
	return ESD_ScissorRect;
}

ESD_CORE_EXPORT ESD_Rect16 ESD_Scissor_Set(ESD_Rect16 rect)
{
	ESD_Rect16 state = ESD_ScissorRect;
	ESD_Scissor_Adjust(rect, state);
	return state;
}

ESD_CORE_EXPORT void ESD_Scissor_Adjust(ESD_Rect16 rect, ESD_Rect16 state)
{
	EVE_HalContext *phost = ESD_GetHost();
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
	if (ESD_ScissorRect.X != rect.X || ESD_ScissorRect.Y != rect.Y)
		EVE_CoDl_scissorXY(phost, rect.X, rect.Y);
	if (ESD_ScissorRect.Width != rect.Width || ESD_ScissorRect.Height != rect.Height)
		EVE_CoDl_scissorSize(phost, rect.Width, rect.Height);
	ESD_ScissorRect = rect;
}

ESD_CORE_EXPORT void ESD_Scissor_Reset(ESD_Rect16 state)
{
	EVE_HalContext *phost = ESD_GetHost();
	if (ESD_ScissorRect.X != state.X || ESD_ScissorRect.Y != state.Y)
		EVE_CoDl_scissorXY(phost, state.X, state.Y);
	if (ESD_ScissorRect.Width != state.Width || ESD_ScissorRect.Height != state.Height)
		EVE_CoDl_scissorSize(phost, state.Width, state.Height);
	ESD_ScissorRect = state;
}

/* end of file */
