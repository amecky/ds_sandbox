#pragma once
#include <diesel.h>
#include <ds_imgui.h>

class SpriteBatchBuffer;

class TweeningListModel : public gui::ListBoxModel {

public:
	virtual uint32_t size() const;
	virtual const char* name(uint32_t index) const;
};

class TweeningTest {

public:
	TweeningTest(SpriteBatchBuffer* buffer);
	~TweeningTest();
	void render();
	void renderGUI();
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
};

