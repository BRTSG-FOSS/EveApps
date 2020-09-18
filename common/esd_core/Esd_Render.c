
#include "Esd_Render.h"

#include "Esd_Context.h"
#include "Esd_Scissor.h"
#include "Esd_BitmapHandle.h"

ESD_CORE_EXPORT void Esd_Render_LineF(esd_int32_f4_t x0, esd_int32_f4_t y0, esd_int32_f4_t x1, esd_int32_f4_t y1, esd_int32_f3_t width, esd_argb32_t color)
{
	EVE_HalContext *phost = Esd_GetHost();
	EVE_CoDl_colorArgb_ex(phost, color);
	EVE_CoDl_lineWidth(phost, width);
	EVE_CoDl_begin(phost, LINES);
	EVE_CoDl_vertex2f_4(phost, x0, y0);
	EVE_CoDl_vertex2f_4(phost, x1, y1);
	EVE_CoDl_end(phost);
}

// Get scaled size
ESD_CORE_EXPORT Esd_Size16 Esd_Math_GetScaledSize(Esd_Size16 boundary, Esd_Size16 original, uint8_t scaling)
{
	switch (scaling)
	{
	case ESD_SCALING_FILL: // Keep Aspect ratio, extend outside rect
	case ESD_SCALING_FIT: // Keep Aspect ratio, stay inside rect
	{
		// There are two real options: scale original to boundary width or scale original to boundary height
		esd_int32_f16_t boundaryRatio = (((esd_int32_f16_t)boundary.Width) << 16) / ((esd_int32_f16_t)boundary.Height);
		esd_int32_f16_t originalRatio = (((esd_int32_f16_t)original.Width) << 16) / ((esd_int32_f16_t)original.Height);
		bool originalWider;
		bool wantFit;
		Esd_Size16 res;
		if (boundaryRatio == originalRatio)
			return boundary;
		originalWider = originalRatio > boundaryRatio;
		wantFit = scaling == ESD_SCALING_FIT;
		if (originalWider ^ wantFit) // (boundary higher && want fill) || (original higher && want fit)
		    //                          (original wider && want fill)  || (boundary wider && want fit)
		{
			// Scale to height
			esd_int32_f16_t scale;
			res.Height = boundary.Height;
			scale = (((esd_int32_f16_t)boundary.Height) << 16) / ((esd_int32_f16_t)original.Height);
			res.Width = ((((esd_int32_f16_t)original.Width) * scale) + 32768) >> 16;
		}
		else
		{
			// Scale to width
			esd_int32_f16_t scale;
			res.Width = boundary.Width;
			scale = (((esd_int32_f16_t)boundary.Width) << 16) / ((esd_int32_f16_t)original.Width);
			res.Height = ((((esd_int32_f16_t)original.Height) * scale) + 32768) >> 16;
		}
		esd_int32_f16_t resRatio = (((esd_int32_f16_t)res.Width) << 16) / ((esd_int32_f16_t)res.Height);
		if (abs(boundaryRatio - originalRatio) < abs(resRatio - originalRatio))
		{
			// Solution is worse, return existing solution
			return boundary;
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
ESD_CORE_EXPORT Esd_Rect16 Esd_Math_GetAlignedRect(Esd_Size16 boundary, Esd_Size16 size, uint8_t align)
{
	Esd_Rect16 res;
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
