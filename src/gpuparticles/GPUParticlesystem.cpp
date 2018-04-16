#include "GPUParticlesystem.h"
#include "..\..\shaders\GPUParticles_VS_Main.h"
#include "..\..\shaders\GPUParticles_PS_Main.h"
#include <ds_profiler.h>

void swap(GPUParticle* a, GPUParticle* b) {
	GPUParticle t = *a;
	*a = *b;
	*b = t;
}

int partition(GPUParticle* arr, int low, int high, const ds::vec3& eyePos) {
	float pivot = sqr_length(eyePos - arr[high].position);
	int i = (low - 1);  
	for (int j = low; j <= high - 1; j++) {
		if (sqr_length(eyePos - arr[j].position) > pivot) {
			i++;
			swap(&arr[i], &arr[j]);
		}
	}
	swap(&arr[i + 1], &arr[high]);
	return (i + 1);
}

void quickSort(GPUParticle* arr, int low, int high, const ds::vec3& eyePos) {
	if (low < high)	{
		int pi = partition(arr, low, high, eyePos);
		quickSort(arr, low, pi - 1, eyePos);
		quickSort(arr, pi + 1, high, eyePos);
	}
}

// -------------------------------------------------------
// create new particlesystem
// -------------------------------------------------------
GPUParticlesystem::GPUParticlesystem(const ParticlesystemDescriptor& descriptor) : _descriptor(descriptor) {
	_array.initialize(descriptor.maxParticles);
	_vertices = new GPUParticle[descriptor.maxParticles];
	/*
	struct BlendStateInfo {
		BlendStates srcBlend;
		BlendStates srcAlphaBlend;
		BlendStates destBlend;
		BlendStates destAlphaBlend;
		bool alphaEnabled;
	};
	*/
	//
	// alpha blending
	//
	ds::BlendStateInfo blendStateInfo{ ds::BlendStates::SRC_ALPHA, ds::BlendStates::ONE, ds::BlendStates::INV_SRC_ALPHA, ds::BlendStates::ONE, true };
	//ds::BlendStateInfo blendStateInfo = { ds::BlendStates::ONE, ds::BlendStates::ZERO, ds::BlendStates::INV_SRC_ALPHA, ds::BlendStates::ZERO, true };
	
	
	//ds::BlendStateInfo blendStateInfo = { ds::BlendStates::SRC_ALPHA, ds::BlendStates::ZERO, ds::BlendStates::ONE, ds::BlendStates::ZERO, true };

	//
	// premultiplied alpha
	//
	//ds::BlendStateInfo blendStateInfo = { ds::BlendStates::ONE, ds::BlendStates::INV_SRC_ALPHA, ds::BlendStates::ONE, ds::BlendStates::ONE, true };
	//
	// additive blending
	//
	//ds::BlendStateInfo blendStateInfo = { ds::BlendStates::SRC_ALPHA, ds::BlendStates::ZERO, ds::BlendStates::ONE, ds::BlendStates::ONE, true };


	RID blendState = ds::createBlendState(blendStateInfo);

	ds::ShaderInfo vsInfo = { 0, GPUParticles_VS_Main, sizeof(GPUParticles_VS_Main), ds::ShaderType::ST_VERTEX_SHADER };
	RID vertexShader = ds::createShader(vsInfo, "ParticlesVS");
	ds::ShaderInfo psInfo = { 0, GPUParticles_PS_Main, sizeof(GPUParticles_PS_Main), ds::ShaderType::ST_PIXEL_SHADER };
	RID pixelShader = ds::createShader(psInfo, "ParticlesPS");


	RID constantBuffer = ds::createConstantBuffer(sizeof(ParticleConstantBuffer), &_constantBuffer);
	ds::SamplerStateInfo samplerInfo = { ds::TextureAddressModes::CLAMP, ds::TextureFilters::LINEAR };
	RID samplerState = ds::createSamplerState(samplerInfo);

	int indices[] = { 0,1,2,1,3,2 };
	RID idxBuffer = ds::createQuadIndexBuffer(descriptor.maxParticles, indices);

	ds::StructuredBufferInfo sbInfo;
	sbInfo.cpuWritable = true;
	sbInfo.data = 0;
	sbInfo.elementSize = sizeof(GPUParticle);
	sbInfo.numElements = descriptor.maxParticles;
	sbInfo.gpuWritable = false;
	sbInfo.renderTarget = NO_RID;
	sbInfo.textureID = NO_RID;
	_structuredBufferId = ds::createStructuredBuffer(sbInfo);

	RID basicGroup = ds::StateGroupBuilder()
		.blendState(blendState)
		.constantBuffer(constantBuffer, vertexShader)
		.structuredBuffer(_structuredBufferId, vertexShader, 1)
		.vertexBuffer(NO_RID)
		.vertexShader(vertexShader)
		.indexBuffer(idxBuffer)
		.pixelShader(pixelShader)
		.samplerState(samplerState, pixelShader)
		.texture(descriptor.texture, pixelShader, 0)
		.build();

	ds::DrawCommand drawCmd = { 100, ds::DrawType::DT_INDEXED, ds::PrimitiveTypes::TRIANGLE_LIST, 0 };

	_drawItem = ds::compile(drawCmd, basicGroup);
	/* quicksort test */
	GPUParticle test[4];
	test[0].position = ds::vec3(0.0f);
	test[1].position = ds::vec3(0.0f,0.0f,1.0f);
	test[2].position = ds::vec3(0.0f,0.5f,0.9f);
	test[3].position = ds::vec3(1.0f,0.8f,0.2f);
	quickSort(test, 0, 3, ds::vec3(0.0f, 6.0f, -6.0f));
	

	_debug = false;
}

GPUParticlesystem::~GPUParticlesystem() {
	delete[] _vertices;
}

// -------------------------------------------------------
// add particle based on ParticleDescriptor
// -------------------------------------------------------
void GPUParticlesystem::add(const ds::vec3& pos, ParticleDescriptor descriptor) {
	int start = _array.countAlive;
	if ((start + 1) < _array.count) {	
		_array.timers[start] = ds::vec3(0.0f,0.0f,descriptor.ttl);
		_array.velocities[start] = ds::vec3(descriptor.velocity);
		_array.positions[start] = pos;
		_array.sizes[start] = ds::vec4(descriptor.minScale.x, descriptor.minScale.y, descriptor.maxScale.x, descriptor.maxScale.y);
		_array.accelerations[start] = descriptor.acceleration;
		_array.rotations[start] = descriptor.rotation;
		_array.rotationSpeeds[start] = descriptor.rotationSpeed;
		_array.startColors[start] = descriptor.startColor;
		_array.endColors[start] = descriptor.endColor;
		_array.wake(start);
	}
}

// -------------------------------------------------------
// tick - updates all particles and manages lifetime
// -------------------------------------------------------
void GPUParticlesystem::tick(float dt) {
	perf::ZoneTracker("GP::tick");
	for (int i = 0; i < _array.countAlive; ++i) {
		ds::vec3 t = _array.timers[i];
		t.x += dt;
		t.y = t.x / t.z;
		_array.timers[i] = t;
		if (t.y >= 1.0f) {
			_array.kill(i);
		}
		//_array.velocities[i] += _array.accelerations[i] * dt;
		//_array.positions[i] += _array.velocities[i] * dt;
	}
}



// -------------------------------------------------------
// render particles
// -------------------------------------------------------
void GPUParticlesystem::render(RID renderPass, const ds::matrix& viewProjectionMatrix, const ds::vec3& eyePos) {
	perf::ZoneTracker("GP::render");
	if (_array.countAlive > 0) {
		// prepare constant buffers
		ds::matrix w = ds::matIdentity();
		_constantBuffer.wvp = ds::matTranspose(viewProjectionMatrix);
		_constantBuffer.eyePos = eyePos;
		_constantBuffer.padding = 0.0f;
		_constantBuffer.world = ds::matTranspose(w);
		_constantBuffer.textureRect = _descriptor.textureRect;
		ds::matrix rx = ds::matRotationX(ds::PI * 0.25f);
		for (int i = 0; i < _array.countAlive; ++i) {
			_vertices[i] = {
				_array.positions[i],
				_array.velocities[i],
				_array.accelerations[i],
				ds::vec2(_array.timers[i].x, _array.timers[i].y),
				ds::vec2(_array.sizes[i].x,_array.sizes[i].y),
				ds::vec2(_array.sizes[i].z,_array.sizes[i].w),
				_array.startColors[i],
				_array.endColors[i],
				_array.rotations[i],
				_array.rotationSpeeds[i],
				rx
			};
		}
		/*
		{
			perf::ZoneTracker("GP::sort");
			quickSort(_vertices, 0, _array.countAlive - 1, eyePos);
		}
		*/
		ds::mapBufferData(_structuredBufferId, _vertices, _array.countAlive * sizeof(GPUParticle));
		ds::submit(renderPass, _drawItem, _array.countAlive * 6);
	}
}