#include "DepthMap.h"
#include "..\..\shaders\Depth_VS_Main.h"
#include <D3dx9math.h>

DepthMap::DepthMap(int dimension) {

	_sceneRadius = 7.0f;
	ds::ShaderInfo dvsInfo = { 0, Depth_VS_Main, sizeof(Depth_VS_Main), ds::ShaderType::ST_VERTEX_SHADER };
	RID depthVertexShader = ds::createShader(dvsInfo, "DepthVS");

	ds::RenderTargetInfo rtInfo = { dimension, dimension, ds::Color(0.0f,0.0f,0.0f,1.0f) };
	_renderTargetId = ds::createDepthRenderTarget(rtInfo, "DepthRT");

	ds::ViewportInfo depthVpInfo = { dimension, dimension, 0.0f, 1.0f };
	RID depthVp = ds::createViewport(depthVpInfo);
	RID targets[] = { _renderTargetId };
	ds::RenderPassInfo rtrpInfo = { 0, depthVp, ds::DepthBufferState::ENABLED, targets, 1 };
	_renderPass = ds::createRenderPass(rtrpInfo, "DepthRenderPass");

	_constantBuffer.viewprojectionMatrix = ds::matTranspose(_viewProjectionMatrix);
	ds::matrix w = ds::matTranslate(ds::vec3(0.0f));
	_constantBuffer.worldMatrix = ds::matTranspose(w);

	RID constantBufferID = ds::createConstantBuffer(sizeof(DepthConstantBuffer), &_constantBuffer);

	ds::InputLayoutDefinition ambientDecl[] = {
		{ "POSITION", 0, ds::BufferAttributeType::FLOAT3 },
		{ "COLOR"   , 0, ds::BufferAttributeType::FLOAT4 },
		{ "NORMAL"   , 0, ds::BufferAttributeType::FLOAT3 }
	};

	ds::InputLayoutInfo ambientLayoutInfo = { ambientDecl, 3, depthVertexShader };
	RID arid = ds::createInputLayout(ambientLayoutInfo);

	ds::RasterizerStateInfo backRasterInfo = { ds::CullMode::FRONT,ds::FillMode::SOLID,true,false,0.0f,0.0f };
	RID backRSS = ds::createRasterizerState(backRasterInfo, "DepthRasterizerState");

	_depthGroup = ds::StateGroupBuilder()
		.inputLayout(arid)
		.constantBuffer(constantBufferID, depthVertexShader, 0)
		//.rasterizerState(backRSS)
		.vertexShader(depthVertexShader)
		.pixelShader(NO_RID)
		.build("DepthBaseGroup");

}

void DepthMap::buildView(const ds::vec3& lightDirection) {
	_lightPos = -2.0f * _sceneRadius * lightDirection;
	_lightDirection = lightDirection;
	ds::matrix V = ds::matLookAtLH(_lightPos, ds::vec3(0, 0, 0), ds::vec3(0, 1, 0));
	ds::matrix P = ds::matOrthoOffCenterLH(-_sceneRadius, _sceneRadius, -_sceneRadius, _sceneRadius, -20.0f, 20.0f);
	_viewProjectionMatrix = V * P;
	ds::matrix ts = ds::matScale(ds::vec3(0.5f, -0.5f, 1.0f));
	ds::matrix tt = ds::matTranslate(ds::vec3(0.5f, 0.5f, 0.0f));
	ds::matrix T = ts * tt;
	_shadowTransformMatrix = V * P * T;
}

void DepthMap::render(const ds::vec3& pos, RID drawItem) {
	_constantBuffer.viewprojectionMatrix = ds::matTranspose(_viewProjectionMatrix);
	ds::matrix w = ds::matTranslate(pos);
	_constantBuffer.worldMatrix = ds::matTranspose(w);
	ds::submit(_renderPass, drawItem);
}
