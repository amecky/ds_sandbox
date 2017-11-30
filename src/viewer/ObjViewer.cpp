#include "ObjViewer.h"
#include <ds_imgui.h>
#include "..\..\shaders\AmbientMultipleLightning_VS_Main.h"
#include "..\..\shaders\AmbientMultipleLightning_PS_Main.h"
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

	RID bumpVS = ds::createVertexShader(AmbientMultipleLightning_VS_Main, sizeof(AmbientMultipleLightning_VS_Main));
	RID bumpPS = ds::createPixelShader(AmbientMultipleLightning_PS_Main, sizeof(AmbientMultipleLightning_PS_Main));

	for (int i = 0; i < 3; ++i) {
		_lightBuffer.lights[i].ambientColor = ds::Color(0.1f, 0.1f, 0.1f, 1.0f);
		_lightBuffer.lights[i].diffuseColor = ds::Color(1.0f, 1.0f, 1.0f, 1.0f);
		_lightBuffer.lights[i].lightDirection = ds::vec3(0.0f, 0.0f, 1.0f);
		_lightBuffer.lights[i].padding = 0.0f;
	}
	_lightBuffer.lights[1].lightDirection = ds::vec3(-1.0f, 0.0f, 1.0f);
	_lightBuffer.lights[2].lightDirection = ds::vec3(1.0f, 0.0f, 1.0f);

	_lightPos[0] = ds::vec3(1.0f, 0.5f, 1.0f);
	_lightPos[1] = ds::vec3(-1.0f, 0.5f, 1.0f);
	_lightPos[2] = ds::vec3(0.0f, 0.5f, 1.0f);


	RID lbid = ds::createConstantBuffer(sizeof(MultipleLightsBuffer), &_lightBuffer);

	//_mesh.loadBin("models\\grid_box.bin", false);
	_mesh.loadBin("models\\ship.bin", false);
	RID hexCubeBuffer = _mesh.assemble();
	RID layout = _mesh.createInputLayout(bumpVS);
	RID cbid = ds::createConstantBuffer(sizeof(InstanceBuffer), &_constantBuffer);

	RID objGroup = ds::StateGroupBuilder()
		.inputLayout(layout)
		.blendState(_blendStateID)
		.constantBuffer(lbid, bumpPS, 0)
		.constantBuffer(cbid, bumpVS, 0)
		.vertexBuffer(hexCubeBuffer)
		.vertexShader(bumpVS)
		.pixelShader(bumpPS)
		.build();


	ds::DrawCommand objDrawCmd = { _mesh.getCount(), ds::DrawType::DT_VERTICES, ds::PrimitiveTypes::TRIANGLE_LIST};

	_objDrawItem = ds::compile(objDrawCmd, objGroup);

	_grid = new SimpleGrid;
	_grid->init(8.0f, 0.7f, 0.2f);

	_worldPos = ds::vec3(0.0f);

	_moving = true;
	_moveTimer = 0.0f;
	_startPosition = ds::vec3(0.0f, 10.0f, 0.0f);
	_targetPosition = ds::vec3(0.0f);
	_targetPosition.y = _mesh.getMin().y * -1.0f;
	_dbgMoveTTL = 1.5f;
	_scale = ds::vec3(1.0f);
	return true;
}

// ----------------------------------------------------
// tick
// ----------------------------------------------------
void ObjViewer::tick(float dt) {
	_fpsCamera->update(dt);
	if (_moving) {
		_worldPos = tweening::interpolate(tweening::easeOutBounce, _startPosition, _targetPosition, _moveTimer, _dbgMoveTTL);
		_moveTimer += dt;
		if (_moveTimer >= _dbgMoveTTL) {
			_moving = false;
			_worldPos = _targetPosition;
		}
	}
}

// ----------------------------------------------------
// render
// ----------------------------------------------------
void ObjViewer::render() {
	for (int i = 0; i < 3; ++i) {
		_lightBuffer.lights[i].lightDirection = normalize(_lightPos[i]);
	}
	_grid->render(_basicPass, _camera.viewProjectionMatrix);
	ds::matrix scaleMatrix = ds::matScale(_scale);
	ds::matrix translationMatrix = ds::matTranslate(_worldPos);
	ds::matrix world = scaleMatrix * translationMatrix;
	_constantBuffer.world = ds::matTranspose(world);
	_constantBuffer.mvp = ds::matTranspose(_camera.viewProjectionMatrix);

	ds::submit(_basicPass, _objDrawItem);

	
}

// ----------------------------------------------------
// renderGUI
// ----------------------------------------------------
void ObjViewer::renderGUI() {
	int state = 1;
	gui::start();
	p2i sp = p2i(10, 760);
	if (gui::begin("Object", &state, &sp, 320)) {
		gui::Value("FPS", ds::getFramesPerSecond());
		gui::Value("Center", _mesh.getCenter());
		gui::Value("Extent", _mesh.getExtent());
		gui::Value("Min", _mesh.getMin());
		gui::Value("Max", _mesh.getMax());
		gui::Input("WorldPos", &_worldPos);
		for (int i = 0; i < 3; ++i) {
			gui::Input("LightDir", &_lightPos[i]);
		}
		if (gui::Input("Scale", &_scale)) {
			_worldPos.y = _mesh.getMin().y * -1.0f * _scale.y;
		}
		gui::Input("Move TTL", &_dbgMoveTTL);
		if (gui::Button("Center")) {
			_worldPos = _mesh.getCenter() * -1.0f;
		}
		if (gui::Button("Reset Camera")) {
			_fpsCamera->setPosition(ds::vec3(0, 3, -6), ds::vec3(0.0f, 0.0f, 0.0f));
		}
		if (gui::Button("Move")) {
			_moveTimer = 0.0f;
			_moving = true;
		}
	}
	gui::end();
}