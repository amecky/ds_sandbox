#include "TowerTestScene.h"
#include <ds_imgui.h>
#include "..\utils\common_math.h"
#include "..\GameContext.h"
#include "..\gpuparticles\ParticleManager.h"

const ds::vec2 CENTER = ds::vec2(-2.5f, -2.5f);

TowerTestScene::TowerTestScene(GameContext* gameContext) : ds::BaseScene() , _gameContext(gameContext) {
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
}

TowerTestScene::~TowerTestScene() {	
	delete _grid;
	delete _isoCamera;
	delete _fpsCamera;
	delete _gridItem;
	delete _cursorItem;
}

// ----------------------------------------------------
// init
// ----------------------------------------------------
void TowerTestScene::initialize() {
	
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
				_ids[idx] = _gridItem->add();
				instance_item& item = _gridItem->get(_ids[idx]);
				item.transform.position = p;

			}
		}
	}
	
	_cursorItem = new RenderItem("cursor", _gameContext->ambientMaterial);

	_gameContext->towers.setWorldOffset(ds::vec2(-2.5f, -2.5f));
	
	addTower(p2i(2, 2), 0);
	addTower(p2i(0, 2), 0);
	addTower(p2i(4, 2), 0);

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
void TowerTestScene::addTower(const p2i& gridPos, int type) {
	if (_grid->get(gridPos) == 0) {
		_grid->set(gridPos.x, gridPos.y, 1);
		_gameContext->towers.addTower(gridPos, type);
	}
}

// ----------------------------------------------------
// tick
// ----------------------------------------------------
void TowerTestScene::update(float dt) {
	
	//_isoCamera->update(dt);
	_fpsCamera->update(dt);
	
	_gameContext->towers.tick(dt, _events);

	Ray r = get_picking_ray(_camera.projectionMatrix, _camera.viewMatrix, _gameViewPort);
	r.setOrigin(_camera.position);
	ds::vec3 ip = _grid->plane.getIntersection(r);
	ip.y = 0.1f;
	_cursorItem->getTransform().position = ip;

	_gameContext->towers.rotateTowers(ip);

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
void TowerTestScene::render() {
	// update lights
	for (int i = 0; i < 3; ++i) {
		_gameContext->instancedAmbientmaterial->setLightDirection(i, _lightDir[i]);
		_gameContext->ambientMaterial->setLightDirection(i, normalize(_lightDir[i]));
	}

	_gridItem->draw(_gameRenderPass, _camera.viewProjectionMatrix);

	_cursorItem->draw(_gameRenderPass, _camera.viewProjectionMatrix);

	_gameContext->towers.render(_gameRenderPass, _camera.viewProjectionMatrix);

	particles::render(_gameContext->particleContext, _particleRenderPass, _camera.viewProjectionMatrix, _camera.position);
}

// ----------------------------------------------------
// renderGUI
// ----------------------------------------------------
void TowerTestScene::drawLeftPanel() {
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
		if (gui::Button("Particles")) {
			float dir = _gameContext->towers.getDirection(t.id);
			//_gameContext->particles->emittParticles(ds::vec3(t.position.x, 0.4f, t.position.z), dir, 8, 0);
		}
		//_gameContext->particles->showGUI(0);
	}
}

