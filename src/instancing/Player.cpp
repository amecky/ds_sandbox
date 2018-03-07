#include "Player.h"
#include <ds_tweakable.h>
#include "..\utils\common_math.h"

namespace player {

	// ----------------------------------------------------
	// prepare settings
	// ----------------------------------------------------
	static void prepare_tweakables(Player* player) {

		twk_add("player", "velocity_damping", &player->settings.velocity_damping);
		twk_add("player", "velocity", &player->settings.velocity);
		twk_add("player", "camera_height", &player->settings.camera_height);
		twk_add("player", "bounding_box", &player->settings.bounding_box);
		
	}

	// ----------------------------------------------------
	// init player
	// ----------------------------------------------------
	void init(Player* player) {
		prepare_tweakables(player);
		reset(player);
	}

	// ----------------------------------------------------
	// reset player
	// ----------------------------------------------------
	void reset(Player* player) {
		initialize(&player->render_item.transform, ds::vec3(0.0f), ds::vec3(0.22f), 0.0f, 0.0f);
		player->render_item.transform.position.z = player->render_item.mesh->getExtent().z * player->render_item.transform.scale.z * -0.5f;
		player->shooting = false;
		player->shootingAngle = 0.0f;
		player->velocity = ds::vec3(0.0f);
		
	}

	// ----------------------------------------------------
	// handle keyboard input
	// ----------------------------------------------------
	void handle_keys(Player* player, float dt) {

		if (ds::isMouseButtonPressed(0) && !player->shooting) {
			player->shooting = true;
		}
		if (!ds::isMouseButtonPressed(0) && player->shooting) {
			player->shooting = false;
		}

		ds::vec3 vel = ds::vec3(0.0f);

		if (ds::isKeyPressed('W')) {
			vel.y = 1.0f;
		}
		if (ds::isKeyPressed('S')) {
			vel.y = -1.0f;
		}
		if (ds::isKeyPressed('A')) {
			vel.x = -1.0f;
		}
		if (ds::isKeyPressed('D')) {
			vel.x = 1.0f;
		}
		if (sqr_length(vel) > 0.1f) {
			player->velocity += vel;
			player->velocity = normalize(player->velocity) * player->settings.velocity;
		}
		
	}

	// ----------------------------------------------------
	// move camera
	// ----------------------------------------------------
	/*
	void move_camera(Player* player, TopDownCamera* topDownCamera, float dt) {
		ds::vec3 cameraTargetPos = player->render_item.transform.position;
		cameraTargetPos.z = player->settings.camera_height;

		float velocity = 2.0f;
		ds::vec3 current = topDownCamera->getPosition();
		current.z = player->settings.camera_height;
		ds::vec3 diff = cameraTargetPos - current;
		if (sqr_length(diff) > 0.1f) {
			ds::vec2 n = normalize(ds::vec2(diff.x, diff.y));
			ds::vec2 desired = n * velocity;
			current += ds::vec3(desired) * dt;
			if (current.x < -1.55f) {
				current.x = -1.55f;
			}
			if (current.x > 1.55f) {
				current.x = 1.55f;
			}
			if (current.y < -0.8f) {
				current.y = -0.8f;
			}
			if (current.y > 0.8f) {
				current.y = 0.8f;
			}

		}
		topDownCamera->setPosition(current);
	}
	*/
	// ----------------------------------------------------
	// move player
	// ----------------------------------------------------
	void move(Player* player, const ds::vec3& cursorPos, float dt) {
		ds::vec3 delta = cursorPos - player->render_item.transform.position;
		player->shootingAngle = math::get_rotation(ds::vec2(delta.x, delta.y));
		ds::vec2 pv = ds::vec2(player->velocity.x, player->velocity.y);
		ds::vec3 new_pos = player->render_item.transform.position + player->velocity * dt;
		if (math::out_of_bounds(new_pos, player->settings.bounding_box)) {
			player->velocity = ds::vec3(0.0f);
		}
		else {
			if (sqr_length(pv) != 0.0f) {
				player->render_item.transform.roll = math::get_rotation(pv);
				player->render_item.transform.pitch += ds::PI * dt;
			}
			player->render_item.transform.position += player->velocity * dt;
			player->velocity = player->velocity * player->settings.velocity_damping;
			if (sqr_length(player->velocity) < 0.1f) {
				player->velocity = ds::vec3(0.0f);
			}
		}
	}

	// ----------------------------------------------------
	// move player to center
	// ----------------------------------------------------
	void move_to_center(Player* player, const ds::vec3& cursorPos, float dt) {
		ds::vec3 delta = cursorPos - player->render_item.transform.position;
		player->shootingAngle = math::get_rotation(ds::vec2(delta.x, delta.y));

		ds::vec3 diff = -1.0f * player->render_item.transform.position;
		ds::vec2 n = normalize(ds::vec2(diff.x, diff.y));
		ds::vec2 desired = n * 2.0f;

		ds::vec2 pv = ds::vec2(desired.x, desired.y);
		if (sqr_length(pv) != 0.0f) {
			player->render_item.transform.roll = math::get_rotation(pv);
			player->render_item.transform.pitch += ds::TWO_PI * dt;
		}
		player->render_item.transform.position += desired * dt;		
	}

}
