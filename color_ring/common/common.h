#pragma once
#include <diesel.h>

struct BasicConstantBuffer {
	ds::matrix viewprojectionMatrix;
	ds::matrix worldMatrix;
	ds::vec4 center;
};

class ParticleManager;
struct SpriteArray;
class SpriteBatchBuffer;

struct RenderEnvironment {
	RID blendState;
	RID inputLayout;
	ds::matrix orthoView;
	ds::matrix orthoProjection;
	ds::Camera camera;
	RID orthoPass;
	RID viewport;
	RID indexBuffer;
	RID samplerState;
	RID baseStateGroup;
	BasicConstantBuffer basicConstantBuffer;
	RID basicConstantBufferId;
	SpriteBatchBuffer* sprites;
	ParticleManager* particles;
	RID textureId;
	SpriteArray* spriteArray;
};

void prepareRenderEnvironment(RenderEnvironment* env, RID vertexShader, RID pixelShader, int numVertices);