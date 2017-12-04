#include "TransformComponent.h"
#include "tweening.h"

void build_world_matrix(const transform_component& t, ds::matrix* w) {
	if (w != 0) {
		ds::matrix scaleMatrix = ds::matScale(t.scale);
		ds::matrix translationMatrix = ds::matTranslate(t.position);
		ds::matrix rotationMatrix = ds::matRotation(t.rotation);
		*w = scaleMatrix * rotationMatrix * translationMatrix;
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


bool rotate_to(transform_component & t, const ds::vec3 & start, const ds::vec3 & end, float dt, float ttl) {
	t.rotation  = tweening::interpolate(tweening::linear, start, end, t.timer[2], ttl);
	t.timer[2] += dt;
	if (t.timer[2] >= ttl) {
		t.rotation = end;
		return false;
	}
	return true;
}

bool is_outside(const ds::vec3& pos, const ds::vec4& box) {
	return (pos.x < box.x || pos.x > box.z || pos.z < box.y || pos.z > box.w);
}