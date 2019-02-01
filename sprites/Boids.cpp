#include "Boids.h"
#include "math.h"
#include "Obstacles.h"

const static ds::vec4 BOID_RECT = ds::vec4(260, 300, 60, 60);
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
		container->colors = new ds::Color[maxBoids];
		container->queue.num = 0;
	}

	// -----------------------------------------------------------------------
	// shutdown
	// -----------------------------------------------------------------------
	void shutdown(BoidContainer* container) {
		delete[] container->colors;
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
			container->colors[container->num] = ds::Color(255, 0, 0, 255);
			++container->num;
		}
	}

	const ds::vec2 STARTING_POINTS[] = {ds::vec2(100,600),ds::vec2(500,600),ds::vec2(900,600),ds::vec2(900,350),ds::vec2(900,100),ds::vec2(500,100),ds::vec2(100,100),ds::vec2(100,350)};
	// -----------------------------------------------------------------------
	// add
	// -----------------------------------------------------------------------
	void add(BoidContainer* container, int count, const ds::vec2& target) {
		int rnd = ds::random(0.0f, 7.9f);
		ds::vec2 start = STARTING_POINTS[rnd];

		ds::vec2 desired = target - start;
		float angle = calculate_rotation(desired);

		QueueEntry& sqe = container->queue.entries[container->queue.num++];
		sqe.state = 1;
		sqe.timer = 0.0f;
		sqe.timerOffset = 0.0f;
		sqe.ttl = 0.6f;
		sqe.pos = start;
		sqe.rotation = angle;
		//add(container, start, target);
		float step = ds::TWO_PI * 0.25f;
		float an = 0.0f;
		for (int i = 1; i < count; ++i) {
			if (container->num < container->max) {
				ds::vec2 cur = start + 40.0f * (ds::vec2(cos(an), sin(an)));
				//add(container, cur, target);
				QueueEntry& sqe = container->queue.entries[container->queue.num++];
				sqe.state = 1;
				sqe.timer = 0.0f;
				sqe.timerOffset = i * 0.2f;
				sqe.ttl = 0.6f;
				sqe.pos = start + 40.0f * (ds::vec2(cos(an), sin(an)));
				sqe.rotation = angle;
				++an;
			}
		}
	}

	// -----------------------------------------------------------------------
	// reset forces
	// -----------------------------------------------------------------------
	void reset_forces(BoidContainer* container) {
		for (int i = 0; i < container->num; ++i) {
			container->accelerations[i].setX(0.0f);
			container->accelerations[i].setY(0.0f);
		}
	}

	// -----------------------------------------------------------------------
	// apply fources
	// -----------------------------------------------------------------------
	void apply_forces(BoidContainer* container, float dt) {
		float maxAngle = ds::TWO_PI - ds::TWO_PI * 0.2f;
		for (int i = 0; i < container->num; ++i) {
			container->velocities[i] += container->accelerations[i] * dt;
			//container->velocities[i] = limit(container->velocities[i], 120.0f);
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
	static void seek(BoidContainer* container, const ds::vec2& target, ObstaclesContainer* obstacles, float velocity) {
		for (int i = 0; i < container->num; ++i) {
			bool spotting = true;
			ds::vec2 desired = target - container->positions[i];
			for (int j = 0; j < obstacles->num; ++j) {
				float r = obstacles->scales[j] * 30.0f * 0.5f;
				if (math::line_circle_intersection(container->positions[i], desired, obstacles->positions[j], r)) {
					spotting = false;
					container->colors[i] = ds::Color(0, 0, 255, 255);
				}
			}
			if (spotting) {
				ds::vec2 n = normalize(desired);
				ds::vec2 df = n * velocity;
				ds::vec2 steer = df - container->velocities[i];
				container->accelerations[i] += steer;
				container->colors[i] = ds::Color(255, 0, 255, 255);
			}
		}
	}

	// -----------------------------------------------------------------------
	// move
	// -----------------------------------------------------------------------
	void move(BoidContainer* container, const ds::vec2& target, ObstaclesContainer* obstacles, float dt) {
		
		if (container->settings.separate) {
			separate(container, container->settings.minDistance, container->settings.relaxation);
		}
		
		if (container->settings.seek) {
			seek(container, target, obstacles, container->settings.seekVelocity);
		}

		for (int i = 0; i <container->num; ++i) {
			ds::vec2 p = container->positions[i] + container->velocities[i];
			bool bounced = false;
			if (p.x() < BOUNDING_RECT.x || p.x() > BOUNDING_RECT.z) {
				container->velocities[i].setX(container->velocities[i].x() * -1.0f);
				bounced = true;
			}
			if (p.y() < BOUNDING_RECT.y || p.y() > BOUNDING_RECT.w) {
				container->velocities[i].setY(container->velocities[i].y() * -1.0f);
				bounced = true;
			}
			if (bounced) {
				container->rotations[i] = calculate_rotation(container->velocities[i]);
			}
		}
		
	}

	void tick_queue(BoidContainer* container, float dt) {
		for (int i = 0; i < container->queue.num; ++i) {
			QueueEntry& qe = container->queue.entries[i];
			if (qe.timerOffset > 0.0f) {
				qe.timerOffset -= dt;
			}
			else {
				qe.timer += dt;
				if (qe.timer >= qe.ttl) {
					add(container, qe.pos, ds::vec2(512, 384));
					container->queue.entries[i] = container->queue.entries[container->queue.num - 1];
					--container->queue.num;
				}
			}
		}
	}

	void kill_boid(BoidContainer* container, int index) {
		if (container->num > 0) {
			container->positions[index] = container->positions[container->num - 1];
			container->rotations[index] = container->rotations[container->num - 1];
			container->velocities[index] = container->velocities[container->num - 1];
			container->accelerations[index] = container->accelerations[container->num - 1];
			container->colors[index] = container->colors[container->num - 1];
		}
		--container->num;
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
					container->colors[i] = container->colors[container->num - 1];
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
			sprites::draw(container->settings.textureId, container->positions[i], BOID_RECT, ds::vec2(1.0f), container->rotations[i], container->colors[i]);
		}
		for (int i = 0; i < container->queue.num; ++i) {
			QueueEntry& qe = container->queue.entries[i];
			float sx = 0.5f + sin(ds::TWO_PI*qe.timer / qe.ttl);
			sprites::draw(container->settings.textureId, qe.pos, BOID_RECT, ds::vec2(sx), qe.rotation, ds::Color(255,0,255,255));
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
				ds::vec2 df = n * container->settings.avoidVelocity;
				ds::vec2 steer = df - container->velocities[i];
				container->accelerations[i] -= steer;
			}
		}
	}

}
