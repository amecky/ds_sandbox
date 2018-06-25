#pragma once
#include <ds_base_app.h>
#include "WarpingGrid.h"

struct GameContext;

class WarpingGridScene : public ds::BaseScene {

public:
	WarpingGridScene(GameContext* gameContext);
	virtual ~WarpingGridScene();
	void initialize();
	void update(float dt);
	void render();
private:
	GameContext* _gameContext;
	WarpingGrid* _grid;
};