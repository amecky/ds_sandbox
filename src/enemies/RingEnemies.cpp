#include "RingEnemies.h"

void RingEnemies::create(const ds::vec3& pos) {
	float r = 0.5f;
	float step = ds::TWO_PI / 8.0f;
	float angle = 0.0f;
	_ring.position = pos;
	for (int i = 0; i < 8; ++i) {
		_ring.items[i] = add_instance(_render_item);
		instance_item& item = get_instance(_render_item, _ring.items[i]);
		item.transform.scale = ds::vec3(0.2f, 0.2f, 0.2f);
		item.transform.roll = angle;
		item.color = ds::Color(0, 255, 0, 255);
		ds::vec3 p = pos + ds::vec3(cos(angle) * r, sin(angle) * r, 0.0f);
		p.z = _render_item->mesh->getExtent().z * item.transform.scale.z * -0.5f;
		item.transform.position = p;
		angle += step;
	}
	_ring.timer = 0.0f;
}

void RingEnemies::tick(float dt) {
	float r = 0.5f;
	_ring.timer += dt;

	_ring.position.x = cos(_ring.timer) * 2.0f;
	_ring.position.y = sin(_ring.timer) * 2.0f + cos(_ring.timer * 0.25f) * 0.2f + sin(_ring.timer * 4.0f) * 0.4f;

	r = 0.5f + sin(_ring.timer * 6.0f) * 0.04f;
	for (int i = 0; i < 8; ++i) {		
		instance_item& item = get_instance(_render_item, _ring.items[i]);
		float angle = item.transform.roll + dt;
		ds::vec3 p = _ring.position + ds::vec3(cos(angle) * r, sin(angle) * r, 0.0f);
		p.z = _render_item->mesh->getExtent().z * item.transform.scale.z * -0.5f;
		item.transform.position = p;
		item.transform.roll = angle;
	}
}