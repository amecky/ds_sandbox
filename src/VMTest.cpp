#include "VMTest.h"
#include "utils\tweening.h"
#include "AABBox.h"
#include <Windows.h>

static vm::Variable variables[] = {
	{ vm::VT_CONSTANT, "PI", 3.141592654f },
	{ vm::VT_CONSTANT, "TWO_PI", 2.0f * 3.141592654f },
	{ vm::VT_VARIABLE, "DT", 1.0f }
};

VMTest::VMTest(SpriteBatchBuffer* buffer) : _sprites(buffer) {

	
	_vmCtx.variables = variables;
	_vmCtx.num_variables = 3;
	_numTokens = 0;
	_timer = 0.0f;
	_dialogPos = p2i(10, 950);
	_dialogState = 1;
	_timer = 0.0f;
	_ttl = 1.0f;
	_startPos = ds::vec2(200, 384);
	_endPos = ds::vec2(700, 384);
	_showDiagram = true;
	_sprite = { ds::vec2(512,384),ds::vec4(0,100,10,10),ds::vec2(1.0f),0.0f,ds::Color(255,255,255,255) };
	//sprintf(_source,"3 * 4 + sin(2 * TWO_PI * DT) * 10");
	//sprintf(_source, "ramp(0.5,DT) * 5");
	sprintf(_source, "range(0.0,0.4,DT) * lerp(0.0,2.0,DT/0.4) + range(0.4,0.8,DT) * 2 + range(0.8,1.0,DT) * lerp(2.0,0.0,(DT-0.8)/0.2)");
	_result = 0.0f;
}

VMTest::~VMTest() {
}

void VMTest::renderGUI() {
	
	gui::start();
	p2i sp = p2i(10, 760);
	if (gui::begin("Debug", &_dialogState, &_dialogPos, 540)) {
		gui::Value("FPS", ds::getFramesPerSecond());
		if (gui::Input("Source", _source, 128)) {
			_numTokens = vm::parse(_source, _vmCtx, _byteCode, 256);
			int quantification = 64;
			float step = 1.0f / static_cast<float>(quantification);
			float t = 0.0f;
			_min = FLT_MAX;
			_max = 0.0f;
			for (int i = 0; i < quantification + 1; ++i) {
				_vmCtx.variables[2].value = t;
				_values[i] = vm::run(_byteCode, _numTokens, _vmCtx);
				if (_values[i] < _min) {
					_min = _values[i];
				}
				if (_values[i] > _max) {
					_max = _values[i];
				}
				t += step;
			}
		}
		gui::Value("Num", _numTokens);
		if (gui::Button("Reset timer")) {
			_timer = 0.0f;
		}
		if (gui::Button("Compile")) {
			_numTokens = vm::parse(_source,_vmCtx, _byteCode, 256);
		}
		if (gui::Button("RUN")) {
			_result = vm::run(_byteCode, _numTokens, _vmCtx);
		}
		gui::Value("Result", _result);
		if (_numTokens > 0) {
			int quantification = 64;
			p2i p = gui::getCurrentPosition();
			p2i c = p;
			p2i tp = p;
			tp.y -= 50;
			//tp.x += 100;
			gui::draw_box(tp, p2i(330, 100), ds::Color(32, 32, 32, 255));
			tp = p;
			gui::draw_box(tp, p2i(330, 1), ds::Color(192, 192, 192, 255));
			tp.x += 350.0f;			
			char buffer[32];
			sprintf(buffer, "max %g", _max);
			gui::draw_text(tp, buffer);
			tp.y -= 50;
			tp.x = p.x;
			gui::draw_box(tp, p2i(330, 1), ds::Color(192, 192, 192, 255));
			tp.y -= 50;
			tp.x = p.x;
			gui::draw_box(tp, p2i(330, 1), ds::Color(192, 192, 192, 255));
			tp.x += 350;
			sprintf(buffer, "min %g", _min);
			gui::draw_text(tp, buffer);
			for (int i = 0; i < quantification + 1; ++i) {
				float y = _values[i];
				c.x += 5;
				c.y = p.y - (_max - y) / (_max - _min) * 100;
				gui::draw_box(c, p2i(5, 5), ds::Color(192, 192, 192, 255));
			}
			gui::moveForward(p2i(300, 140));
		}
	}
	gui::end();
}

void VMTest::render() {
	if (_numTokens > 0) {
		_vmCtx.variables[2].value = _timer;
		_sprite.position.y = 400.0f + vm::run(_byteCode, _numTokens, _vmCtx);
		_sprites->add(_sprite);
		_timer += ds::getElapsedSeconds();
		if (_timer >= 1.0f) {
			_timer -= 1.0f;
		}
	}
}
