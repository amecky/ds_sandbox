#pragma once
#include "..\TestApp.h"
#include "..\instancing\InstanceCommon.h"
#include "..\Mesh.h"
#include "..\kenney\Camera.h"
#include "..\utils\SimpleGrid.h"
#include "..\Material.h"
#include "..\utils\TransformComponent.h"
#include "..\utils\RenderItem.h"

class InstancedObjViewer : public TestApp {

	enum ObjectState {
		IDLE,MOVING,STEPPING,ROTATING,WALKING
	};
public:
	InstancedObjViewer();

	virtual ~InstancedObjViewer();

	ds::RenderSettings getRenderSettings();

	bool usesGUI() {
		return true;
	}

	bool init();

	void tick(float dt);

	void render();

	void renderGUI();

private:
	InstancedAmbientLightningMaterial* _material;
	instanced_render_item _renderItem;
	SimpleGrid* _grid;
	FPSCamera* _fpsCamera;
	ds::vec3 _lightDir[3];
	ID _ids[5];
	int _selected;
};