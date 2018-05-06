#include "StarfieldScene.h"
#include <ds_imgui.h>
#include "..\GameContext.h"
#include "..\instancing\Billboards.h"

StarfieldScene::StarfieldScene(GameContext* gameContext) : ds::BaseScene() , _gameContext(gameContext) {
	
}

StarfieldScene::~StarfieldScene() {
	delete _billboards;
}

// ----------------------------------------------------
// init
// ----------------------------------------------------
void StarfieldScene::initialize() {

	ds::ViewportInfo vpInfo = { 450, 40, 1280, 720, 0.0f, 1.0f };
	_gameViewPort = ds::createViewport(vpInfo);

	ds::RenderPassInfo rpInfo = { &_camera, _gameViewPort, ds::DepthBufferState::ENABLED, 0, 0 };
	_gameRenderPass = ds::createRenderPass(rpInfo);

	_billboards = new Billboards;
	RID textureID = loadImageFromFile("content\\particles.png");
	_billboards->init(textureID);

	for (int i = 0; i < STARFIELD_LAYERS; ++i) {
		for (int j = 0; j < STARS_PER_LAYER; ++j) {
			Star& s = _stars[i * STARS_PER_LAYER + j];
			s.pos = ds::vec3(ds::random(-8.0f, 8.0f), ds::random(-4.0f, 4.0f), static_cast<float>(i));
			float v = static_cast<float>(i) * 0.5f + ds::random(0.2f,0.3f);
			s.velocity = ds::vec3( -v, 0.0f, 0.0f);
		}
	}

}



// ----------------------------------------------------
// tick
// ----------------------------------------------------
void StarfieldScene::update(float dt) {
	for (int i = 0; i < TOTAL_STARS; ++i) {
		_stars[i].pos += _stars[i].velocity * dt;
		if (_stars[i].pos.x < -10.0f) {
			_stars[i].pos.x = 10.0f;
			_stars[i].pos.y = ds::random(-8.0f, 8.0f);
		}
	}
}

// ----------------------------------------------------
// render
// ----------------------------------------------------
void StarfieldScene::render() {
	_billboards->begin();
	for (int i = 0; i < TOTAL_STARS; ++i) {
		_billboards->add(_stars[i].pos, ds::vec2(0.05f,0.05f), ds::vec4(0, 0, 128, 128));
	}
	_billboards->end();
	_billboards->render(_gameRenderPass, _camera.viewProjectionMatrix);
}



