#pragma once
#include <ds_base_app.h>
#define DS_PERF_PANEL
#include "PerfPanel.h"
#define DS_PROFILER_IMPLEMENTATION
#include <ds_profiler.h>
#define DS_VM_IMPLEMENTATION
#include <ds_vm.h>
#include "flow\MainGameScene.h"
#include "scenes\TowerTestScene.h"
#include "GameContext.h"

class SandboxApp: public ds::BaseApp {

public:
	SandboxApp();
	~SandboxApp();
	void initialize();
	void handleEvents(ds::EventStream* events);
	void update(float dt);
private:
	MainGameScene* _flowFieldScene;
	TowerTestScene* _towerTestScene;
	GameContext* _gameContext;
};