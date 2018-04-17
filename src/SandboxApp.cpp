#include "SandboxApp.h"
#include <SpriteBatchBuffer.h>
#include <Windows.h>
#include <stb_image.h>
#include <ds_imgui.h>
#include "Material.h"

ds::BaseApp *app = new SandboxApp();

SandboxApp::SandboxApp() : ds::BaseApp() {
	_settings.screenWidth = 1680;
	_settings.screenHeight = 780;
	_settings.windowTitle = "Flow";
	_settings.useIMGUI = true;
	_settings.clearColor = ds::Color(16, 16, 16, 255);
	_settings.guiToggleKey = 'O';
	_gameContext = new GameContext;
}


SandboxApp::~SandboxApp() {
	delete _flowFieldScene;
	delete _towerTestScene;
	delete _gameContext->particleContext->particleSystem;
	delete _gameContext->particleContext;
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
	RID textureID = loadImageFromFile("content\\particles.png");
	_gameContext->particleContext = particles::create_context(textureID);
	_flowFieldScene = new MainGameScene(_gameContext);
	_towerTestScene = new TowerTestScene(_gameContext);
	_particlesTestScene = new ParticlesTestScene(_gameContext);
	//pushScene(_flowFieldScene);
	//pushScene(_towerTestScene);
	pushScene(_particlesTestScene);
}

// ---------------------------------------------------------------
// showMenu
// ---------------------------------------------------------------
void SandboxApp::showMenu() {
	if (gui::Button("Toggle Update")) {
		_updateActive = !_updateActive;
	}
	if (!_updateActive) {
		if (gui::Button("Single step")) {
			doSingleStep();
		}
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
