#include "TowerTestScene.h"
#include <ds_imgui.h>
#include "..\utils\common_math.h"
#include "..\GameContext.h"

const ds::vec2 CENTER = ds::vec2(-2.5f, -2.5f);

TowerTestScene::TowerTestScene(GameContext* gameContext) : ds::BaseScene() , _gameContext(gameContext) {
	_grid = new Grid(5, 5);
	for (int i = 0; i < 5; ++i) {
		for (int j = 0; j < 5; ++j) {
			_grid->set(i, j, 0);
		}
	}	
	_grid->plane = Plane(ds::vec3(0.0f, 0.0f, 0.0f), ds::vec3(0.0f, -1.0f, 0.0f));	
	_dbgFollowCursor = true;


	_dbgLength = 0.0f;
	_dbgMove = true;
	_dbgNextPos = ds::vec3(0);
	_dbgSelected = p2i(-1, -1);
	_dbgShowOverlay = false;
	_selectedTower = 0;
	_dbgSelectedLight = 0;
	_dbgShowPath = true;
	_dbgHandleButtons = true;
	_dbgSnapToGrid = false;
	_dbgTowerType = 0;
}

TowerTestScene::~TowerTestScene() {	
	delete _grid;
	delete _isoCamera;
}

// ----------------------------------------------------
// init
// ----------------------------------------------------
void TowerTestScene::initialize() {
	
	_isoCamera = new IsometricCamera(&_camera);
	_isoCamera->setPosition(ds::vec3(0, 8, -6), ds::vec3(0.0f, 0.0f, 0.0f));

	_lightDir[0] = ds::vec3(1.0f,-0.31f,1.0f);
	_lightDir[1] = ds::vec3(0.6f,-0.28f,0.34f);
	_lightDir[2] = ds::vec3(-0.3f,0.7f,-0.2f);

	_gridItem = new InstancedRenderItem("basic_floor", _gameContext->instancedAmbientmaterial, 5 * 5);

	for (int y = 0; y < _grid->height; ++y) {
		for (int x = 0; x < _grid->width; ++x) {			
			int type = _grid->get(x,y);
			int idx = x + y * _grid->width;
			if (type == 0) {
				ds::vec3 p = ds::vec3(CENTER.x + x, 0.0f, CENTER.y + y);
				_ids[idx] = _gridItem->add();
				instance_item& item = _gridItem->get(_ids[idx]);
				item.transform.position = p;

			}
		}
	}
	
	_cursorItem = new RenderItem("cursor", _gameContext->ambientMaterial);

	_gameContext->towers.setWorldOffset(ds::vec2(-2.5f, -2.5f));
	
	addTower(p2i(2, 2), 0);

}

// -------------------------------------------------------------
// add tower
// -------------------------------------------------------------
void TowerTestScene::addTower(const p2i& gridPos, int type) {
	if (_grid->get(gridPos) == 0) {
		_grid->set(gridPos.x, gridPos.y, 1);
		_gameContext->towers.addTower(gridPos, type);
	}
}

// ----------------------------------------------------
// tick
// ----------------------------------------------------
void TowerTestScene::update(float dt) {
	
	_isoCamera->update(dt);
	
	_gameContext->towers.tick(dt);

	Ray r = get_picking_ray(_camera.projectionMatrix, _camera.viewMatrix);
	r.setOrigin(_camera.position);
	ds::vec3 ip = _grid->plane.getIntersection(r);
	ip.y = 0.1f;
	if (_dbgSnapToGrid) {
		p2i gp;
		if (convert(ip.x, ip.z, CENTER.x, CENTER.y, &gp)) {
			ds::vec3 np = ds::vec3(gp.x + CENTER.x, 0.0f, gp.y + CENTER.y);
			np.y = 0.1f;
			_cursorItem->getTransform().position = np;
		}
	}
	else {
		_cursorItem->getTransform().position = ip;
	}

	if (_dbgFollowCursor) {
		_gameContext->towers.rotateTowers(ip);
	}
}

// ----------------------------------------------------
// render
// ----------------------------------------------------
void TowerTestScene::render() {
	// update lights
	for (int i = 0; i < 3; ++i) {
		_gameContext->instancedAmbientmaterial->setLightDirection(i, _lightDir[i]);
		_gameContext->ambientMaterial->setLightDirection(i, normalize(_lightDir[i]));
	}

	_gridItem->draw(_basicPass, _camera.viewProjectionMatrix);

	_cursorItem->draw(_basicPass, _camera.viewProjectionMatrix);
	// towers
	_gameContext->towers.render(_basicPass, _camera.viewProjectionMatrix);
}

// ----------------------------------------------------
// renderGUI
// ----------------------------------------------------
void TowerTestScene::showGUI() {
	int state = 1;
	p2i sp = p2i(10, 710);
	gui::start(&sp, 320);
	gui::setAlphaLevel(0.5f);
	if (gui::begin("Debug", &state)) {
		gui::Value("FPS", ds::getFramesPerSecond());
		gui::Checkbox("Follow cursor", &_dbgFollowCursor);
	}
	gui::end();
}

