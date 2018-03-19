#include "ParticleManager.h"

ParticleManager::ParticleManager(RID textureID) {
	
	ParticlesystemDescriptor descriptor;
	descriptor.maxParticles = 16384;
	descriptor.particleDimension = ds::vec2(32, 32);
	//descriptor.startColor = ds::Color(255, 255, 255, 255);
	//descriptor.endColor = ds::Color(128, 128, 128, 0);

	float u1 = 220.0f / 1024.0f;
	float v1 = 30.0f / 1024.0f;
	float u2 = u1 + 20.0f / 1024.0f;
	float v2 = v1 + 20.0f / 1024.0f;
	descriptor.textureRect = ds::vec4(u1, v1, u2, v2);

	// load image using stb_image
	descriptor.texture = textureID;
	_particleSystem = new GPUParticlesystem(descriptor);
}

ParticleManager::~ParticleManager() {
}


void ParticleManager::tick(float dt) {
	_particleSystem->tick(dt);
}

void ParticleManager::render(RID renderPass, const ds::matrix & viewProjectionMatrix, const ds::vec3 & eyePos) {
	_particleSystem->render(renderPass, viewProjectionMatrix, eyePos);
}

void ParticleManager::emittParticles(const ds::vec3& pos, float direction, int num) {
	ds::vec2 radius = ds::vec2(0.15f);
	int alignParticle = 1;
	ds::vec2 rotationSpeed = ds::vec2(0.0f);
	ds::vec2 ttl = ds::vec2(0.5f, 1.5f);
	ds::Color startColor(0, 255, 255, 255);
	ds::Color endColor(0, 32, 32, 255);
	ds::vec2 scale = ds::vec2(0.3f);
	ds::vec2 scaleVariance = ds::vec2(0.02f);
	ds::vec2 growth = ds::vec2(0.0f);
	ParticleDescriptor descriptor;
	for (int i = 0; i < num; ++i) {
		float r = ds::random(radius.x, radius.y);
		float angle = ds::random(direction - ds::PI * 0.25f, direction + ds::PI * 0.25f);
		float x = cos(angle) * r + pos.x;
		float y = pos.y;
		float z = sin(angle) * r + pos.z;
		if (alignParticle == 1) {
			descriptor.rotation = angle;
		}
		descriptor.rotationSpeed = ds::random(rotationSpeed.x, rotationSpeed.y);
		descriptor.ttl = ds::random(ttl.x, ttl.y);
		float acc = ds::random(2.0f,4.0f);
		descriptor.velocity = ds::vec3(cos(direction) * acc, 0.0f, sin(direction) * acc);
		descriptor.startColor = startColor;
		descriptor.endColor = endColor;
		float sx = ds::random(scale.x - scaleVariance.x, scale.x + scaleVariance.x);
		float sy = ds::random(scale.y - scaleVariance.y, scale.y + scaleVariance.y);
		descriptor.minScale = ds::vec2(sx, sy);
		descriptor.maxScale = ds::vec2(sx, sy) - growth;
		descriptor.acceleration = ds::vec3(0.0f);
		//_particleDescriptor.acceleration = ds::vec3(cos(angle) * acc, sin(angle) * acc, 0.0f);
		_particleSystem->add(ds::vec3(x, y, z), descriptor);
	}
}
