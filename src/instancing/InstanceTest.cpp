#include "InstanceTest.h"
#include "..\Mesh.h"
#include "..\..\shaders\InstancedAmbient_VS_Main.h"
#include "..\..\shaders\InstancedAmbient_PS_Main.h"
#include <ds_imgui.h>
#include <ds_tweakable.h>

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
	delete _player;
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
	twk_add("bullets", "bounding_box", &_bulletSettings.boundingBox);
	twk_add("bullets", "fire_rate", &_bulletSettings.fireRate);

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

	ds::BlendStateInfo blendInfo = { ds::BlendStates::SRC_ALPHA, ds::BlendStates::SRC_ALPHA, ds::BlendStates::INV_SRC_ALPHA, ds::BlendStates::INV_SRC_ALPHA, true };
	RID bs_id = ds::createBlendState(blendInfo);

	ds::ShaderInfo bumpVSInfo = { 0, InstancedAmbient_VS_Main, sizeof(InstancedAmbient_VS_Main), ds::ShaderType::ST_VERTEX_SHADER };
	RID bumpVS = ds::createShader(bumpVSInfo);
	ds::ShaderInfo bumpPSInfo = { 0, InstancedAmbient_PS_Main, sizeof(InstancedAmbient_PS_Main), ds::ShaderType::ST_PIXEL_SHADER };
	RID bumpPS = ds::createShader(bumpPSInfo);	

	ds::InputLayoutDefinition decl[] = {
		{ "POSITION", 0, ds::BufferAttributeType::FLOAT3 },
		{ "COLOR"   , 0, ds::BufferAttributeType::FLOAT4 },
		{ "NORMAL"  , 0, ds::BufferAttributeType::FLOAT3 }		
	};

	ds::InstancedInputLayoutDefinition instDecl[] = {
		{ "WORLD", 0, ds::BufferAttributeType::FLOAT4 },
		{ "WORLD", 1, ds::BufferAttributeType::FLOAT4 },
		{ "WORLD", 2, ds::BufferAttributeType::FLOAT4 },
		{ "WORLD", 3, ds::BufferAttributeType::FLOAT4 },
		{ "COLOR", 1, ds::BufferAttributeType::FLOAT4 }
	};
	ds::InstancedInputLayoutInfo iilInfo = { decl, 3, instDecl, 5, bumpVS };
	RID rid = ds::createInstancedInputLayout(iilInfo);

	RID textureID = loadImageFromFile("content\\TextureArray.png");

	//ds::VertexBufferInfo ibInfo = { ds::BufferType::DYNAMIC, TOTAL, sizeof(InstanceData) };
	//_instanceVertexBuffer = ds::createVertexBuffer(ibInfo);

	//RID instanceBuffer = ds::createInstancedBuffer(cubeBuffer, _instanceVertexBuffer);

	_topDownCamera = new TopDownCamera(&_camera);
	_topDownCamera->setPosition(ds::vec3(0, 0, -5), ds::vec3(0.0f, 0.0f, 0.0f));

	_fpsCamera = new FPSCamera(&_camera);
	_fpsCamera->setPosition(ds::vec3(0, 0, -5), ds::vec3(0.0f, 0.0f, 0.0f));

	RID baseGroup = ds::StateGroupBuilder()
		.inputLayout(rid)		
		.blendState(bs_id)
		.vertexShader(bumpVS)
		.pixelShader(bumpPS)
		.build();

	_gridSettings.borderColor = ds::Color(64,64,64,255);
	_gridSettings.gridColor = ds::Color(48,48,48,255);
	_gridSettings.flashColor = ds::Color(192,48,48,255);
	_gridSettings.flashTTL = 0.2f;
	_gridSettings.pulseTTL = 2.0f;
	_gridSettings.pulseAmplitude = 3.0f;

	//_grid = new BackgroundGrid(&_gridSettings);
	//_grid->init(baseGroup, bumpVS, bumpPS);

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

	_cubes.init(baseGroup, bumpVS, bumpPS);

	_border.init(baseGroup, bumpVS, bumpPS);
	_border.create(ds::vec3(0.0f));
	/*
	ds::vec2 gridExtent = _warpingGrid->getExtent();
	ds::vec3 borderExtent = _border.getItemExtent() * _border.getScale();
	ds::vec3 borderCenter = _border.getItemCenter();
	float borderSizeY = borderExtent.x * 0.5f;
	float halfBorderSizeY = borderSizeY * 0.5f;
	float sx = gridExtent.x * 0.5f + borderExtent.x;
	float sy = (gridExtent.y - borderExtent.y * 0.5f) * 0.5f;
	for (int i = 0; i < 18; ++i) {
		_border.create(ds::vec3(-sx, -sy + i * borderSizeY, 0.2f));
		_border.create(ds::vec3( sx, -sy + i * borderSizeY, 0.2f), ds::PI);
	}
	for (int i = 0; i < 17; ++i) {
		_border.create(ds::vec3(-4.2f + i * borderSizeY,  4.2f, 0.2f), ds::PI + ds::PI * 0.5f);
		_border.create(ds::vec3(-4.2f + i * borderSizeY, -4.2f, 0.2f), ds::PI * 0.5f);
	}
	*/
	_queue = new EmitterQueue;

	_player = new Player(_topDownCamera);
	_player->init();

	_billboards.init(textureID);

	_tmpX = 10;
	_tmpY = 10;

	_tmpSide = 2;
	
	_numBullets = 0;

	_shooting = false;

	_bulletTimer = 0.0f;

	_useTopDown = true;

	ParticlesystemDescriptor descriptor;
	descriptor.maxParticles = 16384;
	descriptor.particleDimension = ds::vec2(32, 32);
	descriptor.startColor = ds::Color(255, 255, 0, 255);
	descriptor.endColor = ds::Color(128, 128, 0, 0);

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

	_lightDir[0] = ds::vec3(1.0f, 0.5f, 1.0f);
	_lightDir[1] = ds::vec3(-1.0f, 0.5f, 1.0f);
	_lightDir[2] = ds::vec3(0.0f, 0.5f, 1.0f);

	create_instanced_render_item(&_griddies, "cube", _material, 256);

	_ringEnemies = new RingEnemies(&_griddies);
	_ringEnemies->create(ds::vec3(2, 0, 0));

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

		_cubes.tick(dt, _player->getPosition());

		//_grid->tick(dt);
		_warpingGrid->tick(dt);

		_queue->tick(dt, &_events);

		_numBullets = _cubes.checkCollisions(_bullets, _numBullets, &_events);

		_numBullets = _ringEnemies->checkCollisions(_bullets, _numBullets, &_events);

		movePlayer(dt);

		_ringEnemies->tick(dt);

		handleEvents();

		_particleSystem->tick(dt);
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

		if (ds::isMouseButtonPressed(0) && !_shooting) {
			_shooting = true;
		}
		if (!ds::isMouseButtonPressed(0) && _shooting) {
			_shooting = false;
		}

		_player->tick(dt);
	}
	Ray r = get_picking_ray(_camera.projectionMatrix, _camera.viewMatrix);
	r.setOrigin(_camera.position);
	// get cursor position
	_cursorPos = _warpingGrid->getIntersectionPoint(r);
	// rotate player
	ds::vec3 delta = _cursorPos - _player->getPosition();
	_player->setRotation(getAngle(ds::vec2(delta.x, delta.y), ds::vec2(1, 0)));
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
			_cubes.createCube(_warpingGrid->convert_grid_coords(entry.x, entry.y));
		}
	}
}

// ----------------------------------------------------
// add bullet
// ----------------------------------------------------
void InstanceTest::addBullet() {
	ds::vec3 p = _player->getPosition();
	p.z += 0.01f;
	float r = _player->getRotation();
	Bullet& b = _bullets[_numBullets++];
	b.pos = p;
	b.rotation = r;
	b.velocity = ds::vec3(cosf(r) * _bulletSettings.velocity, sinf(r) * _bulletSettings.velocity, 0.0f);
	b.timer = 0.0f;
	b.scale = ds::vec2(1.0f);
}

// ----------------------------------------------------
// manage bullets
// ----------------------------------------------------
void InstanceTest::manageBullets(float dt) {
	// move bullets
	int cnt = 0;
	while (cnt < _numBullets) {
		_bullets[cnt].pos += _bullets[cnt].velocity * dt;
		_bullets[cnt].timer += dt;
		//_bullets[cnt].scale.x = 1.0f + _bullets[cnt].timer * _bulletSettings.scale.x;
		//_bullets[cnt].scale.y = 1.0f + _bullets[cnt].timer * _bulletSettings.scale.y;
		ds::vec3 p = _bullets[cnt].pos;
		if (p.y < _bulletSettings.boundingBox.y || p.y > _bulletSettings.boundingBox.w || p.x < _bulletSettings.boundingBox.x || p.x > _bulletSettings.boundingBox.z) {
			if (_numBullets > 1) {
				_bullets[cnt] = _bullets[_numBullets - 1];
			}
			--_numBullets;
		}
		else {
			++cnt;
		}
	}
	// create bullet if necessary
	if (_shooting && _useTopDown) {
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

	_billboards.begin();
	
	_cubes.render(_basicPass, _camera.viewProjectionMatrix);

	_border.render(_basicPass, _camera.viewProjectionMatrix);
	
	//_grid->render(_basicPass, _camera.viewProjectionMatrix);

	_warpingGrid->render(_basicPass, _camera.viewProjectionMatrix);

	_particleSystem->render(_particlePass, _camera.viewProjectionMatrix, _camera.position);

	for (int i = 0; i < _numBullets; ++i) {
		_billboards.add(_bullets[i].pos, ds::vec2(0.075f, 0.075f), ds::vec4(233, 7, 22, 22), _bullets[i].rotation, _bullets[i].scale, ds::Color(255,128,0,255));
	}


	_billboards.add(_player->getPosition(), ds::vec2(0.3f, 0.3f), ds::vec4(350, 0, 50, 53), _player->getRotation());

	_billboards.add(_cursorPos, ds::vec2(0.2f, 0.2f), ds::vec4(550, 0, 64, 64));

	_billboards.render(_basicPass, _camera.viewProjectionMatrix);

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
	if (side == 1) {
		sx = 2;
		sy = (GRID_HEIGHT - num) / 2;
		yd = 1;
	}
	else if (side == 2) {
		sx = (GRID_WIDTH - num) / 2;
		sy = GRID_HEIGHT - 3;
		xd = 1;
	}
	else if (side == 3) {
		sx = GRID_WIDTH - 2;
		sy = (GRID_HEIGHT - num) / 2;
		yd = 1;
	}
	else if (side == 4) {
		sx = (GRID_WIDTH - num) / 2;
		sy = 2;
		xd = 1;
	}
	for (int i = 0; i < num; ++i) {
		_queue->emitt(sx + xd * i, sy + yd * i, &_events);
		//_warpingGrid->highlight(sx + xd * i, sy + yd * i);
	}
}

// ----------------------------------------------------
// renderGUI
// ----------------------------------------------------
void InstanceTest::renderGUI() {
	if (_showGUI) {
		int state = 1;
		gui::start();

		const char* TABS[] = { "Emitter","Bullets","Particles" };

		p2i sp = p2i(10, 760);
		if (gui::begin("Debug", &state, &sp, 300)) {

			gui::Tabs(TABS, 3, &_selectedTab);

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
				gui::Value("Bullets", _numBullets);
				/*
				if (gui::Button("Flash ONE")) {
					_grid->highlight(_tmpX, _tmpY, BGF_ONE);
				}
				if (gui::Button("Flash PULSE")) {
					_grid->highlight(_tmpX, _tmpY, BGF_PULSE);
				}
				*/
				if (gui::Button("Emitt")) {
					_queue->emitt(_tmpX, _tmpY, &_events);
				}
				if (gui::Button("Emitt ring")) {
					_ringEnemies->create(ds::vec3(0.0f));
				}
				gui::Input("Side", &_tmpSide);
				if (gui::Button("Emitt Line")) {
					emittCubes(_tmpSide, 8);
				}
				if (gui::Button("Emitt rnd Line")) {
					int side = ds::random(0.0f, 3.9f);
					emittCubes(side, 8);
				}
				if (gui::Button("Step")) {
					_cubes.stepForward();
				}
				if (gui::Button("Rotate")) {
					_cubes.rotateCubes();
				}
			}
			if (_selectedTab == 1) {
				gui::Value("Pos", _player->getPosition());
				gui::Value("Camera", _camera.position);
				ds::vec2 tmp = ds::vec2(_cursorPos.x, _cursorPos.y);
				gui::Value("Cursor", tmp);
				gui::Value("Cubes", _cubes.getNumItems());
				tweakableGUI("bullets");
			}
			if (gui::Button("Reset Camera")) {
				_fpsCamera->setPosition(ds::vec3(0, 0, -6), ds::vec3(0.0f, 0.0f, 0.0f));
				_player->setPosition(ds::vec3(0.0f));
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