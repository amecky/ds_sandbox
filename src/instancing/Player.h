#pragma once
#include <diesel.h>
#include "TopDownCamera.h"
#include "..\utils\TransformComponent.h"
#include "..\utils\RenderItem.h"

struct PlayerSettings {
	float velocity_damping;
	float velocity;
	float camera_height;
	ds::vec4 bounding_box;
};

struct Player {
	render_item render_item;
	ds::vec3 velocity;
	ds::vec3 acceleration;
	bool shooting;
	float shootingAngle;
	PlayerSettings settings;
};



namespace player {

	void init(Player* player);

	void reset(Player* player);

	void handle_keys(Player* player, float dt);

	void move(Player* player, const ds::vec3& cursorPos,float dt);

	void move_to_center(Player* player, const ds::vec3& cursorPos, float dt);

}
