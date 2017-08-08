#pragma once
#include <diesel.h>

// ---------------------------------------------------------
// AABBox
// ---------------------------------------------------------
struct AABBox {

	float top;
	float left;
	float right;
	float bottom;

	AABBox() : top(0.0f), left(0.0f), right(0.0f), bottom(0.0f) {}

	AABBox(const ds::vec2& center, float extent) {
		top = center.y + extent;
		left = center.x - extent;
		right = center.x + extent;
		bottom = center.y - extent;
	}

	bool intersects(const AABBox& other) {
		if (right < other.left || left > other.right) return false;
		if (bottom < other.top || top > other.bottom) return false;
		return true;
	}

	bool isInside(const ds::vec2& p) {
		if ( p.x > right || p.x < left) return false;
		if ( p.y < bottom || p.y > top) return false;
		return true;
	}
};
