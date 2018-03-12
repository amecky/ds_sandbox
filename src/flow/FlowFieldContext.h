#pragma once
#include <diesel.h>
#include <ds_imgui.h>
#include "..\lib\DataArray.h"
#include "..\utils\RenderItem.h"

// ---------------------------------------------------
// Tower
// ---------------------------------------------------
struct Tower {
	ID id;
	int type;
	int gx;
	int gy;
	ds::vec3 position;
	float radius;
	int energy;
	float timer;
	float bulletTTL;
	float direction;
	ID target;
	int level;
	RenderItem* renderItem;
	RenderItem* baseItem;
};

// ---------------------------------------------------
// Walker
// ---------------------------------------------------
struct Walker {
	ID id;
	p2i gridPos;
	float velocity;
	ds::vec3 pos;
	float rotation;
	int definitionIndex;
	int energy;
	RenderItem* renderItem;
};

// ---------------------------------------------------
// PathItem
// ---------------------------------------------------
struct PathItem {
	ds::vec3 pos;
	p2i grid;
	float direction;
};
