#pragma once
#include <diesel.h>
#include "BlockArray.h"

struct Bullets {
	ds::vec2* positions;
	ds::vec2* prevPositions;
	ds::vec2* velocities;
	ds::vec2* scales;
	float* rotations;
	float* timers;
	RID textureId;
	BlockArray* array;
	int _max;
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

	void move(Bullets* bullets, ObstaclesContainer* obstacles, int particleSystem, int emitter, float dt);

	void render(Bullets* bullets);

	void remove(Bullets* bullets, int index);

	void shutdown(Bullets* bullets);

	void add(Bullets* bullets, Player* player);

}

struct TrailPieceSettings {
	float ttl;
};

struct TrailPieces {
	ds::vec2* positions;
	ds::vec2* scales;
	ds::Color* colors;
	float* rotations;
	float* timers;
	int* behaviorIndex;
	RID textureId;
	BlockArray* array;
	int _max;
	TrailPieceSettings settings;
};

namespace trails {

	void initialize(TrailPieces* pieces, int max);

	void render(TrailPieces* pieces);

	void tick(TrailPieces* pieces, float dt);

	void add(TrailPieces* pieces, const ds::vec2& pos, float rotation);

	void shutdown(TrailPieces* pieces);

}