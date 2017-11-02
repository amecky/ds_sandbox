#include "BinaryClock.h"
#include "..\..\shaders\Bump_VS_Main.h"
#include "..\..\shaders\Bump_PS_Main.h"
#include <D3dx9math.h>
#include <ds_imgui.h>
#include <Windows.h>

void bbLogHandler(const LogLevel&, const char* message) {
	OutputDebugString(message);
	OutputDebugString("\n");
}

const ds::vec3 CUBE_VERTICES[8] = {
	ds::vec3(-0.5f,-0.5f,-0.5f),
	ds::vec3(-0.5f, 0.5f,-0.5f),
	ds::vec3(0.5f, 0.5f,-0.5f),
	ds::vec3(0.5f,-0.5f,-0.5f),
	ds::vec3(-0.5f,-0.5f, 0.5f),
	ds::vec3(-0.5f, 0.5f, 0.5f),
	ds::vec3(0.5f, 0.5f, 0.5f),
	ds::vec3(0.5f,-0.5f, 0.5f)
};

const int CUBE_PLANES[6][4] = {
	{ 0, 1, 2, 3 }, // front
	{ 3, 2, 6, 7 }, // left
	{ 1, 5, 6, 2 }, // top
	{ 4, 5, 1, 0 }, // right
	{ 4, 0, 3, 7 }, // bottom
	{ 7, 6, 5, 4 }, // back
};

const ds::Color COLORS[] = {
	ds::Color(1.0f,0.0f,0.0f,1.0f),
	ds::Color(0.0f,1.0f,0.0f,1.0f),
	ds::Color(0.0f,0.0f,1.0f,1.0f),
	ds::Color(1.0f,1.0f,0.0f,1.0f),
	ds::Color(0.0f,1.0f,1.0f,1.0f),
	ds::Color(1.0f,1.0f,1.0f,1.0f),
};

const float U_OFFSET[] = {
	0.0f,0.0f,0.0f,0.0f,0.0f,0.0f
};

void buildCube(const ds::matrix& m, ds::vec3* positions, ds::vec2* uvs, ds::vec3* normals, ds::Color* colors) {
	for (int side = 0; side < 6; ++side) {
		int idx = side * 4;
		float u[4] = { 0.0f,0.0f,0.5f,0.5f };
		float v[4] = { 0.5f,0.0f,0.0f,0.5f };
		for (int i = 0; i < 4; ++i) {
			int p = idx + i;
			positions[p] = m * CUBE_VERTICES[CUBE_PLANES[side][i]];
			if (uvs != 0) {
				uvs[p] = ds::vec2(u[i] + U_OFFSET[side], v[i]);
			}
			if (colors != 0) {
				colors[p] = COLORS[side];
			}
		}
		if (normals != 0) {
			ds::vec3 d0 = positions[idx + 1] - positions[idx];
			ds::vec3 d1 = positions[idx + 2] - positions[idx];
			ds::vec3 c = cross(d0, d1);
			for (int i = 0; i < 4; ++i) {
				normals[idx + i] = c;
			}
		}
	}
}

void calculateTangents(ds::vec3* positions, ds::vec2* uvs, ds::vec3* tangents, int num) {
	int steps = num / 4;
	for (int i = 0; i < steps; ++i) {
		// Shortcuts for vertices
		ds::vec3& vp0 = positions[i * 4 + 0];
		ds::vec3& vp1 = positions[i * 4 + 1];
		ds::vec3& vp2 = positions[i * 4 + 2];

		// Shortcuts for UVs
		ds::vec2 uv0 = uvs[i * 4 + 0];
		ds::vec2 uv1 = uvs[i * 4 + 1];
		ds::vec2 uv2 = uvs[i * 4 + 2];

		// Edges of the triangle : postion delta
		ds::vec3 deltaPos1 = vp1 - vp0;
		ds::vec3 deltaPos2 = vp2 - vp0;

		// UV delta
		ds::vec2 deltaUV1 = uv1 - uv0;
		ds::vec2 deltaUV2 = uv2 - uv0;
		float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
		ds::vec3 tangent = normalize((deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y)*r);
		tangents[i * 4] = tangent;
		tangents[i * 4 + 1] = tangent;
		tangents[i * 4 + 2] = tangent;
		tangents[i * 4 + 3] = tangent;
	}
}

BinaryClock::BinaryClock() {
}

BinaryClock::~BinaryClock() {

}

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

bool BinaryClock::init() {
	ds::vec3 positions[24];
	ds::vec3 normals[24];
	ds::vec3 tangents[24];
	ds::vec2 uvs[24];
	ds::Color colors[24];
	ds::matrix m = ds::matIdentity();
	buildCube(m, positions, uvs, normals, colors);
	calculateTangents(positions, uvs, tangents, 24);
	for (int i = 0; i < 24; ++i) {
		_vertices[i] = Vertex(positions[i], uvs[i], normals[i], tangents[i], colors[i]);
	}

	ds::matrix viewMatrix = ds::matLookAtLH(ds::vec3(0.0f, 2.0f, -3.0f), ds::vec3(0, 0, 0), ds::vec3(0, 1, 0));
	ds::matrix projectionMatrix = ds::matPerspectiveFovLH(ds::PI / 4.0f, ds::getScreenAspectRatio(), 0.01f, 100.0f);
	_camera = {
		viewMatrix,
		projectionMatrix,
		viewMatrix * projectionMatrix,
		ds::vec3(0,2,-3),
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

	ds::InputLayoutDefinition decl[] = {
		{ "POSITION", 0,ds::BufferAttributeType::FLOAT3 },
		{ "TEXCOORD", 0, ds::BufferAttributeType::FLOAT2 },
		{ "NORMAL"  , 0, ds::BufferAttributeType::FLOAT3 },
		{ "TANGENT" , 0 ,ds::BufferAttributeType::FLOAT3 },
		{ "COLOR" , 0 ,ds::BufferAttributeType::FLOAT4 }
	};

	ds::InputLayoutInfo layoutInfo = { decl, 5, bumpVS };
	RID rid = ds::createInputLayout(layoutInfo);

	_lightPos = ds::vec3(0.0f, 6.0f, -6.0f);

	_constantBuffer.viewprojectionMatrix = ds::matTranspose(_camera.viewProjectionMatrix);
	_constantBuffer.worldMatrix = ds::matTranspose(ds::matIdentity());
	_constantBuffer.eyePos = _camera.position;
	_constantBuffer.lightPos = _lightPos;
	_constantBuffer.padding = 0.0f;
	_constantBuffer.more = 0.0f;

	RID cbid = ds::createConstantBuffer(sizeof(BinaryClockConstantBuffer), &_constantBuffer);
	RID indexBuffer = ds::createQuadIndexBuffer(256);
	ds::VertexBufferInfo vbInfo = { ds::BufferType::STATIC, 24, sizeof(Vertex), _vertices };
	RID cubeBuffer = ds::createVertexBuffer(vbInfo);

	ds::SamplerStateInfo samplerInfo = { ds::TextureAddressModes::CLAMP, ds::TextureFilters::LINEAR };
	RID ssid = ds::createSamplerState(samplerInfo);

	_fpsCamera = new FPSCamera(&_camera);
	_fpsCamera->setPosition(ds::vec3(0, 6, -6),ds::vec3(0.0f,0.0f,0.0f));

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

	return true;

}

void BinaryClock::tick(float dt) {
	_fpsCamera->update(dt);
	_timer += dt;
}

void BinaryClock::render() {

	_constantBuffer.viewprojectionMatrix = ds::matTranspose(_camera.viewProjectionMatrix);
	_constantBuffer.eyePos = _camera.position;
	_constantBuffer.lightPos = _lightPos;
	_constantBuffer.padding = 0.0f;

	
	ds::vec3 POSITIONS[] = { 
		ds::vec3(-2.5f,2.0f,0.0f),
		ds::vec3(-1.0f,2.0f,0.0f), 
		ds::vec3(0.5f,2.0f,0.0f),
		ds::vec3(2.0f,2.0f,0.0f)
	};
	ds::vec3 SCALINGS[] = { 
		ds::vec3(1.0f,1.0f,1.0f),
		ds::vec3(1.0f,1.0f,1.0f),
		ds::vec3(0.5f,0.5f,0.5f),
		ds::vec3(1.0f,1.0f,1.0f),
	};
	ds::vec3 ROTATIONS[] = { 
		ds::vec3(0.0f,0.0f,0.0f),
		ds::vec3(0.0f,ds::PI * 0.25f,0.0f),
		ds::vec3(0.0f,ds::PI * 0.5f,0.0f) ,
		ds::vec3(0.0f,ds::PI * 0.75f,0.0f)
	};
	for (int i = 0; i < 4; ++i) {
		ds::matrix s = ds::matScale(SCALINGS[i]);
		ds::matrix r = ds::matRotation(ROTATIONS[i]);
		ds::matrix w = ds::matTranslate(POSITIONS[i]);
		ds::matrix srw = s * r * w;
		_constantBuffer.worldMatrix = ds::matTranspose(srw);
		srw._41 = 0.0f;
		srw._42 = 0.0f;
		srw._43 = 0.0f;
		ds::matrix iw = srw;
		_constantBuffer.invWorld = ds::matTranspose(iw);
		ds::submit(_basicPass, _drawItem);
	}

	
}

void BinaryClock::renderGUI() {
	int state = 1;
	gui::start();
	p2i sp = p2i(10, 760);
	if (gui::begin("Debug", &state, &sp, 540)) {
		gui::Value("FPS", ds::getFramesPerSecond());
		if (gui::Input("Light", &_lightPos)) {
			_fpsCamera->setPosition(_lightPos, ds::vec3(0.0f, 0.0f, 0.0f));
		}
	}
	gui::end();
}