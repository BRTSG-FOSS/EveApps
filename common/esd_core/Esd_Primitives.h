
#ifndef FT_ESD_PRIMITIVES_H
#define FT_ESD_PRIMITIVES_H

#include "Ft_Esd.h"
#include "Ft_Esd_Dl.h"
#include "Ft_Esd_BitmapInfo.h"
#include "Ft_Esd_Math.h"

// Ft_Esd_Render_Rectangle
// Ft_Esd_Render_Line
// Ft_Esd_Render_Point
// Ft_Esd_Render_Bitmap
// Ft_Esd_Render_LineStrip
// Ft_Esd_Render_EdgeStrip

ESD_CATEGORY(EsdElements, DisplayName = "Elements", Category = EsdRenderable)
ESD_CATEGORY(EsdPrimitives, DisplayName = "Primitives", Category = EsdRenderable)

ESD_CATEGORY(EsdBasicWidegts, DisplayName = "Basics", Category = EsdWidgets)

// A function to combine RGB channel with 8 bit Alpha channel data into 32 bit integer data
ESD_FUNCTION(Ft_Esd_ColorARGB_Combine, Type = ft_argb32_t, DisplayName = "Color A+RGB Combine", Category = EsdUtilities, Inline)
ESD_PARAMETER(rgb, Type = ft_rgb32_t, Default = #FFFFFF)
ESD_PARAMETER(a, Type = ft_uint8_t, Default = 255)
static inline ft_argb32_t Ft_Esd_ColorARGB_Combine(ft_rgb32_t rgb, ft_uint8_t a)
{
	return (rgb & 0xFFFFFF) | (((ft_argb32_t)a) << 24);
}

#define ESD_DECOMPOSE_ALPHA(argb) ((argb) >> 24)
#define ESD_DECOMPOSE_RED(rgb) (((rgb) >> 16) & 0xFF)
#define ESD_DECOMPOSE_GREEN(rgb) (((rgb) >> 8) & 0xFF)
#define ESD_DECOMPOSE_BLUE(rgb) ((rgb)&0xFF)

#define ESD_COMPOSE_ARGB4(r, g, b, a) (((a & 0xF0) << 8) | ((r & 0xF0) << 4) | (g & 0xF0) | (b >> 4))
#define ESD_COMPOSE_RGB565(r, g, b) (((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3))
#define ESD_COMPOSE_ARGB1555(r, g, b, a) ((((a)&0x80) << 8) | (((r)&0xF8) << 7) | (((g)&0xF8) << 2) | ((b) >> 3))
#define ESD_COMPOSE_ARGB8888(r, g, b, a) ((a) << 24) | ((r) << 16) | ((g) << 8) | (b)

#define ESD_COLOR_ARGB4(argb) ESD_COMPOSE_ARGB4(ESD_DECOMPOSE_RED(argb), ESD_DECOMPOSE_GREEN(argb), ESD_DECOMPOSE_BLUE(argb), ESD_DECOMPOSE_ALPHA(argb))
#define ESD_COLOR_RGB565(rgb) ESD_COMPOSE_RGB565(ESD_DECOMPOSE_RED(rgb), ESD_DECOMPOSE_GREEN(rgb), ESD_DECOMPOSE_BLUE(rgb))
#define ESD_COLOR_ARGB1555(argb) ESD_COMPOSE_ARGB1555(ESD_DECOMPOSE_RED(argb), ESD_DECOMPOSE_GREEN(argb), ESD_DECOMPOSE_BLUE(argb), ESD_DECOMPOSE_ALPHA(argb))

#define ESD_ARGB_WHITE 0xFFFFFFFF
#define ESD_ARGB_BLACK 0xFF000000

// Rectangular gradient with four colored corners
ESD_RENDER(Esd_Render_MultiGradient, Type = ft_void_t, DisplayName = "ESD Multi Gradient", Include = "Ft_Esd_Primitives.h", Category = EsdPrimitives)
ESD_PARAMETER(x, Type = ft_int16_t, Default = 0)
ESD_PARAMETER(y, Type = ft_int16_t, Default = 0)
ESD_PARAMETER(width, Type = ft_int16_t, Default = 0)
ESD_PARAMETER(height, Type = ft_int16_t, Default = 0)
ESD_PARAMETER(topLeft, Type = ft_argb32_t, DisplayName = "Top Left", Default = #FFFF0000)
ESD_PARAMETER(topRight, Type = ft_argb32_t, DisplayName = "Top Right", Default = #FF00FF00)
ESD_PARAMETER(bottomLeft, Type = ft_argb32_t, DisplayName = "Bottom Left", Default = #FF0000FF)
ESD_PARAMETER(bottomRight, Type = ft_argb32_t, DisplayName = "Bottom Right", Default = #FF000000)
ft_void_t Esd_Render_MultiGradient(ft_int16_t x, ft_int16_t y, ft_int16_t width, ft_int16_t height, ft_argb32_t topLeft, ft_argb32_t topRight, ft_argb32_t bottomLeft, ft_argb32_t bottomRight);

// Rounded rectangular gradient with four colored corners
ESD_RENDER(Esd_Render_MultiGradient_Rounded, Type = ft_void_t, DisplayName = "ESD Multi Gradient (Rounded)", Include = "Ft_Esd_Primitives.h", Category = EsdPrimitives)
ESD_PARAMETER(x, Type = ft_int16_t, Default = 0)
ESD_PARAMETER(y, Type = ft_int16_t, Default = 0)
ESD_PARAMETER(width, Type = ft_int16_t, Default = 0)
ESD_PARAMETER(height, Type = ft_int16_t, Default = 0)
ESD_PARAMETER(radius, Type = ft_int32_f4_t, DisplayName = "Radius", Default = 4)
ESD_PARAMETER(alpha, Type = ft_uint8_t, DisplayName = "Alpha", Default = 255)
ESD_PARAMETER(topLeft, Type = ft_rgb32_t, DisplayName = "Top Left", Default = #FF0012)
ESD_PARAMETER(topRight, Type = ft_rgb32_t, DisplayName = "Top Right", Default = #31FF00)
ESD_PARAMETER(bottomLeft, Type = ft_rgb32_t, DisplayName = "Bottom Left", Default = #6700FF)
ESD_PARAMETER(bottomRight, Type = ft_rgb32_t, DisplayName = "Bottom Right", Default = #00D1FF)
ft_void_t Esd_Render_MultiGradient_Rounded(ft_int16_t x, ft_int16_t y, ft_int16_t width, ft_int16_t height, ft_int32_f4_t radius, ft_uint8_t alpha, ft_argb32_t topLeft, ft_argb32_t topRight, ft_argb32_t bottomLeft, ft_argb32_t bottomRight);

// Circle stroke
ft_void_t Esd_Render_Circle_Stroke(
    ft_int32_f4_t x, ft_int32_f4_t y,
    ft_int32_f4_t radius, ft_int32_f4_t border,
    uint8_t stroke, ft_argb32_t color);

// Rect stroke
ft_void_t Esd_Render_Rect_Stroke(
    ft_int32_f4_t x, ft_int32_f4_t y,
    ft_int32_f4_t width, ft_int32_f4_t height,
    ft_int32_f4_t radius, ft_int32_f4_t border,
    uint8_t stroke, ft_argb32_t color);

// Basic bitmap rendering
ESD_RENDER(Ft_Esd_Render_Bitmap, Type = ft_void_t, DisplayName = "ESD Bitmap", Category = EsdPrimitives, Icon = ":/icons/image.png", Include = "Ft_Esd_Primitives.h")
ESD_PARAMETER(x, Type = ft_int16_t, Default = 0)
ESD_PARAMETER(y, Type = ft_int16_t, Default = 0)
ESD_PARAMETER(bitmapCell, Type = Ft_Esd_BitmapCell, DisplayName = "Bitmap Cell")
// Color to be multiplied with bitmap color, can be used for alpha fade for example
ESD_PARAMETER(c, Type = ft_argb32_t, DisplayName = "Color", Default = #FFFFFFFF)
ft_void_t Ft_Esd_Render_Bitmap(ft_int16_t x, ft_int16_t y, Ft_Esd_BitmapCell bitmapCell, ft_argb32_t c);

// Scaled bitmap rendering
ESD_RENDER(Ft_Esd_Render_BitmapScaled, Type = ft_void_t, DisplayName = "ESD Bitmap Scaled", Category = EsdPrimitives, Icon = ":/icons/image.png", Include = "Ft_Esd_Primitives.h")
ESD_PARAMETER(x, Type = ft_int16_t, Default = 0)
ESD_PARAMETER(y, Type = ft_int16_t, Default = 0)
ESD_PARAMETER(bitmapCell, Type = Ft_Esd_BitmapCell, DisplayName = "Bitmap Cell")
// Color to be multiplied with bitmap color, can be used for alpha fade for example
ESD_PARAMETER(c, Type = ft_argb32_t, DisplayName = "Color", Default = #FFFFFFFF)
// Horizontal scaling factor
ESD_PARAMETER(xscale, Type = ft_int32_f16_t, DisplayName = "Horizontal Scale", Default = 1)
// Vertical scaling factor
ESD_PARAMETER(yscale, Type = ft_int32_f16_t, DisplayName = "Vertical Scale", Default = 1)
// Horizontal offset
ESD_PARAMETER(xoffset, Type = ft_int32_f16_t, DisplayName = "X Offset", Default = 0)
// Vertical offset
ESD_PARAMETER(yoffset, Type = ft_int32_f16_t, DisplayName = "Y Offset", Default = 0)
// Width used as rendering boundary
ESD_PARAMETER(width, Type = ft_int16_t, DisplayName = "Render Width", Default = 200)
// Height used as rendering boundary
ESD_PARAMETER(height, Type = ft_int16_t, DisplayName = "Render Height", Default = 100)
ft_void_t Ft_Esd_Render_BitmapScaled(ft_int16_t x, ft_int16_t y, Ft_Esd_BitmapCell bitmapCell, ft_argb32_t c, ft_int32_f16_t xscale, ft_int32_f16_t yscale, ft_int32_f16_t xoffset, ft_int32_f16_t yoffset, ft_int16_t width, ft_int16_t height);

// Render bitmap using freeform rectangle within a specified global screen rectangle, freeform is relative to global
ft_void_t Ft_Esd_Render_BitmapFreeform(Ft_Esd_BitmapCell bitmapCell, ft_argb32_t c, Ft_Esd_Rect16 globalRect, Ft_Esd_Rect16 freeformRect, ft_uint8_t minAlpha);

//Render bitmap with rotation option
ft_void_t Ft_Esd_Render_BitmapRotate_Scaled(Ft_Esd_BitmapCell bitmapCell, ft_argb32_t c, Ft_Esd_Rect16 globalRect, ft_int32_t rotateAngle, ft_int32_f16_t xscale, ft_int32_f16_t yscale);
ft_void_t Ft_Esd_Render_BitmapRotate(Ft_Esd_BitmapCell bitmapCell, ft_argb32_t c, Ft_Esd_Rect16 globalRect, ft_int32_t rotateAngle);

// Basic rectangle Gradient rendering with fixed point precision
ESD_RENDER(Ft_Esd_Render_RectangleF_Gradient, Type = ft_void_t, DisplayName = "ESD Gradient Rectangle (Fixed Point)", Include = "Ft_Esd_Primitives.h", Category = EsdPrimitives)
ESD_PARAMETER(x, Type = ft_int32_f4_t, Default = 0)
ESD_PARAMETER(y, Type = ft_int32_f4_t, Default = 0)
ESD_PARAMETER(width, Type = ft_int32_f4_t, Default = 60)
ESD_PARAMETER(height, Type = ft_int32_f4_t, Default = 20)
ESD_PARAMETER(color1, Type = ft_argb32_t, DisplayName = "color1", Default = #FF3F3F3F)
ESD_PARAMETER(color2, Type = ft_argb32_t, DisplayName = "color2", Default = #FFFFFFFF)
ESD_PARAMETER(direction, Type = ft_int16_t, DisplayName = "direction", Default = 90)
ft_void_t Ft_Esd_Render_RectangleF_Gradient(ft_int32_f4_t x, ft_int32_f4_t y, ft_int32_f4_t w, ft_int32_f4_t h, ft_argb32_t color1, ft_argb32_t color2, ft_int16_t direction);

// Basic rectangle Gradient rendering
ESD_RENDER(Ft_Esd_Render_Rectangle_Gradient, Type = ft_void_t, DisplayName = "ESD Gradient Rectangle (Integer)", Include = "Ft_Esd_Primitives.h", Category = EsdPrimitives, Inline)
ESD_PARAMETER(x, Type = ft_int32_t, Default = 0)
ESD_PARAMETER(y, Type = ft_int32_t, Default = 0)
ESD_PARAMETER(width, Type = ft_int32_t, Default = 60)
ESD_PARAMETER(height, Type = ft_int32_t, Default = 20)
ESD_PARAMETER(color1, Type = ft_argb32_t, DisplayName = "color1", Default = #FF3F3F3F)
ESD_PARAMETER(color2, Type = ft_argb32_t, DisplayName = "color2", Default = #FFFFFFFF)
ESD_PARAMETER(direction, Type = ft_int16_t, DisplayName = "direction", Default = 90)
static inline ft_void_t Ft_Esd_Render_Rectangle_Gradient(ft_int32_t x, ft_int32_t y, ft_int32_f4_t w, ft_int32_f4_t h, ft_argb32_t color1, ft_argb32_t color2, ft_int16_t direction)
{
	Ft_Esd_Render_RectangleF_Gradient(x << 4, y << 4, w << 4, h << 4, color1, color2, direction);
}

// Basic rectangle rendering with fixed point precision
ESD_RENDER(Ft_Esd_Render_RectangleF, Type = ft_void_t, DisplayName = "ESD Rectangle (Fixed Point)", Include = "Ft_Esd_Primitives.h", Category = EsdPrimitives)
ESD_PARAMETER(x, Type = ft_int32_f4_t, Default = 0)
ESD_PARAMETER(y, Type = ft_int32_f4_t, Default = 0)
ESD_PARAMETER(width, Type = ft_int32_f4_t, Default = 60)
ESD_PARAMETER(height, Type = ft_int32_f4_t, Default = 20)
ESD_PARAMETER(radius, Type = ft_int32_f4_t, Default = 4)
ESD_PARAMETER(color, Type = ft_argb32_t, DisplayName = "Color", Default = #FFFFFFFF)
ft_void_t Ft_Esd_Render_RectangleF(ft_int32_f4_t x, ft_int32_f4_t y, ft_int32_f4_t w, ft_int32_f4_t h, ft_int32_f4_t radius, ft_argb32_t color);

// Basic rectangle rendering
ESD_RENDER(Ft_Esd_Render_Rectangle, Type = ft_void_t, DisplayName = "ESD Rectangle (Integer)", Include = "Ft_Esd_Primitives.h", Category = EsdPrimitives, Inline)
ESD_PARAMETER(x, Type = ft_int32_t, Default = 0)
ESD_PARAMETER(y, Type = ft_int32_t, Default = 0)
ESD_PARAMETER(width, Type = ft_int32_t, Default = 60)
ESD_PARAMETER(height, Type = ft_int32_t, Default = 20)
ESD_PARAMETER(radius, Type = ft_int32_t, Default = 4)
ESD_PARAMETER(color, Type = ft_argb32_t, DisplayName = "Color", Default = #FFFFFFFF)
static inline ft_void_t Ft_Esd_Render_Rectangle(ft_int32_t x, ft_int32_t y, ft_int32_t w, ft_int32_t h, ft_int32_t radius, ft_argb32_t color)
{
	Ft_Esd_Render_RectangleF(x << 4, y << 4, w << 4, h << 4, radius << 4, color);
}

// Basic line rendering with fixed point precision
ESD_RENDER(Ft_Esd_Render_LineF, Type = ft_void_t, DisplayName = "ESD Line (Fixed Point)", Include = "Ft_Esd_Primitives.h", Category = EsdPrimitives)
ESD_PARAMETER(x0, Type = ft_int32_f4_t, Default = 0)
ESD_PARAMETER(y0, Type = ft_int32_f4_t, Default = 0)
ESD_PARAMETER(x1, Type = ft_int32_f4_t, Default = 60)
ESD_PARAMETER(y1, Type = ft_int32_f4_t, Default = 20)
ESD_PARAMETER(width, Type = ft_int32_f3_t, Default = 2, EditRole = None)
ESD_PARAMETER(color, Type = ft_argb32_t, DisplayName = "Color", Default = #FFFFFFFF)
ft_void_t Ft_Esd_Render_LineF(ft_int32_f4_t x0, ft_int32_f4_t y0, ft_int32_f4_t x1, ft_int32_f4_t y1, ft_int32_f3_t width, ft_argb32_t color);

// Basic line rendering
ESD_RENDER(Ft_Esd_Render_Line, Type = ft_void_t, DisplayName = "ESD Line (Integer)", Include = "Ft_Esd_Primitives.h", Category = EsdPrimitives, Inline)
ESD_PARAMETER(x0, Type = ft_int32_t, Default = 0)
ESD_PARAMETER(y0, Type = ft_int32_t, Default = 0)
ESD_PARAMETER(x1, Type = ft_int32_t, Default = 60)
ESD_PARAMETER(y1, Type = ft_int32_t, Default = 20)
ESD_PARAMETER(width, Type = ft_int32_t, Default = 4, EditRole = None)
ESD_PARAMETER(color, Type = ft_argb32_t, DisplayName = "Color", Default = #FFFFFFFF)
static inline ft_void_t Ft_Esd_Render_Line(ft_int32_t x0, ft_int32_t y0, ft_int32_t x1, ft_int32_t y1, ft_int32_t width, ft_argb32_t color)
{
	Ft_Esd_Render_LineF(x0 << 4, y0 << 4, x1 << 4, y1 << 4, width << 3, color);
}

// Get scaled size
Ft_Esd_Size16 Ft_Esd_Primitive_GetScaledSize(Ft_Esd_Size16 boundary, Ft_Esd_Size16 original, ft_uint8_t scaling);

// Get alignment position
Ft_Esd_Rect16 Ft_Esd_Primitive_GetAlignedRect(Ft_Esd_Size16 boundary, Ft_Esd_Size16 size, ft_uint8_t align);

ESD_ENUM(Esd_Orientation, Type = ft_uint8_t)
#define ESD_ORIENTATION_HORIZONTAL (1)
#define ESD_ORIENTATION_VERTICAL (2)
ESD_END()

ESD_ENUM(Esd_Direction, Type = ft_uint8_t)
#define ESD_DIRECTION_NONE (0)
#define ESD_DIRECTION_HORIZONTAL (1)
#define ESD_DIRECTION_VERTICAL (2)
#define ESD_DIRECTION_BOTH (ESD_DIRECTION_HORIZONTAL | ESD_DIRECTION_VERTICAL)
ESD_END()

ESD_ENUM(Esd_AlignHorizontal, Type = ft_uint8_t)
#define ESD_ALIGN_LEFTX (0)
#define ESD_ALIGN_CENTERX (1)
#define ESD_ALIGN_RIGHTX (2)
#define ESD_ALIGN_FILLX (3)
ESD_END()

ESD_ENUM(Esd_AlignVertical, Type = ft_uint8_t)
#define ESD_ALIGN_TOPY (0)
#define ESD_ALIGN_CENTERY (4)
#define ESD_ALIGN_BOTTOMY (8)
#define ESD_ALIGN_FILLY (12)
ESD_END()

ESD_ENUM(Esd_Align, Type = ft_uint8_t)
#define ESD_ALIGN_TOPLEFT (ESD_ALIGN_TOPY | ESD_ALIGN_LEFTX)
#define ESD_ALIGN_TOPCENTER (ESD_ALIGN_TOPY | ESD_ALIGN_CENTERX)
#define ESD_ALIGN_TOPRIGHT (ESD_ALIGN_TOPY | ESD_ALIGN_RIGHTX)
#define ESD_ALIGN_TOPFILL (ESD_ALIGN_TOPY | ESD_ALIGN_FILLX)
#define ESD_ALIGN_CENTERLEFT (ESD_ALIGN_CENTERY | ESD_ALIGN_LEFTX)
#define ESD_ALIGN_CENTERCENTER (ESD_ALIGN_CENTERY | ESD_ALIGN_CENTERX)
#define ESD_ALIGN_CENTERRIGHT (ESD_ALIGN_CENTERY | ESD_ALIGN_RIGHTX)
#define ESD_ALIGN_CENTERFILL (ESD_ALIGN_CENTERY | ESD_ALIGN_FILLX)
#define ESD_ALIGN_BOTTOMLEFT (ESD_ALIGN_BOTTOMY | ESD_ALIGN_LEFTX)
#define ESD_ALIGN_BOTTOMCENTER (ESD_ALIGN_BOTTOMY | ESD_ALIGN_CENTERX)
#define ESD_ALIGN_BOTTOMRIGHT (ESD_ALIGN_BOTTOMY | ESD_ALIGN_RIGHTX)
#define ESD_ALIGN_BOTTOMFILL (ESD_ALIGN_BOTTOMY | ESD_ALIGN_FILLX)
#define ESD_ALIGN_FILLLEFT (ESD_ALIGN_FILLY | ESD_ALIGN_LEFTX)
#define ESD_ALIGN_FILLCENTER (ESD_ALIGN_FILLY | ESD_ALIGN_CENTERX)
#define ESD_ALIGN_FILLRIGHT (ESD_ALIGN_FILLY | ESD_ALIGN_RIGHTX)
#define ESD_ALIGN_FILLFILL (ESD_ALIGN_FILLY | ESD_ALIGN_FILLX)
ESD_END()

ESD_FUNCTION(ESD_ALIGN_HORIZONTAL, Type = Esd_AlignHorizontal, DisplayName = "Get Horizontal Align", Category = EsdLayoutUtilities, Macro)
ESD_PARAMETER(align, Type = Esd_Align)
#define ESD_ALIGN_HORIZONTAL(align) (align & 3)

ESD_FUNCTION(ESD_ALIGN_VERTICAL, Type = Esd_AlignVertical, DisplayName = "Get Vertical Align", Category = EsdLayoutUtilities, Macro)
ESD_PARAMETER(align, Type = Esd_Align)
#define ESD_ALIGN_VERTICAL(align) (align & 12)

ESD_ENUM(Esd_Scaling, Type = ft_uint8_t)
#define ESD_SCALING_ORIGINAL (0)
#define ESD_SCALING_FILL (1) // Keep Aspect ratio, extend outside rect
#define ESD_SCALING_FIT (2) // Keep Aspect ratio, stay inside rect
#define ESD_SCALING_STRETCH (3)
ESD_END()

ESD_ENUM(Esd_Stroke, Type = ft_uint8_t)
#define ESD_STROKE_NONE (0)
#define ESD_STROKE_INNER (1)
#define ESD_STROKE_OUTER (2)
#define ESD_STROKE_CENTER (3)
ESD_END()

#endif /* #ifndef FT_ESD_PRIMITIVES_H */

/* Nothing beyond this */
