#include "Billboards.h"
#include "..\..\shaders\Billboards_VS_Main.h"
#include "..\..\shaders\Billboards_PS_Main.h"

void Billboards::init(RID textureID) {

	ds::ShaderInfo vsInfo = { 0, Billboards_VS_Main, sizeof(Billboards_VS_Main), ds::ShaderType::ST_VERTEX_SHADER };
	RID vertexShader = ds::createShader(vsInfo);
	ds::ShaderInfo psInfo = { 0, Billboards_PS_Main, sizeof(Billboards_PS_Main), ds::ShaderType::ST_PIXEL_SHADER };
	RID pixelShader = ds::createShader(psInfo);

	RID cbid = ds::createConstantBuffer(sizeof(BillboardsConstantBuffer), &_constantBuffer);
	int indices[] = { 0,1,2,1,3,2 };
	RID idxBuffer = ds::createQuadIndexBuffer(MAX_BILLBOARDS, indices);
	ds::SamplerStateInfo samplerInfo = { ds::TextureAddressModes::CLAMP, ds::TextureFilters::LINEAR };
	RID ssid = ds::createSamplerState(samplerInfo);

	ds::StructuredBufferInfo sbInfo;
	sbInfo.cpuWritable = true;
	sbInfo.data = 0;
	sbInfo.elementSize = sizeof(Billboard);
	sbInfo.numElements = MAX_BILLBOARDS;
	sbInfo.gpuWritable = false;
	sbInfo.renderTarget = NO_RID;
	sbInfo.textureID = NO_RID;
	_structuredBufferId = ds::createStructuredBuffer(sbInfo);

	_constantBuffer.screenDimension = ds::vec4(1024.0f, 768.0f, 1024.0f, 1024.0f);
	_constantBuffer.screenCenter = ds::vec4(512.0f, 384.0f, 0.0f, 0.0f);
	
	RID stateGroup = ds::StateGroupBuilder()
		.constantBuffer(cbid, vertexShader)
		.structuredBuffer(_structuredBufferId, vertexShader, 1)
		.vertexBuffer(NO_RID)
		.vertexShader(vertexShader)
		.indexBuffer(idxBuffer)
		.pixelShader(pixelShader)
		.samplerState(ssid, pixelShader)
		.texture(textureID, pixelShader, 0)
		.build();

	_drawCmd = { 100, ds::DrawType::DT_INDEXED, ds::PrimitiveTypes::TRIANGLE_LIST, 0 };

	_drawItem = ds::compile(_drawCmd, stateGroup);

	_num = 0;
}

void Billboards::render(RID renderPass, const ds::matrix& viewProjectionMatrix) {
	_constantBuffer.wvp = ds::matTranspose(viewProjectionMatrix);
	ds::mapBufferData(_structuredBufferId, _items, _num * sizeof(Billboard));
	_drawCmd.size = _num;
	ds::submit(renderPass, _drawItem, _num * 6);
}

void Billboards::begin() {
	_num = 0;
}

void Billboards::add(const ds::vec3& pos, const ds::vec2& dim, const ds::vec4& rect, float rotation, const ds::vec2& scaling, const ds::Color& clr) {
	Billboard& b = _items[_num++];
	b.position = pos;
	b.dimension = dim;
	b.texture = rect;
	b.rotation = rotation;
	b.scaling = scaling;
	b.color = clr;
}

void Billboards::end() {

}