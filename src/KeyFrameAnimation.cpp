#include "KeyFrameAnimation.h"
#include "utils\tweening.h"
//#include <Windows.h>
KeyFrameAnimation::KeyFrameAnimation() {
	for (int i = 0; i < 3; ++i) {
		_num[i] = 0;
		_indices[i] = 0;
	}
	_capacity = 0;
	_numBones = 0;
}

KeyFrameAnimation::~KeyFrameAnimation() {
}

void KeyFrameAnimation::addScaling(float start, const ds::vec2& scale, tweening::TweeningType tweeningType) {
	if (_num[KAT_SCALING] < 32) {
		KeyFrameData& current = _scalings[_num[KAT_SCALING]++];
		current.scaling = scale;
		current.start = start;
		current.tweening = tweeningType;
		current.type = KAT_SCALING;
	}
}

void KeyFrameAnimation::addRotation(float start, float angle, tweening::TweeningType tweeningType) {
	if (_num[KAT_ROTATION] < 32) {
		KeyFrameData& current = _rotations[_num[KAT_ROTATION]++];
		current.rotation = angle;
		current.start = start;
		current.tweening = tweeningType;
		current.type = KAT_ROTATION;
	}
}

void KeyFrameAnimation::addTranslation(float start, const ds::vec2& position, tweening::TweeningType tweeningType) {
	if (_num[KAT_TRANSLATION] < 32) {
		KeyFrameData& current = _translations[_num[KAT_TRANSLATION]++];
		current.translation = position;
		current.start = start;
		current.tweening = tweeningType;
		current.type = KAT_TRANSLATION;
	}
}

void KeyFrameAnimation::get(float t, ds::vec2* scale, float* rotation, ds::vec2* translation) {
	for (uint16_t i = 0; i < _num[KAT_SCALING] - 1; ++i) {
		KeyFrameData& current = _scalings[i];
		KeyFrameData& next = _scalings[i + 1];
		if (t >= current.start && t <= next.start) {
			float nt = (t - current.start) / (next.start - current.start);
			*scale = tweening::interpolate(tweening::linear, current.scaling, next.scaling, nt, 1.0f);
		}
	}
	for (uint16_t i = 0; i < _num[KAT_ROTATION] - 1; ++i) {
		KeyFrameData& current = _rotations[i];
		KeyFrameData& next = _rotations[i + 1];
		if (t >= current.start && t <= next.start) {
			float nt = (t - current.start) / (next.start - current.start);
			*rotation = tweening::interpolate(tweening::linear, current.rotation, next.rotation, nt, 1.0f);
		}
	}
	for (uint16_t i = 0; i < _num[KAT_TRANSLATION] - 1; ++i) {
		KeyFrameData& current = _translations[i];
		KeyFrameData& next = _translations[i + 1];
		if (t >= current.start && t <= next.start) {
			float nt = (t - current.start) / (next.start - current.start);
			*translation = tweening::interpolate(tweening::linear, current.translation, next.translation, nt, 1.0f);
		}
	}
}