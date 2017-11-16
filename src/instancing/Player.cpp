#include "Player.h"
#include "..\..\shaders\AmbientLightning_VS_Main.h"
#include "..\..\shaders\AmbientLightning_PS_Main.h"
#include "..\Mesh.h"

void Player::init() {
	ds::ShaderInfo bumpVSInfo = { 0, AmbientLightning_VS_Main, sizeof(AmbientLightning_VS_Main), ds::ShaderType::ST_VERTEX_SHADER };
	RID bumpVS = ds::createShader(bumpVSInfo);
	ds::ShaderInfo bumpPSInfo = { 0, AmbientLightning_PS_Main, sizeof(AmbientLightning_PS_Main), ds::ShaderType::ST_PIXEL_SHADER };
	RID bumpPS = ds::createShader(bumpPSInfo);

	_lightBuffer.ambientColor = ds::Color(0.6f, 0.6f, 0.6f, 1.0f);
	_lightBuffer.diffuseColor = ds::Color(1.0f, 1.0f, 1.0f, 1.0f);
	_lightBuffer.lightDirection = ds::vec3(0.0f, 0.0f, 1.0f);
	_lightBuffer.padding = 0.0f;
	RID lbid = ds::createConstantBuffer(sizeof(InstanceLightBuffer), &_lightBuffer);

	_constantBuffer.viewprojectionMatrix = ds::matIdentity();
	_constantBuffer.worldMatrix = ds::matTranspose(ds::matIdentity());

	RID cbid = ds::createConstantBuffer(sizeof(PlayerConstantBuffer), &_constantBuffer);

	Mesh mesh;
	mesh.loadBin("models\\player.bin");
	RID cubeBuffer = mesh.assemble();

	RID rid = mesh.createInputLayout(bumpVS);

	ds::SamplerStateInfo samplerInfo = { ds::TextureAddressModes::CLAMP, ds::TextureFilters::LINEAR };
	RID ssid = ds::createSamplerState(samplerInfo);

	RID stateGroup = ds::StateGroupBuilder()
		.inputLayout(rid)
		.constantBuffer(cbid, bumpVS, 0)
		.constantBuffer(lbid, bumpPS, 0)
		.samplerState(ssid, bumpPS)
		.vertexBuffer(cubeBuffer)
		.vertexShader(bumpVS)
		.pixelShader(bumpPS)
		.build();


	ds::DrawCommand drawCmd = { mesh.getCount(), ds::DrawType::DT_VERTICES, ds::PrimitiveTypes::TRIANGLE_LIST };

	_drawItem = ds::compile(drawCmd, stateGroup);

	_rotation = 0.0f;
}

void Player::tick(float dt) {
	_position.z = -2.0f;
	if (ds::isKeyPressed('A')) {
		_rotation += ds::PI * dt;
	}
	if (ds::isKeyPressed('D')) {
		_rotation -= ds::PI * dt;
	}

	_velocity = ds::vec3(cosf(_rotation) * 2.0f, sinf(_rotation)*2.0f, 0.0f);

	ds::vec3 tmp = _position;

	if (ds::isKeyPressed('W')) {
		tmp += _velocity * dt;
	}
	if (ds::isKeyPressed('S')) {
		tmp -= _velocity * dt;
	}
	if (tmp.y > -1.5f && tmp.y < 1.5f && tmp.x > -2.3f && tmp.x < 2.3f) {
		_position = tmp;
	}

	ds::vec3 cameraPos = _position;
	cameraPos.z = -6.0f;
	if (cameraPos.x < -0.5f) {
		cameraPos.x = -0.5f;
	}
	if (cameraPos.x > 0.5f) {
		cameraPos.x = 0.5f;
	}
	if (cameraPos.y < -0.5f) {
		cameraPos.y = -0.5f;
	}
	if (cameraPos.y > 0.5f) {
		cameraPos.y = 0.5f;
	}
	_camera->setPosition(cameraPos);
}

void Player::render(RID renderPass, const ds::matrix& viewProjectionMatrix) {
	_constantBuffer.viewprojectionMatrix = ds::matTranspose(viewProjectionMatrix);
	ds::matrix s = ds::matScale(ds::vec3(0.2f));
	ds::matrix world = ds::matTranslate(_position);
	ds::matrix rz = ds::matRotationZ(_rotation);
	_constantBuffer.worldMatrix = ds::matTranspose(rz * s * world);
	ds::submit(renderPass, _drawItem);
}