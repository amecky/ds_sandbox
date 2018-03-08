#pragma once
#include "..\TestApp.h"
#include "..\instancing\InstanceCommon.h"
#include "..\Mesh.h"
#include "..\kenney\Camera.h"
#include "..\utils\SimpleGrid.h"
#include "..\Material.h"
#include "..\utils\TransformComponent.h"
#include "..\utils\RenderItem.h"
#include "Grid.h"
#include "FlowField.h"

struct Walker {
	ID id;
	p2i gridPos;
	float velocity;
	ds::vec3 pos;
	float rotation;
	int definitionIndex;
	int energy;
	RenderItem* renderItem;
};

class FlowFieldApp : public TestApp {

	enum ObjectState {
		IDLE,MOVING,STEPPING,ROTATING,WALKING
	};
public:
	FlowFieldApp();
	virtual ~FlowFieldApp();
	ds::RenderSettings getRenderSettings();
	bool usesGUI() {
		return true;
	}
	bool init();
	void tick(float dt);
	void render();
	void renderGUI();
private:
	void moveWalkers(float dt);
	AmbientLightningMaterial* _ambient_material;
	InstancedAmbientLightningMaterial* _material;
	InstancedRenderItem* _renderItem;
	InstancedRenderItem* _overlayItem;
	//RenderItem* _walker;
	Walker _walker;
	FPSCamera* _fpsCamera;
	ds::vec3 _lightDir[3];
	ID _ids[GRID_SIZE_X * GRID_SIZE_Y];
	int _selected;
	Grid* _grid;
	p2i _startPoint;
	p2i _endPoint;
	FlowField* _flowField;

	float _dbgLength;
	ds::vec3 _dbgNextPos;
	p2i _dbgFlowNext;
	ds::vec3 _dbgDiff;
	bool _dbgMove;
	ds::vec3 _dbgV;
};