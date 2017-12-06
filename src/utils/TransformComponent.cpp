#include "TransformComponent.h"
#include "tweening.h"

void build_world_matrix(const transform_component& t, ds::matrix* w) {
	if (w != 0) {
		ds::matrix scaleMatrix = ds::matScale(t.scale);
		ds::matrix translationMatrix = ds::matTranslate(t.position);
		ds::matrix rxMatrix = ds::matRotationX(t.rotation.x);
		ds::matrix ryMatrix = ds::matRotationY(t.rotation.y);
		ds::matrix rzMatrix = ds::matRotationZ(t.rotation.z);
		*w = rzMatrix * rxMatrix * ryMatrix * scaleMatrix * translationMatrix;
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

