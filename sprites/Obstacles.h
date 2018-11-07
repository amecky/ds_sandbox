#pragma once
#include <diesel.h>
#include <vector>

struct ObstaclesContainer {
	ds::vec2* positions;
	float* scales;
	int num;
	int max;
	RID textureId;
	int* grid;
	int dimX;
	int dimY;
};

namespace obstacles {

	void intialize(ObstaclesContainer* container, RID textureId, int dimX, int dimY);

	void add(ObstaclesContainer* container, int x, int y);

	void render(ObstaclesContainer* container);

	void shutdown(ObstaclesContainer* container);
}
