#include "ParticleExpressionTest.h"
#include "utils\tweening.h"
#include "AABBox.h"
#include <Windows.h>
#include <fstream>
#include <string>

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
	_vmCtx.add_variable("RND", 1.0f);

	loadExpressionsFile("particles\\ring_emitter.txt", _emitterExpressions, &emitter);
	loadExpressionsFile("particles\\motion.txt", _moduleExpressions, &modules);

	_numParticles = 64;
	_showParticleInfo = false;
	_selectedType = 0;
	_selectedExpression = -1;
}

ParticleExpressionTest::~ParticleExpressionTest() {
}

void ParticleExpressionTest::loadExpressionsFile(const char* name, ParticleExpression* expressions, ParticleEmitter* ret) {
	std::ifstream ifs(name);
	std::string line;
	uint16_t cnt = 0;
	while (std::getline(ifs, line)) {
		// skip empty lines:
		if (line.empty() || line == "-") {
			++cnt;
			continue;
		}
		ParticleExpression& exp = expressions[cnt];
		snprintf(exp.source, 256, "%s", line.c_str());
		vm::parse(exp.source, _vmCtx, exp.expression);
		ret->connections[cnt] = { ALL_CHANNELS[cnt],&exp.expression, cnt };
		++cnt;
	}
}

void ParticleExpressionTest::emitt(uint16_t num, const ds::vec2& pos) {
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
					if (cc.channel == PC_POS_X ) {
						r += pos.x;
					}
					if (cc.channel == PC_POS_Y) {
						r += pos.y;
					}
					_particles.set_value(cc.channel, start + i, r);
				}
			}
		}
	}
}

void ParticleExpressionTest::renderExpressions(ParticleEmitter& emitter, ParticleExpression* expressions) {
	p2i p = gui::getCurrentPosition();
	p2i t = p;
	for (int i = 0; i < NUM_CHANNELS; ++i) {
		gui::draw_text(t, CHANNEL_NAMES[i]);
		t.x += 100;
		const ChannelConnection& cc = emitter.connections[i];
		if (cc.expression != 0) {
			uint16_t id = cc.id;
			gui::pushID(CHANNEL_NAMES[i]);
			gui::checkItem(t, p2i(60, 16));
			if (gui::isClicked()) {
				_selectedExpression = i;
			}
			gui::draw_box(t, p2i(60, 16), gui::getSettings().buttonColor);
			gui::draw_text(t, "edit");
			t.x += 80;
			gui::draw_box(t, p2i(20, 16), gui::getSettings().buttonColor);
			gui::draw_text(t, "-");
		}
		else {
			t.x += 80;
			gui::draw_box(t, p2i(20, 16), gui::getSettings().buttonColor);
			gui::draw_text(t, "+");
		}
		t.x = p.x;
		t.y -= 22;
	}
	gui::moveForward(p2i(200, NUM_CHANNELS * 22));
	if (_selectedExpression != -1) {
		const ChannelConnection& cc = emitter.connections[_selectedExpression];
		ParticleExpression pexp = expressions[cc.id];
		gui::Input("Source",pexp.source,256);			
	}
}

void ParticleExpressionTest::renderGUI() {
	
	gui::start();
	p2i sp = p2i(10, 760);
	if (gui::begin("Debug", &_dialogState, &_dialogPos, 300)) {
		gui::Value("FPS", ds::getFramesPerSecond());
		gui::Value("Particles", _particles.num);
		gui::Input("Num", &_numParticles);
		if (gui::Button("Start")) {
			float px = ds::random(300, 600);
			float py = ds::random(200, 400);
			emitt(_numParticles, ds::vec2(px,py));
		}
		static const char* RButtons[] = {"Emitter","Motion"};
		gui::RadioButtons(RButtons, 2, &_selectedType);
		if (_selectedType == 0) {
			renderExpressions(emitter, _emitterExpressions);
		}
		else {
			renderExpressions(modules, _moduleExpressions);
		}
		gui::Checkbox("Show Info", &_showParticleInfo);
		if (_showParticleInfo) {
			if (_particles.num > 0) {
				for (int i = 0; i < NUM_CHANNELS; ++i) {
					gui::Value(CHANNEL_NAMES[i], _particles.get_value(ALL_CHANNELS[i], 0));
				}
			}
		}
	}
	gui::end();
	
}

void ParticleExpressionTest::render() {
	uint16_t cnt = 0;
	// handle lifetime
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

	// move
	cnt = 0;
	float* px = _particles.get_channel(PC_POS_X);
	float* py = _particles.get_channel(PC_POS_Y);
	float* vx = _particles.get_channel(PC_VELOCITY_X);
	float* vy = _particles.get_channel(PC_VELOCITY_Y);
	while (cnt < _particles.num) {
		*px += *vx * ds::getElapsedSeconds();
		*py += *vy * ds::getElapsedSeconds();
		++cnt;
		++px;
		++py;
		++vx;
		++vy;
	}

	// update others
	for (int j = 0; j < NUM_CHANNELS; ++j) {
		if (modules.connections[j].expression != 0) {
			const ChannelConnection& cc = modules.connections[j];
			float* c = _particles.get_channel(cc.channel);
			for (int i = 0; i < _particles.num; ++i) {
				_vmCtx.variables[5].value = ds::getElapsedSeconds();
				_vmCtx.variables[6].value = _particles.get_value(PC_TIMER,i);
				_vmCtx.variables[7].value = _particles.get_value(PC_TTL, i);
				_vmCtx.variables[8].value = _particles.get_value(PC_RAND, i);
				*c = vm::run(cc.expression->tokens, cc.expression->num, _vmCtx);
				++c;
			}
		}
	}

	// render
	cnt = 0;
	px = _particles.get_channel(PC_POS_X);
	py = _particles.get_channel(PC_POS_Y);
	float* r = _particles.get_channel(PC_ROTATION);
	float* sx = _particles.get_channel(PC_SCALE_X);
	float* sy = _particles.get_channel(PC_SCALE_Y);
	float* ca = _particles.get_channel(PC_COLOR_A);
	while ( cnt < _particles.num) {
		_sprites->add(ds::vec2(*px, *py), ds::vec4(0, 100, 4, 4),ds::vec2(*sx,*sy),*r,ds::Color(1.0f,1.0f,1.0f,*ca));
		++cnt;
		++px;
		++py;
		++sx;
		++sy;
		++ca;
		++r;
	}
}