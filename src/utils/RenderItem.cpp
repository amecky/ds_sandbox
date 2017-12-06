#include "RenderItem.h"

void create_render_item(render_item* item, const char* objName, Material* m) {
	item->mesh = new Mesh;
	item->mesh->loadData(objName);
	initialize(&item->transform, ds::vec3(0.0f), ds::vec3(1.0f), 0.0f, 0.0f);
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
	item->transforms = new transform[maxInstances];
	for (int i = 0; i < maxInstances; ++i) {
		initialize(&item->transforms[i], ds::vec3(0.0f), ds::vec3(1.0f), 0.0f, 0.0f);
	}
	item->material = m;

	RID cubeBuffer = item->mesh->assemble();

	ds::VertexBufferInfo ibInfo = { ds::BufferType::DYNAMIC, maxInstances, sizeof(RenderItemInstanceData) };
	item->instanceVertexBuffer = ds::createVertexBuffer(ibInfo);

	item->instanceData = new RenderItemInstanceData[maxInstances];
	item->numInstances = 0;

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
}

void draw_instanced_render_item(instanced_render_item* item, RID renderPass, const ds::matrix& viewProjectionMatrix) {
	
	ds::matrix world;
	for (int i = 0; i < item->numInstances; ++i) {
		const transform& t = item->transforms[i];
		build_world_matrix(t, &world);
		item->instanceData[i] = { ds::matTranspose(world), ds::Color(255,0,0,255) };
	}
	
	ds::mapBufferData(item->instanceVertexBuffer, item->instanceData, sizeof(RenderItemInstanceData) * item->numInstances);
	item->material->apply();
	item->material->transform(ds::matIdentity(), viewProjectionMatrix);
	ds::submit(renderPass, item->draw_item, -1, item->numInstances);
}