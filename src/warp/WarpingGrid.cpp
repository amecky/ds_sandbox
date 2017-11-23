#include "WarpingGrid.h"
#include <ds_imgui.h>
#include "..\..\shaders\WarpingGrid_VS_Main.h"
#include "..\..\shaders\WarpingGrid_PS_Main.h"

const float TIME_STEPSIZE2 = 0.95f * 0.95f;

void wgLogHandler(const LogLevel&, const char* message) {
	OutputDebugString(message);
	OutputDebugString("\n");
}

WarpingGrid::WarpingGrid(const WarpingGridSettings& settings) : _settings(settings) {
	float halfGridDimension = _settings.size * 0.5f;
	_gridCenter.x = static_cast<float>(_settings.numX) * _settings.size * 0.5f - halfGridDimension;
	_gridCenter.y = static_cast<float>(_settings.numY) * _settings.size * 0.5f - halfGridDimension;
	float zOffset = _settings.zOffset;
	float dd = _settings.borderSize;
	_offsets[0] = ds::vec3(dd, -dd, zOffset);
	_offsets[1] = ds::vec3(-dd, -dd, zOffset);
	_offsets[2] = ds::vec3(dd, dd, zOffset);
	_offsets[3] = ds::vec3(-dd, -dd, zOffset);
	_offsets[4] = ds::vec3(-dd, dd, zOffset);
	_offsets[5] = ds::vec3(dd, dd, zOffset);
}

WarpingGrid::~WarpingGrid() {
	delete _points;
	delete _indices;
	delete _vertices;
}

// ----------------------------------------------------
// convert grid coordinates to screen
// ----------------------------------------------------
ds::vec3 WarpingGrid::convert_grid_coords(int x, int y) {
	return ds::vec3(-_gridCenter.x + x * _settings.size, _gridCenter.y - y * _settings.size, 0.0f);
}

// ----------------------------------------------------
// add spring
// ----------------------------------------------------
void WarpingGrid::addSpring(int x1, int y1, int x2, int y2, float stiffness, float damping) {
	Spring spring;
	spring.sx = x1;
	spring.sy = y1;
	spring.ex = x2;
	spring.ey = y2;
	spring.stiffness = stiffness;
	spring.damping = damping;
	const GridPoint& sp = _points[x1 + y1 * _settings.numX];
	const GridPoint& ep = _points[x2 + y2 * _settings.numX];
	spring.targetLength = distance(sp.position, ep.position);// *0.95f;
	_springs.push_back(spring);
}

// ----------------------------------------------------
// prepareData
// ----------------------------------------------------
void WarpingGrid::prepareData() {
	_plane = Plane(ds::vec3(0.0f, 0.0f, -0.1f), ds::vec3(0.0f, 0.0f, -1.0f));
	_points = new GridPoint[_settings.numX * _settings.numY];
	for (int y = 0; y < _settings.numY; ++y) {
		for (int x = 0; x < _settings.numX; ++x) {
			GridPoint& vp = _points[x + y * _settings.numX];
			vp.position = convert_grid_coords(x, y);
			vp.acceleration = ds::vec3(0.0f);
			vp.damping = 0.98f;
			vp.invMass = 1.0f;
			vp.movable = true;
			vp.color = _settings.color;
			vp.timer = 0.0f;
			vp.marked = false;
			if (x == 0 || y == 0 || x == (_settings.numX - 1) || y == (_settings.numY - 1)) {
				vp.invMass = 0.0f;
				vp.movable = false;
			}
			vp.velocity = ds::vec3(0.0f);
			vp.old_pos = vp.position;
		}
	}

	for (int y = 0; y < _settings.numY; ++y) {
		for (int x = 0; x < _settings.numX; ++x) {
			if (x > 0)
				addSpring(x - 1, y, x, y, _settings.stiffness, _settings.damping);
			if (y > 0)
				addSpring(x, y - 1, x, y, _settings.stiffness, _settings.damping);
		}
	}

	int cnt = 0;
	_numVertices = (_settings.numX - 1) * (_settings.numY - 1) * 6;
	_indices = new uint16_t[_numVertices];
	for (int y = 0; y < _settings.numY - 1; ++y) {
		for (int x = 0; x < _settings.numX - 1; ++x) {
			int idx = x + y * _settings.numX;
			_indices[cnt++] = idx;
			_indices[cnt++] = idx + 1;
			_indices[cnt++] = idx + _settings.numX;
			_indices[cnt++] = idx + 1;
			_indices[cnt++] = idx + _settings.numX + 1;
			_indices[cnt++] = idx + _settings.numX;
		}
	}
	
	_vertices = new WarpingGridVertex[_numVertices];

	for (int i = 0; i < _numVertices; ++i) {
		_vertices[i].pos = _points[_indices[i]].position;
		_vertices[i].color = _settings.color;
	}

}

// ----------------------------------------------------
// init
// ----------------------------------------------------
bool WarpingGrid::init() {

	prepareData();

	ds::BlendStateInfo blendInfo = { ds::BlendStates::SRC_ALPHA, ds::BlendStates::SRC_ALPHA, ds::BlendStates::INV_SRC_ALPHA, ds::BlendStates::INV_SRC_ALPHA, true };
	RID bs_id = ds::createBlendState(blendInfo);

	ds::ShaderInfo bumpVSInfo = { 0, WarpingGrid_VS_Main, sizeof(WarpingGrid_VS_Main), ds::ShaderType::ST_VERTEX_SHADER };
	RID bumpVS = ds::createShader(bumpVSInfo);
	ds::ShaderInfo bumpPSInfo = { 0, WarpingGrid_PS_Main, sizeof(WarpingGrid_PS_Main), ds::ShaderType::ST_PIXEL_SHADER };
	RID bumpPS = ds::createShader(bumpPSInfo);

	ds::InputLayoutDefinition decl[] = {
		{ "POSITION" , 0, ds::BufferAttributeType::FLOAT3 },
		{ "COLOR"    , 0, ds::BufferAttributeType::FLOAT4 }
	};
	ds::InputLayoutInfo ilInfo = { decl, 2, bumpVS };
	RID rid = ds::createInputLayout(ilInfo);

	_constantBuffer.mvp = ds::matTranspose(ds::matIdentity());
	_constantBuffer.world = ds::matTranspose(ds::matIdentity());
	
	RID cbid = ds::createConstantBuffer(sizeof(InstanceBuffer), &_constantBuffer);

	ds::VertexBufferInfo vbInfo = { ds::BufferType::DYNAMIC,_numVertices,sizeof(WarpingGridVertex),_vertices };
	_cubeBuffer = ds::createVertexBuffer(vbInfo);

	ds::SamplerStateInfo samplerInfo = { ds::TextureAddressModes::CLAMP, ds::TextureFilters::LINEAR };
	RID ssid = ds::createSamplerState(samplerInfo);

	RID stateGroup = ds::StateGroupBuilder()
		.inputLayout(rid)
		.constantBuffer(cbid, bumpVS, 0)
		.blendState(bs_id)
		.samplerState(ssid, bumpPS)
		.vertexBuffer(_cubeBuffer)
		.vertexShader(bumpVS)
		.pixelShader(bumpPS)
		.build();

	ds::DrawCommand drawCmd = { _numVertices, ds::DrawType::DT_VERTICES, ds::PrimitiveTypes::TRIANGLE_LIST };

	_drawItem = ds::compile(drawCmd, stateGroup);

	return true;
}

// ----------------------------------------------------
// tick
// ----------------------------------------------------
void WarpingGrid::tick(float dt) {
	for (int y = 0; y < _settings.numY; ++y) {
		for (int x = 0; x < _settings.numX; ++x) {
			GridPoint& gp = _points[x + y * _settings.numX];
			if (gp.movable) {
				ds::vec3 temp = gp.position;
				gp.position = gp.position + (gp.position - gp.old_pos) * gp.damping + gp.acceleration * TIME_STEPSIZE2;
				gp.old_pos = temp;
				gp.acceleration = ds::vec3(0.0f);
			}
			if (gp.timer > 0.0f) {
				gp.timer -= dt;
				float c = abs(sin(gp.timer / 2.0f * ds::PI*4.0f));
				gp.color = ds::Color(c, 0.0f, 0.0f, 1.0f);
				if (gp.timer < 0.0f) {
					gp.timer = 0.0f;
					gp.color = _settings.color;
				}
			}
		}
	}

	for (uint32_t i = 0; i < _springs.size(); ++i) {
		Spring& spring = _springs[i];
		GridPoint& sp = _points[spring.sx + spring.sy * _settings.numX];
		GridPoint& ep = _points[spring.ex + spring.ey * _settings.numX];
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

// ----------------------------------------------------
// render
// ----------------------------------------------------
void WarpingGrid::render(RID renderPass, const ds::matrix& viewProjectionMatrix) {

	_constantBuffer.mvp = ds::matTranspose(viewProjectionMatrix);
	float zOffset = 0.5f;
	float dd = 0.01f;
	for (int i = 0; i < _numVertices; ++i) {
		_vertices[i].pos = _points[_indices[i]].position;
		_vertices[i].pos += _offsets[i % 6];
		_vertices[i].color = _points[_indices[i]].color;
	}
	ds::mapBufferData(_cubeBuffer, _vertices, sizeof(WarpingGridVertex) * _numVertices);
	ds::submit(renderPass, _drawItem);
}

// ----------------------------------------------------
// apply force
// ----------------------------------------------------
void WarpingGrid::applyForce(int x, int y, const ds::vec3& force) {
	GridPoint& gp = _points[x + y * _settings.numX];
	gp.acceleration += force * gp.invMass;
}

// ----------------------------------------------------
// get ray intersection point
// ----------------------------------------------------
ds::vec3 WarpingGrid::getIntersectionPoint(const Ray & r) {
	return _plane.getIntersection(r);
}

// ----------------------------------------------------
// highlight
// ----------------------------------------------------
void WarpingGrid::highlight(int x, int y) {
	int xds[] = { 0,1,0,1 };
	int yds[] = { 0,0,1,1 };
	for (int i = 0; i < 4; ++i) {
		GridPoint& gp = _points[x +xds[i] + (y + yds[i])* _settings.numX];
		gp.color = ds::Color(1.0f, 0.0f, 0.0f, 1.0f);
		gp.timer = 2.0f;
		gp.marked = true;
	}
}

// ----------------------------------------------------
// apply force with radius
// ----------------------------------------------------
void WarpingGrid::applyForce(const ds::vec3& center, float radius, const ds::vec3& force) {
	for (int y = 0; y < _settings.numY; ++y) {
		for (int x = 0; x < _settings.numX; ++x) {
			GridPoint& gp = _points[x + y * _settings.numX];
			ds::vec3 delta = gp.position - center;
			if (sqr_length(delta) < radius * radius) {
				gp.acceleration += force * gp.invMass;
			}
		}
	}
}

// ----------------------------------------------------
// apply force
// ----------------------------------------------------
void WarpingGrid::applyForce(int x, int y, float radius, const ds::vec3& force) {
	ds::vec3 center = convert_grid_coords(x, y);
	applyForce(center, radius, force);
}
