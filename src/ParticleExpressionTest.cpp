#include "ParticleExpressionTest.h"
#include "utils\tweening.h"
#include "AABBox.h"
#include <Windows.h>
#include <fstream>
#include <string>
#include "LogPanel.h"

ParticleExpressionTest::ParticleExpressionTest(SpriteBatchBuffer* buffer) : _sprites(buffer) {

	_dialogPos = p2i(10, 950);
	_dialogState = 1;

	_numParticles = 64;
	_showParticleInfo = false;
	_selectedType = 0;
	_selectedExpression = -1;

	_perfIndex = 0;
	_perfMax = 0;
	_perfTimer = 0.0f;

	_system = new Particlesystem("RingExplosion", ds::vec4(200, 0, 4, 4),4096);
	loadExpressionsFile(_system);

}

ParticleExpressionTest::~ParticleExpressionTest() {
	delete _system;
}

void ParticleExpressionTest::loadExpressionsFile(Particlesystem* system) {
	char buffer[256];
	sprintf(buffer, "particles\\%s.txt", system->name);
	std::ifstream ifs(buffer);
	std::string line;
	uint16_t cnt = 0;
	while (std::getline(ifs, line)) {
		if (line.empty() || line.find("#") != std::string::npos) {
			continue;
		}
		if (line == "-") {
			++cnt;
			continue;
		}
		LOG_DEBUG("line %d = '%s'", cnt, line.c_str());
		ParticleExpression& exp = system->expressions[cnt];
		snprintf(exp.source, 256, "%s", line.c_str());
		vm::parse(exp.source, system->vmCtx, exp.expression);
		int cid = cnt;
		if (cid > NUM_CHANNELS) {
			cid -= NUM_CHANNELS;
		}
		system->connections[cnt] = { ALL_CHANNELS[cid],&exp.expression, cnt };
		++cnt;
	}
}

void ParticleExpressionTest::renderExpressions(Particlesystem* system) {
	perf::ZoneTracker("renderExpressions");
	p2i p = gui::getCurrentPosition();
	p2i t = p;
	int s = 0;
	int e = NUM_CHANNELS;
	int d = 0;
	if (_selectedType == 1) {
		s += NUM_CHANNELS;
		e += NUM_CHANNELS;
		d = NUM_CHANNELS;
	}
	for (uint16_t i = s; i < e; ++i) {
		gui::draw_text(t, CHANNEL_NAMES[i - d]);
		t.x += 100;
		const ChannelConnection& cc = system->connections[i];
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
			gui::pushID("-",i);
			gui::checkItem(t, p2i(20, 16));
			if (gui::isClicked()) {
				system->connections[i].expression = 0;
			}
			gui::draw_box(t, p2i(20, 16), gui::getSettings().buttonColor);
			gui::draw_text(t, "-");
			gui::popID();
			gui::popID();
		}
		else {
			t.x += 80;
			gui::pushID("-", i);
			gui::checkItem(t, p2i(20, 16));
			if (gui::isClicked()) {
				ParticleExpression& exp = system->expressions[i];
				snprintf(exp.source, 256, "1");
				vm::parse(exp.source, system->vmCtx, exp.expression);
				system->connections[i] = { ALL_CHANNELS[i],&exp.expression, i };
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
		ChannelConnection& cc = system->connections[_selectedExpression];
		ParticleExpression& pexp = system->expressions[cc.id];
		if (gui::Input("Source", pexp.source, 256)) {
			vm::parse(pexp.source, system->vmCtx, pexp.expression);
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
		gui::Value("Particles", _system->particles.num);
		gui::Input("Num", &_numParticles);
		if (gui::Button("Start")) {
			float px = ds::random(300, 600);
			float py = ds::random(200, 400);
			_system->emitt(_numParticles, ds::vec2(px,py));
		}
		static const char* RButtons[] = {"Emitter","Motion"};
		gui::RadioButtons(RButtons, 2, &_selectedType);
		renderExpressions(_system);
		gui::Checkbox("Show Info", &_showParticleInfo);
		if (_showParticleInfo) {
			if (_system->particles.num > 0) {
				for (int i = 0; i < NUM_CHANNELS; ++i) {
					gui::Value(CHANNEL_NAMES[i], _system->particles.get_value(ALL_CHANNELS[i], 0));
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
// render
// ----------------------------------------------------
void ParticleExpressionTest::render() {

	float dt = ds::getElapsedSeconds();

	_system->manageLifecycles(dt);
	
	_system->moveParticles(dt);

	_system->updateParticles(dt);

	_system->render(_sprites);
	
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
