#include "RenderItem.h"

#define INDEX_MASK 0xffff

RenderItem::RenderItem(const char* objName, Material* m, ds::matrix* world, bool force) {
	_mesh = new Mesh;
	_mesh->loadData(objName, world, force);
	initialize(&_transform, ds::vec3(0.0f), ds::vec3(1.0f), ds::vec3(0.0f));
	_material = m;
	RID hexCubeBuffer = _mesh->assemble();
	RID layout = _mesh->createInputLayout(m->getVertexShaderID());

	RID objGroup = ds::StateGroupBuilder()
		.inputLayout(layout)
		.vertexBuffer(hexCubeBuffer)
		.build();

	ds::DrawCommand objDrawCmd = { _mesh->getCount(), ds::DrawType::DT_VERTICES, ds::PrimitiveTypes::TRIANGLE_LIST };

	RID groups[] = { objGroup, m->getBasicGroupID() };
	_drawItem = ds::compile(objDrawCmd, groups, 2);
}

RenderItem::~RenderItem() {
	delete _mesh;
}

void RenderItem::draw(RID renderPass, const ds::matrix& viewProjectionMatrix) {
	ds::matrix world;
	build_world_matrix(_transform, &world);
	_material->apply();
	_material->transform(world, viewProjectionMatrix);
	ds::submit(renderPass, _drawItem);
}

void RenderItem::draw(RID renderPass, const ds::matrix& viewProjectionMatrix, const ds::matrix& world) {
	_material->apply();
	_material->transform(world, viewProjectionMatrix);
	ds::submit(renderPass, _drawItem);
}

void create_render_item(render_item* item, const char* objName, Material* m, ds::matrix* world) {
	item->mesh = new Mesh;
	item->mesh->loadData(objName, world);
	initialize(&item->transform, ds::vec3(0.0f), ds::vec3(1.0f), ds::vec3(0.0f));
	item->material = m;
	RID hexCubeBuffer = item->mesh->assemble();
	RID layout = item->mesh->createInputLayout(m->getVertexShaderID());

	RID objGroup = ds::StateGroupBuilder()
		.inputLayout(layout)
		.vertexBuffer(hexCubeBuffer)
		.build();

	ds::DrawCommand objDrawCmd = {item->mesh->getCount(), ds::DrawType::DT_VERTICES, ds::PrimitiveTypes::TRIANGLE_LIST };

	RID groups[] = { objGroup, m->getBasicGroupID() };
	item->draw_item = ds::compile(objDrawCmd, groups, 2);
}

void draw_render_item(render_item* item, RID renderPass, const ds::matrix& viewProjectionMatrix) {
	ds::matrix world;
	build_world_matrix(item->transform, &world);
	item->material->apply();
	item->material->transform(world, viewProjectionMatrix);
	ds::submit(renderPass, item->draw_item);
}

void create_instanced_render_item(instanced_render_item* item, const char* objName, Material* m, int maxInstances) {
	item->mesh = new Mesh;
	item->mesh->loadData(objName);
	//item->transforms = new transform[maxInstances];
	//for (int i = 0; i < maxInstances; ++i) {
		//initialize(&item->transforms[i], ds::vec3(0.0f), ds::vec3(1.0f), 0.0f, 0.0f);
	//}
	item->material = m;

	RID cubeBuffer = item->mesh->assemble();

	ds::VertexBufferInfo ibInfo = { ds::BufferType::DYNAMIC, maxInstances, sizeof(RenderItemInstanceData) };
	item->instanceVertexBuffer = ds::createVertexBuffer(ibInfo);

	item->instanceData = new RenderItemInstanceData[maxInstances];
	//item->numInstances = 0;

	RID hexInstanceBuffer = ds::createInstancedBuffer(cubeBuffer, item->instanceVertexBuffer);

	ds::InputLayoutDefinition decl[] = {
		{ "POSITION", 0, ds::BufferAttributeType::FLOAT3 },
		{ "COLOR"   , 0, ds::BufferAttributeType::FLOAT4 },
		{ "NORMAL"  , 0, ds::BufferAttributeType::FLOAT3 }
	};

	ds::InstancedInputLayoutDefinition instDecl[] = {
		{ "WORLD", 0, ds::BufferAttributeType::FLOAT4 },
		{ "WORLD", 1, ds::BufferAttributeType::FLOAT4 },
		{ "WORLD", 2, ds::BufferAttributeType::FLOAT4 },
		{ "WORLD", 3, ds::BufferAttributeType::FLOAT4 },
		{ "COLOR", 1, ds::BufferAttributeType::FLOAT4 }
	};
	ds::InstancedInputLayoutInfo iilInfo = { decl, 3, instDecl, 5, m->getVertexShaderID() };
	RID rid = ds::createInstancedInputLayout(iilInfo);

	RID stateGroup = ds::StateGroupBuilder()
		.inputLayout(rid)
		.instancedVertexBuffer(hexInstanceBuffer)
		.build();

	ds::DrawCommand drawCmd = { item->mesh->getCount(), ds::DrawType::DT_INSTANCED, ds::PrimitiveTypes::TRIANGLE_LIST, maxInstances };

	RID groups[] = { stateGroup, m->getBasicGroupID() };

	item->draw_item = ds::compile(drawCmd, groups, 2);

	item->numObjects = 0;
	item->indices = new item_index[maxInstances];
	item->objects = new instance_item[maxInstances];
	for (unsigned short i = 0; i < maxInstances; ++i) {
		item->indices[i].id = i;
		item->indices[i].next = i + 1;
		initialize(&item->objects[i].transform, ds::vec3(0.0f), ds::vec3(1.0f), ds::vec3(0.0f));
	}
	item->free_dequeue = 0;
	item->free_enqueue = maxInstances - 1;
}



void draw_instanced_render_item(instanced_render_item* item, RID renderPass, const ds::matrix& viewProjectionMatrix) {
	
	ds::matrix world;
	for (int i = 0; i < item->numObjects; ++i) {
		const transform& t = item->objects[i].transform;
		build_world_matrix(t, &world);
		item->instanceData[i] = { ds::matTranspose(world), item->objects[i].color };
	}
	
	ds::mapBufferData(item->instanceVertexBuffer, item->instanceData, sizeof(RenderItemInstanceData) * item->numObjects);
	item->material->apply();
	item->material->transform(ds::matIdentity(), viewProjectionMatrix);
	ds::submit(renderPass, item->draw_item, -1, item->numObjects);
}

bool contains_instance(instanced_render_item* item, ID id) {
	const item_index& in = item->indices[id & INDEX_MASK];
	return in.id == id && in.index != USHRT_MAX;
}

instance_item& get_instance(instanced_render_item* item, ID id) {
	unsigned short index = item->indices[id & INDEX_MASK].index;
	return item->objects[index];
}

ID add_instance(instanced_render_item* item) {
	item_index&in = item->indices[item->free_dequeue];
	item->free_dequeue = in.next;
	in.index = item->numObjects++;
	instance_item& o = item->objects[in.index];
	o.id = in.id;
	return o.id;
}

void remove_instance(instanced_render_item* item, ID id) {
	item_index& in = item->indices[id & INDEX_MASK];
	int current = in.index;
	instance_item& o = item->objects[in.index];
	o = item->objects[--item->numObjects];
	item->indices[o.id & INDEX_MASK].index = in.index;
	in.index = USHRT_MAX;
	item->indices[item->free_enqueue].next = id & INDEX_MASK;
	item->free_enqueue = id & INDEX_MASK;
}








InstancedRenderItem::InstancedRenderItem(const char* objName, Material* m, int maxInstances) {
	_maxInstances = maxInstances;
	_mesh = new Mesh;
	_mesh->loadData(objName);
	//item->transforms = new transform[maxInstances];
	//for (int i = 0; i < maxInstances; ++i) {
	//initialize(&item->transforms[i], ds::vec3(0.0f), ds::vec3(1.0f), 0.0f, 0.0f);
	//}
	_material = m;

	RID cubeBuffer = _mesh->assemble();

	ds::VertexBufferInfo ibInfo = { ds::BufferType::DYNAMIC, maxInstances, sizeof(RenderItemInstanceData) };
	_instanceVertexBuffer = ds::createVertexBuffer(ibInfo);

	_instanceData = new RenderItemInstanceData[maxInstances];
	//item->numInstances = 0;

	RID hexInstanceBuffer = ds::createInstancedBuffer(cubeBuffer, _instanceVertexBuffer);

	ds::InputLayoutDefinition decl[] = {
		{ "POSITION", 0, ds::BufferAttributeType::FLOAT3 },
		{ "COLOR"   , 0, ds::BufferAttributeType::FLOAT4 },
		{ "NORMAL"  , 0, ds::BufferAttributeType::FLOAT3 }
	};

	ds::InstancedInputLayoutDefinition instDecl[] = {
		{ "WORLD", 0, ds::BufferAttributeType::FLOAT4 },
		{ "WORLD", 1, ds::BufferAttributeType::FLOAT4 },
		{ "WORLD", 2, ds::BufferAttributeType::FLOAT4 },
		{ "WORLD", 3, ds::BufferAttributeType::FLOAT4 },
		{ "COLOR", 1, ds::BufferAttributeType::FLOAT4 }
	};
	ds::InstancedInputLayoutInfo iilInfo = { decl, 3, instDecl, 5, m->getVertexShaderID() };
	RID rid = ds::createInstancedInputLayout(iilInfo);

	RID stateGroup = ds::StateGroupBuilder()
		.inputLayout(rid)
		.instancedVertexBuffer(hexInstanceBuffer)
		.build();

	ds::DrawCommand drawCmd = { _mesh->getCount(), ds::DrawType::DT_INSTANCED, ds::PrimitiveTypes::TRIANGLE_LIST, maxInstances };

	RID groups[] = { stateGroup, m->getBasicGroupID() };

	_drawItem = ds::compile(drawCmd, groups, 2);

	_numObjects = 0;
	_indices = new item_index[maxInstances];
	_objects = new instance_item[maxInstances];
	for (unsigned short i = 0; i < maxInstances; ++i) {
		_indices[i].id = i;
		_indices[i].next = i + 1;
		initialize(&_objects[i].transform, ds::vec3(0.0f), ds::vec3(1.0f), ds::vec3(0.0f));
	}
	_free_dequeue = 0;
	_free_enqueue = maxInstances - 1;
}

InstancedRenderItem::~InstancedRenderItem() {
	delete _mesh;
	delete[] _instanceData;
	delete[] _indices;
	delete[] _objects;
}

void InstancedRenderItem::draw(RID renderPass, const ds::matrix& viewProjectionMatrix) {

	ds::matrix world;
	for (int i = 0; i < _numObjects; ++i) {
		const transform& t = _objects[i].transform;
		build_world_matrix(t, &world);
		_instanceData[i] = { ds::matTranspose(world), _objects[i].color };
	}

	ds::mapBufferData(_instanceVertexBuffer, _instanceData, sizeof(RenderItemInstanceData) * _numObjects);
	_material->apply();
	_material->transform(ds::matIdentity(), viewProjectionMatrix);
	ds::submit(renderPass, _drawItem, -1, _numObjects);
}

bool InstancedRenderItem::contains(ID id) {
	const item_index& in = _indices[id & INDEX_MASK];
	return in.id == id && in.index != USHRT_MAX;
}

instance_item& InstancedRenderItem::get(ID id) {
	unsigned short index = _indices[id & INDEX_MASK].index;
	return _objects[index];
}

ID InstancedRenderItem::add() {
	item_index&in = _indices[_free_dequeue];
	_free_dequeue = in.next;
	in.index = _numObjects++;
	instance_item& o = _objects[in.index];
	o.id = in.id;
	return o.id;
}

void InstancedRenderItem::remove(ID id) {
	item_index& in = _indices[id & INDEX_MASK];
	int current = in.index;
	instance_item& o = _objects[in.index];
	o = _objects[--_numObjects];
	_indices[o.id & INDEX_MASK].index = in.index;
	in.index = USHRT_MAX;
	_indices[_free_enqueue].next = id & INDEX_MASK;
	_free_enqueue = id & INDEX_MASK;
}