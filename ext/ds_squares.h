#pragma once
#include <diesel.h>
//#define QUAD_BATCH_IMPLEMENTATION

struct QuadBatchConstantBuffer {
	ds::vec4 screenDimension;
	ds::vec4 screenCenter;
	ds::matrix wvp;
};

struct quad_texture {
	RID textureID;
	ds::vec2 textureSize;
};

struct vs_input {
	ds::vec3 pos;
	float tex[2];
	ds::Color color;
};

struct QuadBatchBufferInfo {

	unsigned int maxSprites;
	ds::TextureFilters textureFilter;
	RID blendState;
	RID vertexShader;
	RID pixelShader;
	RID constantBuffer;
};

class QuadBatchDesc {

public:
	QuadBatchDesc() {
		_info.maxSprites = 1024;
		_info.textureFilter = ds::TextureFilters::LINEAR;
		_info.blendState = NO_RID;
		_info.pixelShader = NO_RID;
		_info.vertexShader = NO_RID;
		_info.constantBuffer = NO_RID;
	}
	QuadBatchDesc& MaxSprites(int max) {
		_info.maxSprites = max;
		return *this;
	}
	QuadBatchDesc& TextureFilter(ds::TextureFilters filter) {
		_info.textureFilter = filter;
		return *this;
	}
	QuadBatchDesc& BlendState(RID blendState) {
		_info.blendState = blendState;
		return *this;
	}
	QuadBatchDesc& PixelShader(RID pixelShader) {
		_info.pixelShader = pixelShader;
		return *this;
	}
	QuadBatchDesc& VertexShader(RID vertexShader) {
		_info.vertexShader = vertexShader;
		return *this;
	}
	QuadBatchDesc& ConstantBuffer(RID constantBuffer) {
		_info.constantBuffer = constantBuffer;
		return *this;
	}
	const QuadBatchBufferInfo& getInfo() const {
		return _info;
	}
private:
	QuadBatchBufferInfo _info;
};

namespace quads {

	void initialize(const QuadBatchDesc& info);

	void begin();

	void flush();

	void draw(RID textureID, const ds::vec2* positions, const ds::vec4& textureRect, const ds::vec2& scale = ds::vec2(1.0f,1.0f), float rotation = 0.0f, const ds::Color& clr = ds::Color(255,255,255,255));

	void shutdown();

}

#ifdef QUAD_BATCH_IMPLEMENTATION
#include <vector>
#include <assert.h>

const BYTE Quad_VS_Main[] =
{
	68,  88,  66,  67, 190, 223, 60, 190, 237,  80, 102, 169, 230, 201,   8,  45,  12, 251,	233, 191,   1,   0,   0,   0,
	92,   2,   0,   0,   3,   0, 0,   0,  44,   0,   0,   0, 156,   0,   0,   0,  16,   1,	0,   0,  73,  83,  71,  78,
	104,   0,   0,   0,   3,   0, 0,   0,   8,   0,   0,   0, 80,   0,   0,   0,   0,   0, 0,   0,   0,   0,   0,   0,
	3,   0,   0,   0,   0,   0, 0,   0,   7,   3,   0,   0, 89,   0,   0,   0,   0,   0, 0,   0,   0,   0,   0,   0,
	3,   0,   0,   0,   1,   0, 0,   0,   3,   3,   0,   0, 98,   0,   0,   0,   0,   0, 0,   0,   0,   0,   0,   0,
	3,   0,   0,   0,   2,   0, 0,   0,  15,  15,   0,   0, 80,  79,  83,  73,  84,  73, 79,  78,   0,  84,  69,  88,
	67,  79,  79,  82,  68,   0, 67,  79,  76,  79,  82,   0, 79,  83,  71,  78, 108,   0, 0,   0,   3,   0,   0,   0,
	8,   0,   0,   0,  80,   0,	0,   0,   0,   0,   0,   0,	1,   0,   0,   0,   3,   0,	0,   0,   0,   0,   0,   0,
	15,   0,   0,   0,  92,   0,	0,   0,   0,   0,   0,   0,	0,   0,   0,   0,   3,   0,	0,   0,   1,   0,   0,   0,
	3,  12,   0,   0, 101,   0,	0,   0,   0,   0,   0,   0,	0,   0,   0,   0,   3,   0,	0,   0,   2,   0,   0,   0,
	15,   0,   0,   0,  83,  86,	95,  80,  79,  83,  73,  84,	73,  79,  78,   0,  84,  69,	88,  67,  79,  79,  82,  68,
	0,  67,  79,  76,  79,  82,	0, 171,  83,  72,  68,  82,	68,   1,   0,   0,  64,   0,	1,   0,  81,   0,   0,   0,
	89,   0,   0,   4,  70, 142,	32,   0,   0,   0,   0,   0,	6,   0,   0,   0,  95,   0,	0,   3,  50,  16,  16,   0,
	0,   0,   0,   0,  95,   0,	0,   3,  50,  16,  16,   0,	1,   0,   0,   0,  95,   0,	0,   3, 242,  16,  16,   0,
	2,   0,   0,   0, 103,   0,	0,   4, 242,  32,  16,   0,	0,   0,   0,   0,   1,   0,	0,   0, 101,   0,   0,   3,
	50,  32,  16,   0,   1,   0,	0,   0, 101,   0,   0,   3,	242,  32,  16,   0,   2,   0,	0,   0, 104,   0,   0,   2,
	1,   0,   0,   0,  54,   0,	0,   5,  66,  32,  16,   0,	0,   0,   0,   0,   1,  64,	0,   0,   0,   0,   0,   0,
	0,   0,   0,   9,  50,   0,	16,   0,   0,   0,   0,   0,	70,  16,  16,   0,   0,   0,	0,   0,  70, 128,  32, 128,
	65,   0,   0,   0,   0,   0,	0,   0,   1,   0,   0,   0,	54,   0,   0,   5,  66,   0,	16,   0,   0,   0,   0,   0,
	1,  64,   0,   0,   0,   0,	128,  63,  16,   0,   0,   8,	18,  32,  16,   0,   0,   0,	0,   0,  70,   2,  16,   0,
	0,   0,   0,   0,  70, 131,	32,   0,   0,   0,   0,   0,	2,   0,   0,   0,  16,   0,	0,   8,  34,  32,  16,   0,
	0,   0,   0,   0,  70,   2,	16,   0,   0,   0,   0,   0,	70, 131,  32,   0,   0,   0,	0,   0,   3,   0,   0,   0,
	16,   0,   0,   8, 130,  32,	16,   0,   0,   0,   0,   0,	70,   2,  16,   0,   0,   0,	0,   0,  70, 131,  32,   0,
	0,   0,   0,   0,   5,   0,	0,   0,  54,   0,   0,   5,	50,  32,  16,   0,   1,   0,	0,   0,  70,  16,  16,   0,
	1,   0,   0,   0,  54,   0,	0,   5, 242,  32,  16,   0,	2,   0,   0,   0,  70,  30,	16,   0,   2,   0,   0,   0,
	62,   0,   0,   1
};




const BYTE Quad_PS_Main[] =
{
	68,  88,  66,  67, 133, 123, 244, 109,  36, 101, 150, 228, 91, 135, 209, 221,  54, 143, 33,  28,   1,   0,   0,   0,
	112,   1,   0,   0,   3,   0, 0,   0,  44,   0,   0,   0, 160,   0,   0,   0, 212,   0, 0,   0,  73,  83,  71,  78,
	108,   0,   0,   0,   3,   0, 0,   0,   8,   0,   0,   0, 80,   0,   0,   0,   0,   0, 0,   0,   1,   0,   0,   0,
	3,   0,   0,   0,   0,   0, 0,   0,  15,   0,   0,   0, 92,   0,   0,   0,   0,   0, 0,   0,   0,   0,   0,   0,
	3,   0,   0,   0,   1,   0, 0,   0,   3,   3,   0,   0, 101,   0,   0,   0,   0,   0, 0,   0,   0,   0,   0,   0,
	3,   0,   0,   0,   2,   0, 0,   0,  15,  15,   0,   0,  83,  86,  95,  80,  79,  83, 73,  84,  73,  79,  78,   0,
	84,  69,  88,  67,  79,  79, 82,  68,   0,  67,  79,  76, 79,  82,   0, 171,  79,  83,  71,  78,  44,   0,   0,   0,
	1,   0,   0,   0,   8,   0,	0,   0,  32,   0,   0,   0,	0,   0,   0,   0,   0,   0,	0,   0,   3,   0,   0,   0,
	0,   0,   0,   0,  15,   0,	0,   0,  83,  86,  95,  84,	65,  82,  71,  69,  84,   0,	171, 171,  83,  72,  68,  82,
	148,   0,   0,   0,  64,   0,	0,   0,  37,   0,   0,   0,	90,   0,   0,   3,   0,  96,	16,   0,   0,   0,   0,   0,
	88,  24,   0,   4,   0, 112,	16,   0,   0,   0,   0,   0,	85,  85,   0,   0,  98,  16,	0,   3,  50,  16,  16,   0,
	1,   0,   0,   0,  98,  16,	0,   3, 242,  16,  16,   0,	2,   0,   0,   0, 101,   0,	0,   3, 242,  32,  16,   0,
	0,   0,   0,   0, 104,   0,	0,   2,   1,   0,   0,   0,	69,   0,   0,   9, 242,   0,	16,   0,   0,   0,   0,   0,
	70,  16,  16,   0,   1,   0,	0,   0,  70, 126,  16,   0,	0,   0,   0,   0,   0,  96,	16,   0,   0,   0,   0,   0,
	56,   0,   0,   7, 242,  32,	16,   0,   0,   0,   0,   0,	70,  14,  16,   0,   0,   0,	0,   0,  70,  30,  16,   0,
	2,   0,   0,   0,  62,   0,	0,   1
};


struct QuadDrawItem {
	RID textureId;
	RID drawItem;
};

struct QuadBatchContext {
	RID currentTexture;
	bool rendering;
	unsigned int max;
	unsigned int current;
	RID renderPass;
	RID vertexBufferId;
	RID basicGroup;
	QuadBatchConstantBuffer constantBuffer;
	std::vector<QuadDrawItem> drawItems;
	RID pixelShader;
	ds::vec2 screenCenter;
	ds::Camera camera;
	vs_input* vertices;
};

static QuadBatchContext* _qbCtx = 0;

namespace quads {

	void initialize(const QuadBatchDesc& desc) {
		_qbCtx = new QuadBatchContext;
		_qbCtx->current = 0;
		_qbCtx->rendering = false;
		const QuadBatchBufferInfo& info = desc.getInfo();
		_qbCtx->max = info.maxSprites;
		_qbCtx->vertices = new vs_input[info.maxSprites * 4];
		RID vertexShader = info.vertexShader;
		if (vertexShader == NO_RID) {
			vertexShader = ds::createShader(ds::ShaderDesc()
				.Data(Quad_VS_Main)
				.DataSize(sizeof(Quad_VS_Main))
				.ShaderType(ds::ShaderType::ST_VERTEX_SHADER)
				, "QuadVS"
			);
		}
		_qbCtx->pixelShader = info.pixelShader;
		if (_qbCtx->pixelShader == NO_RID) {
			_qbCtx->pixelShader = ds::createShader(ds::ShaderDesc()
				.Data(Quad_PS_Main)
				.DataSize(sizeof(Quad_PS_Main))
				.ShaderType(ds::ShaderType::ST_PIXEL_SHADER)
				, "QuadPS"
			);
		}
		RID bs_id = info.blendState;
		if (bs_id == NO_RID) {
			bs_id = ds::createBlendState(ds::BlendStateDesc()
				.SrcBlend(ds::BlendStates::SRC_ALPHA)
				.SrcAlphaBlend(ds::BlendStates::SRC_ALPHA)
				.DestBlend(ds::BlendStates::INV_SRC_ALPHA)
				.DestAlphaBlend(ds::BlendStates::INV_SRC_ALPHA)
				.AlphaEnabled(true)
			);
		}
		RID constantBuffer = info.constantBuffer;
		if (constantBuffer == NO_RID) {
			constantBuffer = ds::createConstantBuffer(sizeof(QuadBatchConstantBuffer), &_qbCtx->constantBuffer);
		}
		RID sSPID = createSamplerState(ds::SamplerStateDesc()
			.AddressMode(ds::TextureAddressModes::CLAMP)
			.Filter(info.textureFilter)
		);

		int indices[] = { 0,1,2,2,3,0 };
		RID idxBuffer = ds::createQuadIndexBuffer(_qbCtx->max, indices);

		_qbCtx->vertexBufferId = ds::createVertexBuffer(ds::VertexBufferDesc()
			.VertexSize(sizeof(vs_input))
			.NumVertices(_qbCtx->max * 4)
			.BufferType(ds::BufferType::DYNAMIC)
			);

		// create buffer input layout
		ds::InputLayoutDefinition decl[] = {
			{ "POSITION", 0, ds::BufferAttributeType::FLOAT3 },
			{ "TEXCOORD", 0, ds::BufferAttributeType::FLOAT2 },
			{ "COLOR", 0, ds::BufferAttributeType::FLOAT4 }
		};

		RID rid = ds::createInputLayout(ds::InputLayoutDesc()
			.Declarations(decl)
			.NumDeclarations(3)
			.VertexShader(vertexShader)
		);
		assert(rid != NO_RID);

		_qbCtx->basicGroup = ds::StateGroupBuilder()
			.constantBuffer(constantBuffer, vertexShader)
			.blendState(bs_id)
			.inputLayout(rid)
			.vertexBuffer(_qbCtx->vertexBufferId)
			.vertexShader(vertexShader)
			.indexBuffer(idxBuffer)
			.pixelShader(_qbCtx->pixelShader)
			.samplerState(sSPID, _qbCtx->pixelShader)
			.build();

		ds::DrawCommand drawCmd = { info.maxSprites, ds::DrawType::DT_INDEXED, ds::PrimitiveTypes::TRIANGLE_LIST };

		// create orthographic view
		ds::matrix orthoView = ds::matIdentity();
		ds::matrix orthoProjection = ds::matOrthoLH(ds::getScreenWidth(), ds::getScreenHeight(), 0.0f, 1.0f);
		_qbCtx->camera = {
			orthoView,
			orthoProjection,
			orthoView * orthoProjection,
			ds::vec3(0,0,0),
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
			.Width(ds::getScreenWidth())
			.Height(ds::getScreenHeight())
			.MinDepth(0.0f)
			.MaxDepth(1.0f),
			"QuadOrthoViewport"
			);

		_qbCtx->renderPass = ds::createRenderPass(ds::RenderPassDesc()
			.Camera(&_qbCtx->camera)
			.Viewport(vp)
			.DepthBufferState(ds::DepthBufferState::DISABLED)
			.RenderTargets(0)
			.NumRenderTargets(0),
			"QuadOrthoPass"
			);
		_qbCtx->constantBuffer.wvp = ds::matTranspose(_qbCtx->camera.viewProjectionMatrix);
		_qbCtx->screenCenter = ds::vec2(ds::getScreenWidth() * 0.5f, ds::getScreenHeight() * 0.5f);
	}

	void set_screen_center(const ds::vec2& center) {
		_qbCtx->screenCenter = center;
	}

	void shutdown() {
		delete[] _qbCtx->vertices;
		delete _qbCtx;
	}

	void begin() {
		_qbCtx->current = 0;
		_qbCtx->rendering = true;
		_qbCtx->currentTexture = NO_RID;
	}

	static bool spr__requiresFlush(RID textureID) {
		if (_qbCtx->currentTexture == NO_RID) {
			return false;
		}
		return ((_qbCtx->current + 1) >= _qbCtx->max || textureID != _qbCtx->currentTexture);
	}

	static int spr__createDrawItem(RID textureId) {
		QuadDrawItem item;
		ds::DrawCommand drawCmd = { 100, ds::DrawType::DT_INDEXED, ds::PrimitiveTypes::TRIANGLE_LIST };
		RID myGroup = ds::StateGroupBuilder().texture(textureId, _qbCtx->pixelShader, 0).build();
		RID groups[] = { _qbCtx->basicGroup, myGroup };
		item.drawItem = ds::compile(drawCmd, groups, 2);
		item.textureId = textureId;
		_qbCtx->drawItems.push_back(item);
		return _qbCtx->drawItems.size() - 1;
	}

	void draw(RID textureID, const ds::vec2* positions, const ds::vec4& rect, const ds::vec2& scale, float rotation, const ds::Color& clr) {
		if (spr__requiresFlush(textureID)) {
			flush();
		}
		_qbCtx->currentTexture = textureID;
		int cur = _qbCtx->current;
		for (int i = 0; i < 4; ++i) {
			ds::vec2 p = positions[i];
			vs_input& ip = _qbCtx->vertices[_qbCtx->current++];
			ip.pos = ds::vec3(p.x(), p.y(), 0.0f);
			ip.color = clr;
		}
		ds::vec2 ts = ds::getTextureSize(textureID);
		// u0 / v1
		_qbCtx->vertices[cur].tex[0] = rect.x / ts.x();
		_qbCtx->vertices[cur].tex[1] = (rect.y + rect.w) / ts.y();
		// u0 / v0
		_qbCtx->vertices[cur + 1].tex[0] = rect.x / ts.x();
		_qbCtx->vertices[cur + 1].tex[1] = rect.y / ts.y();
		// u1 / v0
		_qbCtx->vertices[cur + 2].tex[0] = (rect.x + rect.z) / ts.x();
		_qbCtx->vertices[cur + 2].tex[1] = rect.y / ts.y();
		// u1 / v1
		_qbCtx->vertices[cur + 3].tex[0] = (rect.x + rect.z) / ts.x();
		_qbCtx->vertices[cur + 3].tex[1] = (rect.y + rect.w) / ts.y();

	}

	static int spr__findDrawItemIndex(RID textureId) {
		for (size_t i = 0; i < _qbCtx->drawItems.size(); ++i) {
			if (_qbCtx->drawItems[i].textureId == textureId) {
				return i;
			}
		}
		return -1;
	}

	void flush() {
		if (_qbCtx->current > 0) {
			ds::vec2 textureSize = ds::getTextureSize(_qbCtx->currentTexture);
			_qbCtx->constantBuffer.screenDimension = { _qbCtx->screenCenter.x(),  _qbCtx->screenCenter.y(), textureSize.x(), textureSize.y() };
			_qbCtx->constantBuffer.screenCenter = { _qbCtx->screenCenter.x(),  _qbCtx->screenCenter.y(), textureSize.x(), textureSize.y() };
			ds::mapBufferData(_qbCtx->vertexBufferId, _qbCtx->vertices, _qbCtx->current * sizeof(vs_input));
			int idx = spr__findDrawItemIndex(_qbCtx->currentTexture);
			if (idx == -1) {
				idx = spr__createDrawItem(_qbCtx->currentTexture);
			}
			const QuadDrawItem drawItem = _qbCtx->drawItems[idx];
			ds::submit(_qbCtx->renderPass, drawItem.drawItem, _qbCtx->current / 4 * 6);
			_qbCtx->current = 0;
			_qbCtx->currentTexture = NO_RID;
		}
		_qbCtx->rendering = false;
	}

}

#endif
