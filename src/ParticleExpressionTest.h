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

struct Particlesystem {

	const char* name;
	ParticleExpression expressions[NUM_CHANNELS * 2];
	ChannelConnection connections[NUM_CHANNELS * 2];
	ds::vec4 textureRect;
	Particles particles;
	vm::Context vmCtx;

	Particlesystem(const char* n, const ds::vec4& r, uint16_t maxParticles) : particles(maxParticles) , name(n) , textureRect(r) {
		vmCtx.add_constant("PI", 3.141592654f);
		vmCtx.add_constant("TWO_PI", 2.0f * 3.141592654f);
		vmCtx.add_variable("DT", 1.0f);
		vmCtx.add_variable("IDX", 1.0f);
		vmCtx.add_variable("COUNT", 32.0f);
		vmCtx.add_variable("DT", 0.0f);
		vmCtx.add_variable("TIMER", 0.0f);
		vmCtx.add_variable("TTL", 1.0f);
		vmCtx.add_variable("RND", 1.0f);
		vmCtx.add_variable("NORM", 1.0f);
	}

	void emitt(uint16_t num, const ds::vec2& pos) {
		uint16_t start = 0;
		uint16_t cnt = 0;
		vmCtx.variables[4].value = num;
		if (particles.wake_up(num, &start, &cnt)) {
			for (int i = 0; i < cnt; ++i) {
				vmCtx.variables[3].value = i;
				for (int j = 0; j < NUM_CHANNELS; ++j) {
					if (connections[j].expression != 0) {
						const ChannelConnection& cc = connections[j];
						float r = vm::run(cc.expression->tokens, cc.expression->num, vmCtx);
						if (cc.channel == PC_POS_X) {
							r += pos.x;
						}
						if (cc.channel == PC_POS_Y) {
							r += pos.y;
						}
						particles.set_value(cc.channel, start + i, r);
					}
				}
			}
		}
	}

	void moveParticles(float dt) {
		perf::ZoneTracker("moveParticles");
		uint16_t cnt = 0;
		uint16_t num = particles.num;
		float* px = particles.get_channel(PC_POS_X);
		float* py = particles.get_channel(PC_POS_Y);
		float* vx = particles.get_channel(PC_VELOCITY_X);
		float* vy = particles.get_channel(PC_VELOCITY_Y);
		while (cnt < num) {
			*px += *vx * dt;
			*py += *vy * dt;
			++cnt;
			++px;
			++py;
			++vx;
			++vy;
		}
	}

	// ----------------------------------------------------
	// kill dead particles
	// ----------------------------------------------------
	void manageLifecycles(float dt) {
		perf::ZoneTracker("manageLifecycles");
		uint16_t cnt = 0;
		float* timer = particles.get_channel(PC_TIMER);
		float* ttl = particles.get_channel(PC_TTL);
		while (cnt < particles.num) {
			*timer += dt;
			if (*timer > *ttl) {
				particles.remove(cnt);
			}
			else {
				++timer;
				++ttl;
				++cnt;
			}
		}
	}

	// ----------------------------------------------------
	// update particles
	// ----------------------------------------------------
	void updateParticles(float dt) {
		perf::ZoneTracker("updateParticles");
		if (particles.num > 0) {
			for (int j = NUM_CHANNELS; j < NUM_CHANNELS * 2; ++j) {
				if (connections[j].expression != 0) {					
					const ChannelConnection& cc = connections[j];
					float* c = particles.get_channel(cc.channel);
					for (int i = 0; i < particles.num; ++i) {
						vmCtx.variables[5].value = dt;
						vmCtx.variables[6].value = particles.get_value(PC_TIMER, i);
						vmCtx.variables[7].value = particles.get_value(PC_TTL, i);
						vmCtx.variables[8].value = particles.get_value(PC_RAND, i);
						vmCtx.variables[9].value = vmCtx.variables[6].value / vmCtx.variables[7].value;
						*c = vm::run(cc.expression->tokens, cc.expression->num, vmCtx);
						++c;
					}
				}
			}
		}
	}


	void render(SpriteBatchBuffer* sprites) {
		perf::ZoneTracker("renderParticles");
		uint16_t cnt = 0;
		float* px = particles.get_channel(PC_POS_X);
		float* py = particles.get_channel(PC_POS_Y);
		float* ro = particles.get_channel(PC_ROTATION);
		float* sx = particles.get_channel(PC_SCALE_X);
		float* sy = particles.get_channel(PC_SCALE_Y);
		float* cr = particles.get_channel(PC_COLOR_R);
		float* cg = particles.get_channel(PC_COLOR_G);
		float* cb = particles.get_channel(PC_COLOR_B);
		float* ca = particles.get_channel(PC_COLOR_A);
		while (cnt < particles.num) {
			sprites->add(ds::vec2(*px, *py), textureRect, ds::vec2(*sx, *sy), *ro, ds::Color(*cr, *cg, *cb, *ca));
			++cnt;
			++px;
			++py;
			++sx;
			++sy;
			++cr;
			++cg;
			++cb;
			++ca;
			++ro;
		}
	}
};

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
	float _perfValues[32];
	uint16_t _perfIndex;
	uint16_t _perfMax;
	float _perfTimer;
	Particlesystem* _system;
};

