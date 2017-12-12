#include "Doors.h"
#include "..\utils\tweening.h"
#include <ds_imgui.h>

static ds::vec3 convert_grid_coords(int x, int z) {
	float cx = static_cast<float>(DOORS_NUM_X) * DOOR_SIZE * 0.5f - DOOR_HALF_SIZE;
	float cy = static_cast<float>(DOORS_NUM_Y) * DOOR_SIZE * 0.5f - DOOR_HALF_SIZE;
	return ds::vec3(-cx + static_cast<float>(x) * DOOR_SIZE, -cy + static_cast<float>(z) * DOOR_SIZE, 0.0f);
}

void Doors::add(int gx, int gy) {
	Door& door = _doors[_num++];
	door.id = add_instance(_render_item);
	instance_item& item = get_instance(_render_item, door.id);
	item.transform.scale = ds::vec3(0.25f);
	item.transform.position = convert_grid_coords(gx,gy);
	item.transform.position.z = _render_item->mesh->getExtent().z * item.transform.scale.z * -0.5f;
	door.state = DS_IDLE;
	door.timer = 0.0f;
}

void Doors::init() {
	_num = 0;
	// left side
	for (int y = 0; y < DOORS_NUM_Y - 1; ++y) {
		add(-1, DOORS_NUM_Y - y - 2);
	}
	// top
	for (int x = 0; x < DOORS_NUM_X - 1; ++x) {
		add(x, DOORS_NUM_Y - 1);
	}
	// right side
	for (int y = 0; y < DOORS_NUM_Y - 1; ++y) {
		add(DOORS_NUM_X - 1, DOORS_NUM_Y - y - 2);
	}
	// bottom
	for (int x = 0; x < DOORS_NUM_X - 1; ++x) {
		add(x, -1);
	}
	add(-1, -1);
	add(DOORS_NUM_X - 1, -1);
	add(-1, DOORS_NUM_Y - 1);
	add(DOORS_NUM_X - 1, DOORS_NUM_Y - 1);
}

void Doors::open(int idx) {
	Door& door = _doors[idx];
	door.state = DS_OPENING;
	door.timer = 0.0f;
	instance_item& item = get_instance(_render_item, door.id);
	item.transform.scale = ds::vec3(0.25f);
}

int Doors::getIndex(int side, int gx, int gy) {
	if (side == 1) {
		return gy;
	}
	else if (side == 2) {
		return gx + DOORS_NUM_Y - 1;
	}
	else if (side == 3) {
		return gy + DOORS_NUM_Y + DOORS_NUM_X - 2;
	}
	else if (side == 4) {
		return gx + DOORS_NUM_Y * 2 + DOORS_NUM_X - 3;
	}
	return -1;
}

void Doors::wiggle(int gx, int gy) {
	int side = 1;
	if (gy == 0) {
		side = 2;
	}
	else if (gx == 0) {
		side = 1;
	}
	else if (gx == DOORS_NUM_X) {
		side = 3;
	}
	else if (gy == DOORS_NUM_Y) {
		side = 4;
	}
	int idx = getIndex(side, gx, gy);
	if (idx != -1) {
		Door& door = _doors[idx];
		if (door.state == DS_IDLE) {
			door.state = DS_WIGGLE;
			door.timer = 0.0f;
		}
	}
}

void Doors::open(int side, int gx, int gy) {
	if (side == 1) {
		open(gy);
	}
	else if (side == 2) {
		open(gx + DOORS_NUM_Y - 1);
	}
	else if (side == 3) {
		open(gy + DOORS_NUM_Y + DOORS_NUM_X - 2);
	}
	else if (side == 4) {
		open(gx + DOORS_NUM_Y * 2 + DOORS_NUM_X - 3);
	}
}

void Doors::close(int idx) {
	Door& door = _doors[idx];
	door.state = DS_CLOSING;
	door.timer = 0.0f;	
}

void Doors::tick(float dt) {
	for (int x = 0; x < _num; ++x) {
		Door& door = _doors[x];
		instance_item& item = get_instance(_render_item, door.id);
		if (door.state == DS_OPENING) {
			float n = door.timer / 0.2f;
			float start = _render_item->mesh->getExtent().z * item.transform.scale.z * -0.5f;
			float end = start + _render_item->mesh->getExtent().z * item.transform.scale.z;
			item.transform.position.z = tweening::interpolate(tweening::linear, start, end, door.timer, 0.2f);
			door.timer += dt;
			if (door.timer >= 0.2f) {
				door.state = DS_OPEN;
				door.timer = 0.0f;
				item.transform.position.z = end;
			}
		}
		else if (door.state == DS_CLOSING) {
			float n = door.timer / 0.2f;
			float start = _render_item->mesh->getExtent().z * item.transform.scale.z * 0.5f;
			float end = _render_item->mesh->getExtent().z * item.transform.scale.z * -0.5f;
			item.transform.position.z = tweening::interpolate(tweening::linear, start, end, door.timer, 0.2f);
			door.timer += dt;
			if (door.timer >= 0.2f) {
				door.state = DS_IDLE;
				door.timer = 0.0f;
				item.transform.position.z = end;
			}
		}
		else if (door.state == DS_WIGGLE) {
			float n = door.timer / 0.6f;
			float s = (0.7f + abs(sin(n * ds::TWO_PI))  * 0.3f) * 0.25f;
			item.transform.scale.x = s;
			item.transform.scale.y = s;
			door.timer += dt;
			if (door.timer >= 0.6f) {
				door.state = DS_IDLE;
				door.timer = 0.0f;
				item.transform.scale.x = 0.25f;
				item.transform.scale.y = 0.25f;
			}
		}
		else if (door.state == DS_OPEN) {
			door.timer += dt;
			if (door.timer >= 1.0f) {
				door.state = DS_CLOSING;
				door.timer = 0.0f;
			}
		}
	}
}

int Doors::checkCollisions(Bullet* bullets, int num, ds::EventStream* events) {
	return num;
}

ds::RenderSettings DoorsViewer::getRenderSettings() {
	ds::RenderSettings rs;
	rs.width = 1024;
	rs.height = 768;
	rs.title = "DoorsViewer";
	rs.clearColor = ds::Color(0.01f, 0.01f, 0.01f, 1.0f);
	rs.multisampling = 4;
	rs.useGPUProfiling = false;
	rs.supportDebug = false;
	return rs;
}

bool DoorsViewer::init() {

	_fpsCamera = new FPSCamera(&_camera);
	_fpsCamera->setPosition(ds::vec3(0, 3, -6), ds::vec3(0.0f, 0.0f, 0.0f));

	_material = new InstancedAmbientLightningMaterial;

	_lightDir[0] = ds::vec3(1.0f, 0.5f, 1.0f);
	_lightDir[1] = ds::vec3(-1.0f, 0.5f, 1.0f);
	_lightDir[2] = ds::vec3(0.0f, 0.5f, 0.5f);

	create_instanced_render_item(&_render_item, "border_box", _material, TOTAL_DOORS);

	_grid = new SimpleGrid;
	_grid->init(SGD_XZ, 8.0f, 0.7f, 0.2f);

	_doors = new Doors(&_render_item);
	_doors->init();

	_selectedDoor = 10;
	return true;
}

void DoorsViewer::tick(float dt) {
	_fpsCamera->update(dt);
	_doors->tick(dt);
}

void DoorsViewer::render() {

	_grid->render(_basicPass, _camera.viewProjectionMatrix);

	draw_instanced_render_item(&_render_item, _basicPass, _camera.viewProjectionMatrix);
}

void DoorsViewer::renderGUI() {
	int state = 1;
	gui::start();
	p2i sp = p2i(10, 760);
	if (gui::begin("Object", &state, &sp, 320)) {
		gui::Value("FPS", ds::getFramesPerSecond());
		gui::Input("Door", &_selectedDoor);
		if (gui::Button("Open door")) {
			if (_selectedDoor != -1) {
				_doors->open(_selectedDoor);
			}
		}
		if (gui::Button("Close door")) {
			if (_selectedDoor != -1) {
				_doors->close(_selectedDoor);
			}
		}
	}
	gui::end();
}