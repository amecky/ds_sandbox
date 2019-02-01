#pragma once
#include <diesel.h>
#include <ds_imgui.h>
#include "BlockArray.h"
#include "fixed_array.h"

struct LinePoint {
	float height;
	float targetHeight;
	float speed;
};

enum class BorderDirection {
	VERTICAL,
	HORIZONTAL
};

struct Border {

	FixedArray<LinePoint> bottomLine;
	FixedArray<LinePoint> topLine;
	FixedArray<LinePoint> leftLine;
	FixedArray<LinePoint> rightLine;

	Border() : bottomLine(95), topLine(95) , leftLine(70), rightLine(70) {

	}
};




struct Bullets;

void initialize_border(Border* border);

void render_border(Border* border, RID textureId);

void update_border(Border* border, float dt);

void check_collisions(Border* border, Bullets* bullets, int particleSystem, int emitter);
