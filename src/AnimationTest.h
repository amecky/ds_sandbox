#pragma once
#include <diesel.h>
#include <ds_imgui.h>
#include <SpriteBatchBuffer.h>
#include "DataArray.h"
#include "World.h"
#include "KeyFrameAnimation.h"

class AnimationTest {

public:
	AnimationTest(SpriteBatchBuffer* buffer);
	~AnimationTest();
	void render();
	void renderGUI();
	void onButtonClicked(int button);

private:
	SpriteBatchBuffer* _sprites;
	int _dialogState;
	p2i _dialogPos;	
	ID _selected;
	World _world;
	float _rotationTTL;
	float _height;
	float _jumpTTL;
	float _squeezeTTL;
	ds::vec2 _squeezeAmplitude;
	float _updateTimer;
	ds::vec2 _velocity;
	int _spriteDialogState;
	p2i _spriteDialogPos;
	int _spriteAnimDialogState;
	ID _player;
	bool _jumping;
	ID _box;
	ID _cursorID;
	Collision _collisions[256];
	float _scalePathTTL;
	ds::Vec2Path _scalePath;
	KeyFrameAnimation _keyFrameAnimation;
};

