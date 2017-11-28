#pragma once
#include "..\TestApp.h"
#include "..\kenney\Camera.h"
#include "TopDownCamera.h"
#include "BackgroundGrid.h"
#include "EmitterQueue.h"
#include "Cubes.h"
#include "Player.h"
#include "Billboards.h"
#include "..\utils\comon_math.h"
#include "..\warp\WarpingGrid.h"
#include "..\utils\EventStream.h"
#include "Border.h"
#include "..\gpuparticles\GPUParticlesystem.h"

struct BulletSettings {
	float velocity;
	ds::vec2 scale;
	ds::vec4 boundingBox;
	float fireRate;
};

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
	void emittExplosion(const ds::vec3& pos);
	void tweakableGUI(const char* category);
	void addBullet();
	void emittCubes(int side, int num);
	void manageBullets(float dt);
	void handleEvents();
	void movePlayer(float dt);
	RID _basicPass;
	RID _particlePass;
	RID _instanceVertexBuffer;
	ds::Camera _camera;
	FPSCamera* _fpsCamera;
	TopDownCamera* _topDownCamera;
	BulletSettings _bulletSettings;
	ParticleSettings _explosionSettings;
	//BackgroundGrid* _grid;
	EmitterQueue* _queue;
	Cubes _cubes;
	Player* _player;
	BackgroundGridSettings _gridSettings;
	WarpingGrid* _warpingGrid;
	Billboards _billboards;
	GPUParticlesystem* _particleSystem;
	ParticleDescriptor _particleDescriptor;
	bool _showGUI;
	bool _pressed;
	int _tmpX;
	int _tmpY;
	int _tmpSide;
	int _cameraMode;
	int _selectedTab;
	Bullet _bullets[256];
	int _numBullets;
	bool _shooting;
	float _bulletTimer;
	ds::vec3 _cursorPos;
	ds::EventStream _events;
	bool _running;
	Border _border;
};