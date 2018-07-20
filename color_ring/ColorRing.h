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
	void markPart(float angle, int color);
	void debug();
	void reset();
protected:
	virtual RID createVertexShader();
	virtual RID createPixelShader();
private:
	int findInactiveSegment();
	int _maxConcurrent;
	int _activeCount;
	void mapColors(ds::Color* colors);
	void checkSegments();
	void resetSegment(int index);
	float _timer;
	int _colors[TOTAL_PARTS];
	Segment _segments[NUM_SEGMENTS];
};

