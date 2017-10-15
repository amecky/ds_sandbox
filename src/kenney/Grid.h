#include <diesel.h>

struct GridVertex {
	ds::vec3 p;
	ds::vec2 uv;
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