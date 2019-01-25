#include "math.h"

float clamp(float d, float minValue, float maxValue) {
	if (d < minValue) {
		return minValue;
	}
	if (d > maxValue) {
		return maxValue;
	}
	return d;
}

float calculate_rotation(const ds::vec2& v) {
	ds::vec2 vn = normalize(v);
	ds::vec2 right(1.0f, 0.0f);
	if (vn != right) {
		float dt = clamp(dot(vn, right), -1.0f, 1.0f);
		float tmp = acos(dt);
		float cross = -1.0f * vn.y();
		if (cross > 0.0f) {
			tmp = 2.0f * ds::PI - tmp;
		}
		return tmp;
	}
	else {
		return 0.0f;
	}
}

bool is_inside(const ds::vec2& pos, const ds::vec2& p, const ds::vec2& r) {
	float xmin = p.x() - r.x() * 0.5f;
	float xmax = p.x() + r.x() * 0.5f;
	float ymin = p.y() - r.y() * 0.5f;
	float ymax = p.y() + r.y() * 0.5f;
	if (pos.x() >= xmin && pos.x() <= xmax) {
		if (pos.y() >= ymin && pos.y() <= ymax) {
			return true;
		}
	}
	return false;
}

namespace math {

	bool collides(const ds::vec2& p1, float r1, const ds::vec2& p2, float r2) {
		ds::vec2 d = p1 - p2;
		float sr = (r1 + r2) * (r1 + r2);
		return sqr_length(d) < sr;
	}

	bool line_circle_intersection(const ds::vec2& O,const ds::vec2& D,const ds::vec2& C, float radius) {
		ds::vec2 d = O - C;
		float a = dot(D, D);
		float b = dot(d, D);
		float c = dot(d, d) - radius * radius;

		float disc = b * b - a * c;
		if (disc < 0.0f) {
			return false;
		}
		return true;
	}
}