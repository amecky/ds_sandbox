#include "common_math.h"
#include <diesel.h>

Plane::Plane(const ds::vec3& p, const ds::vec3& n) {
	ds::vec3 nn = normalize(n);
	_a = nn.x;
	_b = nn.y;
	_c = nn.z;
	_d = -dot(p, n);
	_n = nn;
}

ds::vec3 Plane::getIntersection(const Ray& r) {
	float denominator = dot(_n, r.getDirection());
	float numerator = dot(_n, r.getOrigin()) + _d;
	float t = -(numerator / denominator);
	return r.getOrigin() + r.getDirection() * t;
}

ds::vec3 normalizePoint(float x, float y) {
	ds::vec3 ret;
	ret.x = 2.0f * x / ds::getScreenWidth() - 1.0f;
	ret.y = (2.0f * y / ds::getScreenHeight() - 1.0f);
	ret.z = 0.0f;
	return ret;
}

Ray get_picking_ray(const ds::matrix& projection, const ds::matrix& view, RID viewPortId) {
	ds::vec2 mp = ds::getMousePosition(viewPortId);
	ds::vec3 org = normalizePoint(mp.x, mp.y);
	org.x /= projection._11;
	org.y /= projection._22;
	ds::vec3 origin = ds::vec3(0.0f);
	ds::matrix iv = ds::matInverse(view);
	ds::vec3 direction;
	direction.x = (org.x * iv._11) + (org.y * iv._21) + iv._31;
	direction.y = (org.x * iv._12) + (org.y * iv._22) + iv._32;
	direction.z = (org.x * iv._13) + (org.y * iv._23) + iv._33;
	direction = normalize(direction);
	Ray r = Ray(origin, direction);
	return r;
}

namespace math {

	bool out_of_bounds(const ds::vec3& pos, const ds::vec4& box) {
		return (pos.y < box.y || pos.y > box.w || pos.x < box.x || pos.x > box.z);
	}

	float get_angle(const ds::vec2& v1, const ds::vec2& v2) {
		if (v1 != v2) {
			ds::vec2 vn1 = normalize(v1);
			ds::vec2 vn2 = normalize(v2);
			float dt = dot(vn1, vn2);
			if (dt < -1.0f) {
				dt = -1.0f;
			}
			if (dt > 1.0f) {
				dt = 1.0f;
			}
			float tmp = acos(dt);
			float cross = -1.0f * (vn2 - vn1).y;
			if (cross < 0.0f) {
				tmp = ds::TWO_PI - tmp;
			}
			return tmp;
		}
		else {
			return 0.0f;
		}
	}

	const static ds::vec2 V2_RIGHT = ds::vec2(1, 0);

	float get_rotation(const ds::vec2& v1) {
		return get_angle(v1, V2_RIGHT);
	}

}