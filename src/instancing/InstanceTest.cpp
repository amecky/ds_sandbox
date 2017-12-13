#include "InstanceTest.h"
#include "..\Mesh.h"
#include "..\..\shaders\InstancedAmbient_VS_Main.h"
#include "..\..\shaders\InstancedAmbient_PS_Main.h"
#include <ds_imgui.h>
#include <ds_tweakable.h>

const int GRID_HEIGHT = 20;
const int GRID_WIDTH = 30;

float getAngle(const ds::vec2& v1, const ds::vec2& v2) {
	if (v1 != v2) {
		ds::vec2 vn1 = normalize(v1);
		ds::vec2 vn2 = normalize(v2);
		float dt = dot(vn1, vn2);
		if (dt < -1.0f) {
			dt = -1.0f;
		}
		if (dt > 1.0f) {
			dt = 1.0f;
		}
		float tmp = acos(dt);
		float cross = -1.0f * (vn2 - vn1).y;
		//float crs = cross(vn1, vn2);
		if (cross < 0.0f) {
			tmp = ds::TWO_PI - tmp;
		}
		return tmp;
	}
	else {
		return 0.0f;
	}
}

InstanceTest::InstanceTest() {
	_running = true;
	_showGUI = true;
	_pressed = false;
	_selectedTab = 0;
}

InstanceTest::~InstanceTest() {
	delete _queue;
	//delete _grid;
	delete _warpingGrid;
	delete _fpsCamera;
	delete _topDownCamera;
}

// ----------------------------------------------------
// get render settings
// ----------------------------------------------------
ds::RenderSettings InstanceTest::getRenderSettings() {
	ds::RenderSettings rs;
	rs.width = 1024;
	rs.height = 768;
	rs.title = "Instancing";
	rs.clearColor = ds::Color(0.01f, 0.01f, 0.01f, 1.0f);
	rs.multisampling = 4;
	rs.useGPUProfiling = false;
	rs.supportDebug = true;
	
	return rs;
}

void InstanceTest::registerParticleSettings(const char* category, ParticleSettings* settings) {
	twk_add(category, "num", &settings->num);
	twk_add(category, "radius",&settings->radius);
	twk_add(category, "angle", &settings->angle);
	twk_add(category, "rotation_speed", &settings->rotationSpeed);
	twk_add(category, "align_particle", &settings->alignParticle);
	twk_add(category, "ttl", &settings->ttl);
	twk_add(category, "radial_velocity", &settings->radialVelocity);
	twk_add(category, "radial_acceleration", &settings->radialAcceleration);
	twk_add(category, "scale", &settings->scale);
	twk_add(category, "scale_variance", &settings->scaleVariance);
	twk_add(category, "growth", &settings->growth);
}
// ----------------------------------------------------
// init
// ----------------------------------------------------
bool InstanceTest::init() {

	twk_add("bullets", "velocity", &_bulletSettings.velocity);
	twk_add("bullets", "scale", &_bulletSettings.scale);
	twk_add("bullets", "growth", &_bulletSettings.growth);
	twk_add("bullets", "bounding_box", &_bulletSettings.boundingBox);
	twk_add("bullets", "fire_rate", &_bulletSettings.fireRate);
	twk_add("bullets", "color", &_bulletSettings.color);

	registerParticleSettings("explosion", &_explosionSettings);

	ds::matrix viewMatrix = ds::matLookAtLH(ds::vec3(0.0f, 0.0f, -6.0f), ds::vec3(0, 0, 0), ds::vec3(0, 1, 0));
	ds::matrix projectionMatrix = ds::matPerspectiveFovLH(ds::PI / 4.0f, ds::getScreenAspectRatio(), 0.01f, 100.0f);
	_camera = {
		viewMatrix,
		projectionMatrix,
		viewMatrix * projectionMatrix,
		ds::vec3(0,0,-6),
		ds::vec3(0,0,1),
		ds::vec3(0,1,0),
		ds::vec3(1,0,0),
		0.0f,
		0.0f,
		0.0f
	};

	ds::ViewportInfo vpInfo = { 1024, 768, 0.0f, 1.0f };
	RID vp = ds::createViewport(vpInfo);

	ds::RenderPassInfo rpInfo = { &_camera, vp, ds::DepthBufferState::ENABLED, 0, 0 };
	_basicPass = ds::createRenderPass(rpInfo);

	ds::RenderPassInfo particleRPInfo = { &_camera, vp, ds::DepthBufferState::DISABLED, 0, 0 };
	_particlePass = ds::createRenderPass(particleRPInfo);
	
	RID textureID = loadImageFromFile("content\\TextureArray.png");

	_topDownCamera = new TopDownCamera(&_camera);
	_topDownCamera->setPosition(ds::vec3(0, 0, -5), ds::vec3(0.0f, 0.0f, 0.0f));

	_fpsCamera = new FPSCamera(&_camera);
	_fpsCamera->setPosition(ds::vec3(0, 0, -5), ds::vec3(0.0f, 0.0f, 0.0f));
	
	WarpingGridSettings wgs = {
		30,
		20,
		0.3f,
		0.05f,
		0.0f,
		ds::Color(0,0,50,255),
		0.28f,
		0.06f
	};

	_warpingGrid = new WarpingGrid(wgs);
	_warpingGrid->init();

	_queue = new EmitterQueue;

	

	_billboards.init(textureID);

	_tmpX = 10;
	_tmpY = 10;

	_tmpSide = 2;
	
	//_numBullets = 0;

	_bulletTimer = 0.0f;

	_useTopDown = true;

	_dbgGameMode = false;

	_gameModeTimer = 0.0f;
	_gameModeTTL = 2.0f;

	_dbgFixedOffset = -1;

	ParticlesystemDescriptor descriptor;
	descriptor.maxParticles = 16384;
	descriptor.particleDimension = ds::vec2(32, 32);
	descriptor.startColor = ds::Color(255, 255, 255, 255);
	descriptor.endColor = ds::Color(128, 128, 128, 0);

	float u1 = 220.0f / 1024.0f;
	float v1 = 30.0f / 1024.0f;
	float u2 = u1 + 20.0f / 1024.0f;
	float v2 = v1 + 20.0f / 1024.0f;
	descriptor.textureRect = ds::vec4(u1, v1, u2, v2);

	// load image using stb_image
	descriptor.texture = textureID;
	_particleSystem = new GPUParticlesystem(descriptor);

	_particleDescriptor.ttl = 0.4f;
	_particleDescriptor.velocity = ds::vec3(0.0f);
	_particleDescriptor.friction = 0.25f;
	_particleDescriptor.maxScale = ds::vec2(0.1f, 0.03f);
	_particleDescriptor.minScale = ds::vec2(0.2f, 0.03f);
	_particleDescriptor.acceleration = ds::vec3(0.0f, 0.0f, 0.0f);

	_material = new InstancedAmbientLightningMaterial;

	float lz = 0.2f;
	_lightDir[0] = ds::vec3(1.0f, 0.5f, lz);
	_lightDir[1] = ds::vec3(-1.0f, 0.5f, lz);
	_lightDir[2] = ds::vec3(0.0f, 0.5f, lz);

	_instanced_material = new InstancedMaterial;

	_ambient_material = new AmbientLightningMaterial;
	for (int i = 0; i < 3; ++i) {
		_ambient_material->setLightDirection(i, _lightDir[i]);
	}
	//
	// create render items
	//
	ds::matrix rxMatrix = ds::matRotationX(ds::PI * -0.5f);
	ds::matrix ryMatrix = ds::matRotationY(ds::PI * -0.5f);
	ds::matrix importMatrix = ryMatrix * rxMatrix;
	create_render_item(&_player.render_item, "ship", _ambient_material);// , &importMatrix);
	create_instanced_render_item(&_griddies, "octo", _instanced_material, 256);
	create_instanced_render_item(&_cube_item, "cube", _instanced_material, 256);
	create_instanced_render_item(&_doors_render_item, "border_box", _material, TOTAL_DOORS);

	_ringEnemies = new RingEnemies(&_griddies);
	//_ringEnemies->create(ds::vec3(2, 0, 0));

	_cubesSettings.startTTL = 0.15f;
	_cubes = new Cubes(&_cube_item, &_cubesSettings);
	_cubes->init();

	_doors = new Doors(&_doors_render_item);
	_doors->init();
	
	player::reset(&_player);
	_cameraPos = ds::vec3(0.0f, 0.0f, -6.0f);

	_dbgDoors = true;

	return true;
}

// ----------------------------------------------------
// tick
// ----------------------------------------------------
void InstanceTest::tick(float dt) {

	_events.reset();

	if (_running) {
		if (!_useTopDown) {
			_fpsCamera->update(dt);
		}
		else {
			_topDownCamera->update(dt);
		}

		manageBullets(dt);

		_cubes->tick(dt, _player.render_item.transform.position );

		//_grid->tick(dt);
		_warpingGrid->tick(dt);

		_queue->tick(dt, &_events);

		_cubes->checkCollisions(_bullets, &_events);

		//_numBullets = _ringEnemies->checkCollisions(_bullets, _numBullets, &_events);

		movePlayer(dt);

		_ringEnemies->tick(dt);

		handleEvents();

		_particleSystem->tick(dt);

		if (_dbgDoors) {
			_doors->tick(dt);
		}

		if (_dbgGameMode) {
			_gameModeTimer += dt;
			if (_gameModeTimer >= _gameModeTTL) {
				int num = _cubes->getNumItems();
				if ((24 - num) >= 8) {
					_gameModeTimer = 0.0f;
					int side = ds::random(1.0f, 4.9f);
					emittCubes(side, 8);
				}
			}
		}
	}
	else {
		if (!_useTopDown) {
			_fpsCamera->update(dt);
		}
		else {
			_topDownCamera->update(dt);
		}
	}

	if (ds::isKeyPressed('M') && !_pressed) {
		_showGUI = !_showGUI;
		_pressed = true;
	}
	if (!ds::isKeyPressed('M') && _pressed) {
		_pressed = false;
	}
}

// ----------------------------------------------------
// move player
// ----------------------------------------------------
void InstanceTest::movePlayer(float dt) {
	if (_useTopDown) {
		Ray r = get_picking_ray(_camera.projectionMatrix, _camera.viewMatrix);
		r.setOrigin(_camera.position);
		_cursorPos = _warpingGrid->getIntersectionPoint(r);		
		player::handle_keys(&_player, dt);
		player::move(&_player, _cursorPos, dt);

		ds::vec3 cameraPos = _player.render_item.transform.position;
		cameraPos.z = -6.0f;

		float velocity = 2.0f;
		ds::vec3 diff = cameraPos - _cameraPos;
		if (sqr_length(diff) > 0.1f) {
			ds::vec2 n = normalize(ds::vec2(diff.x, diff.y));
			ds::vec2 desired = n * velocity;
			_cameraPos += ds::vec3(desired) * dt;
			if (_cameraPos.x < -1.55f) {
				_cameraPos.x = -1.55f;
			}
			if (_cameraPos.x > 1.55f) {
				_cameraPos.x = 1.55f;
			}
			if (_cameraPos.y < -0.8f) {
				_cameraPos.y = -0.8f;
			}
			if (_cameraPos.y > 0.8f) {
				_cameraPos.y = 0.8f;
			}
			
		}
		_topDownCamera->setPosition(_cameraPos);
	}	
}

// ----------------------------------------------------
// handle events
// ----------------------------------------------------
void InstanceTest::handleEvents() {
	for (uint32_t i = 0; i < _events.num(); ++i) {
		int type = _events.getType(i);
		if (type == 100 || type == 101) {
			ds::vec3 p;
			_events.get(i, &p);
			_warpingGrid->applyForce(p, 0.4f, ds::vec3(0.0f, 0.0f, 0.15f));
			if (type == 101) {
				emittExplosion(p);
			}
		}
		else if (type == 102) {
			QueueEntry entry;
			_events.get(i, &entry);
			_doors->open(entry.side, entry.x, entry.y);
			ds::vec3 gp = _warpingGrid->convert_grid_coords(entry.x, entry.y);
			gp.x += 0.15f;
			gp.y -= 0.15f;
			_cubes->createCube(gp,entry.side);
		}
	}
}

// ----------------------------------------------------
// add bullet
// ----------------------------------------------------
void InstanceTest::addBullet() {
	ds::vec3 p = _player.render_item.transform.position;
	p.z += 0.01f;
	float r = _player.shootingAngle;
	//Bullet& b = _bullets[_numBullets++];
	Bullet b;
	b.pos = p;
	b.rotation = r;
	b.velocity = ds::vec3(cosf(r) * _bulletSettings.velocity, sinf(r) * _bulletSettings.velocity, 0.0f);
	b.timer = 0.0f;
	b.scale = ds::vec2(1.0f);
	_bullets.add(b);
}

// ----------------------------------------------------
// manage bullets
// ----------------------------------------------------
void InstanceTest::manageBullets(float dt) {
	// move bullets
	int cnt = 0;
	while (cnt < _bullets.size()) {
		Bullet& b = _bullets.get(cnt);
		b.pos += b.velocity * dt;
		b.timer += dt;
		b.scale.x = _bulletSettings.scale.x + b.timer * _bulletSettings.growth.x;
		b.scale.y = _bulletSettings.scale.y + b.timer * _bulletSettings.growth.y;
		ds::vec3 p = b.pos;
		if (p.y < _bulletSettings.boundingBox.y || p.y > _bulletSettings.boundingBox.w || p.x < _bulletSettings.boundingBox.x || p.x > _bulletSettings.boundingBox.z) {

			int ddy = (3.0f - p.y) / 6.0f * 20.0f;
			if (ddy < 0) {
				ddy = 0;
			}
			int ddx = 30.0f - (4.5f - p.x) / 9.0f * 30.0f;

			_doors->wiggle(ddx, ddy);

			_bullets.remove(cnt);
		}
		else {
			++cnt;
		}
	}
	// create bullet if necessary
	if (_player.shooting && _useTopDown) {
		float freq = 1.0f / _bulletSettings.fireRate;
		_bulletTimer += dt;
		if (_bulletTimer >= freq) {
			addBullet();
			_bulletTimer -= freq;
		}
	}
}

// ----------------------------------------------------
// render
// ----------------------------------------------------
void InstanceTest::render() {

	_warpingGrid->render(_basicPass, _camera.viewProjectionMatrix);
	if (_dbgDoors) {
		draw_instanced_render_item(&_doors_render_item, _basicPass, _camera.viewProjectionMatrix);
	}
	_particleSystem->render(_particlePass, _camera.viewProjectionMatrix, _camera.position);

	_billboards.begin();

	for (int i = 0; i < _bullets.size(); ++i) {
		const Bullet& b = _bullets.get(i);
		_billboards.add(b.pos, ds::vec2(0.075f, 0.075f), ds::vec4(233, 7, 22, 22), b.rotation, b.scale, _bulletSettings.color);
	}

	_billboards.add(_cursorPos, ds::vec2(0.2f, 0.2f), ds::vec4(550, 0, 64, 64));

	_billboards.render(_particlePass, _camera.viewProjectionMatrix);

	draw_render_item(&_player.render_item, _basicPass, _camera.viewProjectionMatrix);

	draw_instanced_render_item(&_cube_item, _basicPass, _camera.viewProjectionMatrix);

	draw_instanced_render_item(&_griddies, _basicPass, _camera.viewProjectionMatrix);
}

// ----------------------------------------------------
// emitt cubes
// ----------------------------------------------------
void InstanceTest::emittCubes(int side, int num) {
	int xd = 0;
	int yd = 0;
	int sx = 0;
	int sy = 0;
	if (side == BS_LEFT_SIDE) {
		sx = 2;
		sy = (GRID_HEIGHT - num) / 2;
		if (_dbgFixedOffset != -1) {
			sy = _dbgFixedOffset;
		}
		yd = 1;
	}
	else if (side == BS_TOP_SIDE) {
		sx = (GRID_WIDTH - num) / 2;
		if (_dbgFixedOffset != -1) {
			sx = _dbgFixedOffset;
		}
		sy = GRID_HEIGHT - 3;
		xd = 1;
	}
	else if (side == BS_RIGHT_SIDE) {
		sx = GRID_WIDTH - 2;
		sy = (GRID_HEIGHT - num) / 2;
		if (_dbgFixedOffset != -1) {
			sy = _dbgFixedOffset;
		}
		yd = 1;
	}
	else if (side == BS_BOTTOM_SIDE) {
		sx = (GRID_WIDTH - num) / 2;
		if (_dbgFixedOffset != -1) {
			sx = _dbgFixedOffset;
		}
		sy = 2;
		xd = 1;
	}
	for (int i = 0; i < num; ++i) {
		_queue->emitt(sx + xd * i, sy + yd * i, side, &_events);
	}
}

// ----------------------------------------------------
// renderGUI
// ----------------------------------------------------
void InstanceTest::renderGUI() {
	if (_showGUI) {
		int state = 1;
		gui::start();

		const char* TABS[] = { "Emitter","Bullets","Particles", "Cubes" };

		p2i sp = p2i(10, 760);
		if (gui::begin("Debug", &state, &sp, 340)) {

			gui::Tabs(TABS, 4, &_selectedTab);

			gui::Value("FPS", ds::getFramesPerSecond());
			if (gui::Checkbox("Top Down Camera", &_useTopDown)) {
				if (!_useTopDown) {
					_fpsCamera->setPosition(ds::vec3(0, 0, -5), ds::vec3(0.0f, 0.0f, 0.0f));
				}
				else {
					_topDownCamera->setPosition(ds::vec3(0, 0, -5), ds::vec3(0.0f, 0.0f, 0.0f));
				}
			}
			if (_selectedTab == 0) {
				gui::Input("GX", &_tmpX);
				gui::Input("GY", &_tmpY);
				gui::Value("Bullets", _bullets.size());
				gui::Checkbox("Doors", &_dbgDoors);
				if (gui::Button("Wiggle door")) {
					_doors->wiggle(10,0);
				}
				if (gui::Checkbox("Acitve Game mode", &_dbgGameMode)) {
					_gameModeTimer = 0.0f;
				}
				if (gui::Button("Emitt")) {
					_queue->emitt(_tmpX, _tmpY, 1, &_events);
				}
				if (gui::Button("Emitt ring")) {
					_ringEnemies->create(ds::vec3(0.0f));
				}
				if (gui::Button("Step")) {
					_cubes->stepForward();
				}
				if (gui::Button("Rotate")) {
					_cubes->rotateCubes();
				}
			}
			if (_selectedTab == 1) {
				gui::Value("Pos", _player.render_item.transform.position);
				gui::Value("Camera", _camera.position);
				ds::vec2 tmp = ds::vec2(_cursorPos.x, _cursorPos.y);
				gui::Value("Cursor", tmp);
				
				tweakableGUI("bullets");
			}
			if (gui::Button("Reset Camera")) {
				_fpsCamera->setPosition(ds::vec3(0, 0, -6), ds::vec3(0.0f, 0.0f, 0.0f));
				_player.render_item.transform.position = ds::vec3(0.0f);
			}
			if (_running) {
				if (gui::Button("Stop")) {
					_running = false;
				}
			}
			else {
				if (gui::Button("Start")) {
					_running = true;
				}
			}
			if (_selectedTab == 2) {
				gui::Value("Particles",_particleSystem->countAlive());
				tweakableGUI("explosion");
				if (gui::Button("Particles")) {
					//emittExplosion(ds::vec3(0.0f));
					emittParticles(ds::vec3(0.0f), _explosionSettings);
				}
			}
			if (_selectedTab == 3) {
				gui::Value("Cubes", _cubes->getNumItems());
				gui::Input("Start TTL", &_cubesSettings.startTTL);
				gui::Input("Side", &_tmpSide);
				gui::Input("Fixed offset",&_dbgFixedOffset);
				if (gui::Button("Emitt Line")) {
					emittCubes(_tmpSide, 8);
				}
				if (gui::Button("Emitt rnd Line")) {
					int side = ds::random(1.0f, 4.9f);
					emittCubes(side, 8);
				}
			}

		}
		gui::end();
	}
}

// ----------------------------------------------------
// GUI for tweable variables
// ----------------------------------------------------
void InstanceTest::tweakableGUI(const char* category) {
	Tweakable  vars[256];
	int num = twk_get_tweakables(category, vars, 256);
	for (int i = 0; i < num; ++i) {
		if (vars[i].type == TweakableType::ST_FLOAT) {
			gui::Input(vars[i].name, vars[i].ptr.fPtr);
		}
		else if (vars[i].type == TweakableType::ST_INT) {
			gui::Input(vars[i].name, vars[i].ptr.iPtr);
		}
		else if (vars[i].type == TweakableType::ST_VEC2) {
			gui::Input(vars[i].name, vars[i].ptr.v2Ptr);
		}
		else if (vars[i].type == TweakableType::ST_VEC3) {
			gui::Input(vars[i].name, vars[i].ptr.v3Ptr);
		}
		else if (vars[i].type == TweakableType::ST_VEC4) {
			gui::Input(vars[i].name, vars[i].ptr.v4Ptr);
		}
		else if (vars[i].type == TweakableType::ST_COLOR) {
			gui::Input(vars[i].name, vars[i].ptr.cPtr);
		}
	}
}

// ----------------------------------------------------
// emitt particles using settings
// ----------------------------------------------------
void InstanceTest::emittParticles(const ds::vec3& pos, const ParticleSettings& settings) {
	for (int i = 0; i < settings.num; ++i) {
		float r = ds::random(settings.radius.x, settings.radius.y);
		float angle = ds::random(0.0f, ds::TWO_PI);
		float x = cos(angle) * r + pos.x;
		float z = 0.1f;
		float y = sin(angle) * r + pos.y;
		if (settings.alignParticle == 1) {
			_particleDescriptor.rotation = angle;
		}
		_particleDescriptor.rotationSpeed = ds::random(settings.rotationSpeed.x, settings.rotationSpeed.y);
		_particleDescriptor.ttl = ds::random(settings.ttl.x,settings.ttl.y);
		float acc = ds::random(0.4f, 2.0f);
		_particleDescriptor.velocity = ds::vec3(cos(angle) * acc, sin(angle) * acc, 0.0f);
		//_particleDescriptor.acceleration = ds::vec3(cos(angle) * acc, sin(angle) * acc, 0.0f);
		_particleSystem->add(ds::vec3(x, y, z), _particleDescriptor);
	}
}

void InstanceTest::emittExplosion(const ds::vec3& pos) {
	for (int i = 0; i < 128; ++i) {
		float radius = 0.1f;
		float rmin = radius - radius * 0.8f;
		float r = ds::random(rmin, radius);
		float angle = ds::random(0.0f, ds::TWO_PI);
		float x = cos(angle) * r + pos.x;
		float z = 0.1f;
		float y = sin(angle) * r + pos.y;
		_particleDescriptor.rotation = angle;
		_particleDescriptor.rotationSpeed = 0.0f;// ds::random(-4.0f, 4.0f);
		_particleDescriptor.ttl = ds::random(0.5f, 1.2f);
		float acc = ds::random(0.4f, 2.0f);
		_particleDescriptor.velocity = ds::vec3(cos(angle) * acc, sin(angle) * acc, 0.0f);
		//_particleDescriptor.acceleration = ds::vec3(cos(angle) * acc, sin(angle) * acc, 0.0f);
		_particleSystem->add(ds::vec3(x, y, z), _particleDescriptor);
	}
}