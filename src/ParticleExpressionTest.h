#pragma once
#include <diesel.h>
#include <ds_imgui.h>
#include <SpriteBatchBuffer.h>
#include <ds_vm.h>
#include "Particles.h"
#include <ds_profiler.h>
#include "plugins\ApiRegistry.h"

struct log_panel_plugin;

class ParticleExpressionTest {

public:	
	ParticleExpressionTest(const ds_api_registry& registry,SpriteBatchBuffer* buffer);
	~ParticleExpressionTest();
	void render();
	void renderGUI();
private:
	void loadExpressionsFile(Particlesystem* system);
	void renderExpressions(Particlesystem* system);
	void convertFloat(float v,int precision);
	SpriteBatchBuffer* _sprites;	
	int _dialogState;
	p2i _dialogPos;
	int _numParticles;
	float _emissionTime;
	bool _showParticleInfo;
	bool _showExpressions;
	int _selectedType;
	int _selectedExpression;
	char _tmpBuffer[64];
	
	log_panel_plugin* _logPanelPlugin;
	Particlesystem* _system;
};

