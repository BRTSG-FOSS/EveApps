
#include "ESD_Scissor.h"
#include <EVE_Hal.h>

#include "ESD_Context.h"

#include <stdio.h>
#define _USE_MATH_DEFINES 1
#include <math.h>

// Globals
extern ESD_CORE_EXPORT EVE_HalContext *ESD_Host;

// GPU state for the current display list
ESD_CORE_EXPORT ESD_Rect16 ESD_ScissorRect;

void ESD_Scissor_dlStart() // Begin of frame
{
	EVE_HalContext *phost = ESD_Host;

	// Reset scissor state to display size
	ESD_ScissorRect.X = 0;
	ESD_ScissorRect.Y = 0;
	ESD_ScissorRect.Width = phost->Width;
	ESD_ScissorRect.Height = phost->Height;
}

ESD_Rect16 ESD_Scissor_get()
{
	return ESD_ScissorRect;
}

ESD_Rect16 ESD_Scissor_set(ESD_Rect16 rect)
{
	ESD_Rect16 state = ESD_ScissorRect;
	ESD_Scissor_adjust(rect, state);
	return state;
}

void ESD_Scissor_adjust(ESD_Rect16 rect, ESD_Rect16 state)
{
	EVE_HalContext *phost = ESD_Host;
	(void)phost;

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
	// EVE_CoCmd_startFunc(ESD_Host, FT_CMD_SIZE * 2);
	if (ESD_ScissorRect.X != rect.X || ESD_ScissorRect.Y != rect.Y)
		EVE_CoCmd_dl(ESD_Host, SCISSOR_XY(rect.X, rect.Y));
	if (ESD_ScissorRect.Width != rect.Width || ESD_ScissorRect.Height != rect.Height)
		EVE_CoCmd_dl(ESD_Host, SCISSOR_SIZE(rect.Width, rect.Height));
	// EVE_CoCmd_endFunc(ESD_Host);
	ESD_ScissorRect = rect;
}

void ESD_Scissor_reset(ESD_Rect16 state)
{
	// EVE_CoCmd_startFunc(ESD_Host, FT_CMD_SIZE * 2);
	EVE_HalContext *phost = ESD_Host;
	(void)phost;
	if (ESD_ScissorRect.X != state.X || ESD_ScissorRect.Y != state.Y)
		EVE_CoCmd_dl(ESD_Host, SCISSOR_XY(state.X, state.Y));
	if (ESD_ScissorRect.Width != state.Width || ESD_ScissorRect.Height != state.Height)
		EVE_CoCmd_dl(ESD_Host, SCISSOR_SIZE(state.Width, state.Height));
	// EVE_CoCmd_endFunc(ESD_Host);
	ESD_ScissorRect = state;
}

/* end of file */
