#pragma once
//#include <diesel.h>
#include <ds_imgui.h>
#include <SpriteBatchBuffer.h>
//#include <ds_vm.h>
//#include "Particles.h"
//#include <ds_profiler.h>


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

