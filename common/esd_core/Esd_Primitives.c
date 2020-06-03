
#include "ESD_Primitives.h"
#include <EVE_Hal.h>

#include "ESD_Dl.h"
#include "ESD_BitmapHandle.h"

extern EVE_HalContext *ESD_Host;

// Rectangle Gradient drawing with some logic to convert from radius to line width and width height to positions to simplify usage
void ESD_Render_RectangleF_Gradient(
    int32_f4_t x, int32_f4_t y,
    int32_f4_t w, int32_f4_t h,
    ft_argb32_t color1, ft_argb32_t color2,
    int16_t direction)
{
	int16_t x0 = x >> 4;
	int16_t y0 = y >> 4;
	int16_t w1 = w >> 4;
	int16_t h1 = h >> 4;

	FT_Esd_Render_Rect_Grad(x0, y0, w1, h1, color1, color2, direction);
}

// Rectangle drawing with some logic to convert from radius to line width and width height to positions to simplify usage
void ESD_Render_RectangleF(int32_f4_t x, int32_f4_t y, int32_f4_t w, int32_f4_t h, int32_f4_t radius, ft_argb32_t color)
{
	EVE_HalContext *phost = ESD_Host;
	int32_t width = radius + 8;
	int32_t x0 = x + radius;
	int32_t y0 = y + radius;
	int32_t x1 = x + w - 16 - radius;
	int32_t y1 = y + h - 16 - radius;
	ESD_Dl_COLOR_ARGB(color);
	ESD_Dl_LINE_WIDTH(width);
	ESD_Dl_BEGIN(RECTS);
	if (EVE_CHIPID >= EVE_FT810)
		ESD_Dl_VERTEX_FORMAT(4);
	Ft_Gpu_CoCmd_SendCmd(phost, VERTEX2F(x0, y0));
	Ft_Gpu_CoCmd_SendCmd(phost, VERTEX2F(x1, y1));
	ESD_Dl_END();
}

void ESD_Render_LineF(int32_f4_t x0, int32_f4_t y0, int32_f4_t x1, int32_f4_t y1, int32_f3_t width, ft_argb32_t color)
{
	EVE_HalContext *phost = ESD_Host;
	ESD_Dl_COLOR_ARGB(color);
	ESD_Dl_LINE_WIDTH(width);
	ESD_Dl_BEGIN(LINES);
	if (EVE_CHIPID >= EVE_FT810)
		ESD_Dl_VERTEX_FORMAT(4);
	Ft_Gpu_CoCmd_SendCmd(phost, VERTEX2F(x0, y0));
	Ft_Gpu_CoCmd_SendCmd(phost, VERTEX2F(x1, y1));
	ESD_Dl_END();
}

void ESD_Dl_Bitmap_Vertex(int16_t x, int16_t y, uint8_t handle, uint16_t cell)
{
	EVE_HalContext *phost = ESD_Host;
	if ((EVE_CHIPID >= EVE_FT810) && (x < 0 || y < 0 || x >= 512 || y >= 512))
	{
		ESD_Dl_VERTEX_FORMAT(0);
		ESD_Dl_BITMAP_HANDLE(handle);
		ESD_Dl_Bitmap_Page(handle, cell >> 7);
		ESD_Dl_CELL(cell & 0x7F);
		Ft_Gpu_CoCmd_SendCmd(phost, VERTEX2F(x, y));
	}
	else
	{
		ESD_Dl_Bitmap_Page(handle, cell >> 7);
		Ft_Gpu_CoCmd_SendCmd(phost, VERTEX2II(x, y, handle, cell));
	}
}

// NOTE: This function may only be used within a ESD_Dl_SAVE_CONTEXT block, because it does not clean up state
// Also Ft_Gpu_CoCmd_LoadIdentity must be called afterwards to fully restore the context
void ESD_Dl_Bitmap_Vertex_DXT1(int16_t x, int16_t y, uint8_t handle, uint8_t additional, uint16_t cell, uint16_t cells)
{
	EVE_HalContext *phost = ESD_Host;
	Ft_Gpu_CoCmd_SendCmd(phost, BLEND_FUNC(ONE, ZERO));
	Ft_Gpu_CoCmd_SendCmd(phost, COLOR_A(0x55));
	ESD_Dl_Bitmap_Vertex(x, y, handle, cell);
	Ft_Gpu_CoCmd_SendCmd(phost, BLEND_FUNC(ONE, ONE));
	Ft_Gpu_CoCmd_SendCmd(phost, COLOR_A(0xAA));
	ESD_Dl_Bitmap_Vertex(x, y, handle, cell + cells);
	Ft_Gpu_CoCmd_SendCmd(phost, COLOR_MASK(1, 1, 1, 0));
	Ft_Gpu_CoCmd_Scale(phost, 4UL * 65536UL, 4UL * 65536UL); // Color pass, scaled up 4x, nearest
	Ft_Gpu_CoCmd_SetMatrix(phost);
	Ft_Gpu_CoCmd_SendCmd(phost, BLEND_FUNC(DST_ALPHA, ZERO));
	ESD_Dl_Bitmap_Vertex(x, y, additional, cell + cells);
	Ft_Gpu_CoCmd_SendCmd(phost, BLEND_FUNC(ONE_MINUS_DST_ALPHA, ONE));
	ESD_Dl_Bitmap_Vertex(x, y, additional, cell);
}

#if (EVE_SUPPORT_CHIPID >= EVE_FT810)
// NOTE: This function may only be used within a ESD_Dl_SAVE_CONTEXT block, because it does not clean up state
void ESD_Dl_Bitmap_Vertex_PALETTED8(int16_t x, int16_t y, uint8_t handle, uint16_t cell, uint32_t paletteAddr)
{
	EVE_HalContext *phost = ESD_Host;
	ESD_Dl_Alpha_Func(ALWAYS, 0);
	Ft_Gpu_CoCmd_SendCmd(phost, BLEND_FUNC(ONE, ZERO));
	Ft_Gpu_CoCmd_SendCmd(phost, COLOR_MASK(0, 0, 0, 1));
	ESD_Dl_PALETTE_SOURCE(paletteAddr + 3);
	ESD_Dl_Bitmap_Vertex(x, y, handle, cell);
	Ft_Gpu_CoCmd_SendCmd(phost, BLEND_FUNC(DST_ALPHA, ONE_MINUS_DST_ALPHA));
	Ft_Gpu_CoCmd_SendCmd(phost, COLOR_MASK(1, 0, 0, 0));
	ESD_Dl_PALETTE_SOURCE(paletteAddr + 2);
	ESD_Dl_Bitmap_Vertex(x, y, handle, cell);
	Ft_Gpu_CoCmd_SendCmd(phost, COLOR_MASK(0, 1, 0, 0));
	ESD_Dl_PALETTE_SOURCE(paletteAddr + 1);
	ESD_Dl_Bitmap_Vertex(x, y, handle, cell);
	Ft_Gpu_CoCmd_SendCmd(phost, COLOR_MASK(0, 0, 1, 0));
	ESD_Dl_PALETTE_SOURCE(paletteAddr);
	ESD_Dl_Bitmap_Vertex(x, y, handle, cell);
}
#else
#define ESD_Dl_Bitmap_Vertex_PALETTED8(x, y, handle, cell, paletteAddr) eve_assert(false)
#endif

void ESD_Render_Bitmap(int16_t x, int16_t y, ESD_BitmapCell bitmapCell, ft_argb32_t c)
{
	ESD_BitmapInfo *bitmapInfo;
	uint16_t cell;
	EVE_HalContext *phost;
	uint8_t handle;
	(void)phost;

	if (!bitmapCell.Info)
		return;

	bitmapInfo = bitmapCell.Info;
	cell = bitmapCell.Cell;
	phost = ESD_Host;
	handle = ESD_Dl_Bitmap_Setup(bitmapInfo);

	if (ESD_BITMAPHANDLE_VALID(handle))
	{
		uint8_t additional = bitmapInfo->AdditionalInfo
		    ? ESD_Dl_Bitmap_Setup(bitmapInfo->AdditionalInfo)
		    : ESD_BITMAPHANDLE_INVALID;

		ESD_Dl_Bitmap_WidthHeightReset(handle);
		ESD_Dl_COLOR_ARGB(c);
		ESD_Dl_BEGIN(BITMAPS);
		if (EVE_CHIPID >= EVE_FT810 && bitmapInfo->Format == PALETTED8)
		{
			uint32_t paletteAddr;
			ESD_Dl_SAVE_CONTEXT();
			paletteAddr = ESD_LoadPalette(bitmapInfo);
			ESD_Dl_Bitmap_Vertex_PALETTED8(x, y, handle, cell, paletteAddr);
			if (ESD_BITMAPHANDLE_VALID(additional))
				ESD_Dl_Bitmap_Vertex_PALETTED8(x, y, additional, cell, paletteAddr);
			ESD_Dl_RESTORE_CONTEXT();
		}
		else if (bitmapInfo->Format == DXT1 && ESD_BITMAPHANDLE_VALID(additional))
		{
			ESD_Dl_Bitmap_WidthHeight(additional, bitmapInfo->Width, bitmapInfo->Height);
			ESD_Dl_SAVE_CONTEXT();
			ESD_Dl_Bitmap_Vertex_DXT1(x, y, handle, additional, cell, bitmapInfo->Cells);
			Ft_Gpu_CoCmd_LoadIdentity(ESD_Host);
			ESD_Dl_RESTORE_CONTEXT();
		}
		else
		{
			ESD_Dl_Bitmap_Vertex(x, y, handle, cell);
			if (ESD_BITMAPHANDLE_VALID(additional))
				ESD_Dl_Bitmap_Vertex(x, y, additional, cell);
		}
		ESD_Dl_END();
	}
}

void ESD_Render_BitmapScaled(int16_t x, int16_t y, ESD_BitmapCell bitmapCell, ft_argb32_t c, int32_f16_t xscale, int32_f16_t yscale, int32_f16_t xoffset, int32_f16_t yoffset, int16_t width, int16_t height)
{
	ESD_BitmapInfo *bitmapInfo;
	uint16_t cell;
	EVE_HalContext *phost;
	uint8_t handle;
	(void)phost;

	if (!bitmapCell.Info)
		return;

	bitmapInfo = bitmapCell.Info;
	cell = bitmapCell.Cell;
	phost = ESD_Host;
	handle = ESD_Dl_Bitmap_Setup(bitmapInfo);

	if (ESD_BITMAPHANDLE_VALID(handle))
	{
		uint8_t additional = bitmapInfo->AdditionalInfo
		    ? ESD_Dl_Bitmap_Setup(bitmapInfo->AdditionalInfo)
		    : ESD_BITMAPHANDLE_INVALID;

		if (bitmapInfo->Format == DXT1)
			ESD_Dl_Bitmap_WidthHeight(handle, width, height);
		else
			ESD_Dl_Bitmap_WidthHeight_BILINEAR(handle, width, height);
		ESD_Dl_COLOR_ARGB(c);
		ESD_Dl_SAVE_CONTEXT();
		Ft_Gpu_CoCmd_Translate(ESD_Host, xoffset, yoffset);
		Ft_Gpu_CoCmd_Scale(ESD_Host, xscale, yscale);
		Ft_Gpu_CoCmd_SetMatrix(ESD_Host);
		ESD_Dl_BEGIN(BITMAPS);
		if ((EVE_CHIPID >= EVE_FT810) && (bitmapInfo->Format == PALETTED8))
		{
			uint32_t paletteAddr = ESD_LoadPalette(bitmapInfo);
			ESD_Dl_Bitmap_Vertex_PALETTED8(x, y, handle, cell, paletteAddr);
			if (ESD_BITMAPHANDLE_VALID(additional))
				ESD_Dl_Bitmap_Vertex_PALETTED8(x, y, additional, cell, paletteAddr);
		}
		else if (bitmapInfo->Format == DXT1 && ESD_BITMAPHANDLE_VALID(additional))
		{
			ESD_Dl_Bitmap_WidthHeight(additional, width, height);
			ESD_Dl_Bitmap_Vertex_DXT1(x, y, handle, additional, cell, bitmapInfo->Cells);
		}
		else
		{
			ESD_Dl_Bitmap_Vertex(x, y, handle, cell);
			if (ESD_BITMAPHANDLE_VALID(additional))
				ESD_Dl_Bitmap_Vertex(x, y, additional, cell);
		}
		ESD_Dl_END();
		Ft_Gpu_CoCmd_LoadIdentity(ESD_Host);
		ESD_Dl_RESTORE_CONTEXT();
	}
}

// Render bitmap using freeform rectangle within a specified global screen rectangle, freeform is relative to global
void ESD_Render_BitmapFreeform(ESD_BitmapCell bitmapCell, ft_argb32_t c, ESD_Rect16 globalRect, ESD_Rect16 freeformRect, uint8_t minAlpha)
{
	if (!bitmapCell.Info)
		return;

	ESD_Dl_Alpha_Func((minAlpha > 0) ? GEQUAL : ALWAYS, minAlpha);

	if (freeformRect.Width == bitmapCell.Info->Width // No scaling
	    && freeformRect.Height == bitmapCell.Info->Height)
	{
		freeformRect.X += globalRect.X;
		freeformRect.Y += globalRect.Y;
		if (ESD_Rect16_IsInside(freeformRect, globalRect)) // No scissor
		{
			ESD_Render_Bitmap(freeformRect.X, freeformRect.Y, bitmapCell, c);
		}
		else
		{
			ESD_Rect16 state = ESD_Dl_Scissor_Set(globalRect);
			ESD_Render_Bitmap(freeformRect.X, freeformRect.Y, bitmapCell, c);
			ESD_Dl_Scissor_Reset(state);
		}
	}
	else
	{
		int32_f16_t xscale = (((int32_f16_t)freeformRect.Width) << 16) / ((int32_f16_t)bitmapCell.Info->Width);
		int32_f16_t yscale = (((int32_f16_t)freeformRect.Height) << 16) / ((int32_f16_t)bitmapCell.Info->Height);
		// eve_printf_debug("scale: %i, %i\n", xscale, yscale);
		ESD_Render_BitmapScaled(globalRect.X, globalRect.Y, bitmapCell, c, xscale, yscale,
		    (((int32_f16_t)freeformRect.X) << 16), (((int32_f16_t)freeformRect.Y) << 16),
		    globalRect.Width, globalRect.Height);
	}

	ESD_Dl_Alpha_Func(ALWAYS, 0);
}

void ESD_Render_BitmapRotate_Scaled(ESD_BitmapCell bitmapCell, ft_argb32_t c, ESD_Rect16 globalRect, int32_t rotateAngle, int32_f16_t xscale, int32_f16_t yscale)
{
	EVE_HalContext *phost = ESD_Host;
	ESD_BitmapInfo *bitmapInfo;
	uint16_t cell;
	uint8_t handle;
	(void)phost;

	if (!bitmapCell.Info)
		return;

	bitmapInfo = bitmapCell.Info;
	cell = bitmapCell.Cell;
	handle = ESD_Dl_Bitmap_Setup(bitmapInfo);

	if (ESD_BITMAPHANDLE_VALID(handle))
	{
		int16_t x_center = bitmapInfo->Width / 2;
		int16_t y_center = bitmapInfo->Height / 2;
		int16_t x = globalRect.X; // + x_center;
		int16_t y = globalRect.Y; // + y_center;

		ESD_Dl_COLOR_ARGB(c);

		if (EVE_CHIPID >= EVE_FT810)
			ESD_Dl_VERTEX_FORMAT(4);

		ESD_Dl_Bitmap_WidthHeight_BILINEAR(handle, bitmapInfo->Width, bitmapInfo->Height);
		ESD_Dl_CELL_Paged(handle, cell);

		ESD_Dl_SAVE_CONTEXT();
		Ft_Gpu_CoCmd_LoadIdentity(ESD_Host);
		Ft_Gpu_CoCmd_Translate(ESD_Host, x_center << 16, y_center << 16);
		Ft_Gpu_CoCmd_Scale(ESD_Host, xscale, yscale);
		Ft_Gpu_CoCmd_Rotate(ESD_Host, rotateAngle);
		// eve_printf_debug("xscale: %i, yscale: %i\n", (int)xscale, (int)yscale);
		Ft_Gpu_CoCmd_Translate(ESD_Host, -x_center << 16, -y_center << 16);
		Ft_Gpu_CoCmd_SetMatrix(ESD_Host);
		Ft_Gpu_CoCmd_LoadIdentity(ESD_Host);
		ESD_Dl_BEGIN(BITMAPS);
		Ft_Gpu_CoCmd_SendCmd(ESD_Host, VERTEX2F(x * 16, y * 16));
		ESD_Dl_END();
		ESD_Dl_RESTORE_CONTEXT();
	}
}

void ESD_Render_BitmapRotate(ESD_BitmapCell bitmapCell, ft_argb32_t c, ESD_Rect16 globalRect, int32_t rotateAngle)
{
	EVE_HalContext *phost = ESD_Host;
	ESD_BitmapInfo *bitmapInfo;
	uint16_t cell;
	uint8_t handle;
	(void)phost;

	if (!bitmapCell.Info)
		return;

	bitmapInfo = bitmapCell.Info;
	cell = bitmapCell.Cell;
	handle = ESD_Dl_Bitmap_Setup(bitmapInfo);

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
		ESD_Dl_COLOR_ARGB(c);
		ESD_Dl_SAVE_CONTEXT();

		ESD_Dl_Bitmap_WidthHeight_BILINEAR(handle, TITLE_SIZE, TITLE_SIZE); //Bitmap_Size command

		ESD_Dl_BEGIN(BITMAPS);

		if (EVE_CHIPID >= EVE_FT810)
			ESD_Dl_VERTEX_FORMAT(4);
		ESD_Dl_CELL_Paged(handle, cell);
		scope
		{
			int dx, dy;
			for (dx = (x - radius); dx < (x + radius); dx += TITLE_SIZE)
			{
				for (dy = (y - radius); dy < (y + radius); dy += TITLE_SIZE)
				{
					//eve_printf_debug("draw tile %d\n",tilenumber ++);
					tilenumber++;
					Ft_Gpu_CoCmd_LoadIdentity(ESD_Host);
					Ft_Gpu_CoCmd_Translate(ESD_Host, (x - dx) << 16, (y - dy) << 16);

					Ft_Gpu_CoCmd_Rotate(ESD_Host, rotateAngle);
					Ft_Gpu_CoCmd_Translate(ESD_Host, -x_center << 16, -y_center << 16);
					//Ft_Gpu_CoCmd_Translate(ESD_Host, -x_center, -y_center);

					Ft_Gpu_CoCmd_SetMatrix(ESD_Host);
					Ft_Gpu_CoCmd_SendCmd(ESD_Host, VERTEX2F(dx * 16, dy * 16));
				}
			}
		}
		//eve_printf_debug("draw tile %d\n",tilenumber);
		Ft_Gpu_CoCmd_LoadIdentity(ESD_Host);
		ESD_Dl_RESTORE_CONTEXT();
	}
}

// Get scaled size
ESD_Size16 ESD_Primitive_GetScaledSize(ESD_Size16 boundary, ESD_Size16 original, uint8_t scaling)
{
	switch (scaling)
	{
	case ESD_SCALING_FILL: // Keep Aspect ratio, extend outside rect
	case ESD_SCALING_FIT: // Keep Aspect ratio, stay inside rect
	{
		// There are two real options: scale original to boundary width or scale original to boundary height
		int32_f16_t boundaryRatio = (((int32_f16_t)boundary.Width) << 16) / ((int32_f16_t)boundary.Height);
		int32_f16_t originalRatio = (((int32_f16_t)original.Width) << 16) / ((int32_f16_t)original.Height);
		bool originalWider;
		bool wantFit;
		ESD_Size16 res;
		if (boundaryRatio == originalRatio)
			return boundary;
		originalWider = originalRatio > boundaryRatio;
		wantFit = scaling == ESD_SCALING_FIT;
		if (originalWider ^ wantFit) // (boundary higher && want fill) || (original higher && want fit)
		    //                          (original wider && want fill)  || (boundary wider && want fit)
		{
			// Scale to height
			int32_f16_t scale;
			res.Height = boundary.Height;
			scale = (((int32_f16_t)boundary.Height) << 16) / ((int32_f16_t)original.Height);
			res.Width = (((int32_f16_t)original.Width) * scale) >> 16;
		}
		else
		{
			// Scale to width
			int32_f16_t scale;
			res.Width = boundary.Width;
			scale = (((int32_f16_t)boundary.Width) << 16) / ((int32_f16_t)original.Width);
			res.Height = (((int32_f16_t)original.Height) * scale) >> 16;
		}
		return res;
	}
	case ESD_SCALING_STRETCH:
		return boundary;
	default: // ESD_SCALING_ORIGINAL
		return original;
	}
}

// Get alignment position
ESD_Rect16 ESD_Primitive_GetAlignedRect(ESD_Size16 boundary, ESD_Size16 size, uint8_t align)
{
	ESD_Rect16 res;
	uint8_t halign;
	uint8_t valign;

	res.Size = size;
	halign = ESD_ALIGN_HORIZONTAL(align);
	switch (halign)
	{
	case ESD_ALIGN_LEFTX:
		res.X = 0;
		break;
	case ESD_ALIGN_CENTERX:
		res.X = (boundary.Width - size.Width) >> 1;
		break;
	case ESD_ALIGN_RIGHTX:
		res.X = (boundary.Width - size.Width);
		break;
	default: // ESD_ALIGN_FILLX
		res.X = 0;
		res.Width = boundary.Width;
		break;
	}
	valign = ESD_ALIGN_VERTICAL(align);
	switch (valign)
	{
	case ESD_ALIGN_TOPY:
		res.Y = 0;
		break;
	case ESD_ALIGN_CENTERY:
		res.Y = (boundary.Height - size.Height) >> 1;
		break;
	case ESD_ALIGN_BOTTOMY:
		res.Y = (boundary.Height - size.Height);
		break;
	default: // ESD_ALIGN_FILLY
		res.Y = 0;
		res.Height = boundary.Height;
		break;
	}
	return res;
}

/* end of file */
