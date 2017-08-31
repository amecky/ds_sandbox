#pragma once
#include <diesel.h>
#include <ds_imgui.h>
#include <SpriteBatchBuffer.h>
#include <ds_vm.h>
#include "Particles.h"
#include <ds_profiler.h>

class ParticleExpressionTest {

public:
	ParticleExpressionTest(SpriteBatchBuffer* buffer);
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
	bool _showParticleInfo;
	int _selectedType;
	int _selectedExpression;
	char _tmpBuffer[64];
	
	Particlesystem* _system;
};

