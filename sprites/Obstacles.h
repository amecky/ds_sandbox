#pragma once
#include <diesel.h>
#include <vector>

struct ObstaclesContainer {
	ds::vec2* positions;
	int num;
	int max;
	RID textureId;
};

namespace obstacles {

	void intialize(ObstaclesContainer* container, RID textureId, int maxObstacles);

	void add(ObstaclesContainer* container, const ds::vec2& pos);

	void render(ObstaclesContainer* container);

	void shutdown(ObstaclesContainer* container);
}
