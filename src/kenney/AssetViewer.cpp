#include <diesel.h>
#include <ds_imgui.h>
#include "Scene.h"
#include <Windows.h>

void myLogHandler(const LogLevel&, const char* message) {
	OutputDebugString(message);
	OutputDebugString("\n");
}
// ---------------------------------------------------------------
// main method
// ---------------------------------------------------------------
int run() {
	// prepare application
	ds::RenderSettings rs;
	rs.width = 1024;
	rs.height = 768;
	rs.title = "PCF Shadows";
	rs.clearColor = ds::Color(0.2f, 0.2f, 0.2f, 1.0f);
	rs.multisampling = 4;
	rs.useGPUProfiling = false;
	rs.supportDebug = false;
	rs.logHandler = myLogHandler;
	ds::init(rs);

	gui::init();

	Scene scene;
	//ds::vec3 lightDirection = normalize(ds::vec3( 0.0f,-2.0f,2.0f));
	//ds::vec3 lightDirection = normalize(ds::vec3(-1.0f, 1.0f, 0.0f));

	RID barrelEntity = scene.loadEntity("..\\obj_converter\\barrel.bin");
	RID griddy = scene.loadEntity("..\\obj_converter\\griddy.bin");
	RID gridID = scene.createGrid(10);
	
	//load_entity(&highTile, "..\\obj_converter\\groundTile.bin", shadowGroup, depthGroup);
	//load_entity(&crater, "..\\obj_converter\\crater.bin", shadowGroup, depthGroup);

	scene.createInstance(gridID, ds::vec3(0.0f));
	scene.createInstance(griddy, ds::vec3(0.0f, 0.5f, 0.0f));
	float sx = -1.5f;
	for (int j = 0; j < 2; ++j) {
		float sz = 2.0f;
		for (int i = 0; i < 4; ++i) {
			scene.createInstance(barrelEntity, ds::vec3(sx, 0.0f, sz));
			//sx += 1.5f;
			sz -= 1.5f;
		}
		sx = 1.5f;
	}

	ds::logResources();

	while (ds::isRunning()) {
		ds::begin();

		scene.tick(static_cast<float>(ds::getElapsedSeconds()));

		scene.renderDepthMap();
		
		scene.renderMain();

		scene.renderDebug();

		ds::end();
	}
	ds::shutdown();
	return 0;
}