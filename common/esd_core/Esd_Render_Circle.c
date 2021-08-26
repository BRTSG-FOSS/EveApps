
#include "Esd_Render.h"
#include "Esd_Context.h"
#include "Esd_Scissor.h"

ESD_CORE_EXPORT void Esd_Render_Circle_Stroke(
    esd_int32_f4_t x, esd_int32_f4_t y,
    esd_int32_f4_t radius, esd_int32_f4_t border,
    uint8_t stroke, esd_argb32_t color)
{
	EVE_HalContext *phost = Esd_GetHost();
	esd_int32_f4_t r = radius;
	esd_int32_f4_t innerRadius;
	esd_int32_f4_t outerRadius;

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

	if (innerRadius <= 0)
	{
		// TODO: This is just a circle
	}

	EVE_CoDl_begin(phost, POINTS);
	EVE_CoDl_colorArgb_ex(phost, color);
	EVE_CoDl_vertexFormat(phost, 4);

	/* ---- */
	/* NOTE: Bypassing CoDl optimizer on purpose inside a SAVE_CONTEXT block */
	/* ---- */

	// Use local rendering context, bypass EVE_CoDl display list functions.
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

	// Restore rendering context, EVE_CoDl display list optimizations functions should be used again after this.
	EVE_CoCmd_dl(phost, RESTORE_CONTEXT());

	/* ---- */
	/* NOTE: Bypassed CoDl optimizer on purpose inside a SAVE_CONTEXT block */
	/* ---- */

	EVE_CoDl_end(phost);
}

/* end of file */
