/*
ESD Framework
Copyright (C) 2018  Bridgetek Pte Lte
Author: Jan Boon <jan.boon@kaetemi.be>
*/

/*
Font info structure
*/

#ifndef ESD_COMPATIBILITY_FONTINFO_H
#define ESD_COMPATIBILITY_FONTINFO_H

#include "ESD_Core.h"

#include "Ft_Esd.h"
#include "Ft_Esd_GpuAlloc.h"
#include "Ft_Esd_ResourceInfo.h"

// Structure providing information on how a font is loaded, as well as it's current loaded state
ESD_TYPE(Esd_FontInfo, Native = Struct)
ESD_TYPE(Esd_FontInfo *, Native = Pointer, Edit = Library)
#define Esd_FontInfo ESD_FontInfo
#define Esd_RomFontInfo ESD_RomFontInfo

#define Esd_LoadFont ESD_LoadFont

ESD_UPDATE(Esd_FontPersist, DisplayName = "Persist Font", Category = _GroupHidden, Macro)
ESD_PARAMETER(fontInfo, Type = Esd_FontInfo *)
#define Esd_FontPersist ESD_FontPersist

#endif /* #ifndef ESD_FONTINFO_H */

/* end of file */
