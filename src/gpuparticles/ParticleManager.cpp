#include "ParticleManager.h"
#include <ds_imgui.h>
#include "..\utils\common_math.h"

ParticleManager::ParticleManager(RID textureID) {
	
	ParticlesystemDescriptor descriptor;
	descriptor.maxParticles = 16384;
	descriptor.particleDimension = ds::vec2(20, 20);
	//descriptor.startColor = ds::Color(255, 255, 255, 255);
	//descriptor.endColor = ds::Color(128, 128, 128, 0);

	//float u1 = 233.0f / 1024.0f;
	//float v1 = 7.0f / 1024.0f;
	// plain quad
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
	/*
	SPHERE,
		RING,
		BOX,
		CONE,
		LINE
	*/
	_emitterFunctions[0] = emittSphere;
	_emitterFunctions[1] = emittRing;
	_emitterFunctions[2] = emittRing;
	_emitterFunctions[3] = emittRing;
	_emitterFunctions[4] = emittRing;

	//_emitters.push_back(new RingEmitter(&_emitterData));
	//_emitters.push_back(new SphereEmitter(&_emitterData));
	//_emitters.push_back(new ConeEmitter(&_emitterData));
}

ParticleManager::~ParticleManager() {
	//for (size_t i = 0; i < _emitters.size(); ++i) {
		//delete _emitters[i];
	//}
	delete _particleSystem;
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
	ds::vec2 scale = ds::vec2(0.5f);
	ds::vec2 scaleVariance = ds::vec2(0.02f);
	ds::vec2 growth = ds::vec2(-0.1f,-0.1f);
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
		descriptor.maxScale = ds::vec2(sx, sy) + growth;
		descriptor.acceleration = ds::vec3(0.0f);
		//_particleDescriptor.acceleration = ds::vec3(cos(angle) * acc, sin(angle) * acc, 0.0f);
		_particleSystem->add(ds::vec3(x, y, z), descriptor);
	}
}

void ParticleManager::emittParticles(const ds::vec3& pos, const ds::vec3& direction, int num) {
	ds::vec2 radius = ds::vec2(0.15f);
	int alignParticle = 1;
	ds::vec2 rotationSpeed = ds::vec2(0.0f,ds::TWO_PI);
	ds::vec2 ttl = ds::vec2(1.0f, 1.01f);
	ds::Color startColor(0, 255, 255, 255);
	ds::Color endColor(0, 32, 32, 255);
	ds::vec2 scale = ds::vec2(0.08f);
	ds::vec2 scaleVariance = ds::vec2(0.01f);
	ds::vec2 growth = ds::vec2(0.0f);// -0.1f, -0.1f);
	ParticleDescriptor descriptor;
	for (int i = 0; i < num; ++i) {
		float r = ds::random(radius.x, radius.y);
		float angle = ds::random(-ds::PI * 0.25f, +ds::PI * 0.25f);
		float x = cos(angle) * r + pos.x;
		float y = pos.y;
		float z = sin(angle) * r + pos.z;
		if (alignParticle == 1) {
			descriptor.rotation = math::get_rotation(ds::vec2(direction.x,direction.z));
		}
		descriptor.rotationSpeed = ds::random(rotationSpeed.x, rotationSpeed.y);
		descriptor.ttl = ds::random(ttl.x, ttl.y);
		float acc = ds::random(3.0f, 3.5f);
		descriptor.velocity = direction * acc;
		descriptor.startColor = startColor;
		descriptor.endColor = endColor;
		float sx = ds::random(scale.x - scaleVariance.x, scale.x + scaleVariance.x);
		float sy = ds::random(scale.y - scaleVariance.y, scale.y + scaleVariance.y);
		descriptor.minScale = ds::vec2(sx, sy);
		descriptor.maxScale = ds::vec2(sx, sy) + growth;
		descriptor.acceleration = ds::vec3(0.0f);
		//_particleDescriptor.acceleration = ds::vec3(cos(angle) * acc, sin(angle) * acc, 0.0f);
		_particleSystem->add(pos, descriptor);
	}
}

void ParticleManager::emittLine(const ds::vec3 & start, const ds::vec3 & end) {
	ParticleDescriptor descriptor;
	ds::vec3 diff = end - start;
	float l = length(diff);
	ds::vec3 center = (start + end) * 0.5f;
	float angle = math::get_rotation(ds::vec2(diff.x, diff.z)) + ds::PI;
	//float angle = math::get_angle(ds::vec2(end.x, end.z), ds::vec2(start.x, start.z));
	DBG_LOG("start %2.2f %2.2f %2.2f", start.x, start.y, start.z);
	DBG_LOG("end %2.2f %2.2f %2.2f", end.x, end.y, end.z);
	DBG_LOG("diff %2.2f %2.2f %2.2f", diff.x, diff.y, diff.z);
	DBG_LOG("center %2.2f %2.2f %2.2f", center.x, center.y, center.z);
	DBG_LOG("angle %3.2f length: %3.2f", angle * 360.0f / ds::TWO_PI,l);
	descriptor.rotation = angle;
	descriptor.rotationSpeed = 0.0f;
	descriptor.ttl = 0.1f;
	descriptor.velocity = ds::vec3(0.0f);
	descriptor.startColor = ds::Color(255, 0, 0, 255);
	descriptor.endColor = ds::Color(128, 0, 0, 255);
	float sx = l;
	float sy = 0.08f;
	descriptor.minScale = ds::vec2(sx, sy);
	descriptor.maxScale = ds::vec2(0.0f);
	descriptor.acceleration = ds::vec3(0.0f);
	_particleSystem->add(center, descriptor);
}

void ParticleManager::emittParticles(const ds::vec3& pos, float direction, int num, int descriptorIndex) {
	const ParticleEmitterDescriptor& emitterDescriptor = _descriptors[descriptorIndex];
	ParticleDescriptor descriptor;
	float r = ds::random(emitterDescriptor.radius.x, emitterDescriptor.radius.y);
	float step = ds::TWO_PI / num;


	RingEmitterSettings ringEmitterSettings;
	ringEmitterSettings.radius = r;

	for (int i = 0; i < num; ++i) {
		ds::vec3 p = (_emitterFunctions[0])(&_emitterData, 0, i, num) + pos;// _emitters[1]->getPosition(i, num) + pos;
		float angle = step * i;// ds::random(direction - emitterDescriptor.angleVariance, direction + emitterDescriptor.angleVariance);
		//float x = cos(angle) * r + pos.x;
		//float y = pos.y;
		//float z = sin(angle) * r + pos.z;
		if (emitterDescriptor.alignParticle == 1) {
			descriptor.rotation = angle;
		}
		else {
			descriptor.rotation = 0.0f;
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
		descriptor.maxScale = emitterDescriptor.growth;
		descriptor.acceleration = ds::vec3(0.0f);
		//_particleDescriptor.acceleration = ds::vec3(cos(angle) * acc, sin(angle) * acc, 0.0f);
		descriptor.worldWatrix = ds::matIdentity();
		descriptor.acceleration = ds::vec3(0.0f,0.0f,0.0f);
		_particleSystem->add(p, descriptor);
	}
}

void ParticleManager::emittParticles(ID effectID, const ds::vec3& pos, int num) {
	const ParticleEffect& effect = _effects.get(effectID);
	for (int n = 0; n < effect.numEmitters; ++n) {
		const ParticleEmitterDescriptor& emitterDescriptor = _descriptors[effect.descriptionIDs[n]];
		const ParticleEmitterDefinition& def = _emitterDefinitions.get(effect.emitterIDs[n]);
		ParticleDescriptor descriptor;
		float r = ds::random(emitterDescriptor.radius.x, emitterDescriptor.radius.y);
		float step = ds::TWO_PI / num;
		for (int i = 0; i < num; ++i) {
			ds::vec3 p = (_emitterFunctions[def.type])(&_emitterData, def.emitterID, i, num) + pos;// _emitters[1]->getPosition(i, num) + pos;
			float angle = step * i;// ds::random(direction - emitterDescriptor.angleVariance, direction + emitterDescriptor.angleVariance);
								   //float x = cos(angle) * r + pos.x;
								   //float y = pos.y;
								   //float z = sin(angle) * r + pos.z;
			if (emitterDescriptor.alignParticle == 1) {
				descriptor.rotation = angle;
			}
			else {
				descriptor.rotation = 0.0f;
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
			descriptor.maxScale = emitterDescriptor.growth;
			descriptor.acceleration = ds::vec3(0.0f);
			//_particleDescriptor.acceleration = ds::vec3(cos(angle) * acc, sin(angle) * acc, 0.0f);
			descriptor.worldWatrix = ds::matIdentity();
			descriptor.acceleration = ds::vec3(0.0f, 0.0f, 0.0f);
			_particleSystem->add(p, descriptor);
		}
	}
}

ID ParticleManager::createEffect() {
	return _effects.add();
}

ID ParticleManager::createEmitter(EmitterType::Enum type, void* data, size_t size) {
	ID id = _emitterDefinitions.add();
	ParticleEmitterDefinition& def = _emitterDefinitions.get(id);
	def.emitterID = _emitterData.add(data, size);
	def.type = type;
	return id;
}

void ParticleManager::attachEmitter(ID effectID, ID emitterID, int descriptorIndex) {
	ParticleEffect& effect = _effects.get(effectID);
	effect.descriptionIDs[effect.numEmitters] = descriptorIndex;
	effect.emitterIDs[effect.numEmitters++] = emitterID;
}

ID ParticleManager::createEmitter(EmitterType::Enum type) {
	ID settingsId = INVALID_ID;
	if (type == EmitterType::RING || type == EmitterType::SPHERE) {
		RingEmitterSettings settings;
		settings.radius = 1.0f;
		settingsId = _emitterData.add(&settings, sizeof(RingEmitterSettings));
	}
	else if (type == EmitterType::CONE) {
		ConeEmitterSettings settings;
		settings.radius = 1.0f;
		settings.angle = ds::TWO_PI;
		settings.arc = ds::PI;
		settingsId = _emitterData.add(&settings, sizeof(ConeEmitterSettings));
	}
	ID id = _emitterDefinitions.add();
	ParticleEmitterDefinition& def = _emitterDefinitions.get(id);
	def.type = type;
	return id;
}

bool ParticleManager::getEmitterSettings(ID id, void* data, size_t size) {
	if (_emitterDefinitions.contains(id)) {
		ParticleEmitterDefinition& def = _emitterDefinitions.get(id);
		return _emitterData.get(0, data, size);
	}
	return false;
}

void ParticleManager::setEmitterSettings(ID id, void* data, size_t size) {
	if (_emitterDefinitions.contains(id)) {
		ParticleEmitterDefinition& def = _emitterDefinitions.get(id);
		_emitterData.set(0, data, size);
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
			gui::Input("TTL min/max", &emitterDescriptor.ttl);
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


