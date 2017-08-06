#pragma once
#include <diesel.h>

const float MOVE_TTL = 1.2f;
const int CELL_SIZE = 40;
const int HALF_CELL_SIZE = 20;
const int STARTX = 130;
const int STARTY = 164;
const int MAX_X = 20;
const int MAX_Y = 12;
const int TOTAL = MAX_X * MAX_Y;

const float MESSAGE_TTL = 0.8f;
const float MESSAGE_SCALE = 0.4f;

// Font starts at 200, 230
// x-offset / width
const ds::vec2 FONT_DEF[] = {
	ds::vec2(1,35),   // A
	ds::vec2(35,29),  // B
	ds::vec2(66,27),  // C
	ds::vec2(94,30),  // D
	ds::vec2(125,27), // E
	ds::vec2(152,28), // F
	ds::vec2(180,29), // G
	ds::vec2(210,29), // H
	ds::vec2(240,12), // I
	ds::vec2(252,17), // J
	ds::vec2(269,28), // K
	ds::vec2(297,28), // L
	ds::vec2(325,39), // M
	ds::vec2(365,31), // N
	ds::vec2(396,29), // O
	ds::vec2(426,30), // P
	ds::vec2(456,29), // Q
	ds::vec2(485,30), // R
	ds::vec2(515,27), // S 
	ds::vec2(541,26), // T
	ds::vec2(567,29), // U
	ds::vec2(596,33), // V
	ds::vec2(629,41), // W
	ds::vec2(670,31), // X
	ds::vec2(701,32), // Y
	ds::vec2(733,27)  // Z
};
