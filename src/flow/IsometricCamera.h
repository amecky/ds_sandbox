#pragma once
#include <diesel.h>
#include <cmath>

class IsometricCamera {

public:
	IsometricCamera(ds::Camera* camera);
	virtual ~IsometricCamera();
	void setPosition(const ds::vec3& pos);
	void setPosition(const ds::vec3& pos, const ds::vec3& lookAt);
	void setSpeed(float spd) {
		_speed = spd;
	}
	void setRotation(const ds::vec3& r) {
		_rotation = r;
		buildView();
	}
	// moving the camera
	void move(float unit);
	void strafe(float unit);
	void up(float unit);
	// Y axis
	void setPitch(float angle);
	void resetPitch(float angle);
	// Z axis
	void resetYaw(float angle);
	void setYaw(float angle);
	void update(float elapsedTime);
	void buildView();
private:
	ds::Camera* _camera;
	float _speed;
	ds::vec2 _lastMousePos;
	ds::vec3 _position;
	ds::vec3 _rotation;
};
