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
	Border() : _scale(0.4f) {}
	~Border() {}
	void init(RID basicGroup, RID vertexShaderId, RID pixelShaderId);
	void tick(float dt, const ds::vec3& playerPosition);
	void render(RID renderPass, const ds::matrix viewProjectionMatrix);	
	void create(const ds::vec3& pos, float rotation = 0.0f);
	const ds::vec3& getItemExtent() const {
		return _extent;
	}
	const ds::vec3& getItemCenter() const {
		return _center;
	}
	float getScale() const {
		return _scale;
	}
private:
	BorderItem _items[256];
	int _numItems;
	RID _drawItem;
	InstanceLightBuffer _lightBuffer;
	InstanceData _instances[256];
	RID _instanceVertexBuffer;
	InstanceBuffer _constantBuffer;
	ds::DrawCommand _drawCmd;
	ds::vec3 _extent;
	ds::vec3 _center;
	float _scale;
};