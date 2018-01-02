#include "VMTest.h"
#include "..\utils\tweening.h"
#include "..\AABBox.h"
#include <Windows.h>
#include <fstream>
#include <string>

VMTest::VMTest() : TestSpriteApp() {
}

bool VMTest::init() {
	TestSpriteApp::init();
	_vmCtx.add_constant("PI", 3.141592654f);
	_vmCtx.add_constant("TWO_PI", 2.0f * 3.141592654f);
	_vmCtx.add_variable("DT", 1.0f);
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
	sprintf(_source,"3 * 4 + sin(2 * TWO_PI * DT) * 10");
	//sprintf(_source, "ramp(0.5,DT) * 5");
	//sprintf(_source, "range(0.0,0.4,DT) * lerp(0.0,2.0,DT/0.4) + range(0.4,0.8,DT) * 2 + range(0.8,1.0,DT) * lerp(2.0,0.0,(DT-0.8)/0.2)");


	// read expressions
	std::ifstream ifs("expressions.txt");
	std::string line;
	while (std::getline(ifs, line)) {
		// skip empty lines:
		if (line.empty())
			continue;

		TestExpression exp;
		snprintf(exp.name, 16, "%s", line.c_str());
		snprintf(exp.source, 256, "%s", line.c_str());
		exp.numTokens = 0;
		_listModel.add(exp);
	}


	_result = 0.0f;

	return true;
}

VMTest::~VMTest() {
}

void VMTest::compile(const char* source) {
	_numTokens = vm::parse(source, _vmCtx, _byteCode, 256);
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

void VMTest::renderGUI() {
	p2i sp = p2i(10, 760);
	gui::start(&_dialogPos, 540);	
	if (gui::begin("Debug", &_dialogState)) {
		gui::Value("FPS", ds::getFramesPerSecond());
		gui::ListBox("Source", _listModel, 5);
		if (_listModel.hasSelection()) {
			TestExpression& exp = _listModel.getExpression(_listModel.getSelection());
			if (gui::Input("NSource", exp.source, 128)) {
				compile(exp.source);
			}
		}
		if (gui::Input("Source", _source, 128)) {
			compile(_source);
		}
		gui::Value("Num", _numTokens);
		if (gui::Button("Reset timer")) {
			_timer = 0.0f;
		}
		if (gui::Button("Compile")) {
			compile(_source);
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
	_spriteBuffer->begin();
	if (_numTokens > 0) {
		_vmCtx.variables[2].value = _timer;
		_sprite.position.y = 400.0f + vm::run(_byteCode, _numTokens, _vmCtx);
		_spriteBuffer->add(_sprite);				
	}
	_spriteBuffer->flush();
}

ds::RenderSettings VMTest::getRenderSettings() {
	ds::RenderSettings rs;
	rs.width = 1024;
	rs.height = 768;
	rs.title = "ds_sandbox- VM Test";
	rs.clearColor = ds::Color(0.1f, 0.1f, 0.1f, 1.0f);
	rs.multisampling = 4;
	return rs;
}

void VMTest::tick(float dt) {
	_timer += dt;
	if (_timer >= 1.0f) {
		_timer -= 1.0f;
	}
}

