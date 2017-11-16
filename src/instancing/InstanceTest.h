#pragma once
#include "..\TestApp.h"
#include "TopDownCamera.h"
#include "BackgroundGrid.h"
#include "EmitterQueue.h"
#include "Cubes.h"
#include "Player.h"

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
	RID _basicPass;
	RID _instanceVertexBuffer;
	ds::Camera _camera;
	TopDownCamera* _fpsCamera;
	BackgroundGrid* _grid;
	EmitterQueue* _queue;
	Cubes _cubes;
	Player* _player;
	BackgroundGridSettings _gridSettings;
	int _tmpX;
	int _tmpY;
};