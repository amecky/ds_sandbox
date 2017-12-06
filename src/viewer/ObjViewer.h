#pragma once
#include "..\TestApp.h"
#include "..\instancing\InstanceCommon.h"
#include "..\Mesh.h"
#include "..\kenney\Camera.h"
#include "..\utils\SimpleGrid.h"
#include "..\Material.h"
#include "..\utils\TransformComponent.h"
#include "..\utils\RenderItem.h"

class ObjViewer : public TestApp {

	enum ObjectState {
		IDLE,MOVING,STEPPING,ROTATING,WALKING
	};
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
	bool prepareStep();
	AmbientLightningMaterial* _material;
	render_item _renderItem;

	//RID _objDrawItem;
	//InstanceBuffer _constantBuffer;
	//MultipleLightsBuffer _lightBuffer;
	SimpleGrid* _grid;
	//transform_component _transform;
	//Mesh _mesh;
	FPSCamera* _fpsCamera;
	bool _moving;
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