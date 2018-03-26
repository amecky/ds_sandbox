#pragma once
#include "..\lib\DataArray.h"
#include <ds_base_app.h>
#include <ds_tweening.h>
#include "..\utils\TransformComponent.h"

// ----------------------------------------------------
// Animation data
// ----------------------------------------------------
struct AnimationData {
	ID id;
	ID oid;
	float timer;
	float ttl;
	transform_component* transform;
	tweening::TweeningType tweeningType;
};

// ----------------------------------------------------
// Animation types
// ----------------------------------------------------
struct AnimationTypes {
	enum Enum {
		IDLE,
		ROTATE_X,
		ROTATE_Y,
		ROTATE_Z,
		MOVE_TO,
		SCALE_TO
	};
};

// ----------------------------------------------------
// Animation event
// ----------------------------------------------------
struct AnimationEvent {
	ID oid;
	AnimationTypes::Enum type;
};

// ----------------------------------------------------
// Animation
// ----------------------------------------------------
class Animation {

public:
	Animation() {}
	virtual ~Animation() {}
	virtual void tick(float dt, ds::EventStream* events) = 0;
	virtual ID start(ID oid, transform_component* t, void* data, float ttl, tweening::TweeningType type = tweening::linear) = 0;
	virtual void stop(ID oid) = 0;
protected:
	void fillBasicData(AnimationData* baseData, ID oid, transform_component* t, float ttl, tweening::TweeningType type = tweening::linear) {
		baseData->oid = oid;
		baseData->transform = t;
		baseData->tweeningType = type;
		baseData->ttl = ttl;
		baseData->timer = 0.0f;
	}
};

// ----------------------------------------------------
// Rotation data
// ----------------------------------------------------
struct RotationData : AnimationData {
	
	float angle;
	float direction;
};

// ----------------------------------------------------
// rotate Y
// ----------------------------------------------------
class RotateYAnimation : public Animation {

public:
	RotateYAnimation() : Animation() {}
	virtual ~RotateYAnimation() {}
	ID start(ID oid, transform_component* t, void* data, float ttl, tweening::TweeningType type = tweening::linear);
	virtual void tick(float dt, ds::EventStream* events);
	virtual void stop(ID oid);
private:
	ID alreadyRunning(transform_component* t);
	ds::DataArray<RotationData> _data;
};

// ----------------------------------------------------
// rotate z
// ----------------------------------------------------
class RotateZAnimation : public Animation {

public:
	RotateZAnimation() : Animation() {}
	virtual ~RotateZAnimation() {}
	ID start(ID oid, transform_component* t, void* data, float ttl, tweening::TweeningType type = tweening::linear);
	virtual void tick(float dt, ds::EventStream* events);
	virtual void stop(ID oid);
private:
	ID alreadyRunning(transform_component* t);
	ds::DataArray<RotationData> _data;
};

// ----------------------------------------------------
// rotate X
// ----------------------------------------------------
class RotateXAnimation : public Animation {

public:
	RotateXAnimation() : Animation() {}
	virtual ~RotateXAnimation() {}
	ID start(ID oid, transform_component* t, void* data, float ttl, tweening::TweeningType type = tweening::linear);
	virtual void tick(float dt, ds::EventStream* events);
	virtual void stop(ID oid);
private:
	ID alreadyRunning(transform_component* t);
	ds::DataArray<RotationData> _data;
};

// ----------------------------------------------------
// idle
// ----------------------------------------------------
class IdleAnimation : public Animation {

public:
	IdleAnimation() : Animation() {}
	virtual ~IdleAnimation() {}
	ID start(ID oid, transform_component* t, void* data, float ttl, tweening::TweeningType type = tweening::linear);
	virtual void tick(float dt, ds::EventStream* events);
	virtual void stop(ID oid);
private:
	ID alreadyRunning(transform_component* t);
	ds::DataArray<AnimationData> _data;
};

// ----------------------------------------------------
// move to data
// ----------------------------------------------------
struct MoveToData : AnimationData {
	ds::vec3 start;
	ds::vec3 end;
};

// ----------------------------------------------------
// move to
// ----------------------------------------------------
class MoveToAnimation : public Animation {

public:
	MoveToAnimation() : Animation() {}
	virtual ~MoveToAnimation() {}
	ID start(ID oid, transform_component* t, void* data, float ttl, tweening::TweeningType type = tweening::linear);
	virtual void tick(float dt, ds::EventStream* events);
	virtual void stop(ID oid);
private:
	ID alreadyRunning(transform_component* t);
	ds::DataArray<MoveToData> _data;
};

// ----------------------------------------------------
// scale to data
// ----------------------------------------------------
struct ScaleToData : AnimationData {
	ds::vec3 start;
	ds::vec3 end;
};

// ----------------------------------------------------
// move to
// ----------------------------------------------------
class ScaleToAnimation : public Animation {

public:
	ScaleToAnimation() : Animation() {}
	virtual ~ScaleToAnimation() {}
	ID start(ID oid, transform_component* t, void* data, float ttl, tweening::TweeningType type = tweening::linear);
	virtual void tick(float dt, ds::EventStream* events);
	virtual void stop(ID oid);
private:
	ID alreadyRunning(transform_component* t);
	ds::DataArray<ScaleToData> _data;
};


// ----------------------------------------------------
// Animation manager
// ----------------------------------------------------
class AnimationManager {

public:
	AnimationManager();
	~AnimationManager();
	void stop(AnimationTypes::Enum animationType, ID id);
	void tick(float dt, ds::EventStream* events);
	ID start(ID oid, transform_component* t, AnimationTypes::Enum animationType, void* data, float ttl, tweening::TweeningType type = tweening::linear);
	void stopAll(ID oid);
private:
	std::vector<Animation*> _animations;
};