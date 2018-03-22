#pragma once
#include "..\lib\DataArray.h"
#include <ds_base_app.h>
#include "..\utils\TransformComponent.h"

struct AnimationData {
	float timer;
	float ttl;
	void* data;
};

typedef int (*AnimFunc)(transform_component*, AnimationData*, float, ds::EventStream*);

struct RotationYData {
	float angle;
	float direction;
};

int rotateY(transform_component* t, AnimationData* data, float dt, ds::EventStream* events);

int rotateX(transform_component* t, AnimationData* data, float dt, ds::EventStream* events);

int rotateZ(transform_component* t, AnimationData* data, float dt, ds::EventStream* events);

int idleAnimation(transform_component* t, AnimationData* data, float dt, ds::EventStream* events);

struct AnimationEvent {

};

class AnimationManager {

	struct Animation {
		ID id;
		AnimationData data;
		AnimFunc function;
		transform_component* transform;
	};

public:
	AnimationManager() {}
	~AnimationManager() {}
	ID start(transform_component* t, AnimFunc func, void* data, float ttl);
	void stop(ID id);
	void tick(float dt, ds::EventStream* events);
private:
	ds::DataArray<Animation> _animations;
};