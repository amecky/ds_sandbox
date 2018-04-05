#include "SandboxApp.h"
#include <SpriteBatchBuffer.h>
#include <Windows.h>
#include <stb_image.h>
#include <ds_imgui.h>
#include "Material.h"

ds::BaseApp *app = new SandboxApp();

SandboxApp::SandboxApp() : ds::BaseApp() {
	_settings.screenWidth = 1280;
	_settings.screenHeight = 720;
	_settings.windowTitle = "Flow";
	_settings.useIMGUI = true;
	_settings.clearColor = ds::Color(16, 16, 16, 255);
	_settings.guiToggleKey = 'O';
	_gameContext = new GameContext;
}


SandboxApp::~SandboxApp() {
	delete _flowFieldScene;
	delete _towerTestScene;
	delete _gameContext->particles;
	delete _gameContext->ambientMaterial;
	delete _gameContext->instancedAmbientmaterial;
	delete _gameContext;
}

// ---------------------------------------------------------------
// initialize
// ---------------------------------------------------------------
void SandboxApp::initialize() {
	_gameContext->ambientMaterial = new AmbientLightningMaterial;
	_gameContext->instancedAmbientmaterial = new InstancedAmbientLightningMaterial;
	_gameContext->towers.init(_gameContext->ambientMaterial);
	RID textureID = loadImageFromFile("content\\TextureArray.png");
	_gameContext->particles = new ParticleManager(textureID);
	_flowFieldScene = new MainGameScene(_gameContext);
	_towerTestScene = new TowerTestScene(_gameContext);
	_particlesTestScene = new ParticlesTestScene(_gameContext);
	//pushScene(_flowFieldScene);
	//pushScene(_towerTestScene);
	pushScene(_particlesTestScene);

	ds::vec3 eyePos(0.0f, 2.0f, -1.0f);
	ds::vec3 pos(0.0f);

	ds::vec3 look = normalize(eyePos - pos);
	ds::vec3 right = normalize(cross(ds::vec3(0.0f, 1.0f, 0.0f), look));
	ds::vec3 up = normalize(cross(look, right));

	ds::vec3 fp[4];
	for (int i = 0; i < 4; ++i) {
		float hw = (i & 2) ? 0.5 : -0.5;
		float hh = (i % 2) ? 0.5 : -0.5;

		fp[i] = ds::vec3(pos + hw * right - hh * up);
	}


}

// ---------------------------------------------------------------
// handle events
// ---------------------------------------------------------------
void SandboxApp::handleEvents(ds::EventStream* events) {
	if (events->num() > 0) {
		for (uint32_t i = 0; i < events->num(); ++i) {
			int type = events->getType(i);			
			if (type == 102) {
				stopGame();
			}
		}
	}
}


// ---------------------------------------------------------------
// update
// ---------------------------------------------------------------
void SandboxApp::update(float dt) {
	
}
