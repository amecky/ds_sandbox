#include "SandboxApp.h"
#include <SpriteBatchBuffer.h>
#include <Windows.h>
#include <stb_image.h>
#include <ds_imgui.h>
#include "Material.h"
#include <ds_logpanel.h>

ds::BaseApp *app = new SandboxApp();

void my_debug(const LogLevel& level, const char* message) {
	OutputDebugString(message);
	OutputDebugString("\n");
	logpanel::add_line(message);
}

SandboxApp::SandboxApp() : ds::BaseApp() {
	_settings.screenWidth = 1680;
	_settings.screenHeight = 920;
	_settings.windowTitle = "Flow";
	_settings.useIMGUI = true;
	_settings.clearColor = ds::Color(16, 16, 16, 255);
	_settings.guiToggleKey = 'O';
	_gameContext = new GameContext;
	logpanel::init(32);
	_showScenes = false;
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
	ds::setLogHandler(my_debug);
	_gameContext->ambientMaterial = new AmbientLightningMaterial;
	_gameContext->instancedAmbientmaterial = new InstancedAmbientLightningMaterial;
	_gameContext->towers.init(_gameContext->ambientMaterial);
	RID textureID = loadImageFromFile("content\\particles.png");
	_gameContext->particleContext = particles::create_context(textureID);
	_flowFieldScene = new MainGameScene(_gameContext);
	_sceneListModel.add("Flowfield", _flowFieldScene);
	_towerTestScene = new TowerTestScene(_gameContext);
	_sceneListModel.add("TowerTest", _towerTestScene);
	_particlesTestScene = new ParticlesTestScene(_gameContext);
	_sceneListModel.add("Particle Test", _particlesTestScene);
	//pushScene(_flowFieldScene);
	//pushScene(_towerTestScene);
	pushScene(_particlesTestScene);
}

// ---------------------------------------------------------------
// showMenu
// ---------------------------------------------------------------
void SandboxApp::drawTopPanel() {
	gui::Value("FPS", ds::getFramesPerSecond());
	if (gui::Button("Toggle Update")) {
		_updateActive = !_updateActive;
	}
	if (!_updateActive) {
		if (gui::Button("Single step")) {
			doSingleStep();
		}
	}
	if (gui::Button("Toggle Scenes")) {
		_showScenes = !_showScenes;
	}
}

void SandboxApp::drawLeftPanel() {
	if (_showScenes) {
		gui::ListBox("Scenes", _sceneListModel, 4);
		gui::beginGroup();
		if (gui::Button("Push")) {
			if (_sceneListModel.hasSelection()) {
				const SceneDescriptor& desc = _sceneListModel.get(_sceneListModel.getSelection());
				DBG_LOG("selected: %s", desc.name);
				popScene();
				pushScene(desc.scene);
			}
		}
		if (gui::Button("Pop")) {
			popScene();
		}
		gui::endGroup();
	}
}

void SandboxApp::drawBottomPanel() {
	logpanel::draw_gui(8);
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
