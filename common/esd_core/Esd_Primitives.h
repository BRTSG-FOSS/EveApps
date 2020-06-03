
#ifndef ESD_PRIMITIVES__H
#define ESD_PRIMITIVES__H

#include "ESD_TypeDefs.h"
#include "ESD_Dl.h"
#include "ESD_BitmapInfo.h"
#include "ESD_Math.h"

// ESD_Render_Rectangle
// ESD_Render_Line
// ESD_Render_Point
// ESD_Render_Bitmap
// ESD_Render_LineStrip
// ESD_Render_EdgeStrip

ESD_CATEGORY(EsdElements, DisplayName = "Elements", Category = EsdRenderable)
ESD_CATEGORY(EsdPrimitives, DisplayName = "Primitives", Category = EsdRenderable)

ESD_CATEGORY(EsdBasicWidegts, DisplayName = "Basics", Category = EsdWidgets)

// A function to combine RGB channel with 8 bit Alpha channel data into 32 bit integer data
ESD_FUNCTION(ESD_ColorARGB_Combine, Type = ft_argb32_t, DisplayName = "Color A+RGB Combine", Category = EsdUtilities, Inline)
ESD_PARAMETER(rgb, Type = ft_rgb32_t, Default = #FFFFFF)
ESD_PARAMETER(a, Type = uint8_t, Default = 255)
static inline ft_argb32_t ESD_ColorARGB_Combine(ft_rgb32_t rgb, uint8_t a)
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
ESD_RENDER(Esd_Render_MultiGradient, Type = void, DisplayName = "ESD Multi Gradient", Include = "ESD_Primitives.h", Category = EsdPrimitives)
ESD_PARAMETER(x, Type = int16_t, Default = 0)
ESD_PARAMETER(y, Type = int16_t, Default = 0)
ESD_PARAMETER(width, Type = int16_t, Default = 0)
ESD_PARAMETER(height, Type = int16_t, Default = 0)
ESD_PARAMETER(topLeft, Type = ft_argb32_t, DisplayName = "Top Left", Default = #FFFF0000)
ESD_PARAMETER(topRight, Type = ft_argb32_t, DisplayName = "Top Right", Default = #FF00FF00)
ESD_PARAMETER(bottomLeft, Type = ft_argb32_t, DisplayName = "Bottom Left", Default = #FF0000FF)
ESD_PARAMETER(bottomRight, Type = ft_argb32_t, DisplayName = "Bottom Right", Default = #FF000000)
void Esd_Render_MultiGradient(int16_t x, int16_t y, int16_t width, int16_t height, ft_argb32_t topLeft, ft_argb32_t topRight, ft_argb32_t bottomLeft, ft_argb32_t bottomRight);

// Rounded rectangular gradient with four colored corners
ESD_RENDER(Esd_Render_MultiGradient_Rounded, Type = void, DisplayName = "ESD Multi Gradient (Rounded)", Include = "ESD_Primitives.h", Category = EsdPrimitives)
ESD_PARAMETER(x, Type = int16_t, Default = 0)
ESD_PARAMETER(y, Type = int16_t, Default = 0)
ESD_PARAMETER(width, Type = int16_t, Default = 0)
ESD_PARAMETER(height, Type = int16_t, Default = 0)
ESD_PARAMETER(radius, Type = int32_f4_t, DisplayName = "Radius", Default = 4)
ESD_PARAMETER(alpha, Type = uint8_t, DisplayName = "Alpha", Default = 255)
ESD_PARAMETER(topLeft, Type = ft_rgb32_t, DisplayName = "Top Left", Default = #FF0012)
ESD_PARAMETER(topRight, Type = ft_rgb32_t, DisplayName = "Top Right", Default = #31FF00)
ESD_PARAMETER(bottomLeft, Type = ft_rgb32_t, DisplayName = "Bottom Left", Default = #6700FF)
ESD_PARAMETER(bottomRight, Type = ft_rgb32_t, DisplayName = "Bottom Right", Default = #00D1FF)
void Esd_Render_MultiGradient_Rounded(int16_t x, int16_t y, int16_t width, int16_t height, int32_f4_t radius, uint8_t alpha, ft_argb32_t topLeft, ft_argb32_t topRight, ft_argb32_t bottomLeft, ft_argb32_t bottomRight);

// Circle stroke
void Esd_Render_Circle_Stroke(
    int32_f4_t x, int32_f4_t y,
    int32_f4_t radius, int32_f4_t border,
    uint8_t stroke, ft_argb32_t color);

// Rect stroke
void Esd_Render_Rect_Stroke(
    int32_f4_t x, int32_f4_t y,
    int32_f4_t width, int32_f4_t height,
    int32_f4_t radius, int32_f4_t border,
    uint8_t stroke, ft_argb32_t color);

// Basic bitmap rendering
ESD_RENDER(ESD_Render_Bitmap, Type = void, DisplayName = "ESD Bitmap", Category = EsdPrimitives, Icon = ":/icons/image.png", Include = "ESD_Primitives.h")
ESD_PARAMETER(x, Type = int16_t, Default = 0)
ESD_PARAMETER(y, Type = int16_t, Default = 0)
ESD_PARAMETER(bitmapCell, Type = ESD_BitmapCell, DisplayName = "Bitmap Cell")
// Color to be multiplied with bitmap color, can be used for alpha fade for example
ESD_PARAMETER(c, Type = ft_argb32_t, DisplayName = "Color", Default = #FFFFFFFF)
void ESD_Render_Bitmap(int16_t x, int16_t y, ESD_BitmapCell bitmapCell, ft_argb32_t c);

// Scaled bitmap rendering
ESD_RENDER(ESD_Render_BitmapScaled, Type = void, DisplayName = "ESD Bitmap Scaled", Category = EsdPrimitives, Icon = ":/icons/image.png", Include = "ESD_Primitives.h")
ESD_PARAMETER(x, Type = int16_t, Default = 0)
ESD_PARAMETER(y, Type = int16_t, Default = 0)
ESD_PARAMETER(bitmapCell, Type = ESD_BitmapCell, DisplayName = "Bitmap Cell")
// Color to be multiplied with bitmap color, can be used for alpha fade for example
ESD_PARAMETER(c, Type = ft_argb32_t, DisplayName = "Color", Default = #FFFFFFFF)
// Horizontal scaling factor
ESD_PARAMETER(xscale, Type = int32_f16_t, DisplayName = "Horizontal Scale", Default = 1)
// Vertical scaling factor
ESD_PARAMETER(yscale, Type = int32_f16_t, DisplayName = "Vertical Scale", Default = 1)
// Horizontal offset
ESD_PARAMETER(xoffset, Type = int32_f16_t, DisplayName = "X Offset", Default = 0)
// Vertical offset
ESD_PARAMETER(yoffset, Type = int32_f16_t, DisplayName = "Y Offset", Default = 0)
// Width used as rendering boundary
ESD_PARAMETER(width, Type = int16_t, DisplayName = "Render Width", Default = 200)
// Height used as rendering boundary
ESD_PARAMETER(height, Type = int16_t, DisplayName = "Render Height", Default = 100)
void ESD_Render_BitmapScaled(int16_t x, int16_t y, ESD_BitmapCell bitmapCell, ft_argb32_t c, int32_f16_t xscale, int32_f16_t yscale, int32_f16_t xoffset, int32_f16_t yoffset, int16_t width, int16_t height);

// Render bitmap using freeform rectangle within a specified global screen rectangle, freeform is relative to global
void ESD_Render_BitmapFreeform(ESD_BitmapCell bitmapCell, ft_argb32_t c, ESD_Rect16 globalRect, ESD_Rect16 freeformRect, uint8_t minAlpha);

//Render bitmap with rotation option
void ESD_Render_BitmapRotate_Scaled(ESD_BitmapCell bitmapCell, ft_argb32_t c, ESD_Rect16 globalRect, int32_t rotateAngle, int32_f16_t xscale, int32_f16_t yscale);
void ESD_Render_BitmapRotate(ESD_BitmapCell bitmapCell, ft_argb32_t c, ESD_Rect16 globalRect, int32_t rotateAngle);

// Basic rectangle Gradient rendering with fixed point precision
ESD_RENDER(ESD_Render_RectangleF_Gradient, Type = void, DisplayName = "ESD Gradient Rectangle (Fixed Point)", Include = "ESD_Primitives.h", Category = EsdPrimitives)
ESD_PARAMETER(x, Type = int32_f4_t, Default = 0)
ESD_PARAMETER(y, Type = int32_f4_t, Default = 0)
ESD_PARAMETER(width, Type = int32_f4_t, Default = 60)
ESD_PARAMETER(height, Type = int32_f4_t, Default = 20)
ESD_PARAMETER(color1, Type = ft_argb32_t, DisplayName = "color1", Default = #FF3F3F3F)
ESD_PARAMETER(color2, Type = ft_argb32_t, DisplayName = "color2", Default = #FFFFFFFF)
ESD_PARAMETER(direction, Type = int16_t, DisplayName = "direction", Default = 90)
void ESD_Render_RectangleF_Gradient(int32_f4_t x, int32_f4_t y, int32_f4_t w, int32_f4_t h, ft_argb32_t color1, ft_argb32_t color2, int16_t direction);

// Basic rectangle Gradient rendering
ESD_RENDER(ESD_Render_Rectangle_Gradient, Type = void, DisplayName = "ESD Gradient Rectangle (Integer)", Include = "ESD_Primitives.h", Category = EsdPrimitives, Inline)
ESD_PARAMETER(x, Type = int32_t, Default = 0)
ESD_PARAMETER(y, Type = int32_t, Default = 0)
ESD_PARAMETER(width, Type = int32_t, Default = 60)
ESD_PARAMETER(height, Type = int32_t, Default = 20)
ESD_PARAMETER(color1, Type = ft_argb32_t, DisplayName = "color1", Default = #FF3F3F3F)
ESD_PARAMETER(color2, Type = ft_argb32_t, DisplayName = "color2", Default = #FFFFFFFF)
ESD_PARAMETER(direction, Type = int16_t, DisplayName = "direction", Default = 90)
static inline void ESD_Render_Rectangle_Gradient(int32_t x, int32_t y, int32_f4_t w, int32_f4_t h, ft_argb32_t color1, ft_argb32_t color2, int16_t direction)
{
	ESD_Render_RectangleF_Gradient(x << 4, y << 4, w << 4, h << 4, color1, color2, direction);
}

// Basic rectangle rendering with fixed point precision
ESD_RENDER(ESD_Render_RectangleF, Type = void, DisplayName = "ESD Rectangle (Fixed Point)", Include = "ESD_Primitives.h", Category = EsdPrimitives)
ESD_PARAMETER(x, Type = int32_f4_t, Default = 0)
ESD_PARAMETER(y, Type = int32_f4_t, Default = 0)
ESD_PARAMETER(width, Type = int32_f4_t, Default = 60)
ESD_PARAMETER(height, Type = int32_f4_t, Default = 20)
ESD_PARAMETER(radius, Type = int32_f4_t, Default = 4)
ESD_PARAMETER(color, Type = ft_argb32_t, DisplayName = "Color", Default = #FFFFFFFF)
void ESD_Render_RectangleF(int32_f4_t x, int32_f4_t y, int32_f4_t w, int32_f4_t h, int32_f4_t radius, ft_argb32_t color);

// Basic rectangle rendering
ESD_RENDER(ESD_Render_Rectangle, Type = void, DisplayName = "ESD Rectangle (Integer)", Include = "ESD_Primitives.h", Category = EsdPrimitives, Inline)
ESD_PARAMETER(x, Type = int32_t, Default = 0)
ESD_PARAMETER(y, Type = int32_t, Default = 0)
ESD_PARAMETER(width, Type = int32_t, Default = 60)
ESD_PARAMETER(height, Type = int32_t, Default = 20)
ESD_PARAMETER(radius, Type = int32_t, Default = 4)
ESD_PARAMETER(color, Type = ft_argb32_t, DisplayName = "Color", Default = #FFFFFFFF)
static inline void ESD_Render_Rectangle(int32_t x, int32_t y, int32_t w, int32_t h, int32_t radius, ft_argb32_t color)
{
	ESD_Render_RectangleF(x << 4, y << 4, w << 4, h << 4, radius << 4, color);
}

// Basic line rendering with fixed point precision
ESD_RENDER(ESD_Render_LineF, Type = void, DisplayName = "ESD Line (Fixed Point)", Include = "ESD_Primitives.h", Category = EsdPrimitives)
ESD_PARAMETER(x0, Type = int32_f4_t, Default = 0)
ESD_PARAMETER(y0, Type = int32_f4_t, Default = 0)
ESD_PARAMETER(x1, Type = int32_f4_t, Default = 60)
ESD_PARAMETER(y1, Type = int32_f4_t, Default = 20)
ESD_PARAMETER(width, Type = int32_f3_t, Default = 2, EditRole = None)
ESD_PARAMETER(color, Type = ft_argb32_t, DisplayName = "Color", Default = #FFFFFFFF)
void ESD_Render_LineF(int32_f4_t x0, int32_f4_t y0, int32_f4_t x1, int32_f4_t y1, int32_f3_t width, ft_argb32_t color);

// Basic line rendering
ESD_RENDER(ESD_Render_Line, Type = void, DisplayName = "ESD Line (Integer)", Include = "ESD_Primitives.h", Category = EsdPrimitives, Inline)
ESD_PARAMETER(x0, Type = int32_t, Default = 0)
ESD_PARAMETER(y0, Type = int32_t, Default = 0)
ESD_PARAMETER(x1, Type = int32_t, Default = 60)
ESD_PARAMETER(y1, Type = int32_t, Default = 20)
ESD_PARAMETER(width, Type = int32_t, Default = 4, EditRole = None)
ESD_PARAMETER(color, Type = ft_argb32_t, DisplayName = "Color", Default = #FFFFFFFF)
static inline void ESD_Render_Line(int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t width, ft_argb32_t color)
{
	ESD_Render_LineF(x0 << 4, y0 << 4, x1 << 4, y1 << 4, width << 3, color);
}

// Get scaled size
ESD_Size16 ESD_Primitive_GetScaledSize(ESD_Size16 boundary, ESD_Size16 original, uint8_t scaling);

// Get alignment position
ESD_Rect16 ESD_Primitive_GetAlignedRect(ESD_Size16 boundary, ESD_Size16 size, uint8_t align);

ESD_ENUM(Esd_Orientation, Type = uint8_t)
#define ESD_ORIENTATION_HORIZONTAL (1)
#define ESD_ORIENTATION_VERTICAL (2)
ESD_END()

ESD_ENUM(Esd_Direction, Type = uint8_t)
#define ESD_DIRECTION_NONE (0)
#define ESD_DIRECTION_HORIZONTAL (1)
#define ESD_DIRECTION_VERTICAL (2)
#define ESD_DIRECTION_BOTH (ESD_DIRECTION_HORIZONTAL | ESD_DIRECTION_VERTICAL)
ESD_END()

ESD_ENUM(Esd_AlignHorizontal, Type = uint8_t)
#define ESD_ALIGN_LEFTX (0)
#define ESD_ALIGN_CENTERX (1)
#define ESD_ALIGN_RIGHTX (2)
#define ESD_ALIGN_FILLX (3)
ESD_END()

ESD_ENUM(Esd_AlignVertical, Type = uint8_t)
#define ESD_ALIGN_TOPY (0)
#define ESD_ALIGN_CENTERY (4)
#define ESD_ALIGN_BOTTOMY (8)
#define ESD_ALIGN_FILLY (12)
ESD_END()

ESD_ENUM(Esd_Align, Type = uint8_t)
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

ESD_ENUM(Esd_Scaling, Type = uint8_t)
#define ESD_SCALING_ORIGINAL (0)
#define ESD_SCALING_FILL (1) // Keep Aspect ratio, extend outside rect
#define ESD_SCALING_FIT (2) // Keep Aspect ratio, stay inside rect
#define ESD_SCALING_STRETCH (3)
ESD_END()

ESD_ENUM(Esd_Stroke, Type = uint8_t)
#define ESD_STROKE_NONE (0)
#define ESD_STROKE_INNER (1)
#define ESD_STROKE_OUTER (2)
#define ESD_STROKE_CENTER (3)
ESD_END()

#endif /* #ifndef ESD_PRIMITIVES_H */

/* Nothing beyond this */
