#include "Towers.h"
#include "..\utils\common_math.h"
#include "..\utils\CSVFile.h"
#include <ds_base_app.h>

Towers::~Towers() {
	for (int i = 0; i < TowerItemTypes::EOL; ++i) {
		delete _renderItems[i];
	}
}

// -------------------------------------------------------------
// init
// -------------------------------------------------------------
void Towers::init(Material* material) {	 

	_renderItems[TowerItemTypes::GREEN_BASE] = new RenderItem("green_base", material);
	_renderItems[TowerItemTypes::YELLOW_BASE] = new RenderItem("yellow_base", material);
	_renderItems[TowerItemTypes::RED_BASE] = new RenderItem("red_base", material);
	_renderItems[TowerItemTypes::BASE_TOWER] = new RenderItem("base_tower", material);
	_renderItems[TowerItemTypes::GATLIN_CHASSIS] = new RenderItem("cannon", material);
	_renderItems[TowerItemTypes::GATLIN_WEAPON] = new RenderItem("gatlin", material);
	_renderItems[TowerItemTypes::CANNON_CHASSIS] = new RenderItem("bomber", material);
	_renderItems[TowerItemTypes::CANNON_WEAPON] = new RenderItem("boom_cannon", material);
	_renderItems[TowerItemTypes::BOMBER_CHASSIS] = new RenderItem("rocket", material);
	_renderItems[TowerItemTypes::BOMBER_WEAPON] = new RenderItem("boom_cannon", material);


	//_towerItems[3] = new RenderItem("slow_down", material);
	ds::matrix tm = ds::matTranslate(ds::vec3(0.2f, 0.0f, 0.0f));

	CSVFile file;
	if (file.load("tower_definitions.csv", "resources")) {
		for (size_t i = 0; i < file.size(); ++i) {
			const TextLine& line = file.get(i);
			line.get(0, &_definitions[i].offset);
			line.get(1, &_definitions[i].radius);
			line.get(2, &_definitions[i].bulletTTL);
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
	for (size_t i = 0; i < _towers.numObjects; ++i) {
		const Tower& t = _towers.objects[i];
		if (t.gx == gridPos.x && t.gy == gridPos.y) {
			return i;
		}
	}
	return -1;
}

float Towers::getDirection(ID id) const {
	if (_towers.contains(id)) {
		const Tower& t = _towers.get(id);
		return _parts.get(t.parts[1]).transform.rotation.y;
	}
	return 0.0f;
}
// -------------------------------------------------------------
// upgrade tower
// -------------------------------------------------------------
void Towers::upgradeTower(int index) {
	if (index != -1) {
		Tower& t = _towers.objects[index];
		++t.level;
		if (t.level > 2) {
			t.level = 2;
		}
		//t.baseItem = _baseItems[t.level];
	}
}

int Towers::buildTower(Tower* tower, const ds::vec3& pos, ds::vec3* offsets, int* items, int num, ID* ret) {
	for (int i = 0; i < num; ++i) {
		ds::vec3 p = pos;
		ID baseID = _parts.add();
		TowerPart& basePart = _parts.get(baseID);
		basePart.parent = INVALID_ID;
		basePart.renderItemIndex = items[i];
		p = offsets[i];
		initialize(&basePart.transform, p, ds::vec3(1.0f), ds::vec3(0.0f));
		tower->parts[i] = baseID;
		ret[i] = baseID;
	}
	return num;
}

void Towers::buildCannonTower(Tower* tower, const ds::vec3& pos) {
	int parts[] = { TowerItemTypes::GREEN_BASE ,TowerItemTypes::BASE_TOWER, TowerItemTypes::CANNON_CHASSIS ,TowerItemTypes::CANNON_WEAPON };
	ds::vec3 offsets[] = { pos, pos + ds::vec3(0.0f,tower->offset,0.0f), ds::vec3(0.0f,0.25f,0.0f) , ds::vec3(0.25f,0.0f,0.0f) };
	ID ids[4];
	buildTower(tower, pos, offsets, parts, 4, ids);
	TowerPart& wp = _parts.get(ids[2]);
	wp.parent = ids[1];
	TowerPart& nwp = _parts.get(ids[3]);
	nwp.parent = ids[2];
}

void Towers::buildGatlinTower(Tower* tower, const ds::vec3& pos) {
	int parts[] = { TowerItemTypes::GREEN_BASE, TowerItemTypes::BASE_TOWER, TowerItemTypes::GATLIN_CHASSIS, TowerItemTypes::GATLIN_WEAPON };
	ds::vec3 offsets[] = {pos, pos + ds::vec3(0.0f,tower->offset,0.0f), ds::vec3(0.0f,0.25f,0.0f) , ds::vec3(0.20f,0.0f,0.0f) };
	ID ids[4];
	buildTower(tower, pos, offsets, parts, 4, ids);
	TowerPart& wp = _parts.get(ids[2]);
	wp.parent = ids[1];
	TowerPart& nwp = _parts.get(ids[3]);
	nwp.parent = ids[2];
}

void Towers::buildBomberTower(Tower* tower, const ds::vec3& pos) {
	int parts[] = { TowerItemTypes::GREEN_BASE ,TowerItemTypes::BOMBER_CHASSIS ,TowerItemTypes::BOMBER_WEAPON };
	ds::vec3 offsets[] = { pos, pos + ds::vec3(0.0f,tower->offset,0.0f),ds::vec3(0.25f,0.0f,0.0f) };
	ID ids[3];
	buildTower(tower, pos, offsets, parts, 3, ids);
	TowerPart& wp = _parts.get(ids[2]);
	wp.parent = ids[1];
}
// -------------------------------------------------------------
// add tower
// -------------------------------------------------------------
void Towers::addTower(const p2i& gridPos, int type) {
	ID id = _towers.add();
	Tower& t = _towers.get(id);
	t.offset = _definitions[type].offset;
	t.gx = gridPos.x;
	t.gy = gridPos.y;
	t.position = ds::vec3(_worldOffset.x + gridPos.x, 0.15f, _worldOffset.y + gridPos.y);
	t.level = 0;
	t.radius = _definitions[type].radius;
	t.direction = 0.0f;
	t.animationID = INVALID_ID;
	t.bulletTTL = _definitions[type].bulletTTL;
	t.timer = 0.0f;
	t.target = INVALID_ID;
	ds::vec3 p = t.position;
	t.type = type;
	if (type == 0) {
		buildGatlinTower(&t, t.position);
	}
	if (type == 1) {
		buildCannonTower(&t, t.position);
	}
	if (type == 2) {
		buildBomberTower(&t, t.position);
	}

	startAnimation(id);
}

void Towers::remove(const p2i & gridPos) {
	for (int i = 0; i < _towers.numObjects; ++i) {
		const Tower& t = _towers.objects[i];
		if (gridPos.x == t.gx && gridPos.y == t.gy) {
			_animationManager.stopAll(t.id);
			for (int j = 0; j < 4; ++j) {
				_parts.remove(t.parts[j]);
			}
			_towers.remove(t.id);
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
	// update world matrix
	for (unsigned int i = 0; i < _parts.numObjects; ++i) {
		TowerPart& tp = _parts.objects[i];
		build_world_matrix(tp.transform, &tp.world);
		if (tp.parent != INVALID_ID) {
			const TowerPart& parent = _parts.get(tp.parent);
			tp.world = tp.world * parent.world;
		}
	}
	// render items
	for (unsigned int i = 0; i < _parts.numObjects; ++i) {
		const TowerPart& tp = _parts.objects[i];
		_renderItems[tp.renderItemIndex]->draw(renderPass, viewProjectionMatrix, tp.world);
	}
}

// -------------------------------------------------------------
// tick
// -------------------------------------------------------------
void Towers::tick(float dt, ds::EventStream* events) {

	_animationManager.tick(dt, events);

	for (size_t i = 0; i < _towers.numObjects; ++i) {
		Tower& t = _towers.objects[i];
		if (t.target != INVALID_ID) {
			t.timer += dt;
			if (t.timer >= t.bulletTTL) {
				if (t.type == 1) {
					TowerPart& weaponPart = _parts.get(t.parts[3]);
					/*
					MoveToData md;
					md.start = weaponPart.transform.position;
					md.end = weaponPart.transform.position + ds::vec3(0.05f, 0.0f, 0.0f);
					_animationManager.start(t.id, &weaponPart.transform, AnimationTypes::MOVE_TO, &md, 0.1f, tweening::easeSinus);
					*/
					ScaleToData md;
					md.start = ds::vec3(1.0f);
					md.end = ds::vec3(1.5, 1.0f, 1.0f);
					_animationManager.start(t.id, &weaponPart.transform, AnimationTypes::SCALE_TO, &md, 0.1f, tweening::easeSinus);
				}
				// FIXME: fire bullet
				t.timer -= t.bulletTTL;
				// send event
				DBG_LOG("Firing bullet - tower: %d", t.id);
			}
		}
	}

	if (events->containsType(100)) {
		AnimationEvent evn;
		for (int i = 0; i < events->num(); ++i) {
			events->get(i, &evn);
			Tower& t = _towers.get(evn.oid);
			if (evn.type == AnimationTypes::ROTATE_Y) {				
				TowerPart& part = _parts.get(t.parts[1]);
				_animationManager.start(t.id, &part.transform, AnimationTypes::IDLE, 0, ds::random(1.0f, 2.0f));
			}
			if (evn.type == AnimationTypes::IDLE) {
				startAnimation(t.id);
			}
		}
	}
}

// -------------------------------------------------------------
// rotate towers towards walkers
// -------------------------------------------------------------
void Towers::rotateTowers(ds::DataArray<Walker>* walkers) {
	for (size_t i = 0; i < _towers.numObjects; ++i) {
		Tower& t = _towers.objects[i];
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
				startAnimation(t.id);
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
	for (size_t i = 0; i < _towers.numObjects; ++i) {
		Tower& t = _towers.objects[i];
		TowerPart& part = _parts.get(t.parts[1]);
		ds::vec3 delta = pos - part.transform.position;
		float diff = sqr_length(delta);
		if (diff < t.radius * t.radius) {
			t.direction = math::get_angle(ds::vec2(delta.x, delta.z), ds::vec2(1, 0));
			float look = math::get_angle(ds::vec2(1, 0),ds::vec2(1.0f, delta.y));
			part.transform.rotation = ds::vec3(0.0f, t.direction, 0.0f);
			TowerPart& weaponPart = _parts.get(t.parts[2]);
			weaponPart.transform.rotation.z = look;
			t.target = 1;		
			if (t.animationID == INVALID_ID) {
				if (t.type == 0) {
					RotationData rd;
					rd.angle = ds::PI * 2.5f;
					rd.direction = 1.0f;
					t.animationID = _animationManager.start(t.id, &_parts.get(t.parts[3]).transform, AnimationTypes::ROTATE_X, &rd, 0.0f);
				}				
			}
		}
		else {
			t.target = INVALID_ID;
			if (t.animationID != INVALID_ID) {
				DBG_LOG("stopping animations!!!!!");
				_animationManager.stop(AnimationTypes::ROTATE_X, t.id);
				_parts.get(t.parts[3]).transform.rotation = ds::vec3(0.0f);
				_parts.get(t.parts[3]).transform.position = ds::vec3(0.2f, 0.0f, 0.0f);
				t.animationID = INVALID_ID;
			}
		}
	}
}

// -------------------------------------------------------------
// start rotation animation
// -------------------------------------------------------------
void Towers::startAnimation(ID id) {
	Tower& t = _towers.get(id);
	float min = ds::PI * 0.5f;
	float angle = ds::random(min, min + ds::PI * 0.5f);
	float dir = ds::random(-5.0f, 5.0f);
	if (dir < 0.0f) {
		dir = -1.0f;
	}
	else {
		dir = 1.0f;
	}
	float ttl = angle / min * 2.0f;
	if (_dbgTowerAnim) {		
		TowerPart& part = _parts.get(t.parts[1]);
		RotationData rd;
		rd.angle = angle;
		rd.direction = dir;
		_animationManager.start(t.id, &part.transform, AnimationTypes::ROTATE_Y, &rd, ttl);
		
		TowerPart& wpart = _parts.get(t.parts[2]);
		float current = wpart.transform.rotation.z;
		MoveToData moveData;
		if (current >= 0.0f) {
			moveData.start = ds::vec3(0.0f, 0.0f, current);
			moveData.end = ds::vec3(0.0f, 0.0f, -30.0f * ds::TWO_PI / 360.0f);
		}
		else {
			moveData.start = ds::vec3(0.0f, 0.0f, current);
			moveData.end = ds::vec3(0.0f, 0.0f, 30.0f * ds::TWO_PI / 360.0f);
		}
		
		_animationManager.start(t.id, &wpart.transform, AnimationTypes::ROTATE_TO, &moveData, ttl);
	}
	
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
		gui::SliderAngle("RZ", &_parts.get(t.parts[2]).transform.rotation.z);
		if (gui::Button("Upgrade")) {
			upgradeTower(selectedTower);
		}
	}
}
