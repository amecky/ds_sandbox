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
	delete _material;
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

	_material = new AmbientLightningMaterial;

	_lightDir[0] = ds::vec3(1.0f, 0.5f, 1.0f);
	_lightDir[1] = ds::vec3(-1.0f, 0.5f, 1.0f);
	_lightDir[2] = ds::vec3(0.0f, 0.5f, 1.0f);
	
	//_mesh.loadData("outer_ring");
	_mesh.loadData("grid_box");
	//_mesh.loadObj("obj", "grid_box.obj");
	//_mesh.loadObj("obj", "outer_ring.obj");
	//_mesh.save("models", "outer_ring.bin");
	//_mesh.loadBin("models\\grid_box.bin", false);
	//_mesh.loadBin("models\\ship.bin", false);
	RID hexCubeBuffer = _mesh.assemble();
	RID layout = _mesh.createInputLayout(_material->getVertexShaderID());

	RID objGroup = ds::StateGroupBuilder()
		.inputLayout(layout)
		.vertexBuffer(hexCubeBuffer)
		.build();


	ds::DrawCommand objDrawCmd = { _mesh.getCount(), ds::DrawType::DT_VERTICES, ds::PrimitiveTypes::TRIANGLE_LIST};

	RID groups[] = { objGroup,_material->getBasicGroupID() };
	_objDrawItem = ds::compile(objDrawCmd, groups, 2);

	_grid = new SimpleGrid;
	_grid->init(8.0f, 0.7f, 0.2f);

	_transform = { ds::vec3(0.0f),ds::vec3(1.0f),ds::vec3(0.0f),{0.0f,0.0f,0.0f}};

	_moving = true;
	_startPosition = ds::vec3(0.0f, 10.0f, 0.0f);
	_targetPosition = ds::vec3(0.0f);
	_targetPosition.y = _mesh.getMin().y * -1.0f;
	_dbgMoveTTL = 1.5f;
	_scale = ds::vec3(1.0f);

	_dbgRotating = false;
	_dbgTTL = 0.4f;
	return true;
}

// ----------------------------------------------------
// tick
// ----------------------------------------------------
void ObjViewer::tick(float dt) {
	_fpsCamera->update(dt);
	if (_moving) {
		_moving = move_to(_transform, _startPosition, _targetPosition, dt, _dbgMoveTTL);
	}
	if (_dbgRotating) {
		move_to(_transform, _startPosition, _targetPosition, dt, _dbgTTL);
		_dbgRotating = rotate_to(_transform, ds::vec3(0.0f, 0.0f, 0.0f), ds::vec3(0.0f, 0.0f, ds::PI * -0.5f), dt, _dbgTTL);
	}

}

// ----------------------------------------------------
// render
// ----------------------------------------------------
void ObjViewer::render() {	
	_grid->render(_basicPass, _camera.viewProjectionMatrix);
	ds::matrix world;
	build_world_matrix(_transform, &world);
	for (int i = 0; i < 3; ++i) {
		_material->setLightDirection(i,_lightDir[i]);
	}
	_material->apply();
	_material->transform(world, _camera.viewProjectionMatrix);
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
		gui::Input("WorldPos", &_transform.position);
		for (int i = 0; i < 3; ++i) {
			gui::Input("LightDir", &_lightDir[i]);
		}
		if (gui::Input("Scale", &_scale)) {
			_transform.position.y = _mesh.getMin().y * -1.0f * _scale.y;
		}
		gui::Input("Move TTL", &_dbgMoveTTL);
		if (gui::Button("Center")) {
			_transform.position = _mesh.getCenter() * -1.0f;
		}
		if (gui::Button("Reset Camera")) {
			_fpsCamera->setPosition(ds::vec3(0, 3, -6), ds::vec3(0.0f, 0.0f, 0.0f));
		}
		if (gui::Button("Move")) {
			_transform.timer[0] = 0.0f;
			_startPosition = ds::vec3(0.0f, 10.0f, 0.0f);
			_targetPosition = ds::vec3(0.0f);
			_targetPosition.y = _mesh.getMin().y * -1.0f;
			_moving = true;
		}
		if (gui::Button("Test Move")) {
			_transform.timer[0] = 0.0f;
			_transform.timer[2] = 0.0f;
			_startPosition = _transform.position;
			_targetPosition = _startPosition + ds::vec3(1.0f, 0.0f, 0.0f);
			_targetPosition.y = _mesh.getMin().y * -1.0f;
			_dbgRotating = true;
		}
	}
	gui::end();
}