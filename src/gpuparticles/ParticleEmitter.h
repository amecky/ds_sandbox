#pragma once
#include <diesel.h>
#include "..\lib\MemoryBuffer.h"

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

typedef ds::vec3(*ParticleEmitterFunc)(ds::MemoryBuffer*, ID, int, int);

ds::vec3 emittRing(ds::MemoryBuffer* buffer, ID id, int index, int total);

ds::vec3 emittSphere(ds::MemoryBuffer* buffer, ID id, int index, int total);

struct RingEmitterSettings {
	float radius;
};

struct ConeEmitterSettings {
	float radius;
	float arc;
	float angle;
};

