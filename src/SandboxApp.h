#pragma once
#include <ds_base_app.h>
#define DS_PERF_PANEL
#include "PerfPanel.h"
#include <ds_profiler.h>
#define DS_VM_IMPLEMENTATION
#include <ds_vm.h>
#include "flow\MainGameScene.h"
#include "scenes\TowerTestScene.h"
#include "scenes\ParticlesTestScene.h"
#include "GameContext.h"

class SandboxApp: public ds::BaseApp {

public:
	SandboxApp();
	~SandboxApp();
	void initialize();
	void handleEvents(ds::EventStream* events);
	void update(float dt);
	void showMenu();
	void showBottomPanel();
private:
	
	MainGameScene* _flowFieldScene;
	TowerTestScene* _towerTestScene;
	ParticlesTestScene* _particlesTestScene;
	GameContext* _gameContext;
};