#ifndef GESTURE_H_
#define GESTURE_H_

#include "EVE_CoCmd.h"

typedef struct Gesture_Touch {
	uint16_t tagTrackTouched;
	uint8_t tagPressed;
	uint8_t tagReleased;
	uint8_t isTouch;
	uint8_t isSwipe;
	uint16_t touchX;
	uint16_t touchY;
	int velocityX;
}Gesture_Touch_t;

uint8_t Gesture_GetTag(EVE_HalContext *phost);
void Gesture_Renew(EVE_HalContext *phost);
Gesture_Touch_t* Gesture_Get(EVE_HalContext* phost);

#endif /* GESTURE_H_ */
