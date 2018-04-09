#pragma once
#include "..\gpuparticles\GPUParticlesystem.h"
#include <vector>
#include "..\gpuparticles\ParticleEffect.h"


class ParticleEmitter {

public:
	ParticleEmitter() {}
	virtual ~ParticleEmitter() {}
	virtual ds::vec3 getPosition(int index, int total) = 0;
};

struct RingEmitterSettings {
	float radius;
};

class RingEmitter : public ParticleEmitter {

public:
	RingEmitter(const RingEmitterSettings* settings) : ParticleEmitter(), _settings(settings) {}
	virtual ds::vec3 getPosition(int index, int total);
private:
	const RingEmitterSettings* _settings;
};

class SphereEmitter : public ParticleEmitter {

public:
	SphereEmitter(const RingEmitterSettings* settings) : ParticleEmitter(), _settings(settings) {}
	virtual ds::vec3 getPosition(int index, int total);
private:
	const RingEmitterSettings* _settings;
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
	std::vector<ParticleEmitter*> _emitters;
	RingEmitterSettings _ringEmitterSettings;
};

