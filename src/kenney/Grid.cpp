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

int create_new_grid(int num_cells, const Entity& e, AmbientVertex** vertices) {
	int max = e.numVertices;
	int num = max * num_cells * num_cells;
	*vertices = new AmbientVertex[num];
	float sz = num_cells / 2.0f - 0.5f;
	float radius = 0.0f;
	for (int z = 0; z < num_cells; ++z) {
		float sx = num_cells / 2.0f - 0.5f;
		for (int x = 0; x < num_cells; ++x) {
			int idx = z * num_cells * max + x * max;
			for (int j = 0; j < max; ++j) {
				(*vertices + idx + j)->color = e.vertices[j].color;
				(*vertices + idx + j)->n = e.vertices[j].n;
				(*vertices + idx + j)->p = e.vertices[j].p;
				(*vertices + idx + j)->p.x += sx;
				(*vertices + idx + j)->p.z += sz;
				float ds = sqr_distance(ds::vec3(0.0f), (*vertices + idx + j)->p);
				if (ds > radius * radius) {
					radius = ds;
				}
			}
			sx -= 1.0f;
		}
		sz -= 1.0f;
	}
	radius = sqrtf(radius);
	radius = radius;
	return num;
}


int create_new_grid(int num_cells, float grid_size, const ds::Color & color, AmbientVertex** vertices) {

	float bs = 0.1f;
	float gs = grid_size - bs;

	ds::vec3 positions[] = { 
		ds::vec3(-gs - bs, 0.0f, gs + bs),
		ds::vec3(     -gs, 0.0f, gs + bs),
		ds::vec3(      gs, 0.0f, gs + bs),
		ds::vec3( gs + bs, 0.0f, gs + bs),
		ds::vec3(     -gs, 0.0f, gs),
		ds::vec3(      gs, 0.0f, gs),
		ds::vec3(     -gs, 0.0f, -gs),
		ds::vec3(      gs, 0.0f, -gs),
		ds::vec3(-gs - bs, 0.0f, -gs - bs),
		ds::vec3(     -gs, 0.0f, -gs - bs),
		ds::vec3(      gs, 0.0f, -gs - bs),
		ds::vec3( gs + bs, 0.0f, -gs - bs),
	};

	float depth = 0.05;

	float YP[] = {
		0.0f,0.0f,0.0f,0.0f,
		-depth,-depth,-depth,-depth,
		-depth,-depth,-depth,-depth,
		-depth,-depth,-depth,-depth,
		-depth,-depth,-depth,-depth,
		0.0f,0.0f,-depth,-depth
	};

	int indices[] = { 
		6,4,5,7, // center
		4,1,2,5, // top
		10,2,3,11, // right
		8,0,1,9, // left
		9,6,7,10,  // bottom
		6,7,7,6,
	};

	ds::Color clr[] = {
		ds::Color(1.0f,0.0f,0.0f,1.0f),
		ds::Color(0.0f,1.0f,0.0f,1.0f),
		ds::Color(0.0f,0.0f,1.0f,1.0f),
		ds::Color(1.0f,1.0f,0.0f,1.0f),
		ds::Color(0.0f,1.0f,1.0f,1.0f)
	};

	int max = 24;
	int num = num_cells * num_cells * max;
	*vertices = new AmbientVertex[num];
	float sz = num_cells / 2.0f - 0.5f;
	float radius = 0.0f;
	for (int z = 0; z < num_cells; ++z) {
		float sx = num_cells / 2.0f - 0.5f;
		for (int x = 0; x < num_cells; ++x) {
			int idx = z * num_cells * max + x * max;
			for (int j = 0; j < max; ++j) {
				(*vertices + idx + j)->color = color;// clr[j / 4];
				(*vertices + idx + j)->n = ds::vec3(0, 1, 0);
				(*vertices + idx + j)->p = positions[indices[j]];
				(*vertices + idx + j)->p.y = YP[j];
				if (j > 3) {
					(*vertices + idx + j)->color = ds::Color(0.15f,0.15f,0.15f,1.0f);
				}
				(*vertices + idx + j)->p.x += sx;
				(*vertices + idx + j)->p.z += sz;
				float ds = sqr_distance(ds::vec3(0.0f), (*vertices + idx + j)->p);
				if (ds > radius * radius) {
					radius = ds;
				}
			}
			idx += 16;
			sx -= 1.0f;
		}
		sz -= 1.0f;
	}
	radius = sqrtf(radius);
	radius = radius;
	return num;
}