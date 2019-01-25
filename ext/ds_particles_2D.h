#pragma once
#include <diesel.h>

struct GPUParticle {
	float position[2];
	float rotation;
	float velocity[2];
	float acceleration[2];
	float timer[2];
	float scale[2];
	float growth[2];
};

// ---------------------------------------------------------------
// the sprite constant buffer
// ---------------------------------------------------------------
struct ParticleConstantBuffer {
	ds::vec4 screenCenter;
	ds::Color startColor;
	ds::Color endColor;
	ds::vec4 texture;
	ds::vec2 dimension;
	ds::vec2 dummy;
	ds::matrix wvp;
};

// -------------------------------------------------------
// Particle array
// -------------------------------------------------------
struct ParticleArray {

	ds::vec2* positions;
	ds::vec2* velocities;
	ds::vec2* accelerations;
	float* rotations;
	ds::vec2* scales;
	ds::vec2* growth;
	ds::vec3* timers;
	char* buffer;

	uint32_t count;
	uint32_t countAlive;

	ParticleArray() : count(0), countAlive(0), buffer(0), positions(0), velocities(0), accelerations(0), rotations(0), scales(0), growth(0), timers(0) {}

	~ParticleArray() {
		if (buffer != 0) {
			delete[] buffer;
		}
	}

	void initialize(unsigned int maxParticles) {
		int size = maxParticles * (sizeof(ds::vec2) + sizeof(ds::vec2) + sizeof(ds::vec2) + sizeof(float) + sizeof(ds::vec2) + sizeof(ds::vec2) + sizeof(ds::vec3));
		buffer = new char[size];
		positions = (ds::vec2*)(buffer);
		velocities = (ds::vec2*)(positions + maxParticles);
		accelerations = (ds::vec2*)(velocities + maxParticles);
		rotations = (float*)(accelerations + maxParticles);
		scales = (ds::vec2*)(rotations + maxParticles);
		growth = (ds::vec2*)(scales + maxParticles);
		timers = (ds::vec3*)(growth + maxParticles);
		count = maxParticles;
		countAlive = 0;
	}

	void swapData(uint32_t a, uint32_t b) {
		if (a != b) {
			positions[a] = positions[b];
			velocities[a] = velocities[b];
			accelerations[a] = accelerations[b];
			rotations[a] = rotations[b];
			scales[a] = scales[b];
			growth[a] = growth[b];
			timers[a] = timers[b];
		}
	}

	void kill(uint32_t id) {
		if (countAlive > 0) {
			swapData(id, countAlive - 1);
			--countAlive;
		}
	}

	void wake(uint32_t id) {
		if (countAlive < count) {
			++countAlive;
		}
	}

};

#ifdef DS_PARTICLE_IMPLEMENTATION

struct ParticleContext {
	ParticleConstantBuffer constantBuffer;
	GPUParticle* vertices;
	ds::matrix viewprojectionMatrix;
	RID drawItem;
	RID orthoPass;
	RID structuredBufferId;
};

#endif