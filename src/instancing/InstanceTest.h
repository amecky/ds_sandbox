#pragma once
#include "..\TestApp.h"
#include "..\kenney\Camera.h"
#include "TopDownCamera.h"
#include "BackgroundGrid.h"
#include "EmitterQueue.h"
#include "Cubes.h"
#include "Player.h"
#include "Billboards.h"
#include "..\utils\comon_math.h"
#include "..\warp\WarpingGrid.h"

class InstanceTest : public TestApp {

public:
	InstanceTest();

	virtual ~InstanceTest();

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
	void addBullet();
	void emittCubes(int side, int num);
	RID _basicPass;
	RID _instanceVertexBuffer;
	ds::Camera _camera;
	FPSCamera* _fpsCamera;
	TopDownCamera* _topDownCamera;
	//BackgroundGrid* _grid;
	EmitterQueue* _queue;
	Cubes _cubes;
	Player* _player;
	BackgroundGridSettings _gridSettings;
	WarpingGridBuffer _settings;
	WarpingGrid* _warpingGrid;
	Billboards _billboards;
	int _tmpX;
	int _tmpY;
	int _tmpSide;
	int _cameraMode;
	Bullet _bullets[256];
	int _numBullets;
	bool _shooting;
	float _bulletTimer;
	ds::vec3 _cursorPos;
};