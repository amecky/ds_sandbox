#include "Boids.h"
#include "math.h"
#include "Obstacles.h"

const static ds::vec4 BOID_RECT = ds::vec4(0, 225, 30, 30);
const static ds::vec4 BOUNDING_RECT = ds::vec4(50.0f, 50.0f, 900.0f, 650.0f);


namespace boid {

	struct BoidContext {

	};

	static BoidContext* _boidCtx = 0;

	// ---------------------------------------
	// seekFunc
	// ---------------------------------------
	void seekFunc(BoidContainer* container, BoidSettings* settings, const ds::vec2& target) {
		for (int i = 0; i < container->num; ++i) {
			ds::vec2 desired = target - container->positions[i];
			ds::vec2 n = normalize(desired);
			ds::vec2 df = n * settings->seekVelocity;
			ds::vec2 steer = df - container->velocities[i];
			container->accelerations[i] += steer;
		}
	}

	// -----------------------------------------------------------------------
	// initialize
	// -----------------------------------------------------------------------
	void initialize(BoidContainer* container, int maxBoids) {
		container->num = 0;
		container->max = maxBoids;
		container->accelerations = new ds::vec2[maxBoids];
		container->positions = new ds::vec2[maxBoids];
		container->velocities = new ds::vec2[maxBoids];
		container->rotations = new float[maxBoids];
		container->states = new int[maxBoids];
	}

	// -----------------------------------------------------------------------
	// shutdown
	// -----------------------------------------------------------------------
	void shutdown(BoidContainer* container) {
		delete[] container->accelerations;
		delete[] container->positions;
		delete[] container->velocities;
		delete[] container->rotations;
		delete[] container->states;
	}

	void add(BoidContainer* container, const ds::vec2& p, const ds::vec2& target) {
		if (container->num < container->max) {
			container->positions[container->num] = p;
			ds::vec2 desired = target - container->positions[container->num];
			ds::vec2 n = normalize(desired);
			ds::vec2 df = n * container->settings.seekVelocity;
			float angle = calculate_rotation(df);
			container->rotations[container->num] = angle;
			container->velocities[container->num] = df;
			container->states[container->num] = 1;
			++container->num;
		}
	}
	// -----------------------------------------------------------------------
	// add
	// -----------------------------------------------------------------------
	void add(BoidContainer* container, int count, const ds::vec2& target) {
		for (int i = 0; i < count; ++i) {
			if (container->num < container->max) {
				add(container, ds::vec2(ds::random(100.0f, 900.0f), ds::random(100.0f, 600.0f)), target);
			}
		}
	}

	// -----------------------------------------------------------------------
	// reset forces
	// -----------------------------------------------------------------------
	void reset_forces(BoidContainer* container) {
		for (int i = 0; i < container->num; ++i) {
			container->accelerations[i] = 0.0f;
		}
	}

	// -----------------------------------------------------------------------
	// apply fources
	// -----------------------------------------------------------------------
	void apply_forces(BoidContainer* container, float dt) {
		float maxAngle = ds::TWO_PI - ds::TWO_PI * 0.2f;
		for (int i = 0; i < container->num; ++i) {
			container->velocities[i] += container->accelerations[i] * dt;
			container->velocities[i] = limit(container->velocities[i], 120.0f);
			float angle = calculate_rotation(container->velocities[i]);
			container->rotations[i] = angle;
			container->positions[i] += container->velocities[i] * dt;
		}
	}

	// -----------------------------------------------------------------------
	// separate
	// -----------------------------------------------------------------------
	static void separate(BoidContainer* container, float minDistance, float relaxation) {
		float sqrDist = minDistance * minDistance;
		for (int i = 0; i < container->num; ++i) {
			int cnt = 0;
			ds::vec2 separationForce = ds::vec2(0, 0);
			ds::vec2 averageDirection = ds::vec2(0, 0);
			ds::vec2 distance = ds::vec2(0, 0);
			ds::vec2 currentPos = container->positions[i];
			for (int j = 0; j < container->num; j++) {
				if (i != j) {
					ds::vec2 dist = container->positions[j] - currentPos;
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
				container->accelerations[i] -= averageDirection;
			}
		}
	}

	// -----------------------------------------------------------------------
	// seek
	// -----------------------------------------------------------------------
	static void seek(BoidContainer* container, const ds::vec2& target, float velocity) {
		for (int i = 0; i < container->num; ++i) {
			ds::vec2 desired = target - container->positions[i];
			ds::vec2 n = normalize(desired);
			ds::vec2 df = n * velocity;
			ds::vec2 steer = df - container->velocities[i];
			container->accelerations[i] += steer;
		}
	}

	// -----------------------------------------------------------------------
	// move
	// -----------------------------------------------------------------------
	void move(BoidContainer* container, const ds::vec2& target, float dt) {
		
		if (container->settings.separate) {
			separate(container, container->settings.minDistance, container->settings.relaxation);
		}
		if (!container->settings.seek) {
			for (int i = 0; i <container->num; ++i) {
				ds::vec2 p = container->positions[i] + container->velocities[i];
				bool bounced = false;
				if (p.x < BOUNDING_RECT.x || p.x > BOUNDING_RECT.z) {
					container->velocities[i].x *= -1.0f;
					bounced = true;
				}
				if (p.y < BOUNDING_RECT.y || p.y > BOUNDING_RECT.w) {
					container->velocities[i].y *= -1.0f;
					bounced = true;
				}
				if (bounced) {
					container->rotations[i] = calculate_rotation(container->velocities[i]);
					container->accelerations[i] += container->velocities[i] * dt;
				}
			}
		}
		if (container->settings.seek) {
			seek(container, target, container->settings.seekVelocity);
		}
		
	}

	// -----------------------------------------------------------------------
	// kill boids close to target
	// -----------------------------------------------------------------------
	void kill_boids(BoidContainer* container, const ds::vec2& target, float dist) {
		float sqrDist = dist * dist;
		for (int i = 0; i < container->num; ++i) {
			float d = sqr_length(container->positions[i] - target);
			if (d < sqrDist) {
				if (container->num > 0) {
					container->positions[i] = container->positions[container->num - 1];
					container->rotations[i] = container->rotations[container->num - 1];
					container->velocities[i] = container->velocities[container->num - 1];
					container->accelerations[i] = container->accelerations[container->num - 1];
				}
				--container->num;
			}
		}
	}

	// -----------------------------------------------------------------------
	// render
	// -----------------------------------------------------------------------
	void render(BoidContainer* container) {
		for (int i = 0; i < container->num; ++i) {
			sprites::draw(container->settings.textureId, container->positions[i], BOID_RECT, ds::vec2(1.0f), container->rotations[i]);
		}
	}

	void avoid(BoidContainer* container, ObstaclesContainer* obstacles) {		
		float mind = 60.0f;
		for (int i = 0; i < container->num; ++i) {
			int idx = -1;
			float current = 1024.0f;
			for (int j = 0; j < obstacles->num; ++j) {
				ds::vec2 delta = container->positions[i] - obstacles->positions[j];
				if (sqr_length(delta) < mind * mind) {
					if (current * current > sqr_length(delta)) {
						idx = j;
						current = sqr_length(delta);
					}
				}
			}
			if (idx != -1) {
				ds::vec2 desired = obstacles->positions[idx] - container->positions[i];
				ds::vec2 n = normalize(desired);
				ds::vec2 df = n * 300.0f;
				ds::vec2 steer = df - container->velocities[i];
				container->accelerations[i] -= steer;
			}
		}
	}

}
