#pragma once
#include <diesel.h>
#include <cmath>

struct GameCameraSettings {
	float regular_height;
	float zoom_out_height;
	float zoom_ttl;
};

struct GameCamera {
	ds::Camera* camera;
	GameCameraSettings settings;
	ds::vec3 position;
	float zoom_timer;
	bool zooming;
};

namespace camera {

	void init(GameCamera* gameCamera, ds::Camera* camera);

	bool zoom_out(GameCamera* gameCamera, float dt);

	bool zoom_in(GameCamera* gameCamera, float dt);

	void follow(GameCamera* gameCamera, const ds::vec3& playerPosition, float dt);

	void translate(GameCamera* gameCamera, const ds::vec3& position);
}
