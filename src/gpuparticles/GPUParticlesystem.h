#pragma once
#include <stdint.h>
#include <diesel.h>
#include "..\lib\MemoryBuffer.h"
//
// collection of particle systems
//
struct PEffect {
	ID id;
	ID systems[8];
	int num_systems;
	const char* name;
};

struct PSystemSettings {
	int num_per_seconds;
	float ttl;
	bool one_shot;
};

// collection of particle functions and the IDs of the settings in memory buffer
struct PSystem {
	ID id;
	const char* name;
	float frequency;
	float ttl;
	bool one_shot;
	int functions[16];
	ID settings_ids[16];
	int num_functions;
};

// running instance of a particle system
struct PEffectInstance {
	ID id;
	float elapsed;
	int current;
	ID effect_id;
};

namespace particles {

	struct PSystemContext {
		ds::DataArray<PEffect> effects;
		ds::DataArray<PSystem> systems;
		ds::DataArray<PEffectInstance> instances;
	};

	void initialize();

	void shutdown();

	ID createEffect(const char* name);

	ID createSystem(const char* name, const PSystemSettings& settings);

	void assignSystem(ID effectID, ID systemID);

	ID start_me(ID effectID, const ds::vec3& pos);

	void tick(float dt);
}



// -------------------------------------------------------
// Particle array
// -------------------------------------------------------
struct ParticleArray {

	ds::vec3* positions;
	ds::vec3* velocities;
	ds::vec3* timers;
	ds::vec4* sizes;
	ds::vec3* accelerations;
	float* rotations;
	float* rotationSpeeds;
	ds::Color* startColors;
	ds::Color* endColors;
	char* buffer;

	uint32_t count;
	uint32_t countAlive;

	ParticleArray() : count(0), countAlive(0), buffer(0), positions(0), velocities(0), timers(0), sizes(0), accelerations(0) , rotations(0), rotationSpeeds(0), startColors(0) , endColors(0) {}

	~ParticleArray() {
		if (buffer != 0) {
			delete[] buffer;
		}
	}

	void initialize(unsigned int maxParticles) {
		int size = maxParticles * (sizeof(ds::vec3) + sizeof(ds::vec3) + sizeof(ds::vec3) + sizeof(ds::vec4) + sizeof(ds::vec3) + sizeof(float) + sizeof(float) + 2 * sizeof(ds::Color));
		buffer = new char[size];
		positions = (ds::vec3*)(buffer);
		velocities = (ds::vec3*)(positions + maxParticles);
		timers = (ds::vec3*)(velocities + maxParticles);
		sizes = (ds::vec4*)(timers + maxParticles);
		accelerations = (ds::vec3*)(sizes + maxParticles);
		rotations = (float*)(accelerations + maxParticles);
		rotationSpeeds = (float*)(rotations + maxParticles);
		startColors = (ds::Color*)(rotationSpeeds + maxParticles);
		endColors = (ds::Color*)(startColors + maxParticles);
		count = maxParticles;
		countAlive = 0;
	}

	void swapData(uint32_t a, uint32_t b) {
		if (a != b) {
			positions[a] = positions[b];
			velocities[a] = velocities[b];
			timers[a] = timers[b];
			sizes[a] = sizes[b];
			accelerations[a] = accelerations[b];
			rotations[a] = rotations[b];
			rotationSpeeds[a] = rotationSpeeds[b];
			startColors[a] = startColors[b];
			endColors[a] = endColors[b];
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

// -------------------------------------------------------
// Particlesystem descriptor
// -------------------------------------------------------
struct ParticlesystemDescriptor {
	uint16_t maxParticles;
	RID texture;
	ds::vec2 particleDimension;	
	ds::vec4 textureRect;
};

// -------------------------------------------------------
// Particle descriptor
// -------------------------------------------------------
struct ParticleDescriptor {
	float ttl;
	ds::vec2 minScale;
	ds::vec2 maxScale;
	ds::vec3 velocity;
	float friction;	
	ds::vec3 acceleration;
	float rotation;
	float rotationSpeed;
	ds::Color startColor;
	ds::Color endColor;
	ds::matrix worldWatrix;
};

enum ParticleFunctions {
	PF_GROW_OVER_TIME
};

typedef void(*ParticleFunc)(ParticleArray*, ds::MemoryBuffer*, ID, int, int);

struct GrowSettings {
	ds::vec2 startScale;
	ds::vec2 growth;
};

void growFunction(ParticleArray* array, ds::MemoryBuffer* buffer, ID id, int start, int end);

// ---------------------------------------------------------------
// the sprite constant buffer
// ---------------------------------------------------------------
struct ParticleConstantBuffer {
	ds::matrix wvp;
	ds::matrix world;
	ds::vec3 eyePos;
	float padding;
	ds::vec4 textureRect;
};

struct GPUParticle;
// -------------------------------------------------------
// Particlesystem
// -------------------------------------------------------
class GPUParticlesystem {

public:
	GPUParticlesystem(const ParticlesystemDescriptor& descriptor);
	~GPUParticlesystem();
	void add(const ds::vec3& pos, ParticleDescriptor descriptor);
	void prepare(int num, int* start, int* end);
	void tick(float dt);
	void render(RID renderPass, const ds::matrix& viewProjectionMatrix, const ds::vec3& eyePos);
	int countAlive() const {
		return _array.countAlive;
	}
	void setDebug(bool debug) {
		_debug = debug;
	}
private:
	ParticlesystemDescriptor _descriptor;
	ParticleConstantBuffer _constantBuffer;
	ParticleArray _array;
	GPUParticle* _vertices;
	RID _drawItem;
	RID _structuredBufferId;
	bool _debug;
	ParticleFunc _functions[10];
	float _timers[32];
	int _timerNum;
};