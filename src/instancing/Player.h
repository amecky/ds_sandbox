#pragma once
#include <diesel.h>
#include "TopDownCamera.h"
#include "..\utils\TransformComponent.h"
#include "..\utils\RenderItem.h"

struct Player {
	render_item render_item;
	ds::vec3 velocity;
	ds::vec3 acceleration;
	bool shooting;
	float shootingAngle;
};

namespace player {

	void reset(Player* player);

	void handle_keys(Player* player, float dt);

	void move(Player* player, const ds::vec3& cursorPos, float dt);

}
