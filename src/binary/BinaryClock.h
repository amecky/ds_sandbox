#pragma once
#include "..\TestApp.h"
#include "..\kenney\Camera.h"
// ---------------------------------------------------------------
// Vertex
// ---------------------------------------------------------------
struct Vertex {

	ds::vec3 p;
	ds::vec2 uv;
	ds::vec3 n;
	ds::vec3 t;

	Vertex() : p(0.0f), uv(0.0f), n(0.0f), t(0.0f) {}
	Vertex(const ds::vec3& pv, const ds::vec2& uvv, const ds::vec3& nv, const ds::vec3& tv) : p(pv), uv(uvv), n(nv), t(tv) {}
};

// ---------------------------------------------------------------
// the cube constant buffer
// ---------------------------------------------------------------
struct BinaryClockConstantBuffer {
	ds::matrix viewprojectionMatrix;
	ds::matrix worldMatrix;
	ds::matrix invWorld;
	ds::vec3 eyePos;
	float padding;
};

class BinaryClock : public TestApp {

public:
	BinaryClock();

	virtual ~BinaryClock();

	ds::RenderSettings getRenderSettings();

	bool usesGUI() {
		return false;
	}

	bool init();

	void tick(float dt);

	void render();

private:
	float _timer;
	Vertex _vertices[24];
	RID _drawItem;
	RID _basicPass;
	BinaryClockConstantBuffer _constantBuffer;
	ds::Camera _camera;
	FPSCamera* _fpsCamera;
};