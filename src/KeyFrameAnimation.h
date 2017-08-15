#pragma once
#include <diesel.h>
#include "DataArray.h"
#include "utils\tweening.h"
#include <vector>
#include <SpriteBatchBuffer.h>
// ---------------------------------------------------------------
// Animation type
// ---------------------------------------------------------------
enum KeyAnimationtype {
	KAT_SCALING,
	KAT_ROTATION,
	KAT_TRANSLATION,
	KAT_NONE
};

// ---------------------------------------------------------------
// Keyframe data
// ---------------------------------------------------------------
struct KeyFrameData {
	float start;
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

// ---------------------------------------------------------------
// Keyframe animation
// ---------------------------------------------------------------
class KeyFrameAnimation {

public:
	KeyFrameAnimation();
	
	~KeyFrameAnimation();

	void addScaling(float start, const ds::vec2& scale, tweening::TweeningType = tweening::linear);
	
	void addRotation(float start, float angle, tweening::TweeningType = tweening::linear);
	
	void addTranslation(float start, const ds::vec2& position, tweening::TweeningType = tweening::linear);
	
	void get(float t, ds::vec2* scale, float* rotation, ds::vec2* translation);

	KeyFrameData* getData(KeyAnimationtype type) {
		return &_data[type * 32];
	}

	uint16_t getNum(KeyAnimationtype type) {
		return _num[type];
	}
private:
	KeyFrameData _data[96];
	Bone _bones[128];
	uint16_t _numBones;
	uint16_t _num[3];
};

struct AnimationPart {
	uint16_t id;
	uint16_t parent;
	ds::vec4 textureRect;
	ds::vec2 basePosition;
	ds::vec2 baseScale;	
	float baseRotation;
	ds::vec2 offset;
	ds::vec2 animationPosition;
	ds::vec2 animationScale;
	float animationRotation;
	ds::vec2 finalPosition;
	ds::vec2 finalScale;
	float finalRotation;
	KeyFrameAnimation* animation;
	float timer;
	float ttl;
	int repeat;
};

struct AnimationObject {

	std::vector<AnimationPart> parts;

	uint16_t add(const ds::vec2& position, const ds::vec4& rect, uint16_t parent = UINT16_MAX) {
		AnimationPart part;
		part.parent = parent;
		part.textureRect = rect;
		part.basePosition = position;
		part.baseScale = ds::vec2(1.0f);
		part.baseRotation = 0.0f;
		part.offset = ds::vec2(0.0f);
		part.finalPosition = position;
		part.finalScale = ds::vec2(1.0f);
		part.finalRotation = 0.0f;
		part.animationPosition = ds::vec2(0.0f);
		part.animationScale = ds::vec2(0.0f);
		part.animationRotation = 0.0f;
		part.animation = 0;
		part.timer = 0.0f;
		part.ttl = 1.0f;
		part.repeat = 0;
		parts.push_back(part);
		return parts.size() - 1;
	}

	void start(uint16_t id, KeyFrameAnimation* anim, float ttl) {
		AnimationPart& part = parts[id];
		part.animation = anim;
		part.timer = 0.0f;
		part.ttl = ttl;
	}

	void tick(float dt) {
		for (size_t i = 0; i < parts.size(); ++i) {
			AnimationPart& part = parts[i];
			if (part.animation != 0) {
				float norm = part.timer / part.ttl;
				part.animation->get(norm, &part.animationScale, &part.animationRotation, &part.animationPosition);
				part.timer += dt;
				if (part.timer > part.ttl) {
					if (part.repeat > 0) {
						--part.repeat;
						part.timer = 0.0f;
					}
					else if (part.repeat < 0) {
						part.timer = 0.0f;
						++i;
					}
					else {
						part.animation->get(1.0f, &part.animationScale, &part.animationRotation, &part.animationPosition);
						part.animation = 0;
					}
				}
			}
		}
		for (size_t i = 0; i < parts.size(); ++i) {
			AnimationPart& current = parts[i];
			if (current.parent == UINT16_MAX) {
				current.finalRotation = current.baseRotation + current.animationRotation;
			}
			else {
				const AnimationPart& parent = parts[current.parent];
				current.finalRotation = current.baseRotation + current.animationRotation + parent.finalRotation;
			}
			if (current.parent == UINT16_MAX) {
				if (sqr_length(current.offset) != 0.0f && current.finalRotation != 0.0f) {
					ds::vec2 rot_point = current.basePosition + current.offset;
					ds::vec2 delta = current.basePosition - rot_point;
					float px = cos(current.finalRotation) * delta.x - sin(current.finalRotation) * delta.y + rot_point.x;
					float py = sin(current.finalRotation) * delta.x + cos(current.finalRotation) * delta.y + rot_point.y;
					current.finalPosition = ds::vec2(px, py) + current.animationPosition;
				}
				else {
					current.finalPosition = current.basePosition + current.animationPosition;
				}
			}
			else {
				const AnimationPart& parent = parts[current.parent];
				
				ds::vec2 rot_point = parent.finalPosition;
				ds::vec2 delta = current.basePosition;// -rot_point;
				float px = cos(current.finalRotation) * delta.x - sin(current.finalRotation) * delta.y + rot_point.x;
				float py = sin(current.finalRotation) * delta.x + cos(current.finalRotation) * delta.y + rot_point.y;

				current.finalPosition = ds::vec2(px,py) + current.animationPosition;
			}
			current.finalScale = current.baseScale + current.animationScale;
			//current.box.position = current.position;
		}
	}

	void render(SpriteBatchBuffer* sprites) {
		for (size_t i = 0; i < parts.size(); ++i) {
			const AnimationPart& part = parts[i];
			sprites->add(part.finalPosition, part.textureRect, part.finalScale, part.finalRotation);
		}
	}
};