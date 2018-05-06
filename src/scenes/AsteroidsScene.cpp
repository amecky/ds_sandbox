#include "AsteroidsScene.h"
#include <ds_imgui.h>
#include "..\utils\common_math.h"
#include "..\GameContext.h"
#include "..\gpuparticles\ParticleManager.h"

const ds::vec2 CENTER = ds::vec2(-11.5f, -6.5f);

AsteroidsScene::AsteroidsScene(GameContext* gameContext) : ds::BaseScene() , _gameContext(gameContext) {
	_dbgCameraRotation = ds::vec3(0.0f);

	_scalePath.add(0.0f, 0.2f);
	_scalePath.add(0.5f, 1.5f);
	_scalePath.add(0.75f, 0.6f);
	_scalePath.add(1.0f, 1.0f);
}

AsteroidsScene::~AsteroidsScene() {
	delete _topDownCamera;
	delete _cursorItem;
}

// ----------------------------------------------------
// init
// ----------------------------------------------------
void AsteroidsScene::initialize() {
	
	_camera.position = ds::vec3(0.0f, 0.0f, -16.0f);
	ds::rebuildCamera(&_camera);
	_topDownCamera = new TopDownCamera(&_camera);


	//_fpsCamera = new FPSCamera(&_camera);
	//_fpsCamera->setPosition(ds::vec3(0, 0, -16), ds::vec3(0.0f, 0.0f, 0.0f));

	_lightDir[0] = ds::vec3(0.0f,0.0f,1.0f);
	_lightDir[1] = ds::vec3(0.6f,-0.28f,0.34f);
	_lightDir[2] = ds::vec3(-0.3f,0.7f,-0.2f);


	_enemiesItem = new InstancedRenderItem("square_border", _gameContext->instancedAmbientmaterial, 100);
	
	addEnemy();

	_cursorItem = new RenderItem("player", _gameContext->ambientMaterial);

	_gameContext->towers.setWorldOffset(ds::vec2(-2.5f, -2.5f));
	
	ds::ViewportInfo vpInfo = { 400, 40, 1280, 720, 0.0f, 1.0f };
	_gameViewPort = ds::createViewport(vpInfo);

	ds::RenderPassInfo rpInfo = { &_camera, _gameViewPort, ds::DepthBufferState::ENABLED, 0, 0 };
	_gameRenderPass = ds::createRenderPass(rpInfo);

	ds::RenderPassInfo noDepthInfo = { &_camera, _gameViewPort, ds::DepthBufferState::DISABLED, 0, 0 };
	_particleRenderPass = ds::createRenderPass(noDepthInfo);

}

// -------------------------------------------------------------
// add tower
// -------------------------------------------------------------
void AsteroidsScene::addEnemy() {
	if (_enemies.numObjects < 100) {
		ID id = _enemies.add();
		Enemy& e = _enemies.get(id);
		float angle = ds::random(0.0f, ds::TWO_PI);

		e.instance_id = _enemiesItem->add();
		instance_item& item = _enemiesItem->get(e.instance_id);
		item.transform.position = ds::vec3(ds::random(-8.0f, 8.0f), ds::random(-4.0f, 4.0f), 0.0f);
		item.transform.rotation = ds::vec3(cos(angle), sin(angle), 0.0f);
		e.velocity = ds::vec3(cos(angle), sin(angle), 0.0f);
		e.timer = 0.0f;
		e.animationFlags = 7;
	}
}

// ----------------------------------------------------
// tick
// ----------------------------------------------------
void AsteroidsScene::update(float dt) {
	
	//_isoCamera->update(dt);
	_topDownCamera->update(dt);

	_cursorItem->getTransform().position = _camera.position;
	_cursorItem->getTransform().position.z = 0.0f;
	/*
	_gameContext->towers.tick(dt, _events);

	Ray r = get_picking_ray(_camera.projectionMatrix, _camera.viewMatrix, _gameViewPort);
	r.setOrigin(_camera.position);
	ds::vec3 ip = _grid->plane.getIntersection(r);
	ip.y = 0.1f;
	_cursorItem->getTransform().position = ip;

	_gameContext->towers.rotateTowers(ip);
	*/
	for (int i = 0; i < _enemies.numObjects; ++i) {
		Enemy& e = _enemies.objects[i];
		e.force = ds::vec3(0.0f);
	}

	for (int i = 0; i < _enemies.numObjects; ++i) {
		Enemy& e = _enemies.objects[i];
		instance_item& item = _enemiesItem->get(e.instance_id);
		if ((e.animationFlags & 4) == 4) {
			e.timer += dt;
			float norm = e.timer / 0.5f;
			if (norm >= 1.0f) {
				e.animationFlags &= ~(1 << 3);
				item.transform.scale = ds::vec3(1.0f);
			}
			else {
				float s = 0.0f;
				_scalePath.get(norm, &s);
				item.transform.scale = ds::vec3(s, s, 1.0f);
			}
		}
		if ((e.animationFlags & 2) == 2) {
			item.transform.rotation.x += dt;
		}
		// add velocity to force
		if ((e.animationFlags & 1) == 1) {
			e.force += e.velocity * dt;
		}
	}
	//
	// separate enemies
	//
	const float minDistance = 0.5f;
	const float relaxation = 2.0f;

	for (int i = 0; i < _enemies.numObjects; ++i) {
		Enemy& e = _enemies.objects[i];
		instance_item& item = _enemiesItem->get(e.instance_id);
		if ((e.animationFlags & 1) == 1) {
			ds::vec3 currentPos = item.transform.position;
			float sqrDist = minDistance * minDistance;
			for (int j = 0; j < _enemies.numObjects; ++j) {
				if (i != j) {
					Enemy& ne = _enemies.objects[j];
					instance_item& nextItem = _enemiesItem->get(ne.instance_id);
					ds::vec3 dist = nextItem.transform.position - currentPos;
					dist.z = 0.0f;
					if (sqr_length(dist) < sqrDist) {
						//DBG_LOG("hit between %d and %d", e.id, ne.id);
						ds::vec3 separationForce = dist;
						separationForce = normalize(separationForce);
						separationForce = separationForce * relaxation;
						separationForce.z = 0.0f;
						e.force -= separationForce * dt;
						e.velocity = -1.0f * normalize(dist);
						ne.velocity = normalize(dist);
					}
				}
			}
		}
	}

	for (int i = 0; i < _enemies.numObjects; ++i) {
		Enemy& e = _enemies.objects[i];
		instance_item& item = _enemiesItem->get(e.instance_id);
		item.transform.position += e.force;
		if (item.transform.position.x > 10.0f || item.transform.position.x < -10.0f) {
			e.velocity.x *= -1.0f;
		}
		if (item.transform.position.y > 5.8f || item.transform.position.y < -5.8f) {
			e.velocity.y *= -1.0f;
		}
	}

	particles::tick(_gameContext->particleContext, dt);
	
	for (int i = 0; i < _events->num(); ++i) {
		if (_events->getType(i) == 200) {
			FireEvent event;
			if (_events->get(i, &event, sizeof(FireEvent))) {
				//_gameContext->particles->emittLine(event.pos, ip);
				//ds::vec3 diff = normalize(ip - event.pos);
				//_gameContext->particles->emittParticles(event.pos, diff, 1);
			}
			else {
				DBG_LOG("received invalid event");
			}
		}
	}
	
}

// ----------------------------------------------------
// render
// ----------------------------------------------------
void AsteroidsScene::render() {
	// update lights
	for (int i = 0; i < 3; ++i) {
		_gameContext->instancedAmbientmaterial->setLightDirection(i, _lightDir[i]);
		_gameContext->ambientMaterial->setLightDirection(i, normalize(_lightDir[i]));
	}

	_cursorItem->draw(_gameRenderPass, _camera.viewProjectionMatrix);

	_enemiesItem->draw(_gameRenderPass, _camera.viewProjectionMatrix);

	particles::render(_gameContext->particleContext, _particleRenderPass, _camera.viewProjectionMatrix, _camera.position);
}

void AsteroidsScene::drawTopPanel() {
	if (gui::Input("Camera", &_camera.position)) {
		ds::rebuildCamera(&_camera);
	}
}

// ----------------------------------------------------
// renderGUI
// ----------------------------------------------------
void AsteroidsScene::drawLeftPanel() {
	int state = 1;
	if (gui::begin("Debug", &state)) {
		gui::Value("FPS", ds::getFramesPerSecond());
		gui::Value("Enemies", _enemies.numObjects);
		if (gui::Button("Add Enemy")) {
			addEnemy();
		}
		if (gui::Button("Rotate")) {
			for (int i = 0; i < _enemies.numObjects; ++i) {
				Enemy& e = _enemies.objects[i];
				instance_item& item = _enemiesItem->get(e.instance_id);
				item.transform.rotation = ds::vec3(0.0f);
				if ((e.animationFlags & 2) == 2) {
					e.animationFlags &= ~(1 << 1);
				}
				else {
					e.animationFlags |= 1 << 1;
				}
				DBG_LOG("anim flag %d", e.animationFlags);
			}
		}
		if (gui::Button("Scale")) {
			for (int i = 0; i < _enemies.numObjects; ++i) {
				Enemy& e = _enemies.objects[i];
				e.timer = 0.0f;
				e.animationFlags |= 1 << 2;
			}
		}
	}
}

