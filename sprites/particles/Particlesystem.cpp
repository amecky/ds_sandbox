#include <vector>
#include "Particlesystem.h"
#include "Particles_VS_Main.h"
#include "Particles_PS_Main.h"

struct GPUParticle {
	float position[2];
	float rotation;
	float velocity[2];
	float acceleration[2];
	float timer[2];
	float scale[2];
	float growth[2];
};

// ---------------------------------------------------------------
// the sprite constant buffer
// ---------------------------------------------------------------
struct ParticleConstantBuffer {
	ds::vec4 screenCenter;
	ds::Color startColor;
	ds::Color endColor;
	ds::vec4 texture;
	float dimension[2];
	float dummy[2];
	ds::matrix wvp;
};

// -------------------------------------------------------
// Particlesystem descriptor
// -------------------------------------------------------
struct ParticlesystemDescriptor {
	uint16_t maxParticles;
	ds::vec2 particleDimension;
	RID textureID;
	ds::Color startColor;
	ds::Color endColor;
	ds::vec4 textureRect;
};

ParticleSystemDesc::ParticleSystemDesc() {
	_descriptor = new ParticlesystemDescriptor;
	_descriptor->maxParticles = 4096;
	_descriptor->particleDimension = ds::vec2(64.0f, 64.0f);
	_descriptor->textureID = NO_RID;
	_descriptor->startColor = ds::Color(255, 255, 255, 255);
	_descriptor->endColor = ds::Color(0, 0, 0, 0);
	_descriptor->textureRect = ds::vec4(0.0f, 0.0f, 128.0f, 128.0f);
}
ParticleSystemDesc::~ParticleSystemDesc() {
	delete _descriptor;
}

ParticleSystemDesc& ParticleSystemDesc::MaxParticles(int max) {
	_descriptor->maxParticles = max;
	return *this;
}
ParticleSystemDesc& ParticleSystemDesc::Dimension(const ds::vec2& dim) {
	_descriptor->particleDimension = dim;
	return *this;
}
ParticleSystemDesc& ParticleSystemDesc::TextureId(RID textureId) {
	_descriptor->textureID = textureId;
	return *this;
}
ParticleSystemDesc& ParticleSystemDesc::StartColor(const ds::Color& clr) {
	_descriptor->startColor = clr;
	return *this;
}
ParticleSystemDesc& ParticleSystemDesc::EndColor(const ds::Color& clr) {
	_descriptor->endColor = clr;
	return *this;
}
ParticleSystemDesc& ParticleSystemDesc::TextureRect(float x, float y, float w, float h) {
	_descriptor->textureRect = ds::vec4(x, y, w, h);
	return *this;
}
const ParticlesystemDescriptor& ParticleSystemDesc::getDescriptor() const {
	return *_descriptor;
}

// -------------------------------------------------------
// Particle emitter descriptor
// -------------------------------------------------------
EmitterDesc::EmitterDesc() {
	_settings.count = 32;
	_settings.angleVariance = 0.0f;
	_settings.radius = 10.0f;
	_settings.radiusVariance = 0.0f;
	_settings.ttl = ds::vec2(1.0f, 0.0f);
	_settings.velocity = ds::vec2(0.0f);
	_settings.velocityVariance = 0.0f;
	_settings.size = ds::vec2(1.0f, 1.0f);
	_settings.sizeVariance = ds::vec2(0.0f, 0.0f);
	_settings.acceleration = ds::vec2(0.0f, 0.0f);
	_settings.growth = ds::vec2(0.0f, 0.0f);
	_settings.radialVelocity = 0.0f;
	_settings.radialAcceleration = 0.0f;
}
EmitterDesc& EmitterDesc::Count(int count) {
	_settings.count = count;
	return *this;
}
EmitterDesc& EmitterDesc::AngleVariance(float ar) {
	_settings.angleVariance = ar;
	return *this;
}
EmitterDesc& EmitterDesc::Radius(float r) {
	_settings.radius = r;
	return *this;
}
EmitterDesc& EmitterDesc::RadiusVariance(float rv) {
	_settings.radiusVariance = rv;
	return *this;
}
EmitterDesc& EmitterDesc::TTL(float min, float max) {
	_settings.ttl = ds::vec2(min, max);
	return *this;
}
EmitterDesc& EmitterDesc::Velocity(const ds::vec2& v) {
	_settings.velocity = v;
	return *this;
}
EmitterDesc& EmitterDesc::RadialVelocity(float rv) {
	_settings.radialVelocity = rv;
	return *this;
}
EmitterDesc& EmitterDesc::VelocityVariance(float va) {
	_settings.velocityVariance = va;
	return *this;
}
EmitterDesc& EmitterDesc::Size(float sx, float sy) {
	_settings.size = ds::vec2(sx, sy);
	return *this;
}
EmitterDesc& EmitterDesc::SizeVariance(float sxv, float syv) {
	_settings.sizeVariance = ds::vec2(sxv, syv);
	return *this;
}
EmitterDesc& EmitterDesc::Acceleration(const ds::vec2& acc) {
	_settings.acceleration = acc;
	return *this;
}
EmitterDesc& EmitterDesc::RadialAcceleration(float rv) {
	_settings.radialAcceleration = rv;
	return *this;
}
EmitterDesc& EmitterDesc::Growth(float gx, float gy) {
	_settings.growth = ds::vec2(gx, gy);
	return *this;
}
const EmitterSettings& EmitterDesc::getSettings() const {
	return _settings;
}

// -------------------------------------------------------
// Particle array
// -------------------------------------------------------
struct ParticleArray {

	ds::vec2* positions;
	ds::vec2* velocities;
	ds::vec2* accelerations;
	float* rotations;
	ds::vec2* scales;
	ds::vec2* growth;
	ds::vec3* timers;
	char* buffer;

	uint32_t count;
	uint32_t countAlive;

	ParticleArray() : count(0), countAlive(0), buffer(0), positions(0), velocities(0), accelerations(0), rotations(0), scales(0), growth(0), timers(0) {}

	~ParticleArray() {
		if (buffer != 0) {
			delete[] buffer;
		}
	}

	void initialize(unsigned int maxParticles) {
		int size = maxParticles * (sizeof(ds::vec2) + sizeof(ds::vec2) + sizeof(ds::vec2) + sizeof(float) + sizeof(ds::vec2) + sizeof(ds::vec2) + sizeof(ds::vec3));
		buffer = new char[size];
		positions = (ds::vec2*)(buffer);
		velocities = (ds::vec2*)(positions + maxParticles);
		accelerations = (ds::vec2*)(velocities + maxParticles);
		rotations = (float*)(accelerations + maxParticles);
		scales = (ds::vec2*)(rotations + maxParticles);
		growth = (ds::vec2*)(scales + maxParticles);
		timers = (ds::vec3*)(growth + maxParticles);
		count = maxParticles;
		countAlive = 0;
	}

	void swapData(uint32_t a, uint32_t b) {
		if (a != b) {
			positions[a] = positions[b];
			velocities[a] = velocities[b];
			accelerations[a] = accelerations[b];
			rotations[a] = rotations[b];
			scales[a] = scales[b];
			growth[a] = growth[b];
			timers[a] = timers[b];
		}
	}

	void kill(uint32_t id) {
		if (countAlive > 0) {
			swapData(id, countAlive - 1);
			--countAlive;
		}
	}

	void wake(uint32_t id) {
		if (countAlive < count) {
			++countAlive;
		}
	}

};

// -------------------------------------------------------
// Particlesystem
// -------------------------------------------------------
struct Particlesystem {
	ParticlesystemDescriptor descriptor;
	ParticleArray array;
};

// ------------------------------------------------------ -
// create new particlesystem
// -------------------------------------------------------

/*
ParticleManager::~ParticleManager() {
	// always clean up
	std::vector<Particlesystem*>::iterator it = _particleSystems.begin();
	while (it != _particleSystems.end()) {
		delete (*it);
		it = _particleSystems.erase(it);
	}
	delete[] _vertices;
}

*/

struct ParticleContext {
	std::vector<EmitterSettings> emitterSettings;
	ParticleConstantBuffer constantBuffer;
	std::vector<Particlesystem*> particleSystems;
	GPUParticle* vertices;
	ds::matrix viewprojectionMatrix;
	RID drawItem;
	RID orthoPass;
	RID structuredBufferId;
};

static ParticleContext* pCtx = 0;

// -------------------------------------------------------
// intialize internal particle context
// -------------------------------------------------------
void particles_intialize(int maxParticles, RID textureID) {
	pCtx = new ParticleContext;
	pCtx->vertices = new GPUParticle[maxParticles];
	float sw = ds::getScreenWidth();
	float sh = ds::getScreenHeight();
	pCtx->constantBuffer.screenCenter = ds::vec4(sw * 0.5f, sh * 0.5f, 0.0f, 0.0f);
	ds::matrix viewMatrix = ds::matIdentity();
	ds::matrix projectionMatrix = ds::matOrthoLH(sw, sh, 0.1f, 1.0f);
	pCtx->viewprojectionMatrix = viewMatrix * projectionMatrix;


	RID vertexShader = ds::createShader(ds::ShaderDesc()
		.Data(Particles_VS_Main)
		.DataSize(sizeof(Particles_VS_Main))
		.ShaderType(ds::ShaderType::ST_VERTEX_SHADER),
		"ParticlesVS"
	);

	RID pixelShader = ds::createShader(ds::ShaderDesc()
		.Data(Particles_PS_Main)
		.DataSize(sizeof(Particles_PS_Main))
		.ShaderType(ds::ShaderType::ST_PIXEL_SHADER),
		"ParticlesPS"
	);


	RID bs_id = ds::createBlendState(ds::BlendStateDesc()
		.SrcBlend(ds::BlendStates::SRC_ALPHA)
		.SrcAlphaBlend(ds::BlendStates::ZERO)
		.DestBlend(ds::BlendStates::ONE)
		.DestAlphaBlend(ds::BlendStates::ZERO)
		.AlphaEnabled(true)
	);

	RID constantBuffer = ds::createConstantBuffer(sizeof(ParticleConstantBuffer), &pCtx->constantBuffer);


	RID ssid = ds::createSamplerState(ds::SamplerStateDesc()
		.AddressMode(ds::TextureAddressModes::CLAMP)
		.Filter(ds::TextureFilters::LINEAR)
	);

	int indices[] = { 0,1,2,1,3,2 };
	RID idxBuffer = ds::createQuadIndexBuffer(maxParticles, indices);

	pCtx->structuredBufferId = ds::createStructuredBuffer(ds::StructuredBufferDesc()
		.CpuWritable(true)
		.ElementSize(sizeof(GPUParticle))
		.NumElements(maxParticles)
		.GpuWritable(false)
	);

	RID basicGroup = ds::StateGroupBuilder()
		.constantBuffer(constantBuffer, vertexShader)
		.structuredBuffer(pCtx->structuredBufferId, vertexShader, 1)
		.vertexBuffer(NO_RID)
		.vertexShader(vertexShader)
		.indexBuffer(idxBuffer)
		.pixelShader(pixelShader)
		.samplerState(ssid, pixelShader)
		.blendState(bs_id)
		.texture(textureID, pixelShader, 0)
		.build();

	ds::DrawCommand drawCmd = { 100, ds::DrawType::DT_INDEXED, ds::PrimitiveTypes::TRIANGLE_LIST };

	pCtx->drawItem = ds::compile(drawCmd, basicGroup, "ParticleDrawItem");

	// create orthographic view
	ds::matrix orthoView = ds::matIdentity();
	ds::matrix orthoProjection = ds::matOrthoLH(ds::getScreenWidth(), ds::getScreenHeight(), 0.1f, 1.0f);
	ds::Camera camera = {
		orthoView,
		orthoProjection,
		orthoView * orthoProjection,
		ds::vec3(0,3,-6),
		ds::vec3(0,0,1),
		ds::vec3(0,1,0),
		ds::vec3(1,0,0),
		0.0f,
		0.0f,
		0.0f
	};

	RID vp = ds::createViewport(ds::ViewportDesc()
		.Top(0)
		.Left(0)
		.Width(ds::getScreenWidth())
		.Height(ds::getScreenHeight())
		.MinDepth(0.0f)
		.MaxDepth(1.0f));

	pCtx->orthoPass = ds::createRenderPass(ds::RenderPassDesc()
		.Camera(&camera)
		.Viewport(vp)
		.DepthBufferState(ds::DepthBufferState::DISABLED)
		.NumRenderTargets(0)
		.RenderTargets(0),
		"ParticleOrthoPass"
	);
}

// -------------------------------------------------------
// create emitter
// -------------------------------------------------------
int particles_create_emitter(const EmitterDesc& desc) {
	EmitterSettings settings = desc.getSettings();
	int cur = pCtx->emitterSettings.size();
	pCtx->emitterSettings.push_back(settings);
	return cur;
}

// -------------------------------------------------------
// create particle system
// -------------------------------------------------------
int particles_create_system(const ParticleSystemDesc& descriptor) {
	Particlesystem* ps = new Particlesystem;
	const ParticlesystemDescriptor& desc = descriptor.getDescriptor();
	ps->descriptor = desc;
	ps->array.initialize(desc.maxParticles);
	pCtx->particleSystems.push_back(ps);
	return pCtx->particleSystems.size() - 1;
}

const EmitterSettings& particles_get_emitter(int id) {
	return pCtx->emitterSettings[id];
}

// -------------------------------------------------------
// prepare constant buffer
// -------------------------------------------------------
static void par__prepare_buffer(const ParticlesystemDescriptor& descriptor) {
	pCtx->constantBuffer.startColor = descriptor.startColor;
	pCtx->constantBuffer.endColor = descriptor.endColor;
	pCtx->constantBuffer.texture = descriptor.textureRect;
	descriptor.particleDimension.store(pCtx->constantBuffer.dimension);
	pCtx->constantBuffer.dummy[0] = 0.0f;
	pCtx->constantBuffer.dummy[1] = 0.0f;
}

// -------------------------------------------------------
// tick particles
// -------------------------------------------------------
void particles_tick(float dt) {
	for (size_t p = 0; p < pCtx->particleSystems.size(); ++p) {
		Particlesystem* ps = pCtx->particleSystems[p];
		for (int i = 0; i < ps->array.countAlive; ++i) {
			ds::vec3 t = ps->array.timers[i];
			t.x += dt;
			t.z = t.x / t.y;
			ps->array.timers[i] = t;
			if (t.z >= 1.0f) {
				ps->array.kill(i);
			}
		}
	}
}

// -------------------------------------------------------
// render particles
// -------------------------------------------------------
void particles_render() {
	ds::matrix w = ds::matIdentity();
	pCtx->constantBuffer.wvp = ds::matTranspose(pCtx->viewprojectionMatrix);
	for (size_t p = 0; p < pCtx->particleSystems.size(); ++p) {
		const ParticlesystemDescriptor& desc = pCtx->particleSystems[p]->descriptor;
		par__prepare_buffer(desc);
		const ParticleArray& array = pCtx->particleSystems[p]->array;
		if (array.countAlive > 0) {
			for (int i = 0; i < array.countAlive; ++i) {
				array.positions[i].store(pCtx->vertices[i].position);
				array.velocities[i].store(pCtx->vertices[i].velocity);
				array.accelerations[i].store(pCtx->vertices[i].acceleration);
				array.scales[i].store(pCtx->vertices[i].scale);
				array.growth[i].store(pCtx->vertices[i].growth);
				pCtx->vertices[i].timer[0] = array.timers[i].x;
				pCtx->vertices[i].timer[1] = array.timers[i].z;
				pCtx->vertices[i].rotation = array.rotations[i];
			}
			ds::mapBufferData(pCtx->structuredBufferId, pCtx->vertices, array.countAlive * sizeof(GPUParticle));
			ds::submit(pCtx->orthoPass, pCtx->drawItem, array.countAlive * 6);
		}
	}
}

// -------------------------------------------------------
// emitt particles
// -------------------------------------------------------
void particles_emitt(uint32_t id, const ds::vec2& pos, const EmitterSettings& emitter) {
	ParticleArray& array = pCtx->particleSystems[id]->array;
	int start = array.countAlive;
	for (int i = 0; i < emitter.count; ++i) {
		if ((start + 1) < array.count) {
			float angle = ds::TWO_PI * static_cast<float>(i) / static_cast<float>(emitter.count);
			float x = pos.x() + cos(angle) * (emitter.radius + ds::random(-emitter.radiusVariance, emitter.radiusVariance));
			float y = pos.y() + sin(angle) * (emitter.radius + ds::random(-emitter.radiusVariance, emitter.radiusVariance));
			float da = angle * emitter.angleVariance;
			angle += ds::random(-da, da);
			array.rotations[start] = angle;
			ds::vec2 s(1.0f);
			s.setX(emitter.size.x() + ds::random(-emitter.sizeVariance.x(), emitter.sizeVariance.x()));
			if (s.x() <= 0.0f) {
				s.setX(0.1f);
			}
			s.setY(emitter.size.y() + ds::random(-emitter.sizeVariance.y(), emitter.sizeVariance.y()));
			if (s.y() <= 0.0f) {
				s.setY(0.1f);
			}
			array.scales[start] = s;
			array.growth[start] = emitter.growth;
			float ttl = ds::random(emitter.ttl.x(), emitter.ttl.y());
			array.timers[start] = ds::vec3(0.0f, ttl, 1.0f);
			array.velocities[start] = (emitter.radialVelocity + ds::random(-emitter.velocityVariance,emitter.velocityVariance)) * ds::vec2(cos(angle), sin(angle));
			array.accelerations[start] = (emitter.radialAcceleration) * ds::vec2(cos(angle), sin(angle));
			array.positions[start] = ds::vec2(x, y);
			array.wake(start);
			++start;
		}
	}
}