#include "ParticlesTestScene.h"
#include <ds_imgui.h>
#include "..\utils\common_math.h"
#include "..\GameContext.h"
#include "..\gpuparticles\ParticleManager.h"

const ds::vec2 CENTER = ds::vec2(-2.5f, -2.5f);

ParticlesTestScene::ParticlesTestScene(GameContext* gameContext) : ds::BaseScene() , _gameContext(gameContext) {
	_grid = new Grid(5, 5);
	for (int i = 0; i < 5; ++i) {
		for (int j = 0; j < 5; ++j) {
			_grid->set(i, j, 0);
		}
	}	
	_grid->plane = Plane(ds::vec3(0.0f, 0.0f, 0.0f), ds::vec3(0.0f, -1.0f, 0.0f));	
	
	_dbgCameraRotation = ds::vec3(0.0f);
	_dbgNumParticles = 32;
	ringSettings.radius = 1.0f;
	particleEffect = _gameContext->particles->createEffect();
	ID emitter = _gameContext->particles->createEmitter(EmitterType::RING, &ringSettings, sizeof(RingEmitterSettings));
	_gameContext->particles->attachEmitter(particleEffect, emitter, 0);
	ringSettings.radius = 0.4f;
	ID emitter2 = _gameContext->particles->createEmitter(EmitterType::SPHERE, &ringSettings, sizeof(RingEmitterSettings));
	_gameContext->particles->attachEmitter(particleEffect, emitter2, 0);
}

ParticlesTestScene::~ParticlesTestScene() {
	delete _grid;
	delete _isoCamera;
	delete _fpsCamera;
	delete _gridItem;
	//delete _cursorItem;
}

// ----------------------------------------------------
// init
// ----------------------------------------------------
void ParticlesTestScene::initialize() {
	
	_isoCamera = new IsometricCamera(&_camera);
	_isoCamera->setPosition(ds::vec3(0, 6, -6), ds::vec3(0.0f, 0.0f, 0.0f));

	_fpsCamera = new FPSCamera(&_camera);
	_fpsCamera->setPosition(ds::vec3(0, 6, -6), ds::vec3(0.0f, 0.0f, 0.0f));

	_lightDir[0] = ds::vec3(1.0f,-0.31f,1.0f);
	_lightDir[1] = ds::vec3(0.6f,-0.28f,0.34f);
	_lightDir[2] = ds::vec3(-0.3f,0.7f,-0.2f);

	_gridItem = new InstancedRenderItem("basic_floor", _gameContext->instancedAmbientmaterial, 5 * 5);

	for (int y = 0; y < _grid->height; ++y) {
		for (int x = 0; x < _grid->width; ++x) {			
			int type = _grid->get(x,y);
			int idx = x + y * _grid->width;
			if (type == 0) {
				ds::vec3 p = ds::vec3(CENTER.x + x, 0.0f, CENTER.y + y);
				ID id = _gridItem->add();
				instance_item& item = _gridItem->get(id);
				item.transform.position = p;

			}
		}
	}
	
	//_cursorItem = new RenderItem("cursor", _gameContext->ambientMaterial);

}



// ----------------------------------------------------
// tick
// ----------------------------------------------------
void ParticlesTestScene::update(float dt) {
	
	//_isoCamera->update(dt);
	_fpsCamera->update(dt);
	
	Ray r = get_picking_ray(_camera.projectionMatrix, _camera.viewMatrix);
	r.setOrigin(_camera.position);
	_cursorPos = _grid->plane.getIntersection(r);

	_gameContext->particles->tick(dt);
	
}

// ----------------------------------------------------
// render
// ----------------------------------------------------
void ParticlesTestScene::render() {
	// update lights
	for (int i = 0; i < 3; ++i) {
		_gameContext->instancedAmbientmaterial->setLightDirection(i, _lightDir[i]);
		_gameContext->ambientMaterial->setLightDirection(i, normalize(_lightDir[i]));
	}

	_gridItem->draw(_basicPass, _camera.viewProjectionMatrix);

	//_cursorItem->draw(_basicPass, _camera.viewProjectionMatrix);

	_gameContext->particles->render(_noDepthPass, _camera.viewProjectionMatrix, _camera.position);
}

// ----------------------------------------------------
// renderGUI
// ----------------------------------------------------
void ParticlesTestScene::showGUI() {
	int state = 1;
	p2i sp = p2i(10, 710);
	gui::start(&sp, 320);
	gui::setAlphaLevel(0.5f);
	if (gui::begin("Debug", &state)) {
		gui::Value("FPS", ds::getFramesPerSecond());
		gui::Value("Cursor", _cursorPos,"%2.3f %2.3f %2.3f");	
		/*
		bool changed = false;
		if (gui::SliderAngle("Roll", &_dbgCameraRotation.x)) {
			changed = true;
		}
		if (gui::SliderAngle("Pitch", &_dbgCameraRotation.y)) {
			changed = true;
		}
		if (gui::SliderAngle("Yaw", &_dbgCameraRotation.z)) {
			changed = true;
		}
		if (changed) {
			_isoCamera->setRotation(_dbgCameraRotation);
		}
		*/
		gui::Input("Num", &_dbgNumParticles);
		if (gui::Button("Particles")) {
			//_gameContext->particles->emittParticles(ds::vec3(-0.5f,1.1f,-0.5f), 0.0f, _dbgNumParticles, 0);
			_gameContext->particles->emittParticles(particleEffect, ds::vec3(-0.5f, 1.1f, -0.5f), _dbgNumParticles);
		}
		_gameContext->particles->showGUI(0);
	}
	gui::end();
}

