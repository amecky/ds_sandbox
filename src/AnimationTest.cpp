#include "AnimationTest.h"
#include "utils\tweening.h"
#include "AABBox.h"
#include "LogPanel.h"

AnimationTest::AnimationTest(SpriteBatchBuffer* buffer) : _sprites(buffer) , _world(buffer) {
	_dialogPos = p2i(10, 950);
	_dialogState = 1;
	_player = _world.add(ds::vec2(640, 250), ds::vec4(310, 0, 30, 30));
	_arm = _world.add(ds::vec2(40, 40), ds::vec4(310, 0, 30, 30), _player);

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
	_cursorID = _world.add(ds::vec2(600, 280), ds::vec4(175, 30, 20, 20));
	_ttl = 0.4f;

	// load objects
	animation::load_animation_object("animations\\robot.txt", &_object);
	// load animations
	animation::load_text("animations\\test.txt", &_keyFrameAnimations[0]);
	animation::load_text("animations\\idle.txt", &_keyFrameAnimations[1]);
	animation::load_text("animations\\wave.txt", &_keyFrameAnimations[2]);
	animation::load_text("animations\\eyes.txt", &_keyFrameAnimations[3]);
	// build list box model
	_listModel.add("animations\\test.txt", &_keyFrameAnimations[0]);
	_listModel.add("animations\\idle.txt", &_keyFrameAnimations[1]);
	_listModel.add("animations\\wave.txt", &_keyFrameAnimations[2]);
	_listModel.add("animations\\eyes.txt", &_keyFrameAnimations[3]);

	_objectModel.add(_object);

	_animTypeSelection = 0;
	_selectedAnimationType = -1;
	_selectedFrame = 0;
	_repeat = 0;
}

AnimationTest::~AnimationTest() {
}

void showTimeline(AnimationTimeline& data) {
	int num = data.num;
	char buffer[32];
	for (int i = 0; i < num; ++i) {
		sprintf_s(buffer, "Step %d", (i + 1));
		ds::vec3 tmp = ds::vec3(data.entries[i].start, data.entries[i].value.x, data.entries[i].value.y);
		if (gui::Input(buffer, &tmp)) {
			data.entries[i].start = tmp.x;
			data.entries[i].value.x = tmp.y;
			data.entries[i].value.y = tmp.z;
		}
	}
}

void showRotationTimeline(AnimationTimeline& data) {
	int num = data.num;
	char buffer[32];
	for (int i = 0; i < num; ++i) {
		sprintf_s(buffer, "Step %d", (i + 1));
		float rot = data.entries[i].value.x / ds::TWO_PI * 360.0f;
		ds::vec2 tmp = ds::vec2(data.entries[i].start, rot);
		if (gui::Input(buffer, &tmp)) {
			data.entries[i].start = tmp.x;
			data.entries[i].value.x = tmp.y * ds::TWO_PI / 360.0f;
		}
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
	gui::ListBox("AnimationParts", _objectModel, 5);
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
				const char* LABELS[] = { "Position","Scaling","Rotation" };
				KeyFrameAnimation* kfa = _listModel.getAnimation(_listModel.getSelection());
				gui::RadioButtons(LABELS, 3, &_animTypeSelection);
				if (_animTypeSelection == 0) {
					showTimeline(kfa->translationTimeline);
				}
				if (_animTypeSelection == 1) {
					showTimeline(kfa->scalingTimeline);
				}
				if (_animTypeSelection == 2) {
					showRotationTimeline(kfa->rotationTimeline);
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
	logpanel::draw_gui(8);
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