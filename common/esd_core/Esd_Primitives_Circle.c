
#include "ESD_Core.h"
#include "ESD_Primitives.h"
#include "ESD_Dl.h"

void Esd_Render_Circle_Stroke(
    int32_f4_t x, int32_f4_t y,
    int32_f4_t radius, int32_f4_t border,
    uint8_t stroke, ft_argb32_t color)
{
	EVE_HalContext *phost = ESD_Host;
	int32_f4_t r = radius;
	int32_f4_t innerRadius;
	int32_f4_t outerRadius;

	if (border <= 0)
	{
		return;
	}

	switch (stroke)
	{
	case ESD_STROKE_INNER:
		innerRadius = r - border;
		outerRadius = r;
		break;
	case ESD_STROKE_OUTER:
		innerRadius = r;
		outerRadius = r + border;
		break;
	case ESD_STROKE_CENTER:
		innerRadius = r - (border >> 1);
		outerRadius = innerRadius + border;
		break;
	default:
		return;
	}

	if (border < 16)
	{
		// Expand border centrally to 16 subpixels
		int32_f4_t adjust = (16 - border);
		int32_f4_t innerAdjust = adjust >> 1;
		int32_f4_t outerAdjust = adjust - innerAdjust;
		innerRadius -= innerAdjust;
		outerRadius += outerAdjust;

		// Lessen alpha
		uint32_t alpha = color >> 24;
		alpha *= border;
		alpha >>= 4; // Divide by 4
		color = (alpha << 24) | (color & 0xFFFFFF);
	}

	if (innerRadius <= 0)
	{
		// TODO: This is just a circle
	}

	// Use local rendering context, bypass ESD display list functions.
	EVE_CoCmd_dl(phost, BEGIN(POINTS));
	ESD_Dl_colorArgb(color);
	if (EVE_CHIPID >= EVE_FT810)
		EVE_CoCmd_dl(phost, VERTEX_FORMAT(4));
	EVE_CoCmd_dl(phost, SAVE_CONTEXT());

	// Outer reset
	EVE_CoCmd_dl(phost, COLOR_MASK(0, 0, 0, 1));
	EVE_CoCmd_dl(phost, STENCIL_FUNC(ALWAYS, 0, 1));
	EVE_CoCmd_dl(phost, STENCIL_OP(REPLACE, REPLACE));
	EVE_CoCmd_dl(phost, LINE_WIDTH(outerRadius));
	EVE_CoCmd_dl(phost, VERTEX2F(x, y));

	// Inner alpha quantity
	EVE_CoCmd_dl(phost, COLOR_MASK(0, 0, 0, 1));
	EVE_CoCmd_dl(phost, BLEND_FUNC(ONE, ZERO));
	EVE_CoCmd_dl(phost, POINT_SIZE(innerRadius + 32));
	EVE_CoCmd_dl(phost, VERTEX2F(x, y));

	// Inner alpha edge mask
	EVE_CoCmd_dl(phost, STENCIL_FUNC(ALWAYS, 1, 1));
	EVE_CoCmd_dl(phost, STENCIL_OP(REPLACE, REPLACE));
	EVE_CoCmd_dl(phost, BLEND_FUNC(ZERO, ONE_MINUS_SRC_ALPHA));
	EVE_CoCmd_dl(phost, COLOR_A(255));
	EVE_CoCmd_dl(phost, POINT_SIZE(innerRadius));
	EVE_CoCmd_dl(phost, VERTEX2F(x, y));

	// Inner color, outer circle stencil mask
	EVE_CoCmd_dl(phost, COLOR_MASK(1, 1, 1, 1));
	EVE_CoCmd_dl(phost, BLEND_FUNC(DST_ALPHA, ONE_MINUS_DST_ALPHA));
	// EVE_CoCmd_dl(phost, POINT_SIZE(innerRadius));
	EVE_CoCmd_dl(phost, VERTEX2F(x, y));

	// Outer circle
	EVE_CoCmd_dl(phost, STENCIL_FUNC(NOTEQUAL, 1, 255));
	EVE_CoCmd_dl(phost, BLEND_FUNC(SRC_ALPHA, ONE_MINUS_SRC_ALPHA));
	EVE_CoCmd_dl(phost, COLOR_A(ESD_DECOMPOSE_ALPHA(color)));
	EVE_CoCmd_dl(phost, POINT_SIZE(outerRadius));
	EVE_CoCmd_dl(phost, VERTEX2F(x, y));

	// Restore rendering context, ESD display list optimizations functions should be used again after this.
	EVE_CoCmd_dl(phost, RESTORE_CONTEXT());
	EVE_CoCmd_dl(phost, END());
}

/* end of file */
