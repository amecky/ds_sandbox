#include "Towers.h"
#include "..\utils\common_math.h"
#include "..\utils\CSVFile.h"
#include <ds_base_app.h>

// -------------------------------------------------------------
// init
// -------------------------------------------------------------
void Towers::init(Material* material) {	 
	_towerItems[0] = new RenderItem("cannon", material);
	_towerItems[1] = new RenderItem("bomber", material);
	_towerItems[2] = new RenderItem("rocket", material);
	_towerItems[3] = new RenderItem("slow_down", material);
	ds::matrix tm = ds::matTranslate(ds::vec3(0.2f, 0.0f, 0.0f));
	_gunItems[0] = new RenderItem("gatlin", material, &tm, true);
	_gunItems[1] = new RenderItem("gatlin", material, &tm, true);
	_gunItems[2] = new RenderItem("gatlin", material, &tm, true);
	_gunItems[3] = new RenderItem("gatlin", material, &tm, true);
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
	
	_dbgTowerAnim = true;
	_dbgGunAnim = true;
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
	t.gunItem = _gunItems[type];
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
	ds::vec3 p = t.position;
	ds::vec3 extent = t.baseItem->getExtent();
	p.y += extent.y;
	initialize(&t.baseTransform, p, ds::vec3(1.0f), ds::vec3(0.0f));
	p.y += t.offset;
	initialize(&t.towerTransform, p, ds::vec3(1.0f), ds::vec3(0.0f));
	//p.y += 0.2f;
	p.y = 0.0f;
	p.x += 0.5f;
	p.z += 0.5f;
	initialize(&t.gunTransform, ds::vec3(0.0f), ds::vec3(1.0f), ds::vec3(0.0f));
	_towers.push_back(t);
	startAnimation(_towers.size() - 1);
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
// render
// -------------------------------------------------------------
void Towers::render(RID renderPass, const ds::matrix& viewProjectionMatrix) {
	for (size_t i = 0; i < _towers.size(); ++i) {
		const Tower& t = _towers[i];
		// draw base
		ds::matrix w = ds::matIdentity();
		build_world_matrix(t.baseTransform, &w);
		t.baseItem->draw(renderPass, viewProjectionMatrix, w);
		// draw tower
		build_world_matrix(t.towerTransform, &w);
		t.renderItem->draw(renderPass, viewProjectionMatrix, w);
		// draw gun
		ds::matrix gw = ds::matIdentity();
		build_world_matrix(t.gunTransform, &gw);
		t.gunItem->draw(renderPass, viewProjectionMatrix, gw * w);
	}
}

// -------------------------------------------------------------
// tick
// -------------------------------------------------------------
void Towers::tick(float dt, ds::EventStream* events) {
	for (size_t i = 0; i < _towers.size(); ++i) {
		Tower& t = _towers[i];
		/*
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
		*/
	}

	_animationManager.tick(dt, events);
	if (events->containsType(1000)) {
		Tower& t = _towers[0];
		if (_dbgTowerAnim) {
			_animationManager.start(&t.towerTransform, idleAnimation, 0, 1.0f);
		}
		if (_dbgGunAnim) {
			_animationManager.start(&t.gunTransform, idleAnimation, 0, 1.0f);
		}
	}
	if (events->containsType(1001)) {
		startAnimation(0);
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
		ds::vec3 delta = pos - t.baseTransform.position;
		float diff = sqr_length(delta);
		if (diff < t.radius * t.radius) {
			//t.direction = math::get_rotation(ds::vec2(delta.x, delta.z));
			t.direction = math::get_angle(ds::vec2(1,0),ds::vec2(delta.x, delta.z));
			t.towerTransform.rotation = ds::vec3(0.0f, t.direction, 0.0f);
			//t.gunTransform.rotation = ds::vec3(0.0f, t.direction, 0.0f);
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
	float min = ds::PI * 0.1f;
	float angle = ds::random(min, min + ds::PI * 0.1f);
	//t.animation.angle = angle;
	float dir = ds::random(-5.0f, 5.0f);
	//t.animation.direction = 1;
	//if (dir < 0.0f) {
		//t.animation.direction = -1;
	//}
	float ttl = angle / min * 2.0f;
	if (_dbgTowerAnim) {		
		RotationYData* rd = new RotationYData;
		rd->angle = angle / ttl;
		if (dir < 0.0f) {
			rd->direction = -1.0f;
		}
		else {
			rd->direction = -1.0f;
		}
		_animationManager.start(&t.towerTransform, rotateY, rd, angle / min * 2.0f);
	}

	if (_dbgGunAnim) {
		RotationYData* grd = new RotationYData;
		grd->angle = angle / ttl;
		if (dir < 0.0f) {
			grd->direction = -1.0f;
		}
		else {
			grd->direction = -1.0f;
		}
		_animationManager.start(&t.gunTransform, rotateX, grd, angle / min * 2.0f);
	}

	//t.animation.timer = 0.0f;
	//t.animation.ttl = angle / min * 2.0f;
	//t.animationState = 0;
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
