#include "ParticlePluginTest.h"
#include <Windows.h>
#include <SpriteBatchBuffer.h>
#include <string>
#include <assert.h>
#include "plugins\PluginRegistry.h"
#include "..\plugins\particle_manager\particle_manager.h"

const static ds::vec2 SIDES[] = { ds::vec2(1,0),ds::vec2(0,-1),ds::vec2(-1,0),ds::vec2(0,1) };

static EnemySettings SETTINGS[] = {
	{ ds::Color(200, 160, 160, 255), 24, 3.0f, 2.0f,  40.f, 10.0f, 350.0f, 1.0f },
	{ ds::Color(130, 130,  50, 255), 32, 4.0f, 2.0f,  60.f, 10.0f, 250.0f, 1.0f },
	{ ds::Color( 70, 163,  10, 255), 40, 5.0f, 2.0f,  80.f, 10.0f, 150.0f, 1.25f },
	{ ds::Color(200, 110,  10, 255), 56, 6.0f, 2.0f, 120.f, 15.0f, 100.0f, 1.5f }
};

Enemy::Enemy(EnemySettings* settings) : _settings(settings) , _radius(0) {
	resize();
	_position = { 640.0f,480.0f };
	float rad = ds::random(0.0f, ds::TWO_PI);
	float deg = rad * 360.0f / ds::TWO_PI;
	_velocity = { cos(rad) * _settings->radialVelocity,sin(rad) * _settings->radialVelocity };
}

Enemy::~Enemy() {
	delete[] _radius;
}

void Enemy::resize() {
	if (_radius != 0) {
		delete[] _radius;
	}
	_radius = new float[_settings->pieces];
	for (int i = 0; i < _settings->pieces; ++i) {
		float a = static_cast<float>(i) / static_cast<float>(_settings->pieces) * ds::TWO_PI * _settings->amplitude;
		_radius[i] = sin(a) * _settings->radiusVariance;
	}
	
}

void Enemy::tick(float dt) {
	_timer += dt;
	_position += _velocity * dt;

	ds::vec2 rn = normalize(_velocity);
	if (_position.x < _settings->radius || _position.x > (1280.0f - _settings->radius)) {
		_velocity.x *= -1.0f;
	}
	if (_position.y < _settings->radius || _position.y > (960.0f - _settings->radius)) {
		_velocity.y *= -1.0f;
	}
	/*
	for (int i = 0; i < 4; ++i) {
		float d = dot(rn, SIDES[i]);
		if (d < 0.0f) {
			LOG_DEBUG("%d", i);
		}
	}
	*/

}

void Enemy::render(SpriteBatchBuffer* buffer) {
	ds::Color c = _settings->color;
	c.a = 0.25f;
	float s = _settings->scale;
	for (int i = 0; i < _settings->pieces; ++i) {
		float a = static_cast<float>(i) / static_cast<float>(_settings->pieces) * ds::TWO_PI;
		float r = sin(_timer * _settings->frequency) * _radius[i] + _settings->radius;
		float x = _position.x + cos(a + _timer) * r;
		float y = _position.y + sin(a + _timer) * r;
		buffer->add(ds::vec2(x, y), ds::vec4(451, 1, 46, 46), ds::vec2(s), a, c);
	}
	c.a = 0.75f;
	s -= 0.25f;
	for (int i = 0; i < _settings->pieces; ++i) {
		float a = static_cast<float>(i) / static_cast<float>(_settings->pieces) * ds::TWO_PI;
		float r = sin(_timer * _settings->frequency) * _radius[i] + _settings->radius;
		float x = _position.x + cos(a + _timer) * r;
		float y = _position.y + sin(a + _timer) * r;
		buffer->add(ds::vec2(x, y), ds::vec4(451, 1, 46, 46), ds::vec2(s), a, c);
	}

}

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

	//_enemySettings.color = ds::Color(0, 200, 150, 255);
	//_enemySettings.pieces = 36;
	//_enemySettings.amplitude = 3.0f;
	//_enemySettings.frequency = 2.0f;
	//_enemySettings.radius = 80.0f;
	//_enemySettings.radiusVariance = 20.0f;
	
	//_enemy = new Enemy(&_enemySettings);
	
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
	if (gui::begin("Debug", &_dialogState, &_dialogPos, 420)) {
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
		// enemy
		/*
		if (gui::Input("Pieces", &_enemySettings.pieces)) {
			_enemy->resize();
		}
		if (gui::Input("Amplitude", &_enemySettings.amplitude)) {
			_enemy->resize();
		}
		gui::Input("Frequency", &_enemySettings.frequency);
		gui::Input("Radius", &_enemySettings.radius);
		if (gui::Input("R-Variance", &_enemySettings.radiusVariance)) {
			_enemy->resize();
		}
		gui::Input("Color", &_enemySettings.color);
		*/
		if (gui::Button("Start 1")) {
			Enemy* e = new Enemy(&SETTINGS[0]);
			_enemies.emplace_back(e);
		}
		if (gui::Button("Start 2")) {
			Enemy* e = new Enemy(&SETTINGS[1]);
			_enemies.emplace_back(e);
		}
		if (gui::Button("Start 3")) {
			Enemy* e = new Enemy(&SETTINGS[2]);
			_enemies.emplace_back(e);
		}
		if (gui::Button("Start 4")) {
			Enemy* e = new Enemy(&SETTINGS[3]);
			_enemies.emplace_back(e);
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

	//_player.angle = get_rotation(_player.position - _player.previous);
	_player.previous = _player.position;
	_player.position.y = (15.0f * cos(_player.timer * -6.0f)) + (480.0f + (300.0f * sin(_player.timer * 1.3f)));
	_player.position.x = (15.0f * sin(_player.timer * -6.0f)) + (640.0f + (280.0f * cos(_player.timer / 1.5f)));
	_player.trailTimer += dt;
	pm->emitt_trail(_trailData, _player.position, &_player.trailTimer);
	pm->tick(_trailData, dt);
	pm->update_trail(_trailData, dt);

	for (size_t i = 0; i < _enemies.size(); ++i) {
		_enemies[i]->tick(dt);
	}
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

	_sprites->flush();

	_particleBuffer->begin();

	for (size_t i = 0; i < _enemies.size(); ++i) {
		_enemies[i]->render(_particleBuffer);
	}

	//_sprites->flush();

	//_particleBuffer->begin();
	
	renderParticles(_data, ds::vec4(220, 30, 20, 20));

	renderParticles(_trailData, ds::vec4(220, 30, 20, 20));

	

	_particleBuffer->add(ds::vec2(512, 384), ds::vec4(200, 0, 20, 20), ds::vec2(1, 1), 0.0f, ds::Color(255, 0, 0, 255));
	_particleBuffer->add(ds::vec2(522, 384), ds::vec4(200, 0, 20, 20), ds::vec2(1, 1), 0.0f, ds::Color(0, 255, 0, 255));
	
	_particleBuffer->flush();

	_sprites->begin();

	_sprites->add(_player.position, ds::vec4(350, 0, 52, 52),ds::vec2(1.0f),_player.angle);

}
