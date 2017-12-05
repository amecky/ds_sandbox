#pragma once
#include "..\TestApp.h"
#include "..\instancing\InstanceCommon.h"
#include "..\Mesh.h"
#include "..\kenney\Camera.h"
#include "..\utils\SimpleGrid.h"
#include "..\Material.h"
#include "..\utils\TransformComponent.h"
#include "..\instancing\TopDownCamera.h"

class TopDownObjViewer : public TestApp {

	enum ObjectState {
		IDLE,MOVING,STEPPING,ROTATING,WALKING
	};
public:
	TopDownObjViewer();

	virtual ~TopDownObjViewer();

	ds::RenderSettings getRenderSettings();

	bool usesGUI() {
		return true;
	}

	bool init();

	void tick(float dt);

	void render();

	void renderGUI();

private:
	bool prepareStep();
	AmbientLightningMaterial* _material;
	RID _objDrawItem;
	//InstanceBuffer _constantBuffer;
	//MultipleLightsBuffer _lightBuffer;
	SimpleGrid* _grid;
	transform_component _transform;
	Mesh _mesh;
	TopDownCamera* _fpsCamera;
	bool _moving;
	float _angle;
	ds::vec3 _startPosition;
	ds::vec3 _targetPosition;
	ds::vec3 _lightDir[3];
	float _dbgMoveTTL;
	float _dbgTTL;
	bool _dbgRotating;
	ds::vec3 _dbgRotation;
	ObjectState _state;
	float _startAngle;
	float _endAngle;
	int _steps;
	int _stepCounter;

};