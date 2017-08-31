#pragma once

#include <stdint.h>
#include "ds_vm.h"
#include <stdio.h>
#include <ds_profiler.h>

enum ParticleChannel {
	PC_POS_X,
	PC_POS_Y,
	PC_SCALE_X,
	PC_SCALE_Y,
	PC_ROTATION,
	PC_COLOR_R,
	PC_COLOR_G,
	PC_COLOR_B,
	PC_COLOR_A,
	PC_TTL,
	PC_TIMER,
	PC_RAND,
	PC_VELOCITY_X,
	PC_VELOCITY_Y
};

const static ParticleChannel ALL_CHANNELS[] = { PC_POS_X,PC_POS_Y,PC_SCALE_X,PC_SCALE_Y,PC_ROTATION,PC_COLOR_R,PC_COLOR_G,PC_COLOR_B,PC_COLOR_A,PC_TTL,PC_TIMER,PC_RAND,PC_VELOCITY_X,PC_VELOCITY_Y };

const static char* CHANNEL_NAMES[] = { "POS_X","POS_Y","SCALE_X","SCALE_Y","ROTATION","COLOR_R","COLOR_G","COLOR_B","COLOR_A","TTL","TIMER","RAND","VELOCITY_X","VELOCITY_Y" };

const static uint16_t NUM_CHANNELS = 14;

const static float DEFAULT_VALUES[] = { 0.0f,0.0f,1.0f,1.0f,0.0f,1.0f,1.0f,1.0f,1.0f,1.0f,0.0f,0.0f };

struct Particles {

	float* values;
	uint16_t capacity;
	uint16_t channel_capacity;
	uint16_t num;

	Particles(uint16_t s) {
		capacity = s * NUM_CHANNELS;
		channel_capacity = s;
		values = new float[capacity];
		num = 0;
	}

	~Particles() {
		delete[] values;
	}

	bool wake_up(uint16_t size, uint16_t* start, uint16_t* count) {
		*start = num;
		*count = size;
		if (num + *count > channel_capacity) {
			return false;
		}
		if (*count + num > channel_capacity) {
			*count = channel_capacity - *start;
		}
		num += *count;
		for (uint16_t c = 0; c < NUM_CHANNELS; ++c) {
			for (int i = *start; i < *start + *count; ++i) {
				set_value(ALL_CHANNELS[c], i, DEFAULT_VALUES[c]);
			}
		}
		return true;
	}

	float* get_channel(ParticleChannel channel) {
		return values + channel * channel_capacity;
	}

	void set_value(ParticleChannel channel, uint16_t index, float value) {
		float* v = values + channel * channel_capacity;
		v[index] = value;
	}

	float get_value(ParticleChannel channel, uint16_t index) const {
		float* v = values + channel * channel_capacity;
		return v[index];
	}

	void remove(uint16_t idx) {
		if (num > 0) {
			float* dest = values + idx;
			float* src = values + num - 1;
			for (uint16_t i = 0; i < NUM_CHANNELS; ++i) {
				*dest = *src;
				dest += channel_capacity;
				src += channel_capacity;
			}
			--num;
		}
	}
};

struct ChannelConnection {

	ParticleChannel channel;
	vm::Expression* expression;
	uint16_t id;

	ChannelConnection() : channel(ParticleChannel::PC_POS_X), expression(0) , id(0) {}

	ChannelConnection(ParticleChannel c, vm::Expression* e, uint16_t id) : channel(c), expression(e) , id(id) {}
};

struct ParticleEmitter {

	ChannelConnection connections[NUM_CHANNELS];
};

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

	Particlesystem(const char* n, const ds::vec4& r, uint16_t maxParticles) : particles(maxParticles), name(n), textureRect(r) {
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
