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
class ParticleManager {

public:
	ParticleManager(RID textureID);
	~ParticleManager();
	void tick(float dt);
	void render(RID renderPass, const ds::matrix& viewProjectionMatrix, const ds::vec3& eyePos);

	//void emittParticles(const ds::vec3& pos, float direction, int num);
	//void emittParticles(const ds::vec3& pos, const ds::vec3& direction, int num);
	//void emittLine(const ds::vec3& start, const ds::vec3& end);
	//void emittParticles(const ds::vec3& pos, float direction, int num, int descriptorIndex);
	void showGUI(int descriptorIndex);
	void saveDescriptors(const char* fileName);
	void loadDescriptors(const char* fileName);

	
	ID createEffect(const char* name);
	ID createEmitter(EmitterType::Enum type);
	ID createEmitter(EmitterType::Enum type, void* data, size_t size);
	void attachEmitter(ID effectID, ID emitterID, ID descriptorId);

	void emittParticles(ID effectID, const ds::vec3& pos, int num);

	bool containsDefinition(ID id) const {
		return _context->emitterDefinitions.contains(id);
	}
	const ParticleEmitterDefinition& getDefinition(ID id) const {
		return _context->emitterDefinitions.get(id);
	}
	bool getEmitterSettings(ID id, void* data, size_t size);
	void setEmitterSettings(ID id, void* data, size_t size);

	ParticleSystemContext* getContext() const {
		return _context;
	}
	uint32_t numAlive() const {
		return _context->particleSystem->countAlive();
	}
private:
	void buildEmitterDescriptors();
	ParticleSystemContext* _context;
};

