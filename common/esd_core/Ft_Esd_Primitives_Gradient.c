
#include "Ft_Esd_Core.h"
#include "Ft_Esd_Primitives.h"
#include "Ft_Esd_GpuAlloc.h"
#include "Ft_Esd_BitmapHandle.h"

// Multi gradient rendering state
Ft_Esd_GpuHandle s_MultiGradient_GpuHandle;
ft_uint32_t s_MultiGradient_Cell;

// Switch to use coprocessor for scaling matrix. Uses more bitmap matrix entries in the display list
#define ESD_MULTIGRADIENT_CO_SCALE 0

// The maximum number of multi gradients that can be on screen at once, multiplied by two
#define ESD_MULTIGRADIENT_MAX_NB (1 << 6)

ft_void_t Esd_Render_MultiGradient(ft_int16_t x, ft_int16_t y, ft_int16_t width, ft_int16_t height, ft_argb32_t topLeft, ft_argb32_t topRight, ft_argb32_t bottomLeft, ft_argb32_t bottomRight)
{
	EVE_HalContext *phost = Ft_Esd_Host;
	(void)phost;
	// Don't render empty
	if (width == 0 || height == 0)
		return;

	// Get address of RAM_G used for gradient palette
	ft_uint32_t addr = Ft_Esd_GpuAlloc_Get(Ft_Esd_GAlloc, s_MultiGradient_GpuHandle);
	if (addr == GA_INVALID)
	{
		// Allocate enough memory for 32 gradients.
		// Two bytes * four pixels * 64 gradients, 32 per frame flip.
		s_MultiGradient_GpuHandle = Ft_Esd_GpuAlloc_Alloc(Ft_Esd_GAlloc, 2 * 4 * ESD_MULTIGRADIENT_MAX_NB, GA_GC_FLAG);
		addr = Ft_Esd_GpuAlloc_Get(Ft_Esd_GAlloc, s_MultiGradient_GpuHandle);
	}
	if (addr == GA_INVALID)
	{
		eve_printf_debug("Unable to allocate RAM_G for MultiGradient\n");
		return;
	}

	// Select cell address directly
	addr += (s_MultiGradient_Cell * 8);

	// Check if the colors have alpha, if so we'll use ARGB4, otherwise RGB565
	ft_bool_t alpha = topLeft < 0xFF000000 || topRight < 0xFF000000 || bottomLeft < 0xFF000000 || bottomRight < 0xFF000000;
	ft_uint16_t colors[4];
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
	Ft_Gpu_CoCmd_MemWrite(Ft_Esd_Host, addr, 8);
	Eve_CoCmd_SendCmdArr(Ft_Esd_Host, (ft_uint32_t *)colors, 2);

	// Set required state
	Esd_Dl_COLOR_ARGB(ESD_ARGB_WHITE);

	// Use the scratch handle
	Esd_Dl_BITMAP_HANDLE(ESD_CO_SCRATCH_HANDLE);
	if (EVE_CHIPID >= EVE_FT810)
		Esd_Dl_VERTEX_FORMAT(0);
	Esd_Dl_BEGIN(BITMAPS);

	// Use local rendering context, bypass ESD display list functions.
	// This is useful here, since we're changing bitmap transform matrices, which may use a lot of display list entries.
	Eve_CoCmd_SendCmd(Ft_Esd_Host, SAVE_CONTEXT());

	// Initialize the bitmap options
	if (EVE_CHIPID >= EVE_FT810)
	{
		Eve_CoCmd_SendCmd(Ft_Esd_Host, BITMAP_LAYOUT_H(0, 0));
		Eve_CoCmd_SendCmd(Ft_Esd_Host, BITMAP_SIZE_H(width >> 9, height >> 9));
	}
	Eve_CoCmd_SendCmd(Ft_Esd_Host, BITMAP_LAYOUT(alpha ? ARGB4 : RGB565, 4, 2));
	Eve_CoCmd_SendCmd(Ft_Esd_Host, BITMAP_SIZE(BILINEAR, REPEAT, REPEAT, width, height));

	Eve_CoCmd_SendCmd(Ft_Esd_Host, BITMAP_SOURCE(addr));

	// Set the scaling matrix
#if ESD_MULTIGRADIENT_CO_SCALE
	Ft_Gpu_CoCmd_LoadIdentity(Ft_Esd_Host);
	Ft_Gpu_CoCmd_Scale(Ft_Esd_Host, (ft_int32_t)width << 16, (ft_int32_t)height << 16);
	Ft_Gpu_CoCmd_SetMatrix(Ft_Esd_Host);
#else
	if (EVE_CHIPID >= EVE_BT815)
	{
	Eve_CoCmd_SendCmd(Ft_Esd_Host, BITMAP_TRANSFORM_A_EXT(1, 0x8000 / width));
	Eve_CoCmd_SendCmd(Ft_Esd_Host, BITMAP_TRANSFORM_E_EXT(1, 0x8000 / height));
	}
	else
	{
	Eve_CoCmd_SendCmd(Ft_Esd_Host, BITMAP_TRANSFORM_A(0x0100 / width));
	Eve_CoCmd_SendCmd(Ft_Esd_Host, BITMAP_TRANSFORM_E(0x0100 / height));
	}
#endif

	if (EVE_CHIPID >= EVE_BT815)
	{
	Eve_CoCmd_SendCmd(Ft_Esd_Host, VERTEX2F(x, y));
	}
	else
	{
	Eve_CoCmd_SendCmd(Ft_Esd_Host, VERTEX2II(x, y, ESD_CO_SCRATCH_HANDLE, 0));
	}

	// Restore rendering context, ESD display list optimizations functions should be used again after this.
#if ESD_MULTIGRADIENT_CO_SCALE
	Ft_Gpu_CoCmd_LoadIdentity(Ft_Esd_Host);
#endif
	Eve_CoCmd_SendCmd(Ft_Esd_Host, RESTORE_CONTEXT());
	Esd_Dl_END();

	// Move to the next cell in the bitmap for next gradient
	++s_MultiGradient_Cell;
	s_MultiGradient_Cell &= (ESD_MULTIGRADIENT_MAX_NB - 1);
}

ft_void_t Esd_Render_MultiGradient_Rounded(ft_int16_t x, ft_int16_t y, ft_int16_t width, ft_int16_t height, ft_int32_f4_t radius, ft_uint8_t alpha, ft_argb32_t topLeft, ft_argb32_t topRight, ft_argb32_t bottomLeft, ft_argb32_t bottomRight)
{
	// Esd_Dl_SAVE_CONTEXT();

	// Set alpha of the target rendering area to 255
	// Esd_Dl_CLEAR_COLOR_A(255);
	// Ft_Esd_Rect16 scissor = Esd_Dl_Scissor_Set(globalRect);
	// Esd_Dl_CLEAR(1, 0, 0);
	// Esd_Dl_Scissor_Reset(scissor);
	Esd_Dl_COLOR_ARGB(ESD_ARGB_WHITE);
	Eve_CoCmd_SendCmd(Ft_Esd_Host, COLOR_MASK(0, 0, 0, 1));
	Esd_Dl_LINE_WIDTH(16);
	Esd_Dl_BEGIN(RECTS);
	Esd_Dl_VERTEX2F_0(x, y);
	Esd_Dl_VERTEX2F_0(x + width, y + height);
	Esd_Dl_END();
	Eve_CoCmd_SendCmd(Ft_Esd_Host, COLOR_MASK(1, 1, 1, 1));

	// Draw rounded rectangle as masking shape
	Eve_CoCmd_SendCmd(Ft_Esd_Host, BLEND_FUNC(ZERO, ONE_MINUS_SRC_ALPHA));
	Ft_Esd_Render_RectangleF(x << 4, y << 4, width << 4, height << 4, radius, Ft_Esd_ColorARGB_Combine(0xFFFFFF, alpha));

	// Draw color using mask alpha
	Eve_CoCmd_SendCmd(Ft_Esd_Host, BLEND_FUNC(ONE_MINUS_DST_ALPHA, ONE));
	Esd_Render_MultiGradient(x, y, width, height, topLeft | 0xFF000000, topRight | 0xFF000000, bottomLeft | 0xFF000000, bottomRight | 0xFF000000);

	// Restore context
	// Esd_Dl_RESTORE_CONTEXT();
	Eve_CoCmd_SendCmd(Ft_Esd_Host, BLEND_FUNC(SRC_ALPHA, ONE_MINUS_SRC_ALPHA));
}

/* end of file */
