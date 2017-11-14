#pragma once
#include "..\TestApp.h"
#include "..\kenney\Camera.h"
#include "..\utils\DynamicPath.h"

struct InstanceBuffer {
	ds::matrix mvp;
	ds::matrix world;
};

struct InstanceLightBuffer {
	ds::Color ambientColor;
	ds::Color diffuseColor;
	ds::vec3 lightDirection;
	float padding;
};

struct InstanceData {
	ds::matrix worldMatrix;
	ds::Color color;
};

struct Animation {
	int type;
	float timer;
	float ttl;
};

struct GridItem {
	ds::vec3 pos;
	ds::vec3 velocity;
	float scale;
	float angle;
	float timer;
	ds::Color color;
	int type;
	int animationFlags;
	Animation animations[16];
	int numAnimations;
};

class InstanceTest : public TestApp {

public:
	InstanceTest();

	virtual ~InstanceTest();

	ds::RenderSettings getRenderSettings();

	bool usesGUI() {
		return true;
	}

	bool init();

	void tick(float dt);

	void render();

	void renderGUI();

private:
	ds::FloatPath _scalePath;
	GridItem _items[256];
	int _numItems;
	RID _drawItem;
	RID _basicPass;
	RID _instanceVertexBuffer;
	InstanceBuffer _constantBuffer;
	InstanceLightBuffer _lightBuffer;
	ds::Camera _camera;
	FPSCamera* _fpsCamera;
	InstanceData _instances[256];
};