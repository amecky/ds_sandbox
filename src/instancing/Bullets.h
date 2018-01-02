#pragma once
#include <diesel.h>
#include "..\utils\fixed_array.h"
#include "Billboards.h"
#include "..\utils\EventStream.h"

namespace bullets {

	struct Settings {
		float velocity;
		ds::vec2 growth;
		ds::vec2 scale;
		ds::vec4 boundingBox;
		float fireRate;
		ds::Color color;
		float radius_offset;
		float angle_offset;
	};

	struct Bullet {
		ds::vec3 pos;
		ds::vec3 velocity;
		float rotation;
		float timer;
		ds::vec2 scale;
	};

	typedef fixed_array<Bullet> Bullets;

	struct Context {
		Settings settings;
		Bullets bullets;
		bool shooting;
		float bullet_timer;
		Billboards billboards;
		bool active;
	};


	void init(Context* ctx, RID textureID);

	void add(Context* ctx, const ds::vec3& position, float angle, float radiusOffset, float angleOffset);

	void tick(Context* ctx, const ds::vec3& playerPosition, float shootingAngle, float dt, ds::EventStream* events);

	void render(Context* ctx, RID renderPass, const ds::matrix& viewProjectionMatrix, const ds::vec3& cursorPos);

}