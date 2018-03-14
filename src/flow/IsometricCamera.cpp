#include "IsometricCamera.h"

IsometricCamera::IsometricCamera(ds::Camera* camera) : _camera(camera) {
	_lastMousePos = ds::getMousePosition();
	_speed = 10.0f;
	buildView();
}

IsometricCamera::~IsometricCamera() {

}

void IsometricCamera::setPosition(const ds::vec3& pos, const ds::vec3& target) {
	_camera->position = pos;
	_camera->target = ds::normalize(target - pos);
	buildView();
}

void IsometricCamera::setPosition(const ds::vec3& pos) {
	_camera->position = pos;		
	buildView();
}

void IsometricCamera::move(float unit) {
	_camera->position += _camera->target * unit;
	buildView();
}

void IsometricCamera::strafe(float unit) {
	_camera->position += _camera->right * unit;
	buildView();
}

void IsometricCamera::up(float unit) {
	ds::vec3 tmp = _camera->up * unit;
	_camera->position = _camera->position + tmp;
	buildView();
}

void IsometricCamera::setPitch(float angle) {
	ds::matrix R = ds::matRotation(_camera->right,angle);
	_camera->up = ds::matTransformNormal(_camera->up, R);
	_camera->target = ds::matTransformNormal(_camera->target, R);
	buildView();
}

void IsometricCamera::resetYaw(float angle) {
	_camera->yaw = angle;
	ds::matrix R = ds::matRotationZ(_camera->yaw);
	_camera->right = R * ds::vec3(1, 0, 0);
	_camera->target = R * ds::vec3(0, 0, 1);
	buildView();
}

void IsometricCamera::setYaw(float angle) {
	ds::matrix R = ds::matRotationY(angle);
	_camera->right = ds::matTransformNormal(_camera->right, R);
	_camera->up = ds::matTransformNormal(_camera->up, R);
	_camera->target = ds::matTransformNormal(_camera->target, R);
	buildView();
}
	
void IsometricCamera::resetPitch(float angle) {
	_camera->pitch = angle;
	ds::matrix R = ds::matRotationY(_camera->pitch);
	_camera->right = R * ds::vec3(1, 0, 0);
	_camera->target = R * ds::vec3(0, 0, 1);
	buildView();
}

void IsometricCamera::update(float elapsedTime) {
	bool moved = false;
	ds::vec3 v(0.0f);
	if (ds::isKeyPressed('W')) {
		v.z = 5.0f;
		moved = true;
	}
	if (ds::isKeyPressed('S')) {
		v.z = -5.0f;
		moved = true;
	}
	if (ds::isKeyPressed('A')) {
		v.x = -5.0f;
		moved = true;
	}
	if (ds::isKeyPressed('D')) {
		v.x = 5.0f;
		moved = true;
	}
	if (moved) {
		_camera->position += v * elapsedTime;
		buildView();
	}
}

void IsometricCamera::buildView() {
	ds::rebuildCamera(_camera);
}
