#pragma once
#include <diesel.h>
#include "..\utils\DynamicPath.h"
#include "InstanceCommon.h"
#include "..\utils\EventStream.h"

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
	ds::vec3 force;
};

class Cubes {

public:
	Cubes() {}
	~Cubes() {}
	void init(RID basicGroup, RID vertexShaderId, RID pixelShaderId);
	void tick(float dt, const ds::vec3& playerPosition);
	void render(RID renderPass, const ds::matrix viewProjectionMatrix);
	void create(const ds::vec3& pos, int animationFlags);
	int checkCollisions(Bullet* bullets, int num, ds::EventStream* events);
	int getNumItems() const {
		return _numItems;
	}
private:
	void separate(float minDistance, float relaxation);
	ds::FloatPath _scalePath;
	GridItem _items[256];
	int _numItems;
	RID _drawItem;
	InstanceLightBuffer _lightBuffer;
	InstanceData _instances[256];
	RID _instanceVertexBuffer;
	InstanceBuffer _constantBuffer;
	ds::DrawCommand _drawCmd;
};