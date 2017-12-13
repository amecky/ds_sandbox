#pragma once
#include <diesel.h>

typedef struct transform_t {
	ds::vec3 position;
	ds::vec3 scale;
	float roll;
	float pitch;
	float timer[4];
} transform;

typedef struct transform_component_t {
	ds::vec3 position;
	ds::vec3 scale;
	ds::vec3 rotation;
	float timer[3];
} transform_component;

void build_world_matrix(const transform_component& t, ds::matrix* w);

bool move_to(transform_component& t, const ds::vec3& start, const ds::vec3& end, float dt, float ttl);

bool step_forward(transform_component& t, const ds::vec3& start, const ds::vec3& end, float dt, float ttl);

bool step_forward_xy(transform_component& t, const ds::vec3& start, const ds::vec3& end, float dt, float ttl);

bool step_forward_xy(ds::vec3* t, const ds::vec3& start, const ds::vec3& end, float dt, float ttl);

bool rotate_to(transform_component* t, const ds::vec3& start, const ds::vec3& end, float dt, float ttl);

bool is_outside(const ds::vec3& pos, const ds::vec4& box);


void build_world_matrix(const transform& t, ds::matrix* w);

void initialize(transform* t, const ds::vec3& pos, const ds::vec3& scale, float roll, float pitch);

void reset_timers(transform* t);

bool pitch_to(transform* t, float start, float end, float dt, float ttl);

bool roll_to(transform* t, float start, float end, float dt, float ttl);

bool step_forward_xy(transform* t, const ds::vec3& start, const ds::vec3& end, float dt, float ttl);

bool float_in(transform* t, const ds::vec3& start, const ds::vec3& end, float dt, float ttl);

namespace anim {

	bool wiggle(transform* t, const ds::vec3& baseScale, float wiggleFactor, float dt, float ttl);

}


