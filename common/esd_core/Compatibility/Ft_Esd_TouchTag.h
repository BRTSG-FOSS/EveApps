
#ifndef FT_ESD_COMPATIBILITY_TOUCHTAG_H
#define FT_ESD_COMPATIBILITY_TOUCHTAG_H

#include "Esd_Core.h"
#include "Ft_Esd.h"

// Utility for managing touch tag identifiers. Create one instance for each tag you need to handle
ESD_ACTOR(Ft_Esd_TouchTag, Callback, Include = "Ft_Esd_TouchTag.h", DisplayName = "Touch Tag", Category = _GroupHidden, Icon = ":/icons/hand-point-090.png")
#define Ft_Esd_TouchTag Esd_TouchTag

#define Ft_Esd_TouchTag__Initializer Esd_TouchTag__Initializer
#define Ft_Esd_TouchTag_Start Esd_TouchTag_Start
#define Ft_Esd_TouchTag_Update Esd_TouchTag_Update
#define Ft_Esd_TouchTag_End Esd_TouchTag_End
#define Ft_Esd_TouchTag_Touching Esd_TouchTag_Touching
#define Ft_Esd_TouchTag_Inside Esd_TouchTag_Inside
#define Ft_Esd_TouchTag_Hover Esd_TouchTag_Hover
#define Ft_Esd_TouchTag_Tag Esd_TouchTag_Tag
#define Ft_Esd_TouchTag_CurrentTag Esd_TouchTag_CurrentTag
#define Ft_Esd_TouchTag_TouchX Esd_TouchTag_TouchX
#define Ft_Esd_TouchTag_TouchY Esd_TouchTag_TouchY
#define Ft_Esd_TouchTag_TouchXDelta Esd_TouchTag_TouchXDelta
#define Ft_Esd_TouchTag_TouchYDelta Esd_TouchTag_TouchYDelta
#define Ft_Esd_TouchTag_SuppressCurrentTags Esd_TouchTag_SuppressCurrentTags

ESD_SIGNAL(Down)
ESD_SIGNAL(Up)
ESD_SIGNAL(Tap)

ESD_SLOT(Start)
ESD_SLOT(Update)
ESD_SLOT(End)

ESD_OUTPUT(Touching, Type = ft_bool_t)
ESD_OUTPUT(Inside, Type = ft_bool_t)
ESD_OUTPUT(Hover, Type = ft_bool_t)
ESD_OUTPUT(Tag, Type = ft_uint8_t) // TODO: Move to direct variable output
ESD_OUTPUT(CurrentTag, DisplayName = "Current Tag", Type = ft_uint8_t)
ESD_OUTPUT(TouchX, DisplayName = "Touch X", Type = ft_int16_t)
ESD_OUTPUT(TouchY, DisplayName = "Touch Y", Type = ft_int16_t)
ESD_OUTPUT(TouchXDelta, DisplayName = "Touch X Delta", Type = ft_int16_t)
ESD_OUTPUT(TouchYDelta, DisplayName = "Touch Y Delta", Type = ft_int16_t)

// Suppress the current tag
ESD_FUNCTION(Ft_Esd_TouchTag_SuppressCurrentTags, DisplayName = "Suppress Current Tags", Category = _GroupHidden, Macro)

// Current tag, returns 0 when not touching
ESD_FUNCTION(Ft_Esd_TouchTag_CurrentTag, DisplayName = "Current Tag", Type = ft_uint8_t, Category = _GroupHidden, Macro)
ESD_PARAMETER(context, Type = Ft_Esd_TouchTag *, Static)

// Last touch X position, stays the same after touch has ended
ESD_FUNCTION(Ft_Esd_TouchTag_TouchX, DisplayName = "Touch X", Type = ft_int16_t, Category = _GroupHidden, Macro)
ESD_PARAMETER(context, Type = Ft_Esd_TouchTag *, Static)

// Last touch Y position
ESD_FUNCTION(Ft_Esd_TouchTag_TouchY, DisplayName = "Touch Y", Type = ft_int16_t, Category = _GroupHidden, Macro)
ESD_PARAMETER(context, Type = Ft_Esd_TouchTag *, Static)

// Last touch X position, stays the same after touch has ended
ESD_FUNCTION(Ft_Esd_TouchTag_TouchXDelta, DisplayName = "Touch X Delta", Type = ft_int16_t, Category = _GroupHidden, Macro)
ESD_PARAMETER(context, Type = Ft_Esd_TouchTag *, Static)

// Last touch Y position
ESD_FUNCTION(Ft_Esd_TouchTag_TouchYDelta, DisplayName = "Touch Y Delta", Type = ft_int16_t, Category = _GroupHidden, Macro)
ESD_PARAMETER(context, Type = Ft_Esd_TouchTag *, Static)

ESD_TYPE(Ft_Esd_TouchTag *, Native = Pointer, Edit = None)

#endif /* FT_ESD_TOUCHTAG_H */

/* Nothing beyond this */
