#pragma once
#include "..\TestApp.h"
#include "..\kenney\Camera.h"
#include "..\utils\comon_math.h"
#include "..\instancing\InstanceCommon.h"

struct GridPoint {
	ds::vec3 position;
};

struct WarpingGridVertex {
	ds::vec3 pos;
	ds::Color color;
	ds::vec3 n;	
};

const int GRID_SIZE_X = 20;
const int GRID_SIZE_Y = 15;
const int TOTAL_GRID_SIZE = GRID_SIZE_X * GRID_SIZE_Y;

class WarpingGrid : public TestApp {

public:
	WarpingGrid();

	virtual ~WarpingGrid();

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
	GridPoint* _points;
	uint16_t* _indices;
	WarpingGridVertex* _vertices;
	RID _drawItem;
	RID _basicPass;
	InstanceLightBuffer _lightBuffer;
	InstanceBuffer _constantBuffer;
	ds::Camera _camera;
	FPSCamera* _fpsCamera;
	ds::vec3 _lightPos;
	RID _cubeBuffer;
};