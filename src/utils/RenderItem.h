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

struct instanced_render_item {
	Mesh* mesh;
	transform* transforms;
	Material* material;
	RID draw_item;
	RID instanceVertexBuffer;
	RenderItemInstanceData* instanceData;
	int numInstances;
};

void create_instanced_render_item(instanced_render_item* item, const char* objName, Material* m, int maxInstances);

void draw_instanced_render_item(instanced_render_item* item, RID renderPass, const ds::matrix& viewProjectionMatrix);
