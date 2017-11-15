#pragma once
#include <diesel.h>

struct InstanceLightBuffer {
	ds::Color ambientColor;
	ds::Color diffuseColor;
	ds::vec3 lightDirection;
	float padding;
};

struct InstanceBuffer {
	ds::matrix mvp;
	ds::matrix world;
};

struct InstanceData {
	ds::matrix worldMatrix;
	ds::Color color;
};