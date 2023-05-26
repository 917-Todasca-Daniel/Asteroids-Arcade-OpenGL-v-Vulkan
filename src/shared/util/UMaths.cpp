#include "UMaths.h"

using namespace aa;


void UMaths::worldToGLCoord(float& x, float& y) {
	x /= WINDOW_WIDTH;
	y /= WINDOW_HEIGHT;

	x = (float) 2.0f * x - 1.0;
	y = (float) 2.0f * y - 1.0;
}


void UMaths::worldTriangleTopAltitude(
	float altitude,
	float ax, float ay,
	float& bx, float& by,
	float& cx, float& cy
) {
	float edge = 2 * altitude * SQRT_3 / 3;
	bx = ax - (0.5f * edge);
	cx = bx + edge;

	by = ay - altitude;
	cy = by;
}


void UMaths::worldRectangleBottomLeft(
	float height, float width, 
	float ax, float ay, 
	float& bx, float& by, 
	float& cx, float& cy, 
	float& dx, float& dy
) {
	bx = ax + width;
	by = ay;

	cx = bx;
	cy = by + height;

	dx = ax;
	dy = cy;
}
