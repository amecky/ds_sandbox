#include "ParticleEmitter.h"

const char* EmitterType::Names[] = { "Sphere", "Ring", "Box", "Cone", "Line" };

ds::vec3 RingEmitter::getPosition(int index, int total) {
	float step = ds::TWO_PI / total;
	float angle = step * index;// ds::random(direction - emitterDescriptor.angleVariance, direction + emitterDescriptor.angleVariance);
	float x = cos(angle) * _settings->radius;
	float y = 0.0f;
	float z = sin(angle) * _settings->radius;
	return ds::vec3(x, y, z);
}

ds::vec3 SphereEmitter::getPosition(int index, int total) {
	int rings = 8;
	int sectors = total / rings;
	float s = ds::PI / rings;
	float t = ds::TWO_PI / sectors;
	int ring = index % sectors;
	int sector = index / sectors;
	float x = cos(ring * t) * sin(sector * s) * _settings->radius;
	float z = sin(ring * t) * sin(sector * s) * _settings->radius;
	float y = cos(sector * s) * _settings->radius;
	return ds::vec3(x, y, z);
}