#pragma once
#define PARTICLE_MANAGER_NAME "particle_manager"
#include <stdint.h>
#include "..\..\ext\ds_math.h"

extern "C" {

	struct ParticleData {

		ds::vec2* positions;
		ds::vec2* scales;
		float* rotations;
		ds::Color* colors;
		float* timers;
		float* ttls;

		char* buffer;
		uint16_t capacity;
		uint16_t index;
	};

	struct particle_manager {

		ParticleData* data;

		void(*allocate)(ParticleData* data, uint16_t max_particles);

		void(*reallocate)(ParticleData* data, uint16_t max_particles);

		void(*tick)(ParticleData* data, float dt);

		void(*emitt_explosion)(ParticleData* data);

		void(*update_explosion)(ParticleData* data, float dt);

	};

	struct plugin_registry;

	__declspec(dllexport) void load_particle_manager(plugin_registry* registry);

}
