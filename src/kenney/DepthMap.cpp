#include "DepthMap.h"
#include "..\..\shaders\Depth_VS_Main.h"

DepthMap::DepthMap(int dimension, RID inputLayoutId) {

	ds::ShaderInfo dvsInfo = { 0, Depth_VS_Main, sizeof(Depth_VS_Main), ds::ShaderType::ST_VERTEX_SHADER };
	RID depthVertexShader = ds::createShader(dvsInfo);

	ds::RenderTargetInfo rtInfo = { dimension, dimension, ds::Color(0.0f,0.0f,0.0f,1.0f) };
	_renderTargetId = ds::createDepthRenderTarget(rtInfo);

	ds::ViewportInfo depthVpInfo = { dimension, dimension, 0.0f, 1.0f };
	RID depthVp = ds::createViewport(depthVpInfo);
	RID targets[] = { _renderTargetId };
	ds::RenderPassInfo rtrpInfo = { 0, depthVp, ds::DepthBufferState::ENABLED, targets, 1 };
	_renderPass = ds::createRenderPass(rtrpInfo);

	_constantBuffer.viewprojectionMatrix = ds::matTranspose(_viewProjectionMatrix);
	ds::matrix w = ds::matTranslate(ds::vec3(0.0f));
	_constantBuffer.worldMatrix = ds::matTranspose(w);

	RID constantBufferID = ds::createConstantBuffer(sizeof(DepthConstantBuffer), &_constantBuffer);

	_depthGroup = ds::StateGroupBuilder()
		.inputLayout(inputLayoutId)
		.constantBuffer(constantBufferID, depthVertexShader, 0)
		.vertexShader(depthVertexShader)
		.pixelShader(NO_RID)
		.build();

}

void DepthMap::buildView(const ds::vec3 lightPos, const ds::vec3& lightDirection) {
	_lightPos = lightPos;
	_lightDirection = lightDirection;
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

void DepthMap::render(const ds::vec3& pos, RID drawItem) {
	_constantBuffer.viewprojectionMatrix = ds::matTranspose(_viewProjectionMatrix);
	ds::matrix w = ds::matTranslate(pos);
	_constantBuffer.worldMatrix = ds::matTranspose(w);
	ds::submit(_renderPass, drawItem);
}
