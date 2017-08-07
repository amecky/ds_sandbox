#include "TweeningTest.h"
#include <SpriteBatchBuffer.h>
#include "utils\tweening.h"

uint32_t TweeningListModel::size() const {
	return 18;
}

const char* TweeningListModel::name(uint32_t index) const {
	return tweening::DESCRIPTORS[index].name;
}

TweeningTest::TweeningTest(SpriteBatchBuffer* buffer) : _sprites(buffer) {
	_dialogPos = p2i(10, 750);
	_dialogState = 1;
	_timer = 0.0f;
	_ttl = 1.0f;
	_startPos = ds::vec2(200, 384);
	_endPos = ds::vec2(700, 384);
}

TweeningTest::~TweeningTest() {
}

void TweeningTest::renderGUI() {
	
	gui::start();
	p2i sp = p2i(10, 760);
	if (gui::begin("Debug", &_dialogState, &_dialogPos, 300)) {
		gui::Value("FPS", ds::getFramesPerSecond());
		gui::Input("Start", &_startPos);
		gui::Input("End", &_endPos);
		gui::Input("TTL", &_ttl);
		gui::ListBox("Type", _model, 5);
		if (gui::Button("Reset timer")) {
			_timer = 0.0f;
		}
	}
	gui::end();
}

void TweeningTest::render() {
	ds::vec2 p = _endPos;
	tweening::TweeningType t = tweening::linear;
	if (_model.hasSelection()) {
		t = tweening::DESCRIPTORS[_model.getSelection()].type;
	}
	if (_timer < _ttl) {
		
		_timer += static_cast<float>(ds::getElapsedSeconds());
	}
	else {
		_timer = _ttl;
	}
	p = tweening::interpolate(t, _startPos, _endPos, _timer, _ttl);
	_sprites->add(p, ds::vec4(0, 0, 40, 42));
}