#include "AsteroidsScene.h"
#include <ds_imgui.h>
#include "..\utils\common_math.h"
#include "..\GameContext.h"
#include "..\gpuparticles\ParticleManager.h"

const ds::vec2 CENTER = ds::vec2(-2.5f, -2.5f);

AsteroidsScene::AsteroidsScene(GameContext* gameContext) : ds::BaseScene() , _gameContext(gameContext) {
	_grid = new Grid(5, 5);
	for (int i = 0; i < 5; ++i) {
		for (int j = 0; j < 5; ++j) {
			_grid->set(i, j, 0);
		}
	}	
	_grid->plane = Plane(ds::vec3(0.0f, 0.0f, 0.0f), ds::vec3(0.0f, -1.0f, 0.0f));	
	_selectedTower = 0;
	_dbgCameraRotation = ds::vec3(0.0f);

	//ds::vec3 start(0.0f, 1.0f, 0.0f);
	//ds::vec3 end(2.0f, 0.0f, 0.0f);

	ds::vec3 start(0.50f, 0.7f, -0.68f);
	ds::vec3 end(0.49f, 0.10f, -1.62f);

	ds::vec3 diff = end - start;
	float l = length(diff);
	ds::vec3 center = (start + end) * 0.5f;
	ds::vec3 dir = normalize(diff);
	float angle = math::get_rotation(ds::vec2(diff.x, diff.z)) + ds::PI;

	_scalePath.add(0.0f, 0.2f);
	_scalePath.add(0.5f, 1.5f);
	_scalePath.add(0.75f, 0.6f);
	_scalePath.add(1.0f, 1.0f);
}

AsteroidsScene::~AsteroidsScene() {
	delete _grid;
	delete _isoCamera;
	delete _fpsCamera;
	delete _gridItem;
	delete _cursorItem;
}

// ----------------------------------------------------
// init
// ----------------------------------------------------
void AsteroidsScene::initialize() {
	
	_isoCamera = new IsometricCamera(&_camera);
	_isoCamera->setPosition(ds::vec3(0, 6, -6), ds::vec3(0.0f, 0.0f, 0.0f));

	_fpsCamera = new FPSCamera(&_camera);
	_fpsCamera->setPosition(ds::vec3(0, 0, -16), ds::vec3(0.0f, 0.0f, 0.0f));

	_lightDir[0] = ds::vec3(1.0f,-0.31f,1.0f);
	_lightDir[1] = ds::vec3(0.6f,-0.28f,0.34f);
	_lightDir[2] = ds::vec3(-0.3f,0.7f,-0.2f);

	_gridItem = new InstancedRenderItem("basic_floor", _gameContext->instancedAmbientmaterial, 5 * 5);

	_enemiesItem = new InstancedRenderItem("square_border", _gameContext->instancedAmbientmaterial, 100);

	for (int y = 0; y < _grid->height; ++y) {
		for (int x = 0; x < _grid->width; ++x) {			
			int type = _grid->get(x,y);
			int idx = x + y * _grid->width;
			if (type == 0) {
				ds::vec3 p = ds::vec3(CENTER.x + x, CENTER.y + y, 0.0f);
				_ids[idx] = _gridItem->add();
				instance_item& item = _gridItem->get(_ids[idx]);
				item.transform.position = p;

			}
		}
	}

	addEnemy();

	_cursorItem = new RenderItem("cursor", _gameContext->ambientMaterial);

	_gameContext->towers.setWorldOffset(ds::vec2(-2.5f, -2.5f));
	
	ds::ViewportInfo vpInfo = { 450, 40, 1280, 720, 0.0f, 1.0f };
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
	ID id = _enemies.add();
	Enemy& e = _enemies.get(id);
	float angle = ds::random(0.0f, ds::TWO_PI);

	e.instance_id = _enemiesItem->add();
	instance_item& item = _enemiesItem->get(e.instance_id);
	item.transform.position = ds::vec3(0.0f, 0.0f, 0.0f);
	item.transform.rotation = ds::vec3(cos(angle), sin(angle), 0.0f);
	e.velocity = ds::vec3(cos(angle), sin(angle), 0.0f);
	e.timer = 0.0f;
	e.animationFlags = 7;
}

// ----------------------------------------------------
// tick
// ----------------------------------------------------
void AsteroidsScene::update(float dt) {
	
	//_isoCamera->update(dt);
	_fpsCamera->update(dt);
	
	_gameContext->towers.tick(dt, _events);

	Ray r = get_picking_ray(_camera.projectionMatrix, _camera.viewMatrix, _gameViewPort);
	r.setOrigin(_camera.position);
	ds::vec3 ip = _grid->plane.getIntersection(r);
	ip.y = 0.1f;
	_cursorItem->getTransform().position = ip;

	_gameContext->towers.rotateTowers(ip);

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
	const float minDistance = 0.25f;
	const float relaxation = 1.0f;

	for (int i = 0; i < _enemies.numObjects; ++i) {
		Enemy& e = _enemies.objects[i];
		instance_item& item = _enemiesItem->get(e.instance_id);
		if ((e.animationFlags & 1) == 1) {
			ds::vec3 currentPos = item.transform.position;
			float sqrDist = minDistance * minDistance;
			for (int j = 0; j < _enemies.numObjects; ++j) {
				if ( i != j ) {		
					Enemy& ne = _enemies.objects[j];
					instance_item& nextItem = _enemiesItem->get(ne.instance_id);
					ds::vec3 dist = nextItem.transform.position - currentPos;
					if (sqr_length(dist) < sqrDist) {
						ds::vec3 separationForce = dist;
						separationForce = normalize(separationForce);
						separationForce = separationForce * relaxation;
						separationForce.z = 0.0f;
						e.force -= separationForce * dt;
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
	}

	particles::tick(_gameContext->particleContext, dt);
	
	for (int i = 0; i < _events->num(); ++i) {
		if (_events->getType(i) == 200) {
			FireEvent event;
			if (_events->get(i, &event, sizeof(FireEvent))) {
				//_gameContext->particles->emittLine(event.pos, ip);
				ds::vec3 diff = normalize(ip - event.pos);
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

	_gridItem->draw(_gameRenderPass, _camera.viewProjectionMatrix);

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
		if (gui::StepInput("Tower", &_selectedTower, 0, 4, 1)) {
			// change tower type
			_gameContext->towers.remove(p2i(2, 2));
			_gameContext->towers.addTower(p2i(2, 2), _selectedTower);
		}
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
		_gameContext->towers.showGUI(0);
		const Tower& t = _gameContext->towers.get(0);
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
		//_gameContext->particles->showGUI(0);
	}
}

