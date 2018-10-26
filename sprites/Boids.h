#pragma once
#include <diesel.h>
#include <ds_sprites.h>

struct BoidContainer {
	ds::vec2* positions;
	ds::vec2* velocities;
	ds::vec2* forces;
	float* rotations;
};

struct BoidSettings {
	float minDistance;
	float relaxation;
	bool separate;
	bool seek;
	float seekVelocity;
};

class Boids {

public:
	Boids(RID textureID, BoidSettings* settings);
	~Boids();
	void add(int count);
	void move(const ds::vec2& target, float dt);
	void render();
private:
	void separate(float minDistance, float relaxation, float dt);
	void seek(const ds::vec2& target, float velocity, float dt);
	RID _textureID;
	BoidContainer _boids;
	int _num;
	BoidSettings* _settings;
};

