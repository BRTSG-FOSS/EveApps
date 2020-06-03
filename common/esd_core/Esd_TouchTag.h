
#ifndef ESD_TOUCHTAG__H
#define ESD_TOUCHTAG__H

#include "Ft_Esd.h"

// Utility for managing touch tag identifiers. Create one instance for each tag you need to handle
ESD_ACTOR(ESD_TouchTag, Callback, Include = "ESD_TouchTag.h", DisplayName = "Touch Tag", Category = EsdUtilities, Icon = ":/icons/hand-point-090.png")
typedef struct
{
	// Callback
	void *Owner;

	//
	// Signals
	//

	// Called when the touch interaction is started on this tag.
	ESD_SIGNAL(Down)
	void (*Down)(void *context);

	// Up is called when the touch ends, even if the touch ended outside the tag. One Up is guaranteed to be called for every Down.
	ESD_SIGNAL(Up)
	void (*Up)(void *context);

	// Tap is called when both Down and Up events were called while on the tag.
	ESD_SIGNAL(Tap)
	void (*Tap)(void *context);

	//
	// Variables
	//
	int Tag;
	int Set;

} ESD_TouchTag;

void ESD_TouchTag__Initializer(ESD_TouchTag *context);

//
// Slots
//
ESD_SLOT(Start)
void ESD_TouchTag_Start(ESD_TouchTag *context);

ESD_SLOT(Update)
void ESD_TouchTag_Update(ESD_TouchTag *context);

ESD_SLOT(End)
void ESD_TouchTag_End(ESD_TouchTag *context);

//
// Output
//

// Set while touching and touch started on this tag
ESD_OUTPUT(Touching, Type = ft_bool_t)
ft_bool_t ESD_TouchTag_Touching(ESD_TouchTag *context);

// Set while touching, touch started on this tag, and touch is currently on this tag
ESD_OUTPUT(Inside, Type = ft_bool_t)
ft_bool_t ESD_TouchTag_Inside(ESD_TouchTag *context);

// Set while touching and touch is currently on this tag, but the touch did not necessarily start on this tag (for example when drag and drop something to somewhere)
ESD_OUTPUT(Hover, Type = ft_bool_t)
ft_bool_t ESD_TouchTag_Hover(ESD_TouchTag *context);

// The tag to render for this widget
ESD_OUTPUT(Tag, Type = ft_uint8_t) // TODO: Move to direct variable output
ft_uint8_t ESD_TouchTag_Tag(ESD_TouchTag *context);

// Current tag, returns 0 when not touching, context may be NULL
ESD_OUTPUT(CurrentTag, DisplayName = "Current Tag", Type = ft_uint8_t)
ft_uint8_t ESD_TouchTag_CurrentTag(ESD_TouchTag *context);

// Last touch X position, stays the same after touch has ended, context may be NULL
ESD_OUTPUT(TouchX, DisplayName = "Touch X", Type = ft_int16_t)
ft_int16_t ESD_TouchTag_TouchX(ESD_TouchTag *context);

// Last touch Y position, context may be NULL
ESD_OUTPUT(TouchY, DisplayName = "Touch Y", Type = ft_int16_t)
ft_int16_t ESD_TouchTag_TouchY(ESD_TouchTag *context);

// Touch X delta position since last update, context may be NULL
ESD_OUTPUT(TouchXDelta, DisplayName = "Touch X Delta", Type = ft_int16_t)
ft_int16_t ESD_TouchTag_TouchXDelta(ESD_TouchTag *context);

// Touch Y delta position since last update, context may be NULL
ESD_OUTPUT(TouchYDelta, DisplayName = "Touch Y Delta", Type = ft_int16_t)
ft_int16_t ESD_TouchTag_TouchYDelta(ESD_TouchTag *context);

// Suppress the current tag
ESD_FUNCTION(ESD_TouchTag_SuppressCurrentTags, DisplayName = "Suppress Current Tags", Category = EsdUtilities)
void ESD_TouchTag_SuppressCurrentTags();

// Current tag, returns 0 when not touching
ESD_FUNCTION(ESD_TouchTag_CurrentTag, DisplayName = "Current Tag", Type = ft_uint8_t, Category = EsdUtilities)
ESD_PARAMETER(context, Type = ESD_TouchTag *, Static)

// Last touch X position, stays the same after touch has ended
ESD_FUNCTION(ESD_TouchTag_TouchX, DisplayName = "Touch X", Type = ft_int16_t, Category = EsdUtilities)
ESD_PARAMETER(context, Type = ESD_TouchTag *, Static)

// Last touch Y position
ESD_FUNCTION(ESD_TouchTag_TouchY, DisplayName = "Touch Y", Type = ft_int16_t, Category = EsdUtilities)
ESD_PARAMETER(context, Type = ESD_TouchTag *, Static)

// Last touch X position, stays the same after touch has ended
ESD_FUNCTION(ESD_TouchTag_TouchXDelta, DisplayName = "Touch X Delta", Type = ft_int16_t, Category = EsdUtilities)
ESD_PARAMETER(context, Type = ESD_TouchTag *, Static)

// Last touch Y position
ESD_FUNCTION(ESD_TouchTag_TouchYDelta, DisplayName = "Touch Y Delta", Type = ft_int16_t, Category = EsdUtilities)
ESD_PARAMETER(context, Type = ESD_TouchTag *, Static)

ESD_TYPE(ESD_TouchTag *, Native = Pointer, Edit = None)

#endif /* ESD_TOUCHTAG_H */

/* Nothing beyond this */
