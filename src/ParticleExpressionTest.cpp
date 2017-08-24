#include "ParticleExpressionTest.h"
#include "utils\tweening.h"
#include "AABBox.h"
#include <Windows.h>

ParticleExpressionTest::ParticleExpressionTest(SpriteBatchBuffer* buffer) : _sprites(buffer) , _particles(256) {

	_dialogPos = p2i(10, 950);
	_dialogState = 1;

	_vmCtx.add_constant("PI", 3.141592654f);
	_vmCtx.add_constant("TWO_PI", 2.0f * 3.141592654f);
	_vmCtx.add_variable("DT", 1.0f);
	_vmCtx.add_variable("IDX", 1.0f);
	_vmCtx.add_variable("COUNT", 32.0f);
	_vmCtx.add_variable("DT", 0.0f);
	_vmCtx.add_variable("TIMER", 0.0f);
	_vmCtx.add_variable("TTL", 1.0f);
	vm::parse("40 * cos(TWO_PI * IDX / COUNT) * random(1.0,1.4)", _vmCtx, pxExp);
	vm::parse("40 * sin(TWO_PI * IDX / COUNT) * random(1.0,1.4)", _vmCtx, pyExp);
	vm::parse("0.5 + 1.2 * IDX / COUNT", _vmCtx, ttlExp);
	emitter.connections[0] = { PC_POS_X, &pxExp };
	emitter.connections[1] = { PC_POS_Y, &pyExp };
	emitter.connections[2] = { PC_TTL, &ttlExp };

	vm::parse("1.0 - TIMER/TTL", _vmCtx, fadeExp);
	vm::parse("1.0 + abs(sin(TIMER/TTL * TWO_PI)) * 2", _vmCtx, scaleExp);
	modules.connections[0] = { PC_COLOR_A,&fadeExp };
	modules.connections[1] = { PC_SCALE_X,&scaleExp };
	modules.connections[2] = { PC_SCALE_Y,&scaleExp };
}

ParticleExpressionTest::~ParticleExpressionTest() {
}

void ParticleExpressionTest::emitt(uint16_t num) {
	uint16_t start = 0;
	uint16_t cnt = 0;
	_vmCtx.variables[4].value = num;
	if (_particles.wake_up(num, &start, &cnt)) {
		for (int i = 0; i < cnt; ++i) {
			_vmCtx.variables[3].value = i;
			for (int j = 0; j < NUM_CHANNELS; ++j) {
				if (emitter.connections[j].expression != 0) {
					const ChannelConnection& cc = emitter.connections[j];
					float r = vm::run(cc.expression->tokens, cc.expression->num, _vmCtx);
					_particles.set_value(cc.channel, start + i, r);
				}
			}
		}
	}
}

void ParticleExpressionTest::renderGUI() {
	
	gui::start();
	p2i sp = p2i(10, 760);
	if (gui::begin("Debug", &_dialogState, &_dialogPos, 200)) {
		gui::Value("FPS", ds::getFramesPerSecond());
		gui::Value("Particles", _particles.num);
		if (gui::Button("Start")) {
			emitt(16);
		}
		if (_particles.num > 0) {
			for (int i = 0; i < NUM_CHANNELS; ++i) {
				gui::Value(CHANNEL_NAMES[i], _particles.get_value(ALL_CHANNELS[i], 0));
			}
		}
	}
	gui::end();
	
}

void ParticleExpressionTest::render() {
	uint16_t cnt = 0;
	float* timer = _particles.get_channel(PC_TIMER);
	float* ttl = _particles.get_channel(PC_TTL);
	while (cnt < _particles.num) {
		*timer += ds::getElapsedSeconds();
		if (*timer > *ttl) {
			_particles.remove(cnt);
		}
		else {
			++timer;
			++ttl;
			++cnt;
		}
	}

	for (int i = 0; i < _particles.num; ++i) {
		_vmCtx.variables[5].value = ds::getElapsedSeconds();
		_vmCtx.variables[6].value = _particles.get_value(PC_TIMER,i);
		_vmCtx.variables[7].value = _particles.get_value(PC_TTL, i);
		for (int j = 0; j < NUM_CHANNELS; ++j) {
			if (modules.connections[j].expression != 0) {
				const ChannelConnection& cc = modules.connections[j];
				float r = vm::run(cc.expression->tokens, cc.expression->num, _vmCtx);
				_particles.set_value(cc.channel, i, r);
			}
		}
	}

	cnt = 0;
	float* px = _particles.get_channel(PC_POS_X);
	float* py = _particles.get_channel(PC_POS_Y);
	float* sx = _particles.get_channel(PC_SCALE_X);
	float* sy = _particles.get_channel(PC_SCALE_Y);
	float* ca = _particles.get_channel(PC_COLOR_A);
	while ( cnt < _particles.num) {
		_sprites->add(ds::vec2(512 + *px, 380 + *py), ds::vec4(0, 100, 4, 4),ds::vec2(*sx,*sy),0.0f,ds::Color(1.0f,1.0f,1.0f,*ca));
		++cnt;
		++px;
		++py;
		++sx;
		++sy;
		++ca;
	}
}
