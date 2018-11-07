#pragma once
#include <diesel.h>

struct Bullets {
	ds::vec2* positions;
	ds::vec2* velocities;
	float* rotations;
	float* timers;
	int num;
	int max;
	RID textureId;
};

struct Player {
	ds::vec2 pos;
	float rotation;
	RID textureId;
};

namespace player {

	void move(Player* player, float dt);

	void render(Player* player);
}

struct ObstaclesContainer;

namespace bullets {

	void initialize(Bullets* bullets, int max);

	void move(Bullets* bullets, ObstaclesContainer* obstacles, float dt);

	void render(Bullets* bullets);

	void remove(Bullets* bullets, int index);

	void shutdown(Bullets* bullets);

	void add(Bullets* bullets, Player* player);

}
