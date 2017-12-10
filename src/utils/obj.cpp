#include "obj.h"
#include <fstream>
#include <string>
#include "..\Mesh.h"

namespace obj {

	// -------------------------------------------------------
	// Hashing
	// -------------------------------------------------------
	const uint32_t OBJ__FNV_Prime = 0x01000193;
	const uint32_t OBJ__FNV_Seed = 0x811C9DC5;

	inline uint32_t obj__fnv1a(const char* text, uint32_t hash = OBJ__FNV_Seed) {
		const unsigned char* ptr = (const unsigned char*)text;
		while (*ptr) {
			hash = (*ptr++ ^ hash) * OBJ__FNV_Prime;
		}
		return hash;
	}

	static bool starts_with(const char* text, const std::string& line) {
		int l = strlen(text);
		if (l > line.length()) {
			return false;
		}
		for (int i = 0; i < l; ++i) {
			if (text[i] != line[i]) {
				return false;
			}
		}
		return true;
	}


	// ------------------------------------------------------------------
	// is numeric
	// ------------------------------------------------------------------
	static bool is_numeric(const char c) {
		return ((c >= '0' && c <= '9'));
	}

	// ------------------------------------------------------------------
	// is whitespace
	// ------------------------------------------------------------------
	static bool is_whitespace(const char c) {
		if (c == ' ' || c == '\t' || c == '\r' || c == '\n') {
			return true;
		}
		return false;
	}

	// ------------------------------------------------------------------
	// strtof
	// ------------------------------------------------------------------
	static float my_strtof(const char* p, char** endPtr) {
		while (is_whitespace(*p)) {
			++p;
		}
		float sign = 1.0f;
		if (*p == '-') {
			sign = -1.0f;
			++p;
		}
		else if (*p == '+') {
			++p;
		}
		float value = 0.0f;
		while (is_numeric(*p)) {
			value *= 10.0f;
			value = value + (*p - '0');
			++p;
		}
		if (*p == '.') {
			++p;
			float dec = 1.0f;
			float frac = 0.0f;
			while (is_numeric(*p)) {
				frac *= 10.0f;
				frac = frac + (*p - '0');
				dec *= 10.0f;
				++p;
			}
			value = value + (frac / dec);
		}
		if (*p == 'E' || *p == 'e') {
			++p;
			float nextSign = 1.0f;
			if (*p == '-') {
				nextSign = -1.0f;
				++p;
			}
			else if (*p == '+') {
				++p;
			}
			float exp = 0.0f;
			while (is_numeric(*p)) {
				exp *= 10.0f;
				exp = exp + (*p - '0');
				++p;
			}
			exp *= nextSign;
			value = value * pow(10.0f, exp);
		}
		if (endPtr) {
			*endPtr = (char *)(p);
		}
		return value * sign;
	}

	// ------------------------------------------------------------------
	// strtof
	// ------------------------------------------------------------------
	static float my_strtoi(const char* p, char** endPtr) {
		while (is_whitespace(*p)) {
			++p;
		}
		int sign = 1;
		if (*p == '-') {
			sign = -1;
			++p;
		}
		else if (*p == '+') {
			++p;
		}
		int value = 0;
		while (is_numeric(*p)) {
			value *= 10;
			value = value + (*p - '0');
			++p;
		}
		if (endPtr) {
			*endPtr = (char *)(p);
		}
		return value * sign;
	}

	static bool convert(const char* line, int offset, float* data, int max) {
		const char* ll = line;
		ll += offset;
		char *out;
		for (int i = 0; i < max; ++i) {
			// FIXME: check if there is data
			data[i] = my_strtof(ll, &out);
			ll = out;
			++ll;
		}
		return true;
	}

	static ds::vec3 convert_to_vec3(const char* line, int offset) {
		float v[3];
		convert(line, offset, v, 3);
		ds::vec3 vv;
		for (int i = 0; i < 3; ++i) {
			vv.data[i] = v[i];
		}
		return vv;
	}

	static ds::vec2 convert_to_vec2(const char* line, int offset) {
		float v[2];
		convert(line, offset, v, 2);
		ds::vec2 vv;
		for (int i = 0; i < 2; ++i) {
			vv.data[i] = v[i];
		}
		return vv;
	}

	static int parse_face(const char* cmd, int* v, int max) {
		const char* p = cmd;
		int cnt = 0;
		for (int i = 0; i < max; ++i) {
			v[i] = -1;
		}
		while (*p != 0) {
			if (*p >= '0' && *p <= '9') {
				char *out;
				int d = my_strtoi(p, &out);
				if (cnt < max) {
					v[cnt] = d;
				}
				p = out;
			}
			else if (*p == '/' || *p == ' ') {
				++p;
				++cnt;
			}
			else {
				++p;
			}
		}
		return cnt + 1;
	}

	int load_material(const char* dir, const char* name, color_definition* defs, int max) {
		char fileName[256];
		sprintf_s(fileName, "%s\\%s", dir, name);
		std::ifstream file(fileName);
		std::string curline;
		uint32_t current_hash = 0;
		uint16_t num = 0;
		while (std::getline(file, curline)) {
			if (starts_with("#", curline)) {

			}
			else if (starts_with("newmtl ", curline)) {
				const char* name = curline.c_str();
				name += 7;
				current_hash = obj__fnv1a(name);
			}
			else if (starts_with("Kd ", curline)) {
				ds::vec3 v = convert_to_vec3(curline.c_str(), 3);
				if (num < max) {
					color_definition* cd = &defs[num++];
					for (int i = 0; i < 3; ++i) {
						cd->color.data[i] = v.data[i];
					}
					cd->color.a = 1.0f;
					cd->hash = current_hash;
				}
			}
		}
		return num;
	}

	void load(const char* dir, const char* name, Mesh* mesh, ds::matrix* world) {
		char fileName[256];
		sprintf_s(fileName, "%s\\%s", dir, name);
		std::ifstream file(fileName);
		std::string curline;
		std::vector<ds::vec3> vertices;
		std::vector<ds::vec3> normals;
		std::vector<ds::vec2> uvs;
		std::vector<obj::face> faces;
		obj::color_definition defs[32];
		int num_materials = 0;
		int current_material = -1;
		int entries[48];
		while (std::getline(file, curline)) {
			if (starts_with("#", curline)) {

			}
			else if (starts_with("usemtl ", curline)) {
				const char* name = curline.c_str();
				name += 7;
				uint32_t hash = obj__fnv1a(name);
				current_material = -1;
				for (int i = 0; i < num_materials; ++i) {
					if (defs[i].hash == hash) {
						current_material = i;
					}
				}
			}
			else if (starts_with("mtllib ", curline)) {
				const char* name = curline.c_str();
				name += 7;
				num_materials = obj::load_material(dir, name, defs, 32);
			}
			else if (starts_with("v ", curline)) {
				ds::vec3 v = convert_to_vec3(curline.c_str(), 2);
				vertices.push_back(v);

			}
			else if (starts_with("vn ", curline)) {
				ds::vec3 v = convert_to_vec3(curline.c_str(), 3);
				normals.push_back(v);
			}
			else if (starts_with("vt ", curline)) {
				ds::vec2 v = convert_to_vec2(curline.c_str(), 3);
				uvs.push_back(v);
			}
			else if (starts_with("f ", curline)) {
				// f 27/3/1 26/2/1 25/1/1
				obj::face f;
				const char* cmd = curline.c_str();
				cmd += 2;
				int num = parse_face(cmd, entries, 48);
				int steps = num / 3;
				f.num = steps;
				for (int i = 0; i < steps; ++i) {
					obj::entry* e = &f.entries[i];
					int v = entries[i * 3];
					int t = entries[i * 3 + 1];
					int n = entries[i * 3 + 2];
					if (v != -1 && (v - 1) < vertices.size()) {
						e->pos = vertices[v - 1];
					}
					if (t != -1 && (t - 1) < uvs.size()) {
						e->uvs = uvs[t - 1];
					}
					if (n != -1 && (n - 1) < normals.size()) {
						e->n = normals[n - 1];
					}
					if (current_material != -1) {
						e->color = defs[current_material].color;
					}
					else {
						e->color = ds::Color(255, 255, 255, 255);
					}
				}
				faces.push_back(f);
			}
		}
		// triangulate data

		int idx_ar[] = { 0,1,2,2,3,0 };
		int sx_ar[] = { 0,1,2,0,2,3,0,3,4,0,4,5,0,5,6,0,6,7,0,7,8,0,8,9,0,9,10,0,10,11,0,11,12,0,12,13,0,13,14,0,14,15 };

		int total = 0;
		for (size_t i = 0; i < faces.size(); ++i) {
			total += (faces[i].num - 2) * 3;
		}

		ds::vec3* verts = new ds::vec3[total];
		ds::vec3* ns = new ds::vec3[total];
		ds::vec2* tuv = new ds::vec2[total];
		ds::Color* colors = new ds::Color[total];
		int current = 0;
		float dd = 7.0f;
		for (size_t i = 0; i < faces.size(); ++i) {
			const obj::face& f = faces[i];
			int end = (f.num - 2) * 3;
			for (int j = 0; j < end; ++j) {
				if (world != 0) {
					verts[current] = f.entries[sx_ar[j]].pos * *world;
					ns[current] = f.entries[sx_ar[j]].n * *world;
				}
				else {
					verts[current] = f.entries[sx_ar[j]].pos;
					ns[current] = f.entries[sx_ar[j]].n;
				}
				tuv[current] = f.entries[sx_ar[j]].uvs;
				colors[current] = f.entries[sx_ar[j]].color;
				++current;
			}
		}
		mesh->addStream(AT_VERTEX, (float*)verts, total, 3);
		mesh->addStream(AT_COLOR, (float*)colors, total, 4);
		mesh->addStream(AT_NORMAL, (float*)ns, total, 3);
	}


}