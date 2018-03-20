#include "ParticleManager.h"
#include <ds_imgui.h>

ParticleManager::ParticleManager(RID textureID) {
	
	ParticlesystemDescriptor descriptor;
	descriptor.maxParticles = 16384;
	descriptor.particleDimension = ds::vec2(20, 20);
	//descriptor.startColor = ds::Color(255, 255, 255, 255);
	//descriptor.endColor = ds::Color(128, 128, 128, 0);

	float u1 = 200.0f / 1024.0f;
	float v1 = 0.0f / 1024.0f;
	float u2 = u1 + 20.0f / 1024.0f;
	float v2 = v1 + 20.0f / 1024.0f;
	descriptor.textureRect = ds::vec4(u1, v1, u2, v2);

	// load image using stb_image
	descriptor.texture = textureID;
	_particleSystem = new GPUParticlesystem(descriptor);

	buildEmitterDescriptors();

	loadDescriptors("content\\particle_descriptors.bin");
}

ParticleManager::~ParticleManager() {
}


void ParticleManager::tick(float dt) {
	_particleSystem->tick(dt);
}

void ParticleManager::render(RID renderPass, const ds::matrix & viewProjectionMatrix, const ds::vec3 & eyePos) {
	_particleSystem->render(renderPass, viewProjectionMatrix, eyePos);
}

void ParticleManager::buildEmitterDescriptors() {
	_descriptors[0].radius = ds::vec2(0.15f);
	_descriptors[0].alignParticle = 1;
	_descriptors[0].rotationSpeed = ds::vec2(0.0f);
	_descriptors[0].ttl = ds::vec2(0.5f, 0.6f);
	_descriptors[0].startColor = ds::Color(255, 255, 255, 255);
	_descriptors[0].endColor = ds::Color(255, 255, 0, 192);
	_descriptors[0].scale = ds::vec2(0.35f);
	_descriptors[0].scaleVariance = ds::vec2(0.02f);
	_descriptors[0].growth = ds::vec2(0.0f, 0.8f);
	_descriptors[0].angleVariance = ds::PI * 0.1f;
	_descriptors[0].velocity = ds::vec2(2.0f, 4.0f);
}

void ParticleManager::emittParticles(const ds::vec3& pos, float direction, int num) {
	ds::vec2 radius = ds::vec2(0.15f);
	int alignParticle = 1;
	ds::vec2 rotationSpeed = ds::vec2(0.0f);
	ds::vec2 ttl = ds::vec2(0.5f, 1.5f);
	ds::Color startColor(0, 255, 255, 255);
	ds::Color endColor(0, 32, 32, 255);
	ds::vec2 scale = ds::vec2(0.35f);
	ds::vec2 scaleVariance = ds::vec2(0.02f);
	ds::vec2 growth = ds::vec2(0.0f,0.2f);
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

void ParticleManager::emittParticles(const ds::vec3& pos, float direction, int num, int descriptorIndex) {
	const ParticleEmitterDescriptor& emitterDescriptor = _descriptors[descriptorIndex];
	ParticleDescriptor descriptor;
	for (int i = 0; i < num; ++i) {
		float r = ds::random(emitterDescriptor.radius.x, emitterDescriptor.radius.y);
		float angle = ds::random(direction - emitterDescriptor.angleVariance, direction + emitterDescriptor.angleVariance);
		float x = cos(angle) * r + pos.x;
		float y = pos.y;
		float z = sin(angle) * r + pos.z;
		if (emitterDescriptor.alignParticle == 1) {
			descriptor.rotation = angle;
		}
		descriptor.rotationSpeed = ds::random(emitterDescriptor.rotationSpeed.x, emitterDescriptor.rotationSpeed.y);
		descriptor.ttl = ds::random(emitterDescriptor.ttl.x, emitterDescriptor.ttl.y);
		float vel = ds::random(emitterDescriptor.velocity.x, emitterDescriptor.velocity.y);
		descriptor.velocity = ds::vec3(cos(angle) * vel, 0.0f, sin(angle) * vel);
		descriptor.startColor = emitterDescriptor.startColor;
		descriptor.endColor = emitterDescriptor.endColor;
		float sx = ds::random(emitterDescriptor.scale.x - emitterDescriptor.scaleVariance.x, emitterDescriptor.scale.x + emitterDescriptor.scaleVariance.x);
		float sy = ds::random(emitterDescriptor.scale.y - emitterDescriptor.scaleVariance.y, emitterDescriptor.scale.y + emitterDescriptor.scaleVariance.y);
		descriptor.minScale = ds::vec2(sx, sy);
		descriptor.maxScale = ds::vec2(sx, sy) - emitterDescriptor.growth;
		descriptor.acceleration = ds::vec3(0.0f);
		//_particleDescriptor.acceleration = ds::vec3(cos(angle) * acc, sin(angle) * acc, 0.0f);
		_particleSystem->add(ds::vec3(x, y, z), descriptor);
	}
}

void ParticleManager::showGUI(int descriptorIndex) {
	int state = 1;
	if (gui::begin("Debug", &state)) {
		gui::Value("Particles", _particleSystem->countAlive());
		if (descriptorIndex != -1) {
			ParticleEmitterDescriptor& emitterDescriptor = _descriptors[descriptorIndex];
			gui::Input("Radius min/max",&emitterDescriptor.radius);
			gui::Input("Vel min/max", &emitterDescriptor.velocity);
			gui::Input("align", &emitterDescriptor.alignParticle);
			gui::Input("Angle_Var", &emitterDescriptor.angleVariance);
			gui::Input("Rot-Speed", &emitterDescriptor.rotationSpeed);
			gui::Input("TTL", &emitterDescriptor.ttl.x);
			gui::Input("TTL_Var", &emitterDescriptor.ttl.y);
			gui::Input("Start", &emitterDescriptor.startColor);
			gui::Input("End", &emitterDescriptor.endColor);
			gui::Input("Scale", &emitterDescriptor.scale);
			gui::Input("Scale_Var", &emitterDescriptor.scaleVariance);
			gui::Input("Growth", &emitterDescriptor.growth);
		}
		gui::beginGroup();
		if (gui::Button("Load")) {
			loadDescriptors("content\\particle_descriptors.bin");
		}
		if (gui::Button("Save")) {
			saveDescriptors("content\\particle_descriptors.bin");
		}
		gui::endGroup();
	}
}

void ParticleManager::saveDescriptors(const char * fileName) {
	FILE* fp = fopen(fileName, "wb");
	if (fp) {
		for (int i = 0; i < 32; ++i) {
			fwrite(&_descriptors[i], sizeof(ParticleEmitterDescriptor), 1, fp);
		}
		fclose(fp);
	}
}

void ParticleManager::loadDescriptors(const char * fileName) {
	FILE* fp = fopen(fileName, "rb");
	if (fp) {
		for (int i = 0; i < 32; ++i) {
			fread(&_descriptors[i], sizeof(ParticleEmitterDescriptor), 1, fp);
		}
		fclose(fp);
	}
}
