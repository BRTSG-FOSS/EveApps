
#include "Esd_Render.h"
#include "Esd_Context.h"
#include "Esd_GpuAlloc.h"
#include "Esd_BitmapHandle.h"
#include "Esd_Math.h"

#include "math.h"

// Multi gradient rendering state
Esd_GpuHandle s_MultiGradient_GpuHandle;
uint32_t s_MultiGradient_Cell;

// Switch to use coprocessor for scaling matrix. Uses more bitmap matrix entries in the display list
#define ESD_MULTIGRADIENT_CO_SCALE 0

// The maximum number of multi gradients that can be on screen at once, multiplied by two
#define ESD_MULTIGRADIENT_MAX_NB (1 << 6)

ESD_CORE_EXPORT void Esd_Render_MultiGradient(int16_t x, int16_t y, int16_t width, int16_t height, esd_argb32_t topLeft, esd_argb32_t topRight, esd_argb32_t bottomLeft, esd_argb32_t bottomRight)
{
	EVE_HalContext *phost = Esd_GetHost();
	uint32_t addr;
	bool alpha;
	uint16_t colors[4];

	// Don't render empty
	if (width == 0 || height == 0)
		return;

	// Get address of RAM_G used for gradient palette
	addr = Esd_GpuAlloc_Get(Esd_GAlloc, s_MultiGradient_GpuHandle);
	if (addr == GA_INVALID)
	{
		// Allocate enough memory for 32 gradients.
		// Two bytes * four pixels * 64 gradients, 32 per frame flip.
		s_MultiGradient_GpuHandle = Esd_GpuAlloc_Alloc(Esd_GAlloc, 2 * 4 * ESD_MULTIGRADIENT_MAX_NB, GA_GC_FLAG);
		addr = Esd_GpuAlloc_Get(Esd_GAlloc, s_MultiGradient_GpuHandle);
	}
	if (addr == GA_INVALID)
	{
		eve_printf_debug("Unable to allocate RAM_G for MultiGradient\n");
		return;
	}

	// Select cell address directly
	addr += (s_MultiGradient_Cell * 8);

	// Check if the colors have alpha, if so we'll use ARGB4, otherwise RGB565
	alpha = topLeft < 0xFF000000 || topRight < 0xFF000000 || bottomLeft < 0xFF000000 || bottomRight < 0xFF000000;
	if (alpha)
	{
		colors[0] = ESD_COLOR_ARGB4(topLeft);
		colors[1] = ESD_COLOR_ARGB4(topRight);
		colors[2] = ESD_COLOR_ARGB4(bottomLeft);
		colors[3] = ESD_COLOR_ARGB4(bottomRight);
	}
	else
	{
		colors[0] = ESD_COLOR_RGB565(topLeft);
		colors[1] = ESD_COLOR_RGB565(topRight);
		colors[2] = ESD_COLOR_RGB565(bottomLeft);
		colors[3] = ESD_COLOR_RGB565(bottomRight);
	}

	// Write gradient palette to RAM_G
	EVE_CoCmd_memWrite(phost, addr, 8);
	EVE_Cmd_wrMem(phost, (void *)colors, 8);

	// Set required state
	EVE_CoDl_colorArgb_ex(phost, ESD_ARGB_WHITE);

	// Use the scratch handle
	EVE_CoDl_bitmapHandle(phost, ESD_CO_SCRATCH_HANDLE);
	EVE_CoDl_vertexFormat(phost, 0);
	EVE_CoDl_begin(phost, BITMAPS);

	/* ---- */
	/* NOTE: Bypassing CoDl optimizer on purpose inside a SAVE_CONTEXT block */
	/* ---- */

	// Use local rendering context, bypass ESD display list functions.
	// This is useful here, since we're changing bitmap transform matrices, which may use a lot of display list entries.
	EVE_CoCmd_dl(Esd_Host, SAVE_CONTEXT());

	// Initialize the bitmap options
	if (EVE_CHIPID >= EVE_FT810)
	{
		EVE_CoCmd_dl(Esd_Host, BITMAP_LAYOUT_H(0, 0));
		EVE_CoCmd_dl(Esd_Host, BITMAP_SIZE_H(width >> 9, height >> 9));
	}
	EVE_CoCmd_dl(Esd_Host, BITMAP_LAYOUT(alpha ? ARGB4 : RGB565, 4, 2));
	EVE_CoCmd_dl(Esd_Host, BITMAP_SIZE(BILINEAR, REPEAT, REPEAT, width, height));

	EVE_CoCmd_dl(Esd_Host, BITMAP_SOURCE(addr));

	// Set the scaling matrix
#if ESD_MULTIGRADIENT_CO_SCALE
	EVE_CoCmd_loadIdentity(Esd_Host);
	EVE_CoCmd_scale(Esd_Host, (int32_t)width << 16, (int32_t)height << 16);
	EVE_CoCmd_setMatrix(Esd_Host);
#else
	if (EVE_CHIPID >= EVE_BT815)
	{
		EVE_CoCmd_dl(Esd_Host, BITMAP_TRANSFORM_A_EXT(1, 0x8000 / width));
		EVE_CoCmd_dl(Esd_Host, BITMAP_TRANSFORM_E_EXT(1, 0x8000 / height));
	}
	else
	{
		EVE_CoCmd_dl(Esd_Host, BITMAP_TRANSFORM_A(0x0100 / width));
		EVE_CoCmd_dl(Esd_Host, BITMAP_TRANSFORM_E(0x0100 / height));
	}
#endif

	if (EVE_CHIPID >= EVE_BT815)
	{
		EVE_CoCmd_dl(Esd_Host, VERTEX2F(x, y));
	}
	else
	{
		EVE_CoCmd_dl(Esd_Host, VERTEX2II(x, y, ESD_CO_SCRATCH_HANDLE, 0));
	}

	// Restore rendering context, ESD display list optimizations functions should be used again after this.
#if ESD_MULTIGRADIENT_CO_SCALE
	EVE_CoCmd_loadIdentity(Esd_Host);
#endif
	EVE_CoCmd_dl(Esd_Host, RESTORE_CONTEXT());

	/* ---- */
	/* NOTE: Bypassed CoDl optimizer on purpose inside a SAVE_CONTEXT block */
	/* ---- */

	EVE_CoDl_end(phost);

	// Move to the next cell in the bitmap for next gradient
	++s_MultiGradient_Cell;
	s_MultiGradient_Cell &= (ESD_MULTIGRADIENT_MAX_NB - 1);
}

ESD_CORE_EXPORT void Esd_Render_MultiGradient_Rounded(int16_t x, int16_t y, int16_t width, int16_t height, esd_int32_f4_t radius, uint8_t alpha, esd_argb32_t topLeft, esd_argb32_t topRight, esd_argb32_t bottomLeft, esd_argb32_t bottomRight)
{
	EVE_HalContext *phost = Esd_GetHost();

	// EVE_CoDl_saveContext(phost);

	// Set alpha of the target rendering area to 255
	// EVE_CoDl_clear_COLOR_A(255);
	// Esd_Rect16 scissor = Esd_Scissor_Set(globalRect);
	// EVE_CoDl_clear(1, 0, 0);
	// Esd_Scissor_Reset(scissor);
	EVE_CoDl_colorArgb_ex(phost, ESD_ARGB_WHITE);
	EVE_CoDl_colorMask(Esd_Host, 0, 0, 0, 1);
	EVE_CoDl_lineWidth(phost, 16);
	EVE_CoDl_begin(phost, RECTS);
	EVE_CoDl_vertexFormat(phost, 0);
	EVE_CoDl_vertex2f(phost, x, y);
	EVE_CoDl_vertex2f(phost, x + width, y + height);
	EVE_CoDl_end(phost);
	EVE_CoDl_colorMask(Esd_Host, 1, 1, 1, 1);

	// Draw rounded rectangle as masking shape
	EVE_CoDl_blendFunc(Esd_Host, ZERO, ONE_MINUS_SRC_ALPHA);
	Esd_Render_RectF(x << 4, y << 4, width << 4, height << 4, radius, ESD_COMPOSE_RGB_ALPHA(0xFFFFFF, alpha));

	// Draw color using mask alpha
	EVE_CoDl_blendFunc(Esd_Host, ONE_MINUS_DST_ALPHA, ONE);
	Esd_Render_MultiGradient(x, y, width, height, topLeft | 0xFF000000, topRight | 0xFF000000, bottomLeft | 0xFF000000, bottomRight | 0xFF000000);

	// Restore context
	// EVE_CoDl_restoreContext(phost);
	EVE_CoDl_blendFunc_default(Esd_Host);
}

ESD_CORE_EXPORT void Esd_Render_RectF_Grad(esd_int32_f4_t x, esd_int32_f4_t y, esd_int32_f4_t w, esd_int32_f4_t h, esd_argb32_t color1, esd_argb32_t color2, int16_t direction, uint8_t style)
{

	EVE_HalContext *phost = Esd_GetHost();

	int16_t x0 = x >> 4;
	int16_t y0 = y >> 4;
	int16_t w1 = w >> 4;
	int16_t h1 = h >> 4;

	// Don't render empty
	if (w1 == 0 || h1 == 0)
		return;

	// FIXME: Use rect for parameters
	Esd_Rect16 rect = {
		.X = x0,
		.Y = y0,
		.Width = w1,
		.Height = h1,
	};

	if (color1 == color2)
	{
		// Not a gradient
		EVE_CoDl_saveContext(phost);
		Esd_Scissor_Set(rect);
		Esd_Render_Rect(x0, y0, w1, h1, 1, color1);
		EVE_CoDl_restoreContext(phost);
		return;
	}

	eve_scope()
	{
		int16_t x1 = x0 + (w1 >> 1);
		int16_t y1 = y0 + (h1 >> 1);
		int16_t dx, dy = 0;
		double radius, sine, cosine = 0;

		if (style == ESD_PERPENDICULAR_STYLE_OFF)
		{
			//printf("ESD_PERPENDICULAR_STYLE_OFF\n");
			radius = direction * M_PI / 180.0f;
			sine = sin(radius), cosine = cos(radius);

			int32_t wwhh = (int32_t)w1 * w1 + (int32_t)h1 * h1;
			int16_t l = (int16_t)(sqrt((double)wwhh * 0.8)); // use 80% to apply gradient effect
			int16_t half = l >> 1;
			dy = (int16_t)(half * sine);
			dx = (int16_t)(half * cosine);
		}
		else
		{
			//printf("ESD_PERPENDICULAR_STYLE_ON\n");
			switch (style)
			{
			case ESD_PERPENDICULAR_STYLE_0:
				direction = 0;
				break;
			case ESD_PERPENDICULAR_STYLE_90:
				direction = 90;
				break;
			case ESD_PERPENDICULAR_STYLE_180:
				direction = 180;
				break;
			case ESD_PERPENDICULAR_STYLE_270:
				direction = 270;
				break;
			default:
				direction = 0;
				break;
			}

			radius = direction * M_PI / 180.0f;
			sine = sin(radius), cosine = cos(radius);

			dx = (int16_t)(0.4 * w1 * cosine);
			dy = (int16_t)(0.4 * h1 * sine);
		}

		Esd_Rect16 s = Esd_Scissor_Set(rect);
		EVE_CoCmd_gradient(phost, x1 - dx, y1 - dy, color1, x1 + dx, y1 + dy, color2); // original
		Esd_Scissor_Reset(s);
	}
}

ESD_CORE_EXPORT void Esd_Render_Rect_Grad(int32_t x, int32_t y, esd_int32_f4_t w, esd_int32_f4_t h, esd_argb32_t color1, esd_argb32_t color2, int16_t direction, uint8_t style)
{
	Esd_Render_RectF_Grad(x << 4, y << 4, w << 4, h << 4, color1, color2, direction, style);
}

/* end of file */
