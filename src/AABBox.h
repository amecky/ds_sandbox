#pragma once
#include <diesel.h>

// ---------------------------------------------------------
// AABBox
// ---------------------------------------------------------
struct AABBox {

	ds::vec3 position;
	ds::vec3 previous;
	ds::vec3 extent;

	AABBox() : position(0.0f), previous(0.0f), extent(0.0f) {}

	AABBox(const ds::vec2& center, float ext) {
		position = center;
		extent = ds::vec3(extent);
		previous = position;
	}

	AABBox(const ds::vec3& center, float ext) {
		position = center;
		extent = ds::vec3(extent);
		previous = position;
	}

	AABBox(const ds::vec2& center, const ds::vec4& textureRect) {
		position = center;
		previous = center;
		float hw = textureRect.z * 0.5f;
		float hh = textureRect.w * 0.5f;
		extent = ds::vec2(hw, hh);
	}

	bool intersects(const AABBox& other) const {
		float top = position.y + extent.y;
		float left = position.x - extent.x;
		float right = position.x + extent.x;
		float bottom = position.y - extent.y;

		float otherTop = other.position.y + other.extent.y;
		float otherLeft = other.position.x - other.extent.x;
		float otherRight = other.position.x + other.extent.x;
		float otherBottom = other.position.y - other.extent.y;

		if (right < otherLeft || left > otherRight) return false;
		if (bottom > otherTop || top < otherBottom) return false;
		return true;
	}

	bool isInside(const ds::vec2& p) const {
		float top = position.y + extent.y;
		float left = position.x - extent.x;
		float right = position.x + extent.x;
		float bottom = position.y - extent.y;
		if ( p.x > right || p.x < left) return false;
		if ( p.y < bottom || p.y > top) return false;
		return true;
	}

	bool isInside(const ds::vec3& p) const {
		float top = position.y + extent.y;
		float left = position.x - extent.x;
		float right = position.x + extent.x;
		float bottom = position.y - extent.y;
		float back = position.z + extent.z;
		float front = position.z - extent.z;
		if (p.x > right  || p.x < left) return false;
		if (p.y < bottom || p.y > top ) return false;
		if (p.z < front  || p.z > back) return false;
		return true;
	}
};
