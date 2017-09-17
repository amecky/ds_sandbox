#pragma once
#define PARTICLE_MANAGER_NAME "particle_manager"
#include <stdint.h>
#include "..\..\ext\ds_math.h"

#ifdef DEBUG
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#ifdef DEBUG
extern "C" {
#endif

	struct ParticleData {

		ds::vec2* positions;
		ds::vec2* scales;
		float* rotations;
		ds::Color* colors;
		float* timers;
		float* ttls;
		ds::vec4* additionals;
		char* buffer;
		uint16_t capacity;
		uint16_t index;
		
	};

	struct particle_manager {

		ParticleData* data;

		void(*allocate)(ParticleData* data, uint16_t max_particles);

		void(*reallocate)(ParticleData* data, uint16_t max_particles);

		void(*tick)(ParticleData* data, float dt);

		void(*emitt_explosion)(ParticleData* data, const ds::vec2& pos);

		void(*update_explosion)(ParticleData* data, float dt);

		void(*emitt_trail)(ParticleData* data, const ds::vec2& pos, float* timer);

		void(*update_trail)(ParticleData* data, float dt);

	};

	struct plugin_registry;

	DLL_EXPORT void load_particle_manager(plugin_registry* registry);
#ifdef DEBUG
}
#endif
