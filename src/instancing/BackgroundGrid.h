#pragma once
#include <diesel.h>
#include "InstanceCommon.h"
#include "..\utils\comon_math.h"

const int GRID_HEIGHT = 20;
const int GRID_WIDTH = 30;
const int GRID_TOTAL = GRID_HEIGHT * GRID_WIDTH;
const float GRID_SIZE = 0.3f;
const float HALF_GRID_SIZE = 0.15f;

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
};

struct BackgroundGridSettings {
	ds::Color borderColor;
	ds::Color gridColor;
	ds::Color flashColor;
	float flashTTL;
	float pulseTTL;
	float pulseAmplitude;
};

class BackgroundGrid {

public:
	BackgroundGrid(BackgroundGridSettings* settings) : _settings(settings) {}
	~BackgroundGrid() {}
	void tick(float dt);
	void init(RID basicGroup, RID vertexShaderId, RID pixelShaderId);
	void render(RID renderPass, const ds::matrix& viewProjectionMatrix);
	ds::vec3 convert_grid_coords(int x, int y);
	void highlight(int x, int y, BackgroundGridFlashing type = BGF_ONE);
	ds::vec3 getIntersectionPoint(const Ray& r);
private:
	BackgroundGridItem _items[GRID_TOTAL];
	RID _gridDrawItem;
	InstanceData _instances[GRID_TOTAL];
	RID _gridInstanceVertexBuffer;
	InstanceBuffer _constantBuffer;
	InstanceLightBuffer _lightBuffer;
	BackgroundGridSettings* _settings;
	Plane _plane;
};