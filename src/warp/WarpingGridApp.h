#pragma once
#include "..\TestApp.h"
#include "..\kenney\Camera.h"
#include "..\utils\comon_math.h"
#include "..\instancing\InstanceCommon.h"
#include <vector>
#include "WarpingGrid.h"

class WarpingGridApp : public TestApp {

public:
	WarpingGridApp();

	virtual ~WarpingGridApp();

	ds::RenderSettings getRenderSettings();

	bool usesGUI() {
		return true;
	}

	bool useFixedTimestep() const {
		return true;
	}

	bool init();

	void tick(float dt);

	void render();

	void renderGUI();

private:
	RID _basicPass;
	ds::Camera _camera;
	FPSCamera* _fpsCamera;
	WarpingGrid* _grid;
	int _dbgX;
	int _dbgY;
	float _dbgForce;
};