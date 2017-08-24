#pragma once

#include <stdint.h>
#include "ds_vm.h"
#include <stdio.h>

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
	PC_TIMER
};

const static ParticleChannel ALL_CHANNELS[] = { PC_POS_X,PC_POS_Y,PC_SCALE_X,PC_SCALE_Y,PC_ROTATION,PC_COLOR_R,PC_COLOR_G,PC_COLOR_B,PC_COLOR_A,PC_TTL,PC_TIMER };

const static char* CHANNEL_NAMES[] = { "PC_POS_X","PC_POS_Y","PC_SCALE_X","PC_SCALE_Y","PC_ROTATION","PC_COLOR_R","PC_COLOR_G","PC_COLOR_B","PC_COLOR_A","PC_TTL","PC_TIMER" };

const static uint16_t NUM_CHANNELS = 11;

const static float DEFAULT_VALUES[] = { 0.0f,0.0f,1.0f,1.0f,0.0f,1.0f,1.0f,1.0f,1.0f,1.0f,0.0f };

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

	ChannelConnection() : channel(ParticleChannel::PC_POS_X), expression(0) {}

	ChannelConnection(ParticleChannel c, vm::Expression* e) : channel(c), expression(e) {}
};

struct ParticleEmitter {

	ChannelConnection connections[NUM_CHANNELS];
};

