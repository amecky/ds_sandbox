#pragma once
#include "..\TestApp.h"
#include "..\kenney\Camera.h"
#include "TopDownCamera.h"
//#include "BackgroundGrid.h"
#include "EmitterQueue.h"
#include "Cubes.h"
#include "Player.h"
#include "Billboards.h"
#include "..\utils\comon_math.h"
#include "..\warp\WarpingGrid.h"
#include "..\utils\EventStream.h"
#include "Bullets.h"
#include "..\Material.h"
#include "..\gpuparticles\GPUParticlesystem.h"
#include "..\utils\RenderItem.h"
#include "..\enemies\RingEnemies.h"
#include "..\background\Doors.h"

struct ParticleSettings {
	int num;
	ds::vec2 radius;
	ds::vec2 angle;
	ds::vec2 rotationSpeed;
	ds::vec2 ttl;
	int alignParticle;
	ds::vec2 radialVelocity;
	ds::vec2 radialAcceleration;
	ds::vec2 scale;
	ds::vec2 scaleVariance;
	ds::vec2 growth;
	ds::Color startColor;
	ds::Color endColor;
};
class InstanceTest : public TestApp {

public:
	InstanceTest();

	virtual ~InstanceTest();

	ds::RenderSettings getRenderSettings();

	bool usesGUI() {
		return true;
	}

	bool useFixedTimestep() const {
		return true;
	}

	bool init();

	void tick(float dt);

	void render();

	void renderGUI();

	const char* getSettingsFileName() const {
		return "data\\settings.json";
	}

private:	
	void registerParticleSettings(const char* category, ParticleSettings* settings);
	void emittParticles(const ds::vec3& pos, const ParticleSettings& settings);
	void tweakableGUI(const char* category);
	void emittCubes(int side, int num);
	void handleEvents();
	void movePlayer(float dt);
	RID _basicPass;
	RID _particlePass;
	RID _instanceVertexBuffer;
	ds::Camera _camera;
	FPSCamera* _fpsCamera;
	TopDownCamera* _topDownCamera;
	bool _useTopDown;
	ParticleSettings _explosionSettings;
	ParticleSettings _bulletExplosionSettings;
	EmitterQueue* _queue;
	bool _dbgGameMode;
	float _gameModeTimer;
	float _gameModeTTL;
	CubesSettings _cubesSettings;
	Cubes* _cubes;
	Player _player;
	WarpingGrid* _warpingGrid;
	GPUParticlesystem* _particleSystem;
	ParticleDescriptor _particleDescriptor;
	bool _showGUI;
	bool _pressed;
	int _tmpX;
	int _tmpY;
	int _tmpSide;
	int _selectedTab;
	
	bullets::Context _bulletsCtx;

	ds::vec3 _cursorPos;
	ds::EventStream _events;
	bool _running;
	//Border _border;
	bool _dbgDoors;
	int _dbgFixedOffset;
	Doors* _doors;
	instanced_render_item _doors_render_item;
	ds::vec3 _cameraPos;
	AmbientLightningMaterial* _ambient_material;
	InstancedAmbientLightningMaterial* _material;
	InstancedMaterial* _instanced_material;
	instanced_render_item _griddies;
	instanced_render_item _cube_item;
	ds::vec3 _lightDir[3];
	RingEnemies* _ringEnemies;
};