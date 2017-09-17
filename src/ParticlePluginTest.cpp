#include "ParticlePluginTest.h"
#include <Windows.h>
#include <SpriteBatchBuffer.h>
#include <string>
#include <assert.h>
#include "plugins\PluginRegistry.h"
#include "..\plugins\particle_manager\particle_manager.h"

ParticlePluginTest::ParticlePluginTest(plugin_registry* registry, SpriteBatchBuffer* buffer) : _sprites(buffer) {

	_dialogPos = p2i(10, 950);
	_dialogState = 1;

	_numParticles = 64;
	_emissionTime = 2.0f;
#ifdef DEBUG
	registry->load_plugin("plugins\\particle_manager\\x64\\Debug", "particle_manager");
	OutputDebugString(registry->get_last_error());
#else
	load_particle_manager(registry);
#endif
	_instance = registry->get(PARTICLE_MANAGER_NAME);
	particle_manager* pm = (particle_manager*)_instance->data;
	assert(pm != 0);
	_data = new ParticleData;
	pm->allocate(_data, 4096);
	_trailData = new ParticleData;
	pm->allocate(_trailData, 4096);
	assert(_instance != 0);
	_running = false;
	_timer = 0.0f;
	_delay = 2.0f;

	_player.position = { 512,384 };
	_player.previous = _player.position;
	_player.trailPos = _player.position;
	_player.timer = 0.0f;
	_player.angle = 0.0f;
	_player.trailTimer = 0.0f;

	RID textureID = ds::findResource(SID("content\\TextureArray.png"), ds::ResourceType::RT_SRV);
	ds::BlendStateInfo blendStateInfo{ ds::BlendStates::ONE, ds::BlendStates::ZERO, ds::BlendStates::ONE, ds::BlendStates::ONE, true };
	RID blendState = ds::createBlendState(blendStateInfo,"ParticleBlendState");
	SpriteBatchBufferInfo sbbInfo = { 4096, textureID, ds::TextureFilters::LINEAR, blendState };
	_particleBuffer = new SpriteBatchBuffer(sbbInfo);
}

ParticlePluginTest::~ParticlePluginTest() {
	delete _particleBuffer;
	delete[] _data->buffer;
	delete _data;
}

void ParticlePluginTest::renderGUI() {
	gui::start();
	p2i sp = p2i(10, 760);
	if (gui::begin("Debug", &_dialogState, &_dialogPos, 300)) {
		float fps = ds::getFramesPerSecond();
		gui::Value("FPS", fps);
		float ti = 0.0f;
		if (fps > 0.0f) {
			ti = 1.0f / fps * 1000.0f;
		}
		gui::Value("TI", ti,3,4);
		gui::Value("Particles", _data->index);
		gui::Value("Trail", _trailData->index);
		gui::Input("Num", &_numParticles);
		gui::Input("Delay", &_delay);
		if (!_running && gui::Button("Start")) {
			_timer = 0.0f;
			_running = true;
		}		
		if (_running && gui::Button("Stop")) {
			_running = false;
		}
	}
	
	gui::end();
	
}

// ----------------------------------------------------
// tick
// ----------------------------------------------------
void ParticlePluginTest::tick(float dt) {
	particle_manager* pm = (particle_manager*)_instance->data;
	if (_running) {
		_timer += dt;
		if (_timer >= _delay) {
			ds::vec2 ep = { ds::random(500, 700), ds::random(200, 400) };
			pm->emitt_explosion(_data, ep);
			_timer -= _delay;
		}
	}
	pm->tick(_data, dt);
	pm->update_explosion(_data, dt);

	_player.timer += dt;

	_player.angle = get_rotation(_player.position - _player.previous);
	_player.previous = _player.position;
	_player.position.y = (15.0f * cos(_player.timer * -6.0f)) + (480.0f + (300.0f * sin(_player.timer * 1.3f)));
	_player.position.x = (15.0f * sin(_player.timer * -6.0f)) + (640.0f + (280.0f * cos(_player.timer / 1.5f)));
	_player.trailTimer += dt;
	pm->emitt_trail(_trailData, _player.position, &_player.trailTimer);
	pm->tick(_trailData, dt);
	pm->update_trail(_trailData, dt);
}
	

void ParticlePluginTest::renderParticles(ParticleData* data, const ds::vec4& rect) {
	ds::vec2* pos = data->positions;
	ds::vec2* scales = data->scales;
	float* r = data->rotations;
	ds::Color* c = data->colors;
	uint16_t cnt = 0;
	while (cnt < data->index) {
		_particleBuffer->add(*pos, rect, *scales, *r, *c);
		++cnt;
		++pos;
		++scales;
		++r;
		++c;
	}
}

// ----------------------------------------------------
// render
// ----------------------------------------------------
void ParticlePluginTest::render() {
	/*
	for (int y = 0; y < 4; ++y) {
		for (int x = 0; x < 5; ++x) {
			_sprites->add(ds::vec2(150 + x * 300, 120 + y * 240), ds::vec4(400, 100, 300, 240));
		}
	}
	*/

	for (int i = 0; i < 36; ++i) {
		float a = static_cast<float>(i) / 36.0f * ds::TWO_PI;
		float r = sin(a) * 15.0f + 80.0f;
		float x = 640 + cos(a) * r;
		float y = 480 + sin(a) * r;
		_sprites->add(ds::vec2(x, y), ds::vec4(426, 1, 18, 18), ds::vec2(1, 1), a, ds::Color(255, 255, 255, 255));
	}

	for (int i = 0; i < 36; ++i) {
		float a = static_cast<float>(i) / 36.0f * ds::TWO_PI;
		float r = sin(a) * 15.0f + 80.0f;
		float x = 640 + cos(a) * r;
		float y = 480 + sin(a) * r;
		_sprites->add(ds::vec2(x, y), ds::vec4(426, 1, 18, 18), ds::vec2(0.85f), a, ds::Color(255, 0, 0, 255));
	}

	_sprites->flush();

	_particleBuffer->begin();
	
	renderParticles(_data, ds::vec4(220, 30, 20, 20));

	renderParticles(_trailData, ds::vec4(220, 30, 20, 20));

	

	_particleBuffer->add(ds::vec2(512, 384), ds::vec4(200, 0, 20, 20), ds::vec2(1, 1), 0.0f, ds::Color(255, 0, 0, 255));
	_particleBuffer->add(ds::vec2(522, 384), ds::vec4(200, 0, 20, 20), ds::vec2(1, 1), 0.0f, ds::Color(0, 255, 0, 255));
	
	_particleBuffer->flush();

	_sprites->begin();

	_sprites->add(_player.position, ds::vec4(350, 0, 52, 52),ds::vec2(1.0f),_player.angle);

}
