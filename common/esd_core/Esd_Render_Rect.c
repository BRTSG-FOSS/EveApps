
#include "Esd_Render.h"
#include "Esd_Context.h"

// Rectangle drawing with some logic to convert from radius to line width and width height to positions to simplify usage
ESD_CORE_EXPORT void Esd_Render_RectF(esd_int32_f4_t x, esd_int32_f4_t y, esd_int32_f4_t w, esd_int32_f4_t h, esd_int32_f4_t radius, esd_argb32_t color)
{
	EVE_HalContext *phost = Esd_GetHost();
	int32_t width = radius + 8;
	int32_t x0 = x + radius;
	int32_t y0 = y + radius;
	int32_t x1 = x + w - 16 - radius;
	int32_t y1 = y + h - 16 - radius;
	EVE_CoDl_colorArgb_ex(phost, color);
	EVE_CoDl_lineWidth(phost, width);
	EVE_CoDl_begin(phost, RECTS);
	EVE_CoDl_vertex2f_4(phost, x0, y0);
	EVE_CoDl_vertex2f_4(phost, x1, y1);
	EVE_CoDl_end(phost);
}

ESD_CORE_EXPORT void Esd_Render_Rect_Stroke(
    esd_int32_f4_t x, esd_int32_f4_t y,
    esd_int32_f4_t width, esd_int32_f4_t height,
    esd_int32_f4_t radius, esd_int32_f4_t border,
    uint8_t stroke, esd_argb32_t color)
{
	EVE_HalContext *phost = Esd_GetHost();
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
		uint32_t alpha;

		// Expand border centrally to 16 subpixels
		esd_int32_f4_t adjust = (16 - border);
		esd_int32_f4_t innerAdjust = adjust >> 1;
		esd_int32_f4_t outerAdjust = adjust - innerAdjust;
		innerRadius -= innerAdjust;
		outerRadius += outerAdjust;

		// Lessen alpha
		alpha = color >> 24;
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
	int vertexFormat = 4;
	while (vertexFormat > 0 && (x0 < EVE_VERTEX2F_MIN || x0 > EVE_VERTEX2F_MAX || x1 < EVE_VERTEX2F_MIN || x1 > EVE_VERTEX2F_MAX || x0 < EVE_VERTEX2F_MIN || x0 > EVE_VERTEX2F_MAX || y1 < EVE_VERTEX2F_MIN || y1 > EVE_VERTEX2F_MAX))
	{
		--vertexFormat;
		x0 >>= 1;
		x1 >>= 1;
		y0 >>= 1;
		y1 >>= 1;
	}
	EVE_CoDl_vertexFormat(phost, vertexFormat);

	/* ---- */
	/* NOTE: Bypassing CoDl optimizer on purpose inside a SAVE_CONTEXT block */
	/* ---- */

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

	/* ---- */
	/* NOTE: Bypassed CoDl optimizer on purpose inside a SAVE_CONTEXT block */
	/* ---- */

	EVE_CoDl_end(phost);
}

/* end of file */
