#include "particle_manager.h"
#include <PluginRegistry.h>
#include <random>

static const float PI = 3.141592654f;
static const float TWO_PI = 2.0f * PI;

static std::mt19937 mt;

static void init_random(unsigned long seed) {
	std::random_device r;
	std::seed_seq new_seed{ r(), r(), r(), r(), r(), r(), r(), r() };
	mt.seed(new_seed);
}


float random(float min, float max) {
	std::uniform_real_distribution<float> dist(min, max);
	return dist(mt);
}

static ds::vec2 get_radial(float angle, float v) {
	return{ static_cast<float>(cos(angle)) * v,static_cast<float>(sin(angle)) * v };
}

#ifdef DEBUG
extern "C" {
#endif

	static void pm_allocate(ParticleData* data, uint16_t max_particles) {
		uint32_t sz = max_particles * (sizeof(ds::vec2) + sizeof(ds::vec2) + sizeof(float) + sizeof(ds::Color) + sizeof(float) + sizeof(float) + sizeof(ds::vec4));
		data->buffer = new char[sz];
		data->capacity = max_particles;
		data->positions = (ds::vec2*)data->buffer;
		data->scales = (ds::vec2*)(data->positions + max_particles);
		data->rotations = (float*)(data->scales + max_particles);
		data->colors = (ds::Color*)(data->rotations + max_particles);
		data->timers = (float*)(data->colors + max_particles);
		data->ttls = (float*)(data->timers + max_particles);
		data->additionals = (ds::vec4*)(data->ttls + max_particles);
		data->index = 0;
	}

	static void pm_reallocate(ParticleData* data, uint16_t max_particles) {

	}

	static void remove_particle(ParticleData* data, uint16_t index) {
		if (data->index > 0) {
			uint16_t end = data->index - 1;
			data->positions[end] = data->positions[index];
			data->scales[end] = data->scales[index];
			data->rotations[end] = data->rotations[index];
			data->colors[end] = data->colors[index];
			data->timers[end] = data->timers[index];
			data->ttls[end] = data->ttls[index];
			data->additionals[end] = data->additionals[index];
			--data->index;
		}
	}
	static void pm_tick(ParticleData* data, float dt) {
		uint16_t i = 0;
		while (i < data->index) {
			data->timers[i] += dt;
			if (data->timers[i] >= data->ttls[i]) {
				remove_particle(data, i);
			}
			else {
				++i;
			}
		}

	}

	static bool pm_wake_up(ParticleData* data,uint16_t size, uint16_t* start, uint16_t* count) {
		*start = data->index;
		*count = size;
		if (data->index + *count > data->capacity) {
			return false;
		}
		if (*count + data->index > data->capacity) {
			*count = data->capacity - *start;
		}
		data->index += *count;
		return true;
	}

	void pm_emitt_explosion(ParticleData* data, const ds::vec2& pos) {
		uint16_t start = 0;
		uint16_t count = 0;
		int num = 256;
		float numf = static_cast<float>(num);
		if (pm_wake_up(data, num, &start, &count)) {
			for (uint16_t i = 0; i < count; ++i) {
				float rx = pos.x + cos(i / numf * TWO_PI) * random(60.0f, 80.0f);
				float ry = pos.y + sin(i / numf * TWO_PI) * random(60.0f, 80.0f);
				data->positions[start + i] = ds::vec2(rx, ry);
				data->additionals[start + i].x = random(0.4f, 0.8f);
				data->additionals[start + i].y = random(-1.0f, 1.0f);
				data->scales[start + i] = ds::vec2(data->additionals[start + i].x);
				data->rotations[start + i] = i / numf * TWO_PI;
				data->colors[start + i] = ds::Color(255,255,183,255);
				data->timers[start + i] = 0.0f;
				data->ttls[start + i] = random(0.8f,1.4f);
			}
		}
	}

	

	void pm_update_explosion(ParticleData* data, float dt) {
		uint16_t i = 0;
		while (i < data->index) {
			float v = 500.0f * (1.0f - data->timers[i] / data->ttls[i]);
			data->positions[i] += get_radial(data->rotations[i],v) * dt;
			data->colors[i].b = (1.0f - data->timers[i] / data->ttls[i] * 0.8f);
			data->colors[i].a = (1.0f - data->timers[i] / data->ttls[i]);
			float s = data->additionals[i].x + static_cast<float>(sin(data->timers[i] / data->ttls[i] * TWO_PI)) * 0.3f;
			data->rotations[i] += data->additionals[i].y * 10.0f * dt;
			data->scales[i] = { s,s };
			++i;
		}
	}

	void pm_emitt_trail(ParticleData* data, const ds::vec2& pos,float* timer) {
		if (*timer >= 0.05f) {			
			uint16_t start = 0;
			uint16_t count = 0;
			int num = 16;
			float numf = static_cast<float>(num);
			if (pm_wake_up(data, num, &start, &count)) {
				for (uint16_t i = 0; i < count; ++i) {
					float rx = pos.x + cos(i / numf * TWO_PI) * random(1.0f, 10.0f);
					float ry = pos.y + sin(i / numf * TWO_PI) * random(1.0f, 10.0f);
					data->positions[start + i] = ds::vec2(rx, ry);
					data->additionals[start + i].x = random(0.2f, 0.3f);
					data->additionals[start + i].y = random(-1.0f, 1.0f);
					data->scales[start + i] = ds::vec2(data->additionals[start + i].x);
					data->rotations[start + i] = i / numf * TWO_PI;
					data->colors[start + i] = ds::Color(192, 0, 0, 255);
					data->timers[start + i] = 0.0f;
					data->ttls[start + i] = 1.5f;// random(1.5f, 2.5f);
				}
			}
			*timer -= 0.05f;
		}
	}

	void pm_update_trail(ParticleData* data, float dt) {
		uint16_t i = 0;
		while (i < data->index) {
			//data->colors[i].b = (1.0f - data->timers[i] / data->ttls[i] * 0.8f);
			data->colors[i].a = (1.0f - data->timers[i] / data->ttls[i]);
			float s = data->additionals[i].x + static_cast<float>(sin(data->timers[i] / data->ttls[i] * TWO_PI)) * 0.1f;
			data->scales[i] = { s,s };
			++i;
		}
	}

	static particle_manager INSTANCE = {
		0,
		pm_allocate,
		pm_reallocate,
		pm_tick,
		pm_emitt_explosion,
		pm_update_explosion,
		pm_emitt_trail,
		pm_update_trail
	};

	DLL_EXPORT void load_particle_manager(plugin_registry* registry) {
		init_random(0);
		registry->add(PARTICLE_MANAGER_NAME, &INSTANCE);
	}
#ifdef DEBUG
}
#endif

