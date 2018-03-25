#include "IsometricCamera.h"

IsometricCamera::IsometricCamera(ds::Camera* camera) : _camera(camera) {
	_lastMousePos = ds::getMousePosition();
	_speed = 10.0f;
	_position = ds::vec3(0.0f, 6.0f, -6.0f);
	_rotation = ds::vec3(0.0f);
	buildView();
}

IsometricCamera::~IsometricCamera() {

}

void IsometricCamera::setPosition(const ds::vec3& pos, const ds::vec3& target) {
	_camera->position = pos;
	_camera->target = ds::normalize(target - pos);
	_position = pos;
	_rotation = ds::vec3(0.0f);
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
	float p = 0.0f;
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
	if (ds::isKeyPressed('Q')) {
		p = -5.0f;
		moved = true;
	}
	if (ds::isKeyPressed('E')) {
		p = 5.0f;
		moved = true;
	}
	if (moved) {
		_position += v * elapsedTime;
		_rotation.x += p * elapsedTime;
		buildView();
	}
}

void IsometricCamera::buildView() {
	ds::vec3 m_up = ds::vec3(0.0f, 1.0f, 0.0f);
	ds::vec3 m_look = ds::vec3(0.0f, 0.0f, 1.0f);
	ds::vec3 m_right = ds::vec3(1.0f, 0.0f, 0.0f);

	ds::matrix yawMatrix = ds::matRotation(m_up, _rotation.y);
	m_look = ds::matTransformCoord(yawMatrix,m_look);
	m_right = ds::matTransformCoord(yawMatrix, m_right);

	ds::matrix pitchMatrix = ds::matRotation(m_right, _rotation.x);
	m_look = ds::matTransformCoord(pitchMatrix, m_look);
	m_up = ds::matTransformCoord(pitchMatrix, m_up);

	ds::matrix rollMatrix = ds::matRotation(m_look, _rotation.z);
	m_right = ds::matTransformCoord(rollMatrix, m_right);
	m_up = ds::matTransformCoord(rollMatrix, m_up);

	_camera->viewMatrix = ds::matIdentity();
	//Frenet
	_camera->viewMatrix._11 = m_right.x; _camera->viewMatrix._12 = m_up.x; _camera->viewMatrix._13 = m_look.x;
	_camera->viewMatrix._21 = m_right.y; _camera->viewMatrix._22 = m_up.y; _camera->viewMatrix._23 = m_look.y;
	_camera->viewMatrix._31 = m_right.z; _camera->viewMatrix._32 = m_up.z; _camera->viewMatrix._33 = m_look.z;

	_camera->viewMatrix._41 = -dot(_position, m_right);
	_camera->viewMatrix._42 = -dot(_position, m_up);
	_camera->viewMatrix._43 = -dot(_position, m_look);

	_camera->viewProjectionMatrix = _camera->viewMatrix * _camera->projectionMatrix;
}
