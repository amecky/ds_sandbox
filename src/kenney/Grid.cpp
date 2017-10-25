#include "Grid.h"

int create_grid(int num_cells, float grid_size, const ds::Color & color, AmbientVertex** vertices) {
	ds::vec3 positions[] = { ds::vec3(-grid_size,0.0f,-grid_size),ds::vec3(-grid_size,-0.0f,grid_size) ,ds::vec3(grid_size,-0.0f,grid_size) ,ds::vec3(grid_size,-0.0f,-grid_size) };
	ds::vec3 back_positions[] = { ds::vec3(-grid_size,-0.01,grid_size) ,ds::vec3(-grid_size,-0.01f,-grid_size),ds::vec3(grid_size,-0.01f,-grid_size),ds::vec3(grid_size,-0.01f,grid_size) };
	int num = num_cells * num_cells * 4 * 2;
	*vertices = new AmbientVertex[num_cells * num_cells * 4 * 2];
	float sz = num_cells / 2.0f - 0.5f;
	float radius = 0.0f;
	for (int z = 0; z < num_cells; ++z) {
		float sx = num_cells / 2.0f - 0.5f;
		for (int x = 0; x < num_cells; ++x) {
			int idx = z * num_cells * 4 * 2 + x * 4 * 2;
			for (int j = 0; j < 4; ++j) {
				(*vertices + idx + j)->p = positions[j];
				(*vertices + idx + j)->p.x += sx;
				(*vertices + idx + j)->p.z += sz;
				(*vertices + idx + j)->color = color;
				(*vertices + idx + j)->n = ds::vec3(0, 1, 0);

				float ds = sqr_distance(ds::vec3(0.0f), (*vertices + idx + j)->p);
				if (ds > radius * radius) {
					radius = ds;
				}
			}
			idx += 4;
			for (int j = 0; j < 4; ++j) {
				(*vertices + idx + j)->p = back_positions[j];
				(*vertices + idx + j)->p.x += sx;
				(*vertices + idx + j)->p.z += sz;
				(*vertices + idx + j)->color = color;
				(*vertices + idx + j)->n = ds::vec3(0, -1, 0);
			}
			sx -= 1.0f;
		}
		sz -= 1.0f;
	}
	radius = sqrtf(radius);
	radius = radius;
	return num;
}
