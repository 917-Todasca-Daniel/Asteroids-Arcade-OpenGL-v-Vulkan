#pragma once

#include "window_constants.hpp"


//	prevents redundant slow calls of sqrt() fn
#define SQRT_3	1.73205080757


namespace aa
{
    void U_worldToGLCoord(float &x, float &y) {
		x /= WINDOW_WIDTH;
		y /= WINDOW_HEIGHT;

		x = 2.0f * x - 1.0;
		y = 2.0f * y - 1.0;
    }


	//	given the topmost corner(ax, ay) and the altitude
	//	bx and by make up the leftmost corner
	void U_worldTriangleTopAltitude(
		float altitude,
		float ax, float ay,
		float& bx, float& by,
		float& cx, float &cy
	) {
		float edge = 2.0f * altitude * SQRT_3 / 3;
		bx = ax - (0.5f * edge);
		cx = bx + edge;

		by = ay - altitude;
		cy = by;
	}

}
