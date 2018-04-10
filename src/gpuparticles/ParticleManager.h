#pragma once
#include "GPUParticlesystem.h"
#include "ParticleEffect.h"
#include <vector>




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
	std::vector<ParticleEmitter*> _emitters;
	RingEmitterSettings _ringEmitterSettings;
	ConeEmitterSettings _coneEmitterSettings;
	std::vector<ParticleEffect*> _effects;
};

