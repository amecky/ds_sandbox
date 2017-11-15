#include "BackgroundGrid.h"
#include "..\Mesh.h"

ds::vec3 BackgroundGrid::grid_to_screen(int x, int y) {
	return ds::vec3(-2.8f + x * 0.32f, -2.1f + y * 0.32f, 0.0f);
}

void BackgroundGrid::init(RID basicGroup, RID vertexShaderId, RID pixelShaderId) {
	for (int y = 0; y < GRID_HEIGHT; ++y) {
		for (int x = 0; x < GRID_WIDTH; ++x) {
			BackgroundGridItem& item = _items[x + y * GRID_WIDTH];
			ds::vec3 np = grid_to_screen(x, y);
			np.z = 0.5f;
			item.color = ds::Color(51, 0, 0, 255);
			item.world = ds::matTranslate(np);
			item.timer = 0.0f;
			item.type = BGF_NONE;
		}
	}

	_lightBuffer.ambientColor = ds::Color(0.6f, 0.6f, 0.6f, 1.0f);
	_lightBuffer.diffuseColor = ds::Color(1.0f, 1.0f, 1.0f, 1.0f);
	_lightBuffer.lightDirection = ds::vec3(0.0f, 0.0f, 1.0f);
	_lightBuffer.padding = 0.0f;
	RID lbid = ds::createConstantBuffer(sizeof(InstanceLightBuffer), &_lightBuffer);

	Mesh hexMesh;
	hexMesh.loadBin("models\\floor.bin", false);
	RID hexCubeBuffer = hexMesh.assemble();

	ds::VertexBufferInfo ibInfo = { ds::BufferType::DYNAMIC, GRID_TOTAL, sizeof(BackgroundGridInstanceData) };
	_gridInstanceVertexBuffer = ds::createVertexBuffer(ibInfo);

	RID hexInstanceBuffer = ds::createInstancedBuffer(hexCubeBuffer, _gridInstanceVertexBuffer);
	
	RID cbid = ds::createConstantBuffer(sizeof(BackgroundGridInstanceBuffer), &_constantBuffer);

	RID gridGroup = ds::StateGroupBuilder()
		.constantBuffer(lbid, pixelShaderId, 0)
		.constantBuffer(cbid, vertexShaderId, 0)
		.instancedVertexBuffer(hexInstanceBuffer)
		.build();


	ds::DrawCommand gridDrawCmd = { hexMesh.getCount(), ds::DrawType::DT_INSTANCED, ds::PrimitiveTypes::TRIANGLE_LIST, GRID_TOTAL };

	RID gridGroups[] = { gridGroup,basicGroup };
	_gridDrawItem = ds::compile(gridDrawCmd, gridGroups, 2);

}

void BackgroundGrid::render(RID renderPass, const ds::matrix& viewProjectionMatrix) {
	for (int y = 0; y < GRID_TOTAL; ++y) {
		_instances[y] = { ds::matTranspose(_items[y].world), _items[y].color };
	}
	// map the instance data
	ds::mapBufferData(_gridInstanceVertexBuffer, _instances, sizeof(BackgroundGridInstanceData) * GRID_TOTAL);
	_constantBuffer.mvp = ds::matTranspose(viewProjectionMatrix);
	_constantBuffer.world = ds::matTranspose(ds::matIdentity());

	ds::submit(renderPass, _gridDrawItem);
}

void BackgroundGrid::highlight(int x, int y, float ttl, BackgroundGridFlashing type) {
	_items[x + y * GRID_WIDTH].timer = ttl;
	_items[x + y * GRID_WIDTH].type = type;
	_items[x + y * GRID_WIDTH].ttl = ttl;
}

void BackgroundGrid::tick(float dt) {
	for (int i = 0; i < GRID_TOTAL; ++i) {
		BackgroundGridItem& item = _items[i];
		if (item.type != BGF_NONE) {
			if ( item.type == BGF_ONE) {
				item.timer -= dt;
				if (item.timer < 0.0f) {
					item.timer = 0.0f;
				}
				item.color.r = 0.2f + (item.timer / item.ttl * 0.8f);
			}
			if (item.type == BGF_PULSE) {
				item.timer -= dt;
				if (item.timer < 0.0f) {
					item.timer = 0.0f;
				}
				item.color.r = 0.2f + abs(sinf(item.timer / item.ttl * 2.0f * ds::TWO_PI)) * 0.2f;
			}
		}
	}
}