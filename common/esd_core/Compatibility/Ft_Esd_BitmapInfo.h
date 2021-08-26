/*
Bitmap info structure
*/

#ifndef FT_ESD_COMPATIBILITY_BITMAPINFO_H
#define FT_ESD_COMPATIBILITY_BITMAPINFO_H

#include "Esd_Core.h"

#include "Ft_Esd.h"
#include "Ft_Esd_GpuAlloc.h"
#include "Ft_Esd_ResourceInfo.h"

ESD_TYPE(Ft_Esd_BitmapInfo, Native = Struct) // TODO: Struct support, expose values
ESD_TYPE(Ft_Esd_BitmapInfo *, Native = Pointer, Edit = Library)
#define Ft_Esd_BitmapInfo Esd_BitmapInfo

ESD_TYPE(Ft_Esd_BitmapCell, Native = Struct, Edit = Library) // TODO: Struct support, expose values
ESD_TYPE(Ft_Esd_BitmapCell *, Native = Pointer, Edit = Library)
#define Ft_Esd_BitmapCell Esd_BitmapCell

/// A function to load bitmap data(not including palette data) into RAM_G
ESD_FUNCTION(Ft_Esd_LoadBitmap, Type = ft_uint32_t, Include = "Ft_Esd_BitmapInfo.h", DisplayName = "Load Bitmap to RAM_G", Category = _GroupHidden, Macro)
ESD_PARAMETER(bitmapInfo, Type = Ft_Esd_BitmapInfo *)
#define Ft_Esd_LoadBitmap Esd_LoadBitmap

/// A function to load palette data of bitmap into RAM_G
ESD_FUNCTION(Ft_Esd_LoadPalette, Type = ft_uint32_t, Include = "Ft_Esd_BitmapInfo.h", DisplayName = "Load Palette to RAM_G", Category = _GroupHidden, Macro)
ESD_PARAMETER(bitmapInfo, Type = Ft_Esd_BitmapInfo *)
#define Ft_Esd_LoadPalette Esd_LoadPalette

///  Switch bitmap cell number
ESD_FUNCTION(Ft_Esd_SwitchBitmapCell, Type = Ft_Esd_BitmapCell, DisplayName = "Switch Bitmap Cell", Category = _GroupHidden, Macro)
ESD_PARAMETER(BitmapInfo, Type = Ft_Esd_BitmapCell, DisplayName = "Bitmap Cell")
ESD_PARAMETER(Cell, Type = ft_uint16_t, DisplayName = "Cell")
#define Ft_Esd_SwitchBitmapCell Esd_BitmapCell_Switched

/// A function to make bitmap persistent in memory by reloading the data if necessary, called during the Update cycle of each frame
ESD_UPDATE(Ft_Esd_BitmapPersist, DisplayName = "Persist Bitmap", Category = _GroupHidden, Macro)
ESD_PARAMETER(bitmapCell, Type = Ft_Esd_BitmapCell)
#define Ft_Esd_BitmapPersist Esd_BitmapCell_Persist

/// A function to get bitmap information structure from bitmap cell
ESD_FUNCTION(Ft_Esd_BitmapCell_GetInfo, Type = Ft_Esd_BitmapInfo *, DisplayName = "ESD BitmapCell GetInfo", Category = _GroupHidden, Macro)
ESD_PARAMETER(bitmapCell, Type = Ft_Esd_BitmapCell)
#define Ft_Esd_BitmapCell_GetInfo Esd_BitmapCell_GetInfo

/// A function to get the bitmap width from bitmap information. Returns width of 1 pixel for NULL bitmapInfo
ESD_FUNCTION(Ft_Esd_BitmapInfo_GetWidth, Type = ft_int32_t, DisplayName = "ESD BitmapInfo GetWidth", Category = _GroupHidden, Macro)
ESD_PARAMETER(bitmapInfo, Type = Ft_Esd_BitmapInfo *)
#define Ft_Esd_BitmapInfo_GetWidth Esd_BitmapInfo_GetWidth

/// A function to get the bitmap height from bitmap information. Returns height of 1 pixel for NULL bitmapInfo
ESD_FUNCTION(Ft_Esd_BitmapInfo_GetHeight, Type = ft_int32_t, DisplayName = "ESD BitmapInfo GetHeight", Category = _GroupHidden, Macro)
ESD_PARAMETER(bitmapInfo, Type = Ft_Esd_BitmapInfo *)
#define Ft_Esd_BitmapInfo_GetHeight Esd_BitmapInfo_GetHeight

#endif /* #ifndef FT_ESD_BITMAPINFO_H */

/* end of file */
