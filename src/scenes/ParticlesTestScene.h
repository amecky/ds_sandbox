#pragma once
#include "..\instancing\InstanceCommon.h"
#include "..\Mesh.h"
#include "..\utils\Camera.h"
#include "..\flow\IsometricCamera.h"
#include "..\Material.h"
#include "..\utils\TransformComponent.h"
#include "..\utils\RenderItem.h"
#include "..\lib\DataArray.h"
#include "..\flow\Grid.h"
#include "..\flow\Towers.h"
#include <vector>
#include "..\flow\FlowFieldContext.h"
#include <ds_base_app.h>
#include "..\gpuparticles\ParticleEmitter.h"

struct GameContext;

class ParticlesTestScene : public ds::BaseScene {

public:
	ParticlesTestScene(GameContext* gameContext);
	virtual ~ParticlesTestScene();
	void initialize();
	void update(float dt);
	void render();
	void showGUI();
private:
	GameContext* _gameContext;
	ds::vec3 _cursorPos;
	IsometricCamera* _isoCamera;
	FPSCamera* _fpsCamera;
	ds::vec3 _lightDir[3];
	Grid* _grid;
	RID _particleRenderPass;
	InstancedRenderItem* _gridItem;
	ds::vec3 _dbgCameraRotation;
	int _dbgNumParticles;
	ID particleEffect;
	RingEmitterSettings ringSettings;
	ID _selectedEmitterDescriptor;
};