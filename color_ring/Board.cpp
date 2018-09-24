#include "Board.h"
#include "colors.h"
#include "ColorRing.h"
#include "math.h"
#include <algorithm>

const static float BULLET_OUTER_RADIUS = 280.0f;
const static float SQR_BULLET_OUTER_RADIUS = BULLET_OUTER_RADIUS * BULLET_OUTER_RADIUS;

enum TextureNames {
	TN_PARTICLE,
	TN_STAR,
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
	ds::vec4( 40,  50, 16, 16),
	ds::vec4(  0,  75, 26, 26),
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

Board::Board(RID textureID) {
	_sprites = std::make_unique<SpriteBatchBuffer>(SpriteBatchDesc()
		.MaxSprites(2048)
		.Texture(textureID)
	);
	_particles = std::make_unique<ParticleManager>(textureID);
	_colors[4] = ds::Color(32, 32, 32, 255);
	rebuildColors();
	_selectedColor = 0;
	_colorRing = std::make_unique<ColorRing>();
	_colorRing->createDrawItem(textureID);
	_colorRing->buildRingVertices();
	_colorRing->reset();
	_player.rotation = 0.0f;
	_camera = ds::buildOrthographicCamera();
	_pressed = false;	
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
	_hud.reset();
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
	_sprites->add(pos, NUMBERS[index], ds::vec2(1.0f), rotation);
}

// -------------------------------------------------------
// draw segment timer
// -------------------------------------------------------
void Board::drawSegmentTimer(int value, int segment) {
	float step = ds::TWO_PI / 18.0f;
	float ang = static_cast<float>(segment) * step + step * 0.5f;
	if (value < 10) {
		ds::vec2 pos = ds::vec2(512, 384) + ds::vec2(cosf(ang), sinf(ang)) * 305.0f;
		_sprites->add(pos, NUMBERS[value], ds::vec2(1.0f), ang - ds::PI * 0.5f);
	}
	else {
		int upper = value / 10;
		int lower = value - upper * 10;
		ang -= DEGTORAD(2.0f);
		ds::vec2 pos = ds::vec2(512, 384) + ds::vec2(cosf(ang), sinf(ang)) * 305.0f;
		_sprites->add(pos, NUMBERS[lower], ds::vec2(1.0f), ang - ds::PI * 0.5f);
		ang += DEGTORAD(4.0f);
		pos = ds::vec2(512, 384) + ds::vec2(cosf(ang), sinf(ang)) * 305.0f;
		_sprites->add(pos, NUMBERS[upper], ds::vec2(1.0f), ang - ds::PI * 0.5f);

	}
}


// -------------------------------------------------------
// render
// -------------------------------------------------------
void Board::render() {

	_colorRing->render(_colors);

	_sprites->begin();

	// player
	_sprites->add(ds::vec2(512, 384), ds::vec4(120, 50, 50, 50), ds::vec2(1.0f), _player.rotation, _colors[_selectedColor]);
	
	// draw timer of each segment
	for (int i = 0; i < _colorRing->getNumSegments(); ++i) {
		int clr = _colorRing->getColor(i);
		if (clr != -1) {
			float t = _colorRing->getTimer(i);
			int v = 10 - static_cast<int>(t);
			drawSegmentTimer(v, i);
		}
	}

	// show target indicator
	float ra = _colorRing->rasterizeAngle(_player.rotation);
	ds::vec2 p = ds::vec2(cosf(ra), sinf(ra));
	_sprites->add(ds::vec2(512, 384) + p * 278.0f, ds::vec4(170, 0, 4, 12), ds::vec2(1.0f), ra);
	_sprites->add(ds::vec2(512, 384) + p * 332.0f, ds::vec4(170, 0, 4, 12), ds::vec2(1.0f), ra);

	// draw bullets
	for (auto& b: _bullets) {
		float d = length(ds::vec2(512, 384) - b.pos) / 300.0f;
		float scaleY = 0.8f - d * 0.6f;
		_sprites->add(b.pos, ds::vec4(80, 50, 12, 12), ds::vec2(1.0f, scaleY), b.rotation, _colors[b.color]);
	}

	// draw color selection bars
	ds::vec2 colorPos = ds::vec2(200, 300);
	//colorPos.x = (1024.0f - 20.0f - _selectedColor * 100.0f) / 2;
	colorPos.x = (1024.0f - 4 * 50.0f + 60.0f) / 2;
	for (int i = 0; i < 4; ++i) {
		if (i == _selectedColor) {			
			_sprites->add(colorPos, ds::vec4(172, 2, 40, 30), ds::vec2(1.0f), 0.0f, _colors[i]);
			_sprites->add(colorPos, ds::vec4(300, 0, 50, 30), ds::vec2(1.0f), 0.0f, ds::Color(255, 255, 255, 255));
		}
		else {
			_sprites->add(colorPos, ds::vec4(172, 2, 40, 30), ds::vec2(1.0f), 0.0f, _colors[i]);
			_sprites->add(colorPos, ds::vec4(300, 0, 50, 30), ds::vec2(1.0f),0.0f,ds::Color(32,32,32,255));
		}
		colorPos.x += 60;
	}

	// HUD
	_hud.render(_sprites);

	// animated letter
	for (auto& letter : _animatedLetters) {
		_sprites->add(letter.pos, TEXTURES[letter.texureIndex]);
	}

	_sprites->flush();

	// particles
	_particles->render(_sprites->getRenderPass(), _camera.viewProjectionMatrix);

	
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

	_particles->tick(dt);

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
// emitt partciles for filled segment
// -------------------------------------------------------
void Board::showFilled(int segment, const ds::Color& clr) {
	float rotationStep = ds::TWO_PI / static_cast<float>(segment);
	float step = ds::TWO_PI / static_cast<float>(_colorRing->getNumSegments()) / 16.0f;
	float current = ds::TWO_PI / static_cast<float>(_colorRing->getNumSegments()) * static_cast<float>(segment);
	float var = step * 0.1f;
	ParticleDesc desc[16];
	for (int i = 0; i < 16; ++i) {
		float angle = current + ds::random(-var, var);
		desc[i].position = ds::vec2(cos(angle), sin(angle)) * 315.0f + ds::vec2(512,384);
		desc[i].acceleration = ds::vec2(0.0f);
		desc[i].textureRect = TEXTURES[TN_STAR];
		desc[i].rotation = 0.0f;
		desc[i].rotationSpeed = 0.0f;
		desc[i].startColor = clr;
		desc[i].endColor = ds::Color(clr.r, clr.g, clr.b, 0.2f);
		desc[i].scale = ds::vec2(ds::random(0.6f, 0.9f));
		desc[i].ttl = ds::random(0.5f, 0.8f);
		angle = current + ds::random(-ds::PI * 0.5f, ds::PI * 0.5f);
		desc[i].velocity = ds::vec2(cos(angle), sin(angle)) * ds::random(-150.0f, 150.0f);
		current += step;
	}
	_particles->add(desc, 16);
}

// -------------------------------------------------------
// emitt partciles for dying bullets
// -------------------------------------------------------
void Board::rebound(const ds::vec2& p, float angle, const ds::Color& clr) {
	float step = ds::PI * 0.75f / 16.0f;
	float current = angle - ds::PI * 0.25f + ds::PI;
	float var = step * 0.1f;
	ParticleDesc desc[16];
	for (int i = 0; i < 16; ++i) {
		float angle = current + ds::random(-var, var);
		desc[i].position = ds::vec2(cos(angle), sin(angle)) + p;
		desc[i].acceleration = ds::vec2(0.0f);
		desc[i].textureRect = TEXTURES[TN_PARTICLE];
		desc[i].rotation = 0.0f;
		desc[i].rotationSpeed = 0.0f;
		desc[i].startColor = clr;
		desc[i].endColor = ds::Color(clr.r,clr.g,clr.b,0.2f);
		desc[i].scale = ds::vec2(ds::random(0.6f,0.9f));
		desc[i].ttl = ds::random(0.5f,0.8f);
		desc[i].velocity = ds::vec2(cos(angle), sin(angle)) * ds::random(150.0f,250.0f);
		current += step;
	}
	_particles->add(desc,16);
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
		if (l > SQR_BULLET_OUTER_RADIUS) {
			rebound(b.pos, b.rotation, _colors[b.color]);
			int idx = _colorRing->getPartIndex(_player.rotation);
			int filled = _colorRing->markPart(idx, _selectedColor);
			if (filled != -1) {
				int segment = idx / _colorRing->getNumPartsPerSegment();
				showFilled(segment, _colors[filled]);
				_hud.incrementFilled(filled);				
			}
		}
	}

	_bullets.erase(std::remove_if(_bullets.begin(), _bullets.end(),
		[](const Bullet& b) {
		float l = sqr_length(ds::vec2(512, 384) - b.pos);
		return l > SQR_BULLET_OUTER_RADIUS;
	}), _bullets.end());
}
