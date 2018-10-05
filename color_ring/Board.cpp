#include "Board.h"
#include <SpriteBatchBuffer.h>
#include "common\common.h"
#include "common\colors.h"
#include "ColorRing.h"
#include "math.h"
#include <algorithm>
#include "particles\ParticleManager.h"
#include "particles\particle_effects.h"

const static float BULLET_OUTER_RADIUS = 280.0f;
const static float SQR_BULLET_OUTER_RADIUS = BULLET_OUTER_RADIUS * BULLET_OUTER_RADIUS;

enum TextureNames {
	TN_PARTICLE,
	TN_STAR,
	TN_BULLET,
	TN_G,
	TN_A,
	TN_M,
	TN_E,
	TN_O,
	TN_V,
	TN_R
};

const TextureNames GAME_OVER_TEXTURES[] = { TN_G,TN_A,TN_M,TN_E,TN_O,TN_V,TN_E,TN_R };

const static ds::vec4 TEXTURES[] = {
	ds::vec4(40,  50, 16, 16),
	ds::vec4(0,  75, 26, 26),
	ds::vec4(80, 50, 12, 12), // bullet
	ds::vec4(  0, 180, 53, 42), // G
	ds::vec4( 60, 180, 63, 42), // A
	ds::vec4(126, 180, 69, 42), // M
	ds::vec4(203, 180, 51, 42), // E
	ds::vec4(262, 180, 54, 42), // O
	ds::vec4(320, 180, 63, 42), // V
	ds::vec4(446, 180, 54, 42), // R
};



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

Board::Board(RenderEnvironment* env) : _env(env) , _player(env) , _hud(env) {
	_colors[4] = ds::Color(32, 32, 32, 255);
	_colorRing = std::make_unique<ColorRing>();
	_colorRing->createDrawItem(_env->textureId);
	_colorRing->buildRingVertices();
	_colorRing->reset();
	
	_pressed = false;	

	reset();	
}

Board::~Board() {
}

// -------------------------------------------------------
// reset
// -------------------------------------------------------
void Board::reset() {
	sprites::clear(*_env->spriteArray);
	rebuildColors();
	_selectedColor = 0;
	_colorRing->reset();
	_player.reset();
	_hud.reset();

	ds::vec2 colorPos = ds::vec2(200, 300);
	colorPos.x = (1024.0f - 4 * 50.0f + 60.0f) / 2;
	for (int i = 0; i < 4; ++i) {
		if (i == 0) {
			_colorSelectionIds[i] = sprites::add(*_env->spriteArray, colorPos, ds::vec4(300, 0, 50, 30), ds::vec2(1.0f), 0.0f, ds::Color(255, 255, 255, 255));
		}
		else {
			_colorSelectionIds[i] = sprites::add(*_env->spriteArray, colorPos, ds::vec4(300, 0, 50, 30), ds::vec2(1.0f), 0.0f, ds::Color(32, 32, 32, 255));
		}
		_colorSelectionIds[i + 4] = sprites::add(*_env->spriteArray, colorPos, ds::vec4(172, 2, 40, 20), ds::vec2(1.0f), 0.0f, _colors[i]);
		colorPos.x += 60;
	}
	
	_player.setColor(_colors[_selectedColor]);
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

// -------------------------------------------------------
// debug
// -------------------------------------------------------
void Board::debug() {
	_colorRing->debug();
}

// -------------------------------------------------------
// draw number
// -------------------------------------------------------
void Board::drawNumber(int index, const ds::vec2& pos, float rotation) {
	_env->sprites->add(pos, NUMBERS[index], ds::vec2(1.0f), rotation);
}

// -------------------------------------------------------
// draw segment timer
// -------------------------------------------------------
void Board::drawSegmentTimer(int value, int segment) {
	float step = ds::TWO_PI / 18.0f;
	float ang = static_cast<float>(segment) * step + step * 0.5f;
	if (value < 10) {
		ds::vec2 pos = ds::vec2(512, 384) + ds::vec2(cosf(ang), sinf(ang)) * 305.0f;
		_env->sprites->add(pos, NUMBERS[value], ds::vec2(1.0f), ang - ds::PI * 0.5f);
	}
	else {
		int upper = value / 10;
		int lower = value - upper * 10;
		ang -= DEGTORAD(2.0f);
		ds::vec2 pos = ds::vec2(512, 384) + ds::vec2(cosf(ang), sinf(ang)) * 305.0f;
		_env->sprites->add(pos, NUMBERS[lower], ds::vec2(1.0f), ang - ds::PI * 0.5f);
		ang += DEGTORAD(4.0f);
		pos = ds::vec2(512, 384) + ds::vec2(cosf(ang), sinf(ang)) * 305.0f;
		_env->sprites->add(pos, NUMBERS[upper], ds::vec2(1.0f), ang - ds::PI * 0.5f);

	}
}


// -------------------------------------------------------
// render
// -------------------------------------------------------
void Board::render() {

	_colorRing->render(_colors);

	_env->sprites->render(*_env->spriteArray);

	_env->sprites->begin();

	// draw timer of each segment
	for (int i = 0; i < _colorRing->getNumSegments(); ++i) {
		int clr = _colorRing->getColor(i);
		if (clr != -1) {
			float t = _colorRing->getTimer(i);
			int v = 10 - static_cast<int>(t);
			drawSegmentTimer(v, i);
		}
	}

	// animated letter
	//for (auto& letter : _animatedLetters) {
		//_env->sprites->add(letter.pos, TEXTURES[letter.texureIndex]);
	//}

	_env->sprites->flush();

	// particles
	_env->particles->render(_env->sprites->getRenderPass(), _env->camera.viewProjectionMatrix);

	
}

// -------------------------------------------------------
// tick
// -------------------------------------------------------
void Board::tick(float dt) {

	moveBullets(dt);

	if (ds::isMouseButtonPressed(0)) {
		shootBullets(dt);
	}

	_player.tick(dt, _colorRing->rasterizeAngle(_player.getRotation()));

	_colorRing->tick(dt);

	_env->particles->tick(dt);

	if (ds::isKeyPressed('P')) {
		_pressed = true;
	}
	else if ( _pressed) {
		_animatedLetters.clear();
		float start = (1024.0f - 538.0f + 53.0f) * 0.5f;
		for (int i = 0; i < 8; ++i) {
			AnimatedLetter letter;
			letter.start = ds::vec2(start, 900.0f + ds::random(0,150));
			letter.end = ds::vec2(letter.start.x, 384.0f);
			letter.texureIndex = GAME_OVER_TEXTURES[i];
			start += TEXTURES[GAME_OVER_TEXTURES[i]].z;
			start += 10.0f;
			letter.timer = 0.0f;
			letter.ttl = 2.0f + ds::random(-0.5f,0.5f);
			letter.type = tweening::easeOutElastic;
			_animatedLetters.push_back(letter);
		}
		_pressed = false;
	}

	for (auto& letter : _animatedLetters) {
		if (letter.timer <= letter.ttl) {
			letter.timer += dt;
			letter.pos = tweening::interpolate(letter.type, letter.start, letter.end, letter.timer, letter.ttl);
			if (letter.timer >= letter.ttl) {
				letter.pos = letter.end;
			}
		}
	}

	_hud.tick(dt);
}

// -------------------------------------------------------
// shoot bullets
// -------------------------------------------------------
void Board::shootBullets(float dt) {
	_bulletTimer += dt;
	if (_bulletTimer >= 0.1f) {
		if (_bullets.size() < 64) {
			Bullet b;
			float ra = _colorRing->rasterizeAngle(_player.getRotation());
			b.id = sprites::add(*_env->spriteArray,ds::vec2(512, 384), TEXTURES[TN_BULLET], ds::vec2(1.0f,1.0f),ra,_colors[_selectedColor]);
			b.velocity = ds::vec2(cosf(ra), sinf(ra)) * 500.0f;
			b.color = _selectedColor;
			b.alive = true;
			_bullets.push_back(b);
		}
		_bulletTimer -= 0.1f;
	}
}

// -------------------------------------------------------
// select next color
// -------------------------------------------------------
void Board::selectNextColor() {
	Sprite& current = sprites::get(*_env->spriteArray, _colorSelectionIds[_selectedColor]);
	current.color = ds::Color(32, 32, 32, 255);
	++_selectedColor;
	if (_selectedColor > 3) {
		_selectedColor = 0;
	}
	Sprite& next = sprites::get(*_env->spriteArray, _colorSelectionIds[_selectedColor]);
	next.color = ds::Color(255, 255, 255, 255);
}

// -------------------------------------------------------
// move bullets
// -------------------------------------------------------
void Board::moveBullets(float dt) {

	for (auto& b : _bullets) {
		Sprite& sprite = sprites::get(*_env->spriteArray, b.id);
		sprite.position += b.velocity * ds::getElapsedSeconds();
		float l = sqr_length(ds::vec2(512, 384) - sprite.position);
		if (l > SQR_BULLET_OUTER_RADIUS) {
			rebound(_env, sprite.position, sprite.rotation, _colors[b.color]);
			int idx = _colorRing->getPartIndex(_player.getRotation());
			int filled = _colorRing->markPart(idx, _selectedColor);
			if (filled != -1) {
				int segment = idx / _colorRing->getNumPartsPerSegment();
				showFilled(_env, segment, _colorRing->getNumSegments(), _colors[filled]);
				_hud.incrementFilled(filled);				
			}
			sprites::remove(*_env->spriteArray, b.id);
			b.alive = false;
		}
		else {
			float ln = length(ds::vec2(512, 384) - sprite.position);
			float d = ln / 300.0f;
			float scaleY = 0.8f - d * 0.6f;
			sprite.scaling = ds::vec2(1.0f, scaleY);
		}
	}

	_bullets.erase(std::remove_if(_bullets.begin(), _bullets.end(),
		[](const Bullet& b) {
			return !b.alive;
	}), _bullets.end());
}
