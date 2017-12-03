#pragma once
#include <diesel.h>

typedef struct transform_component_t {
	ds::vec3 position;
	ds::vec3 scale;
	ds::vec3 rotation;
	float timer[3];
} transform_component;

void build_world_matrix(const transform_component& t, ds::matrix* w);

bool move_to(transform_component& t, const ds::vec3& start, const ds::vec3& end, float dt, float ttl);

bool rotate_to(transform_component& t, const ds::vec3& start, const ds::vec3& end, float dt, float ttl);