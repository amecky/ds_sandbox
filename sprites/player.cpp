#include "player.h"
#include <ds_sprites.h>
#include "math.h"
#include "Obstacles.h"

const static ds::vec4 PLAYER_RECT = ds::vec4(150, 250, 48, 48);
const static ds::vec4 BULLET_RECT = ds::vec4(80, 50, 12, 12);
const static ds::vec4 BOUNDING_RECT = ds::vec4(10.0f, 10.0f, 1000.0f, 650.0f);

namespace player {

	void move(Player* player, float dt) {
		ds::vec2 v(0.0f);
		if (ds::isKeyPressed('A')) {
			v.x -= 1.0f;
		}
		if (ds::isKeyPressed('D')) {
			v.x += 1.0f;
		}
		if (ds::isKeyPressed('S')) {
			v.y -= 1.0f;
		}
		if (ds::isKeyPressed('W')) {
			v.y += 1.0f;
		}
		player->pos += v * 150.0f * dt;
		player->rotation = calculate_rotation(ds::getMousePosition() - player->pos);
	}

	void render(Player* player) {
		sprites::draw(player->textureId, player->pos, PLAYER_RECT, ds::vec2(1.0f),player->rotation);
	}
}

namespace bullets {

	void initialize(Bullets* bullets, int max) {
		bullets->max = max;
		bullets->num = 0;
		bullets->positions = new ds::vec2[max];
		bullets->velocities = new ds::vec2[max];
		bullets->rotations = new float[max];
		bullets->timers = new float[max];
	}

	static bool is_outside(const ds::vec2& p) {
		if (p.x < BOUNDING_RECT.x || p.x > BOUNDING_RECT.z) {
			return true;
		}
		if (p.y < BOUNDING_RECT.y || p.y > BOUNDING_RECT.w) {
			return true;
		}
		return false;
	}

	static bool hits_obstacle(const ds::vec2& p, ObstaclesContainer* obstacles) {
		for (int i = 0; i < obstacles->num; ++i) {
			ds::vec2 diff = obstacles->positions[i] - p;
			if (sqr_length(diff) < 20.0f * 20.0f) {
				return true;
			}
		}
		return false;
	}

	void remove(Bullets* bullets, int index) {
		if (bullets->num > 1) {
			bullets->positions[index] = bullets->positions[bullets->num - 1];
			bullets->velocities[index] = bullets->velocities[bullets->num - 1];
			bullets->rotations[index] = bullets->rotations[bullets->num - 1];
		}
		--bullets->num;
	}

	void move(Bullets* bullets, ObstaclesContainer* obstacles, float dt) {
		for (int i = 0; i < bullets->num; ++i) {
			bullets->positions[i] += bullets->velocities[i] * dt;
			bullets->timers[i] += dt;
			if (is_outside(bullets->positions[i]) || hits_obstacle(bullets->positions[i], obstacles)) {
				if (bullets->num > 1) {
					bullets->positions[i] = bullets->positions[bullets->num - 1];
					bullets->velocities[i] = bullets->velocities[bullets->num - 1];
					bullets->rotations[i] = bullets->rotations[bullets->num - 1];
				}
				--bullets->num;
			}
		}
	}

	void render(Bullets* bullets) {
		for (int i = 0; i < bullets->num; ++i) {
			float sx = 0.8f + bullets->timers[i] * 0.4f;
			float r = 1.0f - bullets->timers[i] * 0.4f;
			sprites::draw(bullets->textureId, bullets->positions[i], BULLET_RECT, ds::vec2(sx,0.5f),bullets->rotations[i], ds::Color(r,0.0f,0.0f,1.0f));
		}
	}

	void shutdown(Bullets* bullets) {
		delete[] bullets->timers;
		delete[] bullets->rotations;
		delete[] bullets->velocities;
		delete[] bullets->positions;
	}
	
	void add(Bullets* bullets, Player* player) {
		if (bullets->num + 1 < bullets->max) {
			bullets->positions[bullets->num++] = player->pos;
			bullets->velocities[bullets->num - 1] = ds::vec2(cosf(player->rotation), sinf(player->rotation)) * 400.0f;
			bullets->rotations[bullets->num - 1] = player->rotation;
			bullets->timers[bullets->num - 1] = 0.0f;
		}
	}
}