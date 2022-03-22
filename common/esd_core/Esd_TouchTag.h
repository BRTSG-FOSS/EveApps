
#ifndef ESD_TOUCHTAG__H
#define ESD_TOUCHTAG__H

#include "Esd_Base.h"

#ifdef __cplusplus
extern "C" {
#endif

// Utility for managing touch tag identifiers. Create one instance for each tag you need to handle
ESD_ACTOR(Esd_TouchTag, Callback, Include = "Esd_TouchTag.h", DisplayName = "Touch Tag", Category = EsdUtilities, Icon = ":/icons/hand-point-090.png")
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
	bool Set;

} Esd_TouchTag;

ESD_CORE_EXPORT void Esd_TouchTag__Initializer(Esd_TouchTag *context);

//
// Slots
//
ESD_SLOT(Start)
ESD_CORE_EXPORT void Esd_TouchTag_Start(Esd_TouchTag *context);

ESD_SLOT(Update)
ESD_CORE_EXPORT void Esd_TouchTag_Update(Esd_TouchTag *context);

ESD_SLOT(End)
ESD_CORE_EXPORT void Esd_TouchTag_End(Esd_TouchTag *context);

//
// Output
//

// Set while touching and touch started on this tag
ESD_OUTPUT(Touching, Type = bool)
ESD_CORE_EXPORT bool Esd_TouchTag_Touching(Esd_TouchTag *context);

// Set while touching, touch started on this tag, and touch is currently on this tag
ESD_OUTPUT(Inside, Type = bool)
ESD_CORE_EXPORT bool Esd_TouchTag_Inside(Esd_TouchTag *context);

// Set while touching and touch is currently on this tag, but the touch did not necessarily start on this tag (for example when drag and drop something to somewhere)
ESD_OUTPUT(Hover, Type = bool)
ESD_CORE_EXPORT bool Esd_TouchTag_Hover(Esd_TouchTag *context);

// The tag to render for this widget
ESD_OUTPUT(Tag, Type = uint8_t) // TODO: Move to direct variable output
ESD_CORE_EXPORT uint8_t Esd_TouchTag_Tag(Esd_TouchTag *context);

// Current tag, returns 0 when not touching, context may be NULL
ESD_OUTPUT(CurrentTag, DisplayName = "Current Tag", Type = uint8_t)
ESD_CORE_EXPORT uint8_t Esd_TouchTag_CurrentTag(Esd_TouchTag *context);

// Last touch X position, stays the same after touch has ended, context may be NULL
ESD_OUTPUT(TouchX, DisplayName = "Touch X", Type = int16_t)
ESD_CORE_EXPORT int16_t Esd_TouchTag_TouchX(Esd_TouchTag *context);

// Last touch Y position, context may be NULL
ESD_OUTPUT(TouchY, DisplayName = "Touch Y", Type = int16_t)
ESD_CORE_EXPORT int16_t Esd_TouchTag_TouchY(Esd_TouchTag *context);

// Touch X delta position since last update, context may be NULL
ESD_OUTPUT(TouchXDelta, DisplayName = "Touch X Delta", Type = int16_t)
ESD_CORE_EXPORT int16_t Esd_TouchTag_TouchXDelta(Esd_TouchTag *context);

// Touch Y delta position since last update, context may be NULL
ESD_OUTPUT(TouchYDelta, DisplayName = "Touch Y Delta", Type = int16_t)
ESD_CORE_EXPORT int16_t Esd_TouchTag_TouchYDelta(Esd_TouchTag *context);

// Suppress the current tag
ESD_FUNCTION(Esd_TouchTag_SuppressCurrentTags, Attributes = ESD_CORE_EXPORT, DisplayName = "Suppress Current Tags", Category = EsdUtilities)
ESD_CORE_EXPORT void Esd_TouchTag_SuppressCurrentTags();

// Current tag, returns 0 when not touching
ESD_FUNCTION(Esd_TouchTag_CurrentTag, Attributes = ESD_CORE_EXPORT, DisplayName = "Current Tag", Type = uint8_t, Category = EsdUtilities)
ESD_PARAMETER(context, Type = Esd_TouchTag *, Static)

// Last touch X position, stays the same after touch has ended
ESD_FUNCTION(Esd_TouchTag_TouchX, Attributes = ESD_CORE_EXPORT, DisplayName = "Touch X", Type = int16_t, Category = EsdUtilities)
ESD_PARAMETER(context, Type = Esd_TouchTag *, Static)

// Last touch Y position
ESD_FUNCTION(Esd_TouchTag_TouchY, Attributes = ESD_CORE_EXPORT, DisplayName = "Touch Y", Type = int16_t, Category = EsdUtilities)
ESD_PARAMETER(context, Type = Esd_TouchTag *, Static)

// Last touch X position, stays the same after touch has ended
ESD_FUNCTION(Esd_TouchTag_TouchXDelta, Attributes = ESD_CORE_EXPORT, DisplayName = "Touch X Delta", Type = int16_t, Category = EsdUtilities)
ESD_PARAMETER(context, Type = Esd_TouchTag *, Static)

// Last touch Y position
ESD_FUNCTION(Esd_TouchTag_TouchYDelta, Attributes = ESD_CORE_EXPORT, DisplayName = "Touch Y Delta", Type = int16_t, Category = EsdUtilities)
ESD_PARAMETER(context, Type = Esd_TouchTag *, Static)

ESD_TYPE(Esd_TouchTag *, Native = Pointer, Edit = None)

#ifdef __cplusplus
}
#endif

#endif /* Esd_TOUCHTAG__H */

/* end of file */
