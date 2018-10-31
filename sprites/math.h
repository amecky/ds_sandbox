#pragma once
#include <diesel.h>

float clamp(float d, float minValue, float maxValue);

float calculate_rotation(const ds::vec2& v);

bool is_inside(const ds::vec2& pos, const ds::vec2& p, const ds::vec2& r);