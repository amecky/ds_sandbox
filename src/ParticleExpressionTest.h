#pragma once
#include <diesel.h>
#include <ds_imgui.h>
#include <SpriteBatchBuffer.h>
#include <ds_vm.h>
#include "Particles.h"

class ParticleExpressionTest {

public:
	ParticleExpressionTest(SpriteBatchBuffer* buffer);
	~ParticleExpressionTest();
	void render();
	void renderGUI();
private:
	void emitt(uint16_t);
	SpriteBatchBuffer* _sprites;	
	int _dialogState;
	p2i _dialogPos;
	Particles _particles;
	vm::Context _vmCtx;
	vm::Expression pxExp;
	vm::Expression pyExp;
	vm::Expression ttlExp;
	vm::Expression scaleExp;
	vm::Expression fadeExp;
	ParticleEmitter emitter;
	ParticleEmitter modules;
};

