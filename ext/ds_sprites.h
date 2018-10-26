#pragma once
#include <diesel.h>
//#define SPRITE_IMPLEMENTATION

struct SpriteBatchConstantBuffer {
	ds::vec4 screenCenter;
	ds::matrix wvp;
};

typedef unsigned int SPID;

struct SpriteTexture {
	RID textureID;
	ds::vec2 textureSize;
};

struct Sprite {
	enum BlendMode {
		NONE,
		BLEND,
		ADD,
		ADD_X2
	};
	SPID id;
	ds::vec2 position;
	RID textureID;
	ds::vec4 textureRect;
	ds::vec2 scaling;
	float rotation;
	ds::Color color;
	int type;
	BlendMode blendMode;
};

struct InternalSprite {
	ds::vec2 position;
	ds::vec4 textureRect;
	ds::vec2 scaling;
	float rotation;
	ds::Color color;
};

struct SpriteBatchBufferInfo {

	unsigned int maxSprites;
	ds::TextureFilters textureFilter;
	RID blendState;
	RID vertexShader;
	RID pixelShader;
	RID constantBuffer;
};

struct SpriteArrayIndex {
	SPID id;
	unsigned short index;
	unsigned short next;
};


struct SpriteArray {
	unsigned int maxObjects;
	unsigned int numObjects;
	SpriteArrayIndex* indices;
	Sprite* objects;
	unsigned short free_enqueue;
	unsigned short free_dequeue;
};

namespace sprites {

	void initialize(SpriteArray& array, int maxSprites);

	void shutdown(SpriteArray& array);

	void clear(SpriteArray& array);

	bool contains(SpriteArray& array, SPID id);

	Sprite& get(SpriteArray& array, SPID id);

	SPID add(SpriteArray& array);

	SPID add(SpriteArray& array, const Sprite& sprite);

	SPID add(SpriteArray& array, const ds::vec2& pos, const ds::vec4& textureRect, const ds::vec2& scale = ds::vec2(1.0f), float rotation = 0.0f, const ds::Color& clr = ds::Color(255, 255, 255, 255));

	void remove(SpriteArray& array, SPID id);

}

class SpriteBatchDesc {

public:
	SpriteBatchDesc() {
		_info.maxSprites = 1024;
		_info.textureFilter = ds::TextureFilters::LINEAR;
		_info.blendState = NO_RID;
		_info.pixelShader = NO_RID;
		_info.vertexShader = NO_RID;
		_info.constantBuffer = NO_RID;
	}
	SpriteBatchDesc& MaxSprites(int max) {
		_info.maxSprites = max;
		return *this;
	}
	SpriteBatchDesc& TextureFilter(ds::TextureFilters filter) {
		_info.textureFilter = filter;
		return *this;
	}
	SpriteBatchDesc& BlendState(RID blendState) {
		_info.blendState = blendState;
		return *this;
	}
	SpriteBatchDesc& PixelShader(RID pixelShader) {
		_info.pixelShader = pixelShader;
		return *this;
	}
	SpriteBatchDesc& VertexShader(RID vertexShader) {
		_info.vertexShader = vertexShader;
		return *this;
	}
	SpriteBatchDesc& ConstantBuffer(RID constantBuffer) {
		_info.constantBuffer = constantBuffer;
		return *this;
	}
	const SpriteBatchBufferInfo& getInfo() const {
		return _info;
	}
private:
	SpriteBatchBufferInfo _info;
};

namespace sprites {

	void initialize(int maxSprites);

	void initialize(const SpriteBatchDesc& info);

	void begin();

	void flush();

	void draw(RID textureID, const ds::vec2& pos, const ds::vec4& textureRect, const ds::vec2& scale = ds::vec2(1.0f,1.0f), float rotation = 0.0f, const ds::Color& clr = ds::Color(255,255,255,255));

	void draw(const Sprite& sprite);

	void draw(Sprite* sprites, int num);

	void shutdown();

}

/*
public:
	SpriteBatchBuffer(const SpriteBatchDesc& info);
	~SpriteBatchBuffer();
	void begin();
	void add(const ds::vec2& pos, RID textureID, const ds::vec4& textureRect, const ds::vec2& scaling = ds::vec2(1, 1), float rotation = 0.0f, const ds::Color& color = ds::Color(255, 255, 255, 255));
	void add(const Sprite& sprite);
	void flush();
	RID getRenderPass();
	void render(const SpriteArray& array);
private:
	bool requiresFlush(RID textureID);
	RID _currentTexture;
	bool _rendering;
	unsigned int _max;
	unsigned int _current;
	RID _drawItem;
	RID _renderPass;
	InternalSprite* _buffer;
	RID _structuredBufferId;
	SpriteBatchConstantBuffer _constantBuffer;
};

}
*/
#ifdef SPRITE_IMPLEMENTATION
#include <vector>
#include <assert.h>

const BYTE Sprites_VS_Main[] =
{
	68,  88,  66,  67, 176, 179,  62,  76, 198,  29, 218, 165, 231, 174, 221,  37,  99, 133,  84, 135,   1,   0,   0,   0, 228,   4,   0,   0,   4,   0,   0,   0,
	48,   0,   0,   0, 100,   0,   0,   0, 216,   0,   0,   0, 212,   4,   0,   0,  73,  83,  71,  78,  44,   0,   0,   0,   1,   0,   0,   0,   8,   0,   0,   0,
	32,   0,   0,   0,   0,   0,   0,   0,   6,   0,   0,   0,   1,   0,   0,   0,   0,   0,   0,   0,   1,   1,   0,   0,  83,  86,  95,  86,  69,  82,  84,  69,
	88,  73,  68,   0,  79,  83,  71,  78, 108,   0,   0,   0,   3,   0,   0,   0,   8,   0,   0,   0,  80,   0,   0,   0,   0,   0,   0,   0,   1,   0,   0,   0,
	3,   0,   0,   0,   0,   0,   0,   0,  15,   0,   0,   0,  92,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   3,   0,   0,   0,   1,   0,   0,   0,
	3,  12,   0,   0, 101,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   3,   0,   0,   0,   2,   0,   0,   0,  15,   0,   0,   0,  83,  86,  95,  80,
	79,  83,  73,  84,  73,  79,  78,   0,  84,  69,  88,  67,  79,  79,  82,  68,   0,  67,  79,  76,  79,  82,   0, 171,  83,  72,  69,  88, 244,   3,   0,   0,
	64,   0,   1,   0, 253,   0,   0,   0, 106,  72,   0,   1,  89,   0,   0,   4,  70, 142,  32,   0,   0,   0,   0,   0,   5,   0,   0,   0, 162,   0,   0,   4,
	0, 112,  16,   0,   1,   0,   0,   0,  52,   0,   0,   0,  96,   0,   0,   4,  18,  16,  16,   0,   0,   0,   0,   0,   6,   0,   0,   0, 103,   0,   0,   4,
	242,  32,  16,   0,   0,   0,   0,   0,   1,   0,   0,   0, 101,   0,   0,   3,  50,  32,  16,   0,   1,   0,   0,   0, 101,   0,   0,   3, 242,  32,  16,   0,
	2,   0,   0,   0, 104,   0,   0,   2,   4,   0,   0,   0, 105,   0,   0,   4,   0,   0,   0,   0,   4,   0,   0,   0,   4,   0,   0,   0,  85,   0,   0,   7,
	18,   0,  16,   0,   0,   0,   0,   0,  10,  16,  16,   0,   0,   0,   0,   0,   1,  64,   0,   0,   2,   0,   0,   0, 167,   0,   0,   9, 242,   0,  16,   0,
	1,   0,   0,   0,  10,   0,  16,   0,   0,   0,   0,   0,   1,  64,   0,   0,   8,   0,   0,   0,  70, 126,  16,   0,   1,   0,   0,   0,  14,   0,   0,   8,
	242,   0,  16,   0,   2,   0,   0,   0,  70,  14,  16,   0,   1,   0,   0,   0, 230, 142,  32,   0,   0,   0,   0,   0,   0,   0,   0,   0,  54,   0,   0,   6,
	50,  48,  32,   0,   0,   0,   0,   0,   0,   0,   0,   0,  70,   0,  16,   0,   2,   0,   0,   0,  54,   0,   0,   5, 194,   0,  16,   0,   3,   0,   0,   0,
	86,   1,  16,   0,   2,   0,   0,   0,   0,   0,   0,   7,  50,   0,  16,   0,   3,   0,   0,   0, 230,  10,  16,   0,   2,   0,   0,   0,  70,   0,  16,   0,
	2,   0,   0,   0,  54,   0,   0,   6,  50,  48,  32,   0,   0,   0,   0,   0,   1,   0,   0,   0, 134,   0,  16,   0,   3,   0,   0,   0,  54,   0,   0,   6,
	50,  48,  32,   0,   0,   0,   0,   0,   2,   0,   0,   0, 118,  15,  16,   0,   3,   0,   0,   0,  54,   0,   0,   6,  50,  48,  32,   0,   0,   0,   0,   0,
	3,   0,   0,   0,  70,   0,  16,   0,   3,   0,   0,   0,   1,   0,   0,  10, 226,   0,  16,   0,   0,   0,   0,   0,   6,  16,  16,   0,   0,   0,   0,   0,
	2,  64,   0,   0,   0,   0,   0,   0,   3,   0,   0,   0,   1,   0,   0,   0,   2,   0,   0,   0,  55,   0,   0,  15, 194,   0,  16,   0,   0,   0,   0,   0,
	246,  11,  16,   0,   0,   0,   0,   0,   2,  64,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 191,   0,   0,   0,  63,   2,  64,   0,   0,
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,  63,   0,   0,   0, 191,  54,   0,   0,   7,  50,  32,  16,   0,   1,   0,   0,   0,  70,  48,  32,   4,
	0,   0,   0,   0,  26,   0,  16,   0,   0,   0,   0,   0,  56,   0,   0,   7,  98,   0,  16,   0,   0,   0,   0,   0, 246,  14,  16,   0,   1,   0,   0,   0,
	166,  11,  16,   0,   0,   0,   0,   0, 167,   0,   0,   9, 242,   0,  16,   0,   1,   0,   0,   0,  10,   0,  16,   0,   0,   0,   0,   0,   1,  64,   0,   0,
	24,   0,   0,   0,  70, 126,  16,   0,   1,   0,   0,   0,  56,   0,   0,   7,  98,   0,  16,   0,   0,   0,   0,   0,  86,   6,  16,   0,   0,   0,   0,   0,
	86,   4,  16,   0,   1,   0,   0,   0,  77,   0,   0,   7,  18,   0,  16,   0,   2,   0,   0,   0,  18,   0,  16,   0,   3,   0,   0,   0,  42,   0,  16,   0,
	1,   0,   0,   0,  56,   0,   0,   7,  50,   0,  16,   0,   2,   0,   0,   0, 150,   5,  16,   0,   0,   0,   0,   0,   6,   0,  16,   0,   2,   0,   0,   0,
	50,   0,   0,  10,  66,   0,  16,   0,   0,   0,   0,   0,  10,   0,  16,   0,   3,   0,   0,   0,  42,   0,  16,   0,   0,   0,   0,   0,  10,   0,  16, 128,
	65,   0,   0,   0,   2,   0,   0,   0,  50,   0,   0,   9,  34,   0,  16,   0,   0,   0,   0,   0,  10,   0,  16,   0,   3,   0,   0,   0,  26,   0,  16,   0,
	0,   0,   0,   0,  26,   0,  16,   0,   2,   0,   0,   0, 167,   0,   0,   9,  50,   0,  16,   0,   2,   0,   0,   0,  10,   0,  16,   0,   0,   0,   0,   0,
	1,  64,   0,   0,   0,   0,   0,   0,  70, 112,  16,   0,   1,   0,   0,   0, 167,   0,   0,   9, 114,   0,  16,   0,   1,   0,   0,   0,  10,   0,  16,   0,
	0,   0,   0,   0,   1,  64,   0,   0,  40,   0,   0,   0,  70, 114,  16,   0,   1,   0,   0,   0,  54,   0,   0,   5, 242,  32,  16,   0,   2,   0,   0,   0,
	54,   9,  16,   0,   1,   0,   0,   0,   0,   0,   0,   9, 146,   0,  16,   0,   0,   0,   0,   0,   6,   4,  16,   0,   2,   0,   0,   0,   6, 132,  32, 128,
	65,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   7,  50,   0,  16,   0,   1,   0,   0,   0, 198,   0,  16,   0,   0,   0,   0,   0,
	102,  10,  16,   0,   0,   0,   0,   0,  54,   0,   0,   5,  66,   0,  16,   0,   1,   0,   0,   0,   1,  64,   0,   0,   0,   0, 128,  63,  16,   0,   0,   8,
	18,  32,  16,   0,   0,   0,   0,   0,  70,   2,  16,   0,   1,   0,   0,   0,  70, 131,  32,   0,   0,   0,   0,   0,   1,   0,   0,   0,  16,   0,   0,   8,
	34,  32,  16,   0,   0,   0,   0,   0,  70,   2,  16,   0,   1,   0,   0,   0,  70, 131,  32,   0,   0,   0,   0,   0,   2,   0,   0,   0,  16,   0,   0,   8,
	130,  32,  16,   0,   0,   0,   0,   0,  70,   2,  16,   0,   1,   0,   0,   0,  70, 131,  32,   0,   0,   0,   0,   0,   4,   0,   0,   0,  54,   0,   0,   5,
	66,  32,  16,   0,   0,   0,   0,   0,   1,  64,   0,   0,   0,   0, 128,  63,  62,   0,   0,   1,  83,  70,  73,  48,   8,   0,   0,   0,   2,   0,   0,   0,
	0,   0,   0,   0
};


const BYTE Sprites_PS_Main[] =
{
	68,  88,  66,  67, 128, 110,   6,  17, 183, 249, 240,  47,  66,  79,  68, 157, 198, 184, 192,  10,   1,   0,   0,   0, 160,   1,   0,   0,   3,   0,   0,   0,
	44,   0,   0,   0, 160,   0,   0,   0, 212,   0,   0,   0,  73,  83,  71,  78, 108,   0,   0,   0,   3,   0,   0,   0,   8,   0,   0,   0,  80,   0,   0,   0,
	0,   0,   0,   0,   1,   0,   0,   0,   3,   0,   0,   0,   0,   0,   0,   0,  15,   0,   0,   0,  92,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	3,   0,   0,   0,   1,   0,   0,   0,   3,   3,   0,   0, 101,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   3,   0,   0,   0,   2,   0,   0,   0,
	15,  15,   0,   0,  83,  86,  95,  80,  79,  83,  73,  84,  73,  79,  78,   0,  84,  69,  88,  67,  79,  79,  82,  68,   0,  67,  79,  76,  79,  82,   0, 171,
	79,  83,  71,  78,  44,   0,   0,   0,   1,   0,   0,   0,   8,   0,   0,   0,  32,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   3,   0,   0,   0,
	0,   0,   0,   0,  15,   0,   0,   0,  83,  86,  95,  84,  65,  82,  71,  69,  84,   0, 171, 171,  83,  72,  68,  82, 196,   0,   0,   0,  64,   0,   0,   0,
	49,   0,   0,   0,  90,   0,   0,   3,   0,  96,  16,   0,   0,   0,   0,   0,  88,  24,   0,   4,   0, 112,  16,   0,   0,   0,   0,   0,  85,  85,   0,   0,
	98,  16,   0,   3,  50,  16,  16,   0,   1,   0,   0,   0,  98,  16,   0,   3, 242,  16,  16,   0,   2,   0,   0,   0, 101,   0,   0,   3, 242,  32,  16,   0,
	0,   0,   0,   0, 104,   0,   0,   2,   1,   0,   0,   0,  69,   0,   0,   9, 242,   0,  16,   0,   0,   0,   0,   0,  70,  16,  16,   0,   1,   0,   0,   0,
	70, 126,  16,   0,   0,   0,   0,   0,   0,  96,  16,   0,   0,   0,   0,   0,  56,   0,   0,   7, 242,   0,  16,   0,   0,   0,   0,   0,  70,  14,  16,   0,
	0,   0,   0,   0,  70,  30,  16,   0,   2,   0,   0,   0,  56,   0,   0,   7, 114,  32,  16,   0,   0,   0,   0,   0, 246,  15,  16,   0,   0,   0,   0,   0,
	70,   2,  16,   0,   0,   0,   0,   0,  54,   0,   0,   5, 130,  32,  16,   0,   0,   0,   0,   0,  58,   0,  16,   0,   0,   0,   0,   0,  62,   0,   0,   1
};

struct SpriteDrawItem {
	RID textureId;
	RID drawItem;
};

struct SpritesContext {
	RID currentTexture;
	bool rendering;
	unsigned int max;
	unsigned int current;
	//RID drawItem;
	RID renderPass;
	InternalSprite* buffer;
	RID structuredBufferId;
	RID basicGroup;
	SpriteBatchConstantBuffer constantBuffer;
	std::vector<SpriteDrawItem> drawItems;
	RID pixelShader;
};

static SpritesContext* _ctx = 0;

namespace sprites {

	void initialize(const SpriteBatchDesc& desc) {
		_ctx = new SpritesContext;
		_ctx->current = 0;
		_ctx->rendering = false;
		const SpriteBatchBufferInfo& info = desc.getInfo();
		_ctx->max = info.maxSprites;
		_ctx->buffer = new InternalSprite[_ctx->max];

		RID vertexShader = info.vertexShader;
		if (vertexShader == NO_RID) {
			vertexShader = ds::createShader(ds::ShaderDesc()
				.Data(Sprites_VS_Main)
				.DataSize(sizeof(Sprites_VS_Main))
				.ShaderType(ds::ShaderType::ST_VERTEX_SHADER)
				, "SpritesVS"
			);
		}
		_ctx->pixelShader = info.pixelShader;
		if (_ctx->pixelShader == NO_RID) {
			_ctx->pixelShader = ds::createShader(ds::ShaderDesc()
				.Data(Sprites_PS_Main)
				.DataSize(sizeof(Sprites_PS_Main))
				.ShaderType(ds::ShaderType::ST_PIXEL_SHADER)
				, "SpritesPS"
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
			constantBuffer = ds::createConstantBuffer(sizeof(SpriteBatchConstantBuffer), &_ctx->constantBuffer);
		}
		RID sSPID = createSamplerState(ds::SamplerStateDesc()
			.AddressMode(ds::TextureAddressModes::CLAMP)
			.Filter(info.textureFilter)
		);

		int indices[] = { 0,1,2,1,3,2 };
		RID idxBuffer = ds::createQuadIndexBuffer(_ctx->max, indices);

		_ctx->structuredBufferId = ds::createStructuredBuffer(ds::StructuredBufferDesc()
			.CpuWritable(true)
			.ElementSize(sizeof(InternalSprite))
			.NumElements(_ctx->max)
			.GpuWritable(false)
			);

		_ctx->basicGroup = ds::StateGroupBuilder()
			.constantBuffer(constantBuffer, vertexShader)
			.blendState(bs_id)
			.structuredBuffer(_ctx->structuredBufferId, vertexShader, 1)
			.vertexBuffer(NO_RID)
			.vertexShader(vertexShader)
			.indexBuffer(idxBuffer)
			.pixelShader(_ctx->pixelShader)
			.samplerState(sSPID, _ctx->pixelShader)
			.build();

		ds::DrawCommand drawCmd = { 100, ds::DrawType::DT_INDEXED, ds::PrimitiveTypes::TRIANGLE_LIST };

		// create orthographic view
		ds::matrix orthoView = ds::matIdentity();
		ds::matrix orthoProjection = ds::matOrthoLH(ds::getScreenWidth(), ds::getScreenHeight(), 0.0f, 1.0f);
		ds::Camera camera = {
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
			"SpriteOrthoViewport"
			);

		_ctx->renderPass = ds::createRenderPass(ds::RenderPassDesc()
			.Camera(&camera)
			.Viewport(vp)
			.DepthBufferState(ds::DepthBufferState::DISABLED)
			.RenderTargets(0)
			.NumRenderTargets(0),
			"SpritesOrthoPass"
			);
		_ctx->constantBuffer.wvp = ds::matTranspose(camera.viewProjectionMatrix);
	}

	void shutdown() {
		delete[] _ctx->buffer;
		delete _ctx;
	}

	void begin() {
		_ctx->current = 0;
		_ctx->rendering = true;
		_ctx->currentTexture = NO_RID;
	}

	static bool spr__requiresFlush(RID textureID) {
		return ((_ctx->current + 1) >= _ctx->max || textureID != _ctx->currentTexture);
	}

	static int spr__createDrawItem(RID textureId) {
		SpriteDrawItem item;
		ds::DrawCommand drawCmd = { 100, ds::DrawType::DT_INDEXED, ds::PrimitiveTypes::TRIANGLE_LIST };
		RID myGroup = ds::StateGroupBuilder().texture(textureId, _ctx->pixelShader, 0).build();
		RID groups[] = { _ctx->basicGroup, myGroup };
		item.drawItem = ds::compile(drawCmd, groups, 2);
		item.textureId = textureId;
		_ctx->drawItems.push_back(item);
		return _ctx->drawItems.size() - 1;
	}

	void draw(RID textureID, const ds::vec2& position, const ds::vec4& rect, const ds::vec2& scale, float rotation, const ds::Color& clr) {
		if (spr__requiresFlush(textureID)) {
			flush();
		}
		_ctx->currentTexture = textureID;
		_ctx->buffer[_ctx->current++] = { position, rect, scale, rotation, clr };
	}

	void draw(const SpriteArray& array) {
		if (!_ctx->rendering) {
			begin();
		}
		for (unsigned int i = 0; i < array.numObjects; ++i) {
			draw(array.objects[i]);
		}
		flush();
	}

	void draw(Sprite* sprites, int num) {
		if (!_ctx->rendering) {
			begin();
		}
		for (int i = 0; i < num; ++i) {
			draw(sprites[i]);
		}
		flush();
	}

	void draw(const Sprite& sprite) {
		if (spr__requiresFlush(sprite.textureID)) {
			flush();
		}
		_ctx->currentTexture = sprite.textureID;
		_ctx->buffer[_ctx->current++] = { sprite.position, sprite.textureRect, sprite.scaling, sprite.rotation, sprite.color };
	}

	static int spr__findDrawItemIndex(RID textureId) {
		for (size_t i = 0; i < _ctx->drawItems.size(); ++i) {
			if (_ctx->drawItems[i].textureId == textureId) {
				return i;
			}
		}
		return -1;
	}

	void flush() {
		if (_ctx->current > 0) {
			ds::vec2 textureSize = ds::getTextureSize(_ctx->currentTexture);
			_ctx->constantBuffer.screenCenter = { static_cast<float>(ds::getScreenWidth()) / 2.0f, static_cast<float>(ds::getScreenHeight()) / 2.0f, textureSize.x, textureSize.y };
			ds::mapBufferData(_ctx->structuredBufferId, _ctx->buffer, _ctx->current * sizeof(Sprite));
			int idx = spr__findDrawItemIndex(_ctx->currentTexture);
			if (idx == -1) {
				idx = spr__createDrawItem(_ctx->currentTexture);
			}
			const SpriteDrawItem drawItem = _ctx->drawItems[idx];
			ds::submit(_ctx->renderPass, drawItem.drawItem, _ctx->current * 6);
			_ctx->current = 0;
			_ctx->currentTexture = NO_RID;
		}
		_ctx->rendering = false;
	}

}
#define INDEX_MASK 0xffff

namespace sprites {

	void initialize(SpriteArray& array, int maxSprites) {
		array.maxObjects = maxSprites;
		array.indices = new SpriteArrayIndex[maxSprites];
		array.objects = new Sprite[maxSprites];
		clear(array);
	}

	void clear(SpriteArray& array) {
		array.numObjects = 0;
		for (unsigned short i = 0; i < array.maxObjects; ++i) {
			array.indices[i].id = i;
			array.indices[i].next = i + 1;
		}
		array.free_dequeue = 0;
		array.free_enqueue = array.maxObjects - 1;
	}

	bool contains(SpriteArray& array, SPID id) {
		const SpriteArrayIndex& in = array.indices[id & INDEX_MASK];
		return in.id == id && in.index != USHRT_MAX;
	}

	Sprite& get(SpriteArray& array, SPID id) {
		assert(id != UINT_MAX);
		unsigned short index = array.indices[id & INDEX_MASK].index;
		assert(index != USHRT_MAX);
		return array.objects[index];
	}

	SPID add(SpriteArray& array) {
		assert(array.numObjects != array.maxObjects);
		SpriteArrayIndex& in = array.indices[array.free_dequeue];
		array.free_dequeue = in.next;
		//in.id += NEW_OBJECT_ID_ADD;
		in.index = array.numObjects++;
		Sprite& o = array.objects[in.index];
		o.id = in.id;
		return o.id;
	}

	SPID add(SpriteArray& array, const Sprite& sprite) {
		SPID id = add(array);
		Sprite& sp = get(array, id);
		sp.position = sprite.position;
		sp.textureRect = sprite.textureRect;
		sp.scaling = sprite.scaling;
		sp.rotation = sprite.rotation;
		sp.color = sprite.color;
		return id;
	}

	SPID add(SpriteArray& array, const ds::vec2& pos, const ds::vec4& textureRect, const ds::vec2& scale, float rotation, const ds::Color& clr) {
		SPID id = add(array);
		Sprite& sp = get(array, id);
		sp.position = pos;
		sp.textureRect = textureRect;
		sp.scaling = scale;
		sp.rotation = rotation;
		sp.color = clr;
		return id;
	}

	void remove(SpriteArray& array, SPID id) {
		SpriteArrayIndex &in = array.indices[id & INDEX_MASK];
		assert(in.index != USHRT_MAX);
		int current = in.index;
		Sprite& o = array.objects[in.index];
		o = array.objects[--array.numObjects];
		array.indices[o.id & INDEX_MASK].index = in.index;
		in.index = USHRT_MAX;
		array.indices[array.free_enqueue].next = id & INDEX_MASK;
		array.free_enqueue = id & INDEX_MASK;
	}

	void shutdown(SpriteArray& array) {
		delete[] array.objects;
		delete[] array.indices;
	}

}

#endif
