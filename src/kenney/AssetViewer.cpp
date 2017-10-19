#include <diesel.h>
#include <ds_imgui.h>
#include "..\..\shaders\AmbientLightning_VS_Main.h"
#include "..\..\shaders\AmbientLightning_PS_Main.h"
#include "Camera.h"
#include "Grid.h"
#include "..\..\shaders\Depth_VS_Main.h"
#include "..\..\shaders\Depth_PS_Main.h"
#include "..\..\shaders\Shadow_VS_Main.h"
#include "..\..\shaders\Shadow_PS_Main.h"
#include "RTViewer.h"
#include "DepthMap.h"

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
	RID depthDrawItem;
};

struct MatrixBuffer {
	ds::matrix worldMatrix;
	ds::matrix viewMatrix;
	ds::matrix projectionMatrix;
	ds::matrix shadowTransform;
};

struct NewLightBuffer {
	ds::Color ambientColor;
	ds::Color diffuseColor;
};

struct LightBuffer2 {
	ds::vec3 lightPosition;
	float padding;
};

void create_grid(Entity* e, int numCells, RID baseGroup, RID depthGroup) {
	float uvMax = static_cast<float>(numCells);
	ds::vec2 uvs[] = { ds::vec2(0.0f,1.0f),ds::vec2(0.0f,0.0f),ds::vec2(1.0f,0.0f),ds::vec2(1.0f,1.0f) };
	ds::vec3 positions[] = { ds::vec3(-0.5f,-0.0005f,-0.5f),ds::vec3(-0.5f,-0.0005f,0.5f) ,ds::vec3(0.5f,-0.0005f,0.5f) ,ds::vec3(0.5f,-0.0005f,-0.5f) };
	int num = numCells * numCells * 4 / 6;
	e->vertices = new AmbientVertex[numCells * numCells * 4];
	float sz = numCells / 2.0f - 0.5f;
	for (int z = 0; z < numCells; ++z) {
		float sx = numCells / 2.0f - 0.5f;
		for (int x = 0; x < numCells; ++x) {
			int idx = z * numCells * 4 + x * 4;
			for (int j = 0; j < 4; ++j) {
				e->vertices[idx + j].p = positions[j];
				e->vertices[idx + j].p.x += sx;
				e->vertices[idx + j].p.z += sz;
				e->vertices[idx + j].color = ds::Color(0.2f, 0.2f, 0.2f, 1.0f);
				e->vertices[idx + j].n = ds::vec3(0, 1, 0);
			}
			sx -= 1.0f;
		}
		sz -= 1.0f;
	}

	RID indexBuffer = ds::createQuadIndexBuffer(numCells * numCells, "GridIndexBuffer");
	ds::VertexBufferInfo vbInfo = { ds::BufferType::STATIC, numCells * numCells * 4, sizeof(AmbientVertex), e->vertices };
	RID kvbid = ds::createVertexBuffer(vbInfo);

	RID nextGroup = ds::StateGroupBuilder()
		.vertexBuffer(kvbid)
		.indexBuffer(indexBuffer)
		.build();

	RID myDepthGroup = ds::StateGroupBuilder()
		.vertexBuffer(kvbid)
		.indexBuffer(indexBuffer)
		.build();

	ds::DrawCommand nextDrawCmd = { numCells * numCells * 6, ds::DrawType::DT_INDEXED, ds::PrimitiveTypes::TRIANGLE_LIST };
	RID groups[] = { baseGroup, nextGroup };
	e->drawItem = ds::compile(nextDrawCmd, groups, 2);

	RID dgroups[] = { depthGroup, myDepthGroup };
	e->depthDrawItem = ds::compile(nextDrawCmd, dgroups, 2);

}

void load_entity(Entity* e, const char* fileName, RID baseGroup, RID depthGroup) {
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

	RID myDepthGroup = ds::StateGroupBuilder()
		.vertexBuffer(kvbid)
		.build();

	ds::DrawCommand nextDrawCmd = { total, ds::DrawType::DT_VERTICES, ds::PrimitiveTypes::TRIANGLE_LIST };
	RID groups[] = { baseGroup, nextGroup };
	e->drawItem = ds::compile(nextDrawCmd, groups, 2);

	RID dgroups[] = { depthGroup, myDepthGroup };
	e->depthDrawItem = ds::compile(nextDrawCmd, dgroups, 2);
}

void render_draw_item(const ds::vec3& p, MatrixBuffer* constantBuffer, RID pass, const Entity& e) {
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

	gui::init();

	ds::vec3 lightDirection = normalize(ds::vec3( 0.0f,-2.0f,2.0f));
	//ds::vec3 lightDirection = normalize(ds::vec3(-0.57735f, -0.57735f, 0.57735f));
	float sceneRadius = 8.0f;
	ds::vec3 lightPos = -0.1f * sceneRadius * lightDirection;

	LightBuffer lightBuffer;
	lightBuffer.ambientColor = ds::Color(0.05f, 0.05f, 0.05f, 1.0f);
	lightBuffer.diffuseColor = ds::Color(1.0f, 1.0f, 1.0f, 1.0f);
	lightBuffer.padding = 0.0f;
	RID lightBufferID = ds::createConstantBuffer(sizeof(LightBuffer), &lightBuffer);

	ds::BlendStateInfo bsInfo = { ds::BlendStates::SRC_ALPHA, ds::BlendStates::SRC_ALPHA, ds::BlendStates::INV_SRC_ALPHA, ds::BlendStates::INV_SRC_ALPHA, true };
	RID bs_id = ds::createBlendState(bsInfo);

	//ds::ShaderInfo avsInfo = { 0, AmbientLightning_VS_Main, sizeof(AmbientLightning_VS_Main), ds::ShaderType::ST_VERTEX_SHADER };
	//RID ambientVertexShader = ds::createShader(avsInfo);
	//ds::ShaderInfo apsInfo = { 0, AmbientLightning_PS_Main, sizeof(AmbientLightning_PS_Main), ds::ShaderType::ST_PIXEL_SHADER };
	//RID ambientPixelShader = ds::createShader(apsInfo);
	
	ds::ShaderInfo svsInfo = { 0, Shadow_VS_Main, sizeof(Shadow_VS_Main), ds::ShaderType::ST_VERTEX_SHADER };
	RID shadowVertexShader = ds::createShader(svsInfo);
	ds::ShaderInfo spsInfo = { 0, Shadow_PS_Main, sizeof(Shadow_PS_Main), ds::ShaderType::ST_PIXEL_SHADER };
	RID shadowPixelShader = ds::createShader(spsInfo);

	ds::matrix viewMatrix = ds::matLookAtLH(ds::vec3(0.0f, 4.0f, -6.0f), ds::vec3(0, 0, 1), ds::vec3(0, 1, 0));
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

	// create buffer input layout
	ds::InputLayoutDefinition ambientDecl[] = {
		{ "POSITION", 0, ds::BufferAttributeType::FLOAT3 },
		{ "COLOR"   , 0, ds::BufferAttributeType::FLOAT4 },
		{ "NORMAL"   , 0, ds::BufferAttributeType::FLOAT3 }
	};

	ds::InputLayoutInfo ambientLayoutInfo = { ambientDecl, 3, shadowVertexShader };
	RID arid = ds::createInputLayout(ambientLayoutInfo);

	DepthMap depthMap(2048, arid);
	depthMap.buildView(lightPos, lightDirection);

	MatrixBuffer matrixBuffer;
	matrixBuffer.worldMatrix = ds::matTranspose(ds::matIdentity());
	matrixBuffer.viewMatrix = ds::matTranspose(viewMatrix);
	matrixBuffer.projectionMatrix = ds::matTranspose(projectionMatrix);
	matrixBuffer.shadowTransform = ds::matTranspose(depthMap.getShadowTransform());
	RID matrixBufferID = ds::createConstantBuffer(sizeof(MatrixBuffer), &matrixBuffer);

	NewLightBuffer newLightBuffer;
	newLightBuffer.ambientColor = ds::Color(0.15f, 0.15f, 0.15f, 1.0f);
	newLightBuffer.diffuseColor = ds::Color(1.0f, 1.0f, 1.0f, 1.0f);
	RID newLightBufferID = ds::createConstantBuffer(sizeof(NewLightBuffer), &newLightBuffer);

	LightBuffer2 lightBuffer2;
	lightBuffer2.lightPosition = lightPos;
	lightBuffer2.padding = 0.0f;
	RID lightBuffer2ID = ds::createConstantBuffer(sizeof(LightBuffer2), &lightBuffer2);

	FPSCamera fpsCamera(&camera);
	fpsCamera.setPosition(ds::vec3(0, 4, -6), ds::vec3(0, 0, 0));
	fpsCamera.buildView();

	ds::ViewportInfo vpInfo = { 1024, 768, 0.0f, 1.0f };
	RID vp = ds::createViewport(vpInfo);
	ds::RenderPassInfo rpInfo = { &camera, vp, ds::DepthBufferState::ENABLED, 0, 0 };
	RID basicPass = ds::createRenderPass(rpInfo);

	RID cbid = ds::createConstantBuffer(sizeof(CubeConstantBuffer), &constantBuffer);

	
	RID shadowGroup = ds::StateGroupBuilder()
		.inputLayout(arid)
		.constantBuffer(matrixBufferID, shadowVertexShader, 0)
		.constantBuffer(lightBuffer2ID, shadowVertexShader, 1)
		.constantBuffer(newLightBufferID, shadowPixelShader, 0)
		.blendState(bs_id)
		.textureFromRenderTarget(depthMap.getRenderTarget(), shadowPixelShader,0)
		.vertexShader(shadowVertexShader)
		.pixelShader(shadowPixelShader)
		.build();

	Entity barrel;
	load_entity(&barrel, "..\\obj_converter\\barrel.bin", shadowGroup, depthMap.getDepthBaseGroup());

	Entity grid;
	create_grid(&grid, 10, shadowGroup, depthMap.getDepthBaseGroup());

	//Entity highTile;
	//load_entity(&highTile, "..\\obj_converter\\groundTile.bin", shadowGroup, depthGroup);

	//Entity crater;
	//load_entity(&crater, "..\\obj_converter\\crater.bin", shadowGroup, depthGroup);

	RenderTextureViewer rtViewer(depthMap.getRenderTarget(), 200);

	int dialogState = 1;

	while (ds::isRunning()) {
		ds::begin();

		fpsCamera.update(static_cast<float>(ds::getElapsedSeconds()));
		ds::matrix w = ds::matIdentity();// bY * s;
		
		float sx = -3.0f;
		for (int i = 0; i < 4; ++i) {
			depthMap.render(ds::vec3(sx, 0.0f, 0.0f), barrel.drawItem);
			sx += 1.5f;
		}

		matrixBuffer.viewMatrix = ds::matTranspose(camera.viewMatrix);
		matrixBuffer.worldMatrix = ds::matTranspose(w);
		render_draw_item(ds::vec3(0.0f, 0.0f, 0.0f), &matrixBuffer, basicPass, grid);

		sx = -3.0f;
		for (int i = 0; i < 4; ++i) {
			render_draw_item(ds::vec3(sx, 0.0f, 0.0f), &matrixBuffer, basicPass, barrel);
			sx += 1.5f;
		}
		
		rtViewer.render(basicPass);

		//ds::dbgPrint(0, 0, "FPS: %d", ds::getFramesPerSecond());
		//ds::dbgPrint(0, 1, "Simple spinning cube demo");

		gui::start();
		p2i sp = p2i(10, 760);
		if (gui::begin("Debug", &dialogState, &sp, 400)) {
			gui::Value("FPS", ds::getFramesPerSecond());
			float ti = 1.0f / ds::getFramesPerSecond() * 1000.0f;
			gui::Value("TI", ti, 3, 4);
			gui::Input("Light Direction", &lightDirection);
		}
		gui::end();

		ds::end();
	}
	ds::shutdown();
	return 0;
}