#include "border.h"
#include <ds_sprites.h>
#include "player.h"
#include <ds_squares.h>

const float Tension = 0.025f;
const float Dampening = 0.025f;
const float Spread = 0.15f;

// -------------------------------------------------------------------------
// initialize border
// -------------------------------------------------------------------------
void initialize_border(Border* border) {
	border->bottomLine.fill({ 20.0f,20.0f,0.0f });
	border->topLine.fill({ 20.0f,20.0f,0.0f });
	border->leftLine.fill({ 20.0f,20.0f,0.0f });
	border->rightLine.fill({ 20.0f,20.0f,0.0f });
}

// -------------------------------------------------------------------------
// build vertical column
// -------------------------------------------------------------------------
static void brd__build_vertical_column(const FixedArray<LinePoint>& linePoints, int index, const ds::vec2& startPos, ds::vec2* ret) {
	ds::vec2 f = ds::vec2(startPos.x() + index * 10.0f, startPos.y() + linePoints[index].height);
	ds::vec2 s = ds::vec2(startPos.x() + (index + 1)* 10.0f, startPos.y() + linePoints[index + 1].height);
	ret[0] = ds::vec2(f.x(), f.y() - 20.0f);
	ret[1] = ds::vec2(f.x(), f.y());
	ret[2] = ds::vec2(s.x(), s.y());
	ret[3] = ds::vec2(s.x(), s.y() - 20.0f);
}

// -------------------------------------------------------------------------
// build horizontal column
// -------------------------------------------------------------------------
static void brd__build_horizontal_column(const FixedArray<LinePoint>& linePoints, int index, const ds::vec2& startPos, ds::vec2* ret) {
	ds::vec2 f = ds::vec2(startPos.x() + linePoints[index].height, startPos.y() + index * 10.0f);
	ds::vec2 s = ds::vec2(startPos.x() + linePoints[index + 1].height, startPos.y() + (index + 1) * 10.0f);
	ret[0] = ds::vec2(f.x(), f.y());
	ret[1] = ds::vec2(s.x(), s.y());
	ret[2] = ds::vec2(s.x() + 20.0f, s.y());
	ret[3] = ds::vec2(f.x() + 20.0f, f.y());
}

// -------------------------------------------------------------------------
// render line points - internal
// -------------------------------------------------------------------------
static void brd__render(const FixedArray<LinePoint>& linePoints, const ds::vec2& startPos, RID textureId, BorderDirection dir) {
	ds::vec2 points[4];
	ds::vec4 tr;
	for (int i = 0; i < linePoints._num - 1; ++i) {
		if (dir == BorderDirection::VERTICAL) {
			brd__build_vertical_column(linePoints, i, startPos, points);
			tr = ds::vec4(400, 10, 40, 40);
		}
		else {
			brd__build_horizontal_column(linePoints, i, startPos, points);
			tr = ds::vec4(357, 50, 40, 40);
		}
		float d = linePoints[i].height - linePoints[i].targetHeight;
		if (d < 0.0f) {
			d *= -1.0f;
		}
		float r = 0.25f + d / 10.0f * 0.75f;
		if (r > 1.0f) {
			r = 1.0f;
		}
		quads::draw(textureId, points, tr, ds::vec2(1.0f), 0.0f, ds::Color(r, 0.0f, 0.0f, 1.0f));
	}
}

// -------------------------------------------------------------------------
// update line points
// -------------------------------------------------------------------------
static void brd__update_line(const FixedArray<LinePoint>& linePoints, float dt) {

	float lDeltas[512];
	float rDeltas[512];

	for (int i = 1; i < linePoints._num - 1; ++i) {
		LinePoint& lp = linePoints[i];
		float x = lp.targetHeight - lp.height;
		lp.speed += Tension * x - lp.speed * Dampening;
		lp.height += lp.speed;
	}

	// do some passes where columns pull on their neighbours
	for (int j = 0; j < 2; j++) {
		for (int i = 0; i < linePoints._num; i++) {
			if (i > 0) {
				lDeltas[i] = Spread * (linePoints[i].height - linePoints[i - 1].height);
				linePoints._data[i - 1].speed += lDeltas[i];
			}
			if (i < linePoints._num - 1) {
				rDeltas[i] = Spread * (linePoints[i].height - linePoints[i + 1].height);
				linePoints._data[i + 1].speed += rDeltas[i];
			}
		}

		for (int i = 0; i < linePoints._num; i++) {
			if (i > 0) {
				linePoints._data[i - 1].height += lDeltas[i];
			}
			if (i < linePoints._num - 1) {
				linePoints._data[i + 1].height += rDeltas[i];
			}
		}
	}
	// fix corner points
	linePoints._data[0].height = linePoints[0].targetHeight;
	linePoints._data[linePoints._num - 1].height = linePoints[linePoints._num - 1].targetHeight;
}

// -------------------------------------------------------------------------
// render border
// -------------------------------------------------------------------------
void render_border(Border* border, RID textureId) {
	brd__render(border->topLine, ds::vec2(20.0f, 700.0f), textureId, BorderDirection::VERTICAL);
	brd__render(border->bottomLine, ds::vec2(20.0f, 20.0f), textureId, BorderDirection::VERTICAL);
	brd__render(border->leftLine, ds::vec2(10.0f, 30.0f), textureId, BorderDirection::HORIZONTAL);
	brd__render(border->rightLine, ds::vec2(970.0f, 30.0f), textureId, BorderDirection::HORIZONTAL);
}

// -------------------------------------------------------------------------
// update border
// -------------------------------------------------------------------------
void update_border(Border* border, float dt) {
	brd__update_line(border->topLine, dt);
	brd__update_line(border->bottomLine, dt);
	brd__update_line(border->leftLine, dt);
	brd__update_line(border->rightLine, dt);
}

// -------------------------------------------------------------------------
// hit one entry - internal
// -------------------------------------------------------------------------
static void brd__splash(const FixedArray<LinePoint>& linePoints, int index, float speed) {
	linePoints._data[index].speed = speed;
}
// -------------------------------------------------------------------------
// check collision between bullets and border
// -------------------------------------------------------------------------
void check_collisions(Border* border, Bullets* bullets) {
	for (int i = 0; i < bullets->array->size; ++i) {
		ds::vec2& bp = bullets->positions[i];
		int idx = -1;
		bool hit = false;
		if (bp.y() <= 20.0f) {
			idx = (bp.x() - 20.0f) / 10.0f;
			if (idx >= 0 && idx < 100) {
				brd__splash(border->bottomLine, idx, -15.0f);
				hit = true;
			}
		}
		if (bp.y() >= 700.0f) {
			idx = (bp.x() - 20.0f) / 10.0f;
			if (idx >= 0 && idx < 100)
				brd__splash(border->topLine, idx, -15.0f);
			hit = true;
		}
		if (bp.x() <= 20.0f) {
			idx = (bp.y() - 30.0f) / 10.0f;
			if (idx >= 0 && idx < 70) {
				brd__splash(border->leftLine, idx, -15.0f);
				hit = true;
			}
		}
		if (bp.x() >= 970.0f) {
			idx = (bp.y() - 30.0f) / 10.0f;
			if (idx >= 0 && idx < 70) {
				brd__splash(border->rightLine, idx, -15.0f);
				hit = true;
			}
		}
		if (hit) {
			bullets::remove(bullets, i);
		}
	}
}

