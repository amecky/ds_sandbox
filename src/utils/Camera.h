#pragma once
#include <diesel.h>
#include <cmath>

class TopDownCamera {

public:
	TopDownCamera(ds::Camera* camera);
	virtual ~TopDownCamera();
	void update(float dt, RID viewPortId = INVALID_RID);
private:
	ds::Camera* _camera;
};

class FPSCamera {

public:
	FPSCamera(ds::Camera* camera);
	virtual ~FPSCamera();
	void setPosition(const ds::vec3& pos);
	void setPosition(const ds::vec3& pos, const ds::vec3& lookAt);
	void setSpeed(float spd) {
		_speed = spd;
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
	void update(float elapsedTime, RID viewPortId = INVALID_RID);
	void buildView();
private:
	ds::Camera* _camera;
	float _speed;
	ds::vec2 _lastMousePos;
};
