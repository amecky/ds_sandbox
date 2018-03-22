#include "AnimationManager.h"

int rotateY(transform_component* t, AnimationData* data, float dt, ds::EventStream* events) {
	data->timer += dt;
	RotationYData* internal = (RotationYData*)data;
	t->rotation.y += internal->angle / data->ttl * dt * internal->direction;
	return 1000;
}

int rotateX(transform_component* t, AnimationData* data, float dt, ds::EventStream* events) {
	data->timer += dt;
	RotationYData* internal = (RotationYData*)data;
	t->rotation.x += internal->angle / data->ttl * dt * internal->direction;
	return 1000;
}

int idleAnimation(transform_component* t, AnimationData* data, float dt, ds::EventStream* events) {
	data->timer += dt;
	return 1001;
}

ID AnimationManager::start(transform_component* t, AnimFunc func, void* data, float ttl) {
	ID id = _animations.add();
	DBG_LOG("starting animation %d -> %1.3f", id, ttl);
	Animation& anim = _animations.get(id);
	anim.function = func;
	anim.data.timer = 0.0f;
	anim.data.ttl = ttl;
	anim.data.data = data;
	anim.transform = t;
	return id;
}

void AnimationManager::stop(ID id) {
	_animations.remove(id);
}

// typedef bool (*AnimFunc)(transform*, AnimationData*, float, ds::EventStream* events);
void AnimationManager::tick(float dt, ds::EventStream* events) {
	for (int i = 0; i < _animations.numObjects; ++i) {
		Animation& animData = _animations.objects[i];
		int ret = (animData.function)(animData.transform,&animData.data,dt, events);
		if (animData.data.ttl > 0.0f) {
			if (animData.data.timer >= animData.data.ttl) {
				DBG_LOG("animation stopped - %d - event %d", animData.id, ret);
				events->add(ret);
				if (animData.data.data != 0) {
					DBG_LOG("deleting data");
					delete animData.data.data;
				}
				_animations.remove(animData.id);
			}
		}
		//if (!ret) {
			//_animations.remove(animData.id);
		//}
	}
}