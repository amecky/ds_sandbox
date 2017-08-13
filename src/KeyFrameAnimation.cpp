#include "KeyFrameAnimation.h"
#include "utils\tweening.h"
#include <Windows.h>
KeyFrameAnimation::KeyFrameAnimation() : _ttl(1.0f) , _numScalings(0) , _numRotations(0) , _numTranslations(0) {
}

KeyFrameAnimation::~KeyFrameAnimation() {
}

void KeyFrameAnimation::setTTL(float ttl) {
	_ttl = ttl;
}

void KeyFrameAnimation::addScaling(float start, const ds::vec2& scale, tweening::TweeningType tweeningType) {
	if (_numScalings < 32) {
		KeyFrameData& current = _scalings[_numScalings++];
		current.scaling = scale;
		current.start = start;
		current.tweening = tweeningType;
		current.type = KAT_SCALING;
	}
}

void KeyFrameAnimation::addRotation(float start, float angle, tweening::TweeningType tweeningType) {
	if (_numRotations < 32) {
		KeyFrameData& current = _rotations[_numRotations++];
		current.rotation = angle;
		current.start = start;
		current.tweening = tweeningType;
		current.type = KAT_ROTATION;
	}
}

void KeyFrameAnimation::addTranslation(float start, const ds::vec2& position, tweening::TweeningType tweeningType) {
	if (_numRotations < 32) {
		KeyFrameData& current = _translations[_numTranslations++];
		current.translation = position;
		current.start = start;
		current.tweening = tweeningType;
		current.type = KAT_TRANSLATION;
	}
}

void KeyFrameAnimation::get(float t, ds::vec2* scale, float* rotation, ds::vec2* translation) {
	char b[256];
	for (uint16_t i = 0; i < _numScalings - 1; ++i) {
		KeyFrameData& current = _scalings[i];
		KeyFrameData& next = _scalings[i + 1];
		if (t >= current.start && t <= next.start) {
			float nt = (t - current.start) / (next.start - current.start);
			*scale = tweening::interpolate(tweening::linear, current.scaling, next.scaling, nt, 1.0f);
			//sprintf_s(b,"t: %g s: %g %g c: %g %g n: %g %g\n", t, scale->x, scale->y, current.scaling.x, current.scaling.y, next.scaling.x, next.scaling.y);
			//OutputDebugString(b);
		}
	}
	for (uint16_t i = 0; i < _numRotations - 1; ++i) {
		KeyFrameData& current = _rotations[i];
		KeyFrameData& next = _rotations[i + 1];
		if (t >= current.start && t <= next.start) {
			float nt = (t - current.start) / (next.start - current.start);
			*rotation = tweening::interpolate(tweening::linear, current.rotation, next.rotation, nt, 1.0f);
		}
	}
	for (uint16_t i = 0; i < _numTranslations - 1; ++i) {
		KeyFrameData& current = _translations[i];
		KeyFrameData& next = _translations[i + 1];
		if (t >= current.start && t <= next.start) {
			float nt = (t - current.start) / (next.start - current.start);
			*translation = tweening::interpolate(tweening::linear, current.translation, next.translation, nt, 1.0f);
			sprintf_s(b,"t: %g s: %g %g c: %g %g n: %g %g\n", t, translation->x, translation->y, current.translation.x, current.translation.y, next.translation.x, next.translation.y);
			OutputDebugString(b);
		}
	}
}