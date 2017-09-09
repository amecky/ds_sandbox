#include "ParticlePluginTest.h"
#include <Windows.h>
#include <string>
#include <assert.h>
#include "plugins\PluginRegistry.h"
#include "..\plugins\particle_manager\particle_manager.h"

ParticlePluginTest::ParticlePluginTest(const plugin_registry& registry, SpriteBatchBuffer* buffer) : _sprites(buffer) {

	_dialogPos = p2i(10, 950);
	_dialogState = 1;

	_numParticles = 64;
	_emissionTime = 2.0f;
	registry.load_plugin("plugins\\particle_manager\\x64\\Debug", "particle_manager");
	_instance = registry.get(PARTICLE_MANAGER_NAME);
	particle_manager* pm = (particle_manager*)_instance->data;
	_data = new ParticleData;
	pm->allocate(_data, 4096);
	assert(_instance != 0);
	_running = false;
	_timer = 0.0f;
	_delay = 2.0f;
}

ParticlePluginTest::~ParticlePluginTest() {
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
			float px = ds::random(500, 700);
			float py = ds::random(200, 400);

			pm->emitt_explosion(_data);
			_timer -= _delay;
		}
	}
	pm->tick(_data, dt);
	pm->update_explosion(_data, dt);
}

// ----------------------------------------------------
// render
// ----------------------------------------------------
void ParticlePluginTest::render() {
	ds::vec2* pos = _data->positions;
	ds::vec2* scales = _data->scales;
	float* r = _data->rotations;
	ds::Color* c = _data->colors;
	uint16_t cnt = 0;
	while (cnt < _data->index) {
		_sprites->add(*pos, ds::vec4(220, 30, 20, 20), *scales, *r, *c);
		++cnt;
		++pos;
		++scales;
		++r;
		++c;
	}
}
