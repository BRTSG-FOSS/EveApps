
#include "ESD_Math.h"

// #define ESD_RECT_STRICT (1) //< Results not guaranteed for inverted rectangles when disabled (width or height negative)

bool ESD_Rect16_Intersects(ESD_Rect16 a, ESD_Rect16 b)
{
	return (b.X < (a.X + a.Width)) && (a.X < (b.X + b.Width))
	    && (b.Y < (a.Y + a.Height)) && (a.Y < (b.Y + b.Height));
}

bool ESD_Rect16_IsInside(ESD_Rect16 subrect, ESD_Rect16 rect)
{
#ifdef ESD_RECT_STRICT
	return (subrect.X >= rect.X) && (subrect.X < (rect.X + rect.Width))
	    && ((subrect.X + subrect.Width) > rect.X) && ((subrect.X + subrect.Width) <= (rect.X + rect.Width))
	    && (subrect.Y >= rect.Y) && (subrect.Y < (rect.Y + rect.Height))
	    && ((subrect.Y + subrect.Height) > rect.Y) && ((subrect.Y + subrect.Height) <= (rect.Y + rect.Height));
#else
	return (subrect.X >= rect.X) && ((subrect.X + subrect.Width) <= (rect.X + rect.Width))
	    && (subrect.Y >= rect.Y) && ((subrect.Y + subrect.Height) <= (rect.Y + rect.Height));
#endif
}

bool ESD_Rect16_IsInsideHorizontal(ESD_Rect16 subrect, ESD_Rect16 rect)
{
#ifdef ESD_RECT_STRICT
	return (subrect.X >= rect.X) && (subrect.X < (rect.X + rect.Width))
	    && ((subrect.X + subrect.Width) > rect.X) && ((subrect.X + subrect.Width) <= (rect.X + rect.Width));
#else
	return (subrect.X >= rect.X) && ((subrect.X + subrect.Width) <= (rect.X + rect.Width));
#endif
}

bool ESD_Rect16_IsInsideVertical(ESD_Rect16 subrect, ESD_Rect16 rect)
{
#ifdef ESD_RECT_STRICT
	return (subrect.Y >= rect.Y) && (subrect.Y < (rect.Y + rect.Height))
	    && ((subrect.Y + subrect.Height) > rect.Y) && ((subrect.Y + subrect.Height) <= (rect.Y + rect.Height));
#else
	return (subrect.Y >= rect.Y) && ((subrect.Y + subrect.Height) <= (rect.Y + rect.Height));
#endif
}

ESD_Rect16 ESD_Rect16_Crop(ESD_Rect16 rect, ESD_Rect16 crop)
{
	int16_t x1diff;
	int16_t y1diff;
	int16_t x2diff;
	int16_t y2diff;

	x1diff = (crop.X - rect.X); // old x1 - new x1
	if (x1diff > 0) // old x1 > new x1
	{
		rect.Width -= x1diff;
		rect.X = crop.X;
	}
	y1diff = (crop.Y - rect.Y); // old y1 - new y1
	if (y1diff > 0) // old y1 > new y1
	{
		rect.Height -= y1diff;
		rect.Y = crop.Y;
	}
	x2diff = (crop.X + crop.Width) - (rect.X + rect.Width); // old x2 - new x2
	if (x2diff < 0) // old x2 < new x2
	{
		rect.Width += x2diff;
	}
	y2diff = (crop.Y + crop.Height) - (rect.Y + rect.Height); // old y2 - new y2
	if (y2diff < 0) // old y2 < new y2
	{
		rect.Height += y2diff;
	}
	return rect;
}

/* end of file */