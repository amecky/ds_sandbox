#pragma once
#include <diesel.h>

struct ParticlesystemDescriptor;

class ParticleSystemDesc {

public:
	ParticleSystemDesc();
	~ParticleSystemDesc();
	ParticleSystemDesc& MaxParticles(int max);
	ParticleSystemDesc& Dimension(const ds::vec2& dim);
	ParticleSystemDesc& TextureId(RID textureId);
	ParticleSystemDesc& StartColor(const ds::Color& clr);
	ParticleSystemDesc& EndColor(const ds::Color& clr);
	ParticleSystemDesc& TextureRect(float x, float y, float w, float h);
	const ParticlesystemDescriptor& getDescriptor() const;
private:
	ParticlesystemDescriptor* _descriptor;
};

// -------------------------------------------------------
// Particle emitter settings
// -------------------------------------------------------
struct EmitterSettings {
	int count;
	ds::vec2 ttl;
	float angleVariance;
	float radius;
	float radiusVariance;
	ds::vec2 velocity;
	float velocityVariance;
	float radialVelocity;
	ds::vec2 size;
	ds::vec2 sizeVariance;
	ds::vec2 growth;
	ds::vec2 acceleration;
	float radialAcceleration;
};

// -------------------------------------------------------
// Particle emitter descriptor
// -------------------------------------------------------
class EmitterDesc {

public:
	EmitterDesc();
	EmitterDesc& Count(int count);
	EmitterDesc& AngleVariance(float ar);
	EmitterDesc& Radius(float r);
	EmitterDesc& RadiusVariance(float rv);
	EmitterDesc& TTL(float min, float max);
	EmitterDesc& Velocity(const ds::vec2& v);
	EmitterDesc& RadialVelocity(float rv);
	EmitterDesc& VelocityVariance(float va);
	EmitterDesc& Size(float sx, float sy);
	EmitterDesc& SizeVariance(float sxv, float syv);
	EmitterDesc& Acceleration(const ds::vec2& acc);
	EmitterDesc& RadialAcceleration(float rv);
	EmitterDesc& Growth(float gx, float gy);
	const EmitterSettings& getSettings() const;
private:
	EmitterSettings _settings;
};


void particles_intialize(int maxParticles, RID textureID);

int particles_create_emitter(const EmitterDesc& desc);

const EmitterSettings& particles_get_emitter(int id);

int particles_create_system(const ParticleSystemDesc& descriptor);

void particles_render();

void particles_tick(float dt);

void particles_emitt(uint32_t id, const ds::vec2& pos, const EmitterSettings& emitter);

