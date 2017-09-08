//#define DS_IMPLEMENTATION
//#include "ext\diesel.h"
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

extern "C" {

	static void pm_allocate(ParticleData* data, uint16_t max_particles) {
		//uint16_t sz = max_particles * (sizeof(ds::vec2) + sizeof(ds::vec2) + sizeof(float) + sizeof(ds::Color) + sizeof(float) + sizeof(float));
		uint32_t sz = max_particles * (11 * sizeof(float));
		data->buffer = new char[sz];
		data->capacity = max_particles;
		data->positions = (ds::vec2*)data->buffer;
		data->scales = (ds::vec2*)(data->positions + max_particles);
		data->rotations = (float*)(data->scales + max_particles);
		data->colors = (ds::Color*)(data->rotations + max_particles);
		data->timers = (float*)(data->colors + max_particles);
		data->ttls = (float*)(data->timers + max_particles);
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

	void pm_emitt_explosion(ParticleData* data) {
		uint16_t start = 0;
		uint16_t count = 0;
		if (pm_wake_up(data, 32, &start, &count)) {
			for (uint16_t i = 0; i < count; ++i) {
				float rx = 512.0f + cos(i / 32.0f * TWO_PI) * random(120.0f,150.0f);
				float ry = 384.0f + sin(i / 32.0f * TWO_PI) * random(120.0f, 150.0f);
				data->positions[start + i] = ds::vec2(rx,ry);
				data->scales[start + i] = ds::vec2(0.2f,0.4f);
				data->rotations[start + i] = i / 32.0f * TWO_PI;
				data->colors[start + i] = ds::Color(1.0f, 0.0f, 0.0f, 1.0f);
				data->timers[start + i] = 0.0f;
				data->ttls[start + i] = 1.0f;
			}
		}
	}

	void pm_update_explosion(ParticleData* data, float dt) {

	}

	static particle_manager INSTANCE = {
		0,
		pm_allocate,
		pm_reallocate,
		pm_tick,
		pm_emitt_explosion,
		pm_update_explosion
	};

	struct plugin_registry;

	__declspec(dllexport) void load_particle_manager(plugin_registry* registry) {
		init_random(0);
		registry->add(PARTICLE_MANAGER_NAME, &INSTANCE, sizeof(particle_manager));
	}

}


