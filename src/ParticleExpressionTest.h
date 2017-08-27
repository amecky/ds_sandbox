#pragma once
#include <diesel.h>
#include <ds_imgui.h>
#include <SpriteBatchBuffer.h>
#include <ds_vm.h>
#include "Particles.h"
#include <ds_profiler.h>

struct ParticleExpression {
	char source[256];
	vm::Expression expression;
};

class ParticleExpressionTest {

public:
	ParticleExpressionTest(SpriteBatchBuffer* buffer);
	~ParticleExpressionTest();
	void render();
	void renderGUI();
private:
	void loadExpressionsFile(const char* name, ParticleExpression* expressions, ParticleEmitter* ret);
	void emitt(uint16_t num, const ds::vec2& pos);
	void renderExpressions(ParticleEmitter& emitter, ParticleExpression* expressions);
	void moveParticles();
	void manageLifecycles();
	void updateParticles();
	SpriteBatchBuffer* _sprites;	
	int _dialogState;
	p2i _dialogPos;
	int _numParticles;
	bool _showParticleInfo;
	int _selectedType;
	int _selectedExpression;
	Particles _particles;
	vm::Context _vmCtx;
	ParticleEmitter emitter;
	ParticleEmitter modules;
	ParticleExpression _moduleExpressions[NUM_CHANNELS];
	ParticleExpression _emitterExpressions[NUM_CHANNELS];
};

