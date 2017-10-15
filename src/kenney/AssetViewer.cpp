#include <diesel.h>
#include "..\..\shaders\AmbientLightning_VS_Main.h"
#include "..\..\shaders\AmbientLightning_PS_Main.h"
#include "Camera.h"
#include "Grid.h"

struct AmbientVertex {
	ds::vec3 p;
	ds::Color color;
	ds::vec3 n;

	AmbientVertex() : p(0.0f), color(1.0f, 1.0f, 1.0f, 1.0f), n(0.0f) {}
};

// ---------------------------------------------------------------
// the cube constant buffer used by the vertex shader
// ---------------------------------------------------------------
struct CubeConstantBuffer {
	ds::matrix viewprojectionMatrix;
	ds::matrix worldMatrix;
};

struct LightBuffer {
	ds::Color ambientColor;
	ds::Color diffuseColor;
	ds::vec3 lightDirection;
	float padding;
};

struct Entity {
	AmbientVertex* vertices;
	RID drawItem;
};

void load_entity(Entity* e, const char* fileName, RID baseGroup) {
	FILE* fp = fopen(fileName, "rb");
	int total = 0;
	fread(&total, sizeof(int), 1, fp);
	ds::vec3 center = { 0.0f,0.0f,0.0f };
	fread(&center.x, sizeof(float), 1, fp);
	fread(&center.y, sizeof(float), 1, fp);
	fread(&center.z, sizeof(float), 1, fp);
	ds::vec3 extent = { 0.0f,0.0f,0.0f };
	fread(&extent.x, sizeof(float), 1, fp);
	fread(&extent.y, sizeof(float), 1, fp);
	fread(&extent.z, sizeof(float), 1, fp);
	float mm = 0.0f;
	for (int i = 0; i < 3; ++i) {
		if (extent.data[i] > mm) {
			mm = extent.data[i];
		}
	}
	float s = 1.0f / mm;
	ds::matrix sm = ds::matScale(ds::vec3(s, s, s));
	ds::matrix cm = ds::matTranslate(ds::vec3(-center.x, -center.y, -center.z));
	e->vertices = new AmbientVertex[total];
	for (int i = 0; i < total; ++i) {
		AmbientVertex* c = &e->vertices[i];
		fread(&c->p.x, sizeof(float), 1, fp);
		fread(&c->p.y, sizeof(float), 1, fp);
		fread(&c->p.z, sizeof(float), 1, fp);
		fread(&c->color.r, sizeof(float), 1, fp);
		fread(&c->color.g, sizeof(float), 1, fp);
		fread(&c->color.b, sizeof(float), 1, fp);
		fread(&c->color.a, sizeof(float), 1, fp);
		fread(&c->n.x, sizeof(float), 1, fp);
		fread(&c->n.y, sizeof(float), 1, fp);
		fread(&c->n.z, sizeof(float), 1, fp);
		c->p = sm * c->p;
	}
	fclose(fp);

	

	ds::VertexBufferInfo kvbInfo = { ds::BufferType::STATIC, total, sizeof(AmbientVertex), e->vertices };
	RID kvbid = ds::createVertexBuffer(kvbInfo);

	RID nextGroup = ds::StateGroupBuilder()
		.vertexBuffer(kvbid)
		.build();

	ds::DrawCommand nextDrawCmd = { total, ds::DrawType::DT_VERTICES, ds::PrimitiveTypes::TRIANGLE_LIST };
	RID groups[] = { baseGroup, nextGroup };
	e->drawItem = ds::compile(nextDrawCmd, groups, 2);
}

void render_draw_item(const ds::vec3& p, CubeConstantBuffer* constantBuffer, RID pass, const Entity& e) {
	ds::matrix w = ds::matTranslate(p);
	constantBuffer->worldMatrix = ds::matTranspose(w);
	ds::submit(pass, e.drawItem);
}
// ---------------------------------------------------------------
// main method
// ---------------------------------------------------------------
int run() {
	
	CubeConstantBuffer constantBuffer;
	float t = 0.0f;

	// prepare application
	ds::RenderSettings rs;
	rs.width = 1024;
	rs.height = 768;
	rs.title = "Spinning cube";
	rs.clearColor = ds::Color(0.2f, 0.2f, 0.2f, 1.0f);
	rs.multisampling = 4;
	rs.useGPUProfiling = false;
	rs.supportDebug = true;
	ds::init(rs);

	LightBuffer lightBuffer;
	lightBuffer.ambientColor = ds::Color(0.15f, 0.15f, 0.15f, 1.0f);
	lightBuffer.diffuseColor = ds::Color(1.0f, 1.0f, 1.0f, 1.0f);
	lightBuffer.padding = 0.0f;
	ds::vec3 lightPos = ds::vec3(1.0f, -0.5f, 1.0f);
	lightBuffer.lightDirection = normalize(lightPos);

	RID lightBufferID = ds::createConstantBuffer(sizeof(LightBuffer), &lightBuffer);

	ds::BlendStateInfo bsInfo = { ds::BlendStates::SRC_ALPHA, ds::BlendStates::SRC_ALPHA, ds::BlendStates::INV_SRC_ALPHA, ds::BlendStates::INV_SRC_ALPHA, true };
	RID bs_id = ds::createBlendState(bsInfo);

	ds::ShaderInfo avsInfo = { 0, AmbientLightning_VS_Main, sizeof(AmbientLightning_VS_Main), ds::ShaderType::ST_VERTEX_SHADER };
	RID ambientVertexShader = ds::createShader(avsInfo);
	ds::ShaderInfo apsInfo = { 0, AmbientLightning_PS_Main, sizeof(AmbientLightning_PS_Main), ds::ShaderType::ST_PIXEL_SHADER };
	RID ambientPixelShader = ds::createShader(apsInfo);

	ds::matrix viewMatrix = ds::matLookAtLH(ds::vec3(0.0f, 4.0f, -6.0f), ds::vec3(0, 0, 0), ds::vec3(0, 1, 0));
	ds::matrix projectionMatrix = ds::matPerspectiveFovLH(ds::PI / 4.0f, ds::getScreenAspectRatio(), 0.01f, 100.0f);
	ds::Camera camera = {
		viewMatrix,
		projectionMatrix,
		viewMatrix * projectionMatrix,
		ds::vec3(0,4,-6),
		ds::vec3(0,0,1),
		ds::vec3(0,1,0),
		ds::vec3(1,0,0)
	};

	FPSCamera fpsCamera(&camera);
	fpsCamera.setPosition(ds::vec3(0, 1, -6));
	fpsCamera.buildView();

	


	ds::RenderPassInfo rpInfo = { &camera,ds::DepthBufferState::ENABLED, 0, 0 };
	RID basicPass = ds::createRenderPass(rpInfo);

	Grid grid;
	grid.create(10, basicPass);

	// create buffer input layout
	ds::InputLayoutDefinition ambientDecl[] = {
		{ "POSITION", 0, ds::BufferAttributeType::FLOAT3 },
		{ "COLOR"   , 0, ds::BufferAttributeType::FLOAT4 },
		{ "NORMAL"   , 0, ds::BufferAttributeType::FLOAT3 }
	};

	ds::InputLayoutInfo ambientLayoutInfo = { ambientDecl, 3, ambientVertexShader };
	RID arid = ds::createInputLayout(ambientLayoutInfo);

	RID cbid = ds::createConstantBuffer(sizeof(CubeConstantBuffer), &constantBuffer);

	RID nextGroup = ds::StateGroupBuilder()
		.inputLayout(arid)
		.constantBuffer(cbid, ambientVertexShader, 0)
		.constantBuffer(lightBufferID, ambientPixelShader, 0)
		.blendState(bs_id)
		.vertexShader(ambientVertexShader)
		.pixelShader(ambientPixelShader)
		.build();

	Entity barrel;
	load_entity(&barrel, "..\\..\\..\\obj_converter\\barrel.bin", nextGroup);

	Entity highTile;
	load_entity(&highTile, "..\\..\\..\\obj_converter\\frameHighTile.bin", nextGroup);



	while (ds::isRunning()) {
		ds::begin();

		fpsCamera.update(static_cast<float>(ds::getElapsedSeconds()));

		grid.render(&camera);

		// move cube
		t += static_cast<float>(ds::getElapsedSeconds());
		//ds::vec3 cp = ds::vec3(cos(t), 0.0f, sin(t));
		ds::matrix bY = ds::matRotationY(t);
		//float scale = 0.7f + sin(t) * 0.3f;
		float scale = 1.0f;// 0.15f;
		ds::matrix s = ds::matScale(ds::vec3(scale));
		ds::matrix w = bY * s;

		constantBuffer.viewprojectionMatrix = ds::matTranspose(camera.viewProjectionMatrix);
		constantBuffer.worldMatrix = ds::matTranspose(w);

		//ds::submit(basicPass, drawItem);

		//w = ds::matIdentity();
		//constantBuffer.worldMatrix = ds::matTranspose(w);
		//ds::submit(basicPass, nextDrawItem);

		//ds::submit(basicPass, barrel.drawItem);
		render_draw_item(ds::vec3(-1.0f, 0.0f, 0.0f), &constantBuffer, basicPass, barrel);
		render_draw_item(ds::vec3(1.0f, 0.0f, 0.0f), &constantBuffer, basicPass, highTile);

		//ds::submit(basicPass, highTile.drawItem);


		ds::dbgPrint(0, 0, "FPS: %d", ds::getFramesPerSecond());
		ds::dbgPrint(0, 1, "Simple spinning cube demo");

		ds::end();
	}
	ds::shutdown();
	return 0;
}