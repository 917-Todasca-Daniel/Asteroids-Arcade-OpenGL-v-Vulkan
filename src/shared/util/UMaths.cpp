#include "UMaths.h"

using namespace aa;


void UMaths::worldToGLCoord(float& x, float& y) {
	x /= WINDOW_HEIGHT_SM;
	y /= WINDOW_WIDTH_SM;

	x = 2.0f * x - 1.0;
	y = 2.0f * y - 1.0;
}


void UMaths::worldTriangleTopAltitude(
	float altitude,
	float ax, float ay,
	float& bx, float& by,
	float& cx, float& cy
) {
	float edge = altitude * SQRT_3 / 3;
	bx = ax - (0.5f * edge);
	cx = bx + edge;

	by = ay - altitude;
	cy = by;
}
