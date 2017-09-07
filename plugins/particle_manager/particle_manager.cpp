#define DS_IMPLEMENTATION
//#include "ext\diesel.h"
#include "particle_manager.h"
#include <PluginRegistry.h>

extern "C" {

	static void pm_allocate(ParticleData* data, uint16_t max_particles) {
		uint16_t sz = max_particles * (sizeof(ds::vec2) + sizeof(ds::vec2) + sizeof(float) + sizeof(ds::Color) + sizeof(float) + sizeof(float));
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
		if (index > 0) {
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

	void pm_emitt_explosion(ParticleData* data) {

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
		registry->add(PARTICLE_MANAGER_NAME, &INSTANCE, sizeof(particle_manager));
	}

}


