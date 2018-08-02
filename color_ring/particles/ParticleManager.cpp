#include "ParticleManager.h"
#include "GPUParticles_PS_Main.h"
#include "GPUParticles_VS_Main.h"

ParticleManager::ParticleManager(RID textureID) {

	_array.initialize(MAX_PARTICLES);

	RID blendState = ds::createBlendState(ds::BlendStateDesc()
		.SrcBlend(ds::BlendStates::SRC_ALPHA)
		.SrcAlphaBlend(ds::BlendStates::ZERO)
		.DestBlend(ds::BlendStates::ONE)
		.DestAlphaBlend(ds::BlendStates::ZERO)
		.AlphaEnabled(true)
		);


	RID vertexShader = ds::createShader(ds::ShaderDesc()
		.Data(GPUParticles_VS_Main)
		.DataSize(sizeof(GPUParticles_VS_Main))
		.ShaderType(ds::ShaderType::ST_VERTEX_SHADER),
		"ParticlesVS"
		);

	RID pixelShader = ds::createShader(ds::ShaderDesc()
		.Data(GPUParticles_PS_Main)
		.DataSize(sizeof(GPUParticles_PS_Main))
		.ShaderType(ds::ShaderType::ST_PIXEL_SHADER),
		"ParticlesPS"
		);


	ds::vec2 textureSize = ds::getTextureSize(textureID);
	_constantBuffer.screenDimension = { static_cast<float>(ds::getScreenWidth()) / 2.0f, static_cast<float>(ds::getScreenHeight()) / 2.0f, textureSize.x, textureSize.y };
	RID constantBuffer = ds::createConstantBuffer(sizeof(ParticleConstantBuffer), &_constantBuffer);

	RID samplerState = ds::createSamplerState(ds::SamplerStateDesc()
		.AddressMode(ds::TextureAddressModes::CLAMP)
		.Filter(ds::TextureFilters::LINEAR)
		);

	int indices[] = { 0,1,2,1,3,2 };
	RID idxBuffer = ds::createQuadIndexBuffer(MAX_PARTICLES, indices);

	_structuredBufferId = ds::createStructuredBuffer(ds::StructuredBufferDesc()
		.CpuWritable(true)
		.ElementSize(sizeof(GPUParticle))
		.NumElements(MAX_PARTICLES)
		.GpuWritable(false)
	);

	RID basicGroup = ds::StateGroupBuilder()
		.blendState(blendState)
		.constantBuffer(constantBuffer, vertexShader)
		.structuredBuffer(_structuredBufferId, vertexShader, 1)
		.vertexBuffer(NO_RID)
		.vertexShader(vertexShader)
		.indexBuffer(idxBuffer)
		.pixelShader(pixelShader)
		.samplerState(samplerState, pixelShader)
		.texture(textureID, pixelShader, 0)
		.build();

	ds::DrawCommand drawCmd = { 100, ds::DrawType::DT_INDEXED, ds::PrimitiveTypes::TRIANGLE_LIST, 0 };

	_drawItem = ds::compile(drawCmd, basicGroup);
}

void ParticleManager::add(const ds::vec2& pos) {
	int start = _array.countAlive;
	if ((start + 1) < _array.count) {
		_array.timers[start] = ds::vec3(0.0f, 0.0f, 2.0f);
		_array.velocities[start] = ds::vec2(100.0f,0.0f);
		_array.positions[start] = pos;
		_array.scales[start] = ds::vec2(1.0f);
		_array.accelerations[start] = ds::vec2(0.0f);
		_array.rotations[start] = 0.0f;
		_array.rotationSpeeds[start] = 0.0f;
		_array.startColors[start] = ds::Color(255,0,0,255);
		_array.endColors[start] = ds::Color(0,255,0,255);
		_array.textureRects[start] = ds::vec4(40, 50, 16, 16);
		_array.wake(start);
	}
}

void ParticleManager::add(ParticleDesc* descriptors, int num) {
	int start = _array.countAlive;
	if ((start + num) < _array.count) {
		for (int i = 0; i < num; ++i) {
			const ParticleDesc& d = descriptors[i];
			_array.timers[start + i] = ds::vec3(0.0f, 0.0f, d.ttl);
			_array.velocities[start + i] = d.velocity;
			_array.positions[start + i] = d.position;
			_array.scales[start + i] = d.scale;
			_array.accelerations[start + i] = d.acceleration;
			_array.rotations[start + i] = d.rotation;
			_array.rotationSpeeds[start + i] = d.rotationSpeed;
			_array.startColors[start + i] = d.startColor;
			_array.endColors[start + i] = d.endColor;
			_array.textureRects[start + i] = d.textureRect;
			_array.wake(start);
		}
	}
}

// -------------------------------------------------------
// tick - updates all particles and manages lifetime
// -------------------------------------------------------
void ParticleManager::tick(float dt) {
	
	for (int i = 0; i < _array.countAlive; ++i) {
		ds::vec3 t = _array.timers[i];
		t.x += dt;
		t.y = t.x / t.z;
		_array.timers[i] = t;
		if (t.y >= 1.0f) {
			_array.kill(i);
		}
	}
}

// -------------------------------------------------------
// render particles
// -------------------------------------------------------
void ParticleManager::render(RID renderPass, const ds::matrix& viewProjectionMatrix) {
	if (_array.countAlive > 0) {
		_constantBuffer.wvp = ds::matTranspose(viewProjectionMatrix);
		for (int i = 0; i < _array.countAlive; ++i) {
			_vertices[i] = {
				_array.positions[i],
				_array.velocities[i],
				_array.accelerations[i],
				ds::vec2(_array.timers[i].x, _array.timers[i].y),
				_array.scales[i],
				_array.startColors[i],
				_array.endColors[i],
				_array.rotations[i],
				_array.rotationSpeeds[i],
				_array.textureRects[i]
			};
		}
		ds::mapBufferData(_structuredBufferId, _vertices, _array.countAlive * sizeof(GPUParticle));
		ds::submit(renderPass, _drawItem, _array.countAlive * 6);
	}
}
