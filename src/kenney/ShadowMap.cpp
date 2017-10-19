#include "ShadowMap.h"
#include "..\..\shaders\Depth_VS_Main.h"

ShadowMap::ShadowMap() {

	ds::ShaderInfo dvsInfo = { 0, Depth_VS_Main, sizeof(Depth_VS_Main), ds::ShaderType::ST_VERTEX_SHADER };
	RID depthVertexShader = ds::createShader(dvsInfo);

	MatrixBuffer matrixBuffer;
	matrixBuffer.worldMatrix = ds::matTranspose(ds::matIdentity());
	//matrixBuffer.viewMatrix = ds::matTranspose(viewMatrix);
	//matrixBuffer.projectionMatrix = ds::matTranspose(projectionMatrix);
	//matrixBuffer.shadowTransform = ds::matTranspose(shadowTransform);
	RID matrixBufferID = ds::createConstantBuffer(sizeof(MatrixBuffer), &matrixBuffer);

	_lightColorBuffer.ambientColor = ds::Color(0.15f, 0.15f, 0.15f, 1.0f);
	_lightColorBuffer.diffuseColor = ds::Color(1.0f, 1.0f, 1.0f, 1.0f);
	RID newLightBufferID = ds::createConstantBuffer(sizeof(LightColorBuffer), &_lightColorBuffer);

	_lightBuffer.lightPosition = _lightPos;
	_lightBuffer.padding = 0.0f;
	RID lightBuffer2ID = ds::createConstantBuffer(sizeof(LightBuffer), &_lightBuffer);

	ds::RenderTargetInfo rtInfo = { 1024, 1024, ds::Color(0.0f,0.0f,0.0f,1.0f) };
	RID renderTarget = ds::createDepthRenderTarget(rtInfo);

	ds::ViewportInfo depthVpInfo = { 1024, 1024, 0.0f, 1.0f };
	RID depthVp = ds::createViewport(depthVpInfo);
	RID targets[] = { renderTarget };
	//ds::RenderPassInfo rtrpInfo = { &camera, depthVp, ds::DepthBufferState::ENABLED, targets, 1 };
	//RID rtPass = ds::createRenderPass(rtrpInfo);

	RID depthGroup = ds::StateGroupBuilder()
		//.inputLayout(arid)
		//.constantBuffer(cbid, depthVertexShader, 0)
		.vertexShader(depthVertexShader)
		.pixelShader(NO_RID)
		.build();

}

void ShadowMap::buildView() {
	ds::matrix lightViewMatrix = ds::matLookAtLH(_lightPos, ds::vec3(0, 0, 0), ds::vec3(0, 1, 0));
	ds::matrix lightOrthoProjection = ds::matOrthoOffCenterLH(-_sceneRadius, _sceneRadius, -_sceneRadius, _sceneRadius, -15.0f, 15.0f);
	_viewProjectionMatrix = lightViewMatrix * lightOrthoProjection;
	ds::matrix lightTransform = ds::matIdentity();
	lightTransform(0, 0) = 0.5f;
	lightTransform(1, 1) = -0.5f;
	lightTransform(2, 2) = 1.0f;
	lightTransform(3, 0) = 0.5f;
	lightTransform(3, 1) = 0.5f;
	_shadowTransformMatrix = lightViewMatrix * lightOrthoProjection * lightTransform;
}

void ShadowMap::render(const ds::vec3& pos, RID drawItem) {
	//constantBuffer.viewprojectionMatrix = ds::matTranspose(_viewProjectionMatrix);
	ds::matrix w = ds::matTranslate(pos);
	//constantBuffer->worldMatrix = ds::matTranspose(w);
	//ds::submit(pass, e.depthDrawItem);
}
