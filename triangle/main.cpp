#define DS_IMPLEMENTATION
#define DS_MATH_IMPLEMENTATION
#include <diesel.h>
#include "Triangle_VS_Main.h"
#include "Triangle_PS_Main.h"
#include "Camera.h"

static const ds::Color COLORS[] = {
	ds::Color(255,0,0,255),
	ds::Color(0,255,0,255),
	ds::Color(255,255,0,255),
	ds::Color(0,0,255,255),
	ds::Color(32,32,32,255)
};

struct ConstantBuffer {
	ds::matrix viewprojectionMatrix;
	ds::matrix worldMatrix;
};

// ---------------------------------------------------------------
// Vertex
// ---------------------------------------------------------------
struct Vertex {
	ds::vec3 p;
	ds::Color color;

};

void createTorus(Vertex* v, int maxVertices, int segments, float width, float depth) {
	int cnt = 0;
	float r1 = 1.0f;
	float r2 = r1 + width;
	float step = ds::TWO_PI / 32.0f;
	float angle = 0.0f;
	ds::Color c(255, 0, 0, 255);
	ds::vec3 center(0.0f, 0.0f,1.0f);
	for (int i = 0; i < segments; ++i) {
		int idx = i % 5;
		c = COLORS[idx];
		// front
		ds::vec3 p1 = center + ds::vec3(cos(angle), sin(angle),0.0f) * r1;
		ds::vec3 p2 = center + ds::vec3(cos(angle), sin(angle), 0.0f) * r2;
		ds::vec3 p3 = center + ds::vec3(cos(angle - step), sin(angle - step), 0.0f) * r2;
		ds::vec3 p4 = center + ds::vec3(cos(angle - step), sin(angle - step), 0.0f) * r1;
		v[cnt++] = { p1, c };
		v[cnt++] = { p2, c };
		v[cnt++] = { p3, c };
		v[cnt++] = { p4, c };

		p1 = center + ds::vec3(cos(angle), sin(angle), 0.0f) * r2;
		p2 = center + ds::vec3(cos(angle), sin(angle), depth) * r2;
		p3 = center + ds::vec3(cos(angle - step), sin(angle - step), depth) * r2;
		p4 = center + ds::vec3(cos(angle - step), sin(angle - step), 0.0f) * r2;
		v[cnt++] = { p1, c };
		v[cnt++] = { p2, c };
		v[cnt++] = { p3, c };
		v[cnt++] = { p4, c };

		p1 = center + ds::vec3(cos(angle), sin(angle), depth) * r1;
		p2 = center + ds::vec3(cos(angle), sin(angle), 0.0f) * r1;
		p3 = center + ds::vec3(cos(angle - step), sin(angle - step), 0.0f) * r1;
		p4 = center + ds::vec3(cos(angle - step), sin(angle - step), depth) * r1;
		v[cnt++] = { p1, c };
		v[cnt++] = { p2, c };
		v[cnt++] = { p3, c };
		v[cnt++] = { p4, c };

		p4 = center + ds::vec3(cos(angle), sin(angle), depth) * r1;
		p3 = center + ds::vec3(cos(angle), sin(angle), depth) * r2;
		p2 = center + ds::vec3(cos(angle - step), sin(angle - step), depth) * r2;
		p1 = center + ds::vec3(cos(angle - step), sin(angle - step), depth) * r1;
		v[cnt++] = { p1, c };
		v[cnt++] = { p2, c };
		v[cnt++] = { p3, c };
		v[cnt++] = { p4, c };

		angle += step;
	}
}

// ---------------------------------------------------------------
// main method
// ---------------------------------------------------------------
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow) {
	//
	// prepare application
	//
	ds::RenderSettings rs;
	rs.width = 1024;
	rs.height = 768;
	rs.title = "Triangle demo";
	rs.clearColor = ds::Color(0.1f, 0.1f, 0.1f, 1.0f);
	rs.multisampling = 4;
	rs.useGPUProfiling = false;
	rs.supportDebug = true;
	ds::init(rs);
	//
	// create render with view and projection matrix
	//
	ds::matrix viewMatrix = ds::matLookAtLH(ds::vec3(0.0f, 0.0f, -4.0f), ds::vec3(0, 0, 0), ds::vec3(0, 1, 0));
	ds::matrix projectionMatrix = ds::matPerspectiveFovLH(ds::PI / 4.0f, ds::getScreenAspectRatio(), 0.01f, 100.0f);
	ds::Camera camera = {
		viewMatrix,
		projectionMatrix,
		viewMatrix * projectionMatrix,
		ds::vec3(0,0,-4),
		ds::vec3(0,0,1),
		ds::vec3(0,1,0),
		ds::vec3(1,0,0),
		0.0f,
		0.0f,
		0.0f
	};

	RID vp = ds::createViewport(ds::ViewportDesc()
		.Top(0)
		.Left(0)
		.Width(1024)
		.Height(768)
		.MinDepth(0.0f)
		.MaxDepth(1.0f)
	);

	
	RID basicPass = ds::createRenderPass(ds::RenderPassDesc()
		.Camera(&camera)
		.Viewport(vp)
		.DepthBufferState(ds::DepthBufferState::ENABLED)
		.RenderTargets(0)
		.NumRenderTargets(0));
	//
	// create resources
	//
	
	RID vertexShader = ds::createShader(ds::ShaderDesc()
		.Data(Triangle_VS_Main)
		.DataSize(sizeof(Triangle_VS_Main))
		.ShaderType(ds::ShaderType::ST_VERTEX_SHADER)
	);

	RID pixelShader = ds::createShader(ds::ShaderDesc()
		.Data(Triangle_PS_Main)
		.DataSize(sizeof(Triangle_PS_Main))
		.ShaderType(ds::ShaderType::ST_PIXEL_SHADER)
	);

	// create buffer input layout
	ds::InputLayoutDefinition decl[] = {
		{ "POSITION", 0, ds::BufferAttributeType::FLOAT3 },
		{ "COLOR", 0, ds::BufferAttributeType::FLOAT4 }
	};

	const int segments = 32;
	const int numVertices = segments * 16;
	Vertex v[numVertices];
	createTorus(v, numVertices, segments, 0.2f, 0.2f);
	
	
	RID rid = ds::createInputLayout(ds::InputLayoutDesc()
	.	Declarations(decl)
		.NumDeclarations(2)
		.VertexShader(vertexShader)
	);

	RID vbid = ds::createVertexBuffer(ds::VertexBufferDesc()
		.BufferType(ds::BufferType::STATIC)
		.Data(v)
		.NumVertices(numVertices)
		.VertexSize(sizeof(Vertex))
	);

	RID indexBuffer = ds::createQuadIndexBuffer(numVertices / 4);

	ConstantBuffer constantBuffer;
	constantBuffer.worldMatrix = ds::matIdentity();
	constantBuffer.viewprojectionMatrix = ds::matIdentity();
	RID cbId = ds::createConstantBuffer(sizeof(ConstantBuffer), &constantBuffer);
	//
	// create state group
	//
	RID stateGroup = ds::StateGroupBuilder()
		.inputLayout(rid)
		.constantBuffer(cbId,vertexShader)
		.vertexBuffer(vbid)
		.vertexShader(vertexShader)
		.indexBuffer(indexBuffer)
		.pixelShader(pixelShader)
		.build();
	//
	// the draw command
	//
	ds::DrawCommand drawCmd = { numVertices / 4 * 6, ds::DrawType::DT_INDEXED, ds::PrimitiveTypes::TRIANGLE_LIST };
	//
	// and finally we can create the draw item
	//
	RID drawItem = ds::compile(drawCmd, stateGroup);

	FPSCamera fpsCamera(&camera);
	fpsCamera.setPosition(ds::vec3(0, 0, -4));
	fpsCamera.buildView();

	while (ds::isRunning()) {

		ds::begin();

		fpsCamera.update(static_cast<float>(ds::getElapsedSeconds()));

		constantBuffer.viewprojectionMatrix = ds::matTranspose(camera.viewProjectionMatrix);

		ds::submit(basicPass, drawItem);

		ds::dbgPrint(0, 0, "FPS: %d", ds::getFramesPerSecond());

		ds::end();
	}
	ds::shutdown();
	return 0;
}