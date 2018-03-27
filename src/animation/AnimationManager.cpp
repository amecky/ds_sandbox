#include "AnimationManager.h"

const char* AnimationTypes::NAMES[] = {
	"IDLE",
	"ROTATE_X",
	"ROTATE_Y",
	"ROTATE_Z",
	"MOVE_TO",
	"SCALE_TO",
	"ROTATE_TO"
	"EOL"
};



void PrepareIdleAnimation(AnimationData* animationData, void* data) {
}

void PrepareRotateXAnimation(AnimationData* animationData, void* data) {
	RotationData* rotData = (RotationData*)data;
	animationData->start.x = rotData->angle;
	animationData->start.y = rotData->direction;
}

void PrepareRotateYAnimation(AnimationData* animationData, void* data) {
	RotationData* rotData = (RotationData*)data;	
	animationData->start.x = rotData->angle;
	animationData->start.y = rotData->direction;
}

void PrepareRotateZAnimation(AnimationData* animationData, void* data) {
	RotationData* rotData = (RotationData*)data;
	animationData->start.x = rotData->angle;
	animationData->start.y = rotData->direction;
}

void PrepareMoveToAnimation(AnimationData* animationData, void* data) {
	MoveToData* movData = (MoveToData*)data;
	animationData->start = movData->start;
	animationData->end = movData->end;
}

void PrepareScaleAnimation(AnimationData* animationData, void* data) {
	ScaleToData* movData = (ScaleToData*)data;
	animationData->start = movData->start;
	animationData->end = movData->end;
}

void PrepareRotateToAnimation(AnimationData* animationData, void* data) {
	MoveToData* movData = (MoveToData*)data;
	animationData->start = movData->start;
	animationData->end = movData->end;
}


void RunIdleAnimation(AnimationData* data, float dt) {

}

void RunRotateXAnimation(AnimationData* data, float dt) {
	if (data->ttl != 0.0f) {
		data->transform->rotation.x += data->start.x / data->ttl * dt * data->start.y;
	}
	else {
		data->transform->rotation.x += data->start.x * dt * data->start.y;
	}
}

void RunRotateYAnimation(AnimationData* data, float dt) {
	if (data->ttl != 0.0f) {
		data->transform->rotation.y += data->start.x / data->ttl * dt * data->start.y;
	}
	else {
		data->transform->rotation.y += data->start.x * dt * data->start.y;
	}
}

void RunRotateZAnimation(AnimationData* data, float dt) {
	if (data->ttl != 0.0f) {
		data->transform->rotation.z += data->start.x / data->ttl * dt * data->start.y;
	}
	else {
		data->transform->rotation.z += data->start.x * dt * data->start.y;
	}
}

void RunMoveToAnimation(AnimationData* data, float dt) {
	data->transform->position = tweening::interpolate(data->tweeningType, data->start, data->end, data->timer, data->ttl);
}

void RunScaleToAnimation(AnimationData* data, float dt) {
	data->transform->scale = tweening::interpolate(data->tweeningType, data->start, data->end, data->timer, data->ttl);
}

void RunRotateToAnimation(AnimationData* data, float dt) {
	data->transform->rotation = tweening::interpolate(data->tweeningType, data->start, data->end, data->timer, data->ttl);
}

// ----------------------------------------------------
// AnimationContext
// ----------------------------------------------------
const PrepareAnimationFunction AnimationContext::prepareFunctions[] = {
	PrepareIdleAnimation,
	PrepareRotateXAnimation,
	PrepareRotateYAnimation,
	PrepareRotateZAnimation,
	PrepareMoveToAnimation,
	PrepareScaleAnimation,
	PrepareRotateToAnimation
};

const RunAnimationFunction AnimationContext::runFunctions[] = {
	RunIdleAnimation,
	RunRotateXAnimation,
	RunRotateYAnimation,
	RunRotateZAnimation,
	RunMoveToAnimation,
	RunScaleToAnimation,
	RunRotateToAnimation
};
// ----------------------------------------------------
// AnimationManager
// ----------------------------------------------------
AnimationManager::AnimationManager() {
}

AnimationManager::~AnimationManager() {
}

// ----------------------------------------------------
// stop
// ----------------------------------------------------
void AnimationManager::stop(AnimationTypes::Enum animationType, ID oid) {
	for (unsigned int i = 0; i < _data.numObjects; ++i) {
		AnimationData& rd = _data.objects[i];
		if (rd.oid == oid && rd.animationType == animationType) {
			_data.remove(rd.id);
		}
	}
}

// ----------------------------------------------------
// tick
// ----------------------------------------------------
void AnimationManager::tick(float dt, ds::EventStream* events) {

	for (unsigned int i = 0; i < _data.numObjects; ++i) {
		AnimationData& animationData = _data.objects[i];
		RunAnimationFunction func = _context.runFunctions[animationData.animationType];
		(func)(&animationData, dt);

		animationData.timer += dt;
		if (animationData.timer > animationData.ttl && animationData.ttl > 0.0f) {
			AnimationEvent event;
			event.oid = animationData.oid;
			event.type = animationData.animationType;
			// FIXME: is it clever to send an ID that will be invalid right away?
			event.animationID = animationData.id;
			events->add(100, &event, sizeof(AnimationEvent));
			DBG_LOG("#> stopping %s - id: %d oid: %d", AnimationTypes::NAMES[animationData.animationType], animationData.id, animationData.oid);
			_data.remove(animationData.id);
		}		
	}
	
}

// ----------------------------------------------------
// stop all
// ----------------------------------------------------
void AnimationManager::stopAll(ID oid) {
	for (unsigned int i = 0; i < _data.numObjects; ++i) {
		AnimationData& rd = _data.objects[i];
		if (rd.oid == oid) {
			_data.remove(rd.id);
		}
	}
}

// ----------------------------------------------------
// already running
// ----------------------------------------------------
ID AnimationManager::alreadyRunning(ID oid, transform_component* t) {
	for (unsigned int i = 0; i < _data.numObjects; ++i) {
		AnimationData& rd = (AnimationData)_data.objects[i];
		if ((uintptr_t)rd.transform == (uintptr_t)t && oid == rd.id) {
			return rd.id;
		}
	}
	return INVALID_ID;
}

// ----------------------------------------------------
// start
// ----------------------------------------------------
ID AnimationManager::start(ID oid, transform_component* t, AnimationTypes::Enum animationType, void* data, float ttl, tweening::TweeningType type) {
	ID tmp = alreadyRunning(oid, t);
	// FIXME: maybe we should reset the action or should be ignore the request?
	if (tmp != INVALID_ID) {
		DBG_LOG("=> starting %s - ALREADY RUNNING id: %d oid: %d", AnimationTypes::NAMES[animationType], tmp, oid);
		return tmp;
	}
	PrepareAnimationFunction func = _context.prepareFunctions[animationType];
	ID id = _data.add();
	AnimationData& animData = _data.get(id);
	animData.oid = oid;
	animData.transform = t;
	animData.tweeningType = type;
	animData.ttl = ttl;
	animData.timer = 0.0f;
	animData.animationType = animationType;
	(func)(&animData, data);
	DBG_LOG("=> starting %s - id: %d oid: %d ttl: %3.2f", AnimationTypes::NAMES[animationType],id, oid, ttl);
	return id;
}
