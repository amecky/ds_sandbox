#pragma once
#include "..\gpuparticles\GPUParticlesystem.h"

class ParticleManager {

public:
	ParticleManager(RID textureID);
	~ParticleManager();
private:
	GPUParticlesystem* _particleSystem;
};

