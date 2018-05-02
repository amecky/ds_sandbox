#pragma once
#include <ds_base_app.h>
#define DS_PERF_PANEL
#include "PerfPanel.h"
#include <ds_profiler.h>
#include <ds_imgui.h>
#include "flow\MainGameScene.h"
#include "scenes\TowerTestScene.h"
#include "scenes\ParticlesTestScene.h"
#include "scenes\AsteroidsScene.h"
#include "GameContext.h"

struct SceneDescriptor {
	const char* name;
	ds::BaseScene* scene;
};

class SceneListModel : public gui::ListBoxModel {

public:
	SceneListModel() : gui::ListBoxModel() {}
	~SceneListModel() {}
	uint32_t size() const {
		return _descriptors.size();
	}
	const char* name(uint32_t index) const {
		return _descriptors[index].name;
	}
	void add(const char* name, ds::BaseScene* scene) {
		SceneDescriptor desc;
		desc.name = name;
		desc.scene = scene;
		_descriptors.push_back(desc);
	}
	const SceneDescriptor& get(int selection) const {
		return _descriptors[selection];
	}
private:
	std::vector<SceneDescriptor> _descriptors;
};

class SandboxApp: public ds::BaseApp {

public:
	SandboxApp();
	~SandboxApp();
	void initialize();
	void handleEvents(ds::EventStream* events);
	void update(float dt);
	void drawTopPanel();
	void drawLeftPanel();
	void drawBottomPanel();
private:
	
	MainGameScene* _flowFieldScene;
	TowerTestScene* _towerTestScene;
	ParticlesTestScene* _particlesTestScene;
	AsteroidsScene* _asteroidsScene;
	GameContext* _gameContext;
	SceneListModel _sceneListModel;
	bool _showScenes;
	
};