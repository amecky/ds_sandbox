#pragma once
#include <diesel.h>
#include <SpriteBatchBuffer.h>
#include "lib\DataArray.h"
#include "AABBox.h"
#include <vector>
#include "utils\DynamicPath.h"
#include "KeyFrameAnimation.h"

enum AnimationState {
	AS_STARTED,
	AS_FINISHED
};

// ---------------------------------------------------------------
// Event
// ---------------------------------------------------------------
struct Event {
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

struct SpriteSRT {
	ds::vec2 position;
	ds::vec2 scale;
	float rotation;

	SpriteSRT() : position(0.0f), scale(1.0f), rotation(0.0f) {}
};
// ---------------------------------------------------------------
// MySprite
// ---------------------------------------------------------------
struct MySprite : Sprite {

	ID id;
	ID parent;
	SpriteSRT basic;
	SpriteSRT animation;
	ds::vec2 force;
	ds::vec2 offset;
	int group;
	AABBox box;

	MySprite() : Sprite(), id(INVALID_ID) , parent(INVALID_ID), force(0.0f) , group(-1) , offset(0.0f) {
	}

	MySprite(ID id, const ds::vec2& position, const ds::vec4& textureRect)
		: Sprite({ position,textureRect,ds::vec2(1.0f),0.0f,ds::Color(255,255,255,255) }), id(id), parent(INVALID_ID), force(0.0f) , group(-1), offset(0.0f) {
		box = AABBox(position, textureRect);
		basic.position = position;
		animation.scale = ds::vec2(0.0f);
	}

	MySprite(ID id, ID parent, const ds::vec2& position, const ds::vec4& textureRect)
		: Sprite({ position,textureRect,ds::vec2(1.0f),0.0f,ds::Color(255,255,255,255) }), id(id), parent(parent), force(0.0f), group(-1), offset(0.0f) {
		box = AABBox(position, textureRect);
		basic.position = position;
		animation.scale = ds::vec2(0.0f);
	}

	MySprite(ID id, const ds::vec2& position, const ds::vec4& textureRect, const ds::vec2& s)
		: Sprite({ position,textureRect,s,0.0f,ds::Color(255,255,255,255) }), id(id), parent(INVALID_ID), force(0.0f), offset(0.0f) {
		box = AABBox(position, textureRect);
		basic.position = position;
		basic.scale = s;
		animation.scale = ds::vec2(0.0f);
	}
	MySprite(ID id, const ds::vec2& position, const ds::vec4& textureRect, const ds::vec2& scaling, float rotation, const ds::Color& color)
		: Sprite({ position,textureRect,scaling,rotation,color }), id(id), parent(INVALID_ID), force(0.0f), offset(0.0f) {
		box = AABBox(position, textureRect);
		basic.position = position;
		basic.scale = scaling;
		basic.rotation = rotation;
		animation.scale = ds::vec2(0.0f);
	}
};

struct AnimationData {

	ID id;
	ID parent;
	ID sprite;
	float ttl;
	float timer;
	KeyFrameAnimation* animation;
	int repeat;

	AnimationData() : id(INVALID_ID) , parent(INVALID_ID), sprite(INVALID_ID) , ttl(1.0f) , timer(0.0f) , animation(0) , repeat(0) {}
	
};

// ---------------------------------------------------------------
// BaseAnimation
// ---------------------------------------------------------------
class BaseAnimation {

public:
	BaseAnimation(ds::DataArray<MySprite, 256>* sprites) : _sprites(sprites) {}
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
		data.animation = animData.animation;
		data.repeat = animData.repeat;
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
	ds::DataArray<MySprite, 256>* _sprites;
	ds::DataArray<AnimationData, 256> _data;
};

// ---------------------------------------------------------------
// Follow
// ---------------------------------------------------------------
class KFAnimation : public BaseAnimation {

public:
	KFAnimation(ds::DataArray<MySprite, 256>* sprites) : BaseAnimation(sprites) {}
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
	ID add(const ds::vec2& pos, const ds::vec4& rect, ID parent = INVALID_ID);
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
	void animate(ID id, KeyFrameAnimation* animation, float ttl, int repeat = 0);
	bool getEvent(Event* e) {
		return _queue.get(e);
	}
	void stopAll(ID id);
private:
	ds::vec4 _boundingRect;
	SpriteBatchBuffer* _buffer;
	ds::DataArray<MySprite, 256> _spriteArray;	
	KFAnimation _animations;
	EventQueue _queue;
};

