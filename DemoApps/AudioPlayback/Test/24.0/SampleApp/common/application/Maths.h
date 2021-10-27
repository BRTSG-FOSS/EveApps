#ifndef MATH_H_
#define MATH_H_

#include <stdio.h>
#include <stdint.h>
#include "EVE_CoCmd.h"

typedef struct Math_3d_Xyz {
	double x, y, z;
} Math_3d_Xyz_t;

typedef struct _Math_3d_Face_t {
	Math_3d_Xyz_t p1, p2, p3;
}Math_3d_Face_t;

float Math_Da(float i, int16_t degree);
void Math_Polarxy(int32_t r, float th, int32_t *x, int32_t *y, int32_t ox, int32_t oy);
int16_t Math_Qsin(uint16_t a);
int16_t Math_Qcos(uint16_t a);
float Math_Power(float x, unsigned int y);
uint32_t Math_Points_Distance(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2);
uint32_t Math_Points_Nearby_NextX(uint32_t x1, uint32_t y1, uint32_t y2, uint32_t Distance);

Math_3d_Xyz_t Math_3D_ArbitraryRotate(Math_3d_Xyz_t p, double theta, Math_3d_Xyz_t r);
Math_3d_Xyz_t Math_3D_ArbitraryRotate2(Math_3d_Xyz_t p, double theta, Math_3d_Xyz_t p1, Math_3d_Xyz_t p2);
int Math_3D_Backface_Find_Visible(Math_3d_Face_t face, Math_3d_Xyz_t view);

#endif /* MATH_H_ */
