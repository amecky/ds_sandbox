#include "HUD.h"

const static ds::vec4 NUMBERS[] = {
	ds::vec4(  0, 150, 31, 31),
	ds::vec4( 30, 150, 15, 31),
	ds::vec4( 45, 150, 28, 31),
	ds::vec4( 73, 150, 28, 31),
	ds::vec4(101, 150, 29, 31),
	ds::vec4(130, 150, 28, 31),
	ds::vec4(158, 150, 30, 31),
	ds::vec4(188, 150, 29, 31),
	ds::vec4(217, 150, 30, 31),
	ds::vec4(247, 150, 30, 31)
};

HUD::HUD() {
	_score = 4289;
}

void HUD::drawNumber(const std::unique_ptr<SpriteBatchBuffer>& sprites,const ds::vec2& pos, int value, int digits) {
	int s = pow(10, digits - 1);
	int tmp = value;
	int sx = pos.x - digits * 20;
	for (int i = 0; i < digits; ++i) {
		int t = tmp / s;
		tmp = tmp - s * t;
		sprites->add(ds::vec2(sx + i * 40, 100), NUMBERS[t]);
		s /= 10;
	}
}

void HUD::render(const std::unique_ptr<SpriteBatchBuffer>& sprites) {
	drawNumber(sprites, ds::vec2(200, 200), _score, 6);
}