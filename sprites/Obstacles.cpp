#include "Obstacles.h"
#include <ds_sprites.h>
#include <ds_imgui.h>
#include "sprites_def.h"

namespace obstacles {

	void intialize(ObstaclesContainer* container, RID textureId, int dimX, int dimY) {
		container->num = 0;
		container->max = dimX * dimY;
		container->positions = new ds::vec2[container->max];
		container->scales = new float[container->max];
		container->rectIndices = new int[container->max];
		container->textureId = textureId;
		container->grid = new int[container->max];
		container->dimX = dimX;
		container->dimY = dimY;
		clear(container);
	}

	void clear(ObstaclesContainer* container) {
		for (int i = 0; i < container->max; ++i) {
			container->grid[i] = -1;
		}
	}

	void add(ObstaclesContainer* container, int x, int y, int rectIndex) {
		if (container->num + 1 < container->max) {
			container->positions[container->num++] = ds::vec2(100, 100) + ds::vec2(30*x, 30*y);
			container->scales[container->num - 1] = 1.0f;
			container->rectIndices[container->num - 1] = rectIndex;
		}
		container->grid[x + y * container->dimX] = rectIndex;
	}

	bool contains(ObstaclesContainer* container, int x, int y) {
		return container->grid[x + y * container->dimX] != -1;
	}

	static p2i convert_to_grid(ObstaclesContainer* container, const ds::vec2& pos) {
		int x = (pos.x() - 100.0f) / 30.0f;
		int y = (pos.y() - 100.0f) / 30.0f;
		if (x < 0) {
			x = 0;
		}
		if (y < 0) {
			y = 0;
		}
		if (x >= container->dimX) {
			x = container->dimX - 1;
		}
		if (y >= container->dimY) {
			y = container->dimY - 1;
		}
		return p2i(x, y);
	}

	void render(ObstaclesContainer* container) {
		for (int i = 0; i < container->num; ++i) {
			sprites::draw(container->textureId, container->positions[i], OBSTACLE_RECTS[container->rectIndices[i]], ds::vec2(container->scales[i]));
		}
	}

	void shutdown(ObstaclesContainer* container) {
		delete[] container->rectIndices;
		delete[] container->scales;
		delete[] container->positions;
	}

	void edit(ObstaclesContainer* container, ObstabcleEditorContext* ctx) {
		ds::vec2 mp = ds::getMousePosition();
		p2i gp = convert_to_grid(container, mp);
		ctx->gx = gp.x;
		ctx->gy = gp.y;
		if (ds::isMouseButtonClicked(1)) {
			++ctx->rectIndex;
			if (ctx->rectIndex >= 15) {
				ctx->rectIndex = 0;
			}
		}
		if (ds::isMouseButtonClicked(0)) {
			if (!contains(container, gp.x, gp.y)) {
				add(container, gp.x, gp.y, ctx->rectIndex);
			}
		}
		for (int i = 0; i < 15; ++i) {
			sprites::draw(container->textureId, ds::vec2(450 + i * 40, 700), OBSTACLE_RECTS[i]);
		}
		sprites::draw(container->textureId, ds::vec2(450 + ctx->rectIndex * 40, 700), ds::vec4(200,300,40,40));

		sprites::draw(container->textureId, ds::vec2(100, 100) + ds::vec2(30 * gp.x, 30 * gp.y), OBSTACLE_RECTS[ctx->rectIndex]);
	}
}
