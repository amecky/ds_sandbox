#pragma once
#include <diesel.h>

struct DepthConstantBuffer {
	ds::matrix viewprojectionMatrix;
	ds::matrix worldMatrix;
};

class DepthMap {

public:
	DepthMap(int dimension, RID inputLayoutId);
	void buildView(const ds::vec3 lightPos, const ds::vec3& lightDirection);
	void render(const ds::vec3& pos, RID drawItem);
	RID getDepthBaseGroup() const {
		return _depthGroup;
	}
	const ds::matrix& getShadowTransform() const {
		return _shadowTransformMatrix;
	}

	RID getRenderTarget() const {
		return _renderTargetId;
	}
private:
	float _sceneRadius;
	ds::vec3 _lightPos;
	ds::vec3 _lightDirection;
	ds::matrix _viewProjectionMatrix;
	ds::matrix _shadowTransformMatrix;
	DepthConstantBuffer _constantBuffer;
	RID _renderTargetId;
	RID _depthGroup;
	RID _renderPass;
};