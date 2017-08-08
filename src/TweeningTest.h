#pragma once
#include <diesel.h>
#include <ds_imgui.h>
#include <SpriteBatchBuffer.h>
#include "DataArray.h"

class TweeningListModel : public gui::ListBoxModel {

public:
	virtual uint32_t size() const;
	virtual const char* name(uint32_t index) const;
};

struct TweeningData {
	ds::vec2 start;
	ds::vec2 end;
	float timer;
	float ttl;
	float startScale;
	float endScale;
	bool scaling;
};

struct MySprite {
	ID id;
	ds::vec2 position;
	ds::vec4 textureRect;
	ds::vec2 scale;
};

class TweeningTest {

public:
	TweeningTest(SpriteBatchBuffer* buffer);
	~TweeningTest();
	void render();
	void renderGUI();
	void onButtonClicked(int button);
private:
	SpriteBatchBuffer* _sprites;
	ds::vec2 _startPos;
	ds::vec2 _endPos;
	float _timer;
	float _ttl;
	int _dialogState;
	p2i _dialogPos;
	TweeningListModel _model;
	bool _showDiagram;
	Sprite _sprite;
	DataArray<MySprite, 128> _spriteArray;
	ID _selected;
};

