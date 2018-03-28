#include "MainGameScene.h"
#include <ds_imgui.h>
#include "..\..\shaders\AmbientMultipleLightning_VS_Main.h"
#include "..\..\shaders\AmbientMultipleLightning_PS_Main.h"
#include "..\..\shaders\RepeatingGrid_VS_Main.h"
#include "..\..\shaders\RepeatingGrid_PS_MainXZ.h"
#include "..\utils\common_math.h"
#include "..\GameContext.h"

MainGameScene::MainGameScene(GameContext* gameContext) : ds::BaseScene() , _gameContext(gameContext) {
	_grid = new Grid(GRID_SIZE_X, GRID_SIZE_Y);
	_startPoint = p2i(0, 0);
	_endPoint = p2i(0, 0);
	_grid->load("TestLevel4");
	_startPoint = _grid->getStart();
	_endPoint = _grid->getEnd();
	_flowField = new FlowField(_grid);
	_flowField->build(_endPoint);
	buildPath();
	_dbgCtx.length = 0.0f;
	_dbgCtx.move = true;
	_dbgCtx.nextPos = ds::vec3(0);
	_grid->plane = Plane(ds::vec3(0.0f, 0.0f, 0.0f), ds::vec3(0.0f, -1.0f, 0.0f));
	_dbgCtx.selected = p2i(-1, -1);
	_dbgCtx.showOverlay = false;
	_selectedTower = -1;
	_dbgCtx.moveCamera = true;
	_dbgCtx.selectedLight = 0;
	_dbgCtx.showPath = true;
	_dbgCtx.handleButtons = true;
	_dbgCtx.snapToGrid = true;
	_dbgCtx.isoCamera = true;
	_dbgCtx.towerType = 0;
}

MainGameScene::~MainGameScene() {
	delete _overlayItem;
	delete _flowField;
	delete _grid;
	delete _renderItem;
	delete _fpsCamera;
}

// ----------------------------------------------------
// init
// ----------------------------------------------------
void MainGameScene::initialize() {
	
	_fpsCamera = new FPSCamera(&_camera);
	_fpsCamera->setPosition(ds::vec3(0, 8, -6), ds::vec3(0.0f, 0.0f, 0.0f));
	//_fpsCamera->setPitch(30.0f / 360.0f * ds::TWO_PI);
	//_fpsCamera->setYaw(45.0f / 360.0f * ds::TWO_PI);

	_isoCamera = new IsometricCamera(&_camera);
	_isoCamera->setPosition(ds::vec3(0, 8, -6), ds::vec3(0.0f, 0.0f, 0.0f));

	

	_lightDir[0] = ds::vec3(1.0f,-0.31f,1.0f);
	_lightDir[1] = ds::vec3(0.6f,-0.28f,0.34f);
	_lightDir[2] = ds::vec3(-0.3f,0.7f,-0.2f);
	
	_renderItem = new InstancedRenderItem("basic_floor", _gameContext->instancedAmbientmaterial, GRID_SIZE_X * GRID_SIZE_Y);
	_overlayItem = new InstancedRenderItem("arrow", _gameContext->instancedAmbientmaterial, GRID_SIZE_X * GRID_SIZE_Y);

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
					overitem.transform.rotation.y = static_cast<float>(d) * ds::PI * 0.25f;
				}
			}
		}
	}
	
	_walkerItem = new RenderItem("simple_walker", _gameContext->ambientMaterial);
	_selected = -1;

	_pathItem = new RenderItem("arrow", _gameContext->ambientMaterial);
	_cursorItem = new RenderItem("cursor", _gameContext->ambientMaterial);
	
	_startItem = new RenderItem("start", _gameContext->ambientMaterial);
	_startItem->getTransform().position = ds::vec3(-10.0f + _startPoint.x, 0.2f, -6.0f + _startPoint.y);

	_endItem = new RenderItem("end", _gameContext->ambientMaterial);
	_endItem->getTransform().position = ds::vec3(-10.0f + _endPoint.x, 0.2f, -6.0f + _endPoint.y);

	addTower(p2i(12, 5), 0);
	addTower(p2i(12, 4), 1);
	addTower(p2i(12, 6), 2);
	addTower(p2i(12, 7), 3);
	addTower(p2i(10, 9), 0);

}

void MainGameScene::startWalker() {
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

void MainGameScene::buildPath() {
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
void MainGameScene::moveWalkers(float dt) {
	if (_dbgCtx.move) {
		for (uint32_t i = 0; i < _walkers.numObjects; ++i) {
			Walker& walker = _walkers.objects[i];
			if (_flowField->hasNext(walker.gridPos)) {
				p2i n = _flowField->next(walker.gridPos);
				_dbgCtx.flowNext = n;
				ds::vec3 nextPos = ds::vec3(-10.0f + n.x, 0.2f, -6.0f + n.y);
				_dbgCtx.nextPos = nextPos;
				ds::vec3 diff = walker.pos - nextPos;
				_dbgCtx.diff = diff;
				_dbgCtx.length = sqr_length(diff);
				if (sqr_length(diff) < 0.001f) {
					convert(nextPos.x, nextPos.z, -10, -6, &walker.gridPos);
				}
				walker.pos.y = 0.2f;
				ds::vec3 v = normalize(nextPos - walker.pos);
				_dbgCtx.V = v;
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
void MainGameScene::OnButtonClicked(int index) {
	if (_dbgCtx.handleButtons) {
		if (index == 0) {
			Ray r = get_picking_ray(_camera.projectionMatrix, _camera.viewMatrix);
			r.setOrigin(_camera.position);
			ds::vec3 ip = _grid->plane.getIntersection(r);
			DBG_LOG("intersection point %2.3f %2.3f %2.3f", ip.x, ip.y, ip.z);
			p2i gridPos;
			if (convert(ip.x, ip.z, -10.0f, -6.0f, &gridPos)) {
				_dbgCtx.selected = gridPos;
				int type = _grid->get(gridPos);
				if (type == 0) {
					DBG_LOG("Adding tower at %d %d", gridPos.x, gridPos.y);
					addTower(gridPos, _dbgCtx.towerType);
					_grid->set(gridPos, 1);
				}
				else {
					_selectedTower = _gameContext->towers.findTower(gridPos);
				}
			}
			else {
				_dbgCtx.selected = p2i(-1, -1);
			}
		}
	}
}

// -------------------------------------------------------------
// update overlay
// -------------------------------------------------------------
void MainGameScene::updateOverlay() {
	for (int y = 0; y < _grid->height; ++y) {
		for (int x = 0; x < _grid->width; ++x) {
			int type = _grid->get(x, y);
			if (type == 0) {
				int idx = x + y * _grid->width;
				int d = _flowField->get(x, y);
				if (d >= 0 && d < 9) {
					instance_item& overitem = _overlayItem->get(_overlay_ids[idx]);
					overitem.transform.rotation.y = static_cast<float>(d) * ds::PI * 0.25f;
				}
			}
		}
	}
}

// -------------------------------------------------------------
// add tower
// -------------------------------------------------------------
void MainGameScene::addTower(const p2i& gridPos, int type) {
	if (_grid->get(gridPos) == 0) {
		_grid->set(gridPos.x, gridPos.y, 1);
		_flowField->build(_endPoint);
		updateOverlay();
		buildPath();
		_gameContext->towers.addTower(gridPos, type);
	}
}

// ----------------------------------------------------
// tick
// ----------------------------------------------------
void MainGameScene::update(float dt) {
	if (_dbgCtx.moveCamera) {
		if (_dbgCtx.isoCamera) {
			_isoCamera->update(dt);
		}
		else {
			_fpsCamera->update(dt);
		}
	}
	moveWalkers(dt);

	_gameContext->towers.rotateTowers(&_walkers);

	_gameContext->towers.tick(dt, _events);

	Ray r = get_picking_ray(_camera.projectionMatrix, _camera.viewMatrix);
	r.setOrigin(_camera.position);
	ds::vec3 ip = _grid->plane.getIntersection(r);
	ip.y = 0.1f;
	if (_dbgCtx.snapToGrid) {
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

	_gameContext->particles->tick(dt);
}

// ----------------------------------------------------
// render
// ----------------------------------------------------
void MainGameScene::render() {
	// update lights
	for (int i = 0; i < 3; ++i) {
		_gameContext->instancedAmbientmaterial->setLightDirection(i, _lightDir[i]);
		_gameContext->ambientMaterial->setLightDirection(i, normalize(_lightDir[i]));
	}
	_renderItem->draw(_basicPass, _camera.viewProjectionMatrix);
	// overlay
	if (_dbgCtx.showOverlay) {
		_overlayItem->draw(_basicPass, _camera.viewProjectionMatrix);
	}
	if (_dbgCtx.showPath) {
		for (size_t i = 0; i < _path.size(); ++i) {
			const PathItem& item = _path[i];
			_pathItem->getTransform().position = item.pos;
			_pathItem->getTransform().rotation.y = item.direction;
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
		t.rotation.y = walker.rotation;
		_walkerItem->draw(_basicPass, _camera.viewProjectionMatrix);
	}

	_cursorItem->draw(_basicPass, _camera.viewProjectionMatrix);
	// towers
	_gameContext->particles->render(_basicPass, _camera.viewProjectionMatrix, _camera.position);
	_gameContext->towers.render(_basicPass, _camera.viewProjectionMatrix);
}

// ----------------------------------------------------
// renderGUI
// ----------------------------------------------------
void MainGameScene::showGUI() {
	int state = 1;
	p2i sp = p2i(10, 710);
	gui::start(&sp, 320);
	gui::setAlphaLevel(0.5f);
	if (gui::begin("Debug", &state)) {
		gui::Value("FPS", ds::getFramesPerSecond());
		gui::Checkbox("Move camera", &_dbgCtx.moveCamera);
		gui::Checkbox("ISO camera", &_dbgCtx.isoCamera);
		gui::Checkbox("Handle buttons", &_dbgCtx.handleButtons);
		gui::Checkbox("Snap2Grid", &_dbgCtx.snapToGrid);
		gui::StepInput("Light Index", &_dbgCtx.selectedLight, 0, 2, 1);
		gui::Slider("L-X", &_lightDir[_dbgCtx.selectedLight].x, -1.0f, 1.0f);
		gui::Slider("L-Y", &_lightDir[_dbgCtx.selectedLight].y, -1.0f, 1.0f);
		gui::Slider("L-Z", &_lightDir[_dbgCtx.selectedLight].z, -1.0f, 1.0f);
		gui::Checkbox("Overlay", &_dbgCtx.showOverlay);
		gui::StepInput("Tower Idx", &_dbgCtx.towerType, 0, 3, 1);
		gui::Value("Selected", _dbgCtx.selected);
		if (_dbgCtx.selected.x != -1 && _dbgCtx.selected.y != -1) {
			int type = _grid->get(_dbgCtx.selected);
			gui::Value("Grid Type", type);
			int d = _flowField->get(_dbgCtx.selected.x, _dbgCtx.selected.y);
			gui::Value("Flowfield", d);
			if (gui::Button("Add tower")) {
				addTower(_dbgCtx.selected, _dbgCtx.towerType);
			}
		}
		
		_gameContext->towers.showGUI(_selectedTower);
		
		gui::begin("Walker", 0);
		gui::Checkbox("Move", &_dbgCtx.move);
		//gui::Value("Pos", _walker.renderItem->getTransform().position,"%2.2f %2.2f %2.2f");
		//gui::Value("Grid Pos", _walker.gridPos);
		gui::Value("Next", _dbgCtx.nextPos);
		gui::Value("Next GP", _dbgCtx.flowNext);
		if (gui::Button("Start Walker")) {
			startWalker();
		}		
		if (_selectedTower != -1) {
			const Tower& t = _gameContext->towers.get(_selectedTower);
			if (gui::Button("Particles")) {
				_gameContext->particles->emittParticles(ds::vec3(t.position.x,0.4f, t.position.z), t.direction, 8);
			}
		}
	}
	gui::end();
}

