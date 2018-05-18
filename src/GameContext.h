#pragma once
#include "flow\FlowFieldContext.h"
#include "flow\Towers.h"
#include "gpuparticles\ParticleManager.h"
#include <ds_tweakable.h>

class AmbientLightningMaterial;
class InstancedAmbientLightningMaterial;

struct GameSettings {

	struct Bullets {
		float velocity;
	} bullets;

	GameSettings() {
		twk_add("bullets", "speed", &bullets.velocity);
	}
};

struct GameContext {
	Towers towers;
	AmbientLightningMaterial* ambientMaterial;
	InstancedAmbientLightningMaterial* instancedAmbientmaterial;
	ParticleSystemContext* particleContext;
	RID particlePass;
	RID gameViewPort;
	GameSettings* settings;
};

