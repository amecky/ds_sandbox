#include "TransformComponent.h"
#include <ds_tweening.h>

void build_world_matrix(const transform_component& t, ds::matrix* w) {
	if (w != 0) {
		ds::matrix scaleMatrix = ds::matScale(t.scale);
		ds::matrix translationMatrix = ds::matTranslate(t.position);
		ds::matrix rxMatrix = ds::matRotationX(t.rotation.x);
		ds::matrix ryMatrix = ds::matRotationY(t.rotation.y);
		ds::matrix rzMatrix = ds::matRotationZ(t.rotation.z);
		//*w = rzMatrix * rxMatrix * ryMatrix * scaleMatrix * translationMatrix;
		*w = scaleMatrix * rzMatrix * rxMatrix * ryMatrix * translationMatrix;
	}
}

void build_world_matrix(const transform& t, ds::matrix* w) {
	float x = sin(-t.roll);
	float y = cos(-t.roll);
	ds::matrix m = ds::matRotation(ds::vec3(x, y, 0.0f), t.pitch);
	ds::matrix rzm = ds::matRotationZ(t.roll);
	ds::matrix sm = ds::matScale(t.scale);
	*w = sm * rzm * m * ds::matTranslate(t.position);
}

void initialize(transform_component* t, const ds::vec3& pos, const ds::vec3& scale, const ds::vec3& rotation) {
	t->position = pos;
	t->scale = scale;
	t->rotation = rotation;
	for (int i = 0; i < 4; ++i) {
		t->timer[i] = 0.0f;
	}
}

bool move_to(transform_component & t, const ds::vec3 & start, const ds::vec3 & end, float dt, float ttl) {
	t.position = tweening::interpolate(tweening::easeOutBounce, start, end, t.timer[0], ttl);
	t.timer[0] += dt;
	if (t.timer[0] >= ttl) {
		t.position = end;
		return false;
	}
	return true;
}

bool step_forward(transform_component & t, const ds::vec3 & start, const ds::vec3 & end, float dt, float ttl) {
	t.position = tweening::interpolate(tweening::linear, start, end, t.timer[0], ttl);
	t.position.y = tweening::interpolate(tweening::easeSinus, start.y, end.y, t.timer[0], ttl);
	//t.position.z = tweening::interpolate(tweening::easeSinus, start.z, end.z, t.timer[0], ttl);
	t.timer[0] += dt;
	if (t.timer[0] >= ttl) {
		t.position = end;
		t.position.y = start.y;
		//t.position.z = start.z;
		return false;
	}
	return true;
}

bool step_forward_xy(transform_component & t, const ds::vec3 & start, const ds::vec3 & end, float dt, float ttl) {
	t.position = tweening::interpolate(tweening::linear, start, end, t.timer[0], ttl);
	t.position.z = tweening::interpolate(tweening::easeSinus, start.z, end.z, t.timer[0], ttl);
	t.timer[0] += dt;
	if (t.timer[0] >= ttl) {
		t.position = end;
		t.position.z = start.z;
		return false;
	}
	return true;
}

bool rotate_to(transform_component* t, const ds::vec3 & start, const ds::vec3 & end, float dt, float ttl) {
	t->rotation  = tweening::interpolate(tweening::linear, start, end, t->timer[2], ttl);
	t->timer[2] += dt;
	if (t->timer[2] >= ttl) {
		t->rotation = end;
		return false;
	}
	return true;
}

bool is_outside(const ds::vec3& pos, const ds::vec4& box) {
	return (pos.x < box.x || pos.x > box.z || pos.z < box.y || pos.z > box.w);
}





void reset_timers(transform* t) {
	for (int i = 0; i < 4; ++i) {
		t->timer[i] = 0.0f;
	}
}

void initialize(transform* t, const ds::vec3& pos, const ds::vec3& scale, float roll, float pitch) {
	t->position = pos;
	t->scale = scale;
	t->roll = roll;
	t->pitch = pitch;
	reset_timers(t);
}

bool pitch_to(transform* t, float start, float end, float dt, float ttl) {
	t->timer[3] += dt;
	t->pitch = tweening::interpolate(tweening::linear, start, end, t->timer[3], ttl);	
	if (t->timer[3] >= ttl) {
		t->pitch = end;
		return false;
	}
	return true;
}

bool roll_to(transform* t, float start, float end, float dt, float ttl) {
	t->timer[2] += dt;
	t->roll = tweening::interpolate(tweening::linear, start, end, t->timer[2], ttl);
	if (t->timer[2] >= ttl) {
		t->roll = end;
		return false;
	}
	return true;
}

bool step_forward_xy(transform* t, const ds::vec3& start, const ds::vec3& end, float dt, float ttl) {
	t->timer[0] += dt;
	t->position = tweening::interpolate(tweening::linear, start, end, t->timer[0], ttl);
	t->position.z = tweening::interpolate(tweening::easeSinus, start.z, end.z, t->timer[0], ttl);
	if (t->timer[0] >= ttl) {
		t->position = end;
		t->position.z = start.z;
		return false;
	}
	return true;
}

bool float_in(transform* t, const ds::vec3& start, const ds::vec3& end, float dt, float ttl) {
	t->timer[0] += dt;
	t->position.z = tweening::interpolate(tweening::linear, start.z, end.z, t->timer[0], ttl);
	if (t->timer[0] >= ttl) {
		t->position = end;
		return false;
	}
	return true;
}

namespace anim {

	bool wiggle(transform* t, const ds::vec3& baseScale, float wiggleFactor, float dt, float ttl) {
		float n = t->timer[1] / ttl;
		float wiggleScale = (1.0f - wiggleFactor) + fabs(sinf(n * ds::TWO_PI))  * wiggleFactor;
		t->scale.x = wiggleScale * baseScale.x;
		t->scale.y = wiggleScale * baseScale.y;
		t->timer[1] += dt;
		if (t->timer[1] >= ttl) {
			t->timer[1] = 0.0f;
			t->scale = baseScale;
			return false;
		}
		return true;
	}

	bool wiggle(transform* t, AnimationSettings* settings, float dt) {
		WiggleSettings* my_settings = (WiggleSettings*)settings;
		float n = t->timer[1] / my_settings->ttl;
		float wiggleScale = (1.0f - my_settings->wiggle_factor) + fabs(sinf(n * ds::TWO_PI))  * my_settings->wiggle_factor;
		t->scale.x = wiggleScale * my_settings->base_scale.x;
		t->scale.y = wiggleScale * my_settings->base_scale.y;
		t->timer[1] += dt;
		if (t->timer[1] >= my_settings->ttl) {
			t->timer[1] = 0.0f;
			t->scale = my_settings->base_scale;
			return false;
		}
		return true;
	}

	bool float_in(transform* t, AnimationSettings* settings, float dt) {
		FloatInSettings* my_settings = (FloatInSettings*)settings;
		t->position.z = tweening::interpolate(tweening::easeOutBounce, my_settings->start, my_settings->end, t->timer[0], my_settings->ttl);
		t->timer[0] += dt;
		if (t->timer[0] >= my_settings->ttl) {
			t->timer[0] = 0.0f;
			t->position.z = my_settings->end;
			return false;
		}
		return true;
	}

}

