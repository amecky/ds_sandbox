#include "ColorRing.h"
#include <ds_tweakable.h>
#include "math.h"

static const int NO_COLOR = -1;

ColorRing::ColorRing() : WarpingGrid() , _timer(0.0f) {

	for (int i = 0; i < TOTAL_PARTS; ++i) {
		int s = i / NUM_PARTS_PER_SEGMENT;
		_colors[i] = s % 5;
	}

	_numVertices = TOTAL_PARTS * 4;
	_vertices = new GridVertex[_numVertices];

	int cnt = 0;

	float r1 = 300.0f;
	float r2 = 325.0f;
	_rotationStep = ds::TWO_PI / static_cast<float>(TOTAL_PARTS);
	float angle = 0.0f;
	ds::vec2 center(512, 384);

	_constantBuffer.center = ds::vec4(center.x, center.y, 0.0f, 0.0f);

	float u2 = 36.0f / 512.0f;
	float v2 = 36.0f / 512.0f;

	for (int i = 0; i < TOTAL_PARTS; ++i) {
		ds::vec2 p1 = center + ds::vec2(cos(angle + _rotationStep), sin(angle + _rotationStep)) * r1;
		ds::vec2 p2 = center + ds::vec2(cos(angle + _rotationStep), sin(angle + _rotationStep)) * r2;
		ds::vec2 p3 = center + ds::vec2(cos(angle), sin(angle)) * r2;
		ds::vec2 p4 = center + ds::vec2(cos(angle), sin(angle)) * r1;
		_vertices[cnt++] = { p1, ds::vec2(0.0f,v2), ds::Color(255,255,255,255) };
		_vertices[cnt++] = { p2, ds::vec2(0.0f,0.0f), ds::Color(255,255,255,255) };
		_vertices[cnt++] = { p3, ds::vec2(u2,0.0f), ds::Color(255,255,255,255) };
		_vertices[cnt++] = { p4, ds::vec2(u2,v2), ds::Color(255,255,255,255) };
		angle += _rotationStep;
	}

	for (int i = 0; i < NUM_SEGMENTS; ++i) {
		resetSegment(i);
		//_segments[i].color = ds::random(-1.9f, 3.9f);
	}

}

// -------------------------------------------------------
// reset defined segment
// -------------------------------------------------------
void ColorRing::resetSegment(int index) {
	Segment& seg = _segments[index];
	for (int i = 0; i < NUM_PARTS_PER_SEGMENT; ++i) {
		seg.fillingDegree[i] = 0;
	}
	seg.timer = 0.0f;
	seg.color = NO_COLOR;
}

// -------------------------------------------------------
// create vertex shader
// -------------------------------------------------------
RID ColorRing::createVertexShader() {
	RID vertexShader = ds::createShader(ds::ShaderDesc()
		.CSOName("Solid_vs.cso")
		.ShaderType(ds::ShaderType::ST_VERTEX_SHADER)
		);
	return vertexShader;
}

// -------------------------------------------------------
// create pixel shader
// -------------------------------------------------------
RID ColorRing::createPixelShader() {
	RID pixelShader = ds::createShader(ds::ShaderDesc()
		.CSOName("Solid_ps.cso")
		.ShaderType(ds::ShaderType::ST_PIXEL_SHADER)
		);
	return pixelShader;
}

// -------------------------------------------------------
// pick a random inactive segment
// -------------------------------------------------------
int ColorRing::findInactiveSegment() {
	int tmp[NUM_SEGMENTS];
	int cnt = 0;
	for (int i = 0; i < NUM_SEGMENTS; ++i) {
		if (_segments[i].color == NO_COLOR) {
			tmp[cnt++] = i;
		}
	}
	if (cnt == 0) {
		return -1;
	}
	int ret = ds::random(0, cnt - 1);
	return tmp[ret];
}

// -------------------------------------------------------
// reset
// -------------------------------------------------------
void ColorRing::reset() {
	_maxConcurrent = 6;
	_activeCount = 0;
	for (int i = 0; i < NUM_SEGMENTS; ++i) {
		resetSegment(i);
	}
}

int ColorRing::getPartIndex(float input) {
	float step = ds::TWO_PI / static_cast<float>(TOTAL_PARTS);
	float fidx = input / step;
	return static_cast<int>(fidx);
}


float ColorRing::rasterizeAngle(float input) {
	float step = ds::TWO_PI / static_cast<float>(TOTAL_PARTS);
	float fidx = input / step;
	int idx = static_cast<int>(fidx);
	return static_cast<float>(idx) * _rotationStep + _rotationStep * 0.5f;
}

int ColorRing::getNumSegments() const {
	return NUM_SEGMENTS;
}

int ColorRing::getColor(int idx) const {
	return _segments[idx].color;
}
// -------------------------------------------------------
// tick
// -------------------------------------------------------
void ColorRing::tick(float dt) {
	/*
	_timer += dt;
	if (_timer >= 1.0f) {
		_timer -= 1.0f;
		for (int i = 0; i < TOTAL_PARTS; ++i) {
			++_colors[i];
			if (_colors[i] > 4) {
				_colors[i] = 0;
			}
		}
	}
	*/
	for (int i = 0; i < NUM_SEGMENTS; ++i) {
		Segment& seg = _segments[i];
		if (seg.color != NO_COLOR) {
			seg.timer += dt;
			if (seg.timer > 10.0f) {
				resetSegment(i);
				--_activeCount;
			}
		}
	}

	if (_activeCount <= _maxConcurrent) {
		_timer += dt;
		if (_timer >= 1.0f) {
			_timer -= 1.0f;
			int idx = findInactiveSegment();
			DBG_LOG("=> segment: %d", idx);
			if (idx != -1) {
				resetSegment(idx);
				_segments[idx].color = ds::random(0.0f, 3.9f);
				++_activeCount;
			}
		}
	}

}

const static ds::vec2 UVS[] = {
	ds::vec2(0,1),
	ds::vec2(0,0),
	ds::vec2(1,0),
	ds::vec2(1,1)
};

const static float PIX_OFFSET = 40.0f / 512.0f;
const static float PIX_STEP = 36.0f / 512.0f;
const static float PIX_LINEAR = 1.0f / 512.0f;
const static ds::vec2 PIX_OFF = ds::vec2(PIX_LINEAR);

// -------------------------------------------------------
// map colors
// -------------------------------------------------------
void ColorRing::mapColors(ds::Color* colors) {
	int cnt = 0;
	for (int i = 0; i < NUM_SEGMENTS; ++i) {
		int color = _segments[i].color;
		if (color < 0 || color > 4) {
			color = 4;
		}
		for (int j = 0; j < NUM_PARTS_PER_SEGMENT; ++j) {
			float offset = static_cast<float>(_segments[i].fillingDegree[j]) * PIX_OFFSET;
			for (int k = 0; k < 4; ++k) {
				ds::vec2 uv = UVS[k];
				uv *= PIX_STEP;
				uv.x += offset;
				uv += PIX_OFF;
				_vertices[cnt+k].texture = uv;
				_vertices[cnt+k].color = colors[color];
			}
			cnt += 4;
		}
	}
}

// -------------------------------------------------------
// debug
// -------------------------------------------------------
void ColorRing::debug() {
	for (int i = 0; i < NUM_SEGMENTS; ++i) {
		int filled = 0;
		DBG_LOG("segment %d color: %d", i, _segments[i].color);
		for (int j = 0; j < NUM_PARTS_PER_SEGMENT; ++j) {
			DBG_LOG("  part %d = %d", j, _segments[i].fillingDegree[j]);
		}
	}
}

// -------------------------------------------------------
// check segments
// -------------------------------------------------------
int ColorRing::checkSegments() {
	int ret = 0;
	for (int i = 0; i < NUM_SEGMENTS; ++i) {
		int filled = 0;
		for (int j = 0; j < NUM_PARTS_PER_SEGMENT; ++j) {
			if (_segments[i].fillingDegree[j] == 3) {
				++filled;
			}
		}
		if (filled == (NUM_PARTS_PER_SEGMENT - 1)) {
			DBG_LOG("segment %d filled", i);
			resetSegment(i);
			--_activeCount;
			++ret;
		}
	}
	return ret;
}

// -------------------------------------------------------
// render
// -------------------------------------------------------
void ColorRing::render(ds::Color* colors) {

	mapColors(colors);

	ds::mapBufferData(_vertexBufferID, _vertices, _numVertices * sizeof(GridVertex));

	ds::submit(_orthoPass, _drawItem, _numVertices / 4 * 6);
}

// -------------------------------------------------------
// mark part of a segment
// -------------------------------------------------------
int ColorRing::markPart(float angle, int color) {
	float fidx = ( angle + _rotationStep * 0.5f) /_rotationStep;
	int idx = static_cast<int>(fidx);
	bool match = false;
	if (idx < TOTAL_PARTS) {
		int sidx = idx / NUM_PARTS_PER_SEGMENT;
		int pidx = idx - sidx * NUM_PARTS_PER_SEGMENT;
		if (_segments[sidx].fillingDegree[pidx] < 3 && _segments[sidx].color == color) {
			++_segments[sidx].fillingDegree[pidx];
			match = true;
		}
		DBG_LOG("sidx %d pidx %d", sidx, pidx);
	}
	if (match) {
		return checkSegments();
	}
	return 0;
}

int ColorRing::markPart(int index, int color) {
	bool match = false;
	int sidx = index / NUM_PARTS_PER_SEGMENT;
	int pidx = index - sidx * NUM_PARTS_PER_SEGMENT;
	if (_segments[sidx].fillingDegree[pidx] < 3 && _segments[sidx].color == color) {
		++_segments[sidx].fillingDegree[pidx];
		match = true;
	}
	if (match) {
		return checkSegments();
	}
	return 0;
}
