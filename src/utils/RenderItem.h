#pragma once
#include <diesel.h>
#include "..\Material.h"
#include "..\Mesh.h"
#include "TransformComponent.h"

struct render_item {
	Mesh* mesh;
	transform transform;
	Material* material;
	RID draw_item;
};

void create_render_item(render_item* item, const char* objName, Material* m);

void draw_render_item(render_item* item, RID renderPass, const ds::matrix& viewProjectionMatrix);

struct RenderItemInstanceData {
	ds::matrix worldMatrix;
	ds::Color color;
};

typedef unsigned int ID;

struct instance_item {
	ID id;
	transform transform;
	ds::Color color;
};

struct item_index {
	ID id;
	unsigned short index;
	unsigned short next;
};

struct instanced_render_item {
	Mesh* mesh;
	//transform* transforms;
	Material* material;
	RID draw_item;
	RID instanceVertexBuffer;
	RenderItemInstanceData* instanceData;
	//int numInstances;

	unsigned int numObjects;
	item_index* indices;
	instance_item* objects;
	unsigned short free_enqueue;
	unsigned short free_dequeue;
};

bool contains_instance(instanced_render_item* item, ID id);

instance_item& get_instance(instanced_render_item* item, ID id);

ID add_instance(instanced_render_item* item);

void remove_instance(instanced_render_item* item, ID id);

void create_instanced_render_item(instanced_render_item* item, const char* objName, Material* m, int maxInstances);

void draw_instanced_render_item(instanced_render_item* item, RID renderPass, const ds::matrix& viewProjectionMatrix);
