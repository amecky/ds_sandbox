#include "SandboxApp.h"
#include <SpriteBatchBuffer.h>
#include <Windows.h>
#include <stb_image.h>
#include <ds_imgui.h>

ds::BaseApp *app = new SandboxApp();

SandboxApp::SandboxApp() : ds::BaseApp() {
	_settings.screenWidth = 1280;
	_settings.screenHeight = 720;
	_settings.windowTitle = "Flow";
	_settings.useIMGUI = true;
	_settings.clearColor = ds::Color(16, 16, 16, 255);
	_settings.guiToggleKey = 'O';
}


SandboxApp::~SandboxApp() {
	delete _clock;
	delete _flowFieldScene;
}

// ---------------------------------------------------------------
// initialize
// ---------------------------------------------------------------
void SandboxApp::initialize() {
	_flowFieldScene = new FlowFieldApp;
	_clock = new BinaryClock;
	pushScene(_flowFieldScene);
	//pushScene(_clock);
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
