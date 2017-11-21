#include "WarpingGrid.h"
#include <ds_imgui.h>
#include "..\..\shaders\WarpingGrid_VS_Main.h"
#include "..\..\shaders\WarpingGrid_PS_Main.h"

const float TIME_STEPSIZE2 = 0.95f * 0.95f;

void wgLogHandler(const LogLevel&, const char* message) {
	OutputDebugString(message);
	OutputDebugString("\n");
}

ds::vec3 convert_grid_coords(int x, int y) {
	float cx = static_cast<float>(GRID_SIZE_X) * GRID_DIM * 0.5f - GRID_DIM * 0.5f;
	float cy = static_cast<float>(GRID_SIZE_Y) * GRID_DIM * 0.5f - GRID_DIM * 0.5f;
	return ds::vec3(-cx + x * GRID_DIM, -cy + y * GRID_DIM, 0.0f);
}

WarpingGrid::WarpingGrid() {

}

WarpingGrid::~WarpingGrid() {
	delete _points;
	delete _indices;
	delete _vertices;
}

ds::RenderSettings WarpingGrid::getRenderSettings() {
	ds::RenderSettings rs;
	rs.width = 1024;
	rs.height = 768;
	rs.title = "Warping grid";
	rs.clearColor = ds::Color(0.01f, 0.01f, 0.01f, 1.0f);
	rs.multisampling = 4;
	rs.useGPUProfiling = false;
	rs.supportDebug = true;
	rs.logHandler = wgLogHandler;
	return rs;
}

void WarpingGrid::addSpring(int x1, int y1, int x2, int y2, float stiffness, float damping) {
	Spring spring;
	spring.sx = x1;
	spring.sy = y1;
	spring.ex = x2;
	spring.ey = y2;
	spring.stiffness = stiffness;
	spring.damping = damping;
	const GridPoint& sp = _points[x1 + y1 * GRID_SIZE_X];
	const GridPoint& ep = _points[x2 + y2 * GRID_SIZE_X];
	spring.targetLength = distance(sp.position, ep.position);// *0.95f;
	_springs.push_back(spring);
}

bool WarpingGrid::init() {

	_points = new GridPoint[TOTAL_GRID_SIZE];
	for (int y = 0; y < GRID_SIZE_Y; ++y) {
		for (int x = 0; x < GRID_SIZE_X; ++x) {
			GridPoint& vp = _points[x + y * GRID_SIZE_X];
			vp.position = convert_grid_coords(x, y);
			vp.acceleration = ds::vec3(0.0f);
			vp.damping = 0.98f;
			vp.invMass = 1.0f;
			vp.movable = true;
			if (x == 0 || y == 0 || x == (GRID_SIZE_X - 1) || y == (GRID_SIZE_Y - 1)) {
				vp.invMass = 0.0f;
				vp.movable = false;
			}
			vp.velocity = ds::vec3(0.0f);
			vp.old_pos = vp.position;
		}
	}

	for (int y = 0; y < GRID_SIZE_Y; ++y) {
		for (int x = 0; x < GRID_SIZE_X; ++x) {
			const float stiffness = 0.28f;
			const float damping = 0.06f;
			if (x > 0)
				addSpring(x - 1, y, x, y, stiffness, damping);
			if (y > 0)
				addSpring(x, y - 1, x, y, stiffness, damping);
		}
	}

	int cnt = 0;
	int sz = (GRID_SIZE_X - 1) * (GRID_SIZE_Y - 1) * 6;
	_indices = new uint16_t[sz];
	for (int y = 0; y < GRID_SIZE_Y - 1; ++y) {
		for (int x = 0; x < GRID_SIZE_X - 1; ++x) {
			int idx = x + y * GRID_SIZE_X;
			_indices[cnt++] = idx;
			_indices[cnt++] = idx + 1;
			_indices[cnt++] = idx + GRID_SIZE_X;
			_indices[cnt++] = idx + 1;
			_indices[cnt++] = idx + GRID_SIZE_X + 1;
			_indices[cnt++] = idx + GRID_SIZE_X;
		}
	}
	ds::vec2 UV[] = { ds::vec2(0.0f,0.0f),ds::vec2(1.0f,0.0f),ds::vec2(0.0f,1.0f),ds::vec2(1.0f,0.0f),ds::vec2(1.0f,1.0f),ds::vec2(0.0f,1.0f) };

	int offset = 0;
	_numVertices = sz;
	_vertices = new WarpingGridVertex[sz];
	for (int i = 0; i < sz; ++i) {
		_vertices[i].pos = _points[_indices[i]].position;
		_vertices[i].color = ds::Color(1.0f,1.0f,1.0f, 1.0f);
		//_vertices[i].color = ds::Color(0.5f, 0.0f, 0.0f, 1.0f);
		int idx = i % 3;
		offset = (i / 3) & 1;
		_vertices[i].uv = UV[offset * 3 + idx];				
		ds::log(LogLevel::LL_DEBUG, "%2d offset: %2d idx: %2d", i, offset, idx);
	}
	
	_camera = ds::buildPerspectiveCamera(ds::vec3(0.0f, 0.0f, -6.0f));

	ds::ViewportInfo vpInfo = { 1024, 768, 0.0f, 1.0f };
	RID vp = ds::createViewport(vpInfo);

	ds::RenderPassInfo rpInfo = { &_camera, vp, ds::DepthBufferState::ENABLED, 0, 0 };
	_basicPass = ds::createRenderPass(rpInfo);

	ds::BlendStateInfo blendInfo = { ds::BlendStates::SRC_ALPHA, ds::BlendStates::SRC_ALPHA, ds::BlendStates::INV_SRC_ALPHA, ds::BlendStates::INV_SRC_ALPHA, true };
	RID bs_id = ds::createBlendState(blendInfo);

	ds::ShaderInfo bumpVSInfo = { 0, WarpingGrid_VS_Main, sizeof(WarpingGrid_VS_Main), ds::ShaderType::ST_VERTEX_SHADER };
	RID bumpVS = ds::createShader(bumpVSInfo);
	ds::ShaderInfo bumpPSInfo = { 0, WarpingGrid_PS_Main, sizeof(WarpingGrid_PS_Main), ds::ShaderType::ST_PIXEL_SHADER };
	RID bumpPS = ds::createShader(bumpPSInfo);

	_lightPos = ds::vec3(0.0f, 0.0f, -6.0f);

	ds::InputLayoutDefinition decl[] = {
		{ "POSITION" , 0, ds::BufferAttributeType::FLOAT3 },
		{ "COLOR"    , 0, ds::BufferAttributeType::FLOAT4 },
		{ "TEXCOORD" , 0, ds::BufferAttributeType::FLOAT2 }
	};
	ds::InputLayoutInfo ilInfo = { decl, 3, bumpVS };
	RID rid = ds::createInputLayout(ilInfo);

	_constantBuffer.mvp = ds::matTranspose(_camera.viewProjectionMatrix);
	_constantBuffer.world = ds::matTranspose(ds::matIdentity());
	
	RID cbid = ds::createConstantBuffer(sizeof(InstanceBuffer), &_constantBuffer);

	ds::VertexBufferInfo vbInfo = { ds::BufferType::DYNAMIC,sz,sizeof(WarpingGridVertex),_vertices };
	_cubeBuffer = ds::createVertexBuffer(vbInfo);

	_lightBuffer.ambientColor = ds::Color(0.1f, 0.1f, 0.1f, 1.0f);
	_lightBuffer.diffuseColor = ds::Color(1.0f, 1.0f, 1.0f, 1.0f);
	_lightBuffer.lightDirection = ds::vec3(1.0f, 0.0f, 1.0f);
	_lightBuffer.padding = 0.0f;
	RID lbid = ds::createConstantBuffer(sizeof(InstanceLightBuffer), &_lightBuffer);

	ds::SamplerStateInfo samplerInfo = { ds::TextureAddressModes::CLAMP, ds::TextureFilters::LINEAR };
	RID ssid = ds::createSamplerState(samplerInfo);

	_fpsCamera = new FPSCamera(&_camera);
	_fpsCamera->setPosition(ds::vec3(0, 0, -6), ds::vec3(0.0f, 0.0f, 0.0f));

	RID stateGroup = ds::StateGroupBuilder()
		.inputLayout(rid)
		.constantBuffer(cbid, bumpVS, 0)
		.constantBuffer(lbid, bumpPS, 0)
		.blendState(bs_id)
		.samplerState(ssid, bumpPS)
		.vertexBuffer(_cubeBuffer)
		.vertexShader(bumpVS)
		.pixelShader(bumpPS)
		.build();


	ds::DrawCommand drawCmd = { sz, ds::DrawType::DT_VERTICES, ds::PrimitiveTypes::TRIANGLE_LIST };

	_drawItem = ds::compile(drawCmd, stateGroup);

	_timer = 0.0f;

	_dbgX = 10;
	_dbgY = 10;
	_dbgForce = 0.2f;

	return true;
}

void WarpingGrid::tick(float dt) {
	_fpsCamera->update(dt);
	_timer += dt;

	for (int y = 0; y < GRID_SIZE_Y; ++y) {
		for (int x = 0; x < GRID_SIZE_X; ++x) {
			GridPoint& gp = _points[x + y * GRID_SIZE_X];
			if (gp.movable) {
				ds::vec3 temp = gp.position;
				gp.position = gp.position + (gp.position - gp.old_pos) * gp.damping + gp.acceleration * TIME_STEPSIZE2;
				gp.old_pos = temp;
				gp.acceleration = ds::vec3(0.0f);
			}
		}
	}

	for (uint32_t i = 0; i < _springs.size(); ++i) {
		Spring& spring = _springs[i];
		GridPoint& sp = _points[spring.sx + spring.sy * GRID_SIZE_X];
		GridPoint& ep = _points[spring.ex + spring.ey * GRID_SIZE_X];
		ds::vec3 delta = ep.position - sp.position;
		float current_distance = length(delta);
		ds::vec3 x = sp.position - ep.position;
		float l = length(x);
		if (l > spring.targetLength) {
			x = (x / l) * (l - spring.targetLength);
			ds::vec3 dv = ep.velocity - sp.velocity;
			ds::vec3 force = spring.stiffness * x - dv * spring.damping;
			sp.acceleration -= force;
			ep.acceleration += force;
		}
	}
}

void WarpingGrid::render() {

	_constantBuffer.mvp = ds::matTranspose(_camera.viewProjectionMatrix);

	for (int i = 0; i < _numVertices; ++i) {
		_vertices[i].pos = _points[_indices[i]].position;
	}
	ds::mapBufferData(_cubeBuffer, _vertices, sizeof(WarpingGridVertex) * _numVertices);
	ds::submit(_basicPass, _drawItem);
}

void WarpingGrid::applyForce(int x, int y, float radius, const ds::vec3& force) {
	ds::vec3 center = convert_grid_coords(x, y);
	for (int y = 0; y < GRID_SIZE_Y; ++y) {
		for (int x = 0; x < GRID_SIZE_X; ++x) {
			GridPoint& gp = _points[x + y * GRID_SIZE_X];
			ds::vec3 delta = gp.position - center;
			if (sqr_length(delta) < radius * radius) {
				gp.acceleration += force * gp.invMass;
			}
		}
	}
}

void WarpingGrid::renderGUI() {
	int state = 1;
	gui::start();
	p2i sp = p2i(10, 760);
	if (gui::begin("Debug", &state, &sp, 200)) {
		gui::Value("FPS", ds::getFramesPerSecond());
	}
	gui::Input("X", &_dbgX);
	gui::Input("Y", &_dbgY);
	gui::Input("Force", &_dbgForce);
	if (gui::Button("Apply")) {
		GridPoint& gp = _points[_dbgX + _dbgY * GRID_SIZE_X];
		gp.acceleration += ds::vec3(0.0f,0.0f,_dbgForce) * gp.invMass;
	}
	if (gui::Button("Apply Ring")) {
		applyForce(_dbgX, _dbgY, 0.6f, _dbgForce);
	}
	gui::end();
}