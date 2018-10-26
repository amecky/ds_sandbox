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
		float cross = -1.0f * vn.y;
		if (cross > 0.0f) {
			tmp = 2.0f * ds::PI - tmp;
		}
		return tmp;
	}
	else {
		return 0.0f;
	}
}