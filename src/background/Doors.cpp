#include "Doors.h"
#include "..\utils\tweening.h"
#include <ds_imgui.h>

static ds::vec3 convert_grid_coords(int x, int z) {
	float cx = static_cast<float>(DOORS_NUM_X) * DOOR_SIZE * 0.5f - DOOR_HALF_SIZE;
	float cy = static_cast<float>(DOORS_NUM_Y) * DOOR_SIZE * 0.5f - DOOR_HALF_SIZE;
	return ds::vec3(-cx + static_cast<float>(x) * DOOR_SIZE, -cy + static_cast<float>(z) * DOOR_SIZE, 0.0f);
}


void Doors::init() {
	int cnt = 0;
	for (int x = 0; x < DOORS_NUM_X - 1; ++x) {
		Door& door = _doors[cnt++];
		door.id = add_instance(_render_item);
		instance_item& item = get_instance(_render_item, door.id);
		item.transform.scale = ds::vec3(0.25f);
		item.transform.position = convert_grid_coords(x, -1);
		item.transform.position.z = _render_item->mesh->getExtent().z * item.transform.scale.z * -0.5f;
		door.state = DS_IDLE;
		door.timer = 0.0f;
	}

	for (int x = 0; x < DOORS_NUM_X - 1; ++x) {
		Door& door = _doors[cnt++];
		door.id = add_instance(_render_item);
		instance_item& item = get_instance(_render_item, door.id);
		item.transform.scale = ds::vec3(0.25f);
		item.transform.position = convert_grid_coords(x, DOORS_NUM_Y - 1);
		item.transform.position.z = _render_item->mesh->getExtent().z * item.transform.scale.z * -0.5f;
		door.state = DS_IDLE;
		door.timer = 0.0f;
	}

	for (int y = 0; y < DOORS_NUM_Y; ++y) {
		Door& door = _doors[cnt++];
		door.id = add_instance(_render_item);
		instance_item& item = get_instance(_render_item, door.id);
		item.transform.scale = ds::vec3(0.25f);
		item.transform.position = convert_grid_coords(0, y);
		item.transform.position.z = _render_item->mesh->getExtent().z * item.transform.scale.z * -0.5f;
		door.state = DS_IDLE;
		door.timer = 0.0f;
	}
	for (int y = 0; y < DOORS_NUM_Y; ++y) {
		Door& door = _doors[cnt++];
		door.id = add_instance(_render_item);
		instance_item& item = get_instance(_render_item, door.id);
		item.transform.scale = ds::vec3(0.25f);
		item.transform.position = convert_grid_coords(DOORS_NUM_X - 1, y);
		item.transform.position.z = _render_item->mesh->getExtent().z * item.transform.scale.z * -0.5f;
		door.state = DS_IDLE;
		door.timer = 0.0f;
	}

	_num = TOTAL_DOORS;
}

void Doors::open(int idx) {
	Door& door = _doors[idx];
	door.state = DS_OPENING;
	door.timer = 0.0f;
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
			float start = _render_item->mesh->getExtent().y * item.transform.scale.y * 0.5f;
			float end = -start;
			item.transform.position.y = tweening::interpolate(tweening::linear, start, end, door.timer, 0.2f);
			door.timer += dt;
			if (door.timer >= 0.2f) {
				door.state = DS_OPEN;
				door.timer = 0.0f;
				item.transform.position.y = end;
			}
		}
		else if (door.state == DS_CLOSING) {
			float n = door.timer / 0.2f;
			float start = _render_item->mesh->getExtent().y * item.transform.scale.y * -0.5f;
			float end = _render_item->mesh->getExtent().y * item.transform.scale.y * 0.5f;
			item.transform.position.y = tweening::interpolate(tweening::linear, start, end, door.timer, 0.2f);
			door.timer += dt;
			if (door.timer >= 0.2f) {
				door.state = DS_IDLE;
				door.timer = 0.0f;
				item.transform.position.y = end;
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