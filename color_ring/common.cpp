#include "common.h"
#include <SpriteBatchBuffer.h>
#include "particles\ParticleManager.h"

void prepareRenderEnvironment(RenderEnvironment* env, RID vertexShader, RID pixelShader, int numVertices) {

	env->blendState = ds::createBlendState(ds::BlendStateDesc()
		.SrcBlend(ds::BlendStates::SRC_ALPHA)
		.SrcAlphaBlend(ds::BlendStates::SRC_ALPHA)
		.DestBlend(ds::BlendStates::INV_SRC_ALPHA)
		.DestAlphaBlend(ds::BlendStates::INV_SRC_ALPHA)
		.AlphaEnabled(true)
		);

	ds::InputLayoutDefinition decl[] = {
		{ "POSITION", 0, ds::BufferAttributeType::FLOAT3 },
		{ "TEXCOORD", 0, ds::BufferAttributeType::FLOAT2 },
		{ "COLOR"   , 0, ds::BufferAttributeType::FLOAT4 }
	};

	env->inputLayout = ds::createInputLayout(ds::InputLayoutDesc()
		.Declarations(decl)
		.NumDeclarations(3)
		.VertexShader(vertexShader)
		);

	// create orthographic view
	env->orthoView = ds::matIdentity();
	env->orthoProjection = ds::matOrthoLH(ds::getScreenWidth(), ds::getScreenHeight(), 0.1f, 1.0f);
	env->camera = {
		env->orthoView,
		env->orthoProjection,
		env->orthoView * env->orthoProjection,
		ds::vec3(0,3,-6),
		ds::vec3(0,0,1),
		ds::vec3(0,1,0),
		ds::vec3(1,0,0),
		0.0f,
		0.0f,
		0.0f
	};

	env->viewport = ds::createViewport(ds::ViewportDesc()
		.Top(0)
		.Left(0)
		.Width(1024)
		.Height(768)
		.MinDepth(0.0f)
		.MaxDepth(1.0f)
		);

	env->orthoPass = ds::createRenderPass(ds::RenderPassDesc()
		.Camera(&env->camera)
		.DepthBufferState(ds::DepthBufferState::DISABLED)
		.Viewport(env->viewport)
		);

	env->basicConstantBuffer.viewprojectionMatrix = ds::matTranspose(env->orthoView * env->orthoProjection);
	env->basicConstantBuffer.worldMatrix = ds::matTranspose(ds::matIdentity());
	env->basicConstantBuffer.center = ds::vec4(512.0f, 368.0f, 0.0f, 0.0f);

	env->basicConstantBufferId = ds::createConstantBuffer(sizeof(BasicConstantBuffer), &env->basicConstantBuffer);
	env->indexBuffer = ds::createQuadIndexBuffer(numVertices / 4);

	env->samplerState = ds::createSamplerState(ds::SamplerStateDesc()
		.AddressMode(ds::TextureAddressModes::CLAMP)
		.Filter(ds::TextureFilters::LINEAR)
		);

	env->baseStateGroup = ds::StateGroupBuilder()
		.inputLayout(env->inputLayout)
		.vertexShader(vertexShader)
		.pixelShader(pixelShader)
		.texture(env->textureId, pixelShader, 0)
		.constantBuffer(env->basicConstantBufferId, vertexShader, 0)
		.samplerState(env->samplerState, pixelShader, 0)
		.indexBuffer(env->indexBuffer)
		.build();

	env->sprites = new SpriteBatchBuffer(SpriteBatchDesc()
		.MaxSprites(2048)
		.Texture(env->textureId)
		);

	env->particles = new ParticleManager(env->textureId);
	env->spriteArray = new SpriteArray;
	sprites::initialize(*env->spriteArray, 4096);
}