#include "Scene.h"
#include <ds_imgui.h>
#include "..\..\shaders\Shadow_VS_Main.h"
#include "..\..\shaders\Shadow_PS_Main.h"

Scene::Scene() {
	_entitiesCapacity = 32;
	_entities = new Entity[_entitiesCapacity];
	_numEntities = 0;	
	_numInstances = 0;
	_instancesCapacity = 64;
	_instances = new EntityInstance[_instancesCapacity];

	_lightDirection = normalize(ds::vec3(-0.57735f, -0.57735f, 0.57735f));
	float sceneRadius = 8.0f;
	_lightPosition = -0.1f * sceneRadius * _lightDirection;

	_depthMap = new DepthMap(1024);
	_depthMap->buildView(_lightPosition, _lightDirection);

	ds::BlendStateInfo bsInfo = { ds::BlendStates::SRC_ALPHA, ds::BlendStates::SRC_ALPHA, ds::BlendStates::INV_SRC_ALPHA, ds::BlendStates::INV_SRC_ALPHA, true };
	RID bs_id = ds::createBlendState(bsInfo);

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

	_matrixBuffer.worldMatrix = ds::matTranspose(ds::matIdentity());
	_matrixBuffer.viewMatrix = ds::matTranspose(viewMatrix);
	_matrixBuffer.projectionMatrix = ds::matTranspose(projectionMatrix);
	_matrixBuffer.shadowTransform = ds::matTranspose(_depthMap->getShadowTransform());
	RID matrixBufferID = ds::createConstantBuffer(sizeof(MatrixBuffer), &_matrixBuffer);

	
	_lightBuffer.ambientColor = ds::Color(0.15f, 0.15f, 0.15f, 1.0f);
	_lightBuffer.diffuseColor = ds::Color(1.0f, 1.0f, 1.0f, 1.0f);
	_lightBuffer.lightPosition = _lightPosition;
	_lightBuffer.lightDirection = _lightDirection;
	_lightBuffer.padding = 0.0f;
	_lightBuffer.more = 0.0f;
	RID lightBufferID = ds::createConstantBuffer(sizeof(LightBuffer), &_lightBuffer);

	//FPSCamera fpsCamera(&camera);
	//fpsCamera.setPosition(ds::vec3(0, 4, -6), ds::vec3(0, 0, 0));
	//fpsCamera.buildView();

	ds::ViewportInfo vpInfo = { 1024, 768, 0.0f, 1.0f };
	RID vp = ds::createViewport(vpInfo);
	ds::RenderPassInfo rpInfo = { 0, vp, ds::DepthBufferState::ENABLED, 0, 0 };
	_basicPass = ds::createRenderPass(rpInfo);

	//RID cbid = ds::createConstantBuffer(sizeof(CubeConstantBuffer), &constantBuffer);


	_baseGroup = ds::StateGroupBuilder()
		.inputLayout(arid)
		.constantBuffer(matrixBufferID, shadowVertexShader, 0)
		.constantBuffer(lightBufferID, shadowVertexShader, 1)
		.blendState(bs_id)
		.textureFromRenderTarget(_depthMap->getRenderTarget(), shadowPixelShader, 0)
		.vertexShader(shadowVertexShader)
		.pixelShader(shadowPixelShader)
		.build();

	_rtViewer = new RenderTextureViewer(_depthMap->getRenderTarget(), 200);
}

Scene::~Scene() {
	delete _depthMap;
	if (_entities != 0) {
		delete[] _entities;
	}
	if (_instances != 0) {
		delete[] _instances;
	}
}

int Scene::loadEntity(const char* fileName) {
	Entity* e = &_entities[_numEntities++];
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

	char tmp[256];
	sprintf_s(tmp, "%s-Main", fileName);

	ds::DrawCommand nextDrawCmd = { total, ds::DrawType::DT_VERTICES, ds::PrimitiveTypes::TRIANGLE_LIST };
	RID groups[] = { _baseGroup, nextGroup };
	e->drawItem = ds::compile(nextDrawCmd, groups, 2,tmp);


	sprintf_s(tmp, "%s-Depth", fileName);
	RID dgroups[] = { _depthMap->getDepthBaseGroup(), myDepthGroup };
	e->depthDrawItem = ds::compile(nextDrawCmd, dgroups, 2, tmp);

	return _numEntities - 1;
}

int Scene::createGrid(int numCells) {
	Entity* e = &_entities[_numEntities++];
	float uvMax = static_cast<float>(numCells);
	ds::vec2 uvs[] = { ds::vec2(0.0f,1.0f),ds::vec2(0.0f,0.0f),ds::vec2(1.0f,0.0f),ds::vec2(1.0f,1.0f) };
	ds::vec3 positions[] = { ds::vec3(-0.5f,-0.0005f,-0.5f),ds::vec3(-0.5f,-0.0005f,0.5f) ,ds::vec3(0.5f,-0.0005f,0.5f) ,ds::vec3(0.5f,-0.0005f,-0.5f) };
	int num = numCells * numCells * 4 / 6;
	e->vertices = new AmbientVertex[numCells * numCells * 4];
	float sz = numCells / 2.0f - 0.5f;
	float radius = 0.0f;
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

				float ds = sqr_distance(ds::vec3(0.0f), e->vertices[idx + j].p);
				if (ds > radius * radius) {
					radius = ds;
				}
			}
			sx -= 1.0f;
		}
		sz -= 1.0f;
	}

	radius = sqrtf(radius);
	e->radius = radius;

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
	RID groups[] = { _baseGroup, nextGroup };
	e->drawItem = ds::compile(nextDrawCmd, groups, 2, "GridMain");

	RID dgroups[] = { _depthMap->getDepthBaseGroup(), myDepthGroup };
	e->depthDrawItem = ds::compile(nextDrawCmd, dgroups, 2, "GridDepth");

	return _numEntities - 1;
}

int Scene::createInstance(int entityID, const ds::vec3& pos) {
	EntityInstance* inst = &_instances[_numInstances++];
	inst->castShadows = true;
	inst->pos = pos;
	inst->rotation = ds::vec3(0.0f);
	inst->scale = ds::vec3(0.0f);
	inst->entity = &_entities[entityID];
	return _numInstances - 1;
}

// -------------------------------------------------------------
// render scene to depth buffer
// -------------------------------------------------------------
void Scene::renderDepthMap() {
	for (int i = 0; i < _numInstances; ++i) {
		const EntityInstance& inst = _instances[i];
		if (inst.castShadows) {
			_depthMap->render(inst.pos, inst.entity->depthDrawItem);
		}
	}
}

void Scene::renderMain() {
	/*
	matrixBuffer.viewMatrix = ds::matTranspose(camera.viewMatrix);
	matrixBuffer.worldMatrix = ds::matTranspose(w);
	render_draw_item(ds::vec3(0.0f, 0.0f, 0.0f), &matrixBuffer, basicPass, grid);
	*/
}

void Scene::renderDebug() {

	_rtViewer->render(_basicPass);
	/*
	gui::start();
	p2i sp = p2i(10, 760);
	if (gui::begin("Debug", &dialogState, &sp, 400)) {
		gui::Value("FPS", ds::getFramesPerSecond());
		float ti = 1.0f / ds::getFramesPerSecond() * 1000.0f;
		gui::Value("TI", ti, 3, 4);
		gui::Input("Light Direction", &_lightDirection);
		gui::Input("Light Pos", &_lightPos);
	}
	gui::end();
	*/
}

	