#include "InstanceTest.h"
#include "..\Mesh.h"
#include "..\..\shaders\InstancedAmbient_VS_Main.h"
#include "..\..\shaders\InstancedAmbient_PS_Main.h"
#include <ds_imgui.h>

InstanceTest::InstanceTest() {
}

InstanceTest::~InstanceTest() {
	delete _queue;
}

// ----------------------------------------------------
// get render settings
// ----------------------------------------------------
ds::RenderSettings InstanceTest::getRenderSettings() {
	ds::RenderSettings rs;
	rs.width = 1024;
	rs.height = 768;
	rs.title = "Instancing";
	rs.clearColor = ds::Color(0.01f, 0.01f, 0.01f, 1.0f);
	rs.multisampling = 4;
	rs.useGPUProfiling = false;
	rs.supportDebug = true;
	
	return rs;
}

// ----------------------------------------------------
// init
// ----------------------------------------------------
bool InstanceTest::init() {
	ds::matrix viewMatrix = ds::matLookAtLH(ds::vec3(0.0f, 0.0f, -3.0f), ds::vec3(0, 0, 0), ds::vec3(0, 1, 0));
	ds::matrix projectionMatrix = ds::matPerspectiveFovLH(ds::PI / 4.0f, ds::getScreenAspectRatio(), 0.01f, 100.0f);
	_camera = {
		viewMatrix,
		projectionMatrix,
		viewMatrix * projectionMatrix,
		ds::vec3(0,0,-3),
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

	ds::ShaderInfo bumpVSInfo = { 0, InstancedAmbient_VS_Main, sizeof(InstancedAmbient_VS_Main), ds::ShaderType::ST_VERTEX_SHADER };
	RID bumpVS = ds::createShader(bumpVSInfo);
	ds::ShaderInfo bumpPSInfo = { 0, InstancedAmbient_PS_Main, sizeof(InstancedAmbient_PS_Main), ds::ShaderType::ST_PIXEL_SHADER };
	RID bumpPS = ds::createShader(bumpPSInfo);	

	ds::InputLayoutDefinition decl[] = {
		{ "POSITION", 0, ds::BufferAttributeType::FLOAT3 },
		{ "COLOR"   , 0, ds::BufferAttributeType::FLOAT4 },
		{ "NORMAL"  , 0, ds::BufferAttributeType::FLOAT3 }		
	};

	ds::InstancedInputLayoutDefinition instDecl[] = {
		{ "WORLD", 0, ds::BufferAttributeType::FLOAT4 },
		{ "WORLD", 1, ds::BufferAttributeType::FLOAT4 },
		{ "WORLD", 2, ds::BufferAttributeType::FLOAT4 },
		{ "WORLD", 3, ds::BufferAttributeType::FLOAT4 },
		{ "COLOR", 1, ds::BufferAttributeType::FLOAT4 }
	};
	ds::InstancedInputLayoutInfo iilInfo = { decl, 3, instDecl, 5, bumpVS };
	RID rid = ds::createInstancedInputLayout(iilInfo);

	//ds::VertexBufferInfo ibInfo = { ds::BufferType::DYNAMIC, TOTAL, sizeof(InstanceData) };
	//_instanceVertexBuffer = ds::createVertexBuffer(ibInfo);

	//RID instanceBuffer = ds::createInstancedBuffer(cubeBuffer, _instanceVertexBuffer);

	_fpsCamera = new FPSCamera(&_camera);
	_fpsCamera->setPosition(ds::vec3(0, 0, -5), ds::vec3(0.0f, 0.0f, 0.0f));

	RID baseGroup = ds::StateGroupBuilder()
		.inputLayout(rid)		
		.blendState(bs_id)
		.vertexShader(bumpVS)
		.pixelShader(bumpPS)
		.build();

	_grid.init(baseGroup, bumpVS, bumpPS);

	_cubes.init(baseGroup, bumpVS, bumpPS);

	_cubes.create(ds::vec3(0.0f), 2);

	_queue = new EmitterQueue(&_grid, &_cubes);

	_tmpX = 7;
	_tmpY = 8;
	_ttl = 0.4f;
	return true;
}

// ----------------------------------------------------
// tick
// ----------------------------------------------------
void InstanceTest::tick(float dt) {

	_fpsCamera->update(dt);
	
	_cubes.tick(dt);

	_grid.tick(dt);

	_queue->tick(dt);
}

// ----------------------------------------------------
// render
// ----------------------------------------------------
void InstanceTest::render() {
	
	_cubes.render(_basicPass, _camera.viewProjectionMatrix);
	
	_grid.render(_basicPass, _camera.viewProjectionMatrix);

	ds::dbgPrint(0, 0, "FPS: %d", ds::getFramesPerSecond());
}

void InstanceTest::renderGUI() {
	int state = 1;
	gui::start();
	p2i sp = p2i(10, 760);
	if (gui::begin("Debug", &state, &sp, 300)) {
		gui::Value("FPS", ds::getFramesPerSecond());
		/*
		if (gui::Button("Scale")) {
			_items[5].animations[0].timer = 0.0f;
		}
		if (gui::Button("Fade in")) {
			_items[5].animations[1].timer = 0.0f;
		}
		if (gui::Button("Both")) {
			_items[5].animations[0].timer = 0.0f;
			_items[5].animations[1].timer = 0.0f;
			_grid.highlight(12, 8, 0.4f, BGF_ONE);
		}
		*/
		gui::Input("GX", &_tmpX);
		gui::Input("GY", &_tmpY);
		gui::Input("Flash TTL", &_ttl);
		if (gui::Button("Flash ONE")) {
			_grid.highlight(_tmpX, _tmpY, _ttl, BGF_ONE);
		}
		if (gui::Button("Flash PULSE")) {
			_grid.highlight(_tmpX, _tmpY, _ttl, BGF_PULSE);
		}
		if (gui::Button("Emitt")) {
			_queue->emitt(_tmpX, _tmpY);
		}
		if (gui::Button("Reset Camera")) {
			_fpsCamera->setPosition(ds::vec3(0, 0, -5), ds::vec3(0.0f, 0.0f, 0.0f));
		}
	}
	gui::end();
}