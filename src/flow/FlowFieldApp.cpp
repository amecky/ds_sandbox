#include "FlowFieldApp.h"
#include <ds_imgui.h>
#include "..\..\shaders\AmbientMultipleLightning_VS_Main.h"
#include "..\..\shaders\AmbientMultipleLightning_PS_Main.h"
#include "..\utils\tweening.h"
#include "..\..\shaders\RepeatingGrid_VS_Main.h"
#include "..\..\shaders\RepeatingGrid_PS_MainXZ.h"
#include "..\utils\common_math.h"

FlowFieldApp::FlowFieldApp() : TestApp() {
	_grid = new Grid(GRID_SIZE_X, GRID_SIZE_Y);
	_startPoint = p2i(0, 0);
	_endPoint = p2i(0, 0);
	_grid->load("TestLevel4");
	_startPoint = _grid->getStart();
	_endPoint = _grid->getEnd();
	_flowField = new FlowField(_grid);
	_flowField->build(_endPoint);
	_dbgLength = 0.0f;
	_dbgMove = true;
	_dbgNextPos = ds::vec3(0);
	_grid->plane = Plane(ds::vec3(0.0f, 0.0f, 0.0f), ds::vec3(0.0f, -1.0f, 0.0f));
	_dbgSelected = p2i(-1, -1);
	_dbgShowOverlay = true;
}

FlowFieldApp::~FlowFieldApp() {
	delete _overlayItem;
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

	float lz = 0.2f;
	_lightDir[0] = ds::vec3(1.0f, 0.5f, lz);
	_lightDir[1] = ds::vec3(-1.0f, 0.5f, lz);
	_lightDir[2] = ds::vec3(0.0f, 0.5f, lz);
	
	_renderItem = new InstancedRenderItem("basic_floor", _material, GRID_SIZE_X * GRID_SIZE_Y);
	_overlayItem = new InstancedRenderItem("arrow", _material, GRID_SIZE_X * GRID_SIZE_Y);

	for (int y = 0; y < _grid->height; ++y) {
		for (int x = 0; x < _grid->width; ++x) {			
			int type = _grid->get(x,y);
			int idx = x + y * _grid->width;
			if (type == 0) {
				ds::vec3 p = ds::vec3(-10 + x, 0, -6 + y);
				_ids[idx] = _renderItem->add();
				instance_item& item = _renderItem->get(_ids[idx]);
				item.transform.position = p;

				int d = _flowField->get(x, y);
				if (d >= 0 && d < 9) {
					_overlay_ids[idx] = _overlayItem->add();
					p.y = 0.1f;
					instance_item& overitem = _overlayItem->get(_overlay_ids[idx]);
					overitem.transform.position = p;
					overitem.transform.pitch = static_cast<float>(d) * ds::PI * 0.25f;
				}
			}
		}
	}
	_ambient_material = new AmbientLightningMaterial;
	_walker.renderItem = new RenderItem("rounded_cube", _ambient_material);
	_walker.velocity = 0.5f;
	_walker.definitionIndex = 0;
	_walker.gridPos = _startPoint;
	_walker.pos = ds::vec3(-10.0f + _startPoint.x, 0.2f, -6.0f + _startPoint.y);
	_walker.rotation = 0.0f;
	transform& t = _walker.renderItem->getTransform();
	t.position = _walker.pos;
	_selected = -1;

	_towerItem = new RenderItem("cube", _ambient_material);
	_towerItem->getTransform().position = ds::vec3(0.0f);

	return true;
}

// ---------------------------------------------------------------
// get angle between two ds::vec2 vectors
// ---------------------------------------------------------------
float getAngle(const ds::vec2& u, const ds::vec2& v) {
	double x = v.x - u.x;
	double y = v.y - u.y;
	double ang = atan2(y, x);
	return (float)ang;
}

// ---------------------------------------------------------------
// move walkers
// ---------------------------------------------------------------
void FlowFieldApp::moveWalkers(float dt) {

	ds::vec3 p1(-10.0f, 0.2f, 0.0f);
	ds::vec3 p2(-9.0f, 0.2f, 1.0f);
	ds::vec3 dp = p2 - p1;
	ds::vec3 np = normalize(dp);
	if (_dbgMove) {
		if (_flowField->hasNext(_walker.gridPos)) {
			p2i n = _flowField->next(_walker.gridPos);
			_dbgFlowNext = n;
			ds::vec3 nextPos = ds::vec3(-10.0f + n.x, 0.2f, -6.0f + n.y);
			_dbgNextPos = nextPos;
			ds::vec3 diff = _walker.pos - nextPos;
			_dbgDiff = diff;
			_dbgLength = sqr_length(diff);
			if (sqr_length(diff) < 0.001f) {
				convert(nextPos.x, nextPos.z, -10, -6, &_walker.gridPos);
			}
			_walker.pos.y = 0.2f;
			ds::vec3 v = normalize(nextPos - _walker.pos);
			_dbgV = v;
			v *= _walker.velocity;
			_walker.pos += v * dt;
			_walker.pos.y = 0.2f;
			transform& t = _walker.renderItem->getTransform();
			t.position = _walker.pos;
			t.pitch = getAngle(ds::vec2(_walker.pos.x, _walker.pos.z), ds::vec2(nextPos.x, nextPos.z));
		}
	}
}

void FlowFieldApp::OnButtonClicked(int index) {
	if (index == 0) {
		Ray r = get_picking_ray(_camera.projectionMatrix, _camera.viewMatrix);
		r.setOrigin(_camera.position);
		ds::vec3 ip = _grid->plane.getIntersection(r);
		DBG_LOG("intersection point %2.3f %2.3f %2.3f", ip.x, ip.y, ip.z);
		p2i gridPos;
		if (convert(ip.x, ip.z, -10.0f, -6.0f, &gridPos)) {
			_dbgSelected = gridPos;
		}
		else {
			_dbgSelected = p2i(-1, -1);
		}
	}
}

void FlowFieldApp::updateOverlay() {
	for (int y = 0; y < _grid->height; ++y) {
		for (int x = 0; x < _grid->width; ++x) {
			int type = _grid->get(x, y);
			if (type == 0) {
				int idx = x + y * _grid->width;
				int d = _flowField->get(x, y);
				if (d >= 0 && d < 9) {
					instance_item& overitem = _overlayItem->get(_overlay_ids[idx]);
					overitem.transform.pitch = static_cast<float>(d) * ds::PI * 0.25f;
				}
			}
		}
	}
}

void FlowFieldApp::addTower(p2i gridPos) {
	if (_grid->get(gridPos) == 0) {
		_grid->set(gridPos.x, gridPos.y, 1);
		_flowField->build(_endPoint);
		updateOverlay();
		Tower t;
		t.renderItem = _towerItem;
		t.gx = gridPos.x;
		t.gy = gridPos.y;
		t.position = ds::vec3(-10.0f + gridPos.x, 0.5f, -6.0f + gridPos.y);
		t.level = 1;
		_towers.push_back(t);
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
	if (_dbgShowOverlay) {
		_overlayItem->draw(_basicPass, _camera.viewProjectionMatrix);
	}
	_walker.renderItem->draw(_basicPass, _camera.viewProjectionMatrix);

	for (size_t i = 0; i < _towers.size(); ++i) {
		const Tower& t = _towers[i];
		t.renderItem->getTransform().position = t.position;
		t.renderItem->draw(_basicPass, _camera.viewProjectionMatrix);
	}
}

// ----------------------------------------------------
// renderGUI
// ----------------------------------------------------
void FlowFieldApp::renderGUI() {
	int state = 1;
	p2i sp = p2i(10, 710);
	gui::start(&sp, 320);
	gui::setAlphaLevel(0.3f);
	if (gui::begin("Object", &state)) {
		gui::Value("FPS", ds::getFramesPerSecond());
		gui::Checkbox("Overlay", &_dbgShowOverlay);
		gui::Value("Selected", _dbgSelected);
		if (_dbgSelected.x != -1 && _dbgSelected.y != -1) {
			int type = _grid->get(_dbgSelected);
			gui::Value("Grid Type", type);
			int d = _flowField->get(_dbgSelected.x, _dbgSelected.y);
			gui::Value("Flowfield", d);
			if (gui::Button("Add tower")) {
				addTower(_dbgSelected);
			}
		}
		gui::Checkbox("Move", &_dbgMove);
		gui::Value("Pos", _walker.renderItem->getTransform().position,"%2.2f %2.2f %2.2f");
		gui::Value("Grid Pos", _walker.gridPos);
		gui::Value("Next", _dbgNextPos);
		gui::Value("Next GP", _dbgFlowNext);
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

