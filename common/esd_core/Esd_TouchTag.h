
#ifndef ESD_TOUCHTAG__H
#define ESD_TOUCHTAG__H

#include "ESD_Base.h"

// Utility for managing touch tag identifiers. Create one instance for each tag you need to handle
#pragma ESD_ACTOR(ESD_TouchTag, Callback, Include = "ESD_TouchTag.h", DisplayName = "Touch Tag", Category = EsdUtilities, Icon = ":/icons/hand-point-090.png")
typedef struct
{
	// Callback
	void *Owner;

	//
	// Signals
	//

	// Called when the touch interaction is started on this tag.
#pragma ESD_SIGNAL(Down)
	void (*Down)(void *context);

	// Up is called when the touch ends, even if the touch ended outside the tag. One Up is guaranteed to be called for every Down.
#pragma ESD_SIGNAL(Up)
	void (*Up)(void *context);

	// Tap is called when both Down and Up events were called while on the tag.
#pragma ESD_SIGNAL(Tap)
	void (*Tap)(void *context);

	//
	// Variables
	//
	int Tag;
	bool Set;

} ESD_TouchTag;

ESD_CORE_EXPORT void ESD_TouchTag__Initializer(ESD_TouchTag *context);

//
// Slots
//
#pragma ESD_SLOT(Start)
ESD_CORE_EXPORT void ESD_TouchTag_Start(ESD_TouchTag *context);

#pragma ESD_SLOT(Update)
ESD_CORE_EXPORT void ESD_TouchTag_Update(ESD_TouchTag *context);

#pragma ESD_SLOT(End)
ESD_CORE_EXPORT void ESD_TouchTag_End(ESD_TouchTag *context);

//
// Output
//

// Set while touching and touch started on this tag
#pragma ESD_OUTPUT(Touching, Type = bool)
ESD_CORE_EXPORT bool ESD_TouchTag_Touching(ESD_TouchTag *context);

// Set while touching, touch started on this tag, and touch is currently on this tag
#pragma ESD_OUTPUT(Inside, Type = bool)
ESD_CORE_EXPORT bool ESD_TouchTag_Inside(ESD_TouchTag *context);

// Set while touching and touch is currently on this tag, but the touch did not necessarily start on this tag (for example when drag and drop something to somewhere)
#pragma ESD_OUTPUT(Hover, Type = bool)
ESD_CORE_EXPORT bool ESD_TouchTag_Hover(ESD_TouchTag *context);

// The tag to render for this widget
#pragma ESD_OUTPUT(Tag, Type = uint8_t) // TODO: Move to direct variable output
ESD_CORE_EXPORT uint8_t ESD_TouchTag_Tag(ESD_TouchTag *context);

// Current tag, returns 0 when not touching, context may be NULL
#pragma ESD_OUTPUT(CurrentTag, DisplayName = "Current Tag", Type = uint8_t)
ESD_CORE_EXPORT uint8_t ESD_TouchTag_CurrentTag(ESD_TouchTag *context);

// Last touch X position, stays the same after touch has ended, context may be NULL
#pragma ESD_OUTPUT(TouchX, DisplayName = "Touch X", Type = int16_t)
ESD_CORE_EXPORT int16_t ESD_TouchTag_TouchX(ESD_TouchTag *context);

// Last touch Y position, context may be NULL
#pragma ESD_OUTPUT(TouchY, DisplayName = "Touch Y", Type = int16_t)
ESD_CORE_EXPORT int16_t ESD_TouchTag_TouchY(ESD_TouchTag *context);

// Touch X delta position since last update, context may be NULL
#pragma ESD_OUTPUT(TouchXDelta, DisplayName = "Touch X Delta", Type = int16_t)
ESD_CORE_EXPORT int16_t ESD_TouchTag_TouchXDelta(ESD_TouchTag *context);

// Touch Y delta position since last update, context may be NULL
#pragma ESD_OUTPUT(TouchYDelta, DisplayName = "Touch Y Delta", Type = int16_t)
ESD_CORE_EXPORT int16_t ESD_TouchTag_TouchYDelta(ESD_TouchTag *context);

// Suppress the current tag
#pragma ESD_FUNCTION(ESD_TouchTag_SuppressCurrentTags, Attributes = ESD_CORE_EXPORT, DisplayName = "Suppress Current Tags", Category = EsdUtilities)
ESD_CORE_EXPORT void ESD_TouchTag_SuppressCurrentTags();

// Current tag, returns 0 when not touching
#pragma ESD_FUNCTION(ESD_TouchTag_CurrentTag, Attributes = ESD_CORE_EXPORT, DisplayName = "Current Tag", Type = uint8_t, Category = EsdUtilities)
ESD_PARAMETER(context, Type = ESD_TouchTag *, Static)

// Last touch X position, stays the same after touch has ended
#pragma ESD_FUNCTION(ESD_TouchTag_TouchX, Attributes = ESD_CORE_EXPORT, DisplayName = "Touch X", Type = int16_t, Category = EsdUtilities)
ESD_PARAMETER(context, Type = ESD_TouchTag *, Static)

// Last touch Y position
#pragma ESD_FUNCTION(ESD_TouchTag_TouchY, Attributes = ESD_CORE_EXPORT, DisplayName = "Touch Y", Type = int16_t, Category = EsdUtilities)
ESD_PARAMETER(context, Type = ESD_TouchTag *, Static)

// Last touch X position, stays the same after touch has ended
#pragma ESD_FUNCTION(ESD_TouchTag_TouchXDelta, Attributes = ESD_CORE_EXPORT, DisplayName = "Touch X Delta", Type = int16_t, Category = EsdUtilities)
ESD_PARAMETER(context, Type = ESD_TouchTag *, Static)

// Last touch Y position
#pragma ESD_FUNCTION(ESD_TouchTag_TouchYDelta, Attributes = ESD_CORE_EXPORT, DisplayName = "Touch Y Delta", Type = int16_t, Category = EsdUtilities)
ESD_PARAMETER(context, Type = ESD_TouchTag *, Static)

#pragma ESD_TYPE(ESD_TouchTag *, Native = Pointer, Edit = None)

#endif /* ESD_TOUCHTAG__H */

/* Nothing beyond this */
