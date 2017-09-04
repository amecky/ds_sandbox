#include "ParticleExpressionTest.h"
#include "utils\tweening.h"
#include "AABBox.h"
#include <Windows.h>
#include <fstream>
#include <string>
#include "PerfPanel.h"
#include "plugins\LogPanelPlugin.h"

ParticleExpressionTest::ParticleExpressionTest(const ds_api_registry& registry, SpriteBatchBuffer* buffer) : _sprites(buffer) {

	//_logPanelPlugin = (log_panel_plugin*)registry.get(LOG_PANEL_PLUGIN_NAME);

	_dialogPos = p2i(10, 950);
	_dialogState = 1;

	_numParticles = 64;
	_showParticleInfo = false;
	_showExpressions = false;
	_selectedType = 0;
	_selectedExpression = -1;
	_emissionTime = 2.0f;
	_system = new Particlesystem("new_system", ds::vec4(200, 0, 20, 20), 4096);
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
		int bl = 0;
		const char* p = line.c_str();
		while (*p != '=') {
			buffer[bl++] = *p;
			++p;
		}
		if (buffer[bl - 1] == ' ') {
			--bl;
		}
		buffer[bl] = '\0';
		++p;
		while (*p == ' ') {
			++p;
		}
		int len = strlen(p);
		if (len > 0) {
			ParticleExpression& exp = system->expressions[cnt];
			int l = 0;
			while (*p) {
				if (l < 255) {
					exp.source[l++] = *p;
					++p;
				}
			}
			exp.source[l] = '\0';
			ParticleChannel pc = find_by_name(buffer);
			LOG_DEBUG("line %d = '%s' : '%s'", cnt, buffer, exp.source);
			if (pc != ParticleChannel::UNKNOWN) {				
				vm::parse(exp.source, system->vmCtx, exp.expression);
				system->connections[cnt] = { pc, &exp.expression, cnt };
			}
		}
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
		t.x += 150;
		const ChannelConnection& cc = system->connections[i];
		if (cc.expression != 0) {
			uint16_t id = cc.id;
			gui::pushID(CHANNEL_NAMES[i - d],i);
			gui::checkItem(t, p2i(60, 16));
			if (gui::isClicked()) {
				_selectedExpression = i;
			}
			gui::draw_box(t, p2i(60, 16), gui::getSettings().buttonColor);
			gui::draw_text(t, "edit");
			t.x += 100;			
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
			t.x += 100;
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
		gui::Input("TTL", &_emissionTime);
		if (gui::Button("Start")) {
			float px = ds::random(500, 700);
			float py = ds::random(200, 400);
			_system->emitt(_numParticles, ds::vec2(px,py),_emissionTime);
		}
		gui::Checkbox("Show Expressions", &_showExpressions);
		if (_showExpressions) {
			renderExpressions(_system);
		}
		gui::Checkbox("Show Info", &_showParticleInfo);
		if (_showParticleInfo) {
			if (_system->particles.num > 0) {
				for (int i = 0; i < NUM_CHANNELS; ++i) {
					gui::Value(CHANNEL_NAMES[i], _system->particles.get_value(ALL_CHANNELS[i], 0),4,5);
				}
			}
		}
	}
	
	perfpanel::show_profiler();
	//_logPanelPlugin->draw_gui(8);
	gui::end();
	
}

// ----------------------------------------------------
// render
// ----------------------------------------------------
void ParticleExpressionTest::render() {

	float dt = ds::getElapsedSeconds();

	_system->update(dt);

	_system->render(_sprites);
	
	perfpanel::tick_histogram(dt);
	
}
