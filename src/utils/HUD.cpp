#include "HUD.h"
#include "tweening.h"
#include "..\Constants.h"
#include "..\GameSettings.h"

HUD::HUD(SpriteBatchBuffer* buffer,  GameContext* context, Score* score) 
	: _buffer(buffer) , _gameContext(context), _score(score) , _pieces(3), _points(6), _minutes(2), _seconds(2) {
	reset();
}

HUD::~HUD() {
}

// ------------------------------------------------------
// reset
// ------------------------------------------------------
void HUD::reset(TimerMode timerMode) {
	_timerMode = timerMode;
	if (timerMode == TimerMode::TM_INC) {
		_minutes.setValue(0);
		_seconds.setValue(0);
	}
	else {
		_minutes.setValue(3);
		_seconds.setValue(0);
	}
	_pieces.setValue(TOTAL);
	_timer = 0.0f;
	_score->points = 0;
	_score->minutes = 0;
	_score->seconds = 0;
	_score->itemsCleared = 0;
	_score->highestCombo = 0;
	rebuildScore(false);
	setPieces(TOTAL);
}

// ------------------------------------------------------
// scale number
// ------------------------------------------------------
void HUD::scaleNumber(Number& nr, float dt) {
	for (int i = 0; i < nr.size; ++i) {
		if (nr.scaleFlags[i] == 1) {
			nr.scalingTimers[i] += dt;
			if (nr.scalingTimers[i] >= 0.4f) {
				nr.scaleFlags[i] = 0;
			}
		}
	}
}
// ------------------------------------------------------
// tick
// ------------------------------------------------------
void HUD::tick(float dt) {
	_timer += dt;
	if (_timer > 1.0f) {
		_timer -= 1.0f;
		if (_timerMode == TimerMode::TM_INC) {
			++_seconds.value;
			if (_seconds.value >= 60) {
				_seconds.value = 0;
				++_minutes.value;
			}
		}
		else {
			--_seconds.value;
			if (_seconds.value < 0) {
				_seconds.value = 59;
				--_minutes.value;
			}
		}
	}
	_minutes.setValue(_minutes.value, false);
	_seconds.setValue(_seconds.value, false);
	scaleNumber(_points, dt);
	scaleNumber(_pieces, dt);
}

// ------------------------------------------------------
// set number
// ------------------------------------------------------
void HUD::rebuildScore(bool flash) {
	_points.setValue(_score->points);
}

// ------------------------------------------------------
// set pieces
// ------------------------------------------------------
void HUD::setPieces(int pc) {
	if (pc < 0) {
		pc = 0;
	}
	_pieces.setValue(pc);
}

void HUD::renderNumber(const Number& nr, const ds::vec2& startPos) {
	ds::vec2 p = startPos;
	float width = nr.size * 38.0f;
	p.x = startPos.x - width * 0.5f;
	for (int i = 0; i < nr.size; ++i) {
		float scale = 1.0f;
		if (nr.scaleFlags[i] == 1) {
			scale = tweening::interpolate(tweening::easeOutElastic, 2.5f, 1.0f, nr.scalingTimers[i], 0.4f);
		}
		_buffer->add(p, ds::vec4(200 + nr.items[i] * 38, 200, 38, 30), ds::vec2(1.0f, scale));
		p.x += 38.0f;
	}
}
// ------------------------------------------------------
// render
// ------------------------------------------------------
void HUD::render() {
	
	ds::vec2 p(160, 720);
	p.x += 20.0f;
	renderNumber(_points, p);
	
	p.y = 40.0f;
	p.x = 490.0f;
	p.x = 512.0f;
	renderNumber(_pieces, p);

	p.y = 720.0f;
	p.x = 710.0f;
	p.x = 820.0f;
	p.x = 725.0f;
	renderNumber(_minutes, p);	
	p.x += 38.0f * 3.0f;
	renderNumber(_seconds, p);
}
