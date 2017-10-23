#pragma once
#include "..\TestSpriteApp.h"
#include <ds_imgui.h>
#include "..\DataArray.h"
#include "..\World.h"

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

class TweeningTest : public TestSpriteApp {

public:
	TweeningTest();
	~TweeningTest();
	ds::RenderSettings getRenderSettings();
	bool usesGUI() {
		return true;
	}
	bool init();
	void tick(float dt);
	void render();
	void renderGUI();
private:
	ds::vec2 _startPos;
	ds::vec2 _endPos;
	float _timer;
	float _ttl;
	int _dialogState;
	p2i _dialogPos;
	TweeningListModel _model;
	bool _showDiagram;
	Sprite _sprite;
};

