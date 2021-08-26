
#include "Esd_Render.h"

#include "Esd_Context.h"
#include "Esd_Scissor.h"
#include "Esd_BitmapHandle.h"

void Esd_CoDl_Bitmap_Vertex(int16_t x, int16_t y, uint8_t handle, uint16_t cell)
{
	EVE_HalContext *phost = Esd_GetHost();
	if ((EVE_CHIPID >= EVE_FT810) && (x < 0 || y < 0 || x >= 512 || y >= 512))
	{
		EVE_CoDl_vertexFormat(phost, 0);
		EVE_CoDl_bitmapHandle(phost, handle);
		Esd_CoDl_PagedBitmapSource(handle, cell >> 7);
		EVE_CoDl_cell(phost, cell & 0x7F);
		EVE_CoDl_vertex2f(phost, x, y);
	}
	else
	{
		Esd_CoDl_PagedBitmapSource(handle, cell >> 7);
		EVE_CoDl_vertex2ii(phost, x, y, handle, cell & 0x7F);
	}
}

// NOTE: This function may only be used within a EVE_CoDl_saveContext block, because it does not clean up state, and bypasses some EVE_CoDl optimizations
// Also EVE_CoCmd_loadIdentity must be called afterwards to fully restore the context
void Esd_CoDl_Bitmap_Vertex_DXT1(int16_t x, int16_t y, uint8_t handle, uint8_t additional, uint16_t cell, uint16_t cells)
{
	/* ---- */
	/* NOTE: Partially bypassing CoDl optimizer on purpose inside a saveContext block */
	/* ---- */

	EVE_HalContext *phost = Esd_GetHost();
	EVE_CoCmd_dl(phost, BLEND_FUNC(ONE, ZERO));
	EVE_CoCmd_dl(phost, COLOR_A(0x55));
	Esd_CoDl_Bitmap_Vertex(x, y, handle, cell);
	EVE_CoCmd_dl(phost, BLEND_FUNC(ONE, ONE));
	EVE_CoCmd_dl(phost, COLOR_A(0xAA));
	Esd_CoDl_Bitmap_Vertex(x, y, handle, cell + cells);
	EVE_CoCmd_dl(phost, COLOR_MASK(1, 1, 1, 0));
	EVE_CoCmd_scale(phost, 4UL * 65536UL, 4UL * 65536UL); // Color pass, scaled up 4x, nearest
	EVE_CoCmd_setMatrix(phost);
	EVE_CoCmd_dl(phost, BLEND_FUNC(DST_ALPHA, ZERO));
	Esd_CoDl_Bitmap_Vertex(x, y, additional, cell + cells);
	EVE_CoCmd_dl(phost, BLEND_FUNC(ONE_MINUS_DST_ALPHA, ONE));
	Esd_CoDl_Bitmap_Vertex(x, y, additional, cell);
}

#if (EVE_SUPPORT_CHIPID >= EVE_FT810)
// NOTE: This function may only be used within a Esd_CoDl_SAVE_CONTEXT block, because it does not clean up state
void Esd_CoDl_Bitmap_Vertex_PALETTED8(int16_t x, int16_t y, uint8_t handle, uint16_t cell, uint32_t paletteAddr)
{
	/* ---- */
	/* NOTE: Partially bypassing CoDl optimizer on purpose inside a saveContext block */
	/* ---- */

	EVE_HalContext *phost = Esd_GetHost();
	if (EVE_CHIPID >= EVE_FT810)
	{
		EVE_CoDl_alphaFunc(phost, ALWAYS, 0);
		EVE_CoCmd_dl(phost, BLEND_FUNC(ONE, ZERO));
		EVE_CoCmd_dl(phost, COLOR_MASK(0, 0, 0, 1));
		EVE_CoDl_paletteSource(phost, paletteAddr + 3);
		Esd_CoDl_Bitmap_Vertex(x, y, handle, cell);
		EVE_CoCmd_dl(phost, BLEND_FUNC(DST_ALPHA, ONE_MINUS_DST_ALPHA));
		EVE_CoCmd_dl(phost, COLOR_MASK(1, 0, 0, 0));
		EVE_CoDl_paletteSource(phost, paletteAddr + 2);
		Esd_CoDl_Bitmap_Vertex(x, y, handle, cell);
		EVE_CoCmd_dl(phost, COLOR_MASK(0, 1, 0, 0));
		EVE_CoDl_paletteSource(phost, paletteAddr + 1);
		Esd_CoDl_Bitmap_Vertex(x, y, handle, cell);
		EVE_CoCmd_dl(phost, COLOR_MASK(0, 0, 1, 0));
		EVE_CoDl_paletteSource(phost, paletteAddr);
		Esd_CoDl_Bitmap_Vertex(x, y, handle, cell);
	}
}
#else
#define ESD_CoDl_Bitmap_Vertex_PALETTED8(x, y, handle, cell, paletteAddr) eve_assert(false)
#endif

ESD_CORE_EXPORT void Esd_Render_Bitmap(int16_t x, int16_t y, Esd_BitmapCell bitmapCell, esd_argb32_t c)
{
	Esd_BitmapInfo *bitmapInfo;
	uint16_t cell;
	EVE_HalContext *phost;
	uint8_t handle;
	(void)phost;

	if (!bitmapCell.Info)
		return;

	bitmapInfo = bitmapCell.Info;
	cell = bitmapCell.Cell;
	phost = Esd_Host;
	handle = Esd_CoDl_SetupBitmap(bitmapInfo);

	if (ESD_BITMAPHANDLE_VALID(handle))
	{
		uint8_t additional = bitmapInfo->AdditionalInfo
		    ? Esd_CoDl_SetupBitmap(bitmapInfo->AdditionalInfo)
		    : ESD_BITMAPHANDLE_INVALID;

		Esd_CoDl_BitmapSizeReset(handle);
		EVE_CoDl_colorArgb_ex(phost, c);
		EVE_CoDl_begin(phost, BITMAPS);
#if (EVE_SUPPORT_CHIPID >= EVE_FT810)
		if (EVE_CHIPID >= EVE_FT810 && bitmapInfo->Format == PALETTED8)
		{
			uint32_t paletteAddr;
			EVE_CoDl_saveContext(phost);
			paletteAddr = Esd_LoadPalette(bitmapInfo);
			Esd_CoDl_Bitmap_Vertex_PALETTED8(x, y, handle, cell, paletteAddr);
			if (ESD_BITMAPHANDLE_VALID(additional))
				Esd_CoDl_Bitmap_Vertex_PALETTED8(x, y, additional, cell, paletteAddr);
			EVE_CoDl_restoreContext(phost);
		}
		else
#endif
		    if (bitmapInfo->Format == DXT1 && ESD_BITMAPHANDLE_VALID(additional))
		{
			Esd_CoDl_BitmapWidthHeight(additional, bitmapInfo->Width, bitmapInfo->Height);
			EVE_CoDl_saveContext(phost);
			Esd_CoDl_Bitmap_Vertex_DXT1(x, y, handle, additional, cell, bitmapInfo->Cells);
			EVE_CoCmd_loadIdentity(Esd_Host);
			EVE_CoDl_restoreContext(phost);
		}
		else
		{
			Esd_CoDl_Bitmap_Vertex(x, y, handle, cell);
			if (ESD_BITMAPHANDLE_VALID(additional))
				Esd_CoDl_Bitmap_Vertex(x, y, additional, cell);
		}
		EVE_CoDl_end(phost);
	}
}

ESD_CORE_EXPORT void Esd_Render_Bitmap_Scaled(int16_t x, int16_t y, Esd_BitmapCell bitmapCell, esd_argb32_t c, esd_int32_f16_t xscale, esd_int32_f16_t yscale, esd_int32_f16_t xoffset, esd_int32_f16_t yoffset, int16_t width, int16_t height)
{
	EVE_HalContext *phost;
	Esd_BitmapInfo *bitmapInfo;
	uint16_t cell;
	uint8_t handle;

	if (!bitmapCell.Info)
		return;

	phost = Esd_Host;
	bitmapInfo = bitmapCell.Info;
	cell = bitmapCell.Cell;
	handle = Esd_CoDl_SetupBitmap(bitmapInfo);

	if (ESD_BITMAPHANDLE_VALID(handle))
	{
		uint8_t additional = bitmapInfo->AdditionalInfo
		    ? Esd_CoDl_SetupBitmap(bitmapInfo->AdditionalInfo)
		    : ESD_BITMAPHANDLE_INVALID;

		if (bitmapInfo->Format == DXT1)
			Esd_CoDl_BitmapWidthHeight(handle, width, height);
		else
			Esd_CoDl_BitmapSize(handle, BILINEAR, BORDER, BORDER, width, height);
		EVE_CoDl_colorArgb_ex(phost, c);
		EVE_CoDl_saveContext(phost);
		EVE_CoCmd_translate(Esd_Host, xoffset, yoffset);
		EVE_CoCmd_scale(Esd_Host, xscale, yscale);
		EVE_CoCmd_setMatrix(Esd_Host);
		EVE_CoDl_begin(phost, BITMAPS);
#if (EVE_SUPPORT_CHIPID >= EVE_FT810)
		if ((EVE_CHIPID >= EVE_FT810) && (bitmapInfo->Format == PALETTED8))
		{
			uint32_t paletteAddr = Esd_LoadPalette(bitmapInfo);
			Esd_CoDl_Bitmap_Vertex_PALETTED8(x, y, handle, cell, paletteAddr);
			if (ESD_BITMAPHANDLE_VALID(additional))
				Esd_CoDl_Bitmap_Vertex_PALETTED8(x, y, additional, cell, paletteAddr);
		}
		else
#endif
		    if (bitmapInfo->Format == DXT1 && ESD_BITMAPHANDLE_VALID(additional))
		{
			Esd_CoDl_BitmapWidthHeight(additional, width, height);
			Esd_CoDl_Bitmap_Vertex_DXT1(x, y, handle, additional, cell, bitmapInfo->Cells);
		}
		else
		{
			Esd_CoDl_Bitmap_Vertex(x, y, handle, cell);
			if (ESD_BITMAPHANDLE_VALID(additional))
				Esd_CoDl_Bitmap_Vertex(x, y, additional, cell);
		}
		EVE_CoDl_end(phost);
		EVE_CoCmd_loadIdentity(Esd_Host);
		EVE_CoDl_restoreContext(phost);
	}
}

// Render bitmap using freeform rectangle within a specified global screen rectangle, freeform is relative to global
ESD_CORE_EXPORT void Esd_Render_Bitmap_Freeform(Esd_BitmapCell bitmapCell, esd_argb32_t c, Esd_Rect16 globalRect, Esd_Rect16 freeformRect, uint8_t minAlpha)
{
	EVE_HalContext *phost;

	if (!bitmapCell.Info)
		return;

	phost = Esd_Host;

	EVE_CoDl_alphaFunc(phost, (minAlpha > 0) ? GEQUAL : ALWAYS, minAlpha);

	if (freeformRect.Width == bitmapCell.Info->Width // No scaling
	    && freeformRect.Height == bitmapCell.Info->Height)
	{
		freeformRect.X += globalRect.X;
		freeformRect.Y += globalRect.Y;
		if (Esd_Rect16_IsInside(freeformRect, globalRect)) // No scissor
		{
			Esd_Render_Bitmap(freeformRect.X, freeformRect.Y, bitmapCell, c);
		}
		else
		{
			Esd_Rect16 state = Esd_Scissor_Set(globalRect);
			Esd_Render_Bitmap(freeformRect.X, freeformRect.Y, bitmapCell, c);
			Esd_Scissor_Reset(state);
		}
	}
	else
	{
		esd_int32_f16_t xscale = (((esd_int32_f16_t)freeformRect.Width) << 16) / ((esd_int32_f16_t)bitmapCell.Info->Width);
		esd_int32_f16_t yscale = (((esd_int32_f16_t)freeformRect.Height) << 16) / ((esd_int32_f16_t)bitmapCell.Info->Height);
		// eve_printf_debug("scale: %i, %i\n", xscale, yscale);
		Esd_Render_Bitmap_Scaled(globalRect.X, globalRect.Y, bitmapCell, c, xscale, yscale,
		    (((esd_int32_f16_t)freeformRect.X) << 16), (((esd_int32_f16_t)freeformRect.Y) << 16),
		    globalRect.Width, globalRect.Height);
	}

	EVE_CoDl_alphaFunc(phost, ALWAYS, 0);
}

ESD_CORE_EXPORT void Esd_Render_Bitmap_RotateScaled(Esd_BitmapCell bitmapCell, esd_argb32_t c, Esd_Rect16 globalRect, int32_t rotateAngle, esd_int32_f16_t xscale, esd_int32_f16_t yscale)
{
	EVE_HalContext *phost;
	Esd_BitmapInfo *bitmapInfo;
	uint16_t cell;
	uint8_t handle;
	(void)phost;

	if (!bitmapCell.Info)
		return;

	phost = Esd_Host;
	bitmapInfo = bitmapCell.Info;
	cell = bitmapCell.Cell;
	handle = Esd_CoDl_SetupBitmap(bitmapInfo);

	if (ESD_BITMAPHANDLE_VALID(handle))
	{
		int16_t x_center = bitmapInfo->Width / 2;
		int16_t y_center = bitmapInfo->Height / 2;
		int16_t x = globalRect.X; // + x_center;
		int16_t y = globalRect.Y; // + y_center;

		EVE_CoDl_colorArgb_ex(phost, c);

		EVE_CoDl_vertexFormat(phost, 4);

		Esd_CoDl_BitmapSize(handle, BILINEAR, BORDER, BORDER, bitmapInfo->Width, bitmapInfo->Height);
		Esd_CoDl_PagedCell(handle, cell);

		EVE_CoDl_saveContext(phost);
		EVE_CoCmd_loadIdentity(Esd_Host);
		EVE_CoCmd_translate(Esd_Host, x_center << 16, y_center << 16);
		EVE_CoCmd_scale(Esd_Host, xscale, yscale);
		EVE_CoCmd_rotate(Esd_Host, rotateAngle);
		// eve_printf_debug("xscale: %i, yscale: %i\n", (int)xscale, (int)yscale);
		EVE_CoCmd_translate(Esd_Host, -x_center << 16, -y_center << 16);
		EVE_CoCmd_setMatrix(Esd_Host);
		EVE_CoCmd_loadIdentity(Esd_Host);
		EVE_CoDl_begin(phost, BITMAPS);
		EVE_CoDl_vertex2f_4(Esd_Host, x * 16, y * 16);
		EVE_CoDl_end(phost);
		EVE_CoDl_restoreContext(phost);
	}
}

ESD_CORE_EXPORT void Esd_Render_Bitmap_Rotate(Esd_BitmapCell bitmapCell, esd_argb32_t c, Esd_Rect16 globalRect, int32_t rotateAngle)
{
	EVE_HalContext *phost = Esd_GetHost();
	Esd_BitmapInfo *bitmapInfo;
	uint16_t cell;
	uint8_t handle;
	(void)phost;

	if (!bitmapCell.Info)
		return;

	bitmapInfo = bitmapCell.Info;
	cell = bitmapCell.Cell;
	handle = Esd_CoDl_SetupBitmap(bitmapInfo);

	if (ESD_BITMAPHANDLE_VALID(handle))
	{
		int16_t x_center = bitmapInfo->Width / 2;
		int16_t y_center = bitmapInfo->Height / 2;
		int16_t x = globalRect.X + x_center;
		int16_t y = globalRect.Y + y_center;

		int16_t radius = bitmapInfo->Width / 2;

		//eve_printf_debug("x=%d,y=%d,x_center_x=%d,certer_y = %d, radius = %d\n",x,y,x_center,y_center,radius);

		int tilenumber = 0;
		const int TITLE_SIZE = 64; //Magic number, DONOT CHANGE
		EVE_CoDl_colorArgb_ex(phost, c);
		EVE_CoDl_vertexFormat(phost, 4);
		EVE_CoDl_saveContext(phost);

		Esd_CoDl_BitmapSize(handle, BILINEAR, BORDER, BORDER, TITLE_SIZE, TITLE_SIZE); //Bitmap_Size command

		EVE_CoDl_begin(phost, BITMAPS);

		Esd_CoDl_PagedCell(handle, cell);
		scope
		{
			int dx, dy;
			for (dx = (x - radius); dx < (x + radius); dx += TITLE_SIZE)
			{
				for (dy = (y - radius); dy < (y + radius); dy += TITLE_SIZE)
				{
					//eve_printf_debug("draw tile %d\n",tilenumber ++);
					tilenumber++;
					EVE_CoCmd_loadIdentity(Esd_Host);
					EVE_CoCmd_translate(Esd_Host, (x - dx) << 16, (y - dy) << 16);

					EVE_CoCmd_rotate(Esd_Host, rotateAngle);
					EVE_CoCmd_translate(Esd_Host, -x_center << 16, -y_center << 16);
					//EVE_CoCmd_translate(Esd_Host, -x_center, -y_center);

					EVE_CoCmd_setMatrix(Esd_Host);
					EVE_CoDl_vertex2f_4(Esd_Host, dx * 16, dy * 16);
				}
			}
		}
		//eve_printf_debug("draw tile %d\n",tilenumber);
		EVE_CoCmd_loadIdentity(Esd_Host);
		EVE_CoDl_restoreContext(phost);
	}
}

/* end of file */
