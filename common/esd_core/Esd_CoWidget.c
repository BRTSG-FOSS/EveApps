
#include "Esd_CoWidget.h"
#include "Esd_Context.h"

void Esd_CoWidget_Spinner(void *owner, int16_t x, int16_t y, uint16_t style, uint16_t scale)
{
	Esd_CurrentContext->CmdOwner = owner;
	EVE_CoDl_vertexFormat(Esd_GetHost(), 4);
	EVE_CoCmd_spinner(Esd_GetHost(), x, y, style, scale);
}

void Esd_CoWidget_ScreenSaver(void *owner)
{
	Esd_CurrentContext->CmdOwner = owner;
	EVE_CoCmd_screenSaver(Esd_GetHost());
}

void Esd_CoWidget_Sketch(void *owner, Esd_Rect16 globalRect, uint32_t addr, uint16_t format)
{
	Esd_CurrentContext->CmdOwner = owner;
	EVE_CoCmd_sketch(Esd_GetHost(), globalRect.X, globalRect.Y, globalRect.Width, globalRect.Height, addr, format);
}

void Esd_CoWidget_Stop(void *owner)
{
	if ((owner == Esd_CurrentContext->CmdOwner) || !owner)
	{
		EVE_CoCmd_stop(Esd_GetHost());
		Esd_CurrentContext->CmdOwner = NULL;
	}
}

void Esd_CoWidget_PopupSpinner()
{
	Esd_CurrentContext->SpinnerPopup = true;
}

/* end of file */
