#ifndef SET11_H_
#define SET11_H_

#include "Samapp_Enable_API.h"

#ifdef SAMAPP_ENABLE_APIS_SET11

APP_HEADERS(11)

typedef struct SAMAPP_ColorRGB {
	int r;
	int g;
	int b;
}SAMAPP_ColorRGB_t;

typedef struct SAMAPP_Circle {
	int visible;
	int x;
	int y;
	int radius;
	SAMAPP_ColorRGB_t color;
	int opacity; // 0 to 256
	int step;
}SAMAPP_Circle_t;

#endif //SAMAPP_ENABLE_APIS_SET11
#endif /* SET11_H_ */
