
#include "ESD_Core.h"
#include "ESD_Primitives.h"
#include "ESD_GpuAlloc.h"
#include "ESD_BitmapHandle.h"

// Multi gradient rendering state
ESD_GpuHandle s_MultiGradient_GpuHandle;
uint32_t s_MultiGradient_Cell;

// Switch to use coprocessor for scaling matrix. Uses more bitmap matrix entries in the display list
#define ESD_MULTIGRADIENT_CO_SCALE 0

// The maximum number of multi gradients that can be on screen at once, multiplied by two
#define ESD_MULTIGRADIENT_MAX_NB (1 << 6)

void Esd_Render_MultiGradient(int16_t x, int16_t y, int16_t width, int16_t height, ft_argb32_t topLeft, ft_argb32_t topRight, ft_argb32_t bottomLeft, ft_argb32_t bottomRight)
{
	EVE_HalContext *phost = ESD_Host;
	(void)phost;
	// Don't render empty
	if (width == 0 || height == 0)
		return;

	// Get address of RAM_G used for gradient palette
	uint32_t addr = ESD_GpuAlloc_Get(ESD_GAlloc, s_MultiGradient_GpuHandle);
	if (addr == GA_INVALID)
	{
		// Allocate enough memory for 32 gradients.
		// Two bytes * four pixels * 64 gradients, 32 per frame flip.
		s_MultiGradient_GpuHandle = ESD_GpuAlloc_Alloc(ESD_GAlloc, 2 * 4 * ESD_MULTIGRADIENT_MAX_NB, GA_GC_FLAG);
		addr = ESD_GpuAlloc_Get(ESD_GAlloc, s_MultiGradient_GpuHandle);
	}
	if (addr == GA_INVALID)
	{
		eve_printf_debug("Unable to allocate RAM_G for MultiGradient\n");
		return;
	}

	// Select cell address directly
	addr += (s_MultiGradient_Cell * 8);

	// Check if the colors have alpha, if so we'll use ARGB4, otherwise RGB565
	bool alpha = topLeft < 0xFF000000 || topRight < 0xFF000000 || bottomLeft < 0xFF000000 || bottomRight < 0xFF000000;
	uint16_t colors[4];
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
	ESD_Dl_colorArgb(ESD_ARGB_WHITE);

	// Use the scratch handle
	ESD_Dl_BITMAP_HANDLE(ESD_CO_SCRATCH_HANDLE);
	if (EVE_CHIPID >= EVE_FT810)
		ESD_Dl_VERTEX_FORMAT(0);
	ESD_Dl_BEGIN(BITMAPS);

	// Use local rendering context, bypass ESD display list functions.
	// This is useful here, since we're changing bitmap transform matrices, which may use a lot of display list entries.
	EVE_CoCmd_dl(ESD_Host, SAVE_CONTEXT());

	// Initialize the bitmap options
	if (EVE_CHIPID >= EVE_FT810)
	{
		EVE_CoCmd_dl(ESD_Host, BITMAP_LAYOUT_H(0, 0));
		EVE_CoCmd_dl(ESD_Host, BITMAP_SIZE_H(width >> 9, height >> 9));
	}
	EVE_CoCmd_dl(ESD_Host, BITMAP_LAYOUT(alpha ? ARGB4 : RGB565, 4, 2));
	EVE_CoCmd_dl(ESD_Host, BITMAP_SIZE(BILINEAR, REPEAT, REPEAT, width, height));

	EVE_CoCmd_dl(ESD_Host, BITMAP_SOURCE(addr));

	// Set the scaling matrix
#if ESD_MULTIGRADIENT_CO_SCALE
	EVE_CoCmd_loadIdentity(ESD_Host);
	EVE_CoCmd_scale(ESD_Host, (int32_t)width << 16, (int32_t)height << 16);
	EVE_CoCmd_setMatrix(ESD_Host);
#else
	if (EVE_CHIPID >= EVE_BT815)
	{
	EVE_CoCmd_dl(ESD_Host, BITMAP_TRANSFORM_A_EXT(1, 0x8000 / width));
	EVE_CoCmd_dl(ESD_Host, BITMAP_TRANSFORM_E_EXT(1, 0x8000 / height));
	}
	else
	{
	EVE_CoCmd_dl(ESD_Host, BITMAP_TRANSFORM_A(0x0100 / width));
	EVE_CoCmd_dl(ESD_Host, BITMAP_TRANSFORM_E(0x0100 / height));
	}
#endif

	if (EVE_CHIPID >= EVE_BT815)
	{
	EVE_CoCmd_dl(ESD_Host, VERTEX2F(x, y));
	}
	else
	{
	EVE_CoCmd_dl(ESD_Host, VERTEX2II(x, y, ESD_CO_SCRATCH_HANDLE, 0));
	}

	// Restore rendering context, ESD display list optimizations functions should be used again after this.
#if ESD_MULTIGRADIENT_CO_SCALE
	EVE_CoCmd_loadIdentity(ESD_Host);
#endif
	EVE_CoCmd_dl(ESD_Host, RESTORE_CONTEXT());
	ESD_Dl_END();

	// Move to the next cell in the bitmap for next gradient
	++s_MultiGradient_Cell;
	s_MultiGradient_Cell &= (ESD_MULTIGRADIENT_MAX_NB - 1);
}

void Esd_Render_MultiGradient_Rounded(int16_t x, int16_t y, int16_t width, int16_t height, int32_f4_t radius, uint8_t alpha, ft_argb32_t topLeft, ft_argb32_t topRight, ft_argb32_t bottomLeft, ft_argb32_t bottomRight)
{
	// ESD_Dl_SAVE_CONTEXT();

	// Set alpha of the target rendering area to 255
	// ESD_Dl_CLEAR_COLOR_A(255);
	// ESD_Rect16 scissor = ESD_Dl_Scissor_Set(globalRect);
	// ESD_Dl_CLEAR(1, 0, 0);
	// ESD_Dl_Scissor_Reset(scissor);
	ESD_Dl_colorArgb(ESD_ARGB_WHITE);
	EVE_CoCmd_dl(ESD_Host, COLOR_MASK(0, 0, 0, 1));
	ESD_Dl_LINE_WIDTH(16);
	ESD_Dl_BEGIN(RECTS);
	ESD_Dl_VERTEX2F_0(x, y);
	ESD_Dl_VERTEX2F_0(x + width, y + height);
	ESD_Dl_END();
	EVE_CoCmd_dl(ESD_Host, COLOR_MASK(1, 1, 1, 1));

	// Draw rounded rectangle as masking shape
	EVE_CoCmd_dl(ESD_Host, BLEND_FUNC(ZERO, ONE_MINUS_SRC_ALPHA));
	ESD_Render_RectangleF(x << 4, y << 4, width << 4, height << 4, radius, ESD_ColorARGB_Combine(0xFFFFFF, alpha));

	// Draw color using mask alpha
	EVE_CoCmd_dl(ESD_Host, BLEND_FUNC(ONE_MINUS_DST_ALPHA, ONE));
	Esd_Render_MultiGradient(x, y, width, height, topLeft | 0xFF000000, topRight | 0xFF000000, bottomLeft | 0xFF000000, bottomRight | 0xFF000000);

	// Restore context
	// ESD_Dl_RESTORE_CONTEXT();
	EVE_CoCmd_dl(ESD_Host, BLEND_FUNC(SRC_ALPHA, ONE_MINUS_SRC_ALPHA));
}

/* end of file */
