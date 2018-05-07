#include "ParticleManager.h"
#include <ds_imgui.h>
#include "..\utils\common_math.h"

namespace particles {

	ParticleSystemContext* create_context(RID textureID) {
		ParticleSystemContext* context = new ParticleSystemContext;
		context->emitterFunctions[0] = emittSphere;
		context->emitterFunctions[1] = emittRing;
		context->emitterFunctions[2] = emittRing; // BOX
		context->emitterFunctions[3] = emittCone; // CONE
		context->emitterFunctions[4] = emittRing; // LINE

		ParticlesystemDescriptor descriptor;
		descriptor.maxParticles = 16384;
		descriptor.particleDimension = ds::vec2(20, 20);
		//float u1 = 233.0f / 1024.0f;
		//float v1 = 7.0f / 1024.0f;
		// plain quad
		float left = 0.0f;
		float top = 100.0f;
		float width = 64.0f;
		//descriptor.textureRect = ds::vec4(left, top, left + width, top + width) / 1024.0f;
		//float u1 = 128.0f / 1024.0f;
		//float v1 = 100.0f / 1024.0f;
		//float u2 = u1 + 64.0f / 1024.0f;
		//float v2 = v1 + 64.0f / 1024.0f;
		//descriptor.textureRect = ds::vec4(u1, v1, u2, v2);
		// load image using stb_image
		descriptor.textureRect = ds::vec4(0.0f,0.0f,1.0f,1.0f);
		descriptor.texture = textureID;
		context->particleSystem = new GPUParticlesystem(descriptor);
		ds::RenderTargetInfo info = { 1280,720,ds::Color(0.0f,0.0f,0.0f,0.0f) };
		context->renderTarget = ds::createRenderTarget(ds::RenderTargetDesc()
			.Width(1280)
			.Height(720)
			.ClearColor(ds::Color(0,0,0,0))
			, "ParticlesRT"
		);


		return context;
	}

	void tick(ParticleSystemContext* context, float dt) {
		context->particleSystem->tick(dt);
	}

	void render(ParticleSystemContext* context, RID renderPass, const ds::matrix& viewProjectionMatrix, const ds::vec3& eyePos) {
		
		context->particleSystem->render(renderPass, viewProjectionMatrix, eyePos);
	}

	ID createEffect(ParticleSystemContext* context, const char* name) {
		ID id = context->effects.add();
		ParticleEffect& effect = context->effects.get(id);
		effect.name.append(name);
		return id;
	}

	ID createEmitter(ParticleSystemContext* context, EmitterType::Enum type, void* data, size_t size) {
		ID id = context->emitterDefinitions.add();
		ParticleEmitterDefinition& def = context->emitterDefinitions.get(id);
		def.emitterID = context->emitterData.add(data, size);
		def.type = type;
		return id;
	}

	void attachEmitter(ParticleSystemContext* context, ID effectID, ID emitterID, ID descriptorId) {
		ParticleEffect& effect = context->effects.get(effectID);
		effect.descriptionIDs[effect.numEmitters] = descriptorId;
		effect.emitterIDs[effect.numEmitters++] = emitterID;
	}

	void emitt(ParticleSystemContext* context, ID effectID, const ds::vec3& pos, int num) {
		const ParticleEffect& effect = context->effects.get(effectID);
		for (int n = 0; n < effect.numEmitters; ++n) {
			const ParticleEmitterDescriptor& emitterDescriptor = context->descriptors.get(effect.descriptionIDs[n]);
			const ParticleEmitterDefinition& def = context->emitterDefinitions.get(effect.emitterIDs[n]);
			ParticleDescriptor descriptor;
			float r = ds::random(emitterDescriptor.radius.x, emitterDescriptor.radius.y);
			float step = ds::TWO_PI / num;
			for (int i = 0; i < num; ++i) {
				ds::vec3 p = (context->emitterFunctions[def.type])(&context->emitterData, def.emitterID, i, num) + pos;// _emitters[1]->getPosition(i, num) + pos;
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

				ds::vec3 dir = normalize(p - pos);

				descriptor.velocity = dir * vel;// ds::vec3(cos(angle) * vel, 0.0f, sin(angle) * vel);
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
				context->particleSystem->add(p, descriptor);
			}
		}
	}

}

