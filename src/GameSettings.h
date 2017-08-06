#pragma once
#include <diesel.h>

struct GameContext {
	ds::Color colors[8];
};

struct GameSettings {
	float prepareTTL;
	float messageScale;
	float scaleUpMinTTL;
	float scaleUpMaxTTL;
	float flashTTL;
	float droppingTTL;
	float wiggleTTL;
	float wiggleScale;
	float clearMinTTL;
	float clearMaxTTL;
	float highlightTime;
	float logoSlideTTL;
};