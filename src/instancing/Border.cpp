#include "Border.h"
#include "..\Mesh.h"
// ----------------------------------------------------
// init
// ----------------------------------------------------
void Border::init(RID basicGroup, RID vertexShaderId, RID pixelShaderId) {

	_lightBuffer.ambientColor = ds::Color(0.1f, 0.1f, 0.1f, 1.0f);
	_lightBuffer.diffuseColor = ds::Color(1.0f, 1.0f, 1.0f, 1.0f);
	_lightBuffer.lightDirection = ds::vec3(0.0f, 0.0f, 1.0f);
	_lightBuffer.padding = 0.0f;
	RID lbid = ds::createConstantBuffer(sizeof(InstanceLightBuffer), &_lightBuffer);

	Mesh mesh;
	mesh.loadBin("models\\hex_border.bin",false);
	_extent = mesh.getExtent();
	_center = mesh.getCenter();
	RID cubeBuffer = mesh.assemble();

	ds::VertexBufferInfo ibInfo = { ds::BufferType::DYNAMIC, 256, sizeof(InstanceData) };
	_instanceVertexBuffer = ds::createVertexBuffer(ibInfo);

	RID hexInstanceBuffer = ds::createInstancedBuffer(cubeBuffer, _instanceVertexBuffer);

	RID cbid = ds::createConstantBuffer(sizeof(InstanceBuffer), &_constantBuffer);

	RID stateGroup = ds::StateGroupBuilder()
		.constantBuffer(lbid, pixelShaderId, 0)
		.constantBuffer(cbid, vertexShaderId, 0)
		.instancedVertexBuffer(hexInstanceBuffer)
		.build();

	_drawCmd = { mesh.getCount(), ds::DrawType::DT_INSTANCED, ds::PrimitiveTypes::TRIANGLE_LIST, 256 };

	RID groups[] = { stateGroup,basicGroup };
	_drawItem = ds::compile(_drawCmd, groups, 2);

}

// ----------------------------------------------------
// tick
// ----------------------------------------------------
void Border::tick(float dt, const ds::vec3& playerPosition) {
	
}

// ----------------------------------------------------
// create cube
// ----------------------------------------------------
void Border::create(const ds::vec3& pos, float rotation) {
	if (_numItems < 256) {
		BorderItem& item = _items[_numItems++];
		item.color = ds::Color(48,0,0,255);
		item.pos = pos;
		item.timer = 0.0f;
		item.angle = rotation;
		item.scale = _scale;
	}
}

// ----------------------------------------------------
// render
// ----------------------------------------------------
void Border::render(RID renderPass, const ds::matrix viewProjectionMatrix) {
	for (int y = 0; y < _numItems; ++y) {
		BorderItem& item = _items[y];
		ds::matrix pm = ds::matTranslate(item.pos);
		//ds::matrix rx = ds::matRotationX(item.timer * 4.0f);
		ds::matrix rz = ds::matRotationZ(item.angle);
		ds::matrix sm = ds::matScale(ds::vec3(item.scale, item.scale, item.scale));
		ds::matrix world = sm * rz * pm;
		_instances[y] = { ds::matTranspose(world), item.color };		
	}
	ds::mapBufferData(_instanceVertexBuffer, _instances, sizeof(InstanceData) * _numItems);
	_constantBuffer.mvp = ds::matTranspose(viewProjectionMatrix);
	_constantBuffer.world = ds::matTranspose(ds::matIdentity());
	ds::submit(renderPass, _drawItem, -1, _numItems);
}
