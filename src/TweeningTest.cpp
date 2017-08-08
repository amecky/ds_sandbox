#include "TweeningTest.h"
#include "utils\tweening.h"
#include "AABBox.h"

uint32_t TweeningListModel::size() const {
	return 32;
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
	_showDiagram = true;

	_sprite = { ds::vec2(512,384),ds::vec4(0,100,20,20),ds::vec2(1.0f),0.0f,ds::Color(255,255,255,255) };

	ID one = _spriteArray.add({ INVALID_ID,ds::vec2(512,384),ds::vec4(0,100,20,20),ds::vec2(1.0f)});
	_selected = INVALID_ID;
}

TweeningTest::~TweeningTest() {
}

void TweeningTest::renderGUI() {
	
	gui::start();
	p2i sp = p2i(10, 760);
	if (gui::begin("Debug", &_dialogState, &_dialogPos, 380)) {
		gui::Value("FPS", ds::getFramesPerSecond());
		gui::Input("Start", &_startPos);
		gui::Input("End", &_endPos);
		gui::Input("TTL", &_ttl);
		gui::ListBox("Type", _model, 5);
		gui::Checkbox("Diagram", &_showDiagram);
		if (gui::Button("Reset timer")) {
			_timer = 0.0f;
		}
		gui::Value("Sprites", _spriteArray.numObjects);
		if (gui::Button("Add sprite")) {
			_spriteArray.add({ INVALID_ID,ds::vec2(ds::random(100.0f,900.0f),ds::random(100.0f,700.0f)),ds::vec4(0,100,20,20),ds::vec2(1.0f) });
		}
		if (_selected != INVALID_ID) {
			gui::begin("Sprite",380);
			gui::Value("Selected", _selected);
			MySprite& sp = _spriteArray.get(_selected);
			gui::Input("Pos", &sp.position);
			gui::Input("TR", &sp.textureRect);
			gui::Input("Scale", &sp.scale);
			if (gui::Button("Remove")) {
				_spriteArray.remove(_selected);
				_selected = INVALID_ID;
			}
		}
	}
	gui::end();
}

void TweeningTest::render() {
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
	_sprite.position = tweening::interpolate(t, _startPos, _endPos, _timer, _ttl);
	_sprite.scaling.y = tweening::interpolate(t, 0.7f, 1.0f, _timer, _ttl);
	_sprites->add(_sprite);

	for (uint16_t i = 0; i < _spriteArray.numObjects; ++i) {
		const MySprite& sp = _spriteArray.objects[i];
		_sprites->add(sp.position, sp.textureRect, sp.scale);
	}

	if (_showDiagram) {
		ds::vec2 p;
		_sprites->add(ds::vec2(520, 100), ds::vec4(0, 70, 640, 4));
		_sprites->add(ds::vec2(520, 300), ds::vec4(0, 70, 640, 4));
		int steps = 64;
		for (int i = 0; i < steps; ++i) {
			float time = static_cast<float>(i) / static_cast<float>(steps - 1) * _ttl;
			float y = tweening::interpolate(t, 100.0f, 300.0f, time, _ttl);
			p.x = 200 + i * 10;
			p.y = y;
			_sprites->add(p, ds::vec4(165, 0, 20, 20));
		}
	}
}

void TweeningTest::onButtonClicked(int button) {
	if (button == 1) {
		AABBox box(ds::getMousePosition(), 20.0f);
		_selected = INVALID_ID;
		for (uint16_t i = 0; i < _spriteArray.numObjects; ++i) {
			const MySprite& sp = _spriteArray.objects[i];
			if (box.isInside(sp.position)) {
				_selected = sp.id;
			}
		}
	}
}