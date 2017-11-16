#pragma once
#include <diesel.h>
#include <cmath>

class TopDownCamera {

public:
	TopDownCamera(ds::Camera* camera);
	virtual ~TopDownCamera();
	void setPosition(const ds::vec3& pos);
	const ds::vec3& getPosition() const {
		return _camera->position;
	}
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
	void update(float elapsedTime);
	void buildView();
private:
	ds::Camera* _camera;
	float _speed;
};
