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

struct Spring {
	int sx;
	int sy;
	int ex;
	int ey;
	float targetLength;
	float stiffness;
	float damping;

};

const int GRID_SIZE_X = 20;
const int GRID_SIZE_Y = 15;
const int TOTAL_GRID_SIZE = GRID_SIZE_X * GRID_SIZE_Y;
const float GRID_DIM = 0.2f;

class WarpingGrid : public TestApp {

public:
	WarpingGrid();

	virtual ~WarpingGrid();

	ds::RenderSettings getRenderSettings();

	bool usesGUI() {
		return true;
	}

	bool useFixedTimestep() const {
		return true;
	}

	bool init();

	void tick(float dt);

	void render();

	void renderGUI();

private:
	void addSpring(int x1, int y1, int x2, int y2, float stiffness, float damping);
	void applyForce(int x, int y, float radius, const ds::vec3& force);
	GridPoint* _points;
	uint16_t* _indices;
	WarpingGridVertex* _vertices;
	RID _drawItem;
	RID _basicPass;
	InstanceLightBuffer _lightBuffer;
	InstanceBuffer _constantBuffer;
	ds::Camera _camera;
	FPSCamera* _fpsCamera;
	ds::vec3 _lightPos;
	RID _cubeBuffer;
	int _numVertices;
	float _timer;
	std::vector<Spring> _springs;

	int _dbgX;
	int _dbgY;
	float _dbgForce;
};