#include "ObjViewer.h"
#include <ds_imgui.h>
#include "..\..\shaders\AmbientLightning_VS_Main.h"
#include "..\..\shaders\AmbientLightning_PS_Main.h"
#include "..\utils\tweening.h"
#include "..\..\shaders\RepeatingGrid_VS_Main.h"
#include "..\..\shaders\RepeatingGrid_PS_Main.h"

ObjViewer::ObjViewer() : TestApp() {
}

ObjViewer::~ObjViewer() {
	delete _fpsCamera;
}

ds::RenderSettings ObjViewer::getRenderSettings() {
	ds::RenderSettings rs;
	rs.width = 1024;
	rs.height = 768;
	rs.title = "ObjViewer";
	rs.clearColor = ds::Color(0.01f, 0.01f, 0.01f, 1.0f);
	rs.multisampling = 4;
	rs.useGPUProfiling = false;
	rs.supportDebug = false;
	return rs;
}

// ----------------------------------------------------
// init
// ----------------------------------------------------
bool ObjViewer::init() {
	
	_fpsCamera = new FPSCamera(&_camera);
	_fpsCamera->setPosition(ds::vec3(0, 3, -6), ds::vec3(0.0f, 0.0f, 0.0f));

	RID bumpVS = ds::createVertexShader(AmbientLightning_VS_Main, sizeof(AmbientLightning_VS_Main));
	RID bumpPS = ds::createPixelShader(AmbientLightning_PS_Main, sizeof(AmbientLightning_PS_Main));

	_lightBuffer.ambientColor = ds::Color(0.2f, 0.2f, 0.2f, 1.0f);
	_lightBuffer.diffuseColor = ds::Color(1.0f, 1.0f, 1.0f, 1.0f);
	_lightBuffer.lightDirection = ds::vec3(0.0f, 0.0f, 1.0f);
	_lightBuffer.padding = 0.0f;
	RID lbid = ds::createConstantBuffer(sizeof(InstanceLightBuffer), &_lightBuffer);

	_mesh.loadBin("models\\grid_box.bin", false);
	RID hexCubeBuffer = _mesh.assemble();
	RID layout = _mesh.createInputLayout(bumpVS);
	RID cbid = ds::createConstantBuffer(sizeof(InstanceBuffer), &_constantBuffer);

	RID gridGroup = ds::StateGroupBuilder()
		.inputLayout(layout)
		.blendState(_blendStateID)
		.constantBuffer(lbid, bumpPS, 0)
		.constantBuffer(cbid, bumpVS, 0)
		.vertexBuffer(hexCubeBuffer)
		.vertexShader(bumpVS)
		.pixelShader(bumpPS)
		.build();


	ds::DrawCommand gridDrawCmd = { _mesh.getCount(), ds::DrawType::DT_VERTICES, ds::PrimitiveTypes::TRIANGLE_LIST};

	_gridDrawItem = ds::compile(gridDrawCmd, gridGroup);

	float gridSize = 8.0f;
	ds::vec3 positions[] = {ds::vec3(-gridSize,0.0f,-gridSize),ds::vec3(-gridSize,0.0f,gridSize) ,ds::vec3(gridSize,0.0f, gridSize) ,ds::vec3(gridSize,0.0f,gridSize) ,ds::vec3(gridSize,0.0f,-gridSize) ,ds::vec3(-gridSize,0.0f,-gridSize) };
	_grid.addStream(AttributeType::AT_VERTEX, (float*)positions, 6, 3);
	ds::vec2 uvs[] = { ds::vec2(0.0f,16.0f),ds::vec2(0.0f,0.0f),ds::vec2(16.0f,0.0f),ds::vec2(16.0f,0.0f),ds::vec2(16.0f,16.0f),ds::vec2(0.0f,16.0f) };
	_grid.addStream(AttributeType::AT_UV, (float*)uvs, 6, 2);
	RID gridBuffer = _grid.assemble();
	
	RID gridVS = ds::createVertexShader(RepeatingGrid_VS_Main, sizeof(RepeatingGrid_VS_Main));
	RID gridPS = ds::createPixelShader(RepeatingGrid_PS_Main, sizeof(RepeatingGrid_PS_Main));
	RID gridLayout = _grid.createInputLayout(gridVS);

	RID nextGroup = ds::StateGroupBuilder()
		.inputLayout(gridLayout)
		.blendState(_blendStateID)
		.constantBuffer(cbid, gridVS, 0)
		.vertexBuffer(gridBuffer)
		.vertexShader(gridVS)
		.pixelShader(gridPS)
		.build();


	ds::DrawCommand nextDrawCmd = { _grid.getCount(), ds::DrawType::DT_VERTICES, ds::PrimitiveTypes::TRIANGLE_LIST };

	_nextDrawItem = ds::compile(nextDrawCmd, nextGroup);

	_worldPos = ds::vec3(0.0f);

	_moving = true;
	_moveTimer = 0.0f;
	_startPosition = ds::vec3(0.0f, 10.0f, 0.0f);
	_targetPosition = ds::vec3(0.0f);// _mesh.getCenter();
	_dbgMoveTTL = 0.75f;
	return true;
}

// ----------------------------------------------------
// tick
// ----------------------------------------------------
void ObjViewer::tick(float dt) {
	_fpsCamera->update(dt);
	if (_moving) {
		_worldPos = tweening::interpolate(tweening::easeOutCubic, _startPosition, _targetPosition, _moveTimer, _dbgMoveTTL);
		_moveTimer += dt;
		if (_moveTimer >= _dbgMoveTTL) {
			_moving = false;
		}
	}
}

// ----------------------------------------------------
// render
// ----------------------------------------------------
void ObjViewer::render() {
	_constantBuffer.world = ds::matTranspose(ds::matTranslate(_worldPos));
	_constantBuffer.mvp = ds::matTranspose(_camera.viewProjectionMatrix);

	_constantBuffer.world = ds::matTranspose(ds::matIdentity());
	ds::submit(_basicPass, _nextDrawItem);

	_constantBuffer.world = ds::matTranspose(ds::matTranslate(_worldPos));
	ds::submit(_basicPass, _gridDrawItem);
}

// ----------------------------------------------------
// renderGUI
// ----------------------------------------------------
void ObjViewer::renderGUI() {
	int state = 1;
	gui::start();
	p2i sp = p2i(10, 760);
	if (gui::begin("Debug", &state, &sp, 300)) {
		gui::Value("FPS", ds::getFramesPerSecond());
		gui::Value("Center", _mesh.getCenter());
		gui::Value("Extent", _mesh.getExtent());
		gui::Value("WorldPos", &_worldPos);
		gui::Input("Move TTL", &_dbgMoveTTL);
		if (gui::Button("Center")) {
			_worldPos = _mesh.getCenter() * -1.0f;
		}
		if (gui::Button("Reset Camera")) {
			_fpsCamera->setPosition(ds::vec3(0, 0, -6));
		}
		if (gui::Button("Move")) {
			_moveTimer = 0.0f;
			_moving = true;
		}
	}
	gui::end();
}