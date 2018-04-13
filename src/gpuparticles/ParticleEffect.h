#pragma once
#include <diesel.h>
#include <string.h>
#include "ParticleEmitter.h"
#include <ds_string.h>

struct ParticleEmitterDescriptor {
	ID id;
	ds::vec2 radius;
	int alignParticle;
	float angleVariance;
	ds::vec2 rotationSpeed;
	ds::vec2 velocity;
	ds::vec2 ttl;
	ds::Color startColor;
	ds::Color endColor;
	ds::vec2 scale;
	ds::vec2 scaleVariance;
	ds::vec2 growth;
};

struct ParticleEffect {

	ds::string name;
	ID id;
	ID emitterIDs[16];
	ID descriptionIDs[16];
	EmitterType types[16];
	int numEmitters;
	bool oneShot;

	ParticleEffect() : numEmitters(0) {}


};