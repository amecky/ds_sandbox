#include "BackgroundGrid.h"
#include "..\Mesh.h"
#include "..\utils\tweening.h"

ds::vec3 BackgroundGrid::convert_grid_coords(int x, int y) {
	float cx = static_cast<float>(GRID_WIDTH) * GRID_SIZE * 0.5f - HALF_GRID_SIZE;
	float cy = static_cast<float>(GRID_HEIGHT) * GRID_SIZE * 0.5f - HALF_GRID_SIZE;
	return ds::vec3(-cx + static_cast<float>(x) * GRID_SIZE, -cy + static_cast<float>(y) * GRID_SIZE, 0.0f);
}

void BackgroundGrid::init(RID basicGroup, RID vertexShaderId, RID pixelShaderId) {
	_plane = Plane(ds::vec3(0.0f, 0.0f, -0.1f), ds::vec3(0.0f, 0.0f, -1.0f));
	for (int y = 0; y < GRID_HEIGHT; ++y) {
		for (int x = 0; x < GRID_WIDTH; ++x) {
			BackgroundGridItem& item = _items[x + y * GRID_WIDTH];
			ds::vec3 np = convert_grid_coords(x, y);
			item.color = _settings->gridColor;
			np.z = 0.1f + ds::random(-0.05f, 0.05f);
			if (x == 0 || x == (GRID_WIDTH - 1) || y == 0 || y == (GRID_HEIGHT - 1)) {
				np.z = -0.5f + ds::random(-0.1f, 0.1f);
				item.color = _settings->borderColor;
			}			
			item.world = ds::matTranslate(np);
			item.timer = 0.0f;
			item.type = BGF_NONE;
		}
	}

	_lightBuffer.ambientColor = ds::Color(0.3f, 0.3f, 0.3f, 1.0f);
	_lightBuffer.diffuseColor = ds::Color(1.0f, 1.0f, 1.0f, 1.0f);
	_lightBuffer.lightDirection = ds::vec3(0.0f, 0.0f, 1.0f);
	_lightBuffer.padding = 0.0f;
	RID lbid = ds::createConstantBuffer(sizeof(InstanceLightBuffer), &_lightBuffer);

	Mesh hexMesh;
	hexMesh.loadBin("models\\box.bin", false);
	//hexMesh.align();
	RID hexCubeBuffer = hexMesh.assemble();

	ds::VertexBufferInfo ibInfo = { ds::BufferType::DYNAMIC, GRID_TOTAL, sizeof(InstanceData) };
	_gridInstanceVertexBuffer = ds::createVertexBuffer(ibInfo);

	RID hexInstanceBuffer = ds::createInstancedBuffer(hexCubeBuffer, _gridInstanceVertexBuffer);
	
	RID cbid = ds::createConstantBuffer(sizeof(InstanceBuffer), &_constantBuffer);

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
	ds::mapBufferData(_gridInstanceVertexBuffer, _instances, sizeof(InstanceData) * GRID_TOTAL);
	_constantBuffer.mvp = ds::matTranspose(viewProjectionMatrix);
	_constantBuffer.world = ds::matTranspose(ds::matIdentity());

	ds::submit(renderPass, _gridDrawItem);
}

void BackgroundGrid::highlight(int x, int y, BackgroundGridFlashing type) {
	if (type == BGF_ONE) {
		_items[x + y * GRID_WIDTH].timer = _settings->flashTTL;
	}
	else {
		_items[x + y * GRID_WIDTH].timer = _settings->pulseTTL;
	}
	_items[x + y * GRID_WIDTH].type = type;
}

ds::vec3 BackgroundGrid::getIntersectionPoint(const Ray & r) {
	return _plane.getIntersection(r);
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
				item.color = tweening::interpolate(tweening::linear, _settings->gridColor, _settings->flashColor, item.timer, _settings->flashTTL);
			}
			if (item.type == BGF_PULSE) {
				item.timer -= dt;
				if (item.timer < 0.0f) {
					item.timer = 0.0f;
				}
				item.color = tweening::interpolate(tweening::easeSinus, _settings->gridColor, _settings->flashColor, item.timer, _settings->pulseTTL);
					//_settings->gridColor.data[j] + abs(sinf(item.timer / _settings->pulseTTL * _settings->pulseAmplitude * ds::TWO_PI)) * _settings->flashColor.data[j];
			}
		}
	}
}