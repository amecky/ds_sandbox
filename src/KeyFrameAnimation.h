#pragma once
#include <diesel.h>
#include "DataArray.h"
#include "utils\tweening.h"

enum KeyAnimationtype {
	KAT_SCALING,
	KAT_ROTATION,
	KAT_TRANSLATION
};

struct KeyFrameData {
	ID parent;
	float start;
	KeyAnimationtype type;
	tweening::TweeningType tweening;
	union {
		ds::vec2 scaling;
		float rotation;
		ds::vec2 translation;
	};

	KeyFrameData() {}
};

class KeyFrameAnimation {

public:
	KeyFrameAnimation();
	~KeyFrameAnimation();
	void setTTL(float ttl);
	float getTTL() const {
		return _ttl;
	}
	void addScaling(float start, const ds::vec2& scale, tweening::TweeningType = tweening::linear);
	void addRotation(float start, float angle, tweening::TweeningType = tweening::linear);
	void addTranslation(float start, const ds::vec2& position, tweening::TweeningType = tweening::linear);
	void get(float t, ds::vec2* scale, float* rotation, ds::vec2* translation);
private:
	float _ttl;
	KeyFrameData _scalings[32];
	uint16_t _numScalings;
	KeyFrameData _rotations[32];
	uint16_t _numRotations;
	KeyFrameData _translations[32];
	uint16_t _numTranslations;

};

