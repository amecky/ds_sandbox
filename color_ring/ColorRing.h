#pragma once
#include <diesel.h>
#include <vector>
#include "WarpingGrid.h"

const int NUM_SEGMENTS = 18;
const int NUM_PARTS_PER_SEGMENT = 8;
const int TOTAL_PARTS = NUM_SEGMENTS * NUM_PARTS_PER_SEGMENT;

struct Segment {
	int fillingDegree[NUM_PARTS_PER_SEGMENT];
	int color;
	float timer;
};

class ColorRing : public WarpingGrid {

public:
	ColorRing();
	virtual ~ColorRing() {}
	void tick(float dt);
	virtual void render(ds::Color* colors);
	int markPart(float angle, int color);
	int markPart(int index, int color);
	void debug();
	void reset();
	float rasterizeAngle(float input);
	int getPartIndex(float input);
protected:
	virtual RID createVertexShader();
	virtual RID createPixelShader();
private:
	int findInactiveSegment();
	int _maxConcurrent;
	int _activeCount;
	void mapColors(ds::Color* colors);
	int checkSegments();
	void resetSegment(int index);
	float _timer;
	float _rotationStep;
	int _colors[TOTAL_PARTS];
	Segment _segments[NUM_SEGMENTS];
};

