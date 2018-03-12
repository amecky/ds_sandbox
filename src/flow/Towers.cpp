#include "Towers.h"

void Towers::render(RID renderPass, const ds::matrix& viewProjectionMatrix) {
	for (size_t i = 0; i < _towers.size(); ++i) {
		const Tower& t = _towers[i];
		t.baseItem->getTransform().position = t.position;
		t.baseItem->draw(renderPass, viewProjectionMatrix);
		ds::vec3 tp = t.position + ds::vec3(0.0f, 0.25f, 0.0f);
		t.renderItem->getTransform().position = tp;
		t.renderItem->draw(renderPass, viewProjectionMatrix);
	}
}

void Towers::init(Material* material) {
	_towerItem = new RenderItem("cannon", material);
	_towerItem->getTransform().position = ds::vec3(0.0f);

	_baseItems[0] = new RenderItem("green_base", material);
	_baseItems[1] = new RenderItem("yellow_base", material);
	_baseItems[2] = new RenderItem("red_base", material);
}

// -------------------------------------------------------------
// find tower by grid postion
// -------------------------------------------------------------
int Towers::findTower(const p2i& gridPos) const {
	for (size_t i = 0; i < _towers.size(); ++i) {
		const Tower& t = _towers[i];
		if (t.gx == gridPos.x && t.gy == gridPos.y) {
			return i;
		}
	}
	return -1;
}

// -------------------------------------------------------------
// upgrade tower
// -------------------------------------------------------------
void Towers::upgradeTower(int index) {
	if (index != -1) {
		Tower& t = _towers[index];
		++t.level;
		if (t.level > 2) {
			t.level = 2;
		}
		t.baseItem = _baseItems[t.level];
	}
}

// -------------------------------------------------------------
// add tower
// -------------------------------------------------------------
void Towers::addTower(const p2i& gridPos) {
	Tower t;
	t.renderItem = _towerItem;
	t.baseItem = _baseItems[0];
	t.gx = gridPos.x;
	t.gy = gridPos.y;
	t.position = ds::vec3(-10.0f + gridPos.x, 0.15f, -6.0f + gridPos.y);
	t.level = 0;
	_towers.push_back(t);
}