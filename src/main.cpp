#define DS_IMPLEMENTATION
#include <diesel.h>
//#include <SpriteBatchBuffer.h>
#define DS_IMGUI_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define SPRITE_IMPLEMENTATION
//#include <stb_image.h>
#include "..\resource.h"
#include "tweening\TweeningTest.h"
#include "AnimationTest.h"
//#define DS_LOG_PANEL
//#include "LogPanel.h"
#define DS_VM_IMPLEMENTATION
#include <ds_vm.h>
#include "vm\VMTest.h"
#define DS_PROFILER_IMPLEMENTATION
#include <ds_profiler.h>
#include "particles\ParticleExpressionTest.h"
#define DS_PERF_PANEL
#include "PerfPanel.h"
#include "plugins\PluginRegistry.h"
#include "ParticlePluginTest.h"
#include "kenney\AssetViewer.h"
#include "binary\BinaryClock.h"
#include "instancing\InstanceTest.h"
#include "warp\WarpingGridApp.h"
// ---------------------------------------------------------------
// initialize rendering system
// ---------------------------------------------------------------
void initialize() {
	ds::RenderSettings rs;
	rs.width = 1280;
	rs.height = 960;
	rs.title = "ds_sandbox";
	rs.clearColor = ds::Color(0.1f, 0.1f, 0.1f, 1.0f);
	rs.multisampling = 4;
	ds::init(rs);
}

// ---------------------------------------------------------------
// main method
// ---------------------------------------------------------------
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow) {

	TestApp* app = new WarpingGridApp;
	//TestApp* app = new InstanceTest;
	//TestApp* app = new BinaryClock;
	//TestApp* app = new AssetViewer;
	//TestApp* app = new TweeningTest;
	//TestApp* app = new VMTest;
	//TestApp* app = new ParticleExpressionTest;
	//_CrtSetBreakAlloc(160);
	SetThreadAffinityMask(GetCurrentThread(), 1);

	ds::init(app->getRenderSettings());

	if (app->usesGUI()) {
		gui::init();
	}

	perf::init();

	bool state = app->init();
	assert(state);

	bool running = true;
	
	float timeStep = 1.0f / 60.0f;
	float gameTimer = 0.0f;
	float currentTime = 0.0f;
	float accu = 0.0f;

	while (ds::isRunning() && running) {

		ds::begin();

		perf::reset();

		perf::ZoneTracker("main");

		if (app->useFixedTimestep()) {
			gameTimer += ds::getElapsedSeconds();
			accu += ds::getElapsedSeconds();
			if (accu > 0.25f) {
				accu = 0.25f;
			}
			int cnt = 0;
			while (accu >= timeStep) {
				app->tick(timeStep);
				accu -= timeStep;
				++cnt;
			}
		}
		else {
			app->tick(ds::getElapsedSeconds());
		}

		app->render();

		if (app->usesGUI()) {
			app->renderGUI();
		}
			
		ds::end();

		perf::finalize();

	}
	if (app->usesGUI()) {
		gui::shutdown();
	}
	app->shutdown();
	delete app;
	ds::shutdown();
	
}