#pragma once
#include "..\lib\DataArray.h"
#include <ds_base_app.h>
#include "..\utils\TransformComponent.h"

struct AnimationData {
	ID id;
	ID oid;
	float timer;
	float ttl;
	void* data;
	transform_component* transform;
};

struct AnimationTypes {
	enum Enum {
		IDLE,
		ROTATE_X,
		ROTATE_Y
	};
};
struct AnimationEvent {
	ID oid;
	AnimationTypes::Enum type;
};

class Animation {

public:
	Animation() {}
	virtual ~Animation() {}
	virtual void tick(float dt, ds::EventStream* events) = 0;
	virtual void stop(ID oid) = 0;
};

struct RotationData : AnimationData {
	
	float angle;
	float direction;
};

class RotateYAnimation : public Animation {

public:
	RotateYAnimation() : Animation() {}
	virtual ~RotateYAnimation() {}
	ID start(ID oid, transform_component* t, float angle, float direction, float ttl);
	virtual void tick(float dt, ds::EventStream* events);
	virtual void stop(ID oid);
private:
	ID alreadyRunning(transform_component* t);
	ds::DataArray<RotationData> _data;
};

class RotateXAnimation : public Animation {

public:
	RotateXAnimation() : Animation() {}
	virtual ~RotateXAnimation() {}
	ID start(ID oid, transform_component* t, float angle, float direction, float ttl);
	virtual void tick(float dt, ds::EventStream* events);
	virtual void stop(ID oid);
private:
	ID alreadyRunning(transform_component* t);
	ds::DataArray<RotationData> _data;
};

class IdleAnimation : public Animation {

public:
	IdleAnimation() : Animation() {}
	virtual ~IdleAnimation() {}
	ID start(ID oid, transform_component* t, float ttl);
	virtual void tick(float dt, ds::EventStream* events);
	virtual void stop(ID oid);
private:
	ID alreadyRunning(transform_component* t);
	ds::DataArray<AnimationData> _data;
};


class AnimationManager {

public:
	AnimationManager();
	~AnimationManager();
	void stop(ID id);
	void tick(float dt, ds::EventStream* events);
	ID rotateY(ID oid, transform_component* t, float angle, float direction, float ttl);
	ID rotateX(ID oid, transform_component* t, float angle, float direction, float ttl);
	ID idle(ID oid, transform_component* t, float ttl);
	void stopAll(ID oid);
private:
	std::vector<Animation*> _animations;
};