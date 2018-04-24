#include "ParticleEmitter.h"
#include "GPUParticlesystem.h"

const char* EmitterTypeNames[] = { "Sphere", "Ring", "Box", "Cone", "Line" };

const char* get_emitter_name(EmitterType::Enum type) {
	return EmitterTypeNames[type];
}

ds::vec3 emittRing(ds::MemoryBuffer* buffer, ID id, int index, int total) {
	RingEmitterSettings settings;
	buffer->get(id, &settings, sizeof(RingEmitterSettings));
	float step = ds::TWO_PI / total;
	float angle = step * index;// ds::random(direction - emitterDescriptor.angleVariance, direction + emitterDescriptor.angleVariance);
	float x = cos(angle) * settings.radius;
	float y = 0.0f;
	float z = sin(angle) * settings.radius;
	return ds::vec3(x, y, z);
}

ds::vec3 emittSphere(ds::MemoryBuffer* buffer, ID id, int index, int total) {
	RingEmitterSettings settings;
	buffer->get(id, &settings, sizeof(RingEmitterSettings));
	int rings = 8;
	int sectors = total / rings;
	float s = ds::PI / rings;
	float t = ds::TWO_PI / sectors;
	int ring = index % sectors;
	int sector = index / sectors;
	float x = cos(ring * t) * sin(sector * s) * settings.radius;
	float z = sin(ring * t) * sin(sector * s) * settings.radius;
	float y = cos(sector * s) * settings.radius;
	return ds::vec3(x, y, z);
}

ds::vec3 emittCone(ds::MemoryBuffer* buffer, ID id, int index, int total) {
	ConeEmitterSettings settings;
	buffer->get(id, &settings, sizeof(ConeEmitterSettings));
	//int rings = 1;
	//int sectors = total / rings;
	float s = ds::random(0.0f, settings.angle);// ds::PI / rings;
	float t = ds::random(0.0f, ds::TWO_PI);// ds::TWO_PI / sectors;
	int ring = 1;// index % sectors;
	int sector = 1;// index / sectors;
	float x = cos(ring * t) * sin(sector * s) * settings.radius;
	float z = sin(ring * t) * sin(sector * s) * settings.radius;
	float y = cos(sector * s) * settings.radius;
	return ds::vec3(x, y, z);

}

void grow(ParticleArray* array, ds::MemoryBuffer* buffer, ID id, int start, int end) {

}