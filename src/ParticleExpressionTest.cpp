#include "ParticleExpressionTest.h"
#include "utils\tweening.h"
#include "AABBox.h"
#include <Windows.h>
#include <fstream>
#include <string>
#include "LogPanel.h"

ParticleExpressionTest::ParticleExpressionTest(SpriteBatchBuffer* buffer) : _sprites(buffer) , _particles(4096) {

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
	_vmCtx.add_variable("NORM", 1.0f);

	loadExpressionsFile("particles\\ring_emitter.txt", _emitterExpressions, &emitter);
	loadExpressionsFile("particles\\motion.txt", _moduleExpressions, &modules);

	_numParticles = 64;
	_showParticleInfo = false;
	_selectedType = 0;
	_selectedExpression = -1;

	_perfIndex = 0;
	_perfMax = 0;
	_perfTimer = 0.0f;
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
	perf::ZoneTracker("renderExpressions");
	p2i p = gui::getCurrentPosition();
	p2i t = p;
	for (uint16_t i = 0; i < NUM_CHANNELS; ++i) {
		gui::draw_text(t, CHANNEL_NAMES[i]);
		t.x += 100;
		const ChannelConnection& cc = emitter.connections[i];
		if (cc.expression != 0) {
			uint16_t id = cc.id;
			gui::pushID(CHANNEL_NAMES[i],i);
			gui::checkItem(t, p2i(60, 16));
			if (gui::isClicked()) {
				_selectedExpression = i;
			}
			gui::draw_box(t, p2i(60, 16), gui::getSettings().buttonColor);
			gui::draw_text(t, "edit");
			t.x += 80;
			gui::popID();
			gui::pushID("-",i);
			gui::checkItem(t, p2i(20, 16));
			if (gui::isClicked()) {
				emitter.connections[i].expression = 0;
			}
			gui::draw_box(t, p2i(20, 16), gui::getSettings().buttonColor);
			gui::draw_text(t, "-");
			gui::popID();
		}
		else {
			t.x += 80;
			gui::pushID("-", i);
			gui::checkItem(t, p2i(20, 16));
			if (gui::isClicked()) {
				ParticleExpression& exp = expressions[i];
				snprintf(exp.source, 256, "1");
				vm::parse(exp.source, _vmCtx, exp.expression);
				emitter.connections[i] = { ALL_CHANNELS[i],&exp.expression, i };
			}
			gui::draw_box(t, p2i(20, 16), gui::getSettings().buttonColor);
			gui::draw_text(t, "+");
			gui::popID();
		}
		t.x = p.x;
		t.y -= 22;
	}
	gui::moveForward(p2i(200, NUM_CHANNELS * 22));
	if (_selectedExpression != -1) {
		ChannelConnection& cc = emitter.connections[_selectedExpression];
		ParticleExpression& pexp = expressions[cc.id];
		if (gui::Input("Source", pexp.source, 256)) {
			vm::parse(pexp.source, _vmCtx, pexp.expression);
		}
	}
}

void ParticleExpressionTest::convertFloat(float v,int precision) {
	int cnt = 0;
	if (v < 0.0f) {
		_tmpBuffer[cnt++] = '-';
	}
	int t = static_cast<int>(v);
	int f = t / 10;
	if (f > 0) {
		_tmpBuffer[cnt++] = 48 + f;
	}
	t -= f * 10;
	_tmpBuffer[cnt++] = 48 + t;
	_tmpBuffer[cnt++] = '.';


	double r = 0.0;
	int start = pow(10, precision - 1);
	float frac = modf(v, &r) * start;
	t = static_cast<int>(frac);
	for (int i = 0; i < precision; ++i) {
		f = t / start;
		if (f >= 0) {
			_tmpBuffer[cnt++] = 48 + f;
		}
		t -= f * start;
		start /= 10;
	}
	_tmpBuffer[cnt++] = '\0';
}

void ParticleExpressionTest::renderGUI() {
	perf::ZoneTracker("renderGUI");
	gui::start();
	p2i sp = p2i(10, 760);
	if (gui::begin("Debug", &_dialogState, &_dialogPos, 300)) {
		gui::Value("FPS", ds::getFramesPerSecond());
		float ti = 1.0f / ds::getFramesPerSecond() * 1000.0f;
		gui::Value("TI", ti,3,4);
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

	if (perf::num_events() > 0) {
		for (size_t i = 0; i < perf::num_events(); ++i) {
			gui::Value(perf::get_name(i), perf::avg(i), 4, 8);
		}
		gui::Value("Total", perf::avg_total(), 4, 8);
	}
	if (gui::Button("save")) {
		FILE* fp = fopen("perf.txt", "w");
		if (fp) {
			for (size_t i = 0; i < perf::num_events(); ++i) {
				fprintf(fp, "%s %2.5f %d\n", perf::get_name(i), perf::avg(i),perf::num_calls(i));
				LOG_DEBUG("%s %2.5f %d", perf::get_name(i), perf::avg(i), perf::num_calls(i));
			}
			fprintf(fp, "Total %2.5f\n", perf::avg_total());
			LOG_DEBUG("Total %2.5f", perf::avg_total());
			fclose(fp);
		}
	}

	gui::Histogram(_perfValues, _perfIndex, 0.0f, 5.0f, 1.0f);


	//logpanel::draw_gui(8);
	gui::end();
	
}

// ----------------------------------------------------
// move by particles by velocity
// ----------------------------------------------------
void ParticleExpressionTest::moveParticles() {
	perf::ZoneTracker("moveParticles");
	uint16_t cnt = 0;
	float elapsed = ds::getElapsedSeconds();
	uint16_t num = _particles.num;
	float* px = _particles.get_channel(PC_POS_X);
	float* py = _particles.get_channel(PC_POS_Y);
	float* vx = _particles.get_channel(PC_VELOCITY_X);
	float* vy = _particles.get_channel(PC_VELOCITY_Y);
	while (cnt < num) {
		*px += *vx * elapsed;
		*py += *vy * elapsed;
		++cnt;
		++px;
		++py;
		++vx;
		++vy;
	}
}

// ----------------------------------------------------
// kill dead particles
// ----------------------------------------------------
void ParticleExpressionTest::manageLifecycles() {
	perf::ZoneTracker("manageLifecycles");
	uint16_t cnt = 0;
	float elapsed = ds::getElapsedSeconds();
	float* timer = _particles.get_channel(PC_TIMER);
	float* ttl = _particles.get_channel(PC_TTL);
	while (cnt < _particles.num) {
		*timer += elapsed;
		if (*timer > *ttl) {
			_particles.remove(cnt);
		}
		else {
			++timer;
			++ttl;
			++cnt;
		}
	}
}

// ----------------------------------------------------
// update particles
// ----------------------------------------------------
void ParticleExpressionTest::updateParticles() {
	perf::ZoneTracker("updateParticles");
	for (int j = 0; j < NUM_CHANNELS; ++j) {
		if (modules.connections[j].expression != 0) {
			const ChannelConnection& cc = modules.connections[j];
			float* c = _particles.get_channel(cc.channel);
			for (int i = 0; i < _particles.num; ++i) {
				_vmCtx.variables[5].value = ds::getElapsedSeconds();
				_vmCtx.variables[6].value = _particles.get_value(PC_TIMER, i);
				_vmCtx.variables[7].value = _particles.get_value(PC_TTL, i);
				_vmCtx.variables[8].value = _particles.get_value(PC_RAND, i);
				_vmCtx.variables[9].value = _vmCtx.variables[6].value / _vmCtx.variables[7].value;
				*c = vm::run(cc.expression->tokens, cc.expression->num, _vmCtx);
				++c;
			}
		}
	}
}

// ----------------------------------------------------
// render
// ----------------------------------------------------
void ParticleExpressionTest::render() {

	manageLifecycles();
	
	moveParticles();

	updateParticles();

	perf::ZoneTracker("render");
	// render
	uint16_t cnt = 0;
	float* px = _particles.get_channel(PC_POS_X);
	float* py = _particles.get_channel(PC_POS_Y);
	float* r = _particles.get_channel(PC_ROTATION);
	float* sx = _particles.get_channel(PC_SCALE_X);
	float* sy = _particles.get_channel(PC_SCALE_Y);
	float* cr = _particles.get_channel(PC_COLOR_R);
	float* cg = _particles.get_channel(PC_COLOR_G);
	float* cb = _particles.get_channel(PC_COLOR_B);
	float* ca = _particles.get_channel(PC_COLOR_A);
	while ( cnt < _particles.num) {
		_sprites->add(ds::vec2(*px, *py), ds::vec4(200, 0, 4, 4),ds::vec2(*sx,*sy),*r,ds::Color(*cr,*cg,*cb,*ca));
		++cnt;
		++px;
		++py;
		++sx;
		++sy;
		++cr;
		++cg;
		++cb;
		++ca;
		++r;
	}

	_perfTimer += ds::getElapsedSeconds();
	if (_perfTimer > 0.25f) {
		_perfTimer -= 0.25f;
		if (_perfIndex + 1 >= 32) {
			for (int i = 1; i < 32; ++i) {
				_perfValues[i - 1] = _perfValues[i];
			}
			_perfIndex = 30;
		}
		_perfValues[_perfIndex++] = perf::avg_total();
	}
}
