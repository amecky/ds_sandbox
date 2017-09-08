#define DS_IMPLEMENTATION
#include <diesel.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define SPRITE_IMPLEMENTATION
#include <SpriteBatchBuffer.h>
#define DS_IMGUI_IMPLEMENTATION
#include <ds_imgui.h>
#include "..\resource.h"
#include "utils\tweening.h"
#include "TweeningTest.h"
#include "AnimationTest.h"
//#define DS_LOG_PANEL
//#include "LogPanel.h"
#define DS_VM_IMPLEMENTATION
#include <ds_vm.h>
#include "VMTest.h"
#define DS_PROFILER_IMPLEMENTATION
#include <ds_profiler.h>
#include "ParticleExpressionTest.h"
#define DS_PERF_PANEL
#include "PerfPanel.h"
#include "plugins\PluginRegistry.h"
#include "ParticlePluginTest.h"

// ---------------------------------------------------------------
// load image from the resources
// ---------------------------------------------------------------
RID loadImage(const char* name) {
	int x, y, n;
	HRSRC resourceHandle = ::FindResource(NULL, MAKEINTRESOURCE(IDB_PNG2), "PNG");
	if (resourceHandle == 0) {
		return NO_RID;
	}
	DWORD imageSize = ::SizeofResource(NULL, resourceHandle);
	if (imageSize == 0) {
		return NO_RID;
	}
	HGLOBAL myResourceData = ::LoadResource(NULL, resourceHandle);
	void* pMyBinaryData = ::LockResource(myResourceData);
	unsigned char *data = stbi_load_from_memory((const unsigned char*)pMyBinaryData, imageSize, &x, &y, &n, 4);
	ds::TextureInfo info = { x,y,n,data,ds::TextureFormat::R8G8B8A8_UNORM , ds::BindFlag::BF_SHADER_RESOURCE };
	RID textureID = ds::createTexture(info, name);
	stbi_image_free(data);
	UnlockResource(myResourceData);
	FreeResource(myResourceData);
	return textureID;
}

//static log_panel_plugin* logPanel = 0;

void myLogging(const logging::LogLevel&, const char* message) {
	OutputDebugString(message);
	OutputDebugString("\n");
	//logPanel->add_line(logPanel->data,message);
}

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

	
	//_CrtSetBreakAlloc(160);
	SetThreadAffinityMask(GetCurrentThread(), 1);

	plugin_registry registry = create_registry();
	//load_log_panel_plugin(&registry);
	//logPanel = (log_panel_plugin*)registry.get(LOG_PANEL_PLUGIN_NAME);
	//assert(logPanel != 0);

	logging::logHandler = myLogging;
	logging::currentLevel = logging::LL_DEBUG;

	//load_plugin(&registry, "log_panel_plugin");

	

	//logpanel::init(32);
	perf::init();
	

	initialize();
	
	gui::init();

	// load image using stb_image
	RID textureID = loadImage("content\\TextureArray.png");

	// create the sprite batch buffer
	SpriteBatchBufferInfo sbbInfo = { 2048, textureID, ds::TextureFilters::LINEAR };
	SpriteBatchBuffer spriteBuffer(sbbInfo);
	
	bool running = true;

	//TweeningTest tweeningTest(&spriteBuffer);

	//AnimationTest animationTest(&spriteBuffer);

	//ParticleExpressionTest particleTest(registry,&spriteBuffer);

	ParticlePluginTest particlePluginTest(registry, &spriteBuffer);

	//VMTest vmTest(&spriteBuffer);

	int l_count = 0;

	bool reload = true;
	float reloadTimer = 0.0f;
	
	while (ds::isRunning() && running) {

		if (reload) {
			reloadTimer += ds::getElapsedSeconds();
			if (reloadTimer >= 0.5f) {
				reloadTimer -= 0.5f;
				registry.check_plugins();
			}
		}

		perf::reset();

		perf::ZoneTracker("main");

		{
			perf::ZoneTracker("main::begin");
			ds::begin();
		}
		

		{
			perf::ZoneTracker("main::render");
			spriteBuffer.begin();

			//tweeningTest.render();
			//animationTest.render();
			//vmTest.render();
			//particleTest.render();
			//font::renderText(ds::vec2(20, 550), "Hello World", &spriteBuffer);
			//font::renderText(ds::vec2(20, 90), "RSTUVWXYZ", &spriteBuffer);

			particlePluginTest.render();

			spriteBuffer.flush();
		}
		/*
		{
			perf::ZoneTracker("main::events");
			ds::Event event;
			while (ds::get_event(&event)) {
				LOG_DEBUG("caught event %d", event.type);
				if (event.type == ds::EventType::ET_MOUSEBUTTON_PRESSED) {
					animationTest.onButtonClicked(event.mouse.button);
				}
				if (event.type == ds::EventType::ET_KEY_PRESSED) {
					if (event.key.key == 'l') {
						LOG_INFO("L pressed %d", l_count++);
					}
					if (event.key.key == 'd') {
						int end = logpanel::get_num_lines();
						int start = end - 5;
						if (start < 0) {
							start = 0;
						}
						LOG_INFO("----------------------------------------------\n");
						for (uint16_t i = start; i < end; ++i) {
							LOG_INFO("%s",logpanel::get_line(i));
						}
					}
				}
			}
		}
		*/
		//tweeningTest.renderGUI();
		//animationTest.renderGUI();
		//vmTest.renderGUI();
		{
			perf::ZoneTracker("main::renderGUI");
			particlePluginTest.renderGUI();
		}
		//perf::tickFPS(ds::getElapsedSeconds());		
		
		{
			perf::ZoneTracker("main::end");
			ds::end();
		}
		perf::finalize();
		
		
	}
	perf::shutdown();
	gui::shutdown();
	//logpanel::shutdown();
	//unload_log_panel_plugin(&registry);
	shutdown_registry();
	ds::shutdown();
}