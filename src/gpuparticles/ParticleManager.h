#pragma once
#include "GPUParticlesystem.h"
#include "ParticleEffect.h"
#include <vector>
#include "..\lib\DataArray.h"
#include "..\lib\MemoryBuffer.h"

struct ParticleEmitterDefinition {
	ID id;
	EmitterType::Enum type;
	ID emitterID;
};

struct ParticleSystemContext {
	ds::DataArray<ParticleEmitterDescriptor> descriptors;
	ds::DataArray<ParticleEffect> effects;
	ParticleEmitterFunc emitterFunctions[10];
	ds::MemoryBuffer emitterData;
	ds::DataArray<ParticleEmitterDefinition> emitterDefinitions;
	GPUParticlesystem* particleSystem;
	RID renderTarget;
	RID renderPass;
};

namespace particles {

	ParticleSystemContext* create_context(RID textureID);

	void tick(ParticleSystemContext* context, float dt);

	void render(ParticleSystemContext* context, RID renderPass, const ds::matrix& viewProjectionMatrix, const ds::vec3& eyePos);

	ID createEffect(ParticleSystemContext* context, const char* name);

	ID createEmitter(ParticleSystemContext* context, EmitterType::Enum type, void* data, size_t size);

	void attachEmitter(ParticleSystemContext* context, ID effectID, ID emitterID, ID descriptorId);

	void emitt(ParticleSystemContext* context, ID effectID, const ds::vec3& pos, int num);

}
