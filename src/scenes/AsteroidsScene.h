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
#include "..\utils\DynamicPath.h"
#include "..\instancing\Billboards.h"

struct GameContext;

struct Player {
	RenderItem* renderItem;
	ds::vec3 position;
	float angle;
};

struct Bullet {
	ID id;
	ds::vec3 pos;
	ds::vec3 velocity;
};

void move_player(Player* player, float dt);

struct Enemy {
	ID id;
	ID instance_id;
	ds::vec3 velocity;
	float timer;
	int animationFlags; // 1 = move 2 = rotate 3 = scale
	ds::vec3 force;
};

class AsteroidsScene : public ds::BaseScene {

public:
	AsteroidsScene(GameContext* gameContext);
	virtual ~AsteroidsScene();
	void initialize();
	void update(float dt);
	void render();
	void drawLeftPanel();
	void drawTopPanel();
private:
	void addEnemy();
	void addBullet();

	float _bulletTimer;
	bool _shooting;

	bool _autoEmitt;
	float _emittTimer;

	GameContext* _gameContext;
	Billboards* _bulletBillboards;
	TopDownCamera* _topDownCamera;
	//FPSCamera* _fpsCamera;
	ds::vec3 _lightDir[3];
	InstancedRenderItem* _enemiesItem;
	ds::DataArray<Enemy> _enemies;
	ds::DataArray<Bullet> _bullets;
	Player _player;
	ds::vec3 _dbgCameraRotation;
	RID _gameRenderPass;
	RID _particleRenderPass;
	ds::FloatPath _scalePath;
};