#pragma once
#include "..\instancing\InstanceCommon.h"
#include "..\Mesh.h"
#include "..\utils\Camera.h"
#include "..\flow\IsometricCamera.h"
#include "..\Material.h"
#include "..\utils\TransformComponent.h"
#include "..\utils\RenderItem.h"
#include "..\lib\DataArray.h"
#include "..\flow\Grid.h"
#include "..\flow\Towers.h"
#include <vector>
#include "..\flow\FlowFieldContext.h"
#include <ds_base_app.h>

struct GameContext;

class TowerTestScene : public ds::BaseScene {

public:
	TowerTestScene(GameContext* gameContext);
	virtual ~TowerTestScene();
	void initialize();
	void update(float dt);
	void render();
	void showGUI();
private:
	void addTower(const p2i& gridPos, int type);

	GameContext* _gameContext;
	RenderItem* _cursorItem;
	IsometricCamera* _isoCamera;
	ds::vec3 _lightDir[3];
	Grid* _grid;
	int _selectedTower;
	ID _ids[5 * 5];
	InstancedRenderItem* _gridItem;

	bool _dbgFollowCursor;

	int _dbgSelectedLight;
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
	bool _dbgSnapToGrid;
	int _dbgTowerType;
};