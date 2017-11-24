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

	float timer;
	bool marked;
	ds::Color color;
};

struct WarpingGridVertex {
	ds::vec3 pos;
	ds::Color color;
};

struct WarpingGridSettings {	
	int numX;
	int numY;
	float size;
	float borderSize;
	float zOffset;
	ds::Color color;
	float stiffness;// = 0.28f;
	float damping;// = 0.06f;
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

class WarpingGrid {

public:

	WarpingGrid(const WarpingGridSettings& settings);

	virtual ~WarpingGrid();

	bool init();

	void tick(float dt);

	void render(RID renderPass, const ds::matrix& viewProjectionMatrix);

	void applyForce(int x, int y, float radius, const ds::vec3& force);

	void applyForce(const ds::vec3& center, float radius, const ds::vec3& force);

	void applyForce(int x, int y, const ds::vec3& force);

	ds::vec3 getIntersectionPoint(const Ray& r);

	void highlight(int x, int y);

	ds::vec3 convert_grid_coords(int x, int y);

	const ds::vec2& getExtent() const {
		return _extent;
	}

private:
	void addSpring(int x1, int y1, int x2, int y2, float stiffness, float damping);
	void prepareData();
	GridPoint* _points;
	uint16_t* _indices;
	WarpingGridVertex* _vertices;
	RID _drawItem;
	InstanceBuffer _constantBuffer;
	RID _cubeBuffer;
	int _numVertices;
	std::vector<Spring> _springs;
	WarpingGridSettings _settings;
	Plane _plane;
	ds::vec2 _gridCenter;
	ds::vec3 _offsets[6];
	ds::vec2 _extent;
};