#pragma once
#include "..\TestApp.h"
#include "..\kenney\Camera.h"
#include "..\utils\comon_math.h"
#include "..\instancing\InstanceCommon.h"
#include <vector>

struct GridPoint {
	ds::vec3 position;
	ds::vec3 old_pos;
	ds::vec3 velocity;
	float invMass;
	float damping;
	ds::vec3 acceleration;
	bool movable;
};

struct WarpingGridVertex {
	ds::vec3 pos;
	ds::Color color;
	ds::vec2 uv;
};

struct WarpingGridBuffer {
	float width;
	float base;
	float amplitude;
	float padding;
	ds::Color baseColor;
};

struct Spring {
	int sx;
	int sy;
	int ex;
	int ey;
	float targetLength;
	float stiffness;
	float damping;

};

const int GRID_SIZE_X = 40;
const int GRID_SIZE_Y = 30;
const int TOTAL_GRID_SIZE = GRID_SIZE_X * GRID_SIZE_Y;
const float GRID_DIM = 0.2f;

class WarpingGrid {

public:

	WarpingGrid(WarpingGridBuffer* settings);

	virtual ~WarpingGrid();

	bool init();

	void tick(float dt);

	void render(RID renderPass, const ds::matrix& viewProjectionMatrix);

	void applyForce(int x, int y, float radius, const ds::vec3& force);

	void applyForce(int x, int y, const ds::vec3& force);

	ds::vec3 getIntersectionPoint(const Ray& r);

private:
	void addSpring(int x1, int y1, int x2, int y2, float stiffness, float damping);
	GridPoint* _points;
	uint16_t* _indices;
	WarpingGridVertex* _vertices;
	RID _drawItem;
	InstanceBuffer _constantBuffer;
	RID _cubeBuffer;
	int _numVertices;
	std::vector<Spring> _springs;
	WarpingGridBuffer* _warpingGridBuffer;
	Plane _plane;
};