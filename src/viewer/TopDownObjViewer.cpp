#include "TopDownObjViewer.h"
#include <ds_imgui.h>
#include "..\..\shaders\AmbientMultipleLightning_VS_Main.h"
#include "..\..\shaders\AmbientMultipleLightning_PS_Main.h"
#include "..\utils\tweening.h"
#include "..\..\shaders\RepeatingGrid_VS_Main.h"
#include "..\..\shaders\RepeatingGrid_PS_Main.h"

TopDownObjViewer::TopDownObjViewer() : TestApp() {
}

TopDownObjViewer::~TopDownObjViewer() {
	delete _fpsCamera;
	delete _material;
}

ds::RenderSettings TopDownObjViewer::getRenderSettings() {
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
bool TopDownObjViewer::init() {
	
	_fpsCamera = new TopDownCamera(&_camera);
	_fpsCamera->setPosition(ds::vec3(0, 0, -12), ds::vec3(0.0f, 0.0f, 0.0f));

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
	_grid->init(SGD_XY, 9.0f, 0.7f, 0.2f);

	_transform = { ds::vec3(0.0f),ds::vec3(1.0f),ds::vec3(0.0f),{0.0f,0.0f,0.0f}};
	_state = ObjectState::MOVING;

	_moving = true;
	_startPosition = ds::vec3(0.0f, 0.0f, -10.0f);
	_targetPosition = ds::vec3(0.0f);
	_targetPosition.z = _mesh.getExtent().y * -0.5f;
	_dbgMoveTTL = 1.5f;

	_dbgRotating = false;
	_dbgRotation = ds::vec3(0.0f);
	_dbgTTL = 0.4f;
	_steps = 0;
	_stepCounter = 10;
	return true;
}

// ----------------------------------------------------
// tick
// ----------------------------------------------------
void TopDownObjViewer::tick(float dt) {
	_fpsCamera->update(dt);
	if (_state == ObjectState::MOVING) {
		if (!move_to(_transform, _startPosition, _targetPosition, dt, _dbgMoveTTL)) {
			_state = ObjectState::IDLE;
		}
	}
	else if (_state == ObjectState::STEPPING) {
		step_forward(_transform, _startPosition, _targetPosition, dt, _dbgTTL);
		if (!rotate_to(_transform, ds::vec3(0.0f, _transform.rotation.y, 0.0f), ds::vec3(0.0f, _transform.rotation.y, ds::PI * -0.5f), dt, _dbgTTL)) {
			_state = ObjectState::IDLE;
		}
	}
	else if (_state == ObjectState::ROTATING) {
		if (!rotate_to(_transform, ds::vec3(_transform.rotation.x, _startAngle, _transform.rotation.z), ds::vec3(_transform.rotation.x, _endAngle, _transform.rotation.z), dt, _dbgTTL)) {
			if (_steps > 0) {
				prepareStep();
				_state = ObjectState::WALKING;
			}
			else {
				_state = ObjectState::IDLE;
			}
		}
	}
	else if (_state == ObjectState::WALKING) {
		step_forward(_transform, _startPosition, _targetPosition, dt, _dbgTTL);
		if (!rotate_to(_transform, ds::vec3(0.0f, _transform.rotation.y, 0.0f), ds::vec3(0.0f, _transform.rotation.y, ds::PI * -0.5f), dt, _dbgTTL)) {
			--_steps;
			if (_steps <= 0) {
				_state = ObjectState::IDLE;
			}
			else {
				if (!prepareStep()) {
					_transform.timer[2] = 0.0f;
					_startAngle = _transform.rotation.y;
					_endAngle = _startAngle + ds::PI * 0.5f;
					_state = ObjectState::ROTATING;
				}
			}
		}
	}

}

// ----------------------------------------------------
// render
// ----------------------------------------------------
void TopDownObjViewer::render() {
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
void TopDownObjViewer::renderGUI() {
	int state = 1;
	gui::start();
	p2i sp = p2i(10, 760);
	if (gui::begin("Object", &state, &sp, 320)) {
		gui::Value("FPS", ds::getFramesPerSecond());
		gui::Value("Center", _mesh.getCenter());
		gui::Value("Extent", _mesh.getExtent());
		gui::Value("Min", _mesh.getMin());
		gui::Value("Max", _mesh.getMax());
		gui::SliderAngle("RotY", &_transform.rotation.y);
		gui::Input("WorldPos", &_transform.position);
		for (int i = 0; i < 3; ++i) {
			gui::Input("LightDir", &_lightDir[i]);
		}
		if (gui::Input("Scale", &_transform.scale)) {
			_transform.position.y = _mesh.getExtent().y * 0.5f * _transform.scale.y;
		}
		gui::Input("Move TTL", &_dbgMoveTTL);
		if (gui::Button("Center")) {
			_transform.position = _mesh.getCenter() * -1.0f;
		}
		if (gui::Button("Reset Camera")) {
			_fpsCamera->setPosition(ds::vec3(0, 3, -6), ds::vec3(0.0f, 0.0f, 0.0f));
		}
		if (gui::Button("Move")) {
			if (_state == ObjectState::IDLE) {
				_transform.timer[0] = 0.0f;
				_startPosition = ds::vec3(0.0f, 10.0f, 0.0f);
				_targetPosition = ds::vec3(0.0f);
				_targetPosition.y = _mesh.getExtent().y * 0.5f;
				_state = ObjectState::MOVING;
			}
		}
		if (gui::Button("Step forward")) {
			if (_state == ObjectState::IDLE) {
				prepareStep();
				if (!is_outside(_targetPosition, ds::vec4(-7.0f, -7.0f, 7.0f, 7.0f))) {
					_state = ObjectState::STEPPING;
				}
				else {
					_transform.timer[2] = 0.0f;
					_startAngle = _transform.rotation.y;
					_endAngle = _startAngle + ds::PI * 0.5f;
					_state = ObjectState::ROTATING;
				}
			}
		}
		gui::Input("Steps", &_stepCounter);
		if (gui::Button("Walk")) {
			if (_state == ObjectState::IDLE) {
				_steps = _stepCounter;
				prepareStep();
				_state = ObjectState::WALKING;
			}
		}
		if (gui::Button("Rotate")) {
			if (_state == ObjectState::IDLE) {
				_transform.timer[2] = 0.0f;
				_startAngle = _transform.rotation.y;
				_endAngle = _startAngle + ds::PI * 0.5f;
				_state = ObjectState::ROTATING;
			}
		}
	}
	gui::end();
}

bool TopDownObjViewer::prepareStep() {
	_transform.timer[0] = 0.0f;
	_transform.timer[2] = 0.0f;
	_startPosition = _transform.position;
	ds::vec3 vel = ds::vec3(cos(_transform.rotation.y) * _transform.scale.x, 0.0f, -_transform.scale.x * sin(_transform.rotation.y));
	_targetPosition = _startPosition + vel;
	_targetPosition.y += _mesh.getExtent().y * 0.5f * _transform.scale.x;
	return !is_outside(_targetPosition, ds::vec4(-7.0f, -7.0f, 7.0f, 7.0f));
}