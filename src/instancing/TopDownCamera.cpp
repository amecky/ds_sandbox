#include "TopDownCamera.h"
#include <ds_tweakable.h>
#include "..\utils\tweening.h"

namespace camera {

	void init(GameCamera* gameCamera, ds::Camera* camera) {
		twk_add("camera", "regular_height", &gameCamera->settings.regular_height);
		twk_add("camera", "zoom_out_height", &gameCamera->settings.zoom_out_height);
		twk_add("camera", "zoom_ttl", &gameCamera->settings.zoom_ttl);
		gameCamera->position = ds::vec3(0.0f,0.0f,-6.0f);
		gameCamera->camera = camera;
		camera->position = gameCamera->position;
		ds::rebuildCamera(camera);
		gameCamera->zooming = false;
		gameCamera->zoom_timer = 0.0f;
	}

	static bool zoom(GameCamera* gameCamera, float startZ, float endZ, float dt) {
		if (gameCamera->zoom_timer <= gameCamera->settings.zoom_ttl && gameCamera->zooming) {
			float z = tweening::interpolate(tweening::linear, startZ, endZ, gameCamera->zoom_timer, gameCamera->settings.zoom_ttl);
			gameCamera->position.z = z;
			gameCamera->camera->position = gameCamera->position;
			ds::rebuildCamera(gameCamera->camera);
			gameCamera->zoom_timer += dt;
			return true;
		}
		return false;
	}

	bool zoom_out(GameCamera* gameCamera, float dt) {
		return zoom(gameCamera, gameCamera->settings.regular_height, gameCamera->settings.zoom_out_height, dt);
	}

	bool zoom_in(GameCamera* gameCamera, float dt) {
		return zoom(gameCamera, gameCamera->settings.zoom_out_height, gameCamera->settings.regular_height, dt);
	}

	void translate(GameCamera* gameCamera, const ds::vec3& position) {
		gameCamera->position = position;
		gameCamera->camera->position = gameCamera->position;
		ds::rebuildCamera(gameCamera->camera);
	}

	// ----------------------------------------------------
	// move camera
	// ----------------------------------------------------
	void follow(GameCamera* gameCamera, const ds::vec3& playerPosition, float dt) {
		ds::vec3 cameraTargetPos = playerPosition;
		cameraTargetPos.z = gameCamera->settings.regular_height;

		float velocity = 2.0f;
		ds::vec3 current = gameCamera->position;
		current.z = gameCamera->settings.regular_height;
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
		gameCamera->position = current;
		gameCamera->camera->position = gameCamera->position;
		ds::rebuildCamera(gameCamera->camera);
	}
}
