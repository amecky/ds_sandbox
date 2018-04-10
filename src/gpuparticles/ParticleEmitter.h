#pragma once
#include <diesel.h>

struct EmitterType {

	enum Enum {
		SPHERE,
		RING,
		BOX,
		CONE,
		LINE
	};
	const static char* Names[];
};

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

struct ConeEmitterSettings {
	float radius;
	float arc;
	float angle;
};

class ConeEmitter : public ParticleEmitter {

public:
	ConeEmitter(const ConeEmitterSettings* settings) : ParticleEmitter(), _settings(settings) {}
	virtual ds::vec3 getPosition(int index, int total);
private:
	const ConeEmitterSettings* _settings;
};