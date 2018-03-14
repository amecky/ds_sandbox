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
	buildPath();
	_dbgLength = 0.0f;
	_dbgMove = true;
	_dbgNextPos = ds::vec3(0);
	_grid->plane = Plane(ds::vec3(0.0f, 0.0f, 0.0f), ds::vec3(0.0f, -1.0f, 0.0f));
	_dbgSelected = p2i(-1, -1);
	_dbgShowOverlay = false;
	_selectedTower = -1;
	_dbgMoveCamera = true;
	_dbgSelectedLight = 0;
	_dbgShowPath = true;
	_dbgHandleButtons = true;
	_dbgSnapToGrid = true;
	_dbgIsoCamera = true;
}

FlowFieldApp::~FlowFieldApp() {
	delete _overlayItem;
	delete _flowField;
	delete _grid;
	delete _renderItem;
	delete _fpsCamera;
	delete _material;
}

// -------------------------------------------------------------
// get render settings
// -------------------------------------------------------------
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
	_fpsCamera->setPosition(ds::vec3(0, 12, -6), ds::vec3(0.0f, 0.0f, 0.0f));
	//_fpsCamera->setPitch(30.0f / 360.0f * ds::TWO_PI);
	//_fpsCamera->setYaw(45.0f / 360.0f * ds::TWO_PI);

	_isoCamera = new IsometricCamera(&_camera);
	_isoCamera->setPosition(ds::vec3(0, 12, -6), ds::vec3(0.0f, 0.0f, 0.0f));

	_material = new InstancedAmbientLightningMaterial;

	_lightDir[0] = ds::vec3(1.0f,-0.31f,1.0f);
	_lightDir[1] = ds::vec3(0.6f,-0.28f,0.34f);
	_lightDir[2] = ds::vec3(-0.3f,0.7f,-0.2f);
	
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
					p.y = 0.05f;
					instance_item& overitem = _overlayItem->get(_overlay_ids[idx]);
					overitem.transform.position = p;
					overitem.transform.pitch = static_cast<float>(d) * ds::PI * 0.25f;
				}
			}
		}
	}
	_ambient_material = new AmbientLightningMaterial;
	_walkerItem = new RenderItem("simple_walker", _ambient_material);
	_selected = -1;

	_pathItem = new RenderItem("arrow", _ambient_material);
	_cursorItem = new RenderItem("cursor", _ambient_material);
	
	_towers.init(_ambient_material);

	_startItem = new RenderItem("start", _ambient_material);
	_startItem->getTransform().position = ds::vec3(-10.0f + _startPoint.x, 0.2f, -6.0f + _startPoint.y);

	_endItem = new RenderItem("end", _ambient_material);
	_endItem->getTransform().position = ds::vec3(-10.0f + _endPoint.x, 0.2f, -6.0f + _endPoint.y);

	return true;
}

void FlowFieldApp::startWalker() {
	ID id = _walkers.add();
	Walker& walker = _walkers.get(id);
	walker.renderItem = _walkerItem;
	walker.velocity = 1.5f;
	walker.definitionIndex = 0;
	walker.gridPos = _startPoint;
	walker.pos = ds::vec3(-10.0f + _startPoint.x, 0.2f, -6.0f + _startPoint.y);
	walker.rotation = 0.0f;
	walker.renderItem->getTransform().position = walker.pos;
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

void FlowFieldApp::buildPath() {
	_path.clear();
	p2i gp = _startPoint;
	//_path.push_back(gp);
	while (_flowField->hasNext(gp)) {
		PathItem item;
		int d = _flowField->get(gp.x, gp.y);
		gp = _flowField->next(gp);		
		item.pos = ds::vec3(-10.0f + gp.x, 0.05f, -6.0f + gp.y);
		item.grid = gp;
		item.direction = static_cast<float>(d) * ds::PI * 0.25f;
		_path.push_back(item);
	}

}
// ---------------------------------------------------------------
// move walkers
// ---------------------------------------------------------------
void FlowFieldApp::moveWalkers(float dt) {
	if (_dbgMove) {
		for (uint32_t i = 0; i < _walkers.numObjects; ++i) {
			Walker& walker = _walkers.objects[i];
			if (_flowField->hasNext(walker.gridPos)) {
				p2i n = _flowField->next(walker.gridPos);
				_dbgFlowNext = n;
				ds::vec3 nextPos = ds::vec3(-10.0f + n.x, 0.2f, -6.0f + n.y);
				_dbgNextPos = nextPos;
				ds::vec3 diff = walker.pos - nextPos;
				_dbgDiff = diff;
				_dbgLength = sqr_length(diff);
				if (sqr_length(diff) < 0.001f) {
					convert(nextPos.x, nextPos.z, -10, -6, &walker.gridPos);
				}
				walker.pos.y = 0.2f;
				ds::vec3 v = normalize(nextPos - walker.pos);
				_dbgV = v;
				v *= walker.velocity;
				walker.pos += v * dt;
				walker.pos.y = 0.2f;
				walker.rotation = getAngle(ds::vec2(walker.pos.x, walker.pos.z), ds::vec2(nextPos.x, nextPos.z));
			}
			else {
				_walkers.remove(walker.id);
			}
		}
	}
}



// -------------------------------------------------------------
// on button clicked
// -------------------------------------------------------------
void FlowFieldApp::OnButtonClicked(int index) {
	if (_dbgHandleButtons) {
		if (index == 0) {
			Ray r = get_picking_ray(_camera.projectionMatrix, _camera.viewMatrix);
			r.setOrigin(_camera.position);
			ds::vec3 ip = _grid->plane.getIntersection(r);
			DBG_LOG("intersection point %2.3f %2.3f %2.3f", ip.x, ip.y, ip.z);
			p2i gridPos;
			if (convert(ip.x, ip.z, -10.0f, -6.0f, &gridPos)) {
				_dbgSelected = gridPos;
				int type = _grid->get(gridPos);
				if (type == 0) {
					DBG_LOG("Adding tower at %d %d", gridPos.x, gridPos.y);
					addTower(gridPos);
				}
				else {
					_selectedTower = _towers.findTower(gridPos);
				}
			}
			else {
				_dbgSelected = p2i(-1, -1);
			}
		}
	}
}

// -------------------------------------------------------------
// update overlay
// -------------------------------------------------------------
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

// -------------------------------------------------------------
// add tower
// -------------------------------------------------------------
void FlowFieldApp::addTower(const p2i& gridPos) {
	if (_grid->get(gridPos) == 0) {
		_grid->set(gridPos.x, gridPos.y, 1);
		_flowField->build(_endPoint);
		updateOverlay();
		buildPath();
		_towers.addTower(gridPos);
	}
}

// ----------------------------------------------------
// tick
// ----------------------------------------------------
void FlowFieldApp::tick(float dt) {
	if (_dbgMoveCamera) {
		if (_dbgIsoCamera) {
			_isoCamera->update(dt);
		}
		else {
			_fpsCamera->update(dt);
		}
	}
	moveWalkers(dt);

	_towers.rotateTowers(&_walkers);

	Ray r = get_picking_ray(_camera.projectionMatrix, _camera.viewMatrix);
	r.setOrigin(_camera.position);
	ds::vec3 ip = _grid->plane.getIntersection(r);
	ip.y = 0.1f;
	if (_dbgSnapToGrid) {
		p2i gp;
		if (convert(ip.x, ip.z, -10.0f, -6.0f, &gp)) {
			ds::vec3 np = ds::vec3(gp.x - 10, 0, gp.y - 6);
			np.y = 0.1f;
			_cursorItem->getTransform().position = np;
		}
	}
	else {
		_cursorItem->getTransform().position = ip;
	}
	//_towers.rotateTowers(ip);
}

// ----------------------------------------------------
// render
// ----------------------------------------------------
void FlowFieldApp::render() {
	// update lights
	for (int i = 0; i < 3; ++i) {
		_material->setLightDirection(i, _lightDir[i]);
		_ambient_material->setLightDirection(i, normalize(_lightDir[i]));
	}
	_renderItem->draw(_basicPass, _camera.viewProjectionMatrix);
	// overlay
	if (_dbgShowOverlay) {
		_overlayItem->draw(_basicPass, _camera.viewProjectionMatrix);
	}
	if (_dbgShowPath) {
		for (size_t i = 0; i < _path.size(); ++i) {
			const PathItem& item = _path[i];
			_pathItem->getTransform().position = item.pos;
			_pathItem->getTransform().pitch = item.direction;
			_pathItem->draw(_basicPass, _camera.viewProjectionMatrix);
		}
	}
	// start and end
	_startItem->draw(_basicPass, _camera.viewProjectionMatrix);
	_endItem->draw(_basicPass, _camera.viewProjectionMatrix);
	// walkers
	for (uint32_t i = 0; i < _walkers.numObjects; ++i) {
		const Walker& walker = _walkers.objects[i];
		transform& t = _walkerItem->getTransform();
		t.position = walker.pos;
		t.pitch = walker.rotation;
		_walkerItem->draw(_basicPass, _camera.viewProjectionMatrix);
	}

	_cursorItem->draw(_basicPass, _camera.viewProjectionMatrix);
	// towers
	_towers.render(_basicPass, _camera.viewProjectionMatrix);
}

// ----------------------------------------------------
// renderGUI
// ----------------------------------------------------
void FlowFieldApp::renderGUI() {
	int state = 1;
	p2i sp = p2i(10, 710);
	gui::start(&sp, 320);
	gui::setAlphaLevel(0.5f);
	if (gui::begin("Debug", &state)) {
		gui::Value("FPS", ds::getFramesPerSecond());
		gui::Checkbox("Move camera", &_dbgMoveCamera);
		gui::Checkbox("ISO camera", &_dbgIsoCamera);
		gui::Checkbox("Handle buttons", &_dbgHandleButtons);
		gui::Checkbox("Snap2Grid", &_dbgSnapToGrid);
		gui::StepInput("Light Index", &_dbgSelectedLight, 0, 2, 1);
		gui::Slider("L-X", &_lightDir[_dbgSelectedLight].x, -1.0f, 1.0f);
		gui::Slider("L-Y", &_lightDir[_dbgSelectedLight].y, -1.0f, 1.0f);
		gui::Slider("L-Z", &_lightDir[_dbgSelectedLight].z, -1.0f, 1.0f);
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
		
		_towers.showGUI(_selectedTower);
		
		gui::begin("Walker", 0);
		gui::Checkbox("Move", &_dbgMove);
		//gui::Value("Pos", _walker.renderItem->getTransform().position,"%2.2f %2.2f %2.2f");
		//gui::Value("Grid Pos", _walker.gridPos);
		gui::Value("Next", _dbgNextPos);
		gui::Value("Next GP", _dbgFlowNext);
		if (gui::Button("Start Walker")) {
			startWalker();
		}		
	}
	gui::end();
}

