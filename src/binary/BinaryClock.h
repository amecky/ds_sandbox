#pragma once
#include <ds_base_app.h>
#include "..\kenney\Camera.h"

struct TimeCol {
	int state[4];
	int num;
	float timer[4];
	int flipping[4];
	float angle[4];
};

// ---------------------------------------------------------------
// Vertex
// ---------------------------------------------------------------
struct Vertex {

	ds::vec3 p;
	ds::vec2 uv;
	ds::vec3 n;
	ds::vec3 t;
	ds::Color c;

	Vertex() : p(0.0f), uv(0.0f), n(0.0f), t(0.0f) , c(1.0f,1.0f,1.0f,1.0f) {}
	Vertex(const ds::vec3& pv, const ds::vec2& uvv, const ds::vec3& nv, const ds::vec3& tv, const ds::Color& clr) : p(pv), uv(uvv), n(nv), t(tv) , c(clr) {}
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
	ds::vec3 lightPos;
	float more;
};

class BinaryClock : public ds::BaseScene {

public:
	BinaryClock();

	virtual ~BinaryClock();
	
	void showGUI();

	void initialize();

	void update(float dt);

	void render();

private:
	float _timer;
	Vertex _vertices[24];
	RID _drawItem;
	BinaryClockConstantBuffer _constantBuffer;
	FPSCamera* _fpsCamera;
	ds::vec3 _lightPos;
	bool _rotate;
	bool _running;
	TimeCol _columns[6];
	SYSTEMTIME _systemTime;
	float _rotationDelay;
};