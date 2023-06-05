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


// support function for the GJK algorithm based on the Minkowski difference
Vector3d support(
	const std::vector<Vector3d>& first,
	const std::vector<Vector3d>& second,
	Vector3d direction
);

// helper functions
double dot(
	const Vector3d& first,
	const Vector3d& second
);
Vector3d cross(
	const Vector3d& first,
	const Vector3d& second
);
Vector3d tripleProduct(
	const Vector3d& first,
	const Vector3d& second,
	const Vector3d& third
);

// handles final edge cases in the GJK algorithm
bool handleSimplex(std::vector<Vector3d>& simplex, const Vector3d& direction);
bool handleSimplex2D(
    std::vector<Vector3d>& simplex,
    Vector3d& direction,
    Vector3d& AB,
    Vector3d& AO
);


bool UMaths::GJK(
	const std::vector<Vector3d>& first,
	const std::vector<Vector3d>& second
) {
	std::vector<Vector3d> simplex;
	Vector3d direction(1, 0, 0);

    simplex.push_back(support(first, second, direction));

	direction = -simplex[0];

    while (true) {
        Vector3d point = support(first, second, direction);

        if (dot(point, direction) < 0) {
            return false;
        } 
		else {
            simplex.push_back(point);
        }

        if (handleSimplex(simplex, direction)) {
            return true;
        }
    }
}


Vector3d support(
	const std::vector<Vector3d>& first,
	const std::vector<Vector3d>& second,
	Vector3d direction
) {
	int index1 = 0;
	double maxDot1 = dot(first[0], direction);
	for (int i = 1; i < first.size(); ++i) {
		double cand = dot(first[i], direction);
		if (cand > maxDot1) {
			maxDot1 = cand;
			index1 = i;
		}
	}

	int index2 = 0;
	double maxDot2 = dot(second[0], -direction);
	for (int i = 1; i < second.size(); ++i) {
		double cand = dot(second[i], -direction);
		if (cand > maxDot2) {
			maxDot2 = cand;
			index2 = i;
		}
	}

	return first[index1] - second[index2];
}

double dot(
	const Vector3d& first,
	const Vector3d& second
) {
	return (double)first.x * second.x + (double)first.y * second.y +
		(double)first.z * second.z;
}

Vector3d cross(
	const Vector3d& first,
	const Vector3d& second
) {
	return Vector3d(
		first.y * second.z - first.z * second.y,
		first.z * second.x - first.x * second.z,
		first.x * second.y - first.y * second.x
	);
}

Vector3d tripleProduct(
	const Vector3d& first,
	const Vector3d& second,
	const Vector3d& third
) {
	return second * dot(first, third) - first * dot(first, second);
}

bool handleSimplex(std::vector<Vector3d>& simplex, const Vector3d& dir) {
	Vector3d A = simplex.back(); // point added in the last iteration
    Vector3d direction = dir;

    if (simplex.size() == 3) {
        // line segment case
        Vector3d B = simplex[0];
        Vector3d AB = B - A;
        Vector3d AO = -A; 

        if (dot(AB, AO) > 0) {
            direction = tripleProduct(AB, AO, AB);  
        } else {
            simplex.erase(simplex.begin());
            direction = AO;
        }
    } else if (simplex.size() == 4) {
        // triangle case
        Vector3d B = simplex[1];
        Vector3d C = simplex[0];
        Vector3d AB = B - A;
        Vector3d AC = C - A;
        Vector3d AO = -A;
        Vector3d ABC = cross(AB, AC);  

        if (dot(cross(ABC, AC), AO) > 0) {
            if (dot(AC, AO) > 0) {
                simplex.erase(simplex.begin() + 1); 
                direction = tripleProduct(AC, AO, AC);
            } else {
                return handleSimplex2D(simplex, direction, AB, AO);
            }
        } else {
            if (dot(cross(ABC, AB), AO) > 0) {
                return handleSimplex2D(simplex, direction, AB, AO);
            } else {
                if (dot(ABC, AO) > 0) {
                    direction = ABC;
                } else {
                    std::swap(simplex[0], simplex[1]);
                    direction = -ABC;
                }
            }
        }
    } else if (simplex.size() == 5) {
        // tetrahedon case
        return true;
    }

    return false;
}

bool handleSimplex2D(std::vector<Vector3d>& simplex, Vector3d& direction, Vector3d& AB, Vector3d& AO) {
    if (dot(AB, AO) > 0) {
        simplex.erase(simplex.begin()); 
        direction = tripleProduct(AB, AO, AB);
		return false;
    }
    else {
        simplex.erase(simplex.begin() + 1);  
        simplex.erase(simplex.begin());  
        direction = AO;
		return false;
    }
}
