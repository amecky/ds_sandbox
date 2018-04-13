#include "ParticleGUI.h"
#include "ParticleManager.h"
#include "ParticleEmitter.h"
#include <ds_imgui.h>

namespace particles {

	void editParticleEmitterDescription(ParticleSystemContext* context, ID id) {
		if (id != INVALID_ID) {
			ParticleEmitterDescriptor& emitterDescriptor = context->descriptors.get(id);
			gui::Input("Radius min/max", &emitterDescriptor.radius);
			gui::Input("Vel min/max", &emitterDescriptor.velocity);
			gui::Input("align", &emitterDescriptor.alignParticle);
			gui::Input("Angle_Var", &emitterDescriptor.angleVariance);
			gui::Input("Rot-Speed", &emitterDescriptor.rotationSpeed);
			gui::Input("TTL min/max", &emitterDescriptor.ttl);
			gui::Input("Start", &emitterDescriptor.startColor);
			gui::Input("End", &emitterDescriptor.endColor);
			gui::Input("Scale", &emitterDescriptor.scale);
			gui::Input("Scale_Var", &emitterDescriptor.scaleVariance);
			gui::Input("Growth", &emitterDescriptor.growth);
		}
	}
}