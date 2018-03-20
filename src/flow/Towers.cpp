#include "Towers.h"
#include "..\utils\common_math.h"
#include "..\utils\CSVFile.h"
#include <ds_base_app.h>

// -------------------------------------------------------------
// render
// -------------------------------------------------------------
void Towers::render(RID renderPass, const ds::matrix& viewProjectionMatrix) {
	for (size_t i = 0; i < _towers.size(); ++i) {
		const Tower& t = _towers[i];
		t.baseItem->getTransform().position = t.position;
		t.baseItem->draw(renderPass, viewProjectionMatrix);
		ds::vec3 tp = t.position + ds::vec3(0.0f, t.offset, 0.0f);
		t.renderItem->getTransform().position = tp;
		t.renderItem->getTransform().pitch = t.direction;
		t.renderItem->draw(renderPass, viewProjectionMatrix);
	}
}

// -------------------------------------------------------------
// init
// -------------------------------------------------------------
void Towers::init(Material* material) {	 
	_towerItems[0] = new RenderItem("gatlin", material);
	_towerItems[1] = new RenderItem("bomber", material);
	_towerItems[2] = new RenderItem("rocket", material);
	_towerItems[3] = new RenderItem("slow_down", material);
	_baseItems[0] = new RenderItem("green_base", material);
	_baseItems[1] = new RenderItem("yellow_base", material);
	_baseItems[2] = new RenderItem("red_base", material);

	CSVFile file;
	if (file.load("tower_definitions.csv", "resources")) {
		for (size_t i = 0; i < file.size(); ++i) {
			const TextLine& line = file.get(i);
			line.get(0,&_definitions[i].offset);
		}
	}
	else {
		DBG_LOG("Cannot load csv file");
	}
	
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
void Towers::addTower(const p2i& gridPos, int type) {
	Tower t;
	t.renderItem = _towerItems[type];
	t.offset = _definitions[type].offset;
	t.baseItem = _baseItems[0];
	t.gx = gridPos.x;
	t.gy = gridPos.y;
	t.position = ds::vec3(_worldOffset.x + gridPos.x, 0.15f, _worldOffset.y + gridPos.y);
	t.level = 0;
	t.radius = 2.0f;
	t.direction = 0.0f;
	t.animationState = 1;
	t.bulletTTL = 0.1f;
	t.timer = 0.0f;
	t.target = INVALID_ID;
	t.animation.timer = 0.0f;
	t.animation.ttl = ds::random(0.5f, 1.5f);
	_towers.push_back(t);
}

void Towers::remove(const p2i & gridPos) {
	std::vector<Tower>::iterator it = _towers.begin();
	while (it != _towers.end()) {
		if (gridPos.x == it->gx && gridPos.y == it->gy) {
			it = _towers.erase(it);
		}
		else {
			++it;
		}
	}
}

// ---------------------------------------------------------------
// is close (walker still in reach of tower)
// ---------------------------------------------------------------
bool isClose(const Tower& tower, const Walker& walker) {
	float diff = sqr_length(tower.position - walker.pos);
	return (diff < tower.radius * tower.radius);
}

// -------------------------------------------------------------
// tick
// -------------------------------------------------------------
void Towers::tick(float dt, ds::EventStream* events) {
	for (size_t i = 0; i < _towers.size(); ++i) {
		Tower& t = _towers[i];
		if (t.target == INVALID_ID) {
			t.animation.timer += dt;
			if (t.animationState == 0) {
				t.direction += t.animation.angle * dt * static_cast<float>(t.animation.direction);
				if (t.animation.timer >= t.animation.ttl) {
					t.animationState = 1;
					t.animation.timer = 0.0f;
					t.animation.ttl = ds::random(0.5f, 1.5f);
				}
			}
			else {
				if (t.animation.timer >= t.animation.ttl) {
					startAnimation(i);
				}
			}
		}
		else {
			t.timer += dt;
			if (t.timer >= t.bulletTTL) {
				FireEvent event;
				event.pos = t.position;
				event.target = t.target;
				event.type = t.type;
				events->add(100,&event, sizeof(FireEvent));
				t.timer -= t.bulletTTL;
			}
		}
	}
}

// -------------------------------------------------------------
// rotate towers towards walkers
// -------------------------------------------------------------
void Towers::rotateTowers(ds::DataArray<Walker>* walkers) {
	for (size_t i = 0; i < _towers.size(); ++i) {
		Tower& t = _towers[i];
		if (t.target != INVALID_ID) {
			if (walkers->contains(t.target)) {
				const Walker& w = walkers->get(t.target);
				if (!isClose(t, w)) {
					t.target = INVALID_ID;
					startAnimation(i);
				}
				else {
					ds::vec3 delta = w.pos - t.position;
					t.direction = math::get_rotation(ds::vec2(delta.x, delta.z));
				}
			}
			else {
				t.target = INVALID_ID;
				startAnimation(i);
			}
		}
		if (t.target == INVALID_ID) {
			for (uint32_t i = 0; i < walkers->numObjects; ++i) {
				const Walker& w = walkers->objects[i];
				ds::vec3 delta = w.pos - t.position;
				float diff = sqr_length(delta);
				if (diff < t.radius * t.radius) {
					t.direction = math::get_rotation(ds::vec2(delta.x, delta.z));
					t.target = w.id;

				}
			}
		}
	}
}

// -------------------------------------------------------------
// rotate towers towards fixed position
// -------------------------------------------------------------
void Towers::rotateTowers(const ds::vec3& pos) {
	for (size_t i = 0; i < _towers.size(); ++i) {
		Tower& t = _towers[i];
		ds::vec3 delta = pos - t.position;
		float diff = sqr_length(delta);
		if (diff < t.radius * t.radius) {
			t.direction = math::get_rotation(ds::vec2(delta.x, delta.z));
			t.target = 1;			
		}
		else {
			t.target = INVALID_ID;
		}
	}
}

// -------------------------------------------------------------
// start rotation animation
// -------------------------------------------------------------
void Towers::startAnimation(int index) {
	Tower& t = _towers[index];
	float min = ds::PI * 0.25f;
	float angle = ds::random(min, min + ds::PI * 0.5f);
	t.animation.angle = angle;
	float dir = ds::random(-5.0f, 5.0f);
	t.animation.direction = 1;
	if (dir < 0.0f) {
		t.animation.direction = -1;
	}
	t.animation.timer = 0.0f;
	t.animation.ttl = angle / min * 2.0f;
	t.animationState = 0;
	//DBG_LOG("angle %3.2f ttl %2.4f direction %d", (angle*360.0f / ds::TWO_PI), t.animation.ttl,t.animation.direction);
}

// -------------------------------------------------------------
// show GUI
// -------------------------------------------------------------
void Towers::showGUI(int selectedTower) {
	if (selectedTower != -1) {
		gui::begin("Tower", 0);
		const Tower& t = get(selectedTower);
		gui::Value("Tower", selectedTower);
		gui::Value("Level", t.level);
		gui::Value("Target", t.target);
		gui::Value("Direction", (t.direction*360.0f / ds::TWO_PI));
		if (gui::Button("Upgrade")) {
			upgradeTower(selectedTower);
		}
	}
}
