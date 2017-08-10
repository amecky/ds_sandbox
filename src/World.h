#pragma once
#include <diesel.h>
#include <SpriteBatchBuffer.h>
#include "DataArray.h"
#include "AABBox.h"
#include <vector>

enum AnimationType {
	ANIM_JUMP,
	ANIM_ROTATION,
	ANIM_SQUEEZE,
	ANIM_MOVE_BY
};

enum AnimationState {
	AS_STARTED,
	AS_FINISHED
};

struct Event {
	AnimationType type;
	AnimationState state;
	ID sprite;
};

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

	MySprite() : Sprite(), id(INVALID_ID) {}
	MySprite(ID id, const ds::vec2& position, const ds::vec4& textureRect)
		: Sprite({ position,textureRect,ds::vec2(1.0f),0.0f,ds::Color(255,255,255,255) }), id(id) {
	}
	MySprite(ID id, const ds::vec2& position, const ds::vec4& textureRect, const ds::vec2& s)
		: Sprite({ position,textureRect,s,0.0f,ds::Color(255,255,255,255) }), id(id) {
	}
	MySprite(ID id, const ds::vec2& position, const ds::vec4& textureRect, const ds::vec2& scaling, float rotation, const ds::Color& color)
		: Sprite({ position,textureRect,scaling,rotation,color }), id(id) {
	}
};

// ---------------------------------------------------------------
// BaseAnimation
// ---------------------------------------------------------------
class BaseAnimation {

public:
	BaseAnimation(DataArray<MySprite, 256>* sprites) : _sprites(sprites) {}
	virtual ~BaseAnimation() {}
	virtual void tick(float dt, EventQueue& queue) = 0;
	virtual void stop(ID id) = 0;
	virtual void remove(ID dataID) = 0;
protected:
	DataArray<MySprite, 256>* _sprites;
};

// ---------------------------------------------------------------
// AbstractAnimation
// ---------------------------------------------------------------
template<class T>
class AbstractAnimation : public BaseAnimation {

public:
	AbstractAnimation(DataArray<MySprite, 256>* sprites) : BaseAnimation(sprites) {}
	virtual ~AbstractAnimation() {}
	void stop(ID id) {
		ID dataID = findBySpriteID(id);
		if (dataID != INVALID_ID) {
			remove(dataID);
		}
	}
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
protected:
	DataArray<T, 256> _data;
};

// ---------------------------------------------------------------
// Jump
// ---------------------------------------------------------------
struct JumpData {
	ID id;
	ID sprite;
	ds::vec2 startPos;
	float height;
	float ttl;
	float timer;
};

class JumpAnimation : public AbstractAnimation<JumpData>{

public:
	JumpAnimation(DataArray<MySprite, 256>* sprites) : AbstractAnimation(sprites) {}
	virtual ~JumpAnimation() {}
	void tick(float dt, EventQueue& queue);
	void start(ID id, float height, float ttl);
private:
	void remove(ID id);
};

// ---------------------------------------------------------------
// rotate
// ---------------------------------------------------------------
struct RotateByData {
	ID id;
	ID sprite;
	float rotation;
	float angle;
	float step;
	float timer;
	float ttl;
};
class RotationAnimation : public AbstractAnimation<RotateByData> {

public:
	RotationAnimation(DataArray<MySprite, 256>* sprites) : AbstractAnimation(sprites) {}
	virtual ~RotationAnimation() {}
	void tick(float dt, EventQueue& queue);
	void start(ID id, float angle, float ttl);
private:
	void remove(ID id);
};

// ---------------------------------------------------------------
// squeeze
// ---------------------------------------------------------------
struct SqueezeData {
	ID id;
	ID sprite;
	ds::vec2 amplitude;
	float timer;
	float ttl;
};
class SqueezeAnimation : public AbstractAnimation<SqueezeData> {

public:
	SqueezeAnimation(DataArray<MySprite, 256>* sprites) : AbstractAnimation(sprites) {}
	virtual ~SqueezeAnimation() {}
	void tick(float dt, EventQueue& queue);
	void start(ID id, const ds::vec2& amplitude, float ttl);
private:
	void remove(ID id);
};

// ---------------------------------------------------------------
// Move
// ---------------------------------------------------------------
struct MoveByData {
	ID id;
	ID sprite;
	ds::vec2 velocity;
};
class MoveByAnimation : public AbstractAnimation<MoveByData> {

public:
	MoveByAnimation(DataArray<MySprite, 256>* sprites) : AbstractAnimation(sprites) {}
	virtual ~MoveByAnimation() {}
	void tick(float dt, EventQueue& queue);
	void start(ID id, const ds::vec2& velocity);
private:
	void remove(ID id);
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
	void rotateBy(ID id, float angle, float ttl);
	void jump(ID id, float height, float ttl);
	void squeeze(ID id, const ds::vec2& amplitude, float ttl);
	void moveBy(ID id, const ds::vec2& velocity);
	bool getEvent(Event* e) {
		return _queue.get(e);
	}
	void stopAll(ID id);
private:
	SpriteBatchBuffer* _buffer;
	DataArray<MySprite, 256> _spriteArray;	
	BaseAnimation* _animations[4];
	EventQueue _queue;
};

