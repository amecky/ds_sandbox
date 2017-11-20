#include "WarpingGrid.h"
#include <ds_imgui.h>
#include "..\..\shaders\WarpingGrid_VS_Main.h"
#include "..\..\shaders\WarpingGrid_PS_Main.h"

void wgLogHandler(const LogLevel&, const char* message) {
	OutputDebugString(message);
	OutputDebugString("\n");
}

WarpingGrid::WarpingGrid() {

}

WarpingGrid::~WarpingGrid() {

}

ds::RenderSettings WarpingGrid::getRenderSettings() {
	ds::RenderSettings rs;
	rs.width = 1024;
	rs.height = 768;
	rs.title = "Warping grid";
	rs.clearColor = ds::Color(0.01f, 0.01f, 0.01f, 1.0f);
	rs.multisampling = 4;
	rs.useGPUProfiling = false;
	rs.supportDebug = true;
	rs.logHandler = wgLogHandler;
	return rs;
}

bool WarpingGrid::init() {
	_vertices = new WarpingGridVertex[TOTAL_GRID_SIZE];
	for (int y = 0; y < GRID_SIZE_Y; ++y) {
		for (int x = 0; x < GRID_SIZE_X; ++x) {
			WarpingGridVertex& vp = _vertices[x + y * GRID_SIZE_X];
			vp.pos = ds::vec3((float)x, 1.0f - y,  ds::random(-0.4f, 0.4f));
			vp.n = ds::vec3(0, 0, -1);
			vp.color = ds::Color(192, 0, 0, 255);
		}
	}

	int cnt = 0;
	int sz = (GRID_SIZE_X - 1) * (GRID_SIZE_Y - 1) * 6;
	_indices = new uint16_t[sz];
	for (int y = 0; y < GRID_SIZE_Y - 1; ++y) {
		for (int x = 0; x < GRID_SIZE_X - 1; ++x) {
			int idx = x + y * GRID_SIZE_X;
			_indices[cnt++] = idx;
			_indices[cnt++] = idx + 1;
			_indices[cnt++] = idx + GRID_SIZE_X;
			_indices[cnt++] = idx + 1;
			_indices[cnt++] = idx + GRID_SIZE_X + 1;
			_indices[cnt++] = idx + GRID_SIZE_X;
		}
	}
	
	int tris = sz / 3;
	for (int i = 0; i < tris; ++i) {
		ds::vec3 v0 = _vertices[_indices[i * 3]].pos;
		ds::vec3 v1 = _vertices[_indices[i * 3 + 1]].pos;
		ds::vec3 v2 = _vertices[_indices[i * 3 + 2]].pos;
		ds::vec3 n = normalize(cross(v0 - v1, v0 - v2));
		_vertices[_indices[i * 3]].n= n;
		_vertices[_indices[i * 3 + 1]].n = n;
		_vertices[_indices[i * 3 + 2]].n = n;
	}
	
	ds::matrix viewMatrix = ds::matLookAtLH(ds::vec3(0.0f, 1.0f, -3.0f), ds::vec3(0, 0, 0), ds::vec3(0, 1, 0));
	ds::matrix projectionMatrix = ds::matPerspectiveFovLH(ds::PI / 4.0f, ds::getScreenAspectRatio(), 0.01f, 100.0f);
	_camera = {
		viewMatrix,
		projectionMatrix,
		viewMatrix * projectionMatrix,
		ds::vec3(0,1,-3),
		ds::vec3(0,0,1),
		ds::vec3(0,1,0),
		ds::vec3(1,0,0),
		0.0f,
		0.0f,
		0.0f
	};

	ds::ViewportInfo vpInfo = { 1024, 768, 0.0f, 1.0f };
	RID vp = ds::createViewport(vpInfo);

	ds::RenderPassInfo rpInfo = { &_camera, vp, ds::DepthBufferState::ENABLED, 0, 0 };
	_basicPass = ds::createRenderPass(rpInfo);

	ds::BlendStateInfo blendInfo = { ds::BlendStates::SRC_ALPHA, ds::BlendStates::SRC_ALPHA, ds::BlendStates::INV_SRC_ALPHA, ds::BlendStates::INV_SRC_ALPHA, true };
	RID bs_id = ds::createBlendState(blendInfo);

	ds::ShaderInfo bumpVSInfo = { 0, WarpingGrid_VS_Main, sizeof(WarpingGrid_VS_Main), ds::ShaderType::ST_VERTEX_SHADER };
	RID bumpVS = ds::createShader(bumpVSInfo);
	ds::ShaderInfo bumpPSInfo = { 0, WarpingGrid_PS_Main, sizeof(WarpingGrid_PS_Main), ds::ShaderType::ST_PIXEL_SHADER };
	RID bumpPS = ds::createShader(bumpPSInfo);

	_lightPos = ds::vec3(0.0f, 0.0f, -6.0f);

	ds::InputLayoutDefinition decl[] = {
		{ "POSITION", 0, ds::BufferAttributeType::FLOAT3 },
		{ "COLOR"   , 0, ds::BufferAttributeType::FLOAT4 },
		{ "NORMAL"  , 0, ds::BufferAttributeType::FLOAT3 }
	};
	ds::InputLayoutInfo ilInfo = { decl, 3, bumpVS };
	RID rid = ds::createInputLayout(ilInfo);

	_constantBuffer.mvp = ds::matTranspose(_camera.viewProjectionMatrix);
	_constantBuffer.world = ds::matTranspose(ds::matIdentity());
	
	RID cbid = ds::createConstantBuffer(sizeof(InstanceBuffer), &_constantBuffer);
	ds::IndexBufferInfo ibInfo = { sz,ds::IndexType::UINT_16,ds::BufferType::STATIC,_indices };
	RID indexBuffer = ds::createIndexBuffer(ibInfo);

	ds::VertexBufferInfo vbInfo = { ds::BufferType::DYNAMIC,TOTAL_GRID_SIZE,sizeof(WarpingGridVertex),_vertices };
	_cubeBuffer = ds::createVertexBuffer(vbInfo);

	_lightBuffer.ambientColor = ds::Color(0.1f, 0.1f, 0.1f, 1.0f);
	_lightBuffer.diffuseColor = ds::Color(1.0f, 1.0f, 1.0f, 1.0f);
	_lightBuffer.lightDirection = ds::vec3(1.0f, 0.0f, 1.0f);
	_lightBuffer.padding = 0.0f;
	RID lbid = ds::createConstantBuffer(sizeof(InstanceLightBuffer), &_lightBuffer);

	ds::SamplerStateInfo samplerInfo = { ds::TextureAddressModes::CLAMP, ds::TextureFilters::LINEAR };
	RID ssid = ds::createSamplerState(samplerInfo);

	_fpsCamera = new FPSCamera(&_camera);
	_fpsCamera->setPosition(ds::vec3(0, 0, -5), ds::vec3(0.0f, 0.0f, 0.0f));

	RID stateGroup = ds::StateGroupBuilder()
		.inputLayout(rid)
		.constantBuffer(cbid, bumpVS, 0)
		.constantBuffer(lbid, bumpPS, 0)
		.blendState(bs_id)
		.samplerState(ssid, bumpPS)
		.vertexBuffer(_cubeBuffer)
		.vertexShader(bumpVS)
		.pixelShader(bumpPS)
		.indexBuffer(indexBuffer)
		.build();


	ds::DrawCommand drawCmd = { sz, ds::DrawType::DT_INDEXED, ds::PrimitiveTypes::TRIANGLE_LIST };

	_drawItem = ds::compile(drawCmd, stateGroup);
	return true;
}

void WarpingGrid::tick(float dt) {
	_fpsCamera->update(dt);
	
}

void WarpingGrid::render() {

	_constantBuffer.mvp = ds::matTranspose(_camera.viewProjectionMatrix);

	ds::mapBufferData(_cubeBuffer, _vertices, sizeof(WarpingGridVertex) * TOTAL_GRID_SIZE);
	ds::submit(_basicPass, _drawItem);
}

void WarpingGrid::renderGUI() {
	int state = 1;
	gui::start();
	p2i sp = p2i(10, 760);
	if (gui::begin("Debug", &state, &sp, 200)) {
		gui::Value("FPS", ds::getFramesPerSecond());
	}
	gui::end();
}