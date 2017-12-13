#include "Player.h"

static float get_angle(const ds::vec2& v1, const ds::vec2& v2) {
	if (v1 != v2) {
		ds::vec2 vn1 = normalize(v1);
		ds::vec2 vn2 = normalize(v2);
		float dt = dot(vn1, vn2);
		if (dt < -1.0f) {
			dt = -1.0f;
		}
		if (dt > 1.0f) {
			dt = 1.0f;
		}
		float tmp = acos(dt);
		float cross = -1.0f * (vn2 - vn1).y;
		//float crs = cross(vn1, vn2);
		if (cross < 0.0f) {
			tmp = ds::TWO_PI - tmp;
		}
		return tmp;
	}
	else {
		return 0.0f;
	}
}

namespace player {

	void reset(Player* player) {
		initialize(&player->render_item.transform, ds::vec3(0.0f), ds::vec3(0.35f), 0.0f, 0.0f);
		player->render_item.transform.position.z = player->render_item.mesh->getExtent().z * player->render_item.transform.scale.z * -0.5f;
		player->shooting = false;
		player->shootingAngle = 0.0f;
		player->velocity = ds::vec3(0.0f);
	}

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
			player->velocity = normalize(player->velocity) * 2.0f;
		}
		
	}

	void move(Player* player, const ds::vec3& cursorPos, float dt) {
		ds::vec3 delta = cursorPos - player->render_item.transform.position;
		player->shootingAngle = get_angle(ds::vec2(delta.x, delta.y), ds::vec2(1, 0));
		ds::vec2 pv = ds::vec2(player->velocity.x, player->velocity.y);
		if (sqr_length(pv) != 0.0f) {
			player->render_item.transform.roll = get_angle(pv, ds::vec2(1, 0));
			player->render_item.transform.pitch += ds::TWO_PI * dt;
		}
		player->render_item.transform.position += player->velocity * dt;
		player->velocity = player->velocity * 0.95f;
		if (sqr_length(player->velocity) < 0.1f) {
			player->velocity = ds::vec3(0.0f);
		}
	}

}
