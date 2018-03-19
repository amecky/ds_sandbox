#pragma once
#include "..\gpuparticles\GPUParticlesystem.h"

class ParticleManager {

public:
	ParticleManager(RID textureID);
	~ParticleManager();
	void tick(float dt);
	void render(RID renderPass, const ds::matrix& viewProjectionMatrix, const ds::vec3& eyePos);
	void emittParticles(const ds::vec3& pos, float direction, int num);
private:
	GPUParticlesystem* _particleSystem;
};

