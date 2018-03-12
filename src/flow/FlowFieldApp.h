#pragma once
#include "..\TestApp.h"
#include "..\instancing\InstanceCommon.h"
#include "..\Mesh.h"
#include "..\kenney\Camera.h"
#include "..\utils\SimpleGrid.h"
#include "..\Material.h"
#include "..\utils\TransformComponent.h"
#include "..\utils\RenderItem.h"
#include "..\lib\DataArray.h"
#include "Grid.h"
#include "FlowField.h"
#include "Towers.h"
#include <vector>

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

struct PathItem {
	ds::vec3 pos;
	p2i grid;
	float direction;
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
	void OnButtonClicked(int index);
private:
	void moveWalkers(float dt);
	void updateOverlay();
	void addTower(const p2i& gridPos);
	void buildPath();
	void startWalker();
	AmbientLightningMaterial* _ambient_material;
	InstancedAmbientLightningMaterial* _material;
	InstancedRenderItem* _renderItem;
	InstancedRenderItem* _overlayItem;
	Towers _towers;
	
	RenderItem* _startItem;
	RenderItem* _endItem;
	RenderItem* _pathItem;
	RenderItem* _walkerItem;
	
	FPSCamera* _fpsCamera;
	ds::vec3 _lightDir[3];
	ID _ids[GRID_SIZE_X * GRID_SIZE_Y];
	ID _overlay_ids[GRID_SIZE_X * GRID_SIZE_Y];
	int _selected;
	Grid* _grid;
	p2i _startPoint;
	p2i _endPoint;
	FlowField* _flowField;
	int _selectedTower;
	std::vector<PathItem> _path;
	ds::DataArray<Walker> _walkers;

	int _dbgSelectedLight;
	bool _dbgMoveCamera;
	float _dbgLength;
	ds::vec3 _dbgNextPos;
	p2i _dbgFlowNext;
	ds::vec3 _dbgDiff;
	bool _dbgMove;
	ds::vec3 _dbgV;
	p2i _dbgSelected;
	bool _dbgShowOverlay;
	bool _dbgShowPath;
	bool _dbgHandleButtons;
};