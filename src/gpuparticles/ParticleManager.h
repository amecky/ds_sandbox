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

	
	ID createEffect();
	ID createEmitter(EmitterType::Enum type);
	ID createEmitter(EmitterType::Enum type, void* data, size_t size);
	void attachEmitter(ID effectID, ID emitterID, int descriptorIndex);

	void emittParticles(ID effectID, const ds::vec3& pos, int num);

	bool containsDefinition(ID id) const {
		return _emitterDefinitions.contains(id);
	}
	const ParticleEmitterDefinition& getDefinition(ID id) const {
		return _emitterDefinitions.get(id);
	}
	bool getEmitterSettings(ID id, void* data, size_t size);
	void setEmitterSettings(ID id, void* data, size_t size);
private:
	void buildEmitterDescriptors();
	GPUParticlesystem* _particleSystem;
	ParticleEmitterDescriptor _descriptors[32];
	ds::DataArray<ParticleEffect> _effects;
	ParticleEmitterFunc _emitterFunctions[10];
	ds::MemoryBuffer _emitterData;
	ds::DataArray<ParticleEmitterDefinition> _emitterDefinitions;
};

