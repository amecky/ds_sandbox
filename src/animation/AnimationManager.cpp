#include "AnimationManager.h"

// ----------------------------------------------------
// AnimationManager
// ----------------------------------------------------
AnimationManager::AnimationManager() {
	_animations.push_back(new IdleAnimation());	
	_animations.push_back(new RotateXAnimation());
	_animations.push_back(new RotateYAnimation());
	_animations.push_back(new RotateZAnimation());
	_animations.push_back(new MoveToAnimation());
	_animations.push_back(new ScaleToAnimation());
}

AnimationManager::~AnimationManager() {
	for (size_t i = 0; i < _animations.size(); ++i) {
		delete _animations[i];
	}
}

void AnimationManager::stop(AnimationTypes::Enum animationType, ID id) {
	_animations[animationType]->stop(id);
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

ID AnimationManager::start(ID oid, transform_component* t, AnimationTypes::Enum animationType, void* data, float ttl, tweening::TweeningType type) {
	Animation* animation = _animations[animationType];
	return animation->start(oid, t, data, ttl, type);
}

// ----------------------------------------------------
// RotateYAnimation
// ----------------------------------------------------
ID RotateYAnimation::start(ID oid, transform_component* t, void* data, float ttl, tweening::TweeningType type) {
	RotationData* rotData = (RotationData*)data;
	DBG_LOG("=> starting RotateYAnimation - oid %d angle %3.2f direction %1.0f ttl %2.3f",oid, rotData->angle * 360.0f / ds::TWO_PI, rotData->direction, ttl);
	ID tmp = alreadyRunning(t);
	if (tmp != INVALID_ID) {
		DBG_LOG("=> RotateYAnimation - already running");
		return tmp;
	}
	ID id = _data.add();
	RotationData& rd = _data.get(id);
	fillBasicData(&rd, oid, t, ttl, type);
	rd.angle = rotData->angle;
	rd.direction = rotData->direction;
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

// ----------------------------------------------------
// Idle animation
// ----------------------------------------------------
ID IdleAnimation::start(ID oid, transform_component* t, void* data, float ttl, tweening::TweeningType type) {
	AnimationData* rotData = (AnimationData*)data;
	DBG_LOG("=> starting IdleAnimation - oid %d ttl %2.3f", oid, ttl);
	ID tmp = alreadyRunning(t);
	if (tmp != INVALID_ID) {
		DBG_LOG("=> IdleAnimation - already running");
		return tmp;
	}
	ID id = _data.add();
	AnimationData& rd = _data.get(id);
	fillBasicData(&rd, oid, t, ttl, type);
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

// ----------------------------------------------------
// Rotate X animation
// ----------------------------------------------------
ID RotateXAnimation::start(ID oid, transform_component* t, void* data, float ttl, tweening::TweeningType type) {
	RotationData* rotData = (RotationData*)data;
	DBG_LOG("=> starting RotateXAnimation - oid %d angle %3.2f direction %1.0f ttl %2.3f", oid, rotData->angle * 360.0f / ds::TWO_PI, rotData->direction, ttl);
	ID tmp = alreadyRunning(t);
	if (tmp != INVALID_ID) {
		DBG_LOG("=> RotateXAnimation - already running");
		return tmp;
	}
	ID id = _data.add();
	RotationData& rd = _data.get(id);
	fillBasicData(&rd, oid, t, ttl, type);
	rd.angle = rotData->angle;
	rd.direction = rotData->direction;
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

// ----------------------------------------------------
// Rotate Z animation
// ----------------------------------------------------
ID RotateZAnimation::start(ID oid, transform_component* t, void* data, float ttl, tweening::TweeningType type) {
	RotationData* rotData = (RotationData*)data;
	DBG_LOG("=> starting RotateZAnimation - oid %d angle %3.2f direction %1.0f ttl %2.3f", oid, rotData->angle * 360.0f / ds::TWO_PI, rotData->direction, ttl);
	ID tmp = alreadyRunning(t);
	if (tmp != INVALID_ID) {
		DBG_LOG("=> RotateZAnimation - already running");
		return tmp;
	}
	ID id = _data.add();
	RotationData& rd = _data.get(id);
	fillBasicData(&rd, oid, t, ttl, type);
	rd.angle = rotData->angle;
	rd.direction = rotData->direction;
	return id;
}

ID RotateZAnimation::alreadyRunning(transform_component* t) {
	for (unsigned int i = 0; i < _data.numObjects; ++i) {
		AnimationData& rd = (AnimationData)_data.objects[i];
		if ((uintptr_t)rd.transform == (uintptr_t)t) {
			return rd.id;
		}
	}
	return INVALID_ID;
}

void RotateZAnimation::tick(float dt, ds::EventStream * events) {
	for (unsigned int i = 0; i < _data.numObjects; ++i) {
		RotationData& rd = _data.objects[i];
		rd.transform->rotation.z += rd.angle * dt * rd.direction;
		rd.timer += dt;
		if (rd.timer > rd.ttl && rd.ttl > 0.0f) {
			_data.remove(rd.id);
			AnimationEvent event;
			event.oid = rd.oid;
			event.type = AnimationTypes::ROTATE_Z;
			DBG_LOG("#> stopping RotateZAnimation - oid %d", rd.oid);
			events->add(100, &event, sizeof(AnimationEvent));
		}
	}
}

void RotateZAnimation::stop(ID oid) {
	for (unsigned int i = 0; i < _data.numObjects; ++i) {
		AnimationData& rd = _data.objects[i];
		if (rd.oid == oid) {
			_data.remove(rd.id);
		}
	}
}

// ----------------------------------------------------
// move to animation
// ----------------------------------------------------
ID MoveToAnimation::start(ID oid, transform_component* t, void* data, float ttl, tweening::TweeningType type) {
	MoveToData* rotData = (MoveToData*)data;
	DBG_LOG("=> starting MoveToAnimation - oid %d ttl %2.3f", oid, ttl);
	ID tmp = alreadyRunning(t);
	if (tmp != INVALID_ID) {
		DBG_LOG("=> MoveToAnimation - already running");
		return tmp;
	}
	ID id = _data.add();
	MoveToData& rd = _data.get(id);
	fillBasicData(&rd, oid, t, ttl, type);
	rd.start = rotData->start;
	rd.end = rotData->end;
	return id;
}

ID MoveToAnimation::alreadyRunning(transform_component* t) {
	for (unsigned int i = 0; i < _data.numObjects; ++i) {
		AnimationData& rd = (AnimationData)_data.objects[i];
		if ((uintptr_t)rd.transform == (uintptr_t)t) {
			return rd.id;
		}
	}
	return INVALID_ID;
}

void MoveToAnimation::tick(float dt, ds::EventStream * events) {
	for (unsigned int i = 0; i < _data.numObjects; ++i) {
		MoveToData& rd = _data.objects[i];
		rd.transform->position = tweening::interpolate(rd.tweeningType, rd.start, rd.end, rd.timer, rd.ttl);
		rd.timer += dt;
		if (rd.timer >= rd.ttl && rd.ttl > 0.0f) {
			rd.transform->position = tweening::interpolate(rd.tweeningType, rd.start, rd.end, 1.0f,1.0f);
			_data.remove(rd.id);
			AnimationEvent event;
			event.oid = rd.oid;
			event.type = AnimationTypes::MOVE_TO;
			DBG_LOG("#> stopping MoveToAnimation - oid %d", rd.oid);
			events->add(100, &event, sizeof(AnimationEvent));
		}
	}
}

void MoveToAnimation::stop(ID oid) {
	for (unsigned int i = 0; i < _data.numObjects; ++i) {
		AnimationData& rd = _data.objects[i];
		if (rd.oid == oid) {
			_data.remove(rd.id);
		}
	}
}

// ----------------------------------------------------
// scale to animation
// ----------------------------------------------------
ID ScaleToAnimation::start(ID oid, transform_component* t, void* data, float ttl, tweening::TweeningType type) {
	ScaleToData* rotData = (ScaleToData*)data;
	DBG_LOG("=> starting ScaleToAnimation - oid %d ttl %2.3f", oid, ttl);
	ID tmp = alreadyRunning(t);
	if (tmp != INVALID_ID) {
		DBG_LOG("=> ScaleToAnimation - already running");
		return tmp;
	}
	ID id = _data.add();
	ScaleToData& rd = _data.get(id);
	fillBasicData(&rd, oid, t, ttl, type);
	rd.start = rotData->start;
	rd.end = rotData->end;
	return id;
}

ID ScaleToAnimation::alreadyRunning(transform_component* t) {
	for (unsigned int i = 0; i < _data.numObjects; ++i) {
		AnimationData& rd = (AnimationData)_data.objects[i];
		if ((uintptr_t)rd.transform == (uintptr_t)t) {
			return rd.id;
		}
	}
	return INVALID_ID;
}

void ScaleToAnimation::tick(float dt, ds::EventStream * events) {
	for (unsigned int i = 0; i < _data.numObjects; ++i) {
		ScaleToData& rd = _data.objects[i];
		rd.transform->scale = tweening::interpolate(rd.tweeningType, rd.start, rd.end, rd.timer, rd.ttl);
		rd.timer += dt;
		if (rd.timer >= rd.ttl && rd.ttl > 0.0f) {
			rd.transform->scale = tweening::interpolate(rd.tweeningType, rd.start, rd.end, 1.0f, 1.0f);
			_data.remove(rd.id);
			AnimationEvent event;
			event.oid = rd.oid;
			event.type = AnimationTypes::SCALE_TO;
			DBG_LOG("#> stopping ScaleToAnimation - oid %d", rd.oid);
			events->add(100, &event, sizeof(AnimationEvent));
		}
	}
}

void ScaleToAnimation::stop(ID oid) {
	for (unsigned int i = 0; i < _data.numObjects; ++i) {
		AnimationData& rd = _data.objects[i];
		if (rd.oid == oid) {
			_data.remove(rd.id);
		}
	}
}