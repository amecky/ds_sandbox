#pragma once
#include "flow\FlowFieldContext.h"
#include "flow\Towers.h"
#include "gpuparticles\ParticleManager.h"

class AmbientLightningMaterial;
class InstancedAmbientLightningMaterial;

struct GameContext {
	Towers towers;
	AmbientLightningMaterial* ambientMaterial;
	InstancedAmbientLightningMaterial* instancedAmbientmaterial;
	ParticleSystemContext* particleContext;
	RID particlePass;
	RID gameViewPort;
};
