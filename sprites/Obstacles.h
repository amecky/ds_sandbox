#pragma once
#include <diesel.h>
#include <vector>

struct ObstaclesContainer {
	ds::vec2* positions;
	float* scales;
	int* rectIndices;
	int num;
	int max;
	RID textureId;
	int* grid;
	int dimX;
	int dimY;
};

struct ObstabcleEditorContext {
	int gx;
	int gy;
	int rectIndex;
	bool leftButtonPressed;
	bool rightButtonPressed;
};

namespace obstacles {

	void intialize(ObstaclesContainer* container, RID textureId, int dimX, int dimY);

	void clear(ObstaclesContainer* container);

	void add(ObstaclesContainer* container, int x, int y, int rectIndex);

	bool contains(ObstaclesContainer* container, int x, int y);

	void render(ObstaclesContainer* container);

	void shutdown(ObstaclesContainer* container);

	void edit(ObstaclesContainer* container, ObstabcleEditorContext* ctx);
}
