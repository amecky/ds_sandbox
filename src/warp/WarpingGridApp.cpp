#include "WarpingGridApp.h"
#include <ds_imgui.h>

WarpingGridApp::WarpingGridApp() {

}

WarpingGridApp::~WarpingGridApp() {
	delete _grid;
}

ds::RenderSettings WarpingGridApp::getRenderSettings() {
	ds::RenderSettings rs;
	rs.width = 1024;
	rs.height = 768;
	rs.title = "Warping grid";
	rs.clearColor = ds::Color(0.01f, 0.01f, 0.01f, 1.0f);
	rs.multisampling = 4;
	rs.useGPUProfiling = false;
	rs.supportDebug = true;
	return rs;
}

bool WarpingGridApp::init() {
	
	_camera = ds::buildPerspectiveCamera(ds::vec3(0.0f, 0.0f, -6.0f));

	ds::ViewportInfo vpInfo = { 1024, 768, 0.0f, 1.0f };
	RID vp = ds::createViewport(vpInfo);

	ds::RenderPassInfo rpInfo = { &_camera, vp, ds::DepthBufferState::ENABLED, 0, 0 };
	_basicPass = ds::createRenderPass(rpInfo);
	
	_fpsCamera = new FPSCamera(&_camera);
	_fpsCamera->setPosition(ds::vec3(0, 0, -6), ds::vec3(0.0f, 0.0f, 0.0f));

	WarpingGridSettings wgs;
	wgs.numX = 30;
	wgs.numY = 20;
	wgs.size = 0.3f;
	wgs.borderSize = 0.05f;
	wgs.zOffset = 0.5f;
	wgs.color = ds::Color(0, 0, 50, 255);
	wgs.stiffness = 0.28f;
	wgs.damping = 0.06f;

	_grid = new WarpingGrid(wgs);
	_grid->init();

	_dbgX = 5;
	_dbgY = 5;
	_dbgForce = 0.3f;

	return true;
}

void WarpingGridApp::tick(float dt) {
	_fpsCamera->update(dt);
	_grid->tick(dt);

}

void WarpingGridApp::render() {
	_grid->render(_basicPass, _camera.viewProjectionMatrix);
}

void WarpingGridApp::renderGUI() {
	int state = 1;
	gui::start();
	p2i sp = p2i(10, 760);
	if (gui::begin("Debug", &state, &sp, 250)) {
		gui::Value("FPS", ds::getFramesPerSecond());
	}
	gui::Input("X", &_dbgX);
	gui::Input("Y", &_dbgY);
	gui::Input("Force", &_dbgForce);
	if (gui::Button("Apply")) {
		_grid->applyForce(_dbgX, _dbgY, ds::vec3(0.0f, 0.0f, _dbgForce));
	}
	if (gui::Button("Apply Ring")) {
		_grid->applyForce(_dbgX, _dbgY, 0.6f, _dbgForce);
	}
	if (gui::Button("Highlight")) {
		_grid->highlight(_dbgX, _dbgY);
	}
	if (gui::Button("Reset Camera")) {
		_fpsCamera->setPosition(ds::vec3(0, 0, -6), ds::vec3(0.0f, 0.0f, 0.0f));
	}
	gui::end();
}