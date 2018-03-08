#include "FlowFieldApp.h"
#include <ds_imgui.h>
#include "..\..\shaders\AmbientMultipleLightning_VS_Main.h"
#include "..\..\shaders\AmbientMultipleLightning_PS_Main.h"
#include "..\utils\tweening.h"
#include "..\..\shaders\RepeatingGrid_VS_Main.h"
#include "..\..\shaders\RepeatingGrid_PS_MainXZ.h"

FlowFieldApp::FlowFieldApp() : TestApp() {
	_grid = new Grid(GRID_SIZE_X, GRID_SIZE_Y);
	_startPoint = p2i(0, 0);
	_endPoint = p2i(0, 0);
	_grid->load("TestLevel");
	_startPoint = _grid->getStart();
	_endPoint = _grid->getEnd();
	_flowField = new FlowField(_grid);
	_flowField->build(_endPoint);
}

FlowFieldApp::~FlowFieldApp() {
	delete _flowField;
	delete _grid;
	delete _renderItem;
	delete _fpsCamera;
	delete _material;
}

ds::RenderSettings FlowFieldApp::getRenderSettings() {
	ds::RenderSettings rs;
	rs.width = 1280;
	rs.height = 720;
	rs.title = "FlowFieldApp";
	rs.clearColor = ds::Color(0.01f, 0.01f, 0.01f, 1.0f);
	rs.multisampling = 4;
	rs.useGPUProfiling = false;
	rs.supportDebug = false;
	return rs;
}

// ----------------------------------------------------
// init
// ----------------------------------------------------
bool FlowFieldApp::init() {
	
	_fpsCamera = new FPSCamera(&_camera);
	_fpsCamera->setPosition(ds::vec3(0, 14, -6), ds::vec3(0.0f, 0.0f, 0.0f));

	_material = new InstancedAmbientLightningMaterial;

	_lightDir[0] = ds::vec3(1.0f, 0.5f, 1.0f);
	_lightDir[1] = ds::vec3(-1.0f, 0.5f, 1.0f);
	_lightDir[2] = ds::vec3(0.0f, 0.5f, 0.0f);
	
	_renderItem = new InstancedRenderItem("basic_floor", _material, GRID_SIZE_X * GRID_SIZE_Y);
	for (int y = 0; y < _grid->height; ++y) {
		for (int x = 0; x < _grid->width; ++x) {
			ds::vec3 p = ds::vec3(-10 + x,0,- 6 + y);
			int type = _grid->get(x,y);
			if (type == 0) {
				_ids[x + y * _grid->width] = _renderItem->add();
				instance_item& item = _renderItem->get(_ids[x + y * _grid->width]);
				item.transform.position = p;
			}
		}
	}
	_ambient_material = new AmbientLightningMaterial;
	_walker.renderItem = new RenderItem("simple_walker", _ambient_material);
	_walker.velocity = 1.0f;
	_walker.definitionIndex = 0;
	_walker.gridPos = _startPoint;
	_walker.pos = ds::vec3(-10 + _startPoint.x, 0, -6 + _startPoint.y);
	_walker.rotation = 0.0f;
	transform& t = _walker.renderItem->getTransform();
	t.position = _walker.pos;
	_selected = -1;
	
	return true;
}

// ---------------------------------------------------------------
// move walkers
// ---------------------------------------------------------------
void FlowFieldApp::moveWalkers(float dt) {
	if (_flowField->hasNext(_walker.gridPos)) {
		p2i n = _flowField->next(_walker.gridPos);
		ds::vec3 nextPos = ds::vec3(-10 + n.x, 0, -6 + n.y);
		ds::vec3 diff = _walker.pos - ds::vec3(nextPos.x, 0.0f, nextPos.y);
		if (sqr_length(diff) < 0.01f) {
			convert(_walker.pos.x, _walker.pos.y, -10, -6, &_walker.gridPos);
		}
		ds::vec3 v = normalize(ds::vec3(nextPos.x, 0.0f, nextPos.y) - _walker.pos) * _walker.velocity;
		_walker.pos += v * dt;
		transform& t = _walker.renderItem->getTransform();
		t.position = _walker.pos;
	}
}

// ----------------------------------------------------
// tick
// ----------------------------------------------------
void FlowFieldApp::tick(float dt) {
	_fpsCamera->update(dt);

	moveWalkers(dt);
}

// ----------------------------------------------------
// render
// ----------------------------------------------------
void FlowFieldApp::render() {

	_renderItem->draw(_basicPass, _camera.viewProjectionMatrix);
	_walker.renderItem->draw(_basicPass, _camera.viewProjectionMatrix);
}

// ----------------------------------------------------
// renderGUI
// ----------------------------------------------------
void FlowFieldApp::renderGUI() {
	int state = 1;
	p2i sp = p2i(10, 710);
	gui::start(&sp, 320);
	if (gui::begin("Object", &state)) {
		gui::Value("FPS", ds::getFramesPerSecond());
		if (gui::Button("Reset")) {
			_walker.gridPos = _startPoint;
			_walker.pos = ds::vec3(-10 + _startPoint.x, 0, -6 + _startPoint.y);
			_walker.rotation = 0.0f;
			transform& t = _walker.renderItem->getTransform();
			t.position = _walker.pos;
		}
		/*
		gui::Value("Center", _renderItem.mesh->getCenter());
		gui::Value("Extent", _renderItem.mesh->getExtent());
		gui::Value("Min", _renderItem.mesh->getMin());
		gui::Value("Max", _renderItem.mesh->getMax());
		gui::Input("Selected", &_selected);
		if (_selected != -1) {
			instance_item& item = get_instance(&_renderItem, _ids[_selected]);
			if (gui::Input("Scale", &item.transform.scale)) {
				item.transform.position.y = _renderItem.mesh->getExtent().y * item.transform.scale.y * 0.5f;
			}
		}
		*/
	}
	gui::end();
}

