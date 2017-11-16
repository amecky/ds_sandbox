#include "TopDownCamera.h"

TopDownCamera::TopDownCamera(ds::Camera* camera) : _camera(camera) {
	_speed = 10.0f;
	buildView();
}

TopDownCamera::~TopDownCamera() {

}

void TopDownCamera::setPosition(const ds::vec3& pos, const ds::vec3& target) {
	_camera->position = pos;
	_camera->target = ds::normalize(target - pos);
	buildView();
}

void TopDownCamera::setPosition(const ds::vec3& pos) {
	_camera->position = pos;		
	buildView();
}

void TopDownCamera::move(float unit) {
	_camera->position += _camera->up * unit;
	buildView();
}

void TopDownCamera::strafe(float unit) {
	_camera->position += _camera->right * unit;
	buildView();
}

void TopDownCamera::up(float unit) {
	ds::vec3 tmp = _camera->up * unit;
	_camera->position = _camera->position + tmp;
	buildView();
}

void TopDownCamera::setPitch(float angle) {
	ds::matrix R = ds::matRotation(_camera->right,angle);
	_camera->up = ds::matTransformNormal(_camera->up, R);
	_camera->target = ds::matTransformNormal(_camera->target, R);
	buildView();
}

void TopDownCamera::resetYaw(float angle) {
	_camera->yaw = angle;
	ds::matrix R = ds::matRotationZ(_camera->yaw);
	_camera->right = R * ds::vec3(1, 0, 0);
	_camera->target = R * ds::vec3(0, 0, 1);
	buildView();
}

void TopDownCamera::setYaw(float angle) {
	ds::matrix R = ds::matRotationY(angle);
	_camera->right = ds::matTransformNormal(_camera->right, R);
	_camera->up = ds::matTransformNormal(_camera->up, R);
	_camera->target = ds::matTransformNormal(_camera->target, R);
	buildView();
}
	
void TopDownCamera::resetPitch(float angle) {
	_camera->pitch = angle;
	ds::matrix R = ds::matRotationY(_camera->pitch);
	_camera->right = R * ds::vec3(1, 0, 0);
	_camera->target = R * ds::vec3(0, 0, 1);
	buildView();
}

void TopDownCamera::update(float elapsedTime) {
	if (ds::isKeyPressed('W')) {
		move(5.0f*elapsedTime);
	}
	if (ds::isKeyPressed('S')) {
		move(-5.0f*elapsedTime);
	}
	if (ds::isKeyPressed('A')) {
		strafe(-5.0f*elapsedTime);
	}
	if (ds::isKeyPressed('D')) {
		strafe(5.0f*elapsedTime);
	}
}

void TopDownCamera::buildView() {
	ds::rebuildCamera(_camera);
}
