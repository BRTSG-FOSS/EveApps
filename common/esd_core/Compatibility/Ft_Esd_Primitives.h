
#ifndef FT_ESD_COMPATIBILITY_PRIMITIVES_H
#define FT_ESD_COMPATIBILITY_PRIMITIVES_H

#include "Ft_Esd.h"
#include "Ft_Esd_Dl.h"
#include "Ft_Esd_BitmapInfo.h"
#include "Ft_Esd_Math.h"

ESD_FUNCTION(Ft_Esd_ColorARGB_Combine, Type = ft_argb32_t, DisplayName = "Color A+RGB Combine", Category = _GroupHidden, Macro)
ESD_PARAMETER(rgb, Type = ft_rgb32_t, Default = #FFFFFF)
ESD_PARAMETER(a, Type = ft_uint8_t, Default = 255)
#define Ft_Esd_ColorARGB_Combine ESD_COMPOSE_RGB_ALPHA

// Rectangular gradient with four colored corners
ESD_RENDER(Esd_Render_MultiGradient, Type = ft_void_t, DisplayName = "ESD Multi Gradient", Include = "FT_Esd_Primitives.h", Category = _GroupHidden, Macro)
ESD_PARAMETER(x, Type = ft_int16_t, Default = 0)
ESD_PARAMETER(y, Type = ft_int16_t, Default = 0)
ESD_PARAMETER(width, Type = ft_int16_t, Default = 0)
ESD_PARAMETER(height, Type = ft_int16_t, Default = 0)
ESD_PARAMETER(topLeft, Type = ft_argb32_t, DisplayName = "Top Left", Default = #FFFF0000)
ESD_PARAMETER(topRight, Type = ft_argb32_t, DisplayName = "Top Right", Default = #FF00FF00)
ESD_PARAMETER(bottomLeft, Type = ft_argb32_t, DisplayName = "Bottom Left", Default = #FF0000FF)
ESD_PARAMETER(bottomRight, Type = ft_argb32_t, DisplayName = "Bottom Right", Default = #FF000000)
#define Esd_Render_MultiGradient ESD_Render_MultiGradient

// Rounded rectangular gradient with four colored corners
ESD_RENDER(Esd_Render_MultiGradient_Rounded, Type = ft_void_t, DisplayName = "ESD Multi Gradient (Rounded)", Include = "FT_Esd_Primitives.h", Category = _GroupHidden, Macro)
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
#define Esd_Render_MultiGradient_Rounded ESD_Render_MultiGradient_Rounded

// Circle stroke
#define Esd_Render_Circle_Stroke ESD_Render_Circle_Stroke

// Rect stroke
#define Esd_Render_Rect_Stroke ESD_Render_Rect_Stroke

// Basic bitmap rendering
ESD_RENDER(Ft_Esd_Render_Bitmap, Type = ft_void_t, DisplayName = "ESD Bitmap", Category = _GroupHidden, Icon = ":/icons/image.png", Include = "FT_Esd_Primitives.h", Macro)
ESD_PARAMETER(x, Type = ft_int16_t, Default = 0)
ESD_PARAMETER(y, Type = ft_int16_t, Default = 0)
ESD_PARAMETER(bitmapCell, Type = Ft_Esd_BitmapCell, DisplayName = "Bitmap Cell")
// Color to be multiplied with bitmap color, can be used for alpha fade for example
ESD_PARAMETER(c, Type = ft_argb32_t, DisplayName = "Color", Default = #FFFFFFFF)
#define Ft_Esd_Render_Bitmap ESD_Render_Bitmap

// Scaled bitmap rendering
ESD_RENDER(Ft_Esd_Render_BitmapScaled, Type = ft_void_t, DisplayName = "ESD Bitmap Scaled", Category = _GroupHidden, Icon = ":/icons/image.png", Include = "FT_Esd_Primitives.h", Macro)
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
#define Ft_Esd_Render_BitmapScaled ESD_Render_Bitmap_Scaled

// Render bitmap using freeform rectangle within a specified global screen rectangle, freeform is relative to global
#define Ft_Esd_Render_BitmapFreeform ESD_Render_Bitmap_Freeform

//Render bitmap with rotation option
#define Ft_Esd_Render_BitmapRotate_Scaled ESD_Render_Bitmap_RotateScaled
#define Ft_Esd_Render_BitmapRotate ESD_Render_Bitmap_Rotate

// Basic rectangle Gradient rendering with fixed point precision
/*
Deprecate because it uses expensive SQRT at runtime

ESD_RENDER(Ft_Esd_Render_RectangleF_Gradient, Type = ft_void_t, DisplayName = "ESD Gradient Rectangle (Fixed Point)", Include = "FT_Esd_Primitives.h", Category = _GroupHidden, Macro)
ESD_PARAMETER(x, Type = ft_int32_f4_t, Default = 0)
ESD_PARAMETER(y, Type = ft_int32_f4_t, Default = 0)
ESD_PARAMETER(width, Type = ft_int32_f4_t, Default = 60)
ESD_PARAMETER(height, Type = ft_int32_f4_t, Default = 20)
ESD_PARAMETER(color1, Type = ft_argb32_t, DisplayName = "color1", Default = #FF3F3F3F)
ESD_PARAMETER(color2, Type = ft_argb32_t, DisplayName = "color2", Default = #FFFFFFFF)
ESD_PARAMETER(direction, Type = ft_int16_t, DisplayName = "direction", Default = 90)
#define Ft_Esd_Render_RectangleF_Gradient
*/

/*
Deprecate because it uses expensive SQRT at runtime

ESD_RENDER(Ft_Esd_Render_Rectangle_Gradient, Type = ft_void_t, DisplayName = "ESD Gradient Rectangle (Integer)", Include = "FT_Esd_Primitives.h", Category = _GroupHidden, Macro)
ESD_PARAMETER(x, Type = ft_int32_t, Default = 0)
ESD_PARAMETER(y, Type = ft_int32_t, Default = 0)
ESD_PARAMETER(width, Type = ft_int32_t, Default = 60)
ESD_PARAMETER(height, Type = ft_int32_t, Default = 20)
ESD_PARAMETER(color1, Type = ft_argb32_t, DisplayName = "color1", Default = #FF3F3F3F)
ESD_PARAMETER(color2, Type = ft_argb32_t, DisplayName = "color2", Default = #FFFFFFFF)
ESD_PARAMETER(direction, Type = ft_int16_t, DisplayName = "direction", Default = 90)
#define Ft_Esd_Render_Rectangle_Gradient
*/

ESD_RENDER(Ft_Esd_Render_RectangleF, Type = ft_void_t, DisplayName = "ESD Rectangle (Fixed Point)", Include = "FT_Esd_Primitives.h", Category = _GroupHidden, Macro)
ESD_PARAMETER(x, Type = ft_int32_f4_t, Default = 0)
ESD_PARAMETER(y, Type = ft_int32_f4_t, Default = 0)
ESD_PARAMETER(width, Type = ft_int32_f4_t, Default = 60)
ESD_PARAMETER(height, Type = ft_int32_f4_t, Default = 20)
ESD_PARAMETER(radius, Type = ft_int32_f4_t, Default = 4)
ESD_PARAMETER(color, Type = ft_argb32_t, DisplayName = "Color", Default = #FFFFFFFF)
#define Ft_Esd_Render_RectangleF ESD_Render_RectF

// Basic rectangle rendering
ESD_RENDER(Ft_Esd_Render_Rectangle, Type = ft_void_t, DisplayName = "ESD Rectangle (Integer)", Include = "FT_Esd_Primitives.h", Category = _GroupHidden, Macro)
ESD_PARAMETER(x, Type = ft_int32_t, Default = 0)
ESD_PARAMETER(y, Type = ft_int32_t, Default = 0)
ESD_PARAMETER(width, Type = ft_int32_t, Default = 60)
ESD_PARAMETER(height, Type = ft_int32_t, Default = 20)
ESD_PARAMETER(radius, Type = ft_int32_t, Default = 4)
ESD_PARAMETER(color, Type = ft_argb32_t, DisplayName = "Color", Default = #FFFFFFFF)
#define Ft_Esd_Render_Rectangle ESD_Render_Rect

// Basic line rendering with fixed point precision
ESD_RENDER(Ft_Esd_Render_LineF, Type = ft_void_t, DisplayName = "ESD Line (Fixed Point)", Include = "FT_Esd_Primitives.h", Category = _GroupHidden, Macro)
ESD_PARAMETER(x0, Type = ft_int32_f4_t, Default = 0)
ESD_PARAMETER(y0, Type = ft_int32_f4_t, Default = 0)
ESD_PARAMETER(x1, Type = ft_int32_f4_t, Default = 60)
ESD_PARAMETER(y1, Type = ft_int32_f4_t, Default = 20)
ESD_PARAMETER(width, Type = ft_int32_f3_t, Default = 2, EditRole = None)
ESD_PARAMETER(color, Type = ft_argb32_t, DisplayName = "Color", Default = #FFFFFFFF)
#define Ft_Esd_Render_LineF ESD_Render_LineF

// Basic line rendering
ESD_RENDER(Ft_Esd_Render_Line, Type = ft_void_t, DisplayName = "ESD Line (Integer)", Include = "FT_Esd_Primitives.h", Category = _GroupHidden, Macro)
ESD_PARAMETER(x0, Type = ft_int32_t, Default = 0)
ESD_PARAMETER(y0, Type = ft_int32_t, Default = 0)
ESD_PARAMETER(x1, Type = ft_int32_t, Default = 60)
ESD_PARAMETER(y1, Type = ft_int32_t, Default = 20)
ESD_PARAMETER(width, Type = ft_int32_t, Default = 4, EditRole = None)
ESD_PARAMETER(color, Type = ft_argb32_t, DisplayName = "Color", Default = #FFFFFFFF)
#define Ft_Esd_Render_Line ESD_Render_Line

// Get scaled size
#define Ft_Esd_Primitive_GetScaledSize ESD_Math_GetScaledSize

// Get alignment position
#define Ft_Esd_Primitive_GetAlignedRect ESD_Math_GetAlignedRect

#endif /* #ifndef FT_ESD_PRIMITIVES_H */

/* Nothing beyond this */
