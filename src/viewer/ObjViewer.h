#pragma once
#include "..\TestApp.h"
#include "..\instancing\InstanceCommon.h"
#include "..\Mesh.h"
#include "..\kenney\Camera.h"
#include "..\utils\SimpleGrid.h"

class ObjViewer : public TestApp {

public:
	ObjViewer();

	virtual ~ObjViewer();

	ds::RenderSettings getRenderSettings();

	bool usesGUI() {
		return true;
	}

	bool init();

	void tick(float dt);

	void render();

	void renderGUI();

private:
	RID _objDrawItem;
	InstanceBuffer _constantBuffer;
	MultipleLightsBuffer _lightBuffer;
	SimpleGrid* _grid;
	ds::vec3 _worldPos;
	Mesh _mesh;
	FPSCamera* _fpsCamera;
	float _moveTimer;
	bool _moving;
	ds::vec3 _startPosition;
	ds::vec3 _targetPosition;
	ds::vec3 _scale;
	ds::vec3 _lightPos[3];
	float _dbgMoveTTL;
};