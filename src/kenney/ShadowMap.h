#pragma once
#include <diesel.h>

struct MatrixBuffer {
	ds::matrix worldMatrix;
	ds::matrix viewMatrix;
	ds::matrix projectionMatrix;
	ds::matrix shadowTransform;
};

struct LightColorBuffer {
	ds::Color ambientColor;
	ds::Color diffuseColor;
};

struct LightBuffer {
	ds::vec3 lightPosition;
	float padding;
};

class ShadowMap {

public:
	ShadowMap();
	void buildView();
	void render(const ds::vec3& pos, RID drawItem);
private:
	float _sceneRadius;
	ds::vec3 _lightPos;
	ds::vec3 _lightDirection;
	ds::matrix _viewProjectionMatrix;
	ds::matrix _shadowTransformMatrix;
	MatrixBuffer _matrixBuffer;
	LightColorBuffer _lightColorBuffer;
	LightBuffer _lightBuffer;
};