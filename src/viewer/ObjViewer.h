#pragma once
#include "..\TestApp.h"
#include "..\instancing\InstanceCommon.h"
#include "..\Mesh.h"
#include "..\kenney\Camera.h"

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
	RID _gridDrawItem;
	RID _nextDrawItem;
	InstanceBuffer _constantBuffer;
	InstanceLightBuffer _lightBuffer;
	ds::vec3 _worldPos;
	Mesh _mesh;
	Mesh _grid;
	FPSCamera* _fpsCamera;
	float _moveTimer;
	bool _moving;
	ds::vec3 _startPosition;
	ds::vec3 _targetPosition;
	float _dbgMoveTTL;
};