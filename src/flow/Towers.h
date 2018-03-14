#pragma once
#include <diesel.h>
#include <ds_imgui.h>
#include "..\utils\RenderItem.h"
#include <vector>
#include "..\Material.h"
#include "..\lib\DataArray.h"
#include "FlowFieldContext.h"

class Towers {

public:
	Towers() {}
	~Towers() {}
	void init(Material* material);
	void render(RID renderPass, const ds::matrix& viewProjectionMatrix);
	void upgradeTower(int index);
	void addTower(const p2i& gridPos);
	int findTower(const p2i& gridPos) const;
	const Tower& get(int index) const {
		return _towers[index];
	}
	const Tower& get(const p2i& gridPos) const {
		int idx = findTower(gridPos);
		return _towers[idx];
	}
	void rotateTowers(ds::DataArray<Walker>* walkers);
	void rotateTowers(const ds::vec3& target);
	void showGUI(int selectedTower);
private:
	void startAnimation(int index);
	std::vector<Tower> _towers;
	RenderItem* _towerItem;
	RenderItem* _baseItems[3];
};

