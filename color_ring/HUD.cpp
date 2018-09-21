#include "HUD.h"

float static FIXED_FONT_WIDTH = 32.0f;

const static ds::vec4 BIG_NUMBERS[] = {
	ds::vec4(0, 150, 31, 21),
	ds::vec4(30, 150, 15, 21),
	ds::vec4(45, 150, 28, 21),
	ds::vec4(73, 150, 28, 21),
	ds::vec4(101, 150, 29, 21),
	ds::vec4(130, 150, 28, 21),
	ds::vec4(158, 150, 30, 21),
	ds::vec4(188, 150, 29, 21),
	ds::vec4(217, 150, 30, 21),
	ds::vec4(247, 150, 30, 21)
};

HUD::HUD() {
	for (int i = 0; i < 3; ++i) {
		HUDAnimation& ha = _hudAnimations[i];
		ha.value = 1.0f;
		ha.start = 1.0f;
		ha.end = 1.4f;
		ha.timer = 0.0f;
		ha.ttl = 0.0f;
		ha.type = tweening::easeInOutElastic;
	}
}

void HUD::reset() {
	_filled = 0;
	_score = 0;
	_time = 60;
	_timer = 0.0f;
}

// -------------------------------------------------------
// draw big numbers
// -------------------------------------------------------
void HUD::drawBigNumber(std::unique_ptr<SpriteBatchBuffer>& sprites,const ds::vec2& pos, int value, int digits, float scale) {
	int s = pow(10, digits - 1);
	int tmp = value;
	int sx = pos.x - digits * FIXED_FONT_WIDTH * 0.5f + (1.0f - scale) * BIG_NUMBERS[0].z;
	int sy = pos.y;
	for (int i = 0; i < digits; ++i) {
		int t = tmp / s;
		tmp = tmp - s * t;
		sprites->add(ds::vec2(sx, sy), BIG_NUMBERS[t], ds::vec2(scale));
		s /= 10;
		sx += FIXED_FONT_WIDTH * scale;
	}
}

// -------------------------------------------------------
// tick
// -------------------------------------------------------
void HUD::tick(float dt) {

	if (_time > 0) {
		_timer += dt;
		if (_timer >= 1.0f) {
			--_time;
			_timer -= 1.0f;
			_hudAnimations[2].timer = 0.0f;
			_hudAnimations[2].ttl = 0.5f;
		}
	}

	for (int i = 0; i < 3; ++i) {
		HUDAnimation& ha = _hudAnimations[i];
		if (ha.timer >= ha.ttl) {
			ha.value = ha.end;
		}
		else {
			ha.timer += dt;
			ha.value = tweening::interpolate(ha.type, ha.start, ha.end, ha.timer, ha.ttl);
		}
	}
}

// -------------------------------------------------------
// increment filled and timer
// -------------------------------------------------------
void HUD::incrementFilled(int filled) {
	_filled += filled;
	_time += 2;
	if (_time > 60) {
		_time = 60;
	}
}

// -------------------------------------------------------
// render
// -------------------------------------------------------
void HUD::render(std::unique_ptr<SpriteBatchBuffer>& sprites) {
	drawBigNumber(sprites, ds::vec2(150, 720), _score, 6, _hudAnimations[0].value);
	drawBigNumber(sprites, ds::vec2(950, 720), _filled, 2, _hudAnimations[1].value);
	drawBigNumber(sprites, ds::vec2(950, 100), _time, 2, _hudAnimations[2].value);
}