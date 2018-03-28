#pragma once
#include "..\gpuparticles\GPUParticlesystem.h"

struct ParticleEmitterDescriptor {
	ds::vec2 radius;
	int alignParticle;
	float angleVariance;
	ds::vec2 rotationSpeed;
	ds::vec2 velocity;
	ds::vec2 ttl;
	ds::Color startColor;
	ds::Color endColor;
	ds::vec2 scale;
	ds::vec2 scaleVariance;
	ds::vec2 growth;
};

class ParticleManager {

public:
	ParticleManager(RID textureID);
	~ParticleManager();
	void tick(float dt);
	void render(RID renderPass, const ds::matrix& viewProjectionMatrix, const ds::vec3& eyePos);
	void emittParticles(const ds::vec3& pos, float direction, int num);
	void emittParticles(const ds::vec3& pos, const ds::vec3& direction, int num);
	void emittLine(const ds::vec3& start, const ds::vec3& end);
	void emittParticles(const ds::vec3& pos, float direction, int num, int descriptorIndex);
	void showGUI(int descriptorIndex);
	void saveDescriptors(const char* fileName);
	void loadDescriptors(const char* fileName);
private:
	void buildEmitterDescriptors();
	GPUParticlesystem* _particleSystem;
	ParticleEmitterDescriptor _descriptors[32];
};

