#include "RTViewer.h"
#include "..\..\shaders\Textured_VS_Main.h"
#include "..\..\shaders\Textured_PS_Main.h"

RenderTextureViewer::RenderTextureViewer(RID rtID, int size) {
	ds::BlendStateInfo bsInfo = { ds::BlendStates::SRC_ALPHA, ds::BlendStates::SRC_ALPHA, ds::BlendStates::INV_SRC_ALPHA, ds::BlendStates::INV_SRC_ALPHA, true };
	RID bs_id = ds::createBlendState(bsInfo);

	ds::ShaderInfo vsInfo = { 0, Textured_VS_Main, sizeof(Textured_VS_Main), ds::ShaderType::ST_VERTEX_SHADER };
	RID vertexShader = ds::createShader(vsInfo);
	ds::ShaderInfo psInfo = { 0, Textured_PS_Main, sizeof(Textured_PS_Main), ds::ShaderType::ST_PIXEL_SHADER };
	RID pixelShader = ds::createShader(psInfo);

	ds::vec2 uvs[] = { ds::vec2(0.0f,1.0f),ds::vec2(0.0f,0.0f),ds::vec2(1.0f,0.0f),ds::vec2(1.0f,1.0f) };
	float xp = -1.0f * ds::getScreenWidth() / 2.0f;
	float yp = -1.0f * ds::getScreenHeight() / 2.0f;
	ds::vec3 positions[] = { ds::vec3(xp,yp,0.0f),ds::vec3(xp,yp + size,0.0f) ,ds::vec3(xp + size,yp + size,0.0f) ,ds::vec3(xp + size,yp,0.0f) };
	_vertices = new RTVertex[4];
	for (int j = 0; j < 4; ++j) {
		_vertices[j].p = positions[j];
		_vertices[j].t = uvs[j];
	}

	ds::InputLayoutDefinition ambientDecl[] = {
		{ "POSITION", 0, ds::BufferAttributeType::FLOAT3 },
		{ "TEXCOORD"   , 0, ds::BufferAttributeType::FLOAT2 }
	};

	ds::InputLayoutInfo ambientLayoutInfo = { ambientDecl, 2, vertexShader };
	RID arid = ds::createInputLayout(ambientLayoutInfo);

	RID indexBuffer = ds::createQuadIndexBuffer(1, "RTIndexBuffer");
	ds::VertexBufferInfo vbInfo = { ds::BufferType::STATIC, 4, sizeof(RTVertex), _vertices };
	RID kvbid = ds::createVertexBuffer(vbInfo);

	ds::matrix orthoView = ds::matIdentity();
	ds::matrix orthoProjection = ds::matOrthoLH(ds::getScreenWidth(), ds::getScreenHeight(), 0.0f, 1.0f);
	_vpMatrix = orthoView * orthoProjection;
	_worldMatrix = ds::matIdentity();

	RID constantBufferID = ds::createConstantBuffer(sizeof(RTVConstantBuffer), &_constantBuffer);

	RID nextGroup = ds::StateGroupBuilder()
		.inputLayout(arid)
		.constantBuffer(constantBufferID, vertexShader, 0)
		.blendState(bs_id)
		.textureFromRenderTarget(rtID, pixelShader, 0)
		.vertexShader(vertexShader)
		.pixelShader(pixelShader)
		.vertexBuffer(kvbid)
		.indexBuffer(indexBuffer)
		.build();

	ds::DrawCommand nextDrawCmd = { 6, ds::DrawType::DT_INDEXED, ds::PrimitiveTypes::TRIANGLE_LIST };

	_drawItem = ds::compile(nextDrawCmd, nextGroup);
}

RenderTextureViewer::~RenderTextureViewer() {
	delete[] _vertices;
}

void RenderTextureViewer::render(RID renderPass) {
	_constantBuffer.viewprojectionMatrix = ds::matTranspose(_vpMatrix);
	_constantBuffer.worldMatrix = ds::matTranspose(_worldMatrix);
	ds::submit(renderPass, _drawItem);
}
/*
RID create_rt_view_draw_item(RID constantBuffer, RID rtID) {

	ds::BlendStateInfo bsInfo = { ds::BlendStates::SRC_ALPHA, ds::BlendStates::SRC_ALPHA, ds::BlendStates::INV_SRC_ALPHA, ds::BlendStates::INV_SRC_ALPHA, true };
	RID bs_id = ds::createBlendState(bsInfo);

	ds::ShaderInfo vsInfo = { 0, Textured_VS_Main, sizeof(Textured_VS_Main), ds::ShaderType::ST_VERTEX_SHADER };
	RID vertexShader = ds::createShader(vsInfo);
	ds::ShaderInfo psInfo = { 0, Textured_PS_Main, sizeof(Textured_PS_Main), ds::ShaderType::ST_PIXEL_SHADER };
	RID pixelShader = ds::createShader(psInfo);

	ds::vec2 uvs[] = { ds::vec2(0.0f,1.0f),ds::vec2(0.0f,0.0f),ds::vec2(1.0f,0.0f),ds::vec2(1.0f,1.0f) };
	float xp = -512;
	float yp = -384;
	float size = 400;
	ds::vec3 positions[] = { ds::vec3(xp,yp,0.0f),ds::vec3(xp,yp+size,0.0f) ,ds::vec3(xp+size,yp+size,0.0f) ,ds::vec3(xp+size,yp,0.0f) };
	RTVertex* vertices = new RTVertex[4];
	for (int j = 0; j < 4; ++j) {
		vertices[j].p = positions[j];
		vertices[j].t = uvs[j];
	}

	ds::InputLayoutDefinition ambientDecl[] = {
		{ "POSITION", 0, ds::BufferAttributeType::FLOAT3 },
		{ "TEXCOORD"   , 0, ds::BufferAttributeType::FLOAT2 }
	};

	ds::InputLayoutInfo ambientLayoutInfo = { ambientDecl, 2, vertexShader };
	RID arid = ds::createInputLayout(ambientLayoutInfo);

	RID indexBuffer = ds::createQuadIndexBuffer(1, "RTIndexBuffer");
	ds::VertexBufferInfo vbInfo = { ds::BufferType::STATIC, 4, sizeof(RTVertex), vertices };
	RID kvbid = ds::createVertexBuffer(vbInfo);

	ds::matrix orthoView = ds::matIdentity();
	ds::matrix orthoProjection = ds::matOrthoLH(ds::getScreenWidth(), ds::getScreenHeight(), 0.0f, 1.0f);

	RID nextGroup = ds::StateGroupBuilder()
		.inputLayout(arid)
		.constantBuffer(constantBuffer, vertexShader, 0)
		.blendState(bs_id)
		.textureFromRenderTarget(rtID,pixelShader,0)
		.vertexShader(vertexShader)
		.pixelShader(pixelShader)
		.vertexBuffer(kvbid)
		.indexBuffer(indexBuffer)
		.build();

	ds::DrawCommand nextDrawCmd = { 6, ds::DrawType::DT_INDEXED, ds::PrimitiveTypes::TRIANGLE_LIST };
	return ds::compile(nextDrawCmd, nextGroup);

}
*/