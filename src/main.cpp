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
#define GAMESETTINGS_IMPLEMENTATION
#include <ds_tweakable.h>
#include "plugins\PluginRegistry.h"
#include "ParticlePluginTest.h"
#include "kenney\AssetViewer.h"
#include "binary\BinaryClock.h"
#include "instancing\InstanceTest.h"
#include "warp\WarpingGridApp.h"
#include "viewer\ObjViewer.h"

// ---------------------------------------------------------------
// main method
// ---------------------------------------------------------------
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow) {

	TestApp* app = new ObjViewer;
	//TestApp* app = new WarpingGridApp;
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

	bool useTweaking = false;
	float tweakingReloadTimer = 0.0f;

	const char* settingsFileName = app->getSettingsFileName();
	if (settingsFileName != 0) {
		twk_init(settingsFileName);		
		useTweaking = true;
	}

	bool state = app->init();
	assert(state);

	if (useTweaking) {
		twk_load();
	}

	bool running = true;
	
	float timeStep = 1.0f / 60.0f;
	float gameTimer = 0.0f;
	float currentTime = 0.0f;
	float accu = 0.0f;

	while (ds::isRunning() && running) {

		ds::begin();

		perf::reset();

		perf::ZoneTracker("main");

		if (useTweaking) {
			tweakingReloadTimer += ds::getElapsedSeconds();
			if (tweakingReloadTimer >= 0.5f) {
				tweakingReloadTimer -= 0.5f;
				twk_load();
			}
		}

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