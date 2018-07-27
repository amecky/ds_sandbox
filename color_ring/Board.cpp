#include "Board.h"
#include "colors.h"
#include "ColorRing.h"
#include "math.h"
#include <algorithm>

static const ds::vec4 NUMBERS[] = {
	ds::vec4(  0, 135, 20, 15),
	ds::vec4( 20, 135,  9, 15),
	ds::vec4( 29, 135, 18, 15),
	ds::vec4( 47, 135, 19, 15),
	ds::vec4( 66, 135, 20, 15),
	ds::vec4( 86, 135, 18, 15),
	ds::vec4(106, 135, 20, 15),
	ds::vec4(126, 135, 17, 15),
	ds::vec4(143, 135, 20, 15),
	ds::vec4(163, 135, 20, 15)
};

Board::Board(RID textureID) {
	_sprites = std::make_unique<SpriteBatchBuffer>(SpriteBatchDesc()
		.MaxSprites(2048)
		.Texture(textureID)
	);
	_colors[4] = ds::Color(32, 32, 32, 255);
	rebuildColors();
	_selectedColor = 0;
	_colorRing = std::make_unique<ColorRing>();
	_colorRing->createDrawItem(textureID);
	_colorRing->reset();
	_player.rotation = 0.0f;
}

Board::~Board() {
}

// -------------------------------------------------------
// reset
// -------------------------------------------------------
void Board::reset() {
	rebuildColors();
	_selectedColor = 0;
	_colorRing->reset();
	_player.rotation = 0.0f;
	_filled = 0;
}

// -------------------------------------------------------
// rebuild colors
// -------------------------------------------------------
void Board::rebuildColors() {
	float goldenRatioConjugate = 0.618033988749895f;
	float currentHue = ds::random(0.0f, 1.0f);
	for (int i = 0; i < 4; i++) {
		HSL hslColor = HSL(currentHue * 360.0f, 50.0f, 50.0f);
		_colors[i] = color::hsl2rgb(hslColor);
		currentHue += goldenRatioConjugate;
		if (currentHue > 1.0f) {
			currentHue -= 1.0f;
		}
	}
}

void Board::debug() {
	_colorRing->debug();
}

void Board::drawNumber(int index, const ds::vec2& pos, float rotation) {
	_sprites->add(pos, NUMBERS[index], ds::vec2(1.0f), rotation);
}

void Board::drawNumber(int value, int segment) {
	float step = ds::TWO_PI / 18.0f;
	float ang = static_cast<float>(segment) * step + step * 0.5f;
	if (value < 10) {
		ds::vec2 pos = ds::vec2(512, 384) + ds::vec2(cosf(ang), sinf(ang)) * 335.0f;
		_sprites->add(pos, NUMBERS[value], ds::vec2(1.0f), ang - ds::PI * 0.5f);
	}
	else {
		int upper = value / 10;
		int lower = value - upper * 10;
		ang -= DEGTORAD(2.0f);
		ds::vec2 pos = ds::vec2(512, 384) + ds::vec2(cosf(ang), sinf(ang)) * 335.0f;
		_sprites->add(pos, NUMBERS[lower], ds::vec2(1.0f), ang - ds::PI * 0.5f);
		ang += DEGTORAD(4.0f);
		pos = ds::vec2(512, 384) + ds::vec2(cosf(ang), sinf(ang)) * 335.0f;
		_sprites->add(pos, NUMBERS[upper], ds::vec2(1.0f), ang - ds::PI * 0.5f);

	}
}

// -------------------------------------------------------
// render
// -------------------------------------------------------
void Board::render() {

	_colorRing->render(_colors);

	_sprites->begin();

	_sprites->add(ds::vec2(512, 384), ds::vec4(120, 50, 50, 50), ds::vec2(1.0f), _player.rotation);

	// show target indicator

	float ra = _colorRing->rasterizeAngle(_player.rotation);

	ds::dbgPrint(0, 0, "RA %g", RADTODEG(ra));

	ds::dbgPrint(0, 1, "Filled %d", _filled);

	for (int i = 0; i < _colorRing->getNumSegments(); ++i) {
		int clr = _colorRing->getColor(i);
		if (clr != -1) {
			float t = _colorRing->getTimer(i);
			int v = 10 - static_cast<int>(t);
			drawNumber(v, i);
		}
	}


	ds::dbgPrint(0, 2, "Part %d", _colorRing->getPartIndex(_player.rotation));

	ds::vec2 p = ds::vec2(cosf(ra), sinf(ra));
	_sprites->add(ds::vec2(512, 384) + p * 296.0f, ds::vec4(170, 0, 4, 12), ds::vec2(1.0f), ra);
	_sprites->add(ds::vec2(512, 384) + p * 330.0f, ds::vec4(170, 0, 4, 12), ds::vec2(1.0f), ra);

	// show separators
	
	// draw bullets
	for (auto& b: _bullets) {
		float d = length(ds::vec2(512, 384) - b.pos) / 300.0f;
		float scaleY = 0.8f - d * 0.6f;
		_sprites->add(b.pos, ds::vec4(80, 50, 12, 12), ds::vec2(1.0f, scaleY), b.rotation, _colors[b.color]);
	}

	// draw color selection bars
	ds::vec2 colorPos = ds::vec2(200, 740);
	colorPos.x = (1024.0f - 20.0f - _selectedColor * 100.0f) / 2;
	for (int i = 0; i < 4; ++i) {
		if (i == _selectedColor) {
			_sprites->add(colorPos, ds::vec4(230, 0, 60, 30));
		}
		_sprites->add(colorPos, ds::vec4(172, 2, 40, 20), ds::vec2(1.0f), 0.0f, _colors[i]);
		colorPos.x += 60;
	}

	_hud.render(_sprites);

	_sprites->flush();
}

// -------------------------------------------------------
// tick
// -------------------------------------------------------
void Board::tick(float dt) {

	moveBullets(dt);

	ds::vec2 mp = ds::getMousePosition();

	_player.rotation = get_rotation(mp - ds::vec2(512, 384));

	if (ds::isMouseButtonPressed(0)) {
		shootBullets(dt);
	}

	_colorRing->tick(dt);

}

// -------------------------------------------------------
// shoot bullets
// -------------------------------------------------------
void Board::shootBullets(float dt) {
	_bulletTimer += dt;
	if (_bulletTimer >= 0.1f) {
		if (_bullets.size() < 64) {
			Bullet b;
			b.pos = ds::vec2(512, 384);
			float ra = _colorRing->rasterizeAngle(_player.rotation);
			b.velocity = ds::vec2(cosf(ra), sinf(ra)) * 500.0f;
			b.rotation = ra;
			b.color = _selectedColor;
			_bullets.push_back(b);
		}
		_bulletTimer -= 0.1f;
	}
}

// -------------------------------------------------------
// select next color
// -------------------------------------------------------
void Board::selectNextColor() {
	++_selectedColor;
	if (_selectedColor > 3) {
		_selectedColor = 0;
	}
}

// -------------------------------------------------------
// move bullets
// -------------------------------------------------------
void Board::moveBullets(float dt) {

	for (auto& b : _bullets) {
		b.pos += b.velocity * ds::getElapsedSeconds();
		float l = sqr_length(ds::vec2(512, 384) - b.pos);
		if (l > 300.0f * 300.0f) {
			int idx = _colorRing->getPartIndex(_player.rotation);
			int filled = _colorRing->markPart(idx, _selectedColor);
			if (filled > 0) {
				// do some more here
				_filled += filled;
			}
		}
	}

	_bullets.erase(std::remove_if(_bullets.begin(), _bullets.end(),
		[](const Bullet& b) {
		float l = sqr_length(ds::vec2(512, 384) - b.pos);
		return l > 300.0f * 300.0f;
	}), _bullets.end());
}
