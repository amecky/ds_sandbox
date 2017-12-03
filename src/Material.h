#pragma once
#include <diesel.h>

class Material {

public:
	Material() {}
	virtual ~Material() {}
	virtual void apply() = 0;
	RID getBasicGroupID() const {
		return _basicGroup;
	}
	RID getPixelShaderID() const {
		return _pixelShader;
	}
	RID getVertexShaderID() const {
		return _vertexShader;
	}
protected:
	RID _basicGroup;
	RID _pixelShader;
	RID _vertexShader;
	RID _blendState;
};

class AmbientLightningMaterial : public Material {

struct LightBuffer {
	ds::Color ambientColor;
	ds::Color diffuseColor;
	ds::vec3 lightDirection;
	float padding;
};

struct MultipleLightsBuffer {
	LightBuffer lights[3];
};

struct VBConstantBuffer {
	ds::matrix mvp;
	ds::matrix world;
};

public:
	AmbientLightningMaterial();
	virtual ~AmbientLightningMaterial() {}
	void apply();
	void transform(const ds::matrix& world, const ds::matrix& viewProjection);
	void setLightDirection(int index, const ds::vec3& dir) {
		_lightDirection[index] = normalize(dir);
	}
private:
	ds::vec3 _lightDirection[3];
	MultipleLightsBuffer _lightBuffer;
	VBConstantBuffer _constantBuffer;
};