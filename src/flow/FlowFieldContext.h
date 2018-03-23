#pragma once
#include <diesel.h>
#include <ds_imgui.h>
#include "..\lib\DataArray.h"
#include "..\utils\RenderItem.h"

struct TowerTypes {

	enum Enum {
		GATLIN_TOWER,
		CANNON_TOWER
	};
};

struct TowerPart {
	ID id;
	ID parent;	
	int renderItemIndex;
	transform_component transform;	
	
	TowerPart() : id(INVALID_ID), parent(INVALID_ID), renderItemIndex(-1), transform() {}
};
// ---------------------------------------------------
// Tower
// ---------------------------------------------------
struct Tower {
	ID id;
	int type;
	int gx;
	int gy;
	//transform_component_t baseTransform;
	//transform_component_t towerTransform;
	//transform_component_t gunTransform;
	ds::vec3 position;
	float radius;
	int energy;
	float timer;
	float bulletTTL;
	float direction;
	ID target;
	int level;
	//RenderItem* renderItem;
	//RenderItem* gunItem;
	//RenderItem* baseItem;
	int animationState;
	float offset;

	ID parts[3];
};



struct FireEvent {
	ds::vec3 pos;
	ID target;
	int type;
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

