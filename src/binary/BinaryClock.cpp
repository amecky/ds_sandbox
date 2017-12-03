#include "BinaryClock.h"
#include "..\..\shaders\Bump_VS_Main.h"
#include "..\..\shaders\Bump_PS_Main.h"
#include <D3dx9math.h>
#include <ds_imgui.h>
#include <Windows.h>
#include "..\Mesh.h"

void bbLogHandler(const LogLevel&, const char* message) {
	OutputDebugString(message);
	OutputDebugString("\n");
}

const ds::Color COLORS[] = {
	ds::Color(1.0f,1.0f,1.0f,1.0f),
	ds::Color(1.0f,0.0f,0.0f,1.0f),
	ds::Color(1.0f,1.0f,1.0f,1.0f),
	ds::Color(1.0f,1.0f,1.0f,1.0f),
	ds::Color(1.0f,1.0f,1.0f,1.0f),
	ds::Color(1.0f,0.0f,0.0f,1.0f),
};

// ---------------------------------------------------------------
// convert time into TimeCol
// ---------------------------------------------------------------
static void convert(TimeCol* columns, int index, int value, bool flip = true) {
	int upper = value / 10;
	int lower = value - upper * 10;
	if (value < 10) {
		lower = value;
		upper = 0;
	}
	for (int i = 0; i < 4; ++i) {
		int curUpper = columns[index].state[i];
		int curLower = columns[index + 1].state[i];
		int r = 0;
		if (upper & (1 << i)) {
			r = 1;
		}
		columns[index].state[i] = r;
		if (r != curUpper && flip) {
			if (r == 1) {
				columns[index].flipping[i] = 1;
			}
			else {
				columns[index].flipping[i] = -1;
			}
			columns[index].timer[i] = 0.0f;
		}
		if (!flip) {
			columns[index].angle[i] = r * ds::PI * 0.5f;
		}
		r = 0;
		if (lower & (1 << i)) {
			r = 1;
		}
		if (r != curLower && flip) {
			if (r == 1) {
				columns[index + 1].flipping[i] = 1;
			}
			else {
				columns[index + 1].flipping[i] = -1;
			}
			columns[index + 1].timer[i] = 0.0f;
		}
		columns[index + 1].state[i] = r;
		if (!flip) {
			columns[index + 1].angle[i] = r * ds::PI * 0.5f;
		}
	}
}

BinaryClock::BinaryClock() {
}

BinaryClock::~BinaryClock() {

}

// ---------------------------------------------------------------
// get rendersettings
// ---------------------------------------------------------------
ds::RenderSettings BinaryClock::getRenderSettings() {
	ds::RenderSettings rs;
	rs.width = 1024;
	rs.height = 768;
	rs.title = "Binary Clock";
	rs.clearColor = ds::Color(0.1f, 0.1f, 0.1f, 1.0f);
	rs.multisampling = 4;
	rs.useGPUProfiling = false;
	rs.supportDebug = false;
	rs.logHandler = bbLogHandler;
	return rs;
}

// ---------------------------------------------------------------
// init
// ---------------------------------------------------------------
bool BinaryClock::init() {
	
	ds::matrix viewMatrix = ds::matLookAtLH(ds::vec3(0.0f, 1.0f, -3.0f), ds::vec3(0, 0, 0), ds::vec3(0, 1, 0));
	ds::matrix projectionMatrix = ds::matPerspectiveFovLH(ds::PI / 4.0f, ds::getScreenAspectRatio(), 0.01f, 100.0f);
	_camera = {
		viewMatrix,
		projectionMatrix,
		viewMatrix * projectionMatrix,
		ds::vec3(0,1,-3),
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

	RID textureID = loadImageFromFile("content\\NormalBlock.png");

	ds::BlendStateInfo blendInfo = { ds::BlendStates::SRC_ALPHA, ds::BlendStates::SRC_ALPHA, ds::BlendStates::INV_SRC_ALPHA, ds::BlendStates::INV_SRC_ALPHA, true };
	RID bs_id = ds::createBlendState(blendInfo);
	
	ds::ShaderInfo bumpVSInfo = { 0, Bump_VS_Main, sizeof(Bump_VS_Main), ds::ShaderType::ST_VERTEX_SHADER };
	RID bumpVS = ds::createShader(bumpVSInfo);
	ds::ShaderInfo bumpPSInfo = { 0, Bump_PS_Main, sizeof(Bump_PS_Main), ds::ShaderType::ST_PIXEL_SHADER };
	RID bumpPS = ds::createShader(bumpPSInfo);

	_lightPos = ds::vec3(0.0f, 6.0f, -6.0f);

	_constantBuffer.viewprojectionMatrix = ds::matTranspose(_camera.viewProjectionMatrix);
	_constantBuffer.worldMatrix = ds::matTranspose(ds::matIdentity());
	_constantBuffer.eyePos = _camera.position;
	_constantBuffer.lightPos = _lightPos;
	_constantBuffer.padding = 0.0f;
	_constantBuffer.more = 0.0f;

	RID cbid = ds::createConstantBuffer(sizeof(BinaryClockConstantBuffer), &_constantBuffer);
	RID indexBuffer = ds::createQuadIndexBuffer(256);

	Mesh mesh;	
	mesh.createCube();
	mesh.calculateTangents(0, 1);
	ds::Color* clr = new ds::Color[24];
	for (int i = 0; i < 24; ++i) {
		clr[i] = COLORS[i / 4];
	}
	mesh.addStream(AT_COLOR, (float*)clr, 24, 4);	
	mesh.scaleStream(1, 0.5f);
	//RID cubeBuffer = mesh.assemble();
	mesh.save("models","Test");
	//mesh.load("Test.data");
	RID cubeBuffer = mesh.assemble();

	RID rid = mesh.createInputLayout(bumpVS);

	ds::SamplerStateInfo samplerInfo = { ds::TextureAddressModes::CLAMP, ds::TextureFilters::LINEAR };
	RID ssid = ds::createSamplerState(samplerInfo);

	_fpsCamera = new FPSCamera(&_camera);
	_fpsCamera->setPosition(ds::vec3(0, 0, -5),ds::vec3(0.0f,0.0f,0.0f));

	RID stateGroup = ds::StateGroupBuilder()
		.inputLayout(rid)
		.constantBuffer(cbid, bumpVS, 0)
		.constantBuffer(cbid, bumpPS, 0)
		.blendState(bs_id)
		.samplerState(ssid, bumpPS)
		.vertexBuffer(cubeBuffer)
		.vertexShader(bumpVS)
		.pixelShader(bumpPS)
		.indexBuffer(indexBuffer)
		.texture(textureID, bumpPS, 0)
		.build();


	ds::DrawCommand drawCmd = { 36, ds::DrawType::DT_INDEXED, ds::PrimitiveTypes::TRIANGLE_LIST };
	
	_drawItem = ds::compile(drawCmd, stateGroup);

	_timer = ds::PI;

	_rotate = false;

	for (int i = 0; i < 6; ++i) {
		TimeCol& col = _columns[i];
		col.num = 4;
		col.state[0] = 0;
		col.state[1] = 1;
		col.state[2] = 0;
		col.state[3] = 2;
		for (int j = 0; j < 4; ++j) {
			col.timer[j] = 0.0f;
			col.flipping[j] = 0;
			col.angle[j] = 0.0f;
		}
	}

	GetLocalTime(&_systemTime);
	convert(_columns, 0, _systemTime.wHour, false);
	convert(_columns, 2, _systemTime.wMinute, false);
	convert(_columns, 4, _systemTime.wSecond, false);

	_columns[0].num = 2;
	_columns[2].num = 3;
	_columns[4].num = 3;

	_running = true;

	_rotationDelay = 0.2f;

	return true;

}

// ---------------------------------------------------------------
// tick
// ---------------------------------------------------------------
void BinaryClock::tick(float dt) {
	_fpsCamera->update(dt);
	if (_running) {
		_timer += dt;
		
		GetLocalTime(&_systemTime);

		convert(_columns, 0, _systemTime.wHour);
		convert(_columns, 2, _systemTime.wMinute);
		convert(_columns, 4, _systemTime.wSecond);
	
		for (int i = 0; i < 6; ++i) {
			TimeCol& col = _columns[i];
			for (int j = 0; j < 4; ++j) {
				if (col.flipping[j] == 1) {
					col.timer[j] += dt;
					col.angle[j] = col.timer[j] / _rotationDelay * ds::PI * 0.5f;
					if (col.timer[j] >_rotationDelay) {
						col.flipping[j] = 0;
						col.timer[j] = 0.0f;
						col.angle[j] = col.state[j] * ds::PI * 0.5f;
					}
				}
				else if (col.flipping[j] == -1) {
					col.timer[j] += dt;
					col.angle[j] = (_rotationDelay - col.timer[j]) / _rotationDelay * ds::PI * 0.5f;
					if (col.timer[j] > _rotationDelay) {
						col.flipping[j] = 0;
						col.timer[j] = 0.0f;
						col.angle[j] = col.state[j] * ds::PI * 0.5f;
					}
				}
			}
		}
	}
}

// ---------------------------------------------------------------
// render
// ---------------------------------------------------------------
void BinaryClock::render() {

	_constantBuffer.viewprojectionMatrix = ds::matTranspose(_camera.viewProjectionMatrix);
	_constantBuffer.eyePos = _camera.position;
	_constantBuffer.lightPos = _lightPos;
	_constantBuffer.padding = 0.0f;

	float xp = -2.75f;
	for (int j = 0; j < 6; ++j) {
		const TimeCol& col = _columns[j];
		xp += 0.6f;
		if (j % 2 == 0) {
			xp += 0.3f;
		}
		float yp = -1.0f;
		for (int i = 0; i < col.num; ++i) {			
			float sc = 0.5f;
			ds::vec3 scale = ds::vec3(sc, sc, sc);
			ds::matrix s = ds::matScale(scale);
			ds::vec3 rot = ds::vec3(0.0f);		
			rot.y = col.angle[i];
			ds::matrix r = ds::matRotation(rot);
			ds::vec3 p = ds::vec3(xp, yp, 0.0f);
			ds::matrix w = ds::matTranslate(p);
			ds::matrix srw = s * r * w;
			_constantBuffer.worldMatrix = ds::matTranspose(srw);
			srw._41 = 0.0f;
			srw._42 = 0.0f;
			srw._43 = 0.0f;
			ds::matrix iw = srw;
			_constantBuffer.invWorld = ds::matTranspose(iw);						
			ds::submit(_basicPass, _drawItem);
			yp += 0.6f;
		}
	}
}

// ---------------------------------------------------------------
// renderGUI
// ---------------------------------------------------------------
void BinaryClock::renderGUI() {
	int state = 1;
	gui::start();
	p2i sp = p2i(10, 760);
	if (gui::begin("Debug", &state, &sp, 300)) {
		gui::Value("FPS", ds::getFramesPerSecond());
		if (gui::Input("Light", &_lightPos)) {
			_fpsCamera->setPosition(_lightPos, ds::vec3(0.0f, 0.0f, 0.0f));
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
		gui::Input("Delay", &_rotationDelay);		
	}
	gui::end();
}