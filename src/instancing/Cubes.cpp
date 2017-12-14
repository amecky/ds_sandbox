#include "Cubes.h"

static float getAngle(const ds::vec2& v1, const ds::vec2& v2) {
	if (v1 != v2) {
		ds::vec2 vn1 = normalize(v1);
		ds::vec2 vn2 = normalize(v2);
		float dt = dot(vn1, vn2);
		if (dt < -1.0f) {
			dt = -1.0f;
		}
		if (dt > 1.0f) {
			dt = 1.0f;
		}
		float tmp = acos(dt);
		float cross = -1.0f * (vn2 - vn1).y;
		//float crs = cross(vn1, vn2);
		if (cross < 0.0f) {
			tmp = ds::TWO_PI - tmp;
		}
		return tmp;
	}
	else {
		return 0.0f;
	}
}

// ----------------------------------------------------
// init
// ----------------------------------------------------
void Cubes::init() {
	_num_cubes = 0;
	_dbgTTL = 0.2f;

}

// ------------------------------------------------
// separate
// ------------------------------------------------
void Cubes::separate(float minDistance, float relaxation) {
	float sqrDist = minDistance * minDistance;
	for (int i = 0; i < _num_cubes; ++i) {
		Cube& current = _cubes[i];
		if (current.state == CubeState::IDLE) {
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
							separationForce += ds::vec2(dist.x, dist.y);
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
		if (c.state == CubeState::STARTING) {
			if (!step_forward_xy(&item.transform, c.startPosition, c.targetPosition, dt, _settings->startTTL)) {
				item.transform.timer[0] = 0.0f;
				--c.steps;
				c.startPosition = c.targetPosition;
				c.targetPosition.x += _render_item->mesh->getExtent().x * item.transform.scale.x * c.direction.x;
				c.targetPosition.y += _render_item->mesh->getExtent().y * item.transform.scale.y * c.direction.y;
				if (c.steps == 0) {
					c.state = CubeState::IDLE;
				}
			}
			if (!pitch_to(&item.transform, c.startRotation, c.endRotation, dt, _settings->startTTL)) {
				c.startRotation = c.endRotation;
				if (c.startRotation < -ds::TWO_PI) {
					c.startRotation += ds::TWO_PI;
				}
				if (c.startRotation > ds::TWO_PI) {
					c.startRotation -= ds::TWO_PI;
				}
				c.endRotation = c.startRotation - ds::PI * 0.5f;
				item.transform.timer[3] = 0.0f;
			}
		}
		else if (c.state == CubeState::ROTATING) {
			if (!roll_to(&item.transform, c.startRotation, c.endRotation, dt, _dbgTTL)) {
				c.state = CubeState::IDLE;
			}
		}
		//else if (c.state == CubeState::STARTING) {
			//if (!float_in(&item.transform, c.startPosition, c.targetPosition, dt, _dbgTTL)) {
				//c.state = CubeState::IDLE;
			//}
		//}
	}

	for (int i = 0; i < _num_cubes; ++i) {
		_cubes[i].force = ds::vec3(0.0f);
	}


	for (int y = 0; y < _num_cubes; ++y) {
		Cube& c = _cubes[y];
		if (c.state == CubeState::IDLE) {
			instance_item& item = get_instance(_render_item, c.id);
			if (c.type == 1) {
				// seek
				float velocity = 1.0f;
				ds::vec3 diff = playerPosition - item.transform.position;
				ds::vec2 n = normalize(ds::vec2(diff.x, diff.y));
				ds::vec2 desired = n * velocity;
				c.force += ds::vec3(desired);

			}
		}
	}

	separate(0.3f, 0.9f);

	for (int i = 0; i < _num_cubes; ++i) {
		Cube& c = _cubes[i];
		if (c.state == CubeState::IDLE) {
			instance_item& item = get_instance(_render_item, c.id);
			item.transform.position += c.force * dt;
			if (c.type == 1) {
				ds::vec3 delta = item.transform.position - playerPosition;
				item.transform.roll = getAngle(ds::vec2(delta.x, delta.y), ds::vec2(1, 0));
				if (!pitch_to(&item.transform, c.startRotation, c.endRotation, dt, _dbgTTL)) {
					item.transform.timer[3] = 0.0f;
					c.startRotation = c.endRotation;
					if (c.startRotation < -ds::TWO_PI) {
						c.startRotation += ds::TWO_PI;
					}
					if (c.startRotation > ds::TWO_PI) {
						c.startRotation -= ds::TWO_PI;
					}
					c.endRotation = c.startRotation - ds::PI * 0.25f;
				}
				float zp = _render_item->mesh->getExtent().z * item.transform.scale.z * -0.5f;
				item.transform.position.z = zp + abs(sin(item.transform.pitch)) * zp;
			}
		}
	}
}

// ----------------------------------------------------
// create cube
// ----------------------------------------------------
void Cubes::createCube(const ds::vec3& pos, int side) {
	if (_num_cubes < 256) {
		Cube& c = _cubes[_num_cubes++];
		c.id = add_instance(_render_item);
		instance_item& item = get_instance(_render_item, c.id);
		item.color = ds::Color(1.0f, 0.0f, 0.0f, 1.0f);
		ds::vec3 p = pos;
		ds::vec3 scale = ds::vec3(0.2f, 0.2f, 0.2f);
		p.z = _render_item->mesh->getExtent().z * scale.z * -0.5f;		
		float roll = 0.0f;
		switch (side) {
			case 1: p.x = -5.1f; c.direction = ds::vec2(1, 0); roll = ds::PI; break;
			case 2: p.y = +3.3f; c.direction = ds::vec2( 0,-1); roll = ds::PI * 0.5f; break;
			case 3: p.x = +5.1f; c.direction = ds::vec2(-1, 0); roll = 0.0f; break;
			case 4: p.y = -3.3f; c.direction = ds::vec2( 0, 1); roll = -ds::PI * 0.5f; break;
		}
		
		if (side == 1 || side == 3) {
			c.steps = abs(p.x / 0.3f);
		}
		else {
			c.steps = abs(p.y / 0.3f);
		}
		c.startPosition = p;
		c.targetPosition = p;
		c.targetPosition.x += _render_item->mesh->getExtent().x * scale.x * c.direction.x;
		c.targetPosition.y += _render_item->mesh->getExtent().y * scale.y * c.direction.y;
		int rs = ds::random(0.0f, 8.9f);
		initialize(&item.transform, p, scale, roll, 0.0f);
		c.type = 1;
		c.state = CubeState::STARTING;
		c.startRotation = 0.0f;
		c.endRotation = -ds::PI * 0.25f;
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
void Cubes::checkCollisions(bullets::Bullets& bullets, ds::EventStream * events) {
	int y = 0;
	while ( y < _num_cubes) {
		const Cube& c = _cubes[y];
		const instance_item& item = get_instance(_render_item,c.id);
		ds::vec3 ip = item.transform.position;
		int cnt = 0;
		bool hit = false;
		while (cnt < bullets.size()) {
			const bullets::Bullet& b = bullets.get(cnt);
			if (collides(ip, 0.2f, b.pos, 0.1f)) {
				bullets.remove(cnt);
				ds::vec3 bp = b.pos;
				events->add(100, &bp, sizeof(ds::vec3));
				//--ret;
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
	//return ret;
}