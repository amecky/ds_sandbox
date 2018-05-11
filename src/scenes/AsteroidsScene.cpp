#include "AsteroidsScene.h"
#include <ds_imgui.h>
#include "..\utils\common_math.h"
#include "..\GameContext.h"
#include "..\gpuparticles\ParticleManager.h"

void move_player(Player* player, float dt) {
	bool moved = false;
	ds::vec3 vel(0.0f);
	float v = 4.0f;
	if (ds::isKeyPressed('W')) {
		vel.y = v;
		moved = true;
	}
	if (ds::isKeyPressed('S')) {
		vel.y = -v;
		moved = true;
	}
	if (ds::isKeyPressed('A')) {
		vel.x = -v;
		moved = true;
	}
	if (ds::isKeyPressed('D')) {
		vel.x = v;
		moved = true;
	}
	player->renderItem->getTransform().rotation.x = 0.0f;
	if (moved) {
		player->position += vel * dt;
		player->renderItem->getTransform().position = player->position;
		if (vel.y > 0.0f) {
			player->renderItem->getTransform().rotation.x = DEGTORAD(330.0f);
		}
		else if (vel.y < 0.0f) {
			player->renderItem->getTransform().rotation.x = DEGTORAD(30.0f);
		}
	}
}

bool testBoxIntersection(const ds::vec3& p1, const ds::vec2& e1, const ds::vec3& p2, const ds::vec2& e2) {
	float top = p1.y - e1.y * 0.5f;
	float left = p1.x - e1.x * 0.5f;
	float right = p1.x + e1.x * 0.5f;
	float bottom = p1.y + e1.y * 0.5f;

	float otherTop = p2.y - e2.y * 0.5f;
	float otherLeft = p2.x - e2.x * 0.5f;
	float otherRight = p2.x + e2.x * 0.5f;
	float otherBottom = p2.y + e2.y * 0.5f;

	if (right < otherLeft || left > otherRight) return false;
	if (bottom < otherTop || top > otherBottom) return false;
	return true;
}

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
	delete _player.renderItem;
	delete _bulletBillboards;
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

	_player.renderItem = new RenderItem("player", _gameContext->ambientMaterial);

	_gameContext->towers.setWorldOffset(ds::vec2(-2.5f, -2.5f));
	
	_gameViewPort = ds::createViewport(ds::ViewportDesc()
		.Top(40)
		.Left(450)
		.Width(1280)
		.Height(720)
		.MinDepth(0.0f)
		.MaxDepth(1.0f)
	);

	_gameRenderPass = ds::createRenderPass(ds::RenderPassDesc()
		.Camera(&_camera)
		.Viewport(_gameViewPort)
		.DepthBufferState(ds::DepthBufferState::ENABLED));

	_particleRenderPass = ds::createRenderPass(ds::RenderPassDesc()
		.Camera(&_camera)
		.Viewport(_gameViewPort)
		.DepthBufferState(ds::DepthBufferState::DISABLED));

	_bulletBillboards = new Billboards;
	RID textureID = loadImageFromFile("content\\particles.png");
	_bulletBillboards->init(textureID);

	_shooting = false;
	_bulletTimer = 0.0f;

	_autoEmitt = true;
	_emittTimer = 0.0f;
}

// -------------------------------------------------------------
// add tower
// -------------------------------------------------------------
void AsteroidsScene::addEnemy() {
	if (_enemies.numObjects < 100) {
		ID id = _enemies.add();
		Enemy& e = _enemies.get(id);
		//float angle = ds::random(0.0f, ds::TWO_PI);

		e.instance_id = _enemiesItem->add();
		instance_item& item = _enemiesItem->get(e.instance_id);
		item.transform.position = ds::vec3(10.0f, ds::random(-4.0f, 4.0f), 0.0f);
		e.velocity = ds::vec3(-4.0f, 0.0f, 0.0f);
		e.timer = 0.0f;
		e.animationFlags = 7;
	}
}

void AsteroidsScene::addBullet() {
	ID id = _bullets.add();
	Bullet& b = _bullets.get(id);
	b.pos = _player.position;
	b.pos.x += 0.5f;
	b.velocity = ds::vec3(6.0f, 0.0, 0.0f);
}

// ----------------------------------------------------
// tick
// ----------------------------------------------------
void AsteroidsScene::update(float dt) {
	
	//_isoCamera->update(dt);
	//_topDownCamera->update(dt);

	//_cursorItem->getTransform().position = _camera.position;
	//_cursorItem->getTransform().position.z = 0.0f;
	/*
	_gameContext->towers.tick(dt, _events);

	Ray r = get_picking_ray(_camera.projectionMatrix, _camera.viewMatrix, _gameViewPort);
	r.setOrigin(_camera.position);
	ds::vec3 ip = _grid->plane.getIntersection(r);
	ip.y = 0.1f;
	_cursorItem->getTransform().position = ip;

	_gameContext->towers.rotateTowers(ip);
	*/

	if (ds::isMouseButtonPressed(0) && !_shooting) {
		_shooting = true;
	}
	if (!ds::isMouseButtonPressed(0) && _shooting) {
		_shooting = false;
	}

	if (_shooting) {
		_bulletTimer += dt;
		if (_bulletTimer > 0.2f) {
			addBullet();
			_bulletTimer -= 0.2f;
		}
	}

	if (_autoEmitt) {
		_emittTimer += dt;
		if (_emittTimer > 1.0f && _enemies.numObjects < 20) {
			_emittTimer = 0.0f;
			addEnemy();
		}
	}

	for (int i = 0; i < _bullets.numObjects; ++i) {
		_bullets.objects[i].pos += _bullets.objects[i].velocity * dt;
		if (_bullets.objects[i].pos.x > 12.0f) {
			_bullets.remove(_bullets.objects[i].id);
		}
	}

	move_player(&_player, dt);

	for (int i = 0; i < _bullets.numObjects; ++i) {
		for (int j = 0; j < _enemies.numObjects; ++j) {
			Enemy& e = _enemies.objects[j];
			instance_item& item = _enemiesItem->get(e.instance_id);
			if (testBoxIntersection(_bullets.objects[i].pos, ds::vec2(0.1f, 0.1f), item.transform.position, ds::vec2(0.5f))) {
				DBG_LOG("WE HAVE A HIT - animationFlag %d",e.animationFlags);
				_bullets.remove(_bullets.objects[i].id);
				if ((e.animationFlags & 4) != 4) {
					e.timer = 0.0f;
					e.animationFlags |= 1 << 2;
				}
			}
		}
	}

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
				e.animationFlags &= ~4;
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
	/*
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
	*/

	for (int i = 0; i < _enemies.numObjects; ++i) {
		Enemy& e = _enemies.objects[i];
		instance_item& item = _enemiesItem->get(e.instance_id);
		item.transform.position += e.force;
		if (item.transform.position.x < -12.0f) {
			//e.velocity.x *= -1.0f;
			item.transform.position.y = ds::random(-5.0f, 5.0f);
			item.transform.position.x = 11.5f;
		}
		//if (item.transform.position.y > 5.8f || item.transform.position.y < -5.8f) {
			//e.velocity.y *= -1.0f;
		//}
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

	_player.renderItem->draw(_gameRenderPass, _camera.viewProjectionMatrix);

	_bulletBillboards->begin();
	for (int i = 0; i < _bullets.numObjects; ++i) {
		_bulletBillboards->add(_bullets.objects[i].pos, ds::vec2(0.25f, 0.25f), ds::vec4(0, 0, 128, 128),0.0f,ds::vec2(1.0f),ds::Color(255,255,0,255));
	}
	_bulletBillboards->end();
	_bulletBillboards->render(_gameRenderPass, _camera.viewProjectionMatrix);

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
		gui::Value("Bullets", _bullets.numObjects);
		gui::Value("Shooting", _shooting);
		gui::Checkbox("Emitt enemies", &_autoEmitt);
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

