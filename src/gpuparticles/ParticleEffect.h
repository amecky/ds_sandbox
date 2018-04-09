#pragma once
#include <diesel.h>
#include <string.h>

struct EmitterType {

	enum Enum {
		SPHERE,
		RING,
		BOX,
		CONE
	};
	const static char* Names[];
};



struct ParticleEmitterDescriptor {
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

	EmitterType::Enum types[16];
	char _descriptors[16 * sizeof(ParticleEmitterDescriptor)];
	int _numTypes;
	int _indices[16];
	char _data[1024];
	int _size;
	
	ParticleEffect() : _size(0) , _numTypes(0) {}

	int addEmitter(EmitterType::Enum type, void* data, size_t size) {
		memcpy(_data + _size, data, size);
		_indices[_numTypes] = _size;
		_size += size;
		types[_numTypes++] = type;
		return _numTypes - 1;
	}

	void setDescriptor(int index, const ParticleEmitterDescriptor& descriptor) {
		int offset = index * sizeof(ParticleEmitterDescriptor);
		memcpy(_descriptors + offset, &descriptor, sizeof(ParticleEmitterDescriptor));
	}

	void getDescriptor(int index, ParticleEmitterDescriptor* desc) {
		int offset = index * sizeof(ParticleEmitterDescriptor);
		memcpy(desc, _descriptors + offset, sizeof(ParticleEmitterDescriptor));
	}

	EmitterType::Enum getEmitter(int index, void* data, size_t size) {
		memcpy(data, _data + _indices[index], size);
		return types[index];
	}

	void remove(int index) {
		// FIXME: what shall we do?
	}

};