#pragma once
#include <diesel.h>
#include "..\utils\DynamicPath.h"
#include "..\utils\RenderItem.h"
#include <ds_base_app.h>
#include "InstanceCommon.h"
#include "Bullets.h"

struct CubesSettings {
	float startTTL;
	float seek_velocity;
	float min_distance;
	float relaxation;
};

enum CubeState {
	IDLE, MOVING, STEPPING, ROTATING, WALKING, STARTING
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
	int steps;
	ds::vec2 direction;
};

class Cubes {

public:
	Cubes(instanced_render_item* render_item, CubesSettings* settings) : _render_item(render_item) , _settings(settings) {}
	~Cubes() {}
	void init();
	void tick(float dt, const ds::vec3& playerPosition);
	void createCube(const ds::vec3& pos, int side);
	void rotateCubes();
	void checkCollisions(bullets::Bullets& bullets, ds::EventStream* events);
	int getNumItems() const {
		return _num_cubes;
	}
	void stepForward();
private:
	void separate(float minDistance, float relaxation);
	instanced_render_item* _render_item;
	Cube _cubes[256];
	int _num_cubes;
	CubesSettings* _settings;
	float _dbgTTL;
};