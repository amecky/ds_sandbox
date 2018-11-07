#pragma once
#include <diesel.h>

float clamp(float d, float minValue, float maxValue);

float calculate_rotation(const ds::vec2& v);

bool is_inside(const ds::vec2& pos, const ds::vec2& p, const ds::vec2& r);

namespace math {

	bool collides(const ds::vec2& p1, float r1, const ds::vec2& p2, float r2);

	bool line_circle_intersection(const ds::vec2& O, const ds::vec2& D, const ds::vec2& C, float radius);

}