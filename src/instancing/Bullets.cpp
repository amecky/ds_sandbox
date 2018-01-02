#include "Bullets.h"
#include <ds_tweakable.h>
#include "..\utils\common_math.h"

namespace bullets {

	static void prepare_tweakables(Context* ctx) {

		twk_add("bullets", "velocity", &ctx->settings.velocity);
		twk_add("bullets", "scale", &ctx->settings.scale);
		twk_add("bullets", "growth", &ctx->settings.growth);
		twk_add("bullets", "bounding_box", &ctx->settings.boundingBox);
		twk_add("bullets", "fire_rate", &ctx->settings.fireRate);
		twk_add("bullets", "color", &ctx->settings.color);
		twk_add("bullets", "radius_offset", &ctx->settings.radius_offset);
		twk_add("bullets", "angle_offset", &ctx->settings.angle_offset);

	}

	void init(Context* ctx, RID textureID) {

		prepare_tweakables(ctx);

		ctx->bullet_timer = 0.0f;
		ctx->shooting = false;
		ctx->active = true;

		ctx->billboards.init(textureID);
	}


	// ----------------------------------------------------
	// add bullet
	// ----------------------------------------------------
	void add(Context* ctx, const ds::vec3& position, float angle, float radiusOffset, float angleOffset) {
		ds::vec3 p = position;
		p.x += cos(angle) * radiusOffset + cos(angle + ds::PI * 0.5f) * angleOffset;
		p.y += sin(angle) * radiusOffset + sin(angle + ds::PI * 0.5f) * angleOffset;
		p.z += 0.01f;
		Bullet b;
		b.pos = p;
		b.rotation = angle;
		b.velocity = ds::vec3(cosf(angle) * ctx->settings.velocity, sinf(angle) * ctx->settings.velocity, 0.0f);
		b.timer = 0.0f;
		b.scale = ds::vec2(1.0f);
		ctx->bullets.add(b);
	}

	// ----------------------------------------------------
	// manage bullets
	// ----------------------------------------------------
	void tick(Context* ctx, const ds::vec3& playerPosition, float shootingAngle, float dt, ds::EventStream* events) {

		if (ds::isMouseButtonPressed(0) && !ctx->shooting) {
			ctx->shooting = true;
		}
		if (!ds::isMouseButtonPressed(0) && ctx->shooting) {
			ctx->shooting = false;
		}


		// move bullets
		int cnt = 0;
		while (cnt < ctx->bullets.size()) {
			Bullet& b = ctx->bullets.get(cnt);
			b.pos += b.velocity * dt;
			b.timer += dt;
			b.scale.x = ctx->settings.scale.x + b.timer * ctx->settings.growth.x;
			b.scale.y = ctx->settings.scale.y + b.timer * ctx->settings.growth.y;
			ds::vec3 p = b.pos;
			if ( math::out_of_bounds(b.pos,ctx->settings.boundingBox)) {
			//if (p.y < ctx->settings.boundingBox.y || p.y > ctx->settings.boundingBox.w || p.x < ctx->settings.boundingBox.x || p.x > ctx->settings.boundingBox.z) {

				int ddy = (3.0f - p.y) / 6.0f * 20.0f;
				if (ddy < 0) {
					ddy = 0;
				}
				int ddx = 38.0f - (5.6f - p.x) / 11.2f * 38.0f;

				ds::vec2 pp(ddx, ddy);
				events->add(103, &pp, sizeof(ds::vec2));

				events->add(104, &p, sizeof(ds::vec3));

				ctx->bullets.remove(cnt);
			}
			else {
				++cnt;
			}
		}
		// create bullet if necessary
		if (ctx->shooting && ctx->active) {
			float freq = 1.0f / ctx->settings.fireRate;
			ctx->bullet_timer += dt;
			if (ctx->bullet_timer >= freq) {
				add(ctx, playerPosition, shootingAngle, ctx->settings.radius_offset,  ctx->settings.angle_offset);
				add(ctx, playerPosition, shootingAngle, ctx->settings.radius_offset, -ctx->settings.angle_offset);
				ctx->bullet_timer -= freq;
			}
		}
	}

	void render(Context* ctx, RID renderPass, const ds::matrix& viewProjectionMatrix, const ds::vec3& cursorPos) {

		ctx->billboards.begin();

		for (int i = 0; i < ctx->bullets.size(); ++i) {
			const Bullet& b = ctx->bullets.get(i);
			ctx->billboards.add(b.pos, ds::vec2(0.075f, 0.075f), ds::vec4(233, 7, 22, 22), b.rotation, b.scale, ctx->settings.color);
		}

		ctx->billboards.add(cursorPos, ds::vec2(0.2f, 0.2f), ds::vec4(550, 0, 64, 64));

		ctx->billboards.render(renderPass, viewProjectionMatrix);
	}

}