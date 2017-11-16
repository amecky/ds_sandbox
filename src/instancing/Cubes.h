#pragma once
#include <diesel.h>
#include "..\utils\DynamicPath.h"
#include "InstanceCommon.h"

struct Animation {
	int type;
	float timer;
	float ttl;
};

struct GridItem {
	ds::vec3 pos;
	ds::vec3 velocity;
	float scale;
	float angle;
	float timer;
	ds::Color color;
	int type;
	int animationFlags;
	Animation animations[16];
	int numAnimations;
};

class Cubes {

public:
	Cubes() {}
	~Cubes() {}
	void init(RID basicGroup, RID vertexShaderId, RID pixelShaderId);
	void tick(float dt);
	void render(RID renderPass, const ds::matrix viewProjectionMatrix);
	void create(const ds::vec3& pos, int animationFlags);
private:
	ds::FloatPath _scalePath;
	GridItem _items[256];
	int _numItems;
	RID _drawItem;
	InstanceLightBuffer _lightBuffer;
	InstanceData _instances[256];
	RID _instanceVertexBuffer;
	InstanceBuffer _constantBuffer;
};