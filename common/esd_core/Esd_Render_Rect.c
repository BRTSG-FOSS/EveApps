
#include "ESD_Context.h"
#include "ESD_Render.h"

void ESD_Render_Rect_Stroke(
    esd_int32_f4_t x, esd_int32_f4_t y,
    esd_int32_f4_t width, esd_int32_f4_t height,
    esd_int32_f4_t radius, esd_int32_f4_t border,
    uint8_t stroke, esd_argb32_t color)
{
	EVE_HalContext *phost = ESD_Host;
	esd_int32_f4_t r = radius;
	esd_int32_f4_t x0 = x + radius;
	esd_int32_f4_t y0 = y + radius;
	esd_int32_f4_t x1 = x + width - 16 - radius;
	esd_int32_f4_t y1 = y + height - 16 - radius;

	esd_int32_f4_t innerRadius; // = r - (border >> 1);
	esd_int32_f4_t outerRadius; // = innerRadius + border;

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
		esd_int32_f4_t adjust = (16 - border);
		esd_int32_f4_t innerAdjust = adjust >> 1;
		esd_int32_f4_t outerAdjust = adjust - innerAdjust;
		innerRadius -= innerAdjust;
		outerRadius += outerAdjust;

		// Lessen alpha
		uint32_t alpha = color >> 24;
		alpha *= border;
		alpha >>= 4; // Divide by 4
		color = (alpha << 24) | (color & 0xFFFFFF);
	}

	if (innerRadius < 16)
	{
		// TODO: Use separate inner and outer XY for hard corners
		esd_int32_f4_t adjust = 16 - innerRadius;
		innerRadius += adjust;
		outerRadius += adjust;
		x0 += adjust;
		y0 += adjust;
		x1 -= adjust;
		y1 -= adjust;
	}

	// Use local rendering context, bypass ESD display list functions.
	EVE_CoDl_begin(phost, RECTS);
	EVE_CoDl_colorArgb_ex(phost, color);
	if (EVE_CHIPID >= EVE_FT810)
		EVE_CoDl_vertexFormat(phost, 4);
	EVE_CoCmd_dl(phost, SAVE_CONTEXT());

	// Outer reset
	EVE_CoCmd_dl(phost, COLOR_MASK(0, 0, 0, 1));
	EVE_CoCmd_dl(phost, STENCIL_FUNC(ALWAYS, 0, 1));
	EVE_CoCmd_dl(phost, STENCIL_OP(REPLACE, REPLACE));
	EVE_CoCmd_dl(phost, LINE_WIDTH(outerRadius));
	EVE_CoCmd_dl(phost, VERTEX2F(x0, y0));
	EVE_CoCmd_dl(phost, VERTEX2F(x1, y1));

	// Inner alpha quantity
	EVE_CoCmd_dl(phost, BLEND_FUNC(ONE, ZERO));
	EVE_CoCmd_dl(phost, LINE_WIDTH(innerRadius + 32));
	EVE_CoCmd_dl(phost, VERTEX2F(x0, y0));
	EVE_CoCmd_dl(phost, VERTEX2F(x1, y1));

	// Inner alpha edge mask
	EVE_CoCmd_dl(phost, STENCIL_FUNC(ALWAYS, 1, 1));
	EVE_CoCmd_dl(phost, STENCIL_OP(REPLACE, REPLACE));
	EVE_CoCmd_dl(phost, BLEND_FUNC(ZERO, ONE_MINUS_SRC_ALPHA));
	EVE_CoCmd_dl(phost, COLOR_A(255));
	EVE_CoCmd_dl(phost, LINE_WIDTH(innerRadius));
	EVE_CoCmd_dl(phost, VERTEX2F(x0, y0));
	EVE_CoCmd_dl(phost, VERTEX2F(x1, y1));

	// Inner color, outer rect stencil mask
	EVE_CoCmd_dl(phost, COLOR_MASK(1, 1, 1, 1));
	EVE_CoCmd_dl(phost, BLEND_FUNC(DST_ALPHA, ONE_MINUS_DST_ALPHA));
	// EVE_CoCmd_dl(phost, LINE_WIDTH(innerRadius));
	EVE_CoCmd_dl(phost, VERTEX2F(x0, y0));
	EVE_CoCmd_dl(phost, VERTEX2F(x1, y1));

	// Outer rect
	EVE_CoCmd_dl(phost, STENCIL_FUNC(NOTEQUAL, 1, 255));
	EVE_CoCmd_dl(phost, BLEND_FUNC(SRC_ALPHA, ONE_MINUS_SRC_ALPHA));
	EVE_CoCmd_dl(phost, COLOR_A(ESD_DECOMPOSE_ALPHA(color)));
	EVE_CoCmd_dl(phost, LINE_WIDTH(outerRadius));
	EVE_CoCmd_dl(phost, VERTEX2F(x0, y0));
	EVE_CoCmd_dl(phost, VERTEX2F(x1, y1));

	// Restore rendering context, ESD display list optimizations functions should be used again after this.
	EVE_CoCmd_dl(phost, RESTORE_CONTEXT());
	EVE_CoDl_end(phost);
}

/* end of file */