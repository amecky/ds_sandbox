#pragma once
#include <diesel.h>
#include "DataArray.h"
#include "utils\tweening.h"
#include <vector>
#include <SpriteBatchBuffer.h>

struct AnimationTimelineEntry {

	float start;
	tweening::TweeningType tweening;
	ds::vec2 value;

	AnimationTimelineEntry() {}

	AnimationTimelineEntry(float s, const float v) : start(s), tweening(tweening::linear), value(v) {}

	AnimationTimelineEntry(float s, const ds::vec2& v) : start(s), tweening(tweening::linear), value(v) {}

	AnimationTimelineEntry(float s, tweening::TweeningType t, const ds::vec2& v) : start(s), tweening(t), value(v) {}
};

struct AnimationTimeline {

	AnimationTimelineEntry* entries;
	uint16_t num;
	uint16_t capacity;

	AnimationTimeline() : entries(0), num(0), capacity(0) {}

	~AnimationTimeline() {
		if (entries != 0) {
			delete[] entries;
		}
	}

	void clear() {
		num = 0;
	}

	float getStepAt(uint16_t idx) const {
		return entries[idx].start;
	}

	void add(float start, float value, tweening::TweeningType type = tweening::linear) {
		add(start, ds::vec2(value), type);
	}

	void add(float start, const ds::vec2& value, tweening::TweeningType type = tweening::linear) {
		if (start >= 0.0f && start <= 1.0f) {
			if ((num + 1) > capacity) {
				alloc(capacity * 2 + 8);
			}
			int idx = -1;
			for (uint16_t i = 0; i < num; ++i) {
				if (entries[i].start > start && idx == -1) {
					idx = i;
				}
			}
			if (idx == -1) {
				entries[num++] = { start, type, value };
			}
			else {
				for (int i = num - 1; i >= idx; --i) {
					entries[i + 1] = entries[i];
					entries[idx] = { start, type, value };
				}
				++num;
			}
		}
	}

	bool get(float normTime, float* ret) const {
		ds::vec2 r(0.0f);
		if (get(normTime, &r)) {
			*ret = r.x;
			return true;
		}
		return false;
	}

	bool get(float normTime, ds::vec2* ret) const {
		if (num == 0) {
			return false;
		}
		if (normTime <= 0.0f) {
			*ret = entries[0].value;
			return true;
		}
		if (normTime >= 1.0f) {
			*ret = entries[num - 1].value;
			return true;
		}
		for (uint16_t i = 0; i < num - 1; ++i) {
			AnimationTimelineEntry& current = entries[i];
			AnimationTimelineEntry& next = entries[i + 1];
			if (normTime >= current.start && normTime <= next.start) {
				float nt = (normTime - current.start) / (next.start - current.start);
				*ret = tweening::interpolate(current.tweening, current.value, next.value, nt, 1.0f);
				return true;
			}
		}
		return false;
	}

	void alloc(int newCapacity) {
		AnimationTimelineEntry* tmp = new AnimationTimelineEntry[newCapacity];
		if (entries != 0) {
			memcpy(tmp, entries, num * sizeof(AnimationTimelineEntry));
			delete[] entries;
		}
		capacity = newCapacity;
		entries = tmp;
	}
};

struct KeyFrameAnimation {

	AnimationTimeline scalingTimeline;
	AnimationTimeline rotationTimeline;
	AnimationTimeline translationTimeline;

	KeyFrameAnimation() {}

	void addScaling(float start, const ds::vec2& scale, tweening::TweeningType tweeningType = tweening::linear) {
		scalingTimeline.add(start, scale, tweeningType);
	}

	void addRotation(float start, float angle, tweening::TweeningType tweeningType = tweening::linear) {
		rotationTimeline.add(start, angle, tweeningType);
	}

	void addTranslation(float start, const ds::vec2& position, tweening::TweeningType tweeningType = tweening::linear) {
		translationTimeline.add(start, position, tweeningType);
	}

	void clear() {
		scalingTimeline.clear();
		rotationTimeline.clear();
		translationTimeline.clear();
	}

	void get(float t, ds::vec2* scale, float* rotation, ds::vec2* translation) {
		scalingTimeline.get(t, scale);
		rotationTimeline.get(t, rotation);
		translationTimeline.get(t, translation);
	}
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

	AnimationPart& getPart(uint16_t idx) {
		return parts[idx];
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
				ds::vec2 delta = current.basePosition;
				float px = cos(current.finalRotation) * delta.x - sin(current.finalRotation) * delta.y + rot_point.x;
				float py = sin(current.finalRotation) * delta.x + cos(current.finalRotation) * delta.y + rot_point.y;

				current.finalPosition = ds::vec2(px,py) + current.animationPosition;
			}
			current.finalScale = current.baseScale + current.animationScale;
		}
	}

	void render(SpriteBatchBuffer* sprites) {
		for (size_t i = 0; i < parts.size(); ++i) {
			const AnimationPart& part = parts[i];
			sprites->add(part.finalPosition, part.textureRect, part.finalScale, part.finalRotation);
		}
	}
};

namespace animation {

	bool load_text(const char* name, KeyFrameAnimation* animation);

	bool save_text(const char* name, KeyFrameAnimation* animation);

	bool load_animation_object(const char* name, AnimationObject* object);
}