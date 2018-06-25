#define DS_IMPLEMENTATION
#include <diesel.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define SPRITE_IMPLEMENTATION
#include <SpriteBatchBuffer.h>
#define DS_TWEAKABLE_IMPLEMENTATION
#include <ds_tv.h>
#define GAMESETTINGS_IMPLEMENTATION
#include <ds_tweakable.h>
#define DS_PROFILER
#include <ds_profiler.h>
//#define DS_GAME_UI_IMPLEMENTATION
//#include <ds_game_ui.h>
#define DS_IMGUI_IMPLEMENTATION
#include <ds_imgui.h>
#define BASE_APP_IMPLEMENTATION
#include <ds_base_app.h>
#define DS_TWEENING_IMPLEMENTATION
#include <ds_tweening.h>
#define DS_STRING_IMPLEMENTATION
#include <ds_string.h>
#define DS_LOG_PANEL
#include <ds_logpanel.h>


extern ds::BaseApp* app;


// ---------------------------------------------------------------
// main method
// ---------------------------------------------------------------
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow) {

	//_CrtSetBreakAlloc(1198);

	app->init();

	perf::init();

	double timers[64];
	int num = 0;
	
	while (ds::isRunning() && app->isRunning()) {

		std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();

		perf::reset();

		ds::begin();

		float dt = static_cast<float>(ds::getElapsedSeconds());

		app->tick(dt);

		ds::dbgPrint(0, 34, "FPS: %d", ds::getFramesPerSecond());

		ds::end();

		perf::finalize();

		std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

		timers[num++] = std::chrono::duration<double, std::milli>(end - now).count();

		if (num >= 64) {
			num = 0;
		}
	}

	for (int i = 0; i < num; ++i) {
		int fps = 15.0f / timers[i] * 60.0f;
		DBG_LOG("%d = %g %d", i, timers[i], fps);
	}
	ds::shutdown();

	delete app;

}
/*
#define DS_IMPLEMENTATION
#include <diesel.h>
//#include <SpriteBatchBuffer.h>
#define DS_IMGUI_IMPLEMENTATION
#include <ds_imgui.h>
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
#include "viewer\InstancedObjViewer.h"
#include "background\Doors.h"
#include "flow\FlowFieldApp.h"

struct ButtonState {
	bool pressed;
	bool clicked;
};

// ---------------------------------------------------------------
// handle buttons
// ---------------------------------------------------------------
void handleButton(int index, ButtonState* state) {
	if (ds::isMouseButtonPressed(index)) {
		state->pressed = true;
	}
	else if (state->pressed) {
		state->pressed = false;
		if (!state->clicked) {
			state->clicked = true;
		}
		else {
			state->clicked = false;
		}
	}
}

void debug(const LogLevel& level, const char* message) {
#ifdef DEBUG
	OutputDebugString(message);
	OutputDebugString("\n");
#endif
}

// ---------------------------------------------------------------
// main method
// ---------------------------------------------------------------
//int main() {
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow) {

	ButtonState leftButton;
	ButtonState rightButton;
	//TestApp* app = new DoorsViewer;
	//TestApp* app = new InstancedObjViewer;
	//TestApp* app = new TopDownObjViewer;
	//TestApp* app = new ObjViewer;
	//TestApp* app = new WarpingGridApp;
	//TestApp* app = new InstanceTest;
	//TestApp* app = new BinaryClock;
	//TestApp* app = new AssetViewer;
	//TestApp* app = new TweeningTest;
	//TestApp* app = new VMTest;
	//TestApp* app = new ParticleExpressionTest;
	TestApp* app = new FlowFieldApp;
	//_CrtSetBreakAlloc(160);
	SetThreadAffinityMask(GetCurrentThread(), 1);

	ds::RenderSettings settings = app->getRenderSettings();
	settings.supportDebug = true;
	settings.logHandler = debug;
	ds::init(settings);

	

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

	app->prepareDefaults();

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

	p2i sp = p2i(10, ds::getScreenHeight() - 10);

	bool show_gui = true;
	bool one_pressed = false;

	while (ds::isRunning() && running) {

		ds::begin();

		perf::reset();

		perf::ZoneTracker("main");

		if (ds::isKeyPressed('Y') && !one_pressed) {
			show_gui = !show_gui;
			one_pressed = true;
		}
		if (!ds::isKeyPressed('Y') && one_pressed) {
			one_pressed = false;
		}

		if (useTweaking) {
			tweakingReloadTimer += ds::getElapsedSeconds();
			if (tweakingReloadTimer >= 0.5f) {
				tweakingReloadTimer -= 0.5f;
				if (twk_load()) {
					app->OnReload();
				}
			}
		}

		handleButton(0, &leftButton);
		if (leftButton.clicked) {
			app->OnButtonClicked(0);
			leftButton.clicked = false;
		}
		handleButton(1, &rightButton);
		if (rightButton.clicked) {
			app->OnButtonClicked(1);
			rightButton.clicked = false;
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

		if (app->usesGUI() && show_gui) {
			gui::start(&sp, 350);
			app->renderGUI();
			gui::end();
		}

		ds::dbgPrint(0, 34, "FPS: %d", ds::getFramesPerSecond());
			
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
*/