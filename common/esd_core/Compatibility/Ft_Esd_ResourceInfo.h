/*
ESD Framework
Copyright (C) 2018  Bridgetek Pte Lte
Author: Jan Boon <jan.boon@kaetemi.be>
*/

/*
Resource info structure
*/

#ifndef ESD_COMPATIBILITY_RESOURCEINFO_H
#define ESD_COMPATIBILITY_RESOURCEINFO_H

#include "ESD_Core.h"
#include "Ft_Esd.h"
#include "Ft_Esd_GpuAlloc.h"

ESD_TYPE(Esd_ResourceInfo, Native = Struct)
ESD_TYPE(Esd_ResourceInfo *, Native = Pointer, Edit = Library)
#define Esd_ResourceInfo ESD_ResourceInfo

#define Esd_LoadResource ESD_LoadResource
#define Esd_FreeResource ESD_FreeResource

/// A function to make fonts persistent in memory by reloading the data if necessary, called during the Update cycle of each frame
ESD_UPDATE(Esd_ResourcePersist, DisplayName = "Persist Resource", Category = _GroupHidden, Macro)
ESD_PARAMETER(resourceInfo, Type = Esd_ResourceInfo *)
#define Esd_ResourcePersist ESD_ResourcePersist

/// Number of available bitmap handles
#define FT_ESD_BITMAPHANDLE_NB ESD_BITMAPHANDLE_NB
#define FT_ESD_BITMAPHANDLE_CAP ESD_BITMAPHANDLE_CAP
#define FT_ESD_BITMAPHANDLE_INVALID ESD_BITMAPHANDLE_INVALID
#define FT_ESD_BITMAPHANDLE_VALID ESD_BITMAPHANDLE_VALID

#endif /* #ifndef ESD_FONTINFO_H */

/* end of file */
