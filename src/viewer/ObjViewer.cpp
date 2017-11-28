#include "ObjViewer.h"
#include <ds_imgui.h>
#include "..\..\shaders\AmbientLightning_VS_Main.h"
#include "..\..\shaders\AmbientLightning_PS_Main.h"

ObjViewer::ObjViewer() : TestApp() {
}

ObjViewer::~ObjViewer() {
	delete _fpsCamera;
}

ds::RenderSettings ObjViewer::getRenderSettings() {
	ds::RenderSettings rs;
	rs.width = 1024;
	rs.height = 768;
	rs.title = "ObjViewer";
	rs.clearColor = ds::Color(0.3f, 0.3f, 0.3f, 1.0f);
	rs.multisampling = 4;
	rs.useGPUProfiling = false;
	rs.supportDebug = false;
	return rs;
}

bool ObjViewer::init() {

	_camera = ds::buildPerspectiveCamera(ds::vec3(0.0f, 0.0f, -6.0f));

	_fpsCamera = new FPSCamera(&_camera);
	_fpsCamera->setPosition(ds::vec3(0, 0, -6), ds::vec3(0.0f, 0.0f, 0.0f));

	ds::ViewportInfo vpInfo = { 1024, 768, 0.0f, 1.0f };
	RID vp = ds::createViewport(vpInfo);

	ds::RenderPassInfo rpInfo = { &_camera, vp, ds::DepthBufferState::ENABLED, 0, 0 };
	_basicPass = ds::createRenderPass(rpInfo);

	ds::BlendStateInfo blendInfo = { ds::BlendStates::SRC_ALPHA, ds::BlendStates::SRC_ALPHA, ds::BlendStates::INV_SRC_ALPHA, ds::BlendStates::INV_SRC_ALPHA, true };
	RID bs_id = ds::createBlendState(blendInfo);

	ds::ShaderInfo bumpVSInfo = { 0, AmbientLightning_VS_Main, sizeof(AmbientLightning_VS_Main), ds::ShaderType::ST_VERTEX_SHADER };
	RID bumpVS = ds::createShader(bumpVSInfo);
	ds::ShaderInfo bumpPSInfo = { 0, AmbientLightning_PS_Main, sizeof(AmbientLightning_PS_Main), ds::ShaderType::ST_PIXEL_SHADER };
	RID bumpPS = ds::createShader(bumpPSInfo);

	_lightBuffer.ambientColor = ds::Color(0.1f, 0.1f, 0.1f, 1.0f);
	_lightBuffer.diffuseColor = ds::Color(1.0f, 1.0f, 1.0f, 1.0f);
	_lightBuffer.lightDirection = ds::vec3(0.0f, 0.0f, 1.0f);
	_lightBuffer.padding = 0.0f;
	RID lbid = ds::createConstantBuffer(sizeof(InstanceLightBuffer), &_lightBuffer);

	_mesh.loadBin("models\\grid_box.bin", false);
	RID hexCubeBuffer = _mesh.assemble();
	RID layout = _mesh.createInputLayout(bumpVS);
	RID cbid = ds::createConstantBuffer(sizeof(InstanceBuffer), &_constantBuffer);

	RID gridGroup = ds::StateGroupBuilder()
		.inputLayout(layout)
		.blendState(bs_id)
		.constantBuffer(lbid, bumpPS, 0)
		.constantBuffer(cbid, bumpVS, 0)
		.vertexBuffer(hexCubeBuffer)
		.vertexShader(bumpVS)
		.pixelShader(bumpPS)
		.build();


	ds::DrawCommand gridDrawCmd = { _mesh.getCount(), ds::DrawType::DT_VERTICES, ds::PrimitiveTypes::TRIANGLE_LIST};

	_gridDrawItem = ds::compile(gridDrawCmd, gridGroup);

	_worldPos = ds::vec3(0.0f);

	return true;
}

void ObjViewer::tick(float dt) {
	_fpsCamera->update(dt);
}

void ObjViewer::render() {
	_constantBuffer.world = ds::matTranspose(ds::matTranslate(_worldPos));
	_constantBuffer.mvp = ds::matTranspose(_camera.viewProjectionMatrix);
	ds::submit(_basicPass, _gridDrawItem);
}

// ----------------------------------------------------
// renderGUI
// ----------------------------------------------------
void ObjViewer::renderGUI() {
	int state = 1;
	gui::start();
	p2i sp = p2i(10, 760);
	if (gui::begin("Debug", &state, &sp, 300)) {
		gui::Value("FPS", ds::getFramesPerSecond());
		gui::Value("Center", _mesh.getCenter());
		gui::Value("Extent", _mesh.getExtent());
		gui::Value("WorldPos", &_worldPos);
		if (gui::Button("Center")) {
			_worldPos = _mesh.getCenter() * -1.0f;
		}
	}
	gui::end();
}