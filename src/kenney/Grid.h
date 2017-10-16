#include <diesel.h>

struct GridVertex {
	ds::vec3 p;
	ds::Color color;
	ds::vec3 n;

	GridVertex() : p(0.0f), color(1.0f, 1.0f, 1.0f, 1.0f), n(0.0f) {}
};

// ---------------------------------------------------------------
// the cube constant buffer
// ---------------------------------------------------------------
struct GridConstantBuffer {
	ds::matrix viewprojectionMatrix;
	ds::matrix worldMatrix;
};

class Grid {

public:
	Grid() {}
	~Grid() {}
	void create(int numCells, RID renderPass);
	void render(ds::Camera* camera);
private:
	GridVertex* _vertices;
	GridConstantBuffer _constantBuffer;
	RID _drawItem;
	RID _renderPass;
};