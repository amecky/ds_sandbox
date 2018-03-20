#pragma once
#include "flow\FlowFieldContext.h"
#include "flow\Towers.h"
#include "flow\ParticleManager.h"

class AmbientLightningMaterial;
class InstancedAmbientLightningMaterial;

struct GameContext {

	Towers towers;
	AmbientLightningMaterial* ambientMaterial;
	InstancedAmbientLightningMaterial* instancedAmbientmaterial;
	ParticleManager* particles;
	RID particlePass;
	ParticleEmitterDescriptor emitterDescriptors[32];
};
