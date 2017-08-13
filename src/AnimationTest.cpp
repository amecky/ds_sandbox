#include "AnimationTest.h"
#include "utils\tweening.h"
#include "AABBox.h"

AnimationTest::AnimationTest(SpriteBatchBuffer* buffer) : _sprites(buffer) , _world(buffer) {
	_dialogPos = p2i(10, 950);
	_dialogState = 1;
	_player = _world.add(ds::vec2(100, 250), ds::vec4(310, 0, 30, 30));
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
	_box = _world.add(ds::vec2(400, 280), ds::vec4(40, 100, 80, 80));
	_world.add(ds::vec2(700, 280), ds::vec4(60, 100, 40, 80));
	_world.add(ds::vec2(1100, 260), ds::vec4(40, 100, 80, 40));
	_cursorID = _world.add(ds::vec2(600, 280), ds::vec4(175, 30, 20, 20));
	_scalePath.add( 0.0f, ds::vec2(0.1f, 0.1f));
	_scalePath.add(0.25f, ds::vec2(0.5f, 0.5f));
	_scalePath.add( 0.5f, ds::vec2(1.5f, 1.5f));
	_scalePath.add(0.75f, ds::vec2(0.8f, 0.8f));
	_scalePath.add( 1.0f, ds::vec2(1.0f, 1.0f));
	_scalePathTTL = 0.4f;

	_keyFrameAnimation.addScaling(0.0f, ds::vec2(0.1f, 0.1f));
	//_keyFrameAnimation.addScaling(0.25f, ds::vec2(0.5f, 0.5f));
	_keyFrameAnimation.addScaling(0.5f, ds::vec2(1.5f, 1.5f));
	_keyFrameAnimation.addScaling(0.75f, ds::vec2(0.8f, 0.8f));
	_keyFrameAnimation.addScaling(1.0f, ds::vec2(1.0f, 1.0f));
	_keyFrameAnimation.addRotation(0.0f, 0.0f);
	_keyFrameAnimation.addRotation(0.5f, 0.0f);
	_keyFrameAnimation.addRotation(0.75f, ds::PI * 0.5f);
	_keyFrameAnimation.addRotation(1.0f, 0.0f);
	_keyFrameAnimation.addTranslation(0.0f, ds::vec2(0.0f));
	_keyFrameAnimation.addTranslation(0.5f, ds::vec2(0.0f,120.0f));
	_keyFrameAnimation.addTranslation(1.0f, ds::vec2(0.0f,-120.0f));
	_keyFrameAnimation.setTTL(2.0f);
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
			gui::Input("Scale Path TTL", &_scalePathTTL);
			
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
			if (gui::Button("Wiggle")) {
				_world.wiggle(_selected, ds::PI*0.25f, 2.0f);
			}
			if (gui::Button("Follow")) {
				_world.follow(_selected, _cursorID, 200.0f);
			}
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
			if (gui::Button("ScaleByPath")) {
				//_world.scaleByPath(_selected, &_scalePath, _scalePathTTL);
				_world.animate(_selected, &_keyFrameAnimation);
			}
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
				//_world.rotateBy(_selected, ds::PI * 0.5f, _squeezeTTL);
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

	MySprite& cursor = _world.get(_cursorID);
	cursor.position = ds::getMousePosition();
	
	int num = _world.findIntersections(_collisions, 256);
	for (int i = 0; i < num; ++i) {
		const Collision& c = _collisions[i];
		if (!c.containsID(_cursorID)) {
			if (c.containsID(_player)) {
				_world.stopAll(_player);
				MySprite& ps = _world.get(_player);
				ps.position.x = 100.0f;
				_jumping = false;
			}
		}
	}
	
	_world.render();
}

void AnimationTest::onButtonClicked(int button) {
	if (button == 1) {
		int num = _world.findIntersections(_collisions, 256);
		for (int i = 0; i < num; ++i) {
			const Collision& c = _collisions[i];
			if (c.containsID(_cursorID)) {
				if (c.firstID == _cursorID) {
					_selected = c.secondID;
				}
				else {
					_selected = c.firstID;
				}
			}
		}
	}
}