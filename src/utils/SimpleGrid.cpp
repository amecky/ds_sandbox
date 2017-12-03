#include "SimpleGrid.h"
#include "..\..\shaders\RepeatingGrid_VS_Main.h"
#include "..\..\shaders\RepeatingGrid_PS_Main.h"

SimpleGrid::SimpleGrid() {
}

SimpleGrid::~SimpleGrid() {
}

// ----------------------------------------------------
// init
// ----------------------------------------------------
bool SimpleGrid::init(float gridSteps, float base, float amplitude) {
	
	RID cbid = ds::createConstantBuffer(sizeof(InstanceBuffer), &_constantBuffer);

	ds::vec3 positions[] = {
		ds::vec3(-gridSteps, 0.0f,-gridSteps),
		ds::vec3(-gridSteps, 0.0f, gridSteps) ,
		ds::vec3( gridSteps, 0.0f, gridSteps) ,
		ds::vec3( gridSteps, 0.0f, gridSteps) ,
		ds::vec3( gridSteps, 0.0f,-gridSteps) ,
		ds::vec3(-gridSteps, 0.0f,-gridSteps) 
	};
	_grid.addStream(AttributeType::AT_VERTEX, (float*)positions, 6, 3);
	RID gridBuffer = _grid.assemble();

	_settingsBuffer.amplitude = amplitude;
	_settingsBuffer.base = base;
	_settingsBuffer.one = 0.0f;
	_settingsBuffer.two = 0.0f;
	
	RID gridVS = ds::createVertexShader(RepeatingGrid_VS_Main, sizeof(RepeatingGrid_VS_Main));
	RID gridPS = ds::createPixelShader(RepeatingGrid_PS_Main, sizeof(RepeatingGrid_PS_Main));
	RID gridLayout = _grid.createInputLayout(gridVS);
	RID settingsBufferId = ds::createConstantBuffer(sizeof(GridSettingsBuffer), &_settingsBuffer);
	RID nextGroup = ds::StateGroupBuilder()
		.inputLayout(gridLayout)
		.constantBuffer(cbid, gridVS, 0)
		.constantBuffer(settingsBufferId, gridPS, 0)
		.vertexBuffer(gridBuffer)
		.vertexShader(gridVS)
		.pixelShader(gridPS)
		.build();


	ds::DrawCommand nextDrawCmd = { _grid.getCount(), ds::DrawType::DT_VERTICES, ds::PrimitiveTypes::TRIANGLE_LIST };

	_gridDrawItem = ds::compile(nextDrawCmd, nextGroup);

	return true;
}

// ----------------------------------------------------
// render
// ----------------------------------------------------
void SimpleGrid::render(RID renderPass, const ds::matrix& viewProjectionMatrix) {
	_constantBuffer.mvp = ds::matTranspose(viewProjectionMatrix);
	_constantBuffer.world = ds::matTranspose(ds::matIdentity());
	ds::submit(renderPass, _gridDrawItem);
}