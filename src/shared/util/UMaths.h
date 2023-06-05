#pragma once

#include "data/Vector3d.h"

#include "window_constants.hpp"

#include <vector>


namespace aa
{

	class UMaths {

	public:
		static void worldToGLCoord(float& x, float& y);

		//	given the topmost corner(ax, ay) and the altitude
		//	bx and by make up the leftmost corner
		static void worldTriangleTopAltitude(
				float altitude,
				float ax, float ay,
				float& bx, float& by,
				float& cx, float& cy
		);

		//	rectangle given by abcd
		//	a is the bottomleft corner, b is bottomright
		static void worldRectangleBottomLeft(
			float height, float width,
			float ax, float ay,
			float& bx, float& by,
			float& cx, float& cy,
			float& dx, float& dy
		);

		// heuristic algorithm for checking the interesection of two distinct
		// 3d convex objects
		static bool GJK(
			const std::vector<Vector3d> &first, 
			const std::vector<Vector3d> &second
		);

	private:
		//	prevents redundant slow calls of sqrt() fn
		static constexpr float SQRT_3 = 1.73205080757f;

	};

}
