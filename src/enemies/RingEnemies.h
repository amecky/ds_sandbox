#pragma once
#include <diesel.h>
#include "..\utils\RenderItem.h"

class Enemies {

public:
	Enemies(instanced_render_item* render_item) : _render_item(render_item) {}
	virtual ~Enemies() {}
	virtual void create(const ds::vec3& pos) = 0;
	virtual void tick(float dt) = 0;
protected:
	instanced_render_item* _render_item;
};

class RingEnemies : public Enemies {

	struct Ring {
		ds::vec3 position;
		float radius;
		ID items[8];
		float timer;
	};

public:
	RingEnemies(instanced_render_item* render_item) : Enemies(render_item) {}
	void create(const ds::vec3& pos);
	void tick(float dt);
private:
	Ring _ring;
};