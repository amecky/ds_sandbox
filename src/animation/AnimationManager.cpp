#include "AnimationManager.h"

AnimationManager::AnimationManager() {
	_animations.push_back(new IdleAnimation());	
	_animations.push_back(new RotateXAnimation());
	_animations.push_back(new RotateYAnimation());
}

AnimationManager::~AnimationManager() {
	for (size_t i = 0; i < _animations.size(); ++i) {
		delete _animations[i];
	}
}

void AnimationManager::stop(ID id) {
	for (size_t i = 0; i < _animations.size(); ++i) {
		//_animations[i]->stop(id);
	}
}

void AnimationManager::tick(float dt, ds::EventStream* events) {
	for (size_t i = 0; i < _animations.size(); ++i) {
		_animations[i]->tick(dt, events);
	}
}

void AnimationManager::stopAll(ID oid) {
	for (size_t i = 0; i < _animations.size(); ++i) {
		_animations[i]->stop(oid);
	}
}

ID AnimationManager::rotateY(ID oid, transform_component * t, float angle, float direction, float ttl) {
	RotateYAnimation* rya = (RotateYAnimation*)_animations[AnimationTypes::ROTATE_Y];
	return rya->start(oid, t, angle, direction, ttl);
}

ID AnimationManager::rotateX(ID oid, transform_component * t, float angle, float direction, float ttl) {
	RotateXAnimation* rxa = (RotateXAnimation*)_animations[AnimationTypes::ROTATE_X];
	return rxa->start(oid, t, angle, direction, ttl);
}


ID AnimationManager::idle(ID oid, transform_component* t, float ttl) {
	IdleAnimation* rya = (IdleAnimation*)_animations[AnimationTypes::IDLE];
	return rya->start(oid, t, ttl);
}

ID RotateYAnimation::start(ID oid, transform_component* t, float angle, float direction, float ttl) {
	DBG_LOG("=> starting RotateYAnimation - oid %d angle %3.2f direction %1.0f ttl %2.3f", oid, angle * 360.0f / ds::TWO_PI, direction, ttl);
	ID tmp = alreadyRunning(t);
	if (tmp != INVALID_ID) {
		DBG_LOG("=> RotateYAnimation - already running");
		return tmp;
	}
	ID id = _data.add();
	RotationData& rd = _data.get(id);
	rd.angle = angle;
	rd.transform = t;
	rd.direction = direction;
	rd.oid = oid;
	rd.timer = 0.0f;
	rd.ttl = ttl;
	return id;
}

ID RotateYAnimation::alreadyRunning(transform_component* t) {
	for (unsigned int i = 0; i < _data.numObjects; ++i) {
		AnimationData& rd = (AnimationData)_data.objects[i];
		if ((uintptr_t)rd.transform == (uintptr_t)t) {
			return rd.id;
		}
	}
	return INVALID_ID;
}

void RotateYAnimation::tick(float dt, ds::EventStream * events) {
	for (unsigned int i = 0; i < _data.numObjects; ++i) {
		RotationData& rd = _data.objects[i];
		rd.transform->rotation.y += rd.angle / rd.ttl * dt * rd.direction;
		rd.timer += dt;
		if (rd.timer > rd.ttl) {
			_data.remove(rd.id);
			AnimationEvent event;
			event.oid = rd.oid;
			event.type = AnimationTypes::ROTATE_Y;
			DBG_LOG("#> stopping RotateYAnimation - oid %d", rd.oid);
			events->add(100, &event, sizeof(AnimationEvent));
		}
	}
}

void RotateYAnimation::stop(ID oid) {
	for (unsigned int i = 0; i < _data.numObjects; ++i) {
		AnimationData& rd = _data.objects[i];
		if (rd.oid == oid) {
			_data.remove(rd.id);
		}
	}
}

ID IdleAnimation::start(ID oid, transform_component* t, float ttl) {
	DBG_LOG("=> starting IdleAnimation - oid %d ttl %2.3f", oid, ttl);
	ID tmp = alreadyRunning(t);
	if (tmp != INVALID_ID) {
		DBG_LOG("=> IdleAnimation - already running");
		return tmp;
	}
	ID id = _data.add();
	AnimationData& rd = _data.get(id);
	rd.transform = t;
	rd.oid = oid;
	rd.timer = 0.0f;
	rd.ttl = ttl;
	return id;
}

void IdleAnimation::tick(float dt, ds::EventStream * events) {
	for (unsigned int i = 0; i < _data.numObjects; ++i) {
		AnimationData& rd = _data.objects[i];
		rd.timer += dt;
		if (rd.timer > rd.ttl) {
			_data.remove(rd.id);
			AnimationEvent event;
			event.oid = rd.oid;
			event.type = AnimationTypes::IDLE;
			DBG_LOG("#> stopping IdleAnimation - oid %d", rd.oid);
			events->add(100, &event, sizeof(AnimationEvent));
		}
	}
}

void IdleAnimation::stop(ID oid) {
	for (unsigned int i = 0; i < _data.numObjects; ++i) {
		AnimationData& rd = _data.objects[i];
		if (rd.oid == oid) {
			_data.remove(rd.id);
		}
	}
}

ID IdleAnimation::alreadyRunning(transform_component* t) {
	for (unsigned int i = 0; i < _data.numObjects; ++i) {
		AnimationData& rd = _data.objects[i];
		if ((uintptr_t)rd.transform == (uintptr_t)t) {
			return rd.id;
		}
	}
	return INVALID_ID;
}

ID RotateXAnimation::start(ID oid, transform_component* t, float angle, float direction, float ttl) {
	DBG_LOG("=> starting RotateXAnimation - oid %d angle %3.2f direction %1.0f ttl %2.3f", oid, angle * 360.0f / ds::TWO_PI, direction, ttl);
	ID tmp = alreadyRunning(t);
	if (tmp != INVALID_ID) {
		DBG_LOG("=> RotateXAnimation - already running");
		return tmp;
	}
	ID id = _data.add();
	RotationData& rd = _data.get(id);
	rd.angle = angle;
	rd.transform = t;
	rd.direction = direction;
	rd.oid = oid;
	rd.timer = 0.0f;
	rd.ttl = ttl;
	return id;
}

ID RotateXAnimation::alreadyRunning(transform_component* t) {
	for (unsigned int i = 0; i < _data.numObjects; ++i) {
		AnimationData& rd = (AnimationData)_data.objects[i];
		if ((uintptr_t)rd.transform == (uintptr_t)t) {
			return rd.id;
		}
	}
	return INVALID_ID;
}

void RotateXAnimation::tick(float dt, ds::EventStream * events) {
	for (unsigned int i = 0; i < _data.numObjects; ++i) {
		RotationData& rd = _data.objects[i];
		rd.transform->rotation.x += rd.angle * dt * rd.direction;
		rd.timer += dt;
		if (rd.timer > rd.ttl && rd.ttl > 0.0f) {
			_data.remove(rd.id);
			AnimationEvent event;
			event.oid = rd.oid;
			event.type = AnimationTypes::ROTATE_X;
			DBG_LOG("#> stopping RotateXAnimation - oid %d", rd.oid);
			events->add(100, &event, sizeof(AnimationEvent));
		}
	}
}

void RotateXAnimation::stop(ID oid) {
	for (unsigned int i = 0; i < _data.numObjects; ++i) {
		AnimationData& rd = _data.objects[i];
		if (rd.oid == oid) {
			_data.remove(rd.id);
		}
	}
}