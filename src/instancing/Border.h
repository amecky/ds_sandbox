#pragma once
#include <diesel.h>
#include "..\utils\DynamicPath.h"
#include "InstanceCommon.h"
#include "..\utils\EventStream.h"

struct BorderItem {
	ds::vec3 pos;
	float scale;
	float angle;
	float timer;
	ds::Color color;
};

class Border {

public:
	Border() {}
	~Border() {}
	void init(RID basicGroup, RID vertexShaderId, RID pixelShaderId);
	void tick(float dt, const ds::vec3& playerPosition);
	void render(RID renderPass, const ds::matrix viewProjectionMatrix);	
	void create(const ds::vec3& pos);
private:
	BorderItem _items[256];
	int _numItems;
	RID _drawItem;
	InstanceLightBuffer _lightBuffer;
	InstanceData _instances[256];
	RID _instanceVertexBuffer;
	InstanceBuffer _constantBuffer;
	ds::DrawCommand _drawCmd;
};