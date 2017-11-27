#include "AssetViewer.h"
#include <ds_imgui.h>
#include "Scene.h"
#include <Windows.h>

void myLogHandler(const LogLevel&, const char* message) {
	OutputDebugString(message);
	OutputDebugString("\n");
}

AssetViewer::AssetViewer() : TestApp() {
}

AssetViewer::~AssetViewer() {
	delete _scene;
}

ds::RenderSettings AssetViewer::getRenderSettings() {
	ds::RenderSettings rs;
	rs.width = 1024;
	rs.height = 768;
	rs.title = "PCF Shadows";
	rs.clearColor = ds::Color(0.3f, 0.3f, 0.3f, 1.0f);
	rs.multisampling = 4;
	rs.useGPUProfiling = false;
	rs.supportDebug = false;
	rs.logHandler = myLogHandler;
	return rs;
}

bool AssetViewer::init() {

	gui::init();

	_scene = new Scene;

	//RID barrelEntity = _scene->loadEntity("models\\barrel.bin");
	//RID frameHighTile = _scene->loadEntity("models\\frameHighTile.bin");
	//RID griddy = _scene->loadEntity("models\\griddy.bin");
	//RID gridBlockID = _scene->loadEntity("models\\grid_block.bin");
	//RID hexBorderID = _scene->loadEntity("models\\hex_border.bin");
	RID stID = _scene->loadEntity("models\\outer_ring.bin",false);
	//int gridId = _scene->createGrid(gridBlockID, 8);
	//_scene->createInstance(gridId, ds::vec3(0.0f, -0.05f, 0.0f));
	//_scene->createInstance(frameHighTile, ds::vec3(0.0f,0.1f,0.0f));
	_scene->createInstance(stID, ds::vec3(0.0f, 0.5f, 0.0f));
	/*
	float sx = -1.6f;
	for (int j = 0; j < 2; ++j) {
		float sz = 2.0f;
		for (int i = 0; i < 4; ++i) {
			_scene->createInstance(barrelEntity, ds::vec3(sx, -0.001f, sz));
			sz -= 1.5f;
		}
		sx = 2.4f;
	}
	*/
	//ds::logResources();
	return true;
}

void AssetViewer::tick(float dt) {
	_scene->tick(dt);
}

void AssetViewer::render() {
	
	_scene->renderDepthMap();

	_scene->renderMain();

	_scene->renderDebug();

}
