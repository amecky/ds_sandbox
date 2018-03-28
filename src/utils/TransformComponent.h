#pragma once
#include <diesel.h>

struct transform {
	ds::vec3 position;
	ds::vec3 scale;
	ds::vec3 rotation;
};

void build_world_matrix(const transform& t, ds::matrix* w);

void initialize(transform* t, const ds::vec3& pos, const ds::vec3& scale, const ds::vec3& rotation);
