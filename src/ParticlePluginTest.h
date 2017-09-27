#pragma once
#include <ds_imgui.h>
#include <vector>

class SpriteBatchBuffer;
struct plugin_registry;
struct PluginInstance;
struct particle_manager;
struct ParticleData;

struct EnemySettings {
	ds::Color color;
	int pieces;
	float amplitude;
	float frequency;
	float radius;
	float radiusVariance;
	float radialVelocity;
	float scale;
};

class Enemy {

public:
	Enemy(EnemySettings* settings);
	~Enemy();
	void tick(float dt);
	void render(SpriteBatchBuffer* buffer);
	void resize();
private:
	ds::vec2 _position;
	ds::vec2 _velocity;
	EnemySettings* _settings;
	float* _radius;
	float _timer;
};

class ParticlePluginTest {

	struct Player {
		ds::vec2 position;
		ds::vec2 previous;
		ds::vec2 trailPos;
		float timer;
		float trailTimer;
		float angle;
	};

public:	
	ParticlePluginTest(plugin_registry* registry,SpriteBatchBuffer* buffer);
	~ParticlePluginTest();
	void render();
	void renderGUI();
	void tick(float dt);
private:
	SpriteBatchBuffer* _sprites;	
	void renderParticles(ParticleData* data, const ds::vec4& rect);
	int _dialogState;
	p2i _dialogPos;
	int _numParticles;
	float _emissionTime;
	SpriteBatchBuffer* _particleBuffer;
	particle_manager* _particles;
	PluginInstance* _instance;
	ParticleData* _data;
	ParticleData* _trailData;
	float _timer;
	bool _running;
	float _delay;
	Player _player;

	std::vector<Enemy*> _enemies;
};

