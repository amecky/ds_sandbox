#include "comon_math.h"

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
	ret.x = 2.0f * x / 1024.0f - 1.0f;
	ret.y = (2.0f * y / 768.0f - 1.0f);
	ret.z = 0.0f;
	return ret;
}

Ray get_picking_ray(const ds::matrix& projection, const ds::matrix& view) {
	ds::vec2 mp = ds::getMousePosition();
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