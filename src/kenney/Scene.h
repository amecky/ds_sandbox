#pragma once
#include <diesel.h>
#include <vector>
#include "DepthMap.h"
#include "RTViewer.h"
#include "Camera.h"

struct AmbientVertex {
	ds::vec3 p;
	ds::Color color;
	ds::vec3 n;

	AmbientVertex() : p(0.0f), color(1.0f, 1.0f, 1.0f, 1.0f), n(0.0f) {}
};

struct BoundingSphere {
	ds::vec3 pos;
	float radius;
};

struct Entity {
	AmbientVertex* vertices;
	RID drawItem;
	RID depthDrawItem;
	ds::vec3 extent;
	float radius;
};

struct EntityInstance {
	ds::vec3 pos;
	ds::vec3 scale;
	ds::vec3 rotation;
	Entity* entity;
	bool castShadows;
	BoundingSphere boundingSphere;
};

struct LightBuffer {
	ds::Color ambientColor;
	ds::Color diffuseColor;
	ds::vec4 lightPosition;
	ds::vec4 lightDirection;	
	ds::vec4 eyePosition;
};

struct MatrixBuffer {
	ds::matrix worldMatrix;
	ds::matrix viewMatrix;
	ds::matrix projectionMatrix;
	ds::matrix shadowTransform;
};

// ---------------------------------------------------------------
// the cube constant buffer used by the vertex shader
// ---------------------------------------------------------------
struct CubeConstantBuffer {
	ds::matrix viewprojectionMatrix;
	ds::matrix worldMatrix;
};

class Scene {

public:
	Scene();
	~Scene();
	void tick(float dt);
	int loadEntity(const char* fileName);
	int createGrid(int numCells);
	int createInstance(int entityID, const ds::vec3& pos, bool castShadows = true);
	void renderDepthMap();
	void renderMain();
	void renderDebug();
private:
	Entity* _entities;
	int _numEntities;
	int _entitiesCapacity;
	EntityInstance* _instances;
	int _numInstances;
	int _instancesCapacity;
	RID _baseGroup;
	ds::Camera _camera;
	FPSCamera* _fpsCamera;
	DepthMap* _depthMap;
	RenderTextureViewer* _rtViewer;
	RID _basicPass;
	MatrixBuffer _matrixBuffer;
	LightBuffer _lightBuffer;
	ds::vec3 _lightPosition;
	ds::vec3 _lightDirection;
};