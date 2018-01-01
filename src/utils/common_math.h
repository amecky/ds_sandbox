#pragma once
#include <diesel.h>

class Ray {

public:
	Ray() : _origin(0.0f), _direction(0.0f) {}
	~Ray() {}
	Ray(const ds::vec3& origin, const ds::vec3& direction) : _origin(origin), _direction(direction) {}
	const ds::vec3& getOrigin() const {
		return _origin;
	}
	const ds::vec3& getDirection() const {
		return _direction;
	}
	void setOrigin(const ds::vec3& origin) {
		_origin = origin;
	}
	void setDirection(const ds::vec3& direction) {
		_direction = direction;
	}
private:
	ds::vec3 _origin;
	ds::vec3 _direction;
};

class Plane {

public:
	Plane() : _a(1.0f), _b(1.0f), _c(1.0f), _d(0.0f), _n(0.0f, 0.0f, 1.0f) {}
	Plane(const ds::vec3& p, const ds::vec3& n);
	ds::vec3 getIntersection(const Ray& r);
private:
	float _a, _b, _c, _d;
	ds::vec3 _n;
};

Ray get_picking_ray(const ds::matrix& projection, const ds::matrix& view);

namespace math {

	bool out_of_bounds(const ds::vec3& pos, const ds::vec4& box);

	float get_angle(const ds::vec2& v1, const ds::vec2& v2);

	float get_rotation(const ds::vec2& v1);	
	
}