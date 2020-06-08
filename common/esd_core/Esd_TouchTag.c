
#include "ESD_TouchTag.h"
#include <EVE_Hal.h>

#include "ESD_Context.h"

extern void ESD_noop(void *context);

extern ESD_CORE_EXPORT EVE_HalContext *ESD_Host;

static uint32_t s_LastTagFrame = ~0;
static ESD_TouchTag s_NullTag = {
	.Down = ESD_noop,
	.Up = ESD_noop,
	.Tap = ESD_noop,
	.Tag = 0,
	.Set = false
};
static bool s_SuppressCurrentTags = 0;
static uint8_t s_GpuRegTouchTag = 0;
static uint8_t s_TagDown = 0;
static uint8_t s_TagUp = 0;
typedef union
{
	uint32_t XY;
	struct
	{
		int16_t Y;
		int16_t X;
	};
} ESD_TouchPos_t;
static ESD_TouchPos_t s_TouchPos = { 0 };
int16_t s_TouchPosXDelta = 0;
int16_t s_TouchPosYDelta = 0;

static ESD_TouchTag *s_TagHandlers[256] = {
	[0] = &s_NullTag,
	[255] = &s_NullTag
};

void ESD_TouchTag__Initializer(ESD_TouchTag *context)
{
	*context = s_NullTag;
}

void ESD_TouchTag_Start(ESD_TouchTag *context)
{
	int i;

#ifdef ESD_SIMULATION
	if (s_LastTagFrame > ESD_CurrentContext->Frame && (s_LastTagFrame != ~0))
	{
		// Reset static
		s_LastTagFrame = ~0;
		for (int i = 1; i < 255; ++i)
		{
			s_TagHandlers[i] = NULL;
		}
		s_GpuRegTouchTag = 0;
		s_TagDown = 0;
		s_TagUp = 0;
		s_TouchPos.XY = 0;
		s_TouchPosXDelta = 0;
		s_TouchPosYDelta = 0;
	}
#endif

	if (context->Tag)
	{
		ESD_TouchTag_End(context);
	}

	// Allocate tag
	for (i = 1; i < 255; ++i)
	{
		if (!s_TagHandlers[i])
		{
			s_TagHandlers[i] = context;
			context->Tag = i;
			break;
		}
	}
}

void ESD_TouchTag_Update(ESD_TouchTag *context)
{
	EVE_HalContext *phost = ESD_Host;
	(void)phost;
	if (s_LastTagFrame != ESD_CurrentContext->Frame)
	{
		uint32_t regTouchXY;
		uint8_t regTouchTag;

		// Global tag update
		s_LastTagFrame = ESD_CurrentContext->Frame;

		// Read registers
		regTouchXY = EVE_Hal_rd32(ESD_Host, REG_TOUCH_TAG_XY);
		if (regTouchXY & 0x80008000)
		{
			// No touch
			regTouchTag = 0;
			s_TouchPosXDelta = 0;
			s_TouchPosYDelta = 0;
		}
		else
		{
			ESD_TouchPos_t prevPos;
			regTouchTag = EVE_Hal_rd8(ESD_Host, REG_TOUCH_TAG);
			if (!regTouchTag)
			{
				// Fallback when touching but touch tag 0 reported, stick to the last recorded tag
				// Happens when REG_TOUCH_TAG_XY is ahead of REG_TOUCH_TAG (tag is only detected after render), previous recorded tag will be 0 after no touch because of 0x80008000
				// Can also happen in case the user improperly draws a tag 0 somewhere, this causes issue when initial touch down occurs on a tag 0. Swiping over a tag 0 should cause no issue
				regTouchTag = s_GpuRegTouchTag;
			}
			if (regTouchTag && s_SuppressCurrentTags)
			{
				regTouchTag = 255;
			}
			prevPos = s_TouchPos;
			s_TouchPos.XY = regTouchXY;
			if (s_GpuRegTouchTag)
			{
				s_TouchPosXDelta = s_TouchPos.X - prevPos.X;
				s_TouchPosYDelta = s_TouchPos.Y - prevPos.Y;
			}
		}

		// Update up and down tags
		if (!s_TagDown)
		{
			s_TagDown = regTouchTag;
		}

		if (!regTouchTag)
		{
			s_TagUp = s_GpuRegTouchTag;
			s_TagDown = 0;
			s_SuppressCurrentTags = 0;
		}

		s_GpuRegTouchTag = regTouchTag;
	}

	if (!context)
		return;

	if (s_TagHandlers[context->Tag] != context)
	{
		// Tag was forced taken over by another widget (CMD_KEYS for example), need to get a new tag...
		context->Tag = 0;
		ESD_TouchTag_Start(context);
	}

	if (!context->Tag)
		return;

	// Tag is set
	if (context->Set)
	{
		// Tag unset
		if (s_TagDown != context->Tag)
		{
			context->Set = false;
			context->Up(context->Owner);
			if (s_TagUp == context->Tag)
			{
				// Down and up were the same, tap (TODO: Allow to supress tap when user was swiping)
				context->Tap(context->Owner);
			}
		}
	}
	else
	{
		// Tag set
		if (s_TagDown == context->Tag)
		{
			context->Set = true;
			context->Down(context->Owner);
		}
	}
}

void ESD_TouchTag_End(ESD_TouchTag *context)
{
	if (context->Set)
	{
		context->Set = false;
		// NOTE: Not necessarily a safe thing to call Up from End, but necessary when tags change
		context->Up(context->Owner);
	}
	if (context->Tag)
	{
		// Free tag
		if (s_TagHandlers[context->Tag] == context)
		{
			s_TagHandlers[context->Tag] = 0;
		}
		context->Tag = 0;
	}
}

bool ESD_TouchTag_Touching(ESD_TouchTag *context)
{
	return context->Tag && (s_TagDown == context->Tag);
}

bool ESD_TouchTag_Inside(ESD_TouchTag *context)
{
	return context->Tag && (s_TagDown == context->Tag) && (s_GpuRegTouchTag == context->Tag);
}

bool ESD_TouchTag_Hover(ESD_TouchTag *context)
{
	return context->Tag && (s_GpuRegTouchTag == context->Tag);
}

uint8_t ESD_TouchTag_Tag(ESD_TouchTag *context)
{
	if (context->Tag)
		return context->Tag;
	else
		return 255; // Always return non-tag value if no tag
}

uint8_t ESD_TouchTag_CurrentTag(ESD_TouchTag *context)
{
	return s_GpuRegTouchTag;
}

int16_t ESD_TouchTag_TouchX(ESD_TouchTag *context)
{
	return s_TouchPos.X;
}

int16_t ESD_TouchTag_TouchY(ESD_TouchTag *context)
{
	return s_TouchPos.Y;
}

int16_t ESD_TouchTag_TouchXDelta(ESD_TouchTag *context)
{
	return s_TouchPosXDelta;
}

int16_t ESD_TouchTag_TouchYDelta(ESD_TouchTag *context)
{
	return s_TouchPosYDelta;
}

void ESD_TouchTag_SuppressCurrentTags()
{
	s_SuppressCurrentTags = 1;
}

/* Nothing beyond this */
