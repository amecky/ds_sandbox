#pragma once
#include <diesel.h>
#include <ds_sprites.h>

struct BoidSettings {
	float minDistance;
	float relaxation;
	bool separate;
	bool seek;
	float seekVelocity;
	float avoidVelocity;
	RID textureId;
};

struct BoidContainer {
	ds::vec2* positions;
	ds::vec2* velocities;
	ds::vec2* accelerations;
	float* rotations;
	int* states;
	ds::Color* colors;
	int num;
	int max;
	BoidSettings settings;
};

typedef void(*BoidMoveFunc)(BoidContainer*, BoidSettings*, const ds::vec2&);

struct ObstaclesContainer;

namespace boid {

	void seekFunc(BoidContainer* container, BoidSettings* settings, const ds::vec2& target);

	void kill_boids(BoidContainer* container, const ds::vec2& target, float dist);

	void kill_boid(BoidContainer* container, int index);

	void initialize(BoidContainer* container, int maxBoids);

	void shutdown(BoidContainer* container);

	void add(BoidContainer* container, int count, const ds::vec2& target);

	void add(BoidContainer* container, const ds::vec2& p, const ds::vec2& target);

	void reset_forces(BoidContainer* container);

	void apply_forces(BoidContainer* container, float dt);

	void move(BoidContainer* container, const ds::vec2& target, ObstaclesContainer* obstacles, float dt);

	void avoid(BoidContainer* container, ObstaclesContainer* obstacles);

	void render(BoidContainer* container);

}
