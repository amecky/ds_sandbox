#include "Grid.h"
#include "..\..\shaders\Grid_VS_Main.h"
#include "..\..\shaders\Grid_PS_Main.h"


void Grid::create(int numCells, RID renderPass) {
	_renderPass = renderPass;
	float uvMax = static_cast<float>(numCells);
	ds::vec2 uvs[] = { ds::vec2(0.0f,1.0f),ds::vec2(0.0f,0.0f),ds::vec2(1.0f,0.0f),ds::vec2(1.0f,1.0f) };
	ds::vec3 positions[] = { ds::vec3(-0.5f,0.0f,-0.5f),ds::vec3(-0.5f,0.0f,0.5f) ,ds::vec3(0.5f,0.0f,0.5f) ,ds::vec3(0.5f,0.0f,-0.5f) };
	int num = numCells * numCells * 4 / 6;
	_vertices = new GridVertex[numCells * numCells * 4];	
	float sz = numCells / 2.0f - 0.5f;
	for (int z = 0; z < numCells; ++z) {
		float sx = numCells / 2.0f - 0.5f;
		for (int x = 0; x < numCells; ++x) {
			int idx = z * numCells * 4 + x * 4;
			for (int j = 0; j < 4; ++j) {
				_vertices[idx + j].p = positions[j];
				_vertices[idx + j].p.x += sx;
				_vertices[idx + j].p.z += sz;
				_vertices[idx + j].uv = uvs[j];
			}
			sx -= 1.0f;
		}
		sz -= 1.0f;
	}

	ds::ShaderInfo vsInfo = { 0, Grid_VS_Main, sizeof(Grid_VS_Main), ds::ShaderType::ST_VERTEX_SHADER };
	RID vertexShader = ds::createShader(vsInfo);
	ds::ShaderInfo psInfo = { 0, Grid_PS_Main, sizeof(Grid_PS_Main), ds::ShaderType::ST_PIXEL_SHADER };
	RID pixelShader = ds::createShader(psInfo);

	ds::InputLayoutDefinition decl[] = {
		{ "POSITION", 0, ds::BufferAttributeType::FLOAT3 },
		{ "TEXCOORD", 0, ds::BufferAttributeType::FLOAT2 }
	};
	ds::InputLayoutInfo layoutInfo = { decl, 2, vertexShader };
	RID vertexDeclaration = ds::createInputLayout(layoutInfo, "PT_Layout");
	RID bufferID = ds::createConstantBuffer(sizeof(GridConstantBuffer), &_constantBuffer, "GridConstantBuffer");
	RID indexBuffer = ds::createQuadIndexBuffer(numCells * numCells, "GridIndexBuffer");
	ds::VertexBufferInfo vbInfo = { ds::BufferType::STATIC, numCells * numCells * 4, sizeof(GridVertex), _vertices };
	RID gridBuffer = ds::createVertexBuffer(vbInfo, "GridVertexBuffer");

	RID gridStates = ds::StateGroupBuilder()
		.inputLayout(vertexDeclaration)
		.indexBuffer(indexBuffer)
		.vertexBuffer(gridBuffer)
		.vertexShader(vertexShader)
		.pixelShader(pixelShader)
		.constantBuffer(bufferID, vertexShader, 0)
		.build();

	ds::DrawCommand drawCmd = { numCells * numCells * 6, ds::DrawType::DT_INDEXED, ds::PrimitiveTypes::TRIANGLE_LIST };

	_drawItem = ds::compile(drawCmd, gridStates, "Grid");
}

void Grid::render(ds::Camera* camera) {
	_constantBuffer.viewprojectionMatrix = ds::matTranspose(camera->viewProjectionMatrix);
	ds::matrix world = ds::matIdentity();
	_constantBuffer.worldMatrix = ds::matTranspose(world);
	ds::submit(_renderPass, _drawItem);
}