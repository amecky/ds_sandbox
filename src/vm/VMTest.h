#pragma once
#include "..\TestSpriteApp.h"
#include "..\DataArray.h"
#include "..\World.h"
#include <ds_vm.h>
#include <ds_imgui.h>

struct TestExpression {

	char name[32];
	char source[256];
	vm::Token byteCode[256];
	uint16_t numTokens;
};

class ExpressionListBoxModel : public gui::ListBoxModel {
	
public:
	ExpressionListBoxModel() {}
	virtual ~ExpressionListBoxModel() {}
	void add(const TestExpression& exp) {
		_data.push_back(exp);
	}
	uint32_t size() const {
		return _data.size();
	}
	const char* name(uint32_t index) const {
		return _data[index].name;
	}
	const TestExpression& getExpression(uint32_t idx) const {
		return _data[idx];
	}
	TestExpression& getExpression(uint32_t idx) {
		return _data[idx];
	}
private:
	std::vector<TestExpression> _data;
};


class VMTest : public TestSpriteApp {

public:
	VMTest();
	~VMTest();
	bool init();
	ds::RenderSettings getRenderSettings();
	void tick(float dt);
	void render();
	void renderGUI();
	bool usesGUI() {
		return true;
	}
private:
	void compile(const char* source);
	ds::vec2 _startPos;
	ds::vec2 _endPos;
	float _timer;
	float _ttl;
	int _dialogState;
	p2i _dialogPos;
	bool _showDiagram;
	Sprite _sprite;
	char _source[128];
	vm::Context _vmCtx;
	vm::Token _byteCode[256];
	uint16_t _numTokens;
	float _result;
	float _values[65];
	float _min;
	float _max;
	ExpressionListBoxModel _listModel;
};

