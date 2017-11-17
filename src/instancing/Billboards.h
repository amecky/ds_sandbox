#pragma once
#include <diesel.h>

struct BillboardsConstantBuffer {
	ds::vec4 screenDimension;
	ds::vec4 screenCenter;
	ds::matrix wvp;
};

struct Billboard {
	ds::vec3 position;
	ds::vec2 dimension;
	float rotation;
	ds::vec2 scaling;
	ds::vec4 texture;
	ds::Color color;
};

const int MAX_BILLBOARDS = 4096;

class Billboards {

public:
	Billboards() {}
	~Billboards() {}
	void init(RID textureID);
	void render(RID renderPass, const ds::matrix& viewProjectionMatrix);
	void begin();
	void add(const ds::vec3& pos, const ds::vec2& dim, const ds::vec4& rect, float rotation = 0.0f, const ds::vec2& scaling = ds::vec2(1.0f), const ds::Color& clr = ds::Color(255, 255, 255, 255));
	void end();
private:
	Billboard _items[MAX_BILLBOARDS];
	int _num;
	RID _drawItem;
	ds::DrawCommand _drawCmd;
	RID _structuredBufferId;
	BillboardsConstantBuffer _constantBuffer;
};