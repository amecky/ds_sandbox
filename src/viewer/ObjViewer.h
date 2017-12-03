#pragma once
#include "..\TestApp.h"
#include "..\instancing\InstanceCommon.h"
#include "..\Mesh.h"
#include "..\kenney\Camera.h"
#include "..\utils\SimpleGrid.h"
#include "..\Material.h"
#include "..\utils\TransformComponent.h"

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
	AmbientLightningMaterial* _material;
	RID _objDrawItem;
	//InstanceBuffer _constantBuffer;
	//MultipleLightsBuffer _lightBuffer;
	SimpleGrid* _grid;
	transform_component _transform;
	Mesh _mesh;
	FPSCamera* _fpsCamera;
	bool _moving;
	ds::vec3 _startPosition;
	ds::vec3 _targetPosition;
	ds::vec3 _scale;
	ds::vec3 _lightDir[3];
	float _dbgMoveTTL;
	float _dbgTTL;
	bool _dbgRotating;

};