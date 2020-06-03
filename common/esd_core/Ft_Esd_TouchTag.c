
#include "Ft_Esd_TouchTag.h"
#include "Ft_Gpu_Hal.h"

#include "Ft_Esd_Core.h"

extern void Ft_Esd_Noop(void *context);

extern EVE_HalContext *Ft_Esd_Host;

static ft_uint32_t s_LastTagFrame = ~0;
static Ft_Esd_TouchTag s_NullTag = {
	.Down = Ft_Esd_Noop,
	.Up = Ft_Esd_Noop,
	.Tap = Ft_Esd_Noop,
	.Tag = 0,
	.Set = FT_FALSE
};
static ft_bool_t s_SuppressCurrentTags = 0;
static ft_uint8_t s_GpuRegTouchTag = 0;
static ft_uint8_t s_TagDown = 0;
static ft_uint8_t s_TagUp = 0;
typedef union
{
	ft_uint32_t XY;
	struct
	{
		ft_int16_t Y;
		ft_int16_t X;
	};
} Ft_Esd_TouchPos_t;
static Ft_Esd_TouchPos_t s_TouchPos = { 0 };
ft_int16_t s_TouchPosXDelta = 0;
ft_int16_t s_TouchPosYDelta = 0;

static Ft_Esd_TouchTag *s_TagHandlers[256] = {
	[0] = &s_NullTag,
	[255] = &s_NullTag
};

void Ft_Esd_TouchTag__Initializer(Ft_Esd_TouchTag *context)
{
	*context = s_NullTag;
}

void Ft_Esd_TouchTag_Start(Ft_Esd_TouchTag *context)
{
	int i;

#ifdef ESD_SIMULATION
	if (s_LastTagFrame > Esd_CurrentContext->Frame && (s_LastTagFrame != ~0))
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
		Ft_Esd_TouchTag_End(context);
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

void Ft_Esd_TouchTag_Update(Ft_Esd_TouchTag *context)
{
	EVE_HalContext *phost = Ft_Esd_Host;
	(void)phost;
	if (s_LastTagFrame != Esd_CurrentContext->Frame)
	{
		ft_uint32_t regTouchXY;
		ft_uint8_t regTouchTag;

		// Global tag update
		s_LastTagFrame = Esd_CurrentContext->Frame;

		// Read registers
		regTouchXY = Ft_Gpu_Hal_Rd32(Ft_Esd_Host, REG_TOUCH_TAG_XY);
		if (regTouchXY & 0x80008000)
		{
			// No touch
			regTouchTag = 0;
			s_TouchPosXDelta = 0;
			s_TouchPosYDelta = 0;
		}
		else
		{
			Ft_Esd_TouchPos_t prevPos;
			regTouchTag = Ft_Gpu_Hal_Rd8(Ft_Esd_Host, REG_TOUCH_TAG);
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
		Ft_Esd_TouchTag_Start(context);
	}

	if (!context->Tag)
		return;

	// Tag is set
	if (context->Set)
	{
		// Tag unset
		if (s_TagDown != context->Tag)
		{
			context->Set = FT_FALSE;
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
			context->Set = FT_TRUE;
			context->Down(context->Owner);
		}
	}
}

void Ft_Esd_TouchTag_End(Ft_Esd_TouchTag *context)
{
	if (context->Set)
	{
		context->Set = FT_FALSE;
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

ft_bool_t Ft_Esd_TouchTag_Touching(Ft_Esd_TouchTag *context)
{
	return context->Tag && (s_TagDown == context->Tag);
}

ft_bool_t Ft_Esd_TouchTag_Inside(Ft_Esd_TouchTag *context)
{
	return context->Tag && (s_TagDown == context->Tag) && (s_GpuRegTouchTag == context->Tag);
}

ft_bool_t Ft_Esd_TouchTag_Hover(Ft_Esd_TouchTag *context)
{
	return context->Tag && (s_GpuRegTouchTag == context->Tag);
}

ft_uint8_t Ft_Esd_TouchTag_Tag(Ft_Esd_TouchTag *context)
{
	if (context->Tag)
		return context->Tag;
	else
		return 255; // Always return non-tag value if no tag
}

ft_uint8_t Ft_Esd_TouchTag_CurrentTag(Ft_Esd_TouchTag *context)
{
	return s_GpuRegTouchTag;
}

ft_int16_t Ft_Esd_TouchTag_TouchX(Ft_Esd_TouchTag *context)
{
	return s_TouchPos.X;
}

ft_int16_t Ft_Esd_TouchTag_TouchY(Ft_Esd_TouchTag *context)
{
	return s_TouchPos.Y;
}

ft_int16_t Ft_Esd_TouchTag_TouchXDelta(Ft_Esd_TouchTag *context)
{
	return s_TouchPosXDelta;
}

ft_int16_t Ft_Esd_TouchTag_TouchYDelta(Ft_Esd_TouchTag *context)
{
	return s_TouchPosYDelta;
}

void Ft_Esd_TouchTag_SuppressCurrentTags()
{
	s_SuppressCurrentTags = 1;
}

/* Nothing beyond this */
