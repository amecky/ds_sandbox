#pragma once
#include <diesel.h>
#include <ds_imgui.h>
#include "..\utils\RenderItem.h"
#include <vector>
#include "..\Material.h"
#include "..\lib\DataArray.h"
#include "FlowFieldContext.h"
#include "..\animation\AnimationManager.h"

namespace  ds {
	class EventStream;
}
struct TowerDefinition {
	float offset;
	float radius;
	float bulletTTL;
};

struct TowerItemTypes {

	enum Enum {
		GREEN_BASE,
		YELLOW_BASE,
		RED_BASE,
		GATLIN_CHASSIS,
		GATLIN_WEAPON,
		CANNON_CHASSIS,
		CANNON_WEAPON,
		BOMBER_CHASSIS,
		BOMBER_WEAPON,
		EOL
	};
};

class Towers {

public:
	Towers() : _worldOffset(-10.0f,-6.0f) {}
	~Towers();
	void init(Material* material);
	void setWorldOffset(const ds::vec2& offset) {
		_worldOffset = offset;
	}
	void render(RID renderPass, const ds::matrix& viewProjectionMatrix);
	void upgradeTower(int index);
	void addTower(const p2i& gridPos,int type);
	void remove(const p2i& gridPos);
	int findTower(const p2i& gridPos) const;
	const Tower& get(int index) const {
		return _towers.objects[index];
	}
	const Tower& get(const p2i& gridPos) const {
		int idx = findTower(gridPos);
		return _towers.objects[idx];
	}
	void tick(float dt, ds::EventStream* events);
	void rotateTowers(ds::DataArray<Walker>* walkers);
	void rotateTowers(const ds::vec3& target);
	void showGUI(int selectedTower);
	float getDirection(ID id) const;
private:
	void startAnimation(ID id);
	void buildCannonTower(Tower* tower, const ds::vec3& pos);
	void buildGatlinTower(Tower* tower, const ds::vec3& pos);
	void buildBomberTower(Tower* tower, const ds::vec3& pos);
	int buildTower(Tower* tower, const ds::vec3& pos, ds::vec3* offsets, int* items, int num, ID* ret);
	ds::DataArray<TowerPart> _parts;

	ds::DataArray<Tower> _towers;
	RenderItem* _renderItems[32];
	TowerDefinition _definitions[10];
	ds::vec2 _worldOffset;
	AnimationManager _animationManager;
	bool _dbgTowerAnim;
	bool _dbgGunAnim;
};

