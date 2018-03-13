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
	/*
	if (ds::isKeyPressed('W')) {
		move(5.0f*elapsedTime);
	}
	if (ds::isKeyPressed('S')) {
		move(-5.0f*elapsedTime);
	}
	*/
	if (ds::isKeyPressed('A')) {
		strafe(-5.0f*elapsedTime);
	}
	if (ds::isKeyPressed('D')) {
		strafe(5.0f*elapsedTime);
	}
	if (ds::isKeyPressed('W')) {
		up(5.0f*elapsedTime);
	}
	if (ds::isKeyPressed('S')) {
		up(-5.0f*elapsedTime);
	}
	/*
	ds::vec2 mp = ds::getMousePosition();
	if (ds::isMouseButtonPressed(0)) {
		// Make each pixel correspond to a quarter of a degree.
		float dx = DEGTORAD(0.25f*(mp.x - _lastMousePos.x));
		float dy = DEGTORAD(0.25f*(mp.y - _lastMousePos.y));
		setPitch(dy);
		setYaw(dx);			
	}
	_lastMousePos.x = mp.x;
	_lastMousePos.y = mp.y;
	*/
}

void IsometricCamera::buildView() {
	ds::rebuildCamera(_camera);
}
