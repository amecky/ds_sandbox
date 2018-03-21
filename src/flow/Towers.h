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
};

class Towers {

public:
	Towers() : _worldOffset(-10.0f,-6.0f) {}
	~Towers() {}
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
		return _towers[index];
	}
	const Tower& get(const p2i& gridPos) const {
		int idx = findTower(gridPos);
		return _towers[idx];
	}
	void tick(float dt, ds::EventStream* events);
	void rotateTowers(ds::DataArray<Walker>* walkers);
	void rotateTowers(const ds::vec3& target);
	void showGUI(int selectedTower);
private:
	void startAnimation(int index);
	std::vector<Tower> _towers;
	RenderItem* _towerItems[10];
	RenderItem* _gunItems[10];
	RenderItem* _baseItems[3];
	TowerDefinition _definitions[10];
	ds::vec2 _worldOffset;

	AnimationManager _animationManager;
};

