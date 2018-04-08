#pragma once
#include <stdint.h>
#include <diesel.h>

struct GPUParticle {
	ds::vec3 position;
	ds::vec3 velocity;
	ds::vec3 acceleration;
	ds::vec2 timer;
	ds::vec2 scale;
	ds::vec2 growth;
	ds::Color startColor;
	ds::Color endColor;
	float rotation;
	float rotationSpeed;
	ds::matrix world;
};

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

// -------------------------------------------------------
// Particlesystem
// -------------------------------------------------------
class GPUParticlesystem {

public:
	GPUParticlesystem(const ParticlesystemDescriptor& descriptor);
	~GPUParticlesystem();
	void add(const ds::vec3& pos, ParticleDescriptor descriptor);
	void tick(float dt);
	void render(RID renderPass, const ds::matrix& viewProjectionMatrix, const ds::vec3& eyePos);
	int countAlive() const {
		return _array.countAlive;
	}
private:
	ParticlesystemDescriptor _descriptor;
	ParticleConstantBuffer _constantBuffer;
	ParticleArray _array;
	GPUParticle* _vertices;
	RID _drawItem;
	RID _structuredBufferId;
};