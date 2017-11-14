#include "InstanceTest.h"
#include "..\Mesh.h"
#include "..\..\shaders\InstancedAmbient_VS_Main.h"
#include "..\..\shaders\InstancedAmbient_PS_Main.h"
#include <ds_imgui.h>

InstanceTest::InstanceTest() {

}

InstanceTest::~InstanceTest() {

}

ds::RenderSettings InstanceTest::getRenderSettings() {
	ds::RenderSettings rs;
	rs.width = 1024;
	rs.height = 768;
	rs.title = "Instancing";
	rs.clearColor = ds::Color(0.1f, 0.1f, 0.1f, 1.0f);
	rs.multisampling = 4;
	rs.useGPUProfiling = false;
	rs.supportDebug = true;
	
	return rs;
}

bool InstanceTest::init() {
	ds::matrix viewMatrix = ds::matLookAtLH(ds::vec3(0.0f, 0.0f, -3.0f), ds::vec3(0, 0, 0), ds::vec3(0, 1, 0));
	ds::matrix projectionMatrix = ds::matPerspectiveFovLH(ds::PI / 4.0f, ds::getScreenAspectRatio(), 0.01f, 100.0f);
	_camera = {
		viewMatrix,
		projectionMatrix,
		viewMatrix * projectionMatrix,
		ds::vec3(0,0,-3),
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

	_constantBuffer.mvp = ds::matTranspose(_camera.viewProjectionMatrix);
	_constantBuffer.world = ds::matTranspose(ds::matIdentity());
	
	RID cbid = ds::createConstantBuffer(sizeof(InstanceBuffer), &_constantBuffer);

	_lightBuffer.ambientColor = ds::Color(0.1f, 0.1f, 0.1f, 1.0f);
	_lightBuffer.diffuseColor = ds::Color(1.0f, 1.0f, 1.0f, 1.0f);
	_lightBuffer.lightDirection = ds::vec3(0.0f, 0.0f, 1.0f);
	_lightBuffer.padding = 0.0f;
	RID lbid = ds::createConstantBuffer(sizeof(InstanceLightBuffer), &_lightBuffer);

	int TOTAL = 256;

	Mesh mesh;
	mesh.loadBin("models\\griddy.bin");
	RID cubeBuffer = mesh.assemble();

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
	ds::VertexBufferInfo ibInfo = { ds::BufferType::DYNAMIC, TOTAL, sizeof(InstanceData) };
	_instanceVertexBuffer = ds::createVertexBuffer(ibInfo);
	RID instanceBuffer = ds::createInstancedBuffer(cubeBuffer, _instanceVertexBuffer);

	_fpsCamera = new FPSCamera(&_camera);
	_fpsCamera->setPosition(ds::vec3(0, 0, -5), ds::vec3(0.0f, 0.0f, 0.0f));

	RID stateGroup = ds::StateGroupBuilder()
		.inputLayout(rid)
		.constantBuffer(cbid, bumpVS, 0)
		.constantBuffer(lbid, bumpPS, 0)
		.blendState(bs_id)
		.instancedVertexBuffer(instanceBuffer)
		.vertexShader(bumpVS)
		.pixelShader(bumpPS)
		.build();


	ds::DrawCommand drawCmd = { mesh.getCount(), ds::DrawType::DT_INSTANCED, ds::PrimitiveTypes::TRIANGLE_LIST, TOTAL };

	_drawItem = ds::compile(drawCmd, stateGroup);

	_numItems = 0;
	for (int i = 0; i < 10; ++i) {
		GridItem& item = _items[_numItems++];
		item.color = ds::Color(1.0f, 0.0f, 0.0f, 1.0f);
		item.type = 0;
		item.pos = ds::vec3(-1.0f + i * 0.3f, 0.0f, 0.0f);
		item.timer = 0.0f;
		item.angle = ds::random(0.0f, ds::TWO_PI);
		item.velocity = ds::vec3(1.2f * cosf(item.angle), 1.2f * sinf(item.angle), 0.0f);
		item.animationFlags = 1;
		item.scale = 0.15f;
		item.numAnimations = 0;
		Animation& anim = item.animations[item.numAnimations++];
		anim.timer = 0.0f;
		anim.ttl = 0.6f;
		anim.type = 1;
	}

	_scalePath.add(0.0f, 0.05f);
	_scalePath.add(0.4f, 0.25f);
	_scalePath.add(0.6f, 0.15f);
	_scalePath.add(0.8f, 0.25f);
	_scalePath.add(1.0f, 0.15f);
	return true;
}

void InstanceTest::tick(float dt) {
	_fpsCamera->update(dt);
	for (int y = 0; y < _numItems; ++y) {
		GridItem& item = _items[y];
		item.timer += dt;
		/*
		item.pos += item.velocity * ds::getElapsedSeconds();
		if (item.pos.x < -2.0f || item.pos.x > 2.0f) {
			item.velocity.x *= -1.0f;
			item.angle += ds::PI;
		}
		if (item.pos.y < -2.0f || item.pos.y > 2.0f) {
			item.velocity.y *= -1.0f;
			item.angle += ds::PI;
		}
		*/
		for (int i = 0; i < item.numAnimations; ++i) {
			Animation& anim = item.animations[i];
			if (anim.timer <= anim.ttl) {
				anim.timer += dt;
				if (anim.type == 1) {
					float n = anim.timer / anim.ttl;
					if (n > 1.0f) {
						n = 1.0f;
					}
					item.scale = _scalePath.get(n);					
				}
			}			
		}
	}
}

void InstanceTest::render() {
	for (int y = 0; y < _numItems; ++y) {
		GridItem& item = _items[y];
		if (item.type == 0) {
			ds::matrix pm = ds::matTranslate(item.pos);			
			ds::matrix rx = ds::matRotationX(item.timer * 4.0f);
			ds::matrix ry = ds::matRotationY(item.angle);
			ds::matrix sm = ds::matScale(ds::vec3(item.scale, item.scale, item.scale));
			ds::matrix world = ry * rx * sm * pm;
			_instances[y] = { ds::matTranspose(world), item.color };
		}
		else {
			ds::matrix rx = ds::matRotationX(-ds::PI * 0.5f);
			ds::matrix ry = ds::matRotationY(item.angle);
			ds::matrix pm = ds::matTranslate(item.pos);
			ds::matrix world = rx * pm;
			_instances[y] = { ds::matTranspose(world), item.color };
		}

	}
	// map the instance data
	ds::mapBufferData(_instanceVertexBuffer, _instances, sizeof(InstanceData) * _numItems);
	_constantBuffer.mvp = ds::matTranspose(_camera.viewProjectionMatrix);
	_constantBuffer.world = ds::matTranspose(ds::matIdentity());
	ds::submit(_basicPass, _drawItem);

	ds::dbgPrint(0, 0, "FPS: %d", ds::getFramesPerSecond());
}

void InstanceTest::renderGUI() {
	int state = 1;
	gui::start();
	p2i sp = p2i(10, 760);
	if (gui::begin("Debug", &state, &sp, 300)) {
		gui::Value("FPS", ds::getFramesPerSecond());
		if (gui::Button("Scale")) {
			_items[5].animations[0].timer = 0.0f;
		}
	}
	gui::end();
}