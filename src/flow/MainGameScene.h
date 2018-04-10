#pragma once
#include <ds_base_app.h>
#include "..\instancing\InstanceCommon.h"
#include "..\Mesh.h"
#include "..\utils\Camera.h"
#include "IsometricCamera.h"
#include "..\Material.h"
#include "..\utils\TransformComponent.h"
#include "..\utils\RenderItem.h"
#include "..\lib\DataArray.h"
#include "Grid.h"
#include "FlowField.h"
#include "Towers.h"
#include <vector>
#include "FlowFieldContext.h"
#include "IsometricCamera.h"
#include "..\gpuparticles\ParticleManager.h"

struct DebugContext {
	int selectedLight;
	bool moveCamera;
	float length;
	ds::vec3 nextPos;
	p2i flowNext;
	ds::vec3 diff;
	bool move;
	ds::vec3 V;
	p2i selected;
	bool showOverlay;
	bool showPath;
	bool handleButtons;
	bool snapToGrid;
	bool isoCamera;
	int towerType;
};

struct GameContext;

class MainGameScene : public ds::BaseScene {

	enum ObjectState {
		IDLE,MOVING,STEPPING,ROTATING,WALKING
	};
public:
	MainGameScene(GameContext* gameContext);
	virtual ~MainGameScene();
	void initialize();
	void update(float dt);
	void render();
	void showGUI();
	void OnButtonClicked(int index);
private:
	void moveWalkers(float dt);
	void updateOverlay();
	void addTower(const p2i& gridPos, int type);
	void buildPath();
	void startWalker();

	GameContext* _gameContext;
	
	InstancedRenderItem* _renderItem;
	InstancedRenderItem* _overlayItem;
	
	
	RenderItem* _startItem;
	RenderItem* _endItem;
	RenderItem* _pathItem;
	RenderItem* _walkerItem;
	RenderItem* _cursorItem;
	
	FPSCamera* _fpsCamera;
	IsometricCamera* _isoCamera;

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

	DebugContext _dbgCtx;
};