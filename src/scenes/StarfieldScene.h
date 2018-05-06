#pragma once
#include <ds_base_app.h>

struct GameContext;
class Billboards;

struct Star {
	ds::vec3 pos;
	ds::vec3 velocity;
};

const int STARFIELD_LAYERS = 3;
const int STARS_PER_LAYER = 128;
const int TOTAL_STARS = STARFIELD_LAYERS * STARS_PER_LAYER;

class StarfieldScene : public ds::BaseScene {

public:
	StarfieldScene(GameContext* gameContext);
	virtual ~StarfieldScene();
	void initialize();
	void update(float dt);
	void render();
private:
	GameContext* _gameContext;
	RID _gameViewPort;
	RID _gameRenderPass;
	Billboards* _billboards;
	Star _stars[TOTAL_STARS];
};