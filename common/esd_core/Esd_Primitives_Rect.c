
#include "ESD_Core.h"
#include "ESD_Primitives.h"

ft_void_t Esd_Render_Rect_Stroke(
    ft_int32_f4_t x, ft_int32_f4_t y,
    ft_int32_f4_t width, ft_int32_f4_t height,
    ft_int32_f4_t radius, ft_int32_f4_t border,
    uint8_t stroke, ft_argb32_t color)
{
	EVE_HalContext *phost = ESD_Host;
	ft_int32_f4_t r = radius;
	ft_int32_f4_t x0 = x + radius;
	ft_int32_f4_t y0 = y + radius;
	ft_int32_f4_t x1 = x + width - 16 - radius;
	ft_int32_f4_t y1 = y + height - 16 - radius;

	ft_int32_f4_t innerRadius; // = r - (border >> 1);
	ft_int32_f4_t outerRadius; // = innerRadius + border;

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
		ft_int32_f4_t adjust = (16 - border);
		ft_int32_f4_t innerAdjust = adjust >> 1;
		ft_int32_f4_t outerAdjust = adjust - innerAdjust;
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
		ft_int32_f4_t adjust = 16 - innerRadius;
		innerRadius += adjust;
		outerRadius += adjust;
		x0 += adjust;
		y0 += adjust;
		x1 -= adjust;
		y1 -= adjust;
	}

	// Use local rendering context, bypass ESD display list functions.
	Esd_Dl_BEGIN(RECTS);
	Esd_Dl_COLOR_ARGB(color);
	if (EVE_CHIPID >= EVE_FT810)
		Esd_Dl_VERTEX_FORMAT(4);
	Eve_CoCmd_SendCmd(phost, SAVE_CONTEXT());

	// Outer reset
	Eve_CoCmd_SendCmd(phost, COLOR_MASK(0, 0, 0, 1));
	Eve_CoCmd_SendCmd(phost, STENCIL_FUNC(ALWAYS, 0, 1));
	Eve_CoCmd_SendCmd(phost, STENCIL_OP(REPLACE, REPLACE));
	Eve_CoCmd_SendCmd(phost, LINE_WIDTH(outerRadius));
	Eve_CoCmd_SendCmd(phost, VERTEX2F(x0, y0));
	Eve_CoCmd_SendCmd(phost, VERTEX2F(x1, y1));

	// Inner alpha quantity
	Eve_CoCmd_SendCmd(phost, BLEND_FUNC(ONE, ZERO));
	Eve_CoCmd_SendCmd(phost, LINE_WIDTH(innerRadius + 32));
	Eve_CoCmd_SendCmd(phost, VERTEX2F(x0, y0));
	Eve_CoCmd_SendCmd(phost, VERTEX2F(x1, y1));

	// Inner alpha edge mask
	Eve_CoCmd_SendCmd(phost, STENCIL_FUNC(ALWAYS, 1, 1));
	Eve_CoCmd_SendCmd(phost, STENCIL_OP(REPLACE, REPLACE));
	Eve_CoCmd_SendCmd(phost, BLEND_FUNC(ZERO, ONE_MINUS_SRC_ALPHA));
	Eve_CoCmd_SendCmd(phost, COLOR_A(255));
	Eve_CoCmd_SendCmd(phost, LINE_WIDTH(innerRadius));
	Eve_CoCmd_SendCmd(phost, VERTEX2F(x0, y0));
	Eve_CoCmd_SendCmd(phost, VERTEX2F(x1, y1));

	// Inner color, outer rect stencil mask
	Eve_CoCmd_SendCmd(phost, COLOR_MASK(1, 1, 1, 1));
	Eve_CoCmd_SendCmd(phost, BLEND_FUNC(DST_ALPHA, ONE_MINUS_DST_ALPHA));
	// Eve_CoCmd_SendCmd(phost, LINE_WIDTH(innerRadius));
	Eve_CoCmd_SendCmd(phost, VERTEX2F(x0, y0));
	Eve_CoCmd_SendCmd(phost, VERTEX2F(x1, y1));

	// Outer rect
	Eve_CoCmd_SendCmd(phost, STENCIL_FUNC(NOTEQUAL, 1, 255));
	Eve_CoCmd_SendCmd(phost, BLEND_FUNC(SRC_ALPHA, ONE_MINUS_SRC_ALPHA));
	Eve_CoCmd_SendCmd(phost, COLOR_A(ESD_DECOMPOSE_ALPHA(color)));
	Eve_CoCmd_SendCmd(phost, LINE_WIDTH(outerRadius));
	Eve_CoCmd_SendCmd(phost, VERTEX2F(x0, y0));
	Eve_CoCmd_SendCmd(phost, VERTEX2F(x1, y1));

	// Restore rendering context, ESD display list optimizations functions should be used again after this.
	Eve_CoCmd_SendCmd(phost, RESTORE_CONTEXT());
	Esd_Dl_END();
}

/* end of file */
