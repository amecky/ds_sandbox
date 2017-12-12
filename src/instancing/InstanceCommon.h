#pragma once
#include <diesel.h>

enum BackgroundSide {
	BS_LEFT_SIDE = 1,
	BS_TOP_SIDE,
	BS_RIGHT_SIDE,
	BS_BOTTOM_SIDE
};

struct InstanceLightBuffer {
	ds::Color ambientColor;
	ds::Color diffuseColor;
	ds::vec3 lightDirection;
	float padding;
};

//struct MultipleLightsBuffer {
	//InstanceLightBuffer lights[3];
//};

struct InstanceBuffer {
	ds::matrix mvp;
	ds::matrix world;
};

struct InstanceData {
	ds::matrix worldMatrix;
	ds::Color color;
};

struct Bullet {
	ds::vec3 pos;
	ds::vec3 velocity;
	float rotation;
	float timer;
	ds::vec2 scale;
};