#include "Obstacles.h"
#include <ds_sprites.h>

const ds::vec4 OBSTACLE_RECT = ds::vec4(0, 260, 30, 30);

namespace obstacles {

	void intialize(ObstaclesContainer* container, RID textureId, int maxObstacles) {
		container->num = 0;
		container->max = maxObstacles;
		container->positions = new ds::vec2[maxObstacles];
		container->textureId = textureId;
	}

	void add(ObstaclesContainer* container, const ds::vec2& pos) {
		if (container->num + 1 < container->max) {
			container->positions[container->num++] = pos;
		}
	}

	void render(ObstaclesContainer* container) {
		for (int i = 0; i < container->num; ++i) {
			sprites::draw(container->textureId, container->positions[i], OBSTACLE_RECT);
		}
	}

	void shutdown(ObstaclesContainer* container) {
		delete[] container->positions;
	}
}
