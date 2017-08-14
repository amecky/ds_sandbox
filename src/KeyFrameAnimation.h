#pragma once
#include <diesel.h>
#include "DataArray.h"
#include "utils\tweening.h"

enum KeyAnimationtype {
	KAT_SCALING,
	KAT_ROTATION,
	KAT_TRANSLATION,
	KAT_NONE
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

struct Bone {
	ID parent;
	ID child;
	float scale;
	float angle;
};

class KeyFrameAnimation {

public:
	KeyFrameAnimation();
	~KeyFrameAnimation();
	void addScaling(float start, const ds::vec2& scale, tweening::TweeningType = tweening::linear);
	void addRotation(float start, float angle, tweening::TweeningType = tweening::linear);
	void addTranslation(float start, const ds::vec2& position, tweening::TweeningType = tweening::linear);
	void get(float t, ds::vec2* scale, float* rotation, ds::vec2* translation);
	KeyFrameData* getScalingData() {
		return &_scalings[0];
	}

	KeyFrameData* getData(KeyAnimationtype type) {
		return &_data[_indices[type]];
	}

	uint16_t getNum(KeyAnimationtype type) {
		return _num[type];
	}

	uint16_t getNumScalingData() const {
		return _num[KAT_SCALING];
	}
	KeyFrameData* getTranslationData() {
		return &_translations[0];
	}
	uint16_t getNumTranslationData() const {
		return _num[KAT_TRANSLATION];
	}
	KeyFrameData* getRotationData() {
		return &_rotations[0];
	}
	uint16_t getNumRotationData() const {
		return _num[KAT_ROTATION];
	}
private:
	KeyFrameData _scalings[32];
	KeyFrameData _rotations[32];
	KeyFrameData _translations[32];
	KeyFrameData _data[256];
	Bone _bones[128];
	uint16_t _numBones;
	uint16_t _num[3];
	uint16_t _indices[3];
	uint16_t _capacity;

};

