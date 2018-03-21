#pragma once
#include "..\lib\DataArray.h"
#include <ds_base_app.h>
#include "..\utils\TransformComponent.h"

struct AnimationData {
	float timer;
	float ttl;
	void* data;
};

typedef int (*AnimFunc)(transform*, AnimationData*, float, ds::EventStream*);

struct RotationYData {
	float angle;
	float direction;
};

int rotateY(transform* t, AnimationData* data, float dt, ds::EventStream* events);

int rotateX(transform* t, AnimationData* data, float dt, ds::EventStream* events);

int idleAnimation(transform* t, AnimationData* data, float dt, ds::EventStream* events);

struct AnimationEvent {

};

class AnimationManager {

	struct Animation {
		ID id;
		AnimationData data;
		AnimFunc function;
		transform* transform;
	};

public:
	AnimationManager() {}
	~AnimationManager() {}
	ID start(transform* t, AnimFunc func, void* data, float ttl);
	void stop(ID id);
	void tick(float dt, ds::EventStream* events);
private:
	ds::DataArray<Animation> _animations;
};