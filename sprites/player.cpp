#include "player.h"
#include <ds_sprites.h>
#include "math.h"
#include "Obstacles.h"
#include "particles/Particlesystem.h"

const static ds::vec4 PLAYER_RECT = ds::vec4(260, 240, 60, 60);
//const static ds::vec4 BULLET_RECT = ds::vec4(80, 50, 12, 12);
const static ds::vec4 BULLET_RECT = ds::vec4(0, 375, 40, 40);
const static ds::vec4 PARTICLE_RECT = ds::vec4(0, 375, 40, 40);
const static ds::vec4 BOUNDING_RECT = ds::vec4(10.0f, 10.0f, 1000.0f, 650.0f);

namespace player {

	void move(Player* player, float dt) {
		ds::vec2 v(0.0f);
		if (ds::isKeyPressed('A')) {
			v.setX(-1.0f);
		}
		if (ds::isKeyPressed('D')) {
			v.setX(1.0f);
		}
		if (ds::isKeyPressed('S')) {
			v.setY(-1.0f);
		}
		if (ds::isKeyPressed('W')) {
			v.setY(1.0f);
		}
		player->pos += v * 200.0f * dt;
		player->rotation = calculate_rotation(ds::getMousePosition() - player->pos);
	}

	void render(Player* player) {
		sprites::draw(player->textureId, player->pos, PLAYER_RECT, ds::vec2(1.0f),player->rotation);
	}
}

namespace bullets {

	void initialize(Bullets* bullets, int max) {
		bullets->_max = max;
		bullets->array = new BlockArray;
		int sizes[] = { sizeof(ds::vec2),sizeof(ds::vec2),sizeof(ds::vec2),sizeof(ds::vec2),sizeof(float),sizeof(float) };
		bullets->array->init(sizes, 6);
		bullets->array->resize(max);
		bullets->positions = (ds::vec2*)bullets->array->get_ptr(0);
		bullets->prevPositions = (ds::vec2*)bullets->array->get_ptr(1);
		bullets->velocities = (ds::vec2*)bullets->array->get_ptr(2);
		bullets->scales = (ds::vec2*)bullets->array->get_ptr(3);
		bullets->rotations = (float*)bullets->array->get_ptr(4);
		bullets->timers = (float*)bullets->array->get_ptr(5);
	}

	static bool is_outside(const ds::vec2& p) {
		if (p.x() < BOUNDING_RECT.x || p.x() > BOUNDING_RECT.z) {
			return true;
		}
		if (p.y() < BOUNDING_RECT.y || p.y() > BOUNDING_RECT.w) {
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
		if (bullets->array->size > 0) {
			bullets->array->remove(index);			
		}
	}

	void move(Bullets* bullets, ObstaclesContainer* obstacles, int particleSystem, int emitter, float dt) {
		
		for (int i = 0; i < bullets->array->size; ++i) {
			bullets->timers[i] += dt;
		}

		for (int i = 0; i < bullets->array->size; ++i) {
			bullets->positions[i] += bullets->velocities[i] * dt;
			//if (is_outside(bullets->positions[i]) || hits_obstacle(bullets->positions[i], obstacles)) {
			if (hits_obstacle(bullets->positions[i], obstacles)) {
				const EmitterSettings& settings = particles_get_emitter(emitter);
				particles_emitt(particleSystem, bullets->positions[i], settings);
				remove(bullets, i);					
			}
		}
	}

	void render(Bullets* bullets) {
		for (int i = 0; i < bullets->array->size; ++i) {
			//float sx = 0.8f + bullets->timers[i] * 2.4f;
			//float r = 1.0f - bullets->timers[i] * 0.4f;
			sprites::draw(bullets->textureId, bullets->positions[i], BULLET_RECT, ds::vec2(1.5f,0.4f),bullets->rotations[i], ds::Color(230,230,180,255));
		}
	}

	void shutdown(Bullets* bullets) {
		delete bullets->array;
	}
	
	void add(Bullets* bullets, Player* player) {
		if (bullets->array->size + 1 < bullets->_max) {
			float offset = ds::PI * 0.02f;
			float ra = player->rotation + ds::random(-offset, offset);
			ds::vec2 np = player->pos + ds::vec2(cosf(ra), sinf(ra)) * 20.0f;
			bullets->positions[bullets->array->size++] = np;
			bullets->prevPositions[bullets->array->size - 1] = np;
			bullets->velocities[bullets->array->size - 1] = ds::vec2(cosf(ra), sinf(ra)) * 600.0f;
			bullets->rotations[bullets->array->size - 1] = ra;
			bullets->scales[bullets->array->size - 1] = ds::vec2(1.0f,0.5f);
			bullets->timers[bullets->array->size - 1] = 0.0f;
		}
	}
}

namespace trails {

	void initialize(TrailPieces* pieces, int max) {
		pieces->_max = max;
		pieces->array = new BlockArray;
		int sizes[] = { sizeof(ds::vec2),sizeof(ds::vec2),sizeof(ds::Color),sizeof(float),sizeof(float),sizeof(int) };
		pieces->array->init(sizes, 6);
		pieces->array->resize(max);
		pieces->positions = (ds::vec2*)pieces->array->get_ptr(0);
		pieces->scales = (ds::vec2*)pieces->array->get_ptr(1);
		pieces->colors = (ds::Color*)pieces->array->get_ptr(2);
		pieces->rotations = (float*)pieces->array->get_ptr(3);
		pieces->timers = (float*)pieces->array->get_ptr(4);
		pieces->behaviorIndex = (int*)pieces->array->get_ptr(5);
	}

	void render(TrailPieces* pieces) {
		for (int i = 0; i < pieces->array->size; ++i) {
			float sx = 0.5f - pieces->timers[i] / pieces->settings.ttl * 0.3f;
			ds::Color c = pieces->colors[i];
			float norm = pieces->timers[i] / pieces->settings.ttl;
			c.a = 1.0f - norm;
			sprites::draw(pieces->textureId, pieces->positions[i], PARTICLE_RECT, pieces->scales[i], pieces->rotations[i], c);
		}
	}

	void tick(TrailPieces* pieces, float dt) {
		for (int i = 0; i < pieces->array->size; ++i) {
			pieces->timers[i] += dt;
			if (pieces->timers[i] >= pieces->settings.ttl || pieces->colors[i].a <= 0.0f) {
				pieces->array->remove(i);
			}
		}
	}

	void add(TrailPieces* pieces, const ds::vec2& pos, float rotation) {
		int num = 4;
		if (pieces->array->size + num < pieces->_max) {
			for (int i = 0; i < num; ++i) {
				float rnd = ds::random(-ds::PI * 0.1f, ds::PI*0.1f);
				ds::vec2 p = pos + ds::random(4.0f,10.0f) * ds::vec2(cosf(rotation + rnd), sinf(rotation + rnd));
				pieces->positions[pieces->array->size++] = p;
				pieces->colors[pieces->array->size - 1] = ds::Color(255, 128, 0, 255);
				pieces->rotations[pieces->array->size - 1] = rotation;
				pieces->scales[pieces->array->size - 1] = ds::vec2(ds::random(0.3f,0.5f), ds::random(0.3f, 0.5f));
				pieces->timers[pieces->array->size - 1] = 0.0f;
			}
		}
	}

	void shutdown(TrailPieces* pieces) {
		delete pieces->array;
	}

}