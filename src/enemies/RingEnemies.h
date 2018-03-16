#pragma once
#include <diesel.h>
#include "..\utils\RenderItem.h"
#include "..\instancing\InstanceCommon.h"
#include <ds_base_app.h>
#include "..\instancing\Bullets.h"

class Enemies {

public:
	Enemies(instanced_render_item* render_item) : _render_item(render_item) {}
	virtual ~Enemies() {}
	virtual void create(const ds::vec3& pos) = 0;
	virtual void tick(float dt) = 0;
	virtual void checkCollisions(bullets::Bullets& bullets, ds::EventStream* events) = 0;
protected:
	instanced_render_item* _render_item;
};

class RingEnemies : public Enemies {

	struct Ring {
		ds::vec3 position;
		float radius;
		ID items[8];
		float timer;
		int num;
	};

public:
	RingEnemies(instanced_render_item* render_item) : Enemies(render_item) , _num(0) {}
	void create(const ds::vec3& pos);
	void tick(float dt);
	void checkCollisions(bullets::Bullets& bullets, ds::EventStream* events);
private:
	Ring _rings[16];
	int _num;
};