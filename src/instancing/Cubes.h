#pragma once
#include <diesel.h>
#include "..\utils\DynamicPath.h"
#include "..\utils\RenderItem.h"
#include "..\utils\EventStream.h"
#include "InstanceCommon.h"

enum CubeState {
	IDLE, MOVING, STEPPING, ROTATING, WALKING
};

struct Cube {
	ID id;		
	ds::vec3 startPosition;
	ds::vec3 targetPosition;
	float startRotation;
	float endRotation;
	CubeState state;
	int type;
	ds::vec3 force;
};

class Cubes {

public:
	Cubes(instanced_render_item* render_item) : _render_item(render_item) {}
	~Cubes() {}
	void init();
	void tick(float dt, const ds::vec3& playerPosition);
	void createCube(const ds::vec3& pos);
	void rotateCubes();
	int checkCollisions(Bullet* bullets, int num, ds::EventStream* events);
	int getNumItems() const {
		return _num_cubes;
	}
	void stepForward();
private:
	void separate(float minDistance, float relaxation);
	instanced_render_item* _render_item;
	Cube _cubes[256];
	int _num_cubes;
	float _dbgTTL;
};