#pragma once
#include "..\TestApp.h"
#include "..\kenney\Camera.h"
#include "BackgroundGrid.h"
#include "EmitterQueue.h"
#include "Cubes.h"

class InstanceTest : public TestApp {

public:
	InstanceTest();

	virtual ~InstanceTest();

	ds::RenderSettings getRenderSettings();

	bool usesGUI() {
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
	FPSCamera* _fpsCamera;
	BackgroundGrid _grid;
	EmitterQueue* _queue;
	Cubes _cubes;

	int _tmpX;
	int _tmpY;
	float _ttl;	
};