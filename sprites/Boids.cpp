#include "Boids.h"
#include "math.h"

const static int MAX_BOIDS = 1024;
const static ds::vec4 BOID_RECT = ds::vec4(0, 225, 30, 30);
const static ds::vec4 BOUNDING_RECT = ds::vec4(50.0f, 50.0f, 900.0f, 650.0f);


// ---------------------------------------
// cstr
// ---------------------------------------
Boids::Boids(RID textureID, BoidSettings* settings) : _textureID(textureID), _settings(settings), _num(0) {
	_boids.forces = new ds::vec2[MAX_BOIDS];
	_boids.positions = new ds::vec2[MAX_BOIDS];
	_boids.velocities = new ds::vec2[MAX_BOIDS];
	_boids.rotations = new float[MAX_BOIDS];
	add(32);
}

// ---------------------------------------
// dest
// ---------------------------------------
Boids::~Boids() {

}

// ---------------------------------------
// add
// ---------------------------------------
void Boids::add(int count) {
	for (int i = 0; i < count; ++i) {
		if (_num < MAX_BOIDS) {
			_boids.positions[_num + i] = ds::vec2(ds::random(100.0f, 900.0f), ds::random(100.0f, 600.0f));
			float angle = ds::random(0.0f, ds::TWO_PI);
			_boids.rotations[_num + i] = angle;
			_boids.velocities[_num + i] = ds::vec2(cosf(angle), sinf(angle)) * ds::random(100.0f, 250.0f);
			++_num;
		}
	}
}

// ------------------------------------------------
// separate
// ------------------------------------------------
void Boids::separate(float minDistance, float relaxation, float dt) {
	float sqrDist = minDistance * minDistance;
	for (int i = 0; i < _num; ++i) {
		int cnt = 0;
		ds::vec2 separationForce = ds::vec2(0, 0);
		ds::vec2 averageDirection = ds::vec2(0, 0);
		ds::vec2 distance = ds::vec2(0, 0);
		ds::vec2 currentPos = _boids.positions[i];
		for (int j = 0; j < _num; j++) {
			if (i != j) {
				ds::vec2 dist = _boids.positions[j] - currentPos;
				if (sqr_length(dist) < sqrDist) {
					++cnt;
					separationForce += dist;
					separationForce = normalize(separationForce);
					separationForce = separationForce * relaxation;
					averageDirection += separationForce;
				}
			}
		}
		if (cnt > 0) {
			_boids.forces[i] -= averageDirection;
		}
	}
}

// ---------------------------------------
// seek
// ---------------------------------------
void Boids::seek(const ds::vec2& target, float velocity, float dt) {
	for (int i = 0; i < _num; ++i) {
		ds::vec2 diff = target - _boids.positions[i];
		ds::vec2 n = normalize(diff);
		ds::vec2 desired = n * velocity;
		if (sqr_length(diff) > 100.0f) {
			_boids.forces[i] += desired * dt;
		}
	}
}

// ---------------------------------------
// move
// ---------------------------------------
void Boids::move(const ds::vec2& target, float dt) {

	for (int i = 0; i < _num; ++i) {
		_boids.forces[i] = 0.0f;
	}
	if (_settings->separate) {
		separate(_settings->minDistance, _settings->relaxation, dt);
	}
	if (!_settings->seek) {
		for (int i = 0; i < _num; ++i) {
			ds::vec2 p = _boids.positions[i] + _boids.forces[i];
			bool bounced = false;
			if (p.x < BOUNDING_RECT.x || p.x > BOUNDING_RECT.z) {
				_boids.velocities[i].x *= -1.0f;
				bounced = true;
			}
			if (p.y < BOUNDING_RECT.y || p.y > BOUNDING_RECT.w) {
				_boids.velocities[i].y *= -1.0f;
				bounced = true;
			}
			if (bounced) {
				_boids.rotations[i] = calculate_rotation(_boids.velocities[i]);
				_boids.forces[i] += _boids.velocities[i] * dt;
			}
		}
	}
	if (_settings->seek) {
		seek(target, _settings->seekVelocity, dt);
	}
	float maxAngle = ds::TWO_PI - ds::TWO_PI * 0.2f;
	for (int i = 0; i < _num; ++i) {
		float angle = calculate_rotation(_boids.forces[i]);
		_boids.rotations[i] = angle;
		_boids.positions[i] += _boids.forces[i];
	}
}

// ---------------------------------------
// render
// ---------------------------------------
void Boids::render() {
	for (int i = 0; i < _num; ++i) {
		sprites::draw(_textureID, _boids.positions[i], BOID_RECT, ds::vec2(1.0f), _boids.rotations[i]);
	}
}
