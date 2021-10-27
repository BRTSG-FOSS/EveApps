#include "Event.h"
#include <time.h>
#define MIN_MOVE         15
#define MIN_TOUCH        14

void getTouchEvent(EVE_HalContext *pHalContext, int *x, int *y, int8_t *isTouch)
{
	*x = Gpu_Hal_Rd16(pHalContext, REG_TOUCH_SCREEN_XY);
	*y = Gpu_Hal_Rd16(pHalContext, REG_TOUCH_SCREEN_XY + 2);

	*isTouch = !(Gpu_Hal_Rd16(pHalContext, REG_TOUCH_RAW_XY) & 0x8000);
}

uchar8_t is_touch(Gpu_Hal_Context_t *pHalContext) {
	uchar8_t ret = !(Gpu_Hal_Rd16(pHalContext, REG_TOUCH_RAW_XY) & 0x8000);
	return ret;
}
