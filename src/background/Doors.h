#pragma once
#include <diesel.h>
#include "..\TestApp.h"
#include "..\utils\RenderItem.h"
#include "..\instancing\InstanceCommon.h"
#include "..\utils\EventStream.h"
#include "..\kenney\Camera.h"
#include "..\utils\SimpleGrid.h"
#include "..\Material.h"

const int DOORS_NUM_X = 30;
const int DOORS_NUM_Y = 20;
const int TOTAL_DOORS = 2 * (DOORS_NUM_X + DOORS_NUM_Y);
const float DOOR_SIZE = 0.3f;
const float DOOR_HALF_SIZE = 0.3f;

class Doors {

	enum DoorState {
		DS_IDLE,DS_OPENING,DS_CLOSING,DS_OPEN,DS_WIGGLE
	};

	struct Door {
		ID id;
		DoorState state;
		float timer;		
	};

public:
	Doors(instanced_render_item* render_item) : _render_item(render_item) {}
	~Doors() {}
	void init();
	void tick(float dt);
	int checkCollisions(Bullet* bullets, int num, ds::EventStream* events);
	void open(int idx);
	void wiggle(int gx, int gy);
	void close(int idx);
	void open(int side, int gx, int gy);
private:
	int getIndex(int side, int gx, int gy);
	void add(int gx, int gy);
	instanced_render_item* _render_item;
	Door _doors[TOTAL_DOORS];
	int _num;
};


class DoorsViewer : public TestApp {

public:
	DoorsViewer() : TestApp() {}

	virtual ~DoorsViewer() {}

	ds::RenderSettings getRenderSettings();

	bool usesGUI() {
		return true;
	}

	bool init();

	void tick(float dt);

	void render();

	void renderGUI();

private:
	instanced_render_item _render_item;
	Doors* _doors;
	InstancedAmbientLightningMaterial* _material;
	SimpleGrid* _grid;
	FPSCamera* _fpsCamera;
	ds::vec3 _lightDir[3];
	int _selectedDoor;
};

