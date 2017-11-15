#pragma once
#include <diesel.h>
#include "InstanceCommon.h"

const int GRID_HEIGHT = 16;
const int GRID_WIDTH = 24;
const int GRID_TOTAL = GRID_HEIGHT * GRID_WIDTH;

enum BackgroundGridFlashing {
	BGF_NONE,
	BGF_ONE,
	BGF_PULSE
};

struct BackgroundGridItem {
	ds::matrix world;
	ds::Color color;
	float timer;
	BackgroundGridFlashing type;
	float ttl;
};

struct BackgroundGridInstanceBuffer {
	ds::matrix mvp;
	ds::matrix world;
};

struct BackgroundGridInstanceData {
	ds::matrix worldMatrix;
	ds::Color color;
};

class BackgroundGrid {

public:
	BackgroundGrid() {}
	~BackgroundGrid() {}
	void tick(float dt);
	void init(RID basicGroup, RID vertexShaderId, RID pixelShaderId);
	void render(RID renderPass, const ds::matrix& viewProjectionMatrix);
	ds::vec3 grid_to_screen(int x, int y);
	void highlight(int x, int y, float ttl, BackgroundGridFlashing type = BGF_ONE);
private:
	BackgroundGridItem _items[GRID_TOTAL];
	RID _gridDrawItem;
	BackgroundGridInstanceData _instances[GRID_TOTAL];
	RID _gridInstanceVertexBuffer;
	BackgroundGridInstanceBuffer _constantBuffer;
	InstanceLightBuffer _lightBuffer;
};