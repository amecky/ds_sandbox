#include "ParticlesTestScene.h"
#include <ds_imgui.h>
#include "..\utils\common_math.h"
#include "..\GameContext.h"
#include "..\gpuparticles\ParticleManager.h"
#include "..\gpuparticles\ParticleGUI.h"
#include <ds_profiler.h>
#include "..\PerfPanel.h"
#include <ds_logpanel.h>

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
	_dbgNumParticles = 2048;
	ringSettings.radius = 1.0f;

	ID descID = _gameContext->particleContext->descriptors.add();
	ParticleEmitterDescriptor& descriptor = _gameContext->particleContext->descriptors.get(descID);
	descriptor.radius = ds::vec2(0.15f);
	descriptor.alignParticle = 1;
	descriptor.rotationSpeed = ds::vec2(0.0f);
	descriptor.ttl = ds::vec2(0.5f, 0.6f);
	descriptor.startColor = ds::Color(255, 255, 255, 255);
	descriptor.endColor = ds::Color(255, 255, 0, 192);
	descriptor.scale = ds::vec2(0.35f);
	descriptor.scaleVariance = ds::vec2(0.02f);
	descriptor.growth = ds::vec2(0.0f, 0.8f);
	descriptor.angleVariance = ds::PI * 0.1f;
	descriptor.velocity = ds::vec2(0.2f,0.4f);


	particleEffect = particles::createEffect(_gameContext->particleContext, "Test");
	ID emitter = particles::createEmitter(_gameContext->particleContext,EmitterType::RING, &ringSettings, sizeof(RingEmitterSettings));
	particles::attachEmitter(_gameContext->particleContext,particleEffect, emitter, descID);
	ringSettings.radius = 0.4f;
	ID emitter2 = particles::createEmitter(_gameContext->particleContext, EmitterType::SPHERE, &ringSettings, sizeof(RingEmitterSettings));
	particles::attachEmitter(_gameContext->particleContext, particleEffect, emitter2, descID);

	_selectedEmitterDescriptor = descID;
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

	ds::ViewportInfo vpInfo = { 450, 40, 1280, 720, 0.0f, 1.0f };
	_gameViewPort = ds::createViewport(vpInfo);

	ds::RenderPassInfo rpInfo = { &_camera, _gameViewPort, ds::DepthBufferState::ENABLED, 0, 0 };
	_gameRenderPass = ds::createRenderPass(rpInfo);

	ds::RenderPassInfo noDepthInfo = { &_camera, _gameViewPort, ds::DepthBufferState::DISABLED, 0, 0 };
	_particleRenderPass = ds::createRenderPass(noDepthInfo);
	
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
	perf::ZoneTracker("Scene::update");
	//_isoCamera->update(dt);
	_fpsCamera->update(dt, _gameViewPort);
	
	Ray r = get_picking_ray(_camera.projectionMatrix, _camera.viewMatrix);
	r.setOrigin(_camera.position);
	_cursorPos = _grid->plane.getIntersection(r);

	particles::tick(_gameContext->particleContext, dt);
	
}

// ----------------------------------------------------
// render
// ----------------------------------------------------
void ParticlesTestScene::render() {
	perf::ZoneTracker("Scene::rener");
	// update lights
	for (int i = 0; i < 3; ++i) {
		_gameContext->instancedAmbientmaterial->setLightDirection(i, _lightDir[i]);
		_gameContext->ambientMaterial->setLightDirection(i, normalize(_lightDir[i]));
	}

	_gridItem->draw(_gameRenderPass, _camera.viewProjectionMatrix);

	//_cursorItem->draw(_basicPass, _camera.viewProjectionMatrix);

	particles::render(_gameContext->particleContext, _particleRenderPass, _camera.viewProjectionMatrix, _camera.position);
}

// ----------------------------------------------------
// renderGUI
// ----------------------------------------------------
void ParticlesTestScene::drawLeftPanel() {
	//gui::setAlphaLevel(0.5f);
	if (gui::begin("Debug",0)) {
		gui::Value("FPS", ds::getFramesPerSecond());
		gui::Value("Cursor", _cursorPos,"%2.3f %2.3f %2.3f");	
		gui::Value("Particles", _gameContext->particleContext->particleSystem->countAlive());
		particles::editParticleEmitterDescription(_gameContext->particleContext, _selectedEmitterDescriptor);
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
			particles::emitt(_gameContext->particleContext, particleEffect, ds::vec3(-0.5f, 1.1f, -0.5f), _dbgNumParticles);
		}
		//_gameContext->particles->showGUI(0);
		//perfpanel::show_profiler();
	}

	
}

