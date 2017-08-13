#pragma once
#include <diesel.h>
#include <SpriteBatchBuffer.h>
#include "DataArray.h"
#include "AABBox.h"
#include <vector>
#include "utils\DynamicPath.h"
#include "KeyFrameAnimation.h"

enum AnimationType {
	ANIM_JUMP,
	ANIM_ROTATION,
	ANIM_SQUEEZE,
	ANIM_MOVE_BY,
	ANIM_WIGGLE,
	ANIM_FOLLOW_TARGET,
	ANIM_SCALE_BY_PATH,
	ANIM_KEYFRAME
};

enum AnimationState {
	AS_STARTED,
	AS_FINISHED
};

// ---------------------------------------------------------------
// Event
// ---------------------------------------------------------------
struct Event {
	AnimationType type;
	AnimationState state;
	ID sprite;
};

// ---------------------------------------------------------------
// Event queue
// ---------------------------------------------------------------
struct EventQueue {

	int readIndex;
	int writeIndex;
	Event events[64];

	EventQueue() : readIndex(0), writeIndex(0) {}

	bool get(Event* e) {
		if (readIndex != writeIndex) {
			*e = events[readIndex++];
			if (readIndex >= 64) {
				readIndex = 0;
			}
			return true;
		}
		return false;
	}

	bool has_events() {
		return readIndex != writeIndex;
	}

	void push_event(const Event& e) {
		events[writeIndex++] = e;
		if (writeIndex >= 64) {
			writeIndex = 0;
		}
	}
};

// ---------------------------------------------------------------
// MySprite
// ---------------------------------------------------------------
struct MySprite : Sprite {

	ID id;
	ds::vec2 basePosition;
	ds::vec2 force;
	int group;
	AABBox box;
	int state;

	MySprite() : Sprite(), id(INVALID_ID) , force(0.0f) , group(-1) , state(0) , offset(0.0f) {
	}

	MySprite(ID id, const ds::vec2& position, const ds::vec4& textureRect)
		: Sprite({ position,textureRect,ds::vec2(1.0f),0.0f,ds::Color(255,255,255,255) }), id(id), force(0.0f) , group(-1), state(0) {
		box = AABBox(position, textureRect);
	}
	MySprite(ID id, const ds::vec2& position, const ds::vec4& textureRect, const ds::vec2& s)
		: Sprite({ position,textureRect,s,0.0f,ds::Color(255,255,255,255) }), id(id), force(0.0f), state(0) {
		box = AABBox(position, textureRect);
	}
	MySprite(ID id, const ds::vec2& position, const ds::vec4& textureRect, const ds::vec2& scaling, float rotation, const ds::Color& color)
		: Sprite({ position,textureRect,scaling,rotation,color }), id(id), force(0.0f), state(0) {
		box = AABBox(position, textureRect);
	}
};

struct JumpData {
	ds::vec2 startPos;
	float height;
};

struct RotateByData {
	float rotation;
	float angle;
	float step;
};

struct SqueezeData {
	ds::vec2 amplitude;
};

struct ScaleByPathData {
	ds::Vec2Path* path;
};

struct MoveByData {
	ds::vec2 velocity;
};

struct WiggleData {
	float angle;
	float frequency;
};

struct FollowData {
	ID target;
	float velocity;
};

struct KFData {
	KeyFrameAnimation* keyframe;
};

struct AnimationData {

	ID id;
	ID sprite;
	float ttl;
	float timer;
	AnimationType animationType;

	AnimationData() {}

	union {
		JumpData jump;
		RotateByData rotateBy;
		SqueezeData squeeze;
		ScaleByPathData scaleByPath;
		MoveByData moveBy;
		WiggleData wiggle;
		FollowData follow;
		KFData keyFrame;
	};

	//AnimationData() : id(INVALID_ID), sprite(INVALID_ID), ttl(0.0f), timer(0.0f) {}
	//AnimationData(ID sprite, float ttl) : id(INVALID_ID), sprite(sprite), ttl(ttl), timer(0.0f) {}
};

// ---------------------------------------------------------------
// BaseAnimation
// ---------------------------------------------------------------
class BaseAnimation {

public:
	BaseAnimation(DataArray<MySprite, 256>* sprites) : _sprites(sprites) {}
	virtual ~BaseAnimation() {}
	virtual void tick(float dt, EventQueue& queue) = 0;
	void stop(ID id) {
		ID dataID = findBySpriteID(id);
		if (dataID != INVALID_ID) {
			remove(dataID);
		}
	}	
	virtual void remove(ID dataID) = 0;
	void setBoundingRect(const ds::vec4& r) {
		_boundingRect = r;
	}	
	void start(ID id, const AnimationData& animData) {
		ID jid = _data.add();
		AnimationData& data = _data.get(jid);
		data.sprite = id;
		data.timer = 0.0f;
		data.ttl = animData.ttl;
		data.animationType = animData.animationType;
		switch (animData.animationType) {
			case ANIM_JUMP: data.jump = animData.jump; break;
			case ANIM_ROTATION: data.rotateBy = animData.rotateBy; break;
			case ANIM_SQUEEZE: data.squeeze = animData.squeeze; break;
			case ANIM_MOVE_BY: data.moveBy = animData.moveBy; break;
			case ANIM_WIGGLE: data.wiggle = animData.wiggle; break;
			case ANIM_FOLLOW_TARGET: data.follow = animData.follow; break;
			case ANIM_SCALE_BY_PATH: data.scaleByPath = animData.scaleByPath; break;
			case ANIM_KEYFRAME: data.keyFrame = animData.keyFrame; break;
		}
	}
protected:	
	ID findBySpriteID(ID id) {
		int i = 0;
		while (i < _data.numObjects) {
			if (_data.objects[i].sprite == id) {
				return _data.objects[i].id;
			}
			++i;
		}
		return INVALID_ID;
	}
	ds::vec4 _boundingRect;
	DataArray<MySprite, 256>* _sprites;
	DataArray<AnimationData, 256> _data;
};

// ---------------------------------------------------------------
// Jump
// ---------------------------------------------------------------
class JumpAnimation : public BaseAnimation {

public:
	JumpAnimation(DataArray<MySprite, 256>* sprites) : BaseAnimation(sprites) {}
	virtual ~JumpAnimation() {}
	void tick(float dt, EventQueue& queue);
	void remove(ID id);
};

// ---------------------------------------------------------------
// rotate
// ---------------------------------------------------------------
class RotationAnimation : public BaseAnimation {

public:
	RotationAnimation(DataArray<MySprite, 256>* sprites) : BaseAnimation(sprites) {}
	virtual ~RotationAnimation() {}
	void tick(float dt, EventQueue& queue);
	void remove(ID id);
};

// ---------------------------------------------------------------
// squeeze
// ---------------------------------------------------------------
class SqueezeAnimation : public BaseAnimation {

public:
	SqueezeAnimation(DataArray<MySprite, 256>* sprites) : BaseAnimation(sprites) {}
	virtual ~SqueezeAnimation() {}
	void tick(float dt, EventQueue& queue);
	void remove(ID id);
};

// ---------------------------------------------------------------
// squeeze
// ---------------------------------------------------------------
class ScaleByPathAnimation : public BaseAnimation {

public:
	ScaleByPathAnimation(DataArray<MySprite, 256>* sprites) : BaseAnimation(sprites) {}
	virtual ~ScaleByPathAnimation() {}
	void tick(float dt, EventQueue& queue);
	void remove(ID id);
};

// ---------------------------------------------------------------
// Move
// ---------------------------------------------------------------
class MoveByAnimation : public BaseAnimation {

public:
	MoveByAnimation(DataArray<MySprite, 256>* sprites) : BaseAnimation(sprites) {}
	virtual ~MoveByAnimation() {}
	void tick(float dt, EventQueue& queue);
	void remove(ID id);
};

// ---------------------------------------------------------------
// Wiggle
// ---------------------------------------------------------------
class WiggleAnimation : public BaseAnimation {

public:
	WiggleAnimation(DataArray<MySprite, 256>* sprites) : BaseAnimation(sprites) {}
	virtual ~WiggleAnimation() {}
	void tick(float dt, EventQueue& queue);
	void remove(ID id);
};

// ---------------------------------------------------------------
// Follow
// ---------------------------------------------------------------
class FollowAnimation : public BaseAnimation {

public:
	FollowAnimation(DataArray<MySprite, 256>* sprites) : BaseAnimation(sprites) {}
	virtual ~FollowAnimation() {}
	void tick(float dt, EventQueue& queue);
	void remove(ID id);
};

// ---------------------------------------------------------------
// Follow
// ---------------------------------------------------------------
class KFAnimation : public BaseAnimation {

public:
	KFAnimation(DataArray<MySprite, 256>* sprites) : BaseAnimation(sprites) {}
	virtual ~KFAnimation() {}
	void tick(float dt, EventQueue& queue);
	void remove(ID id);
};



static const int MAX_ANIMATIONS = 8;

struct Collision {

	ds::vec2 firstPos;
	ds::vec2 secondPos;
	ID firstID;
	ID secondID;
	//v2 norm;
	//float distance;

	bool containsID(ID id) const {
		if (firstID == id) {
			return true;
		}
		if (secondID == id) {
			return true;
		}
		return false;
	}
	/*
	const bool containsType(int type) const {
		if (firstType == type) {
			return true;
		}
		if (secondType == type) {
			return true;
		}
		return false;
	}

	ID getIDByType(int type) const {
		if (firstType == type) {
			return firstSID;
		}
		return secondSID;
	}
	*/
};
// ---------------------------------------------------------------
// World
// ---------------------------------------------------------------
class World {

public:
	World(SpriteBatchBuffer* buffer);
	~World();
	uint32_t numSprites() const {
		return _spriteArray.numObjects;
	}
	void render();
	ID add(const ds::vec2& pos, const ds::vec4& rect);
	void tick(float elapsed);
	MySprite& get(ID id) {
		return _spriteArray.get(id);
	}
	const MySprite& get(ID id) const {
		return _spriteArray.get(id);
	}
	void remove(ID id) {
		_spriteArray.remove(id);
	}
	ID findIntersection(const AABBox& box) const;
	int findIntersections(Collision* collisions, int max);
	void rotateBy(ID id, float angle, float ttl);
	void jump(ID id, float height, float ttl);
	void squeeze(ID id, const ds::vec2& amplitude, float ttl);
	void moveBy(ID id, const ds::vec2& velocity);
	void wiggle(ID id, float angle, float frequency);
	void follow(ID id, ID target, float velocity);
	void scaleByPath(ID id, ds::Vec2Path* path, float ttl);
	void animate(ID id, KeyFrameAnimation* animation);
	bool getEvent(Event* e) {
		return _queue.get(e);
	}
	void stopAll(ID id);
private:
	ds::vec4 _boundingRect;
	SpriteBatchBuffer* _buffer;
	DataArray<MySprite, 256> _spriteArray;	
	BaseAnimation* _animations[MAX_ANIMATIONS];
	EventQueue _queue;
};

