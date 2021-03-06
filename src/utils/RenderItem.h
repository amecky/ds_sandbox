#pragma once
#include <diesel.h>
#include "..\Material.h"
#include "..\Mesh.h"
#include "TransformComponent.h"

class AbstractRenderItem {

public:
	virtual void draw(RID renderPass, const ds::matrix& viewProjectionMatrix) = 0;
	virtual void draw(RID renderPass, const ds::matrix& viewProjectionMatrix, const ds::matrix& world) = 0;

};

class RenderItem {

public:
	RenderItem(const char* objName, Material* m, ds::matrix* world = 0, bool force = false);
	~RenderItem();
	void draw(RID renderPass, const ds::matrix& viewProjectionMatrix);
	void draw(RID renderPass, const ds::matrix& viewProjectionMatrix, const ds::matrix& world);
	transform& getTransform() {
		return _transform;
	}
	const transform& getTransform() const {
		return _transform;
	}
	void setTransform(const transform& t) {
		_transform = t;
	}
	const ds::vec3& getExtent() const {
		return _mesh->getExtent();
	}
	const ds::vec3& getCenter() const {
		return _mesh->getCenter();
	}
	const ds::vec3& getMin() const {
		return _mesh->getMin();
	}
	const ds::vec3& getMax() const {
		return _mesh->getMax();
	}
private:
	Mesh* _mesh;
	transform _transform;
	Material* _material;
	RID _drawItem;
};

struct render_item {
	Mesh* mesh;
	transform transform;
	Material* material;
	RID draw_item;
};

void create_render_item(render_item* item, const char* objName, Material* m, ds::matrix* world = 0);

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

// ------------------------------------------------------------
// Instanced render item
// ------------------------------------------------------------
class InstancedRenderItem {

public:
	InstancedRenderItem(const char* objName, Material* m, int maxInstances);
	~InstancedRenderItem();
	bool contains(ID id);
	instance_item& get(ID id);
	ID add();
	void remove(ID id);
	void draw(RID renderPass, const ds::matrix& viewProjectionMatrix);
private:
	Mesh* _mesh;
	Material* _material;
	RID _drawItem;
	RID _instanceVertexBuffer;
	RenderItemInstanceData* _instanceData;
	int _maxInstances;
	unsigned int _numObjects;
	item_index* _indices;
	instance_item* _objects;
	unsigned short _free_enqueue;
	unsigned short _free_dequeue;
};


