#include "Material.h"
#include "..\shaders\AmbientMultipleLightning_VS_Main.h"
#include "..\shaders\AmbientMultipleLightning_PS_Main.h"
#include "..\shaders\InstancedAmbientMultiple_VS_Main.h"
#include "..\shaders\InstancedAmbientMultiple_PS_Main.h"

AmbientLightningMaterial::AmbientLightningMaterial() {
	_vertexShader = ds::createVertexShader(AmbientMultipleLightning_VS_Main, sizeof(AmbientMultipleLightning_VS_Main));
	_pixelShader = ds::createPixelShader(AmbientMultipleLightning_PS_Main, sizeof(AmbientMultipleLightning_PS_Main));
	ds::BlendStateInfo blendInfo = { ds::BlendStates::SRC_ALPHA, ds::BlendStates::SRC_ALPHA, ds::BlendStates::INV_SRC_ALPHA, ds::BlendStates::INV_SRC_ALPHA, true };
	_blendState = ds::createBlendState(blendInfo);
	for (int i = 0; i < 3; ++i) {
		_lightBuffer.lights[i].ambientColor = ds::Color(0.1f, 0.1f, 0.1f, 1.0f);
		_lightBuffer.lights[i].diffuseColor = ds::Color(1.0f, 1.0f, 1.0f, 1.0f);
		_lightBuffer.lights[i].lightDirection = ds::vec3(0.0f, 0.0f, 1.0f);
		_lightBuffer.lights[i].padding = 0.0f;
	}
	_lightBuffer.lights[1].lightDirection = ds::vec3(-1.0f, 0.0f, 1.0f);
	_lightBuffer.lights[2].lightDirection = ds::vec3(1.0f, 0.0f, 1.0f);

	_lightDirection[0] = ds::vec3(1.0f, 0.5f, 1.0f);
	_lightDirection[1] = ds::vec3(-1.0f, 0.5f, 1.0f);
	_lightDirection[2] = ds::vec3(0.0f, 0.5f, 1.0f);


	RID lbid = ds::createConstantBuffer(sizeof(MultipleLightsBuffer), &_lightBuffer);

	RID cbid = ds::createConstantBuffer(sizeof(VBConstantBuffer), &_constantBuffer);

	_basicGroup = ds::StateGroupBuilder()
		.blendState(_blendState)
		.constantBuffer(lbid, _pixelShader, 0)
		.constantBuffer(cbid, _vertexShader, 0)
		.vertexShader(_vertexShader)
		.pixelShader(_pixelShader)
		.build();
}

void AmbientLightningMaterial::apply() {
	for (int i = 0; i < 3; ++i) {
		_lightBuffer.lights[i].lightDirection = normalize(_lightDirection[i]);
	}
}

void AmbientLightningMaterial::transform(const ds::matrix & world, const ds::matrix & viewProjection) {
	_constantBuffer.world = ds::matTranspose(world);
	_constantBuffer.mvp = ds::matTranspose(viewProjection);
}

InstancedAmbientLightningMaterial::InstancedAmbientLightningMaterial() {
	_vertexShader = ds::createVertexShader(InstancedAmbientMultiple_VS_Main, sizeof(InstancedAmbientMultiple_VS_Main));
	_pixelShader = ds::createPixelShader(InstancedAmbientMultiple_PS_Main, sizeof(InstancedAmbientMultiple_PS_Main));
	ds::BlendStateInfo blendInfo = { ds::BlendStates::SRC_ALPHA, ds::BlendStates::SRC_ALPHA, ds::BlendStates::INV_SRC_ALPHA, ds::BlendStates::INV_SRC_ALPHA, true };
	_blendState = ds::createBlendState(blendInfo);
	for (int i = 0; i < 3; ++i) {
		_lightBuffer.lights[i].ambientColor = ds::Color(0.1f, 0.1f, 0.1f, 1.0f);
		_lightBuffer.lights[i].diffuseColor = ds::Color(1.0f, 1.0f, 1.0f, 1.0f);
		_lightBuffer.lights[i].lightDirection = ds::vec3(0.0f, 0.0f, 1.0f);
		_lightBuffer.lights[i].padding = 0.0f;
	}
	_lightBuffer.lights[1].lightDirection = ds::vec3(-1.0f, 0.0f, 1.0f);
	_lightBuffer.lights[2].lightDirection = ds::vec3(1.0f, 0.0f, 1.0f);

	_lightDirection[0] = ds::vec3(1.0f, 0.5f, 1.0f);
	_lightDirection[1] = ds::vec3(-1.0f, 0.5f, 1.0f);
	_lightDirection[2] = ds::vec3(0.0f, 0.5f, 1.0f);


	RID lbid = ds::createConstantBuffer(sizeof(MultipleLightsBuffer), &_lightBuffer);

	RID cbid = ds::createConstantBuffer(sizeof(VBConstantBuffer), &_constantBuffer);

	_basicGroup = ds::StateGroupBuilder()
		.blendState(_blendState)
		.constantBuffer(lbid, _pixelShader, 0)
		.constantBuffer(cbid, _vertexShader, 0)
		.vertexShader(_vertexShader)
		.pixelShader(_pixelShader)
		.build();
}