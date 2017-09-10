#pragma once
#include <ds_imgui.h>

class SpriteBatchBuffer;
struct plugin_registry;
struct PluginInstance;
struct particle_manager;
struct ParticleData;

class ParticlePluginTest {

public:	
	ParticlePluginTest(const plugin_registry& registry,SpriteBatchBuffer* buffer);
	~ParticlePluginTest();
	void render();
	void renderGUI();
	void tick(float dt);
private:
	SpriteBatchBuffer* _sprites;	
	int _dialogState;
	p2i _dialogPos;
	int _numParticles;
	float _emissionTime;
	
	particle_manager* _particles;
	PluginInstance* _instance;
	ParticleData* _data;
	float _timer;
	bool _running;
	float _delay;
};

