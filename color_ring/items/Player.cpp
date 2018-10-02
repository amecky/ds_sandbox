#include "Player.h"
#include "..\common\math.h"
#include "..\common\common.h"

// -------------------------------------------------------
// reset
// -------------------------------------------------------
void Player::reset() {
	_id = sprites::add(*_env->spriteArray, ds::vec2(512, 384), ds::vec4(120, 50, 50, 50));
	//_id = sprites::add(*_env->spriteArray, ds::vec2(512, 384), ds::vec4(24, 239, 65, 65));
	_rotation = 0.0f;
	_dirIndicatorInnerId = sprites::add(*_env->spriteArray, ds::vec2(512, 384), ds::vec4(170, 0, 4, 12));
	_dirIndicatorOuterId = sprites::add(*_env->spriteArray, ds::vec2(512, 384), ds::vec4(170, 0, 4, 12));
}

// -------------------------------------------------------
// get rotation
// -------------------------------------------------------
float Player::getRotation() const {
	return _rotation;
}

// -------------------------------------------------------
// set color
// -------------------------------------------------------
void Player::setColor(const ds::Color& clr) {
	Sprite& sp = sprites::get(*_env->spriteArray, _id);
	sp.color = clr;
}

// -------------------------------------------------------
// tick
// -------------------------------------------------------
void Player::tick(float dt, float rasterizedAngle) {
	// update player
	ds::vec2 mp = ds::getMousePosition();
	_rotation = get_rotation(mp - ds::vec2(512, 384));
	Sprite& sp = sprites::get(*_env->spriteArray, _id);
	sp.rotation = _rotation;
	// update direction indicator
	ds::vec2 p = ds::vec2(cosf(rasterizedAngle), sinf(rasterizedAngle));
	Sprite& inner = sprites::get(*_env->spriteArray, _dirIndicatorInnerId);
	inner.position = ds::vec2(512, 384) + p * 278.0f;
	inner.rotation = rasterizedAngle;
	Sprite& outer = sprites::get(*_env->spriteArray, _dirIndicatorOuterId);
	outer.position = ds::vec2(512, 384) + p * 332.0f;
	outer.rotation = rasterizedAngle;

}