#pragma once
#include "..\TestApp.h"
#include "..\instancing\InstanceCommon.h"
#include "..\Mesh.h"
#include "..\kenney\Camera.h"

struct GridSettingsBuffer {
	float amplitude;
	float base;
	float one;
	float two;
};

enum SimpleGridDirection {
	SGD_XZ,
	SGD_XY,
	SGD_YZ
};

class SimpleGrid {

public:
	SimpleGrid();

	virtual ~SimpleGrid();

	bool init(SimpleGridDirection direction, float gridSteps, float base, float amplitude);

	void render(RID renderPass, const ds::matrix& viewProjectionMatrix);

private:
	RID _gridDrawItem;
	InstanceBuffer _constantBuffer;
	GridSettingsBuffer _settingsBuffer;
	Mesh _grid;
};