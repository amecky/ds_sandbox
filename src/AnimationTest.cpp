#include "AnimationTest.h"
#include "utils\tweening.h"
#include "AABBox.h"

AnimationTest::AnimationTest(SpriteBatchBuffer* buffer) : _sprites(buffer) , _world(buffer) {
	_dialogPos = p2i(10, 750);
	_dialogState = 1;
	_player = _world.add(ds::vec2(512, 250), ds::vec4(0, 100, 20, 20));
	_selected = INVALID_ID;
	_rotationTTL = 1.0f;
	_updateTimer = 0.0f;
	_jumpTTL = 1.0f;
	_height = 200.0f;
	_squeezeAmplitude = ds::vec2(1.0f, 0.4f);
	_squeezeTTL = 0.1f;
	_velocity = ds::vec2(200, 0);
	_spriteDialogPos = p2i(700, 750);
	_spriteDialogState = 1;
	_spriteAnimDialogState = 1;
	_jumping = false;
}

AnimationTest::~AnimationTest() {
}

void AnimationTest::renderGUI() {
	
	gui::start();
	p2i sp = p2i(10, 760);
	if (gui::begin("Debug", &_dialogState, &_dialogPos, 540)) {
		gui::Value("FPS", ds::getFramesPerSecond());		
		gui::Value("Sprites", _world.numSprites());
		if (gui::Button("Add sprite")) {
			_world.add(ds::vec2(ds::random(100.0f, 900.0f), ds::random(100.0f, 700.0f)), ds::vec4(0, 100, 20, 20));
		}
	}
	if (_selected != INVALID_ID) {
		if (gui::begin("Sprite", &_spriteDialogState, 540)) {
			gui::Value("Selected", _selected);
			MySprite& sp = _world.get(_selected);
			gui::Input("Pos", &sp.position);
			gui::Input("TR", &sp.textureRect);
			gui::Input("Scale", &sp.scaling);
			if (gui::Button("Remove")) {
				_world.remove(_selected);
				_selected = INVALID_ID;
			}
			gui::Input("Rotation-TTL", &_rotationTTL);
			gui::Input("Jump TTL", &_jumpTTL);
			gui::Input("Height", &_height);
			gui::Input("Squeeze TTL", &_squeezeTTL);
			gui::Input("Squeeze Amp", &_squeezeAmplitude);
			gui::Input("Velocity", &_velocity);
			
		}
		if (gui::begin("Animations", &_spriteAnimDialogState, 540)) {
			gui::beginGroup();
			if (gui::Button("Rotate")) {
				_world.rotateBy(_selected, -ds::PI * 0.5f, _rotationTTL);
			}
			if (gui::Button("Jump")) {
				if (!_jumping) {
					_jumping = true;
					_world.jump(_selected, _height, _jumpTTL);
				}
			}
			if (gui::Button("Squeeze")) {
				_world.squeeze(_selected, _squeezeAmplitude, _squeezeTTL);
			}
			if (gui::Button("Move")) {
				_world.moveBy(_selected, _velocity);
			}
			gui::endGroup();
			gui::beginGroup();
			if (gui::Button("Both")) {
				if (!_jumping) {
					_jumping = true;
					_world.rotateBy(_selected, -ds::PI * 0.5f, _rotationTTL);
					_world.jump(_selected, _height, _jumpTTL);
				}
			}
			if (gui::Button("Stop all")) {
				_world.stopAll(_selected);
			}
			gui::endGroup();
		}
	}
	gui::end();
}

void AnimationTest::render() {
	Event ev;
	float step = 1.0f / 60.0f;
	_updateTimer += static_cast<float>(ds::getElapsedSeconds());
	if (_updateTimer >= step) {
		_world.tick(step);
		_updateTimer -= step;
		while (_world.getEvent(&ev)) {
			if (ev.type == AnimationType::ANIM_ROTATION && ev.sprite == _player) {
				MySprite& sp = _world.get(ev.sprite);
				sp.rotation = 0.0f;
				_world.squeeze(_selected, _squeezeAmplitude, _squeezeTTL);
				_jumping = false;
			}
			else if (ev.type == AnimationType::ANIM_MOVE_BY && ev.sprite == _player) {
				_world.stopAll(ev.sprite);
				_jumping = false;
				MySprite& sp = _world.get(ev.sprite);
				sp.position.x = 20.0f;
				_world.moveBy(ev.sprite, _velocity);
			}
		}
	}

	_world.render();
}

void AnimationTest::onButtonClicked(int button) {
	if (button == 1) {
		AABBox box(ds::getMousePosition(), 20.0f);
		_selected = _world.findIntersection(box);		
	}
}