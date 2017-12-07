#include "RingEnemies.h"

// ----------------------------------------------------
// create ring
// ----------------------------------------------------
void RingEnemies::create(const ds::vec3& pos) {
	if (_num < 16) {
		Ring& ring = _rings[_num++];
		float r = 0.5f;
		float step = ds::TWO_PI / 8.0f;
		float angle = 0.0f;
		ring.timer = 0.0f;
		ring.position.x = cos(ring.timer) * 2.0f;
		ring.position.y = sin(ring.timer) * 2.0f + cos(ring.timer * 0.25f) * 0.2f + sin(ring.timer * 4.0f) * 0.4f;
		for (int i = 0; i < 8; ++i) {
			ring.items[i] = add_instance(_render_item);
			instance_item& item = get_instance(_render_item, ring.items[i]);
			item.transform.scale = ds::vec3(0.2f, 0.2f, 0.2f);
			item.transform.roll = angle;
			item.color = ds::Color(0, 255, 0, 255);
			ds::vec3 p = pos + ds::vec3(cos(angle) * r, sin(angle) * r, 0.0f);
			p.z = _render_item->mesh->getExtent().z * item.transform.scale.z * -0.5f;
			item.transform.position = p;
			angle += step;
		}		
		ring.num = 8;
	}
}

// ----------------------------------------------------
// tick
// ----------------------------------------------------
void RingEnemies::tick(float dt) {
	float r = 0.5f;
	for (int j = 0; j < _num; ++j) {
		Ring& ring = _rings[j];
		ring.timer += dt;

		ring.position.x = cos(ring.timer) * 2.0f + sin(ring.timer * 4.0f) * 0.4f;
		ring.position.y = sin(ring.timer) * 2.0f + cos(ring.timer * 0.25f) * 0.2f + sin(ring.timer * 4.0f) * 0.4f;

		r = 0.5f + sin(ring.timer * 6.0f) * 0.04f;
		for (int i = 0; i < 8; ++i) {
			instance_item& item = get_instance(_render_item, ring.items[i]);
			float angle = item.transform.roll + dt;
			ds::vec3 p = ring.position + ds::vec3(cos(angle) * r, sin(angle) * r, 0.0f);
			p.z = _render_item->mesh->getExtent().z * item.transform.scale.z * -0.5f;
			item.transform.position = p;
			item.transform.roll = angle;
		}
	}
}

// ----------------------------------------------------
// collides
// ----------------------------------------------------
static bool collides(const ds::vec3& p1, float r1, const ds::vec3& p2, float r2) {
	ds::vec2 d = ds::vec2(p2.x, p2.y) - ds::vec2(p1.x, p1.y);
	if (sqr_length(d) < r1 * r2) {
		return true;
	}
	return false;
}

int RingEnemies::checkCollisions(Bullet * bullets, int num, ds::EventStream * events) {
	int ret = num;
	for (int i = 0; i < _num; ++i) {
		int y = 0;
		Ring& ring = _rings[i];
		while (y < ring.num) {
			instance_item& item = get_instance(_render_item, ring.items[y]);
			ds::vec3 ip = item.transform.position;
			int cnt = 0;
			bool hit = false;
			while (cnt < ret) {
				const Bullet& b = bullets[cnt];
				if (collides(ip, 0.2f, b.pos, 0.1f)) {
					bullets[cnt] = bullets[ret - 1];
					ds::vec3 bp = b.pos;
					events->add(100, &bp, sizeof(ds::vec3));
					--ret;
					hit = true;
				}
				else {
					++cnt;
				}
			}
			if (hit) {
				events->add(101, &item.transform.position, sizeof(ds::vec3));
				if (ring.num > 1) {
					ring.items[y] = ring.items[ring.num - 1];
				}
				remove_instance(_render_item, item.id);
				--ring.num;
			}
			else {
				++y;
			}
		}
		if (ring.num == 0) {
			// FIXME: move out
		}
	}
	return ret;
}
