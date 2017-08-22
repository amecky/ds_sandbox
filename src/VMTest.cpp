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

	_dialogPos = p2i(10, 950);
	_dialogState = 1;
	_timer = 0.0f;
	_ttl = 1.0f;
	_startPos = ds::vec2(200, 384);
	_endPos = ds::vec2(700, 384);
	_showDiagram = true;
	_sprite = { ds::vec2(512,384),ds::vec4(0,100,10,10),ds::vec2(1.0f),0.0f,ds::Color(255,255,255,255) };
	sprintf(_source,"3 * 4 + sin(2 * TWO_PI * DT) * 10");
	_result = 0.0f;
}

VMTest::~VMTest() {
}

void VMTest::renderGUI() {
	
	gui::start();
	p2i sp = p2i(10, 760);
	if (gui::begin("Debug", &_dialogState, &_dialogPos, 540)) {
		gui::Value("FPS", ds::getFramesPerSecond());
		gui::Input("Source", _source, 128);
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
			float step = 1.0f / 32.0f;
			float t = 0.0f;
			p2i p = gui::getCurrentPosition();
			p2i c = p;
			float min = 0.0f;
			float max = 0.0f;
			for (int i = 0; i < 32; ++i) {
				_vmCtx.variables[2].value = t;
				float y = vm::run(_byteCode, _numTokens, _vmCtx);
				if (y < min) {
					min = y;
				}
				if (y > max) {
					max = y;
				}
				t += step;
			}
			t = 0.0f;
			p2i tp = p;
			tp.y -= 10;
			gui::draw_box(tp, p2i(320, 1), ds::Color(192, 192, 192, 255));
			tp.x += 350.0f;			
			char buffer[32];
			sprintf(buffer, "max %g", max);
			gui::draw_text(tp, buffer);
			tp.y -= 100;
			tp.x = p.x;
			gui::draw_box(tp, p2i(320, 1), ds::Color(192, 192, 192, 255));
			tp.x += 350;
			sprintf(buffer, "min %g", min);
			gui::draw_text(tp, buffer);
			for (int i = 0; i < 32; ++i) {
				int x = i * 10;
				_vmCtx.variables[2].value = t;
				float y = vm::run(_byteCode, _numTokens, _vmCtx);
				c.x += 10;
				c.y = p.y - (y - min) / (max - min) * 100;
				gui::draw_box(c, p2i(5, 5), ds::Color(192, 192, 192, 255));
				t += step;
			}
			gui::moveForward(p2i(300, 100));
		}
	}
	gui::end();
}

void VMTest::render() {
	if (_numTokens > 0) {
		float step = 1.0f / 32.0f;
		float t = 0.0f;
		for (int i = 0; i < 32; ++i) {
			_sprite.position.x = 100.0f + i * 10.0f;
			_vmCtx.variables[2].value = t;
			_sprite.position.y = 400.0f + vm::run(_byteCode, _numTokens, _vmCtx);
			_sprites->add(_sprite);
			t += step;
		}
	}
	//_sprite.position = tweening::interpolate(t, _startPos, _endPos, _timer, _ttl);
	//_sprite.rotation = tweening::interpolate(t, ds::PI * 0.5f, 0.0f, _timer, _ttl);
	//_sprite.scaling.y = tweening::interpolate(t, 0.7f, 1.0f, _timer, _ttl);
	//_sprites->add(_sprite);
}
