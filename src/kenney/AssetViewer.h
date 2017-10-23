#pragma once
#include "..\TestApp.h"
#include "Scene.h"

class AssetViewer : public TestApp {

public:
	AssetViewer();

	virtual ~AssetViewer();

	ds::RenderSettings getRenderSettings();

	bool init();

	void tick(float dt);

	void render();

private:
	Scene _scene;
};