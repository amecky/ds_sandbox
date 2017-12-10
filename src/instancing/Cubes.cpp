#include "Cubes.h"

// ----------------------------------------------------
// init
// ----------------------------------------------------
void Cubes::init() {
	_num_cubes = 0;
	_dbgTTL = 0.4f;

}

// ------------------------------------------------
// separate
// ------------------------------------------------
void Cubes::separate(float minDistance, float relaxation) {
	float sqrDist = minDistance * minDistance;
	for (int i = 0; i < _num_cubes; ++i) {
		Cube& current = _cubes[i];
		instance_item& item = get_instance(_render_item, current.id);
		int cnt = 0;
		ds::vec2 separationForce = ds::vec2(0, 0);
		ds::vec2 averageDirection = ds::vec2(0, 0);
		ds::vec2 distance = ds::vec2(0, 0);
		ds::vec3 currentPos = item.transform.position;
		for (int j = 0; j < _num_cubes; j++) {
			if (i != j) {
				if (_cubes[j].type == 1) {
					instance_item& other = get_instance(_render_item, _cubes[j].id);
					ds::vec3 dist = other.transform.position - currentPos;
					if (sqr_length(dist) < sqrDist) {
						++cnt;
						separationForce += ds::vec2(dist.x,dist.y);
						separationForce = normalize(separationForce);
						separationForce = separationForce * relaxation;
						averageDirection += separationForce;
					}
				}
			}
		}
		if (cnt > 0) {
			current.force -= averageDirection;
		}
	}
}

void Cubes::stepForward() {
	for (int i = 0; i < _num_cubes; ++i) {
		Cube& c = _cubes[i];
		instance_item& item = get_instance(_render_item, c.id);
		if (c.state == CubeState::IDLE) {
			reset_timers(&item.transform);
			c.startPosition = item.transform.position;
			ds::vec3 vel = ds::vec3(cos(item.transform.roll) * item.transform.scale.x, item.transform.scale.x * sin(item.transform.roll), 0.0f);
			c.targetPosition = c.startPosition + vel;
			c.targetPosition.z =_render_item->mesh->getExtent().z * 0.5f * item.transform.scale.z * -1.0f;
			c.startRotation = item.transform.pitch;
			c.endRotation = c.startRotation + ds::PI * 0.5f;
			c.state = CubeState::STEPPING;
		}
	}
}

void Cubes::rotateCubes() {
	for (int i = 0; i < _num_cubes; ++i) {
		Cube& c = _cubes[i];
		instance_item& item = get_instance(_render_item, c.id);
		if (c.state == CubeState::IDLE) {
			reset_timers(&item.transform);
			float r = ds::random(0.0f, 1.0f);
			float an = ds::PI * 0.5f;
			if (r >= 0.5f) {
				an *= -1.0f;
			}
			c.startRotation = item.transform.roll;
			c.endRotation = c.startRotation + an;
			c.state = CubeState::ROTATING;
		}
	}
}

// ----------------------------------------------------
// tick
// ----------------------------------------------------
void Cubes::tick(float dt, const ds::vec3& playerPosition) {


	for (int i = 0; i < _num_cubes; ++i) {
		Cube& c = _cubes[i];
		instance_item& item = get_instance(_render_item, c.id);
		if (c.state == CubeState::STEPPING) {
			if (!step_forward_xy(&item.transform, c.startPosition, c.targetPosition, dt, _dbgTTL)) {
				c.state = CubeState::IDLE;
			}
			if (!pitch_to(&item.transform, c.startRotation, c.endRotation, dt, _dbgTTL)) {
				c.state = CubeState::IDLE;
			}
		}
		else if (c.state == CubeState::ROTATING) {
			if (!roll_to(&item.transform, c.startRotation, c.endRotation, dt, _dbgTTL)) {
				c.state = CubeState::IDLE;
			}
		}
	}

	for (int i = 0; i < _num_cubes; ++i) {
		_cubes[i].force = ds::vec3(0.0f);
	}


	for (int y = 0; y < _num_cubes; ++y) {
		Cube& c = _cubes[y];
		instance_item& item = get_instance(_render_item, c.id);
		if (c.type == 1) {
			// seek
			float velocity = 1.0f;
			ds::vec3 diff = playerPosition - item.transform.position;
			ds::vec2 n = normalize(ds::vec2(diff.x,diff.y));
			ds::vec2 desired = n * velocity;
			c.force += ds::vec3(desired);
		}
	}

	separate(0.2f, 0.9f);

	for (int i = 0; i < _num_cubes; ++i) {
		instance_item& item = get_instance(_render_item, _cubes[i].id);
		item.transform.position += _cubes[i].force * dt;
	}
}

// ----------------------------------------------------
// create cube
// ----------------------------------------------------
void Cubes::createCube(const ds::vec3& pos) {
	if (_num_cubes < 256) {
		Cube& c = _cubes[_num_cubes++];
		c.id = add_instance(_render_item);
		instance_item& item = get_instance(_render_item, c.id);
		item.color = ds::Color(1.0f, 0.0f, 0.0f, 1.0f);
		ds::vec3 p = pos;
		ds::vec3 scale = ds::vec3(0.2f, 0.2f, 0.2f);
		p.z = _render_item->mesh->getExtent().z * scale.z * -0.5f;
		int rs = ds::random(0.0f, 8.9f);
		float roll = static_cast<float>(rs) * ds::TWO_PI * 0.25f;// 0.0f;// ds::PI * 0.25f;// ds::random(0.0f, ds::TWO_PI);
		initialize(&item.transform, p, scale, roll, 0.0f);
		c.state = CubeState::IDLE;
	}
}

// ----------------------------------------------------
// collides
// ----------------------------------------------------
bool collides(const ds::vec3& p1, float r1, const ds::vec3& p2, float r2) {
	ds::vec2 d = ds::vec2(p2.x,p2.y) - ds::vec2(p1.x,p1.y);
	if (sqr_length(d) < r1 * r2) {
		return true;
	}
	return false;
}

// ----------------------------------------------------
// check collisions with bullets
// ----------------------------------------------------
int Cubes::checkCollisions(Bullet* bullets, int num, ds::EventStream* events) {
	int ret = num;
	int y = 0;
	while ( y < _num_cubes) {
		const Cube& c = _cubes[y];
		const instance_item& item = get_instance(_render_item,c.id);
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
			ds::vec3 p = item.transform.position;
			events->add(101, &p, sizeof(ds::vec3));
			remove_instance(_render_item,c.id);
			if (_num_cubes > 1) {
				_cubes[y] = _cubes[_num_cubes - 1];
			}
			--_num_cubes;
		}
		else {
			++y;
		}
	}
	return ret;
}