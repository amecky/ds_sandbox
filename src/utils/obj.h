#pragma once
#include <diesel.h>

class Mesh;

namespace obj {

	typedef struct color_definition_t {
		ds::Color color;
		uint32_t hash;
	} color_definition;

	typedef struct entry_t {
		ds::vec3 pos;
		ds::vec2 uvs;
		ds::vec3 n;
		ds::Color color;
	} entry;

	typedef struct face_t {
		entry entries[16];
		int num;
	} face;

	int load_material(const char* dir, const char* name, color_definition* defs, int max);

	void load(const char* dir, const char* name, Mesh* mesh);
}