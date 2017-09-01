#pragma once

#include <stdint.h>
#include "ds_vm.h"
#include <stdio.h>
#include <ds_profiler.h>
#include <string.h>

enum ParticleChannel {
	EMITTER_POS_X,
	EMITTER_POS_Y,
	EMITTER_SCALE_X,
	EMITTER_SCALE_Y,
	EMITTER_ROTATION,
	EMITTER_COLOR_R,
	EMITTER_COLOR_G,
	EMITTER_COLOR_B,
	EMITTER_COLOR_A,
	COMMON_TTL,
	COMMON_TIMER,
	COMMON_DATA_0,
	COMMON_DATA_1,
	COMMON_DATA_2,
	COMMON_DATA_3,
	MOTION_POS_X,
	MOTION_POS_Y,
	MOTION_SCALE_X,
	MOTION_SCALE_Y,
	MOTION_ROTATION,
	MOTION_COLOR_R,
	MOTION_COLOR_G,
	MOTION_COLOR_B,
	MOTION_COLOR_A,
	MOTION_VELOCITY_X,
	MOTION_VELOCITY_Y,
	UNKNOWN
};

const static ParticleChannel ALL_CHANNELS[] = { 
		EMITTER_POS_X,
		EMITTER_POS_Y,
		EMITTER_SCALE_X,
		EMITTER_SCALE_Y,
		EMITTER_ROTATION,
		EMITTER_COLOR_R,
		EMITTER_COLOR_G,
		EMITTER_COLOR_B,
		EMITTER_COLOR_A,
		COMMON_TTL,
		COMMON_TIMER,
		COMMON_DATA_0,
		COMMON_DATA_1,
		COMMON_DATA_2,
		COMMON_DATA_3,
		MOTION_POS_X,
		MOTION_POS_Y,
		MOTION_SCALE_X,
		MOTION_SCALE_Y,
		MOTION_ROTATION,
		MOTION_COLOR_R,
		MOTION_COLOR_G,
		MOTION_COLOR_B,
		MOTION_COLOR_A,
		MOTION_VELOCITY_X,
		MOTION_VELOCITY_Y,
		UNKNOWN
};

const static char* CHANNEL_NAMES[] = { 
		"EMITTER_POS_X",
		"EMITTER_POS_Y",
		"EMITTER_SCALE_X",
		"EMITTER_SCALE_Y",
		"EMITTER_ROTATION",
		"EMITTER_COLOR_R",
		"EMITTER_COLOR_G",
		"EMITTER_COLOR_B",
		"EMITTER_COLOR_A",
		"COMMON_TTL",
		"COMMON_TIMER",
		"COMMON_DATA_0",
		"COMMON_DATA_1",
		"COMMON_DATA_2",
		"COMMON_DATA_3",
		"MOTION_POS_X",
		"MOTION_POS_Y",
		"MOTION_SCALE_X",
		"MOTION_SCALE_Y",
		"MOTION_ROTATION",
		"MOTION_COLOR_R",
		"MOTION_COLOR_G",
		"MOTION_COLOR_B",
		"MOTION_COLOR_A",
		"MOTION_VELOCITY_X",
		"MOTION_VELOCITY_Y",
		"UNKNOWN"
};

const static uint16_t NUM_CHANNELS = 26;

inline ParticleChannel find_by_name(const char* tmp) {
	int len = strlen(tmp);
	for (int i = 0; i < NUM_CHANNELS + 1; ++i) {
		if (strncmp(CHANNEL_NAMES[i], tmp, len) == 0 && strlen(CHANNEL_NAMES[i]) == len) {
			return ALL_CHANNELS[i];
		}
	}
	return ParticleChannel::UNKNOWN;
}



const static float DEFAULT_VALUES[] = { 
	0.0f,0.0f,1.0f,1.0f,0.0f,1.0f,1.0f,1.0f,1.0f, // emitter
	1.0f,0.0f,0.0f,0.0f,0.0f,0.0f, // common
	0.0f,0.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,0.0f,0.0f // motion
};

struct Particles {

	float* values;
	uint32_t capacity;
	uint32_t channel_capacity;
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

	ChannelConnection() : channel(ParticleChannel::EMITTER_POS_X), expression(0) , id(0) {}

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
	ParticleExpression expressions[NUM_CHANNELS];
	ChannelConnection connections[NUM_CHANNELS];
	ds::vec4 textureRect;
	Particles particles;
	vm::Context vmCtx;
	float timer;
	float* final_values;
	uint16_t capacity;
	float frequency;
	float ttl;

	Particlesystem(const char* n, const ds::vec4& r, uint16_t maxParticles) : particles(maxParticles), name(n), textureRect(r), timer(0.0f) , capacity(maxParticles) , frequency(0.0f) , ttl(0.0f) {
		vmCtx.add_constant("PI", 3.141592654f);
		vmCtx.add_constant("TWO_PI", 2.0f * 3.141592654f);
		vmCtx.add_variable("DT", 1.0f);
		vmCtx.add_variable("IDX", 1.0f);
		vmCtx.add_variable("COUNT", 32.0f);
		vmCtx.add_variable("DT", 0.0f);
		vmCtx.add_variable("TIMER", 0.0f);
		vmCtx.add_variable("TTL", 1.0f);
		vmCtx.add_variable("NORM", 1.0f);
		vmCtx.add_variable("DATA0", 1.0f);
		vmCtx.add_variable("DATA1", 1.0f);
		vmCtx.add_variable("DATA2", 1.0f);
		vmCtx.add_variable("DATA3", 1.0f);

		final_values = new float[maxParticles * 9];
	}

	~Particlesystem() {
		delete[] final_values;
	}

	void set_emission_rate(uint16_t numPerSecond, float runTTL) {
		frequency = 1.0f / static_cast<float>(numPerSecond);
		ttl = runTTL;
	}

	void emitt(uint16_t num, const ds::vec2& pos) {
		uint16_t start = 0;
		uint16_t cnt = 0;
		vmCtx.variables[4].value = num;
		if (particles.wake_up(num, &start, &cnt)) {
			for (int i = 0; i < cnt; ++i) {
				vmCtx.variables[3].value = i;
				for (int j = 0; j < 15; ++j) {
					if (connections[j].expression != 0) {
						const ChannelConnection& cc = connections[j];
						float r = vm::run(cc.expression->tokens, cc.expression->num, vmCtx);
						if (cc.channel == EMITTER_POS_X) {
							r += pos.x;
						}
						if (cc.channel == EMITTER_POS_Y) {
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
		float* px = particles.get_channel(MOTION_POS_X);
		float* py = particles.get_channel(MOTION_POS_Y);
		float* vx = particles.get_channel(MOTION_VELOCITY_X);
		float* vy = particles.get_channel(MOTION_VELOCITY_Y);
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
		float* timer = particles.get_channel(COMMON_TIMER);
		float* ttl = particles.get_channel(COMMON_TTL);
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
			for (int j = 15; j < NUM_CHANNELS; ++j) {
				if (connections[j].expression != 0) {
					const ChannelConnection& cc = connections[j];
					float* c = particles.get_channel(cc.channel);
					for (int i = 0; i < particles.num; ++i) {
						vmCtx.variables[5].value = dt;
						vmCtx.variables[6].value = particles.get_value(COMMON_TIMER, i);
						vmCtx.variables[7].value = particles.get_value(COMMON_TTL, i);
						vmCtx.variables[8].value = vmCtx.variables[6].value / vmCtx.variables[7].value;

						vmCtx.variables[9].value = particles.get_value(COMMON_DATA_0, i);
						vmCtx.variables[10].value = particles.get_value(COMMON_DATA_1, i);
						vmCtx.variables[11].value = particles.get_value(COMMON_DATA_2, i);
						vmCtx.variables[12].value = particles.get_value(COMMON_DATA_3, i);
						
						*c = vm::run(cc.expression->tokens, cc.expression->num, vmCtx);
						++c;
					}
				}
			}
		}
	}

	const float PARTICLE_SIM_STEP = 1.0f / 60.0f;

	void build_final_values(uint16_t index, ParticleChannel emitterChannel,ParticleChannel motionChannel) {
		float* px = particles.get_channel(motionChannel);
		float* epx = particles.get_channel(emitterChannel);
		float* fp = final_values + index * capacity;
		for (uint16_t i = 0; i < particles.num; ++i) {
			*fp = *px + *epx;
			++fp;
			++px;
			++epx;
		}
	}

	void update(float dt) {

		timer += dt;

		if (timer >= PARTICLE_SIM_STEP) {

			timer -= PARTICLE_SIM_STEP;

			manageLifecycles(PARTICLE_SIM_STEP);

			moveParticles(PARTICLE_SIM_STEP);

			updateParticles(PARTICLE_SIM_STEP);

		}

		build_final_values(0, EMITTER_POS_X, MOTION_POS_X);
		build_final_values(1, EMITTER_POS_Y, MOTION_POS_Y);
		build_final_values(2, EMITTER_SCALE_X, MOTION_SCALE_X);
		build_final_values(3, EMITTER_SCALE_Y, MOTION_SCALE_Y);
		build_final_values(4, EMITTER_ROTATION, MOTION_ROTATION);
	}


	void render(SpriteBatchBuffer* sprites) {
		perf::ZoneTracker("renderParticles");
		uint16_t cnt = 0;
		float* px = final_values;
		float* py = final_values + 1 * capacity;
		float* sx = final_values + 2 * capacity;
		float* sy = final_values + 3 * capacity;
		float* ro = final_values + 4 * capacity;		
		float* cr = particles.get_channel(MOTION_COLOR_R);
		float* cg = particles.get_channel(MOTION_COLOR_G);
		float* cb = particles.get_channel(MOTION_COLOR_B);
		float* ca = particles.get_channel(MOTION_COLOR_A);
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
