#include "AnimationTest.h"
#include "utils\tweening.h"
#include "AABBox.h"

AnimationTest::AnimationTest(SpriteBatchBuffer* buffer) : _sprites(buffer) , _world(buffer) {
	_dialogPos = p2i(10, 950);
	_dialogState = 1;
	_player = _world.add(ds::vec2(640, 250), ds::vec4(310, 0, 30, 30));
	_arm = _world.add(ds::vec2(40, 40), ds::vec4(310, 0, 30, 30), _player);

	uint16_t body = _object.add(ds::vec2(900, 600), ds::vec4(175, 440, 102, 96));
	uint16_t head = _object.add(ds::vec2(0, 80), ds::vec4(0, 300, 215, 140),body);
	_object.add(ds::vec2(-20, 0), ds::vec4(220, 300, 72, 68), head);
	_object.add(ds::vec2(30, 0), ds::vec4(220, 370, 52, 68), head);

	_testCube = _world.add(ds::vec2(310, 300), ds::vec4(0,300,215,140));
	_world.add(ds::vec2(-20, 0), ds::vec4(220, 300, 72, 68), _testCube);
	_world.add(ds::vec2(20, 0), ds::vec4(220, 370, 52, 68), _testCube);

	MySprite& cube = _world.get(_testCube);
	cube.offset = ds::vec2(0, -20);
	_cubeAngle = 0.0f;

	_selected = INVALID_ID;
	_updateTimer = 0.0f;
	_spriteDialogPos = p2i(700, 750);
	_spriteDialogState = 1;
	_spriteAnimDialogState = 1;
	//_box = _world.add(ds::vec2(400, 280), ds::vec4(40, 100, 80, 80));
	//_world.add(ds::vec2(700, 280), ds::vec4(60, 100, 40, 80));
	//_world.add(ds::vec2(1100, 260), ds::vec4(40, 100, 80, 40));
	_cursorID = _world.add(ds::vec2(600, 280), ds::vec4(175, 30, 20, 20));
	_ttl = 0.4f;

	_keyFrameAnimations[0].addScaling(0.0f, ds::vec2(0.1f, 0.1f));
	//_keyFrameAnimation.addScaling(0.25f, ds::vec2(0.5f, 0.5f));
	_keyFrameAnimations[0].addScaling(0.5f, ds::vec2(1.5f, 1.5f));
	_keyFrameAnimations[0].addScaling(0.75f, ds::vec2(0.8f, 0.8f));
	_keyFrameAnimations[0].addScaling(1.0f, ds::vec2(1.0f, 1.0f));
	_keyFrameAnimations[0].addRotation(0.0f, 0.0f);
	_keyFrameAnimations[0].addRotation(0.5f, 0.0f);
	_keyFrameAnimations[0].addRotation(0.75f, ds::PI * 0.5f);
	_keyFrameAnimations[0].addRotation(1.0f, 0.0f);
	_keyFrameAnimations[0].addTranslation(0.0f, ds::vec2(0.0f));
	_keyFrameAnimations[0].addTranslation(0.5f, ds::vec2(0.0f,120.0f));
	_keyFrameAnimations[0].addTranslation(1.0f, ds::vec2(0.0f,0.0f));

	_keyFrameAnimations[1].addTranslation(0.0f, ds::vec2(0.0f));
	_keyFrameAnimations[1].addTranslation(0.25f, ds::vec2(0.0f,30.0f));
	_keyFrameAnimations[1].addTranslation(0.5f, ds::vec2(0.0f, 0.0f));
	_keyFrameAnimations[1].addTranslation(0.75f, ds::vec2(0.0f, 30.0f));
	_keyFrameAnimations[1].addTranslation(1.0f, ds::vec2(0.0f, 0.0f));
	_keyFrameAnimations[1].addScaling(0.0f, ds::vec2(0.0f, 0.0f));
	_keyFrameAnimations[1].addScaling(0.5f, ds::vec2(0.0f, 0.0f));
	_keyFrameAnimations[1].addScaling(0.6f, ds::vec2(0.0f, -0.2f));
	_keyFrameAnimations[1].addScaling(0.75f, ds::vec2(0.0f, 0.0f));
	_keyFrameAnimations[1].addScaling(1.0f, ds::vec2(0.0f, 0.0f));

	_keyFrameAnimations[2].addRotation(0.0f, 0.0f);
	_keyFrameAnimations[2].addRotation(0.25f, ds::PI * 0.1f);
	_keyFrameAnimations[2].addRotation(0.75f, -ds::PI *0.1f);
	_keyFrameAnimations[2].addRotation(1.0f, 0.0f);

	_keyFrameAnimations[3].addScaling(0.0f, ds::vec2(0.0f, 0.0f));
	_keyFrameAnimations[3].addScaling(0.2f, ds::vec2(0.0f, -0.6f));
	_keyFrameAnimations[3].addScaling(0.8f, ds::vec2(0.0f, -0.6f));
	_keyFrameAnimations[3].addScaling(1.0f, ds::vec2(0.0f, 0.0f));

	_listModel.add("Test", &_keyFrameAnimations[0]);
	_listModel.add("Idle", &_keyFrameAnimations[1]);
	_listModel.add("Wave", &_keyFrameAnimations[2]);
	_listModel.add("Eyes", &_keyFrameAnimations[3]);

	

	_animTypeSelection = 0;
	_selectedAnimationType = -1;
	_selectedFrame = 0;
	_repeat = 0;
}

AnimationTest::~AnimationTest() {
}

void showTimeline(AnimationTimeline& data, int selectedFrame) {
	ds::vec3 tmp = ds::vec3(data.entries[selectedFrame].start, data.entries[selectedFrame].value.x, data.entries[selectedFrame].value.y);
	if (gui::Input("Step", &tmp)) {
		data.entries[selectedFrame].start = tmp.x;
		data.entries[selectedFrame].value.x = tmp.y;
		data.entries[selectedFrame].value.y = tmp.z;
	}
}

void AnimationTest::renderGUI() {
	
	gui::start();
	p2i sp = p2i(10, 760);
	if (gui::begin("Debug", &_dialogState, &_dialogPos, 540)) {
		gui::Value("FPS", ds::getFramesPerSecond());	
		gui::Value("Mouse", ds::getMousePosition());
		gui::Value("Sprites", _world.numSprites());
		if (gui::Button("Add sprite")) {
			_world.add(ds::vec2(ds::random(100.0f, 900.0f), ds::random(100.0f, 700.0f)), ds::vec4(0, 100, 20, 20));
		}
		if (gui::Button("TestRot")) {
			_cubeAngle += ds::PI * 0.5f;
			MySprite& sp = _world.get(_testCube);
			sp.basic.rotation = _cubeAngle;
		}
	}
	if (_selected != INVALID_ID) {
		if (gui::begin("Sprite", &_spriteDialogState, 540)) {
			gui::Value("Selected", _selected);
			MySprite& sp = _world.get(_selected);
			gui::Input("Pos", &sp.position);
			gui::Input("TR", &sp.textureRect);
			gui::Input("Scale", &sp.scaling);
			float rot = sp.rotation / ds::TWO_PI * 360.0f;
			gui::Input("Rotation", &rot);
			sp.rotation = rot / 360.0f * ds::TWO_PI;
			if (gui::Button("Remove")) {
				_world.remove(_selected);
				_selected = INVALID_ID;
			}
			
		}
		if (gui::begin("Animations", &_spriteAnimDialogState, 540)) {
			gui::Input("TTL", &_ttl);
			gui::Input("Repeat", &_repeat);
			gui::ListBox("Animations", _listModel, 5);
			if (_listModel.hasSelection()) {
				//const char* LABELS[] = { "Position","Scaling","Rotation" };
				KeyFrameAnimation* kfa = _listModel.getAnimation(_listModel.getSelection());
				//gui::RadioButtons(LABELS, 3, &_animTypeSelection);
				p2i p = gui::getCurrentPosition();
				p.x += 20;
				p2i lp = p;
				lp.y -= 30;
				for (int i = 0; i < 21; ++i) {
					gui::draw_box(lp, p2i(1, 60), ds::Color(64, 64, 64, 255));
					lp.x += 20.0f;
				}
				const AnimationTimeline& positionData = kfa->translationTimeline;
				for (int i = 0; i < positionData.num; ++i) {
					gui::pushID("KeyFramePos", i);
					p2i cp = p;
					cp.x = p.x - 3.0f + 400.0f * positionData.entries[i].start;
					gui::checkItem(cp, p2i(6, 10));
					gui::draw_box(cp, p2i(6, 10), ds::Color(192, 0, 0, 255));
					if (gui::isClicked()) {
						_selectedAnimationType = 2;
						_selectedFrame = i;
					}
					gui::popID();
				}
				p.y -= 20.0f;
				const AnimationTimeline& scalingData = kfa->scalingTimeline;
				for (int i = 0; i < scalingData.num; ++i) {
					gui::pushID("KeyFrameScale", i);
					p2i cp = p;
					cp.x = p.x - 3.0f + 400.0f * scalingData.entries[i].start;
					gui::checkItem(cp, p2i(6, 10));
					gui::draw_box(cp, p2i(6, 10), ds::Color(0, 192, 0, 255));
					if (gui::isClicked()) {
						_selectedAnimationType = 0;
						_selectedFrame = i;
					}
					gui::popID();
				}
				p.y -= 20.0f;
				const AnimationTimeline& rotationData = kfa->rotationTimeline;
				for (int i = 0; i < rotationData.num; ++i) {
					gui::pushID("KeyFrameRot", i);
					p2i cp = p;
					cp.x = p.x - 3.0f + 400.0f * rotationData.entries[i].start;
					gui::checkItem(cp, p2i(6, 10));
					gui::draw_box(cp, p2i(6, 10), ds::Color(0, 0, 192, 255));
					if (gui::isClicked()) {
						_selectedAnimationType = 1;
						_selectedFrame = i;
					}
					gui::popID();
				}

				gui::moveForward(p2i(40, 80));

				
				if (_selectedAnimationType == 0) {
					showTimeline(kfa->scalingTimeline, _selectedFrame);
				}
				if (_selectedAnimationType == 1) {
					showTimeline(kfa->rotationTimeline, _selectedFrame);
				}
				if (_selectedAnimationType == 2) {
					showTimeline(kfa->translationTimeline, _selectedFrame);
				}
				if (gui::Button("Add step")) {
					kfa->addScaling(1.0f, ds::vec2(1.0f));
				}
			}
			gui::beginGroup();
			if (gui::Button("Stop all")) {
				_world.stopAll(_selected);
			}			
			if (gui::Button("Start animation")) {
				if (_listModel.hasSelection()) {
					_world.animate(_selected, _listModel.getAnimation(_listModel.getSelection()), _ttl, _repeat);
					_object.start(1, _listModel.getAnimation(_listModel.getSelection()), _ttl);
					_object.start(2, &_keyFrameAnimations[3], _ttl);
					_object.start(3, &_keyFrameAnimations[3], _ttl);
				}
			}
			gui::endGroup();
		}
	}
	gui::end();
}

void AnimationTest::render() {
	// background
	for (int y = 0; y < 4; ++y) {
		for (int x = 0; x < 5; ++x) {
			_sprites->add(ds::vec2(150 + x * 300, 120 + y * 240), ds::vec4(400, 100, 300, 240));
		}
	}

	if (_selected != INVALID_ID) {
		const MySprite& sp = _world.get(_selected);
		float sx = (sp.textureRect.z + 8.0f) / 100.0f;
		float sy = (sp.textureRect.w + 8.0f) / 100.0f;
		_sprites->add(sp.position, ds::vec4(800, 100, 100, 100), ds::vec2(sx,sy));
	}

	Event ev;
	float step = 1.0f / 60.0f;
	_updateTimer += static_cast<float>(ds::getElapsedSeconds());
	if (_updateTimer >= step) {
		_object.tick(step);
		_world.tick(step);
		_updateTimer -= step;
		while (_world.getEvent(&ev)) {
			/*
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
			*/
		}
	}

	MySprite& cursor = _world.get(_cursorID);
	cursor.basic.position = ds::getMousePosition();
	cursor.box.position = cursor.basic.position;
	
	int num = _world.findIntersections(_collisions, 256);
	for (int i = 0; i < num; ++i) {
		const Collision& c = _collisions[i];
		if (!c.containsID(_cursorID)) {
			if (c.containsID(_player)) {
				_world.stopAll(_player);
				MySprite& ps = _world.get(_player);
				ps.position.x = 100.0f;
			}
		}
	}
	
	_world.render();

	_object.render(_sprites);
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