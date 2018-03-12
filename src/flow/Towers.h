#pragma once
#include <diesel.h>
#include <ds_imgui.h>
#include "..\utils\RenderItem.h"
#include <vector>
#include "..\Material.h"

struct Tower {
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
private:
	std::vector<Tower> _towers;
	RenderItem* _towerItem;
	RenderItem* _baseItems[3];
};

