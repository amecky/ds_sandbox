#include "particle_effects.h"
#include "..\common\common.h"
#include "ParticleManager.h"

const static ds::vec4 STAR_TEXTURE = ds::vec4(40, 50, 16, 16);
const static ds::vec4 TEXTURE = ds::vec4(0, 75, 26, 26);
// -------------------------------------------------------
// emitt partciles for filled segment
// -------------------------------------------------------
void showFilled(RenderEnvironment* env, int segment, int numSegments, const ds::Color& clr) {
	float rotationStep = ds::TWO_PI / static_cast<float>(segment);
	float step = ds::TWO_PI / static_cast<float>(numSegments) / 16.0f;
	float current = ds::TWO_PI / static_cast<float>(numSegments) * static_cast<float>(segment);
	float var = step * 0.1f;
	ParticleDesc desc[16];
	for (int i = 0; i < 16; ++i) {
		float angle = current + ds::random(-var, var);
		desc[i].position = ds::vec2(cosf(angle), sinf(angle)) * 315.0f + ds::vec2(512, 384);
		desc[i].acceleration = ds::vec2(0.0f);
		desc[i].textureRect = TEXTURE;
		desc[i].rotation = 0.0f;
		desc[i].rotationSpeed = 0.0f;
		desc[i].startColor = clr;
		desc[i].endColor = ds::Color(clr.r, clr.g, clr.b, 0.2f);
		desc[i].scale = ds::vec2(ds::random(0.6f, 0.9f));
		desc[i].ttl = ds::random(0.5f, 0.8f);
		angle = current + ds::random(-ds::PI * 0.5f, ds::PI * 0.5f);
		desc[i].velocity = ds::vec2(cosf(angle), sinf(angle)) * ds::random(-150.0f, 150.0f);
		current += step;
	}
	env->particles->add(desc, 16);
}

// -------------------------------------------------------
// emitt partciles for dying bullets
// -------------------------------------------------------
void rebound(RenderEnvironment* env, const ds::vec2& p, float angle, const ds::Color& clr) {
	float step = ds::PI * 0.75f / 16.0f;
	float current = angle - ds::PI * 0.25f + ds::PI;
	float var = step * 0.1f;
	ParticleDesc desc[16];
	for (int i = 0; i < 16; ++i) {
		float angle = current + ds::random(-var, var);
		desc[i].position = ds::vec2(cosf(angle), sinf(angle)) + p;
		desc[i].acceleration = ds::vec2(0.0f);
		desc[i].textureRect = STAR_TEXTURE;
		desc[i].rotation = 0.0f;
		desc[i].rotationSpeed = 0.0f;
		desc[i].startColor = clr;
		desc[i].endColor = ds::Color(clr.r, clr.g, clr.b, 0.2f);
		desc[i].scale = ds::vec2(ds::random(0.6f, 0.9f));
		desc[i].ttl = ds::random(0.5f, 0.8f);
		desc[i].velocity = ds::vec2(cosf(angle), sinf(angle)) * ds::random(150.0f, 250.0f);
		current += step;
	}
	env->particles->add(desc, 16);
}