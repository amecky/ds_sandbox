#include "WarpingGridScene.h"
#include "..\GameContext.h"
#include <ds_tv.h>
#include <chrono>

WarpingGridScene::WarpingGridScene(GameContext* gameContext) : ds::BaseScene() , _gameContext(gameContext) {
	
}

WarpingGridScene::~WarpingGridScene() {
	delete _grid;
}

// ----------------------------------------------------
// init
// ----------------------------------------------------
void WarpingGridScene::initialize() {

	WarpingGridData gridData;
	gridData.width = 41;
	gridData.height = 22;
	gridData.cellSize = 40.0f;
	gridData.flashTTL = 0.4f;
	gridData.regularColor = ds::Color(64, 64, 64, 255);
	gridData.flashColor = ds::Color(192, 0, 0, 255);
	std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
	std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
	_grid = new WarpingGrid;
	_grid->createGrid(gridData);
	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
	DBG_LOG("creating the grid took: %d ms", std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count());
}



// ----------------------------------------------------
// tick
// ----------------------------------------------------
void WarpingGridScene::update(float dt) {

	if (ds::isMouseButtonPressed(0)) {
		_grid->applyForce(ds::getMousePosition(), _TV(0.01f), 80.0f, 120.0f);
	}
	if (ds::isMouseButtonPressed(1)) {
		_grid->applyForce(ds::getMousePosition(), _TV(0.01f), 80.0f);
	}

	_grid->tick(dt);
}

// ----------------------------------------------------
// render
// ----------------------------------------------------
void WarpingGridScene::render() {
	ds::vec2 wp(512, 384);
	_grid->render(wp);
}



