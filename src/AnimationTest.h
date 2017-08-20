#pragma once
#include <diesel.h>
#include <ds_imgui.h>
#include <SpriteBatchBuffer.h>
#include "DataArray.h"
#include "World.h"
#include "KeyFrameAnimation.h"
#include <vector>

class AnimationListBoxModel : public gui::ListBoxModel {

	struct InternalData {
		const char* name;
		const char* fileName;
		KeyFrameAnimation* data;
	};

public:
	AnimationListBoxModel() {}
	virtual ~AnimationListBoxModel() {}
	void add(const char* name, const char* fileName,KeyFrameAnimation* anim) {
		_data.push_back({ name, fileName, anim });
	}
	void add(const char* fileName, KeyFrameAnimation* anim) {
		_data.push_back({ anim->name, fileName, anim });
	}
	uint32_t size() const {
		return _data.size();
	}
	const char* name(uint32_t index) const {
		return _data[index].name;
	}
	const char* fileName(uint32_t index) const {
		return _data[index].fileName;
	}
	KeyFrameAnimation* getAnimation(uint32_t index) {
		return _data[index].data;
	}
private:
	std::vector<InternalData> _data;
};

class AnimationObjectListBoxModel : public gui::ListBoxModel {

	struct InternalData {
		const char* name;
		uint16_t id;
	};

public:
	AnimationObjectListBoxModel() {}
	virtual ~AnimationObjectListBoxModel() {}
	void add(const char* name, uint16_t id) {
		_data.push_back({ name, id });
	}
	void add(const AnimationObject& object) {
		_data.clear();
		for (int i = 0; i < object.numParts(); ++i) {
			const AnimationPart& part = object.getPart(i);
			_data.push_back({ part.name, part.id });
		}
	}
	uint32_t size() const {
		return _data.size();
	}
	const char* name(uint32_t index) const {
		return _data[index].name;
	}
	uint16_t getID(uint32_t index) {
		return _data[index].id;
	}
	void clear() {
		_data.clear();
	}
private:
	std::vector<InternalData> _data;
};

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
	float _ttl;
	World _world;
	float _updateTimer;
	int _spriteDialogState;
	p2i _spriteDialogPos;
	int _spriteAnimDialogState;
	ID _player;
	ID _arm;
	ID _box;
	ID _cursorID;
	ID _testCube;
	float _cubeAngle;
	int _repeat;
	Collision _collisions[256];
	KeyFrameAnimation _keyFrameAnimations[16];
	int _selectedAnimationType;
	int _selectedFrame;
	AnimationListBoxModel _listModel;
	int _animTypeSelection;

	AnimationObject _object;
	AnimationObjectListBoxModel _objectModel;
};

