
#include "ESD_CoWidget.h"
#include "ESD_Context.h"

void ESD_CoWidget_Spinner(void *owner, int16_t x, int16_t y, uint16_t style, uint16_t scale)
{
	ESD_CurrentContext->CmdOwner = owner;
	EVE_CoDl_vertexFormat(ESD_GetHost(), 4);
	EVE_CoCmd_spinner(ESD_GetHost(), x, y, style, scale);
}

void ESD_CoWidget_ScreenSaver(void *owner)
{
	ESD_CurrentContext->CmdOwner = owner;
	EVE_CoCmd_screenSaver(ESD_GetHost());
}

void ESD_CoWidget_Sketch(void *owner, ESD_Rect16 globalRect, uint32_t addr, uint16_t format)
{
	ESD_CurrentContext->CmdOwner = owner;
	EVE_CoCmd_sketch(ESD_GetHost(), globalRect.X, globalRect.Y, globalRect.Width, globalRect.Height, addr, format);
}

void ESD_CoWidget_Stop(void *owner)
{
	if ((owner == ESD_CurrentContext->CmdOwner) || !owner)
	{
		EVE_CoCmd_stop(ESD_GetHost());
		ESD_CurrentContext->CmdOwner = NULL;
	}
}

/* end of file */
