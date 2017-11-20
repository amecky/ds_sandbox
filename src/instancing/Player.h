#pragma once
#include <diesel.h>
#include "InstanceCommon.h"
#include "TopDownCamera.h"

struct PlayerConstantBuffer {
	ds::matrix viewprojectionMatrix;
	ds::matrix worldMatrix;
};

class Player {

public:
	Player(TopDownCamera* camera) : _camera(camera) {}

	void init();

	void render(RID renderPass, const ds::matrix& viewProjectionMatrix);

	void tick(float dt);

	const ds::vec3& getPosition() const {
		return _position;
	}

	void setPosition(const ds::vec3& p);

	float getRotation() const {
		return _rotation;
	}

	void setRotation(float angle) {
		_rotation = angle;
	}

private:
	TopDownCamera* _camera;
	RID _drawItem;
	ds::vec3 _position;
	ds::vec3 _velocity;
	float _rotation;
	PlayerConstantBuffer _constantBuffer;
	InstanceLightBuffer _lightBuffer;
};
