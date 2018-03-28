#pragma once
#include "..\lib\DataArray.h"
#include <ds_base_app.h>
#include <ds_tweening.h>
#include "..\utils\TransformComponent.h"

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
		SCALE_TO,
		ROTATE_TO,
		EOL
	};
	const static const char* NAMES[];
};

// ----------------------------------------------------
// Animation data
// ----------------------------------------------------
struct AnimationData {
	ID id;
	ID oid;
	float timer;
	float ttl;
	transform* transform;
	tweening::TweeningType tweeningType;
	ds::vec3 start;
	ds::vec3 end;
	AnimationTypes::Enum animationType;
};

// ----------------------------------------------------
// Animation event
// ----------------------------------------------------
struct AnimationEvent {
	ID oid;
	ID animationID;
	AnimationTypes::Enum type;
};

// ----------------------------------------------------
// function pointer definitions
// ----------------------------------------------------
typedef void(*PrepareAnimationFunction)(AnimationData*, void*);

typedef void(*RunAnimationFunction)(AnimationData*, float);

// ----------------------------------------------------
// Animation context
// ----------------------------------------------------
struct AnimationContext {

	const static PrepareAnimationFunction prepareFunctions[];

	const static RunAnimationFunction runFunctions[];

};

// ----------------------------------------------------
// Rotation data
// ----------------------------------------------------
struct RotationData {

	float angle;
	float direction;
};

// ----------------------------------------------------
// move to data
// ----------------------------------------------------
struct MoveToData {
	ds::vec3 start;
	ds::vec3 end;
};
// ----------------------------------------------------
// scale to data
// ----------------------------------------------------
struct ScaleToData {
	ds::vec3 start;
	ds::vec3 end;
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
	ID start(ID oid, transform* t, AnimationTypes::Enum animationType, void* data, float ttl, tweening::TweeningType type = tweening::linear);
	void stopAll(ID oid);
private:
	ID alreadyRunning(ID oid, transform* t);
	AnimationContext _context;
	ds::DataArray<AnimationData> _data;
};