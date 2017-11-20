#include "InstanceTest.h"
#include "..\Mesh.h"
#include "..\..\shaders\InstancedAmbient_VS_Main.h"
#include "..\..\shaders\InstancedAmbient_PS_Main.h"
#include <ds_imgui.h>

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
}

InstanceTest::~InstanceTest() {
	delete _queue;
	delete _grid;
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

// ----------------------------------------------------
// init
// ----------------------------------------------------
bool InstanceTest::init() {
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

	_grid = new BackgroundGrid(&_gridSettings);

	_grid->init(baseGroup, bumpVS, bumpPS);

	_cubes.init(baseGroup, bumpVS, bumpPS);

	_queue = new EmitterQueue(_grid, &_cubes);

	_player = new Player(_topDownCamera);
	_player->init();

	_billboards.init(textureID);

	_tmpX = 10;
	_tmpY = 10;

	_tmpSide = 2;
	
	_cameraMode = 1;

	_numBullets = 0;

	_shooting = false;

	_bulletTimer = 0.0f;

	return true;
}

void InstanceTest::addBullet() {
	ds::vec3 p = _player->getPosition();
	p.z += 0.01f;
	float r = _player->getRotation();
	Bullet& b = _bullets[_numBullets++];
	b.pos = p;
	b.rotation = r;
	b.velocity = ds::vec3(cosf(r) * 4.0f, sinf(r) * 4.0f, 0.0f);
	b.timer = 0.0f;
	b.scale = ds::vec2(1.0f);
}
// ----------------------------------------------------
// tick
// ----------------------------------------------------
void InstanceTest::tick(float dt) {
	if (_cameraMode == 1) {
		_fpsCamera->update(dt);
	}
	else if (_cameraMode == 2) {
		_topDownCamera->update(dt);
	}
	else if (_cameraMode == 3) {
		_player->tick(dt);
	}

	int cnt = 0;
	while ( cnt < _numBullets) {
		_bullets[cnt].pos += _bullets[cnt].velocity * dt;
		_bullets[cnt].timer += dt;
		_bullets[cnt].scale.x = 1.0f + _bullets[cnt].timer * 4.0f;
		ds::vec3 p = _bullets[cnt].pos;
		if (p.y < -2.1f || p.y > 1.9f || p.x < -3.2f || p.x > 3.2f) {
			if (_numBullets > 1) {
				_bullets[cnt] = _bullets[_numBullets - 1];
			}
			--_numBullets;
		}
		else {
			++cnt;
		}
	}

	_cubes.tick(dt, _player->getPosition());

	_grid->tick(dt);

	_queue->tick(dt);

	if (ds::isMouseButtonPressed(0) && !_shooting ) {
		_shooting = true;
	}
	if (!ds::isMouseButtonPressed(0) && _shooting) {
		_shooting = false;
	}

	if (_shooting && _cameraMode == 3) {
		_bulletTimer += dt;
		if (_bulletTimer >= 0.1f) {
			addBullet();
			_bulletTimer -= 0.1f;
		}
	}

	_numBullets = _cubes.checkCollisions(_bullets, _numBullets);

	Ray r = get_picking_ray(_camera.projectionMatrix, _camera.viewMatrix);
	r.setOrigin(_camera.position);
	//ds::vec3 n = ds::vec3(0.0f, 0.0f, -1.0f);
	//float denominator = dot(n, r.direction);
	//float numerator = dot(n, r.origin) - 0.1;
	//float t = -(numerator / denominator);
	//_cursorPos = r.origin + r.direction * t;
	_cursorPos = _grid->getIntersectionPoint(r);

	ds::vec3 delta = _cursorPos - _player->getPosition();

	_player->setRotation(getAngle(ds::vec2(delta.x, delta.y), ds::vec2(1, 0)));
}

// ----------------------------------------------------
// render
// ----------------------------------------------------
void InstanceTest::render() {

	_billboards.begin();
	
	_cubes.render(_basicPass, _camera.viewProjectionMatrix);
	
	_grid->render(_basicPass, _camera.viewProjectionMatrix);

	/*
	for (int j = 0; j < 5; ++j) {
		for (int i = 0; i < 12; ++i) {
			_billboards.add(ds::vec3(-2.0f + i * 0.3f, 1.0f - j * 0.3f, 0.0f), ds::vec2(0.2f, 0.2f), ds::vec4(0, 700, 256, 256));
		}
	}
	*/

	


	for (int i = 0; i < _numBullets; ++i) {
		_billboards.add(_bullets[i].pos, ds::vec2(0.075f, 0.075f), ds::vec4(233, 7, 22, 22), _bullets[i].rotation, _bullets[i].scale, ds::Color(255,128,0,255));
	}

	_billboards.add(_cursorPos, ds::vec2(0.2f, 0.2f), ds::vec4(550, 0, 64, 64));

	_billboards.add(_player->getPosition(), ds::vec2(0.3f, 0.3f), ds::vec4(350, 0, 50, 53), _player->getRotation());

	//_billboards.add(_player->getPosition(), ds::vec2(1.2f, 1.2f), ds::vec4(770, 100, 150, 150), _player->getRotation());

	_billboards.render(_basicPass, _camera.viewProjectionMatrix);



	ds::dbgPrint(0, 0, "FPS: %d", ds::getFramesPerSecond());
}

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
		_queue->emitt(sx + xd * i, sy + yd * i);
	}
}

void InstanceTest::renderGUI() {
	int state = 1;
	gui::start();
	p2i sp = p2i(10, 760);
	if (gui::begin("Debug", &state, &sp, 300)) {
		gui::Value("FPS", ds::getFramesPerSecond());
		gui::Input("Camera Mode", &_cameraMode);
		gui::Input("GX", &_tmpX);
		gui::Input("GY", &_tmpY);
		gui::Value("Bullets", _numBullets);
		if (gui::Button("Flash ONE")) {
			_grid->highlight(_tmpX, _tmpY, BGF_ONE);
		}
		if (gui::Button("Flash PULSE")) {
			_grid->highlight(_tmpX, _tmpY, BGF_PULSE);
		}
		if (gui::Button("Emitt")) {
			_queue->emitt(_tmpX, _tmpY);
		}
		if (gui::Button("Emitt Line X")) {
			for (int i = 0; i < 8; ++i) {
				_queue->emitt(_tmpX + i, _tmpY);
			}
		}
		gui::Input("Side", &_tmpSide);
		if (gui::Button("Emitt Line")) {
			emittCubes(_tmpSide, 8);
		}
		gui::Value("Pos", _player->getPosition());
		gui::Value("Camera", _camera.position);
		ds::vec2 tmp = ds::vec2(_cursorPos.x, _cursorPos.y);
		gui::Value("Cursor", tmp);
		gui::Value("Cubes", _cubes.getNumItems());
		if (gui::Button("Reset Camera")) {
			_fpsCamera->setPosition(ds::vec3(0, 0, -6), ds::vec3(0.0f, 0.0f, 0.0f));
			_player->setPosition(ds::vec3(0.0f));
		}
	}
	gui::end();
}