#include "Obstacles.h"
#include <ds_sprites.h>

const ds::vec4 OBSTACLE_RECT = ds::vec4(0, 260, 30, 30);

namespace obstacles {

	void intialize(ObstaclesContainer* container, RID textureId, int dimX, int dimY) {
		container->num = 0;
		container->max = dimX * dimY;
		container->positions = new ds::vec2[container->max];
		container->scales = new float[container->max];
		container->textureId = textureId;
		container->grid = new int[container->max];
		container->dimX = dimX;
		container->dimY = dimY;
		for (int i = 0; i < container->max; ++i) {
			container->grid[i] = 0;
		}
	}

	void add(ObstaclesContainer* container, int x, int y) {
		if (container->num + 1 < container->max) {
			container->positions[container->num++] = ds::vec2(100, 100) + ds::vec2(30*x, 30*y);
			container->scales[container->num - 1] = 1.0f;
		}
	}

	void render(ObstaclesContainer* container) {
		for (int i = 0; i < container->num; ++i) {
			sprites::draw(container->textureId, container->positions[i], OBSTACLE_RECT, ds::vec2(container->scales[i]));
		}
	}

	void shutdown(ObstaclesContainer* container) {
		delete[] container->scales;
		delete[] container->positions;
	}
}
