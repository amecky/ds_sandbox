#include "Board.h"
#include "colors.h"
#include "ColorRing.h"
#include "math.h"

Board::Board(RID textureID) {
	_sprites = new SpriteBatchBuffer(SpriteBatchDesc()
		.MaxSprites(2048)
		.Texture(textureID)
	);
	_colors[4] = ds::Color(32, 32, 32, 255);
	rebuildColors();
	_selectedColor = 0;
	_numBullets = 0;
	_colorRing = new ColorRing;
	_colorRing->createDrawItem(textureID);
	_colorRing->reset();
	_player.rotation = 0.0f;
}

Board::~Board() {
	delete _colorRing;
	delete _sprites;
}

// -------------------------------------------------------
// reset
// -------------------------------------------------------
void Board::reset() {
	rebuildColors();
	_selectedColor = 0;
	_numBullets = 0;
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
// -------------------------------------------------------
// render
// -------------------------------------------------------
void Board::render() {

	_colorRing->render(_colors);

	_sprites->begin();

	_sprites->add(ds::vec2(512, 384), ds::vec4(0, 60, 52, 52), ds::vec2(1.0f), _player.rotation);

	// show target indicator

	float ra = _colorRing->rasterizeAngle(_player.rotation);

	ds::dbgPrint(0, 0, "RA %g", RADTODEG(ra));

	ds::dbgPrint(0, 1, "Fiiled %d", _filled);

	ds::vec2 p = ds::vec2(cosf(ra), sinf(ra));
	_sprites->add(ds::vec2(512, 384) + p * 295.0f, ds::vec4(80, 50, 8, 12), ds::vec2(1.0f), ra);
	_sprites->add(ds::vec2(512, 384) + p * 325.0f, ds::vec4(80, 50, 8, 12), ds::vec2(1.0f), ra);

	// show separators
	
	// draw bullets
	for (int i = 0; i < _numBullets; ++i) {
		Bullet& b = _bullets[i];
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
		Bullet& b = _bullets[_numBullets++];
		b.pos = ds::vec2(512, 384);
		float ra = _colorRing->rasterizeAngle(_player.rotation);
		b.velocity = ds::vec2(cosf(ra), sinf(ra)) * 500.0f;
		b.rotation = ra;
		b.color = _selectedColor;
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

	for (int i = 0; i < _numBullets; ++i) {
		Bullet& b = _bullets[i];
		b.pos += b.velocity * ds::getElapsedSeconds();
		float l = sqr_length(ds::vec2(512, 384) - b.pos);
		if (l > 300.0f * 300.0f) {
			int filled = _colorRing->markPart(b.rotation, _selectedColor);
			if (filled > 0) {
				// do some more here
				_filled += filled;
			}
			_bullets[i] = _bullets[_numBullets - 1];
			--_numBullets;
		}
	}
}
